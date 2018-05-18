#include "app.h"
#include "products.h"
#include "string.h"
#include "tur.h"

u8 bitmodbus;
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
		case CMD_CALIB_STEP1:
		{
			calib_settings.calibCommand=CMD_NONE;
			system_status.calibStatus=NOERR;
			
			TIM_Cmd(TIM2,DISABLE); 
			s365CalibL=Calib_S365();
			calib_settings.s365L=s365CalibL*10;
			TIM_Cmd(TIM2,ENABLE); 
			break;	
		}
		case CMD_CALIB_STEP2:
		{
			calib_settings.calibCommand=CMD_NONE;
			
			TIM_Cmd(TIM2,DISABLE); 
			s365CalibH=Calib_S365();
			calib_settings.s365H=s365CalibH*10;
			TIM_Cmd(TIM2,ENABLE); 
			if((calib_settings.s365H-calib_settings.s365L)!=0)
			{
				calib_settings.k=(calib_settings.solutionH-calib_settings.solutionL)/(s365CalibH-s365CalibL);
				calib_settings.b=calib_settings.solutionL-calib_settings.k*s365CalibL;
				if(calib_settings.k>0)
				{
					s365DiCalib=-calib_settings.b/calib_settings.k;
					if(s365DiCalib>0)
					{
						S365Calib=s365CalibH/s365DiCalib;
						if(S365Calib-1.0>0)
						{
							filter_settings.slope=calib_settings.solutionH/(S365Calib-1.0);
							filter_settings.s365di=s365DiCalib*10;
							system_status.calibStatus=NOERR;
							StoreModbusReg();
						}
					}
					else
					{
						system_status.calibStatus=S365DI_ERR;
					}
				}
				else
				{
					system_status.calibStatus=SOLUTION_VALUE_ERR;
				}
			}
			else
			{
				system_status.calibStatus=S365_ERR;
			}
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
