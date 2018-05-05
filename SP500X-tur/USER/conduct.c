#include "conduct.h"
#include "ltc2440.h"
#include "ad5410.h"
#include "app.h"
#include "stm32f10x_iwdg.h"

float Vva1,Via1;
u8 ResIdx;
float conduct;
float lastConduct=0;
u8 isFirstValue=1;
float measure_G(void)
{
	float I,G,T,V,R;
	
	USART_Cmd(USART2, DISABLE);
	
	if((measure_settings.smoothingFactor<0.00001)||(measure_settings.smoothingFactor>1))
	{
		measure_settings.smoothingFactor=0.3;
	}
	
	T=temperatureRead();
	T=T;	
	getCond();
	if(ResIdx==0)
		R=calib_settings.Ra;	
	else if(ResIdx==1)
		R=calib_settings.Rb;
	else if(ResIdx==2)
		R=calib_settings.Rc;	
	else if(ResIdx==3)
		R=calib_settings.Rd;	
	I=Via1/R;
	V=(Vva1-0.020)/6;
	conduct=I/V*1000000/(1+0.02*(T-25));
	if(!isFirstValue)
	{
		conduct=conduct*measure_settings.smoothingFactor+lastConduct*(1-measure_settings.smoothingFactor);
	}
	else
	{
		isFirstValue=0;
	}
	lastConduct=conduct;
 	G=conduct*calib_settings.K;	
  if(G<0)G=0;
	if(filter_settings.filterCoefficient_1==filter_settings.filterCoefficient_2)
	{
		filter_settings.filterCoefficient_1=0;
		filter_settings.filterCoefficient_2=10000;
	}
  measure_values.sensorValue_mA=((G-filter_settings.filterCoefficient_1)/(filter_settings.filterCoefficient_2-filter_settings.filterCoefficient_1)*16+4);    //modbus address 16003
  AD5410_IOUT(measure_values.sensorValue_mA); //4-20ma output
	measure_values.sensorValue=G;       //modbus address 46001
	measure_values.temperatureValue=T;    //modbus address 46005
// 	measure_values.sensorValue_mA=T;
// 	measure_values.temperatureValue=ResIdx;
	measure_values.i=Via1;
	measure_values.v=Vva1;
	
	USART_Cmd(USART2, ENABLE);
	
	return G;
}

void Tim3_init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  //
  uint16_t PrescalerValue = 0;
  
  //ʹ�ܶ�ʱ��3ʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  
  //��ʱ��ʱ�������˵��
  //HCLKΪ72MHz��APB1����2��ƵΪ36MHz
  //TIM3��ʱ�ӱ�Ƶ��Ϊ72MHz��Ӳ���Զ���Ƶ,�ﵽ���
  //TIM3�ķ�Ƶϵ��Ϊ3599��ʱ���Ƶ��Ϊ72M/ (1 + Prescaler) = 10KHz,��׼Ϊ0.1ms
  //TIM������ֵΪusTim1Timerout50u
  PrescalerValue = (uint16_t) (SystemCoreClock / 10000) - 1;// 
  //��ʱ��3��ʼ��
  TIM_TimeBaseStructure.TIM_Period =39999;     //    timing cycle = 4S
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;  //Ԥ��Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;      //���÷�Ƶϵ��������Ƶ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  //Ԥװ��ʹ��
  TIM_ARRPreloadConfig(TIM3, ENABLE);
  
  //
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  //��ʱ��3�ж����ȼ�
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
//   //�������жϱ�־λ
//   TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
//   //��ʱ��3����жϹر�
//   TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);

  //��������жϱ�־
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); 
	//ʹ��TIM3�����ж�
	TIM_ITConfig(TIM3, TIM_IT_Update,ENABLE);
  
//   //��ʱ��3����
//   TIM_Cmd(TIM3,  DISABLE);

     TIM_Cmd(TIM3,ENABLE);

}

/* use TIM to prevent modbus time out*/
u8 isMeasureFlg=0;
void TIM3_IRQHandler(void)
{
  if( TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET )
  {
    IWDG_ReloadCounter();//ι��
	  
		isMeasureFlg=1; 
		
	  TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
  }
}
