#ifndef __APP_H

#define __APP_H
/* Includes -------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "port.h"
#include "mbport.h"
#include "mb.h"
#include "regmap.h"
#include "flash.h"

#define CONDUCTIVITY	4

#define MODBUS_PARITY_NONE 	0
#define MODBUS_PARITY_ODD 	1
#define MODBUS_PARITY_EVEN 	2

#define TMP_OVERRANGE		6

//校准点个数
#define CAL_POINT_NUM	2

#define HW_VERSION  "1.0"
#define SW_VERSION  "1.0"

//系统相关寄存器声明
extern SYS_STATUS_T system_status;

extern COMM_SETTINGS_T comm_settings;

extern MEASURE_SETTINGS_T measure_settings;

extern CALIB_SETTINGS_T calib_settings;

extern FILTER_SETTINGS_T filter_settings;

extern MEASURE_VALUES_T measure_values;

extern SENSOR_PARAM_T sensor_param;

void FunctionPoll(void);
void SENSOR_MeasureParameterReset(void);
void Rset(void);

#endif
