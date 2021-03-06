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

#define HW_VERSION  "1.0"
#define SW_VERSION  "1.1"

#define MODBUS_PARITY_NONE 	0
#define MODBUS_PARITY_ODD 	1
#define MODBUS_PARITY_EVEN 	2

/*-------------------功能轮询命令宏-------------------*/
#define CMD_JUMP_BOOTLOADER  7  //进入BootLoader
#define CMD_PARA_RESET	3		//参数复位
#define CMD_CALIB_STEP2	2		//4-20MA设置
#define CMD_CALIB_STEP1	1		//校准
#define CMD_NONE      0    //NONE

extern u8 bitmodbus;

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
