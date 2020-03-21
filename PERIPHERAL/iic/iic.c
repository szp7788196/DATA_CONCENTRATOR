#include "iic.h" 
#include "delay.h" 
#include "sys.h"

void IIC_Init(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	IIC_SCL_H;
	IIC_SDA_H;
}

void IIC_SDA_Out(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void IIC_SDA_In(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void IIC_Start(void)
{
	IIC_SDA_Out();
	IIC_SCL_H;
	IIC_SDA_H;
	delay_us(10);
	IIC_SDA_L;
	delay_us(10);
	IIC_SCL_L;
	
}

void IIC_Stop(void)
{
	IIC_SCL_L;
	IIC_SDA_Out();
	IIC_SDA_L;
	delay_us(10);
	IIC_SCL_H;
	delay_us(10);
	IIC_SDA_H;
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 cnt =0;
	IIC_SCL_L;
	delay_us(10);
	IIC_SDA_In();
	IIC_SCL_H;
	delay_us(10);
	IIC_SDA_H;
	delay_us(10);
	while(IIC_SDA_IN==1)
	{
		cnt++;
		if(cnt >200)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_L;
	return 0;
}
	
void IIC_Ack(void)
{
	IIC_SDA_Out();
	IIC_SCL_H;
	delay_us(10);
	IIC_SDA_L;
	delay_us(10);
	IIC_SCL_L;
}

void IIC_NAck(void)
{
	IIC_SDA_Out();
	IIC_SCL_H;
	delay_us(10);
	IIC_SDA_H;
	delay_us(10);
	IIC_SCL_L;
}

u8 IIC_Read_Byte(u8 ACK)
{
	u8 i, reseive = 0;
	IIC_SDA_In();
	for(i =0; i<8; i++)
	{
		IIC_SCL_L;
		delay_us(10);
		IIC_SCL_H;
		reseive = reseive<<1;
		if(IIC_SDA_IN==1)
		{
			reseive += 1;
		}
		else
		{
			reseive += 0;
		}
	}
	
	if(ACK)
		IIC_Ack();
	else
		IIC_NAck();
	
	return reseive;
}

void IIC_Send_Byte(u8 txd)
{
	u8 i;
	IIC_SDA_Out();
	
	for(i =0; i<8; i++)
	{
		IIC_SCL_L;
		delay_us(10);
		if((txd&0x80) ==0x80)
			IIC_SDA_H;
		else
			IIC_SDA_L;		
		
		delay_us(10);
		IIC_SCL_H;
		delay_us(10);
		txd= txd<<1;
	}
}






