#include "app.h"
#include "LTC2630ISC6.h"
#include "color.h"
#include "math.h"
#include "stdio.h"

void Configure_IWDG(void)
{
  RCC->CSR |= RCC_CSR_LSION; 
  while((RCC->CSR & RCC_CSR_LSIRDY) != RCC_CSR_LSIRDY) 
  { 
    /* add time out here for a robust application */
  }
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	IWDG_SetReload(1562);//10S
	
	IWDG_ReloadCounter();
	IWDG_Enable();	
}

// u32 i=0;
u8 m=0,n=0;
u16 maxDI=1;
u16 minDI=65535;
u16 max=1;
u16 min=65535;
u16 s365Results[60]={0};
float sumS365DI=0;
float sumS365=0;
int main()
{
	#define VECT_TAB_OFFSET  0x3000
	
	float temp=1.0;

	/* Set the Vector Table base address */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, VECT_TAB_OFFSET);
	
	/* Configure HCLK clock as SysTick clock source. */
	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);
  
 	delay_init();
	
	Configure_IWDG();  //≈‰÷√ø¥√≈π∑
	
 	RestoreModbusReg(); 
		
 	eMBInit(MB_RTU, comm_settings.modbusAddr, 0x02, comm_settings.modbusBaud, comm_settings.modbusParity);
  eMBEnable();  
	LTC2630ISC6_Init();
	switch_GPIOInit();
	//Adc_Init();
	TIM2_MeasureInit();
	TIM1_ModpollInit();
	
  while(1)
 { 	
		//eMBPoll(); 
	  FunctionPoll(); 
	  sensor_param.SS365=filter_settings.s365;
	  if(system_status.runMode==AUTO_MODE)
		{
			 if(isMeasureFlg==1)
				{
					if(calib_settings.type==1)
					{
						isMeasureFlg=0;
						measureClF();
					}
					else if(calib_settings.type==0)
					{
						isMeasureFlg=0;
						measureUrea();
					}
					else
					{
						calib_settings.type=0;
						isMeasureFlg=0;
						measureUrea();			
					}
				}
			m=0;
			n=0;
			maxDI=1;
			minDI=65535;
			max=1;
			min=65535;
			sumS365DI=0;
		  sumS365=0;
		}
	  else if(system_status.runMode==COMMAND_MODE)
		{
			switch (system_status.command)
			{
				case CMD_IDLE:
				{
					break;
				}					
				case CMD_DI_START:
				{ 
					if(isSecond==1)
					{					
						isSecond=0;
						if(m<60)
						{
							s365Results[m]=StartMeasureS365(calib_settings.type);
							sumS365DI+=s365Results[m];
							if(s365Results[m]>maxDI)maxDI=s365Results[m];
							if(s365Results[m]<minDI)minDI=s365Results[m];
							n++;
						}
						else
						{
							sumS365DI=sumS365DI-maxDI-minDI;
					    measure_values.cmdMode_s365di=sumS365DI/(float)(m-2)*10;
						  m=0;
							maxDI=1;
							minDI=65535;
							sumS365DI=0;
							system_status.runMode=AUTO_MODE;
							system_status.command=CMD_IDLE;
						}						
					}
					break;
				}
				case CMD_DI_STOP:
				{
					sumS365DI=sumS365DI-maxDI-minDI;
					measure_values.cmdMode_s365di=sumS365DI/(float)(m-2);
					m=0;
					sumS365DI=0;
					maxDI=1;
					minDI=65535;
					sumS365DI=0;
					system_status.runMode=AUTO_MODE;
					system_status.command=CMD_IDLE;
					break;
				}
				case CMD_SAMPLE_START:
				{
					if(isSecond==1)
					{					
						isSecond=0;
						if(n<60)
						{
							s365Results[n]=StartMeasureS365(calib_settings.type);
							sumS365+=s365Results[n];
							if(s365Results[n]>max)max=s365Results[n];
							if(s365Results[n]<min)min=s365Results[n];
							n++;
						}
						else
						{
							sumS365=sumS365-max-min;
					    measure_values.cmdMode_s365=sumS365/(float)(n-2);
						  n=0;
							max=1;
							min=65535;
							sumS365=0;
							system_status.runMode=AUTO_MODE;
							system_status.command=CMD_IDLE;
						}						
					}
					break;
				}
				case CMD_SAMPLE_STOP:
				{
					sumS365=sumS365-max-min;
					measure_values.cmdMode_s365=sumS365/(float)(n-2)*10;
					n=0;
					max=1;
					min=65535;
					sumS365=0;
					
					temp=(float)measure_values.cmdMode_s365di/(float)measure_values.cmdMode_s365;
					measure_values.absbValue=log10(temp);
					
					system_status.runMode=AUTO_MODE;
					system_status.command=CMD_IDLE;					
					
					break;
				}
			}			
		}
		else
		{
			system_status.runMode=AUTO_MODE;
			system_status.command=CMD_IDLE;	
		}
	}
}

