/**
  ******************************************************************************
  * @file    regmap.h
  * @author  
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   Modbus register address map
  ******************************************************************************
  * @attention
  *  
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion --------------------------------------*/

#ifndef __REGMAP_H

#define __REGMAP_H

/* Includes -------------------------------------------------------------------*/

#include "stm32f10x.h"

/* Exported_Macros ------------------------------------------------------------*/

typedef uint16_t uint16;
typedef uint32_t uint32;

/* Exported_Types -------------------------------------------------------------*/

#pragma pack(2)

typedef	struct sysStatus
{											/* Register		Type  		 R/W */
	uint16		runStatus;					/* 41001	16bit integer	read  运行状态(1正常 ，其余值故障) */
	uint16   	commStatus;					/* 41002	16bit integer	read  通信状态(1正常 ，其余值故障) */
	uint16 		calibStatus;				/* 41003 	16bit integer read  标定状态(0空闲,1标定中,2标定已完成,3标定失败)*/
	uint16		configStatus;				/* 41004	16bit integer	read  配置状态(0系统未配置,1系统已配置)*/
	uint32		productNum;					/* 41005	16bit integer	read  r read */
	char		deviceName[16];       /* 41007	128bit ch探头类型 1ph 2orp 3ph&orp 4电导*/	
	char		serial[16];           /* 41015	128bit chaar read */
	char		hardwareVer[16];      /* 41023	128bit char read */
	char		softwareVer[16];      /* 41031	128bit char read */
	uint16		reserved[12];				/* 41039-41050  reserved */
} SYS_STATUS_T;

typedef struct commSettings
{											/* Register		Type  		 R/W */
	uint16		modbusAddr;					/* 42001 	 16bit integer   r/w modbus地址  */
	uint16		modbusDatabits;			/* 42002 	 16bit integer   r/w 数据位   */
	uint16		modbusParity;				/* 42003 	 16bit integer   r/w 奇偶校验 */
	uint32		modbusBaud;					/* 42004 	 32bit integer   r/w 波特率 */
	uint16		reserved[45];				/* 42006-42050   reserved  保留*/
} COMM_SETTINGS_T;

typedef	struct measureSettings
{											/* Register		Type  		 R/W */
	uint16		sampleCycle;			  	/* 43001	 16bit integer	r/w 采样频率(单位：秒)  */
	float		measureRange;			   	  /* 43002	 32bit float	r/w  最大量程(e.g,300) */
	float		smoothingFactor;		    /* 43004 	 32bit float  r/w    滑动平均系数(取值范围0-1) */
	uint16		reserved[45];			    /* 43006-43050   reserved  保留*/
} MEASURE_SETTINGS_T;


typedef	struct calibSettings
{																		/* Register		Type  		 R/W */
	uint16		calibType;							/* 44001	16bit integer	 r/w 标定类型(1点标定,2点标定) */
	uint16		firstCalibSolution;			/* 44002	32bit float		r/w  第一点标定值 */
	uint16	  secondCalibSolution;		/* 44003	32bit float		r/w  第二点标定值 */
	uint16		calibCommand;						/* 44004	16bit integer    r/w 标定命令 1校准  2(4-20ma设置)  3参数复位*/
	float    K;                  		  /* 44005  电导率系数*/
	float    Ra;											/* 44007  */
	float    Rb;											/* 44009  */
	float		 Rc;											/* 44011  */
	float    Rd;											/* 44013  */
	uint16		reserved[36];				    /* 44015-44050 reserved  保留*/
} CALIB_SETTINGS_T;


typedef	struct filterSettings
{																		/* Register		Type  		 R/W */
	uint16		reserved[50];						/* 45001-45050   reserved  保留*/
} FILTER_SETTINGS_T;

typedef struct measureValues
{																/* Register		Type  		 R/W */
	float		sensorValue;					/* 46001	32bit float		read  测量值*/
	float		sensorValue_mA;				/* 46003	32bit float		read  测量值(4-20ma形式)*/
	float   temperatureValue;			/* 46005	32bit float		read  温度(摄氏度)*/
	float   i;										/* 46007  */
	float   v;										/* 46009  */
	uint16		reserved[40];				/* 46011-46050  1eserved  保留*/
} MEASURE_VALUES_T;


// Below are sensor specific registers, pH meter may have different definition
// All sensor specific registers start from 48001
// 暂时没开辟
typedef	struct sensorParam
{											/* Register		Type  		 R/W */	
	uint16 reserved[50];        /* 48001-48050 */
} SENSOR_PARAM_T;
#pragma pack()

#endif
