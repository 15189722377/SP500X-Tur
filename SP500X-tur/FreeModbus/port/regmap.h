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
	uint16		runStatus;					/* 41001	16bit integer	read  ����״̬(1���� ������ֵ����) */
	uint16   	commStatus;					/* 41002	16bit integer	read  ͨ��״̬(1���� ������ֵ����) */
	uint16 		calibStatus;				/* 41003 	16bit integer read  �궨״̬(0����,1�궨��,2�궨�����,3�궨ʧ��)*/
	uint16		configStatus;				/* 41004	16bit integer	read  ����״̬(0ϵͳδ����,1ϵͳ������)*/
	uint32		productNum;					/* 41005	16bit integer	read  r read */
	char		deviceName[16];       /* 41007	128bit ch̽ͷ���� 1ph 2orp 3ph&orp 4�絼*/	
	char		serial[16];           /* 41015	128bit chaar read */
	char		hardwareVer[16];      /* 41023	128bit char read */
	char		softwareVer[16];      /* 41031	128bit char read */
	uint16		reserved[12];				/* 41039-41050  reserved */
} SYS_STATUS_T;

typedef struct commSettings
{											/* Register		Type  		 R/W */
	uint16		modbusAddr;					/* 42001 	 16bit integer   r/w modbus��ַ  */
	uint16		modbusDatabits;			/* 42002 	 16bit integer   r/w ����λ   */
	uint16		modbusParity;				/* 42003 	 16bit integer   r/w ��żУ�� */
	uint32		modbusBaud;					/* 42004 	 32bit integer   r/w ������ */
	uint16		reserved[45];				/* 42006-42050   reserved  ����*/
} COMM_SETTINGS_T;

typedef	struct measureSettings
{											/* Register		Type  		 R/W */
	uint16		sampleCycle;			  	/* 43001	 16bit integer	r/w ����Ƶ��(��λ����)  */
	float		measureRange;			   	  /* 43002	 32bit float	r/w  �������(e.g,300) */
	float		smoothingFactor;		    /* 43004 	 32bit float  r/w    ����ƽ��ϵ��(ȡֵ��Χ0-1) */
	uint16		reserved[45];			    /* 43006-43050   reserved  ����*/
} MEASURE_SETTINGS_T;


typedef	struct calibSettings
{																		/* Register		Type  		 R/W */
	uint16		calibType;							/* 44001	16bit integer	 r/w �궨����(1��궨,2��궨) */
	uint16		firstCalibSolution;			/* 44002	32bit float		r/w  ��һ��궨ֵ */
	uint16	  secondCalibSolution;		/* 44003	32bit float		r/w  �ڶ���궨ֵ */
	uint16		calibCommand;						/* 44004	16bit integer    r/w �궨���� 1У׼  2(4-20ma����)  3������λ*/
	float    K;                  		  /* 44005  �絼��ϵ��*/
	float    Ra;											/* 44007  */
	float    Rb;											/* 44009  */
	float		 Rc;											/* 44011  */
	float    Rd;											/* 44013  */
	uint16		reserved[36];				    /* 44015-44050 reserved  ����*/
} CALIB_SETTINGS_T;


typedef	struct filterSettings
{																		/* Register		Type  		 R/W */
	uint16		reserved[50];						/* 45001-45050   reserved  ����*/
} FILTER_SETTINGS_T;

typedef struct measureValues
{																/* Register		Type  		 R/W */
	float		sensorValue;					/* 46001	32bit float		read  ����ֵ*/
	float		sensorValue_mA;				/* 46003	32bit float		read  ����ֵ(4-20ma��ʽ)*/
	float   temperatureValue;			/* 46005	32bit float		read  �¶�(���϶�)*/
	float   i;										/* 46007  */
	float   v;										/* 46009  */
	uint16		reserved[40];				/* 46011-46050  1eserved  ����*/
} MEASURE_VALUES_T;


// Below are sensor specific registers, pH meter may have different definition
// All sensor specific registers start from 48001
// ��ʱû����
typedef	struct sensorParam
{											/* Register		Type  		 R/W */	
	uint16 reserved[50];        /* 48001-48050 */
} SENSOR_PARAM_T;
#pragma pack()

#endif
