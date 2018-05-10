#ifndef __TUR_H

#define __TUR_H

#include "app.h"
#include "embreg.h"
#include "LTC2630ISC6.h"
#include "SWITCH.H"
#include "ADC.H"

#define GROUND_ERR	0x01

extern float s365F;

//float getS365(u16 aveTimes);
void TIM2_MeasureInit(void);
void TIM3_ModpollInit(void);

#endif
