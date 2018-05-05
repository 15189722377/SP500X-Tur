#include "AD5410.h"

void DA_SendByte(unsigned char byte);
extern u8 data_ReadFormRegister[3];
void DA_control(unsigned char addr,unsigned int data) ;

void AD5410GpioInit()
{

	GPIO_InitTypeDef g;
	g.GPIO_Mode=GPIO_Mode_Out_PP;
	g.GPIO_Speed=GPIO_Speed_50MHz;
	
	g.GPIO_Pin=GPIO_Pin_10;   //5410_CLEAR
	GPIO_Init(GPIOB,&g);
	
	g.GPIO_Pin=GPIO_Pin_11;   //5410_LATCH
	GPIO_Init(GPIOB,&g);
	
	g.GPIO_Pin=GPIO_Pin_12;	 //5410_CLOCK
	GPIO_Init(GPIOB,&g);
	
	g.GPIO_Pin=GPIO_Pin_13;		//5410_SLAVE_IN
	GPIO_Init(GPIOB,&g);
	
	
	g.GPIO_Mode=GPIO_Mode_IPU;
	g.GPIO_Pin=GPIO_Pin_1;    //5410_FAULT
	GPIO_Init(GPIOB,&g);
	
	g.GPIO_Mode=GPIO_Mode_IPU;
	g.GPIO_Pin=GPIO_Pin_2;    //5410_SLAVE_OUT
	GPIO_Init(GPIOB,&g);
	
	
 }
 
 /******************************/
 /*   ��AD5410��д��һ���ֽ�   */
 /******************************/
 void DA_SendByte(unsigned char byte)  
{		
		
    unsigned char i;
    for ( i = 0; i < 8; i++)
    {
        DA_CLK_LOW; 
        if ( byte & 0x80 )
				{DA_SIMO_HIGH;
					delay_us(1);}
        else
				{DA_SIMO_LOW;
					delay_us(1);}
        DA_CLK_HIGH; 
        byte <<= 1;     
        DA_CLK_LOW;
    }
}
/***********************************/
/*   ��AD5410��д������������      */
/*   �ܳ�3���ֽ� addr�Ĵ�����ַ    */
/*            data����ָ��         */
 void DA_control(unsigned char addr,unsigned int data)    
{
 u8 Hdata,Ldata;
 Hdata=data>>8;
 Ldata=data&255;
 DA_latch_low;
 DA_SendByte(addr);
 DA_SendByte(Hdata);
 DA_SendByte(Ldata);
 DA_latch_high;
	delay_us(2);
}

 
 void LATCH()
 {
	 DA_latch_low;
	 delay_us(5);
	 DA_latch_high;
	  delay_us(5);
	 DA_latch_low;

}
/**********************/
/*    ��ʼ��AD5410    */
void AD5410_init()
{
	AD5410GpioInit();
	DA_control(0x56,0x0001);   //��λ  0x56��λ�Ĵ�����ַ   0x0001��λָ��
	DA_CLEAR_LOW;
	LATCH();
	DA_control(0x55,0x1016);   //0-20ma  257730����Ƶ��   1/16������С
	LATCH();
}
/********************/
/* IOUT  0<=DATA<=20*/
void AD5410_IOUT(float DATA)
{
	u16 I_OUT;
	if(DATA>=20)
	{
		I_OUT=4095;
	}
	else if(DATA<=4)
	{
		DATA=4;
		I_OUT=DATA*4096/20;
	}
	else
	{
		I_OUT=DATA*4096/20;
	}
	I_OUT=((int)I_OUT)<<4;
	DA_control(0x01,I_OUT);///��AD5410��д������������
	LATCH();

}
/************************************/
/*    ��AD5410��д��ض�ָ��        */
/*    addrΪ�ض��ļĴ���������      */ 
/*    0x00��ȡ״̬�Ĵ���            */ 
/*    0x01��ȡ���ݼĴ���            */
/*    0x02��ȡ���ƼĴ���            */  
void DA_Read_Register(unsigned char addr)
{
 DA_latch_low; 
 DA_SendByte(0x02);               //�ض��Ĵ���   
 DA_SendByte(0x00);
 DA_SendByte(addr);
 DA_latch_high; 
}

/***********************************************/
/*            ��Register�лض�����             */
/*���ض������ݴ洢��data_ReadFormRegister[x] ��*/
void ReadDataFormRegister()    //���ض������ݴ洢��data_ReadFormRegister[x] ��
{
	u8 temp;
	u8 i,j;
	//DA_CLK_HIGH;
	DA_latch_low;
	DA_SIMO_LOW;
	delay_us(1);
	for(i=0; i<3; i++)
	{
		for(j=0; j<8; j++)
		{

			DA_CLK_LOW;

			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2)==0)
			{
				temp=temp<<1;
			}
			else
			{
				temp=temp<<1;
				temp=temp+0x01;
			}
	    data_ReadFormRegister[i]=temp;
			temp=0;
			DA_CLK_HIGH; 
		}
  }
	DA_SIMO_HIGH;
	DA_latch_high;
}
/************************************/
/*    ReadDatafromAD5410 Only       */
/*    addrΪ�ض��ļĴ���������      */  
/*    0x00��ȡ״̬�Ĵ���            */ 
/*    0x01��ȡ���ݼĴ���            */
/*    0x02��ȡ���ƼĴ���            */ 
void ReadDataFromAD5410(unsigned char addr)
{
	DA_Read_Register(addr);
	ReadDataFormRegister();
}
/********************/
/********************/
/*****���Իض�*******/
// void DA_Read_only(void)
// {
//  DA_latch_low; 
//  DA_SendByte(0x00);
//  DA_SendByte(0x00);
//  DA_SendByte(0x00);
//  DA_latch_high;
// }
// void TEST(unsigned char addr)
// {
// DA_Read_Register( addr);
// DA_Read_only();
// }
