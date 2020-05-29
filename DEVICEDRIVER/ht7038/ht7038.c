#include "ht7038.h"
#include "spi.h"
#include "delay.h"
#include <math.h>

float HT7038_K = 0.0;

ELECTRICITY_METER_INFO BuiltInMeter;

void HT7038_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	//CAT 25640 CS配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	HT7038_CS = 1;
	
	SPI3_Init();
	SPI3_SetSpeed(SPI_BaudRatePrescaler_16);
}

u32 HT7038ReadDeviceID(void)
{
	u8 deviceid[3];
	
	HT7038_CS = 0;
	
	delay_us(1);
	
	SPI3_ReadWriteByte(0x00);
	
	deviceid[0] = SPI3_ReadWriteByte(0xff);
	deviceid[1] = SPI3_ReadWriteByte(0xff);
	deviceid[2] = SPI3_ReadWriteByte(0xff);
	
	HT7038_CS = 1;
	
	return (((u32)deviceid[0] << 16) + ((u32)deviceid[1] << 8) + (u32)deviceid[2]);
}

//读数据
void HT7038Read(u8* Buff,u8 ReadAddr)   
{ 
 	u16 i;
	
	HT7038_CS = 0;
	
	delay_us(1);
	
    SPI3_ReadWriteByte((u8)ReadAddr);  
	
    for(i = 0; i < 3; i ++)
	{ 
        Buff[i] = SPI3_ReadWriteByte(0XFF);   
    }

	HT7038_CS = 1; 	
} 
//mode:0,切换为读计量数据寄存器参数，1校表数据寄存器参数
void HT7038ModeConfig(u8 mode)   
{   										    
	HT7038_CS = 0;
	
	delay_us(1);
	
	SPI3_ReadWriteByte(0xC6);
	
	if(mode == 0)
	{   
		SPI3_ReadWriteByte(0xFF);
		SPI3_ReadWriteByte(0xFF);
		SPI3_ReadWriteByte(0xFF);
	}
	else
	{  
		SPI3_ReadWriteByte(0x00);
		SPI3_ReadWriteByte(0x00);
		SPI3_ReadWriteByte(0x5A);
	}
	
	HT7038_CS = 1; 	
} 
//校表数据写使能0关闭，1开启
void HT7038WriteEn(u8 mode)   
{   										    
	HT7038_CS = 0;
	
	delay_us(1);
	
	SPI3_ReadWriteByte(0xC9);
	
	if(mode == 0)
	{   
		SPI3_ReadWriteByte(0xFF);
		SPI3_ReadWriteByte(0xFF);
		SPI3_ReadWriteByte(0xFF);
	}
	else
	{  
		SPI3_ReadWriteByte(0x00);
		SPI3_ReadWriteByte(0x00);
		SPI3_ReadWriteByte(0x5A);
	}
	
	HT7038_CS = 1; 	
}
//写数据
void HT7038Write(u8* Buff,u8 ReadAddr)   
{ 	
	HT7038_CS = 0;
	
	delay_us(1);
	
	SPI3_ReadWriteByte(((u8)ReadAddr | 0x80));	
	
	SPI3_ReadWriteByte(Buff[0]);
	SPI3_ReadWriteByte(Buff[1]);
	SPI3_ReadWriteByte(Buff[2]);

	HT7038_CS = 1; 	
}

//配置相关寄存器
void HT7038ConfigClibration(void)
{
	u8 tempbuf[3] = {0,0xB9,0x7E};
	u8 data[3] = {0};
//	u16 hfconst = 0;

	HT7038WriteEn(1);		//写使能
	HT7038ModeConfig(1);	//切换为校表寄存器

	
	tempbuf[1] = 0xB9;
	tempbuf[2] = 0x7E;
	HT7038Write(tempbuf,HT7038_REG_MODE_CONFIG);		//模式配置
	
	tempbuf[1] = 0x0;
	tempbuf[2] = 0xFC;
	HT7038Write(tempbuf,HT7038_REG_ADC_CONFIG);
	
	tempbuf[1] = 0xF8;
	tempbuf[2] = 0x04;
	HT7038Write(tempbuf,HT7038_REG_EMU_CONFIG);
	
	tempbuf[1] = 0x34;
	tempbuf[2] = 0x27;
	HT7038Write(tempbuf,HT7038_REG_ANMODULE_EN);
	
	tempbuf[1] = 0x00;
	tempbuf[2] = 0x0C;
	HT7038Write(tempbuf,HT7038_REG_HF_CONST);
	
	tempbuf[1] = 0x52;
	tempbuf[2] = 0x18;
	HT7038Write(tempbuf,HT7038_REG_UGAIN_A);
	HT7038Write(tempbuf,HT7038_REG_UGAIN_B);
	HT7038Write(tempbuf,HT7038_REG_UGAIN_C);
	
	tempbuf[1] = 0xF4;
	tempbuf[2] = 0x99;
	HT7038Write(tempbuf,HT7038_REG_IGAIN_A);
	HT7038Write(tempbuf,HT7038_REG_IGAIN_B);
	HT7038Write(tempbuf,HT7038_REG_IGAIN_C);

	tempbuf[1] = 0x0C;
	tempbuf[2] = 0xA8;
	HT7038Write(tempbuf,HT7038_REG_PGAIN_A);
	HT7038Write(tempbuf,HT7038_REG_PGAIN_B);
	HT7038Write(tempbuf,HT7038_REG_PGAIN_C);
	HT7038Write(tempbuf,HT7038_REG_QGAIN_A);
	HT7038Write(tempbuf,HT7038_REG_QGAIN_B);
	HT7038Write(tempbuf,HT7038_REG_QGAIN_C);
	HT7038Write(tempbuf,HT7038_REG_SGAIN_A);
	HT7038Write(tempbuf,HT7038_REG_SGAIN_B);
	HT7038Write(tempbuf,HT7038_REG_SGAIN_C);
	
//	计算公式HFConst＝INT[2.592*10^10*G*G*Vu*Vi/(EC*Un*Ib)]
//	hfconst = 2.592 * pow(10,10) * 1.163 * 1.163*

	HT7038Read(data,HT7038_REG_HF_CONST);
	
	HT7038WriteEn(0);
	
	HT7038_K = (2.592f * (float)pow(10,10)) / ((float)HF_CONST * 6400.0f * (float)pow(2,23));

	HT7038ModeConfig(0);//切换为计量参数寄存器
}


u32 HT7038GetSingleParameter(u8 ReadAddr)
{
	u8 buf[3];
	u32 value;
	
	HT7038Read(buf,ReadAddr);
	
	value = (((u32)buf[0] << 16) + ((u32)buf[1] << 8) + (u32)buf[2]);
	
	return value;
}

//void HT7038GetParameters(void)
//{
//	float temp = 0.0f;
//	
//	s32 value = 0;

//	double active_energy[4] = {0.0f};
//	double reactive_energy[4] = {0.0f};
//	double apparent_energy[4] = {0.0f};
//	
//	//读取电压
//	value = HT7038GetSingleParameter(HT7038_REG_RMS_VOL_A);
//	BuiltInMeter.voltage[0] = (float)value / 8192.0f;

//	value = HT7038GetSingleParameter(HT7038_REG_RMS_VOL_B);
//	BuiltInMeter.voltage[1] = (float)value / 8192.0f;

//	value = HT7038GetSingleParameter(HT7038_REG_RMS_VOL_C);
//	BuiltInMeter.voltage[2] = (float)value / 8192.0f;

//	//读取电流
//	value = HT7038GetSingleParameter(HT7038_REG_RMS_CURRENT_A);
//	BuiltInMeter.current[0] = (float)value / 8192.0f / 10.2f * 40.0f / 16.0f;
//	if((BuiltInMeter.current[0] > 0.0f) && (BuiltInMeter.current[0] < 0.3f))
//	{
//		BuiltInMeter.current[0] = BuiltInMeter.current[0] - 0.2f;
//	}
//	
//	value = HT7038GetSingleParameter(HT7038_REG_RMS_CURRENT_B);
//	BuiltInMeter.current[1] = (float)value / 8192.0f / 10.2f * 40.0f / 16.0f;
//	if((BuiltInMeter.current[1] > 0.0f) && (BuiltInMeter.current[1] < 0.3f))
//	{
//		BuiltInMeter.current[1] = BuiltInMeter.current[1] - 0.2f;
//	}
//	
//	value = HT7038GetSingleParameter(HT7038_REG_RMS_CURRENT_C);
//	BuiltInMeter.current[2] = (float)value / 8192.0f / 10.2f * 40.0f / 16.0f;
//	if((BuiltInMeter.current[2] > 0.0f) && (BuiltInMeter.current[0] < 0.3f))
//	{
//		BuiltInMeter.current[2] = BuiltInMeter.current[2] - 0.2f;
//	}
//	
//	value = HT7038GetSingleParameter(HT7038_REG_RMS_CURRENT_ABC);
//	BuiltInMeter.current[3] = (float)value / 8192.0f / 10.2f * 40.0f / 16.0f;
//	if((BuiltInMeter.current[3] > 0.0f) && (BuiltInMeter.current[3] < 0.3f))
//	{
//		BuiltInMeter.current[3] = BuiltInMeter.current[3] - 0.2f;
//	}

//	//读取有功功率
//	value = HT7038GetSingleParameter(HT7038_REG_ACTIVE_POWER_A);
//	if(value & (1 << 23))
//	{
//		value = value - 0x01000000;
//	}
//	BuiltInMeter.active_power[0] = (float)value * HT7038_K;
//	
//	value = HT7038GetSingleParameter(HT7038_REG_ACTIVE_POWER_B);
//	if(value & (1 << 23))
//	{
//		value = value - 0x01000000;
//	}
//	BuiltInMeter.active_power[1] = (float)value * HT7038_K;
//	
//	value = HT7038GetSingleParameter(HT7038_REG_ACTIVE_POWER_C);
//	if(value & (1 << 23))
//	{
//		value = value - 0x01000000;
//	}
//	BuiltInMeter.active_power[2] = (float)value * HT7038_K;
//	
//	value = HT7038GetSingleParameter(HT7038_REG_ACTIVE_POWER_ABC);
//	if(value & (1 << 23))
//	{
//		value = value - 0x01000000;
//	}
//	BuiltInMeter.active_power[3] = (float)value * HT7038_K;
//		
//	//读取无功功率
//	value = HT7038GetSingleParameter(HT7038_REG_REACTIVE_POWER_A);
//	if(value & (1 << 23))
//	{
//		value = value - 0x01000000;
//	}
//	BuiltInMeter.reactive_power[0] = (float)value * HT7038_K;
//	
//	value = HT7038GetSingleParameter(HT7038_REG_REACTIVE_POWER_B);
//	if(value & (1 << 23))
//	{
//		value = value - 0x01000000;
//	}
//	BuiltInMeter.reactive_power[1] = (float)value * HT7038_K;
//	
//	value = HT7038GetSingleParameter(HT7038_REG_REACTIVE_POWER_B);
//	if(value & (1 << 23))
//	{
//		value = value - 0x01000000;
//	}
//	BuiltInMeter.reactive_power[2] = (float)value * HT7038_K;
//	
//	value = HT7038GetSingleParameter(HT7038_REG_REACTIVE_POWER_A);
//	if(value & (1 << 23))
//	{
//		value = value - 0x01000000;
//	}
//	BuiltInMeter.reactive_power[3] = (float)value * HT7038_K;
//	
//	//读取视在功率
//	value = HT7038GetSingleParameter(HT7038_REG_APPARENT_POWER_A);
//	if(value & (1 << 23))
//	{
//		value = value - 0x01000000;
//	}
//	BuiltInMeter.apparent_power[0] = (float)value * HT7038_K;
//	
//	value = HT7038GetSingleParameter(HT7038_REG_APPARENT_POWER_B);
//	if(value & (1 << 23))
//	{
//		value = value - 0x01000000;
//	}
//	BuiltInMeter.apparent_power[1] = (float)value * HT7038_K;
//	
//	value = HT7038GetSingleParameter(HT7038_REG_APPARENT_POWER_C);
//	if(value & (1 << 23))
//	{
//		value = value - 0x01000000;
//	}
//	BuiltInMeter.apparent_power[2] = (float)value * HT7038_K;
//	
//	value = HT7038GetSingleParameter(HT7038_REG_APPARENT_POWER_ABC);
//	if(value & (1 << 23))
//	{
//		value = value - 0x01000000;
//	}
//	BuiltInMeter.apparent_power[3] = (float)value * HT7038_K;
//	
//	//读取功率因数
//	value = HT7038GetSingleParameter(HT7038_REG_PFACTOR_A);
//	if(value & (1 << 23))
//	{
//		value = value - 0x01000000;
//	}
//	BuiltInMeter.power_factor[0] = (float)value / 0x800000;
//	
//	value = HT7038GetSingleParameter(HT7038_REG_PFACTOR_B);
//	if(value & (1 << 23))
//	{
//		value = value - 0x01000000;
//	}
//	BuiltInMeter.power_factor[1] = (float)value / 0x800000;
//	
//	value = HT7038GetSingleParameter(HT7038_REG_PFACTOR_C);
//	if(value & (1 << 23))
//	{
//		value = value - 0x01000000;
//	}
//	BuiltInMeter.power_factor[2] = (float)value / 0x800000;
//	
//	value = HT7038GetSingleParameter(HT7038_REG_PFACTOR_ABC);
//	if(value & (1 << 23))
//	{
//		value = value - 0x01000000;
//	}
//	BuiltInMeter.power_factor[3] = (float)value / 0x800000;
//	
//	//读取有功电能
//	value = HT7038GetSingleParameter(HT7038_REG_ACTIVE_ENERGY_A);
//	temp = (float)value / 6400.0f / 12.0f;
//	BuiltInMeter.active_energy[0] = (double)(active_energy[0] + temp);
//	
//	value = HT7038GetSingleParameter(HT7038_REG_ACTIVE_ENERGY_B);
//	temp = (float)value / 6400.0f / 12.0f;
//	BuiltInMeter.active_energy[1] = (double)(active_energy[1] + temp);
//	
//	value = HT7038GetSingleParameter(HT7038_REG_ACTIVE_ENERGY_C);
//	temp = (float)value / 6400.0f / 12.0f;
//	BuiltInMeter.active_energy[2] = (double)(active_energy[2] + temp);
//	
//	value = HT7038GetSingleParameter(HT7038_REG_ACTIVE_ENERGY_ABC);
//	temp = (float)value / 6400.0f / 12.0f;
//	BuiltInMeter.active_energy[3] = (double)(active_energy[3] + temp);
//	
//	//读取无功电能
//	value = HT7038GetSingleParameter(HT7038_REG_REACTIVE_ENERGY_A);
//	temp = (float)value / 6400.0f / 12.0f;
//	BuiltInMeter.reactive_energy[0] = (double)(reactive_energy[0] + temp);
//	
//	value = HT7038GetSingleParameter(HT7038_REG_REACTIVE_ENERGY_B);
//	temp = (float)value / 6400.0f / 12.0f;
//	BuiltInMeter.reactive_energy[1] = (double)(reactive_energy[1] + temp);
//	
//	value = HT7038GetSingleParameter(HT7038_REG_REACTIVE_ENERGY_C);
//	temp = (float)value / 6400.0f / 12.0f;
//	BuiltInMeter.reactive_energy[2] = (double)(reactive_energy[2] + temp);
//	
//	value = HT7038GetSingleParameter(HT7038_REG_REACTIVE_ENERGY_ABC);
//	temp = (float)value / 6400.0f / 12.0f;
//	BuiltInMeter.reactive_energy[3] = (double)(reactive_energy[3] + temp);
//	
//	//读取视在电能
//	value = HT7038GetSingleParameter(HT7038_REG_APPARENT_ENERGY_A);
//	temp = (float)value / 6400.0f / 12.0f;
//	BuiltInMeter.apparent_energy[0] = (double)(apparent_energy[0] + temp);
//	
//	value = HT7038GetSingleParameter(HT7038_REG_APPARENT_ENERGY_B);
//	temp = (float)value / 6400.0f / 12.0f;
//	BuiltInMeter.apparent_energy[1] = (double)(apparent_energy[1] + temp);
//	
//	value = HT7038GetSingleParameter(HT7038_REG_APPARENT_ENERGY_C);
//	temp = (float)value / 6400.0f / 12.0f;
//	BuiltInMeter.apparent_energy[2] = (double)(apparent_energy[2] + temp);
//	
//	value = HT7038GetSingleParameter(HT7038_REG_APPARENT_ENERGY_ABC);
//	temp = (float)value / 6400.0f / 12.0f;
//	BuiltInMeter.apparent_energy[3] = (double)(apparent_energy[3] + temp);
//	
//	//读取频率
//	value = HT7038GetSingleParameter(HT7038_REG_FREQUENCY);
//	BuiltInMeter.frequency = (float)value / 0x2000;
//}


































