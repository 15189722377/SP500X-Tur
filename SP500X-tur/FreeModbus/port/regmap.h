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

typedef enum
{
	NOERR=0,
	S365_ERR=0x0001,
	SOLUTION_VALUE_ERR=0x0002,
	S365DI_ERR=0x0004	
}CalibStatus;

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
	float     reserved1;              /* 44002-44003  float */
	uint16		calibCommand;						/* 44004	16bit integer    r/w �궨���� 1У׼  2(4-20ma����)  3������λ*/
	float     solutionL;              /* 44005-44006  float */
	float     solutionH;              /* 44007-44008  float */
	uint16     s365L;                 /* 44009 */
	uint16     s365H;                 /* 44010 */
	float     k;                      /* 44011-44012  float */
	float     b;                      /* 44013-44014  float */
	uint16    type;                   /* 44015                   �������� 0���� 1����*/
	uint16		reserved[35];				    /* 44016-44050 reserved  ����*/
} CALIB_SETTINGS_T;

typedef	struct filterSettings    //sensorParam�洢δ��ͨ ����ʱʹ�ô˼Ĵ�������� 20180506
{	
	uint16    darks365;           /* 45001   16bit integer   it's useless for st510, only uesd to compat with st500 register */
	float	  slope;			          /* 45002   32bit float     r/w */
	float     intercept;          /* 45004   32bit float     useless r/w */
	float	  t1;				            /* 45006   32bit float     r/w */
	float	  t2;				            /* 45008   32bit float     r/w */
	uint16    mARange;            /* 45010   16bit integer   r/w   4-20mA scaling factor, mA=ppb*16/mARange+4 */
	uint16	  ct365;			        /* 45011   16bit integer   r/w */
	uint16	  cs365;			        /* 45012   16bit integer   r/w */
	uint16	  ct420;			        /* 45013   16bit integer   r/w */
	uint16	  cs420;			        /* 45014   16bit integer   r/w */
	uint16	  t365di;			        /* 45015   16bit integer   r/w */
	uint16	  s365di;			        /* 45016   16bit integer   r/w */
	uint16	  t420di;			        /* 45017   16bit integer   r/w */
	uint16	  s420di;			        /* 45018   16bit integer   r/w */
	uint16	  t365;				        /* 45019   16bit integer   r/w */
	uint16	  s365;				        /* 45020   16bit integer   r/w */
	uint16	  t420;				        /* 45021   16bit integer   r/w */
	uint16	  s420;				        /* 45022   16bit integer   r/w */
	uint16	  ct365Factory;		    /* 45023   16bit integer   r/w */
	uint16	  cs365Factory;		    /* 45024   16bit integer   r/w */
	uint16	  ct420Factory;		    /* 45025   16bit integer   r/w */
	uint16	  cs420Factory;		    /* 45026   16bit integer   r/w */
	uint16	  t365diFactory;	    /* 45027   16bit integer   r/w */
	uint16	  s365diFactory;	    /* 45028   16bit integer   r/w */
	uint16	  t420diFactory;	    /* 45029   16bit integer   r/w */
	uint16	  s420diFactory;	    /* 45030   16bit integer   r/w */
	uint16    errorCode;		      /* 45031   16bit integer   useless r/w */
	float	  slopeFactory;		      /* 45032   float */
	uint16    factoryParamSaved;  /* 45034 */
	uint16		reserved[16];				/* 45035-45050   reserved  ??*/
} FILTER_SETTINGS_T;

typedef struct measureValues
{																/* Register		Type  		 R/W */
	float		sensorValue;					/* 46001	32bit float		read  ����ֵ*/
	float		sensorValue_mA;				/* 46003	32bit float		read  ����ֵ(4-20ma��ʽ)*/
	float   temperatureValue;			/* 46005	32bit float		read  �¶�(���϶�)*/
	uint16		reserved[40];				/* 46011-46050  1eserved  ����*/
} MEASURE_VALUES_T;


// Below are sensor specific registers, pH meter may have different definition
// All sensor specific registers start from 48001
// ��ʱû����
typedef	struct sensorParam
{											/* Register		Type  		 R/W */	
	uint16 	reserved[50];        /* 48001-48050 */
} SENSOR_PARAM_T;
#pragma pack()

#endif
