#include "tur.h"
#include "stm32f10x_iwdg.h"

u8 isFirstValue=1;
float lastNTU=0;
float s365F=1.0;
u8 measureCount=0;
u8 isMeasureFlg=0;

// float getT365(u16 aveTimes)
// {
// 	float T365;
// 	turnLed2();
// 	delay_ms(10);
// 	write_to_LTC2630ISC6(0X30,filter_settings.cs365);
// 	T365=Get_Adc_Average(6,aveTimes);
// 	filter_settings.t365=T365;
// 	return T365;
// }

 void measureTurb_1(void)
{
	float ntu,S365,darks365F,adcF;
	
	turnOffLeds();
	delay_ms(15);
	darks365F=ADC_ReadChannel(7,100);
	filter_settings.darks365=darks365F*10;
	
	write_to_LTC2630ISC6(0X30,filter_settings.cs365);
	turnLed2();
	delay_ms(15);
	adcF=ADC_ReadChannel(7,200);
	turnOffLeds();
	
	if(adcF>darks365F)
	{
		s365F=adcF-darks365F;
		filter_settings.s365=s365F*10;
		filter_settings.errorCode&=~GROUND_ERR;
	}
	else
	{
		filter_settings.s365=filter_settings.s365di;
		filter_settings.errorCode|=GROUND_ERR;
	}
	S365=s365F*10/((float)filter_settings.s365di);
	ntu=filter_settings.slope*(S365-1.0);
	if(isFirstValue==1)
	{
		isFirstValue=0;
	}
	else
	{
		ntu=measure_settings.smoothingFactor*ntu+(1.0-measure_settings.smoothingFactor)*lastNTU;
	}
	lastNTU=ntu;
	if(ntu<0) ntu=0;
	measure_values.sensorValue=ntu;
}
/* ADC_ReadChannel的采样次数不可以超过DMA接收数组的长度 */
void measureTurb(void)
{
	float ntu,S365,darks365F,adcF;
	
	turnOffLeds();
	delay_ms(15);
	darks365F=ADC_ReadChannel(6,100);
	filter_settings.darks365=darks365F*10;
	
	write_to_LTC2630ISC6(0X30,filter_settings.cs365);
	turnLed1();
	delay_ms(15);
	adcF=ADC_ReadChannel(6,200);
	turnOffLeds();
	
	if(adcF>darks365F)
	{
		s365F=adcF-darks365F;
		filter_settings.s365=s365F*10;
		filter_settings.errorCode&=~GROUND_ERR;
	}
	else
	{
		filter_settings.s365=filter_settings.s365di;
		filter_settings.errorCode|=GROUND_ERR;
	}
	S365=s365F*10/((float)filter_settings.s365di);
	ntu=filter_settings.slope*(S365-1.0);
	if(isFirstValue==1)
	{
		isFirstValue=0;
	}
	else
	{
		ntu=measure_settings.smoothingFactor*ntu+(1.0-measure_settings.smoothingFactor)*lastNTU;
	}
	lastNTU=ntu;
	if(ntu<0) ntu=0;
	measure_values.sensorValue=ntu;
}


float Calib_S365(void)
{
	float darkC,adcC,s365C;
	
	turnOffLeds();
	delay_ms(15);
	darkC=ADC_ReadChannel(S365CHANNEL,100);
	filter_settings.darks365=darkC*10;
	
	write_to_LTC2630ISC6(0X30,filter_settings.cs365);
	turnLed1();
	delay_ms(15);
	adcC=ADC_ReadChannel(S365CHANNEL,200);
	//turnOffLeds();
	
	if(adcC>darkC)
	{
		s365C=adcC-darkC;
		filter_settings.s365=s365C*10;
		system_status.calibStatus&=~GROUND_ERR;
	}
	else
	{
		s365C=1.0;
		filter_settings.s365=filter_settings.s365di;
		system_status.calibStatus|=GROUND_ERR;
	}
	return s365C;
}

void TIM2_MeasureInit(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  uint16_t PrescalerValue = 0;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  PrescalerValue = (uint16_t) (SystemCoreClock / 10000) - 1; 
  
  TIM_TimeBaseStructure.TIM_Period =9999;     //    9999,timing cycle = 1S;改为1秒进一次中断，从而测量周期可调
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  
  TIM_ARRPreloadConfig(TIM2, ENABLE);
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //1,2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	TIM_ClearITPendingBit(TIM2,TIM_IT_Update); 
	TIM_ITConfig(TIM2, TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM2,ENABLE);
}

void TIM2_IRQHandler(void)
{  
	if( TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
	{
		if(++measureCount>=measure_settings.sampleCycle)
		{
 			IWDG_ReloadCounter();    //喂狗
			isMeasureFlg=1;
			measureCount=0;
		} 
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}	
}

void TIM1_ModpollInit(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  uint16_t PrescalerValue = 0;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  
  PrescalerValue = (uint16_t) (SystemCoreClock / 10000) - 1; // 0.1ms
  
  TIM_TimeBaseStructure.TIM_Period =99;     
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  
  TIM_ARRPreloadConfig(TIM1, ENABLE);
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_ClearFlag(TIM1, TIM_FLAG_Update); 
	//TIM_ClearITPendingBit(TIM1,TIM_IT_Update); 
	TIM_ITConfig(TIM1, TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM1,ENABLE);
}

void TIM1_UP_IRQHandler(void)
{  
	if( TIM_GetITStatus(TIM1,TIM_IT_Update)!=RESET)
	{
		eMBPoll(); 
		IWDG_ReloadCounter();    //喂狗
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);	
	}	
}
