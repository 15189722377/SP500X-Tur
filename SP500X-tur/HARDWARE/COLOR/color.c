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
/*
*/
void measurePD1(u8 channel)
{
	float dark,signle;
	turnOffLeds();
	dark=Get_Adc_Average(channel,100);
}