#include "stdio.h"
#include "cat25x.h"
#include "spi.h"
#include "delay.h"


void CAT25X_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	//CAT 25640 CS配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	CAT25X_CS = 1;

	SPI2_Init();
}


//写使能
void CAT25X_Write_Enable(void)
{
	CAT25X_CS = 0;

    SPI2_ReadWriteByte(CAT25X_WRITE_EN);

	CAT25X_CS = 1;
}

//写禁止
void CAT25X_Write_Disable(void)
{
	CAT25X_CS =0;

    SPI2_ReadWriteByte(CAT25X_WRITE_DIS);

	CAT25X_CS = 1;
}

//写状态
void CAT25X_Write_SR(u8 stat)
{
	CAT25X_CS = 0;

	SPI2_ReadWriteByte(CAT25X_WRITE_SR);
	SPI2_ReadWriteByte(stat);

	CAT25X_CS = 1;
}

//读状态
u8 CAT25X_Read_SR(void)
{
	u8 stat = 0;

	CAT25X_CS = 0;

	SPI2_ReadWriteByte(CAT25X_READ_SR);
	stat = SPI2_ReadWriteByte(0xFF);

	CAT25X_CS = 1;

	return stat;
}

//等待空闲
void CAT25X_Wait_Busy(void)
{
	while((CAT25X_Read_SR() & 0x01) == 0x01);   // 等待RDY位清空
}

//读数据
void CAT25X_Read(u8* Buff,u32 ReadAddr,u16 Num)
{
 	u16 i;

	if(xSchedulerRunning == 1)
		xSemaphoreTake(xMutex_SPI2, portMAX_DELAY);
	
	CAT25X_CS = 0;

    SPI2_ReadWriteByte(CAT25X_READ_MMY);
    SPI2_ReadWriteByte((u8)(ReadAddr>>8));
    SPI2_ReadWriteByte((u8)ReadAddr);

    for(i = 0; i < Num; i ++)
	{
        Buff[i] = SPI2_ReadWriteByte(0XFF);
    }

	CAT25X_CS = 1;
	
	if(xSchedulerRunning == 1)
		xSemaphoreGive(xMutex_SPI2);
}

//写单页数据数据，Num不超过64byte
void CAT25X_Write_Page(u8* Buff,u32 ReadAddr,u16 Num)
{
 	u16 i;

	if(xSchedulerRunning == 1)
		xSemaphoreTake(xMutex_SPI2, portMAX_DELAY);
	
	CAT25X_CS = 0;

	CAT25X_Write_Enable();

//	delay_us(10);
	CAT25X_CS = 1;
	CAT25X_CS = 0;

	SPI2_ReadWriteByte(CAT25X_WRITE_MMY);
	SPI2_ReadWriteByte((u8)(ReadAddr>>8));
	SPI2_ReadWriteByte((u8)ReadAddr);

	for(i = 0; i < Num; i ++)
	{
		SPI2_ReadWriteByte(*Buff ++);
	}

	CAT25X_CS = 1;

	CAT25X_Wait_Busy();
	
	if(xSchedulerRunning == 1)
		xSemaphoreGive(xMutex_SPI2);
}

//写数据
void CAT25X_Write(u8* Buff,u32 WriteAddr,u16 Num)
{
	u16 i = 0;
	uint16_t addr = 0;
	uint16_t len = 0, len1 = 0;

	if(xSchedulerRunning == 1)
		xSemaphoreTake(xMutex_SPI2, portMAX_DELAY);
	
	len = 64 - WriteAddr % 64;

	if(Num < len)
	{
		len = Num;
	}

	addr = WriteAddr;

	while(1)
	{
		CAT25X_CS = 0;

		CAT25X_Write_Enable();

		CAT25X_CS = 1;
		CAT25X_CS = 0;

		SPI2_ReadWriteByte(CAT25X_WRITE_MMY);
		SPI2_ReadWriteByte((u8)(addr >> 8));
		SPI2_ReadWriteByte((u8)addr);

		for(i = 0; i < len; i ++)
		{
			SPI2_ReadWriteByte(*Buff ++);
		}

		CAT25X_CS = 1;

		CAT25X_Wait_Busy();

		len1 += len;
		len = Num - len1;

		if(len == 0)
		{
			break;
		}

		addr = WriteAddr + len1;

		if(len > 64)
		{
			len = 64;
		}
	}
	
	if(xSchedulerRunning == 1)
		xSemaphoreGive(xMutex_SPI2);
}
































