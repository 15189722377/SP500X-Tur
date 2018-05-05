#include "app.h"
#include "products.h"
#include "string.h"

/*-------------------功能轮询命令宏-------------------*/
#define CMD_JUMP_BOOTLOADER  7  //进入BootLoader
#define CMD_PARA_RESET	3		//参数复位
#define CMD_DIAGNOSTIC	2		//4-20MA设置
#define CMD_CALIBRATE	1		//校准
#define CMD_NONE      0    //NONE

/*------------------Modbus寄存器定义-----------------*/
SYS_STATUS_T system_status;

COMM_SETTINGS_T comm_settings;

MEASURE_SETTINGS_T measure_settings;

CALIB_SETTINGS_T calib_settings;

FILTER_SETTINGS_T filter_settings;

MEASURE_VALUES_T measure_values;

SENSOR_PARAM_T sensor_param;

/**
  * @brief  电导率测量参数复位
  * @param  无
  * @retval 无
  */
void SENSOR_MeasureParameterReset(void)
{
	memset(system_status.deviceName,0,sizeof(system_status.deviceName));
	memset(system_status.serial,0,sizeof(system_status.serial));
	memset(system_status.hardwareVer,0,sizeof(system_status.hardwareVer));
	memset(system_status.softwareVer,0,sizeof(system_status.softwareVer));
	
	//系统状态寄存器复位
	system_status.runStatus=0;
	system_status.commStatus=0;
	system_status.calibStatus=0;     //0  标定空闲
	system_status.configStatus=1;    //0  未配置    1 已配置
	system_status.productNum=ST720;
	strcpy(system_status.deviceName,"CONDUCTIVITY");
	strcpy(system_status.serial,"10");
	strcpy(system_status.hardwareVer,HW_VERSION);
	strcpy(system_status.softwareVer,SW_VERSION);
	memset(system_status.reserved,0,sizeof(system_status.reserved));
			
	//通信设置寄存器复位
	comm_settings.modbusAddr=0x0E;	
	comm_settings.modbusDatabits=8;
	comm_settings.modbusParity=MODBUS_PARITY_EVEN;
	comm_settings.modbusBaud=9600;
	memset(comm_settings.reserved,0,sizeof(comm_settings.reserved));

	memset(filter_settings.reserved,0,sizeof(filter_settings.reserved));
	
	calib_settings.firstCalibSolution=1400;//第一校准值
	calib_settings.calibCommand=CMD_NONE;   // 功能轮询寄存器值 1校准  2测量范围(4-20代表值)  3恢复出厂设置  7进入BootLoader  8木有功能，直接退出
	calib_settings.K=0.30;              //初始K值
	calib_settings.secondCalibSolution=0;
	calib_settings.Ra=510;									//四档位电阻
	calib_settings.Rb=1500;
	calib_settings.Rc=6800;
	calib_settings.Rd=27000;	
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
  * @brief  功能轮询
  * @param  无
  * @retval 无
  */
extern u8 bitmodbus;
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
	
	switch(calib_settings.calibCommand)   //地址44004
	{	
		case CMD_PARA_RESET:
		{		
			calib_settings.calibCommand=CMD_NONE;
			Rset();
			StoreModbusReg(); 
			break;
		}	
		case CMD_CALIBRATE:
		{
			calib_settings.calibCommand=CMD_NONE;
			system_status.calibStatus=0;
			break;	
		}
		case CMD_DIAGNOSTIC:
		{
			calib_settings.calibCommand=CMD_NONE;
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
