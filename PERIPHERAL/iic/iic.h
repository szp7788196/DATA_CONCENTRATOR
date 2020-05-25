#ifndef __IIC_H
#define __IIC_H

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "delay.h"


#define IIC_GPIO_PORT              	 	GPIOB
#define IIC_SCL_GPIO_Pin             	GPIO_Pin_8
#define IIC_SDA_GPIO_Pin             	GPIO_Pin_9
#define IIC_GPIO_CLK                 	RCC_AHB1Periph_GPIOB
#define IIC_SCL                      	PBout(8)
#define IIC_SDA               		 	PBout(9)
#define IIC_SCL_H					 	(PBout(8)=1)
#define IIC_SCL_L					 	(PBout(8)=0)
#define IIC_SDA_H					 	(PBout(9)=1)
#define IIC_SDA_L					 	(PBout(9)=0)
#define IIC_SDA_IN					 	PBin(9)


//#define IIC_SCL_H					 	PBout(8) = 1
//#define IIC_SCL_L					 	PBout(8) = 0
//#define IIC_SDA_H					 	PBout(9) = 1
//#define IIC_SDA_L					 	PBout(9) = 0

//#define IIC_SDA_In()	GPIOB->MODER|=GPIO_Mode_IN<<18; //sdaÊäÈë
//#define IIC_SDA_Out()   GPIOB->MODER|=GPIO_Mode_OUT<<18;//sdaÊä³ö

#define IIC_SDA_IN					 	PBin(9)



void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
u8 IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Send_Byte(u8 txd);
u8 IIC_Read_Byte(unsigned char ack);



#endif
