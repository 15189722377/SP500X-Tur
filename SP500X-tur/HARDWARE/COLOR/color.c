/**
  ******************************************************************************
  * @file    color.c
  * @author  
  * @version V1.0
  * @date    2018-06-14
  * @brief   Colorimetric measurement
  ******************************************************************************
  * @attention
  *  
  ******************************************************************************
  */ 
#include "color.h"
#include "app.h"

void measurePD1(u8 channel)
{
	float dark;
	turnOffLeds();
	dark=Get_Adc_Average(channel,100);
}

 void measureUrea(void)
{
	float darks365f,adcf,s365f;
	
	turnOffLeds();
	delay_ms(15);
	darks365f=ADC_ReadChannel(T420CHANNEL,100);
	/*保持与FA100一致性，去掉*10操作		20180822 boZhu */
	filter_settings.darks365=darks365f;			
	
	write_to_LTC2630ISC6(0X30,filter_settings.cs365);
	turnLed2();
	delay_ms(15);
	adcf=ADC_ReadChannel(T420CHANNEL,200);
	turnOffLeds();
	
	if(adcf>darks365f)
	{
		s365f=adcf-darks365f;
		/*保持与FA100一致性，去掉*10操作  20180822 boZhu */
		filter_settings.s365=s365f;
		filter_settings.errorCode&=~ERR_DARK_HIGH;
	}
	else
	{
		filter_settings.s365=filter_settings.s365di;
		filter_settings.errorCode|=ERR_DARK_HIGH;
	}
	//sensor_param.SS365=filter_settings.s365;
}
/* ADC_ReadChannel的采样次数不可以超过DMA接收数组的长度 */
void measureClF(void)
{
	float darks365f,adcf,s365f;
	
	turnOffLeds();
	delay_ms(15);
	darks365f=ADC_ReadChannel(T365CHANNEL,100);
	filter_settings.darks365=darks365f*10;
	
	write_to_LTC2630ISC6(0X30,filter_settings.cs365);
	turnLed1();
	delay_ms(15);
	adcf=ADC_ReadChannel(T365CHANNEL,200);
	turnOffLeds();
	
	if(adcf>darks365f)
	{
		s365f=adcf-darks365f;
		filter_settings.s365=s365f*10;
		filter_settings.errorCode&=~ERR_DARK_HIGH;
	}
	else
	{
		filter_settings.s365=filter_settings.s365di;
		filter_settings.errorCode|=ERR_DARK_HIGH;
	}
	//sensor_param.SS365=filter_settings.s365;
}

u16 StartMeasureS365(u8 type)
{
	float darks365f,adcf,s365f;
	u8 channel;
	u16 s365mult10=1;
	
	filter_settings.errorCode=0;
		
	if(type==0)
	{
		channel=T420CHANNEL;  //urea
	}
	else if(type==1)
	{
		channel=T365CHANNEL;  //ClF
	}
	else
	{
		type=0;
		channel=T420CHANNEL;  //urea
	}
	
	turnOffLeds();
	delay_ms(15);
	darks365f=ADC_ReadChannel(channel,100);
	
	write_to_LTC2630ISC6(0X30,filter_settings.cs365);
	if(type==0)
	{
		turnLed2();  //urea
	}
	else if(type==1)
	{
		turnLed1();  //ClF
	}
	
	delay_ms(15);
	adcf=ADC_ReadChannel(T365CHANNEL,200);
	turnOffLeds();
	
	if(adcf>darks365f)
	{
		s365f=adcf-darks365f;
// 		measure_values.cmdMode_s365di=s365f*10;
		s365mult10=s365f*10;
		filter_settings.errorCode&=~ERR_DARK_HIGH;
	}
	else
	{
// 		measure_values.cmdMode_s365di=1;
		s365mult10=1;
		filter_settings.errorCode|=ERR_DARK_HIGH;
	}
	return s365mult10;
}
