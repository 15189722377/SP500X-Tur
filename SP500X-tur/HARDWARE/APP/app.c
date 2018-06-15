#include "app.h"
#include "products.h"
#include "string.h"

u8 bitmodbus;
u8 isMeasureFlg=0;
u8 measureCount=0;
float s365DiCalib,s365CalibL,s365CalibH,S365Calib;

/*------------------Modbus�Ĵ�������-----------------*/
SYS_STATUS_T system_status;

COMM_SETTINGS_T comm_settings;

MEASURE_SETTINGS_T measure_settings;

CALIB_SETTINGS_T calib_settings;

FILTER_SETTINGS_T filter_settings;

MEASURE_VALUES_T measure_values;

SENSOR_PARAM_T sensor_param;

/**
  * @brief  �絼�ʲ���������λ
  * @param  ��
  * @retval ��
  */
void SENSOR_MeasureParameterReset(void)
{
	memset(system_status.deviceName,0,sizeof(system_status.deviceName));
	memset(system_status.serial,0,sizeof(system_status.serial));
	memset(system_status.hardwareVer,0,sizeof(system_status.hardwareVer));
	memset(system_status.softwareVer,0,sizeof(system_status.softwareVer));
	
	//ϵͳ״̬�Ĵ�����λ
	system_status.runStatus=0;
	system_status.commStatus=0;
	system_status.calibStatus=0;     //0  �궨����
	system_status.configStatus=1;    //0  δ����    1 ������
	system_status.productNum=65535;
	strcpy(system_status.deviceName,"TURBIDITY");
	strcpy(system_status.serial,"10");
	strcpy(system_status.hardwareVer,HW_VERSION);
	strcpy(system_status.softwareVer,SW_VERSION);
	memset(system_status.reserved,0,sizeof(system_status.reserved));
			
	//ͨ�����üĴ�����λ
	comm_settings.modbusAddr=0x0E;	
	comm_settings.modbusDatabits=8;
	comm_settings.modbusParity=MODBUS_PARITY_EVEN;
	comm_settings.modbusBaud=9600;
	memset(comm_settings.reserved,0,sizeof(comm_settings.reserved));

	memset(filter_settings.reserved,0,sizeof(filter_settings.reserved));
	
	calib_settings.calibCommand=CMD_NONE;   // ������ѯ�Ĵ���ֵ 1У׼  2������Χ(4-20����ֵ)  3�ָ���������  7����BootLoader 
	calib_settings.solutionL=3.0;//��һУ׼ֵ
	calib_settings.solutionH=7.0;
	memset(calib_settings.reserved,0,sizeof(calib_settings.reserved));
	
	measure_settings.smoothingFactor=0.3;   //fitPar
	measure_settings.sampleCycle=4;  //uint16
	memset(measure_settings.reserved,0,sizeof(measure_settings.reserved));
	
	memset(filter_settings.reserved,0,sizeof(filter_settings.reserved));
	memset(sensor_param.reserved,0,sizeof(sensor_param.reserved));
	
	StoreModbusReg();
}
void Rset()
{
	SENSOR_MeasureParameterReset();
	StoreModbusReg();
}

/**
  * @brief  ������ѯ
  * @param  ��
  * @retval ��
  */
void FunctionPoll(void)
{
	if(MODBUS_isRegWrite)
	{
		MODBUS_isRegWrite=0;
		StoreModbusReg();
	}
	if(bitmodbus)
	{
		bitmodbus=0;
		StoreModbusReg();
	}
	
	switch(calib_settings.calibCommand)   //��ַ44004
	{	
		case CMD_PARA_RESET:
		{		
			calib_settings.calibCommand=CMD_NONE;
			Rset();
			StoreModbusReg(); 
			break;
		}	
		case CMD_JUMP_BOOTLOADER:
		{
			calib_settings.calibCommand=CMD_NONE;
			StoreModbusReg();
			delay_ms(1500);
			__disable_irq();
			NVIC_SystemReset();
			break;
		}
		default: break;
	}	
}

void TIM2_MeasureInit(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  uint16_t PrescalerValue = 0;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  PrescalerValue = (uint16_t) (SystemCoreClock / 10000) - 1; 
  
  TIM_TimeBaseStructure.TIM_Period =9999;     //    9999,timing cycle = 1S;��Ϊ1���һ���жϣ��Ӷ��������ڿɵ�
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  
  TIM_ARRPreloadConfig(TIM2, ENABLE);
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //1,2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	TIM_ClearITPendingBit(TIM2,TIM_IT_Update); 
	TIM_ITConfig(TIM2, TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM2,ENABLE);
}

void TIM2_IRQHandler(void)
{  
	if( TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
	{
		if(++measureCount>=measure_settings.sampleCycle)
		{
 			IWDG_ReloadCounter();    //ι��
			isMeasureFlg=1;
			measureCount=0;
		} 
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}	
}

void TIM1_ModpollInit(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  uint16_t PrescalerValue = 0;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  
  PrescalerValue = (uint16_t) (SystemCoreClock / 10000) - 1; // 0.1ms
  
  TIM_TimeBaseStructure.TIM_Period =99;     
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  
  TIM_ARRPreloadConfig(TIM1, ENABLE);
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_ClearFlag(TIM1, TIM_FLAG_Update); 
	//TIM_ClearITPendingBit(TIM1,TIM_IT_Update); 
	TIM_ITConfig(TIM1, TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM1,ENABLE);
}

void TIM1_UP_IRQHandler(void)
{  
	if( TIM_GetITStatus(TIM1,TIM_IT_Update)!=RESET)
	{
		eMBPoll(); 
		IWDG_ReloadCounter();    //ι��
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);	
	}	
}
