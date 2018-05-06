#include "tur.h"

float getS365(u16 aveTimes)
{
	float S365;
	turnLed1();
	delay_ms(10);
	write_to_LTC2630ISC6(0X30,filter_settings.cs365);
	S365=Get_Adc_Average(7,aveTimes);
	filter_settings.s365=S365;
	return S365;
}