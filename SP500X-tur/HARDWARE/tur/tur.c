#include "tur.h"
#include "stm32f10x_iwdg.h"

u8 isFirstValue=1;
float lastNTU=0;
float s365F=1.0;
u8 measureCount=0;
u8 isMeasureFlg=0;

// float getS365(u16 aveTimes)
// {
// 	float S365;
// 	turnLed1();
// 	delay_ms(10);
// 	write_to_LTC2630ISC6(0X30,filter_settings.cs365);
// 	S365=Get_Adc_Average(7,aveTimes);
// 	filter_settings.s365=S365;
// 	return S365;
// }

void measureTurb(void)
{
	float ntu,S365,darks365F,adcF;
	
	turnOffLeds();
	delay_ms(15);
	darks365F=Get_Adc_Average(7,10);
	filter_settings.darks365=darks365F*10;
	
	write_to_LTC2630ISC6(0X30,filter_settings.cs365);
	turnLed1();
	delay_ms(15);
	adcF=Get_Adc_Average(7,20);
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
	measure_values.sensorValue=ntu;
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
  TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	if( TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		if(++measureCount>=measure_settings.sampleCycle)
		{
 			IWDG_ReloadCounter();    //喂狗
			isMeasureFlg=1;
			measureCount=0;
		} 
	}	
}

void TIM3_ModpollInit(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  uint16_t PrescalerValue = 0;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  
  PrescalerValue = (uint16_t) (SystemCoreClock / 1000000) - 1; // 1M, 1us
  
  TIM_TimeBaseStructure.TIM_Period =24;     
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  
  TIM_ARRPreloadConfig(TIM3, ENABLE);
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); 
	TIM_ITConfig(TIM3, TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM3,ENABLE);
}

void TIM3_IRQHandler(void)
{  
  TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	if( TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		eMBPoll(); 
	}	
}
