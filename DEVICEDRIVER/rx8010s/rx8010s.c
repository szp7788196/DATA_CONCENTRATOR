#include "math.h"
#include "stdio.h"
#include "rx8010s.h"
#include "iic.h"
#include "delay.h"

_calendar_obj calendar;//时钟结构体


void RX8010S_Init(void)
{
	u32 data = 0;
	
	IIC_Init();
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	data = RTC_ReadBackupRegister(RTC_BKP_DR0);

	RX8010S_ReadOneByte(0x20);
	RX8010S_WriteOneByte(0x17,0x58);
	RX8010S_WriteOneByte(0x30,0x0);
	RX8010S_WriteOneByte(0x31,0x18);
	RX8010S_WriteOneByte(0x32,0x0);

	RX8010S_WriteOneByte(RX8010_REG_EXT,0x04);
	RX8010S_WriteOneByte(RX8010_REG_EXT,0);
	RX8010S_WriteOneByte(RX8010_REG_CTRL,0x02);

//	RX8010S_Set_Time(20,3,18,20,28,30,3);
	RX8010S_WriteOneByte(RX8010_REG_FLAG,0x0);
	RX8010S_WriteOneByte(RX8010_REG_CTRL,0x0);

	RX8010S_Get_Time();
}

//指定地址读出一个数据
//ReadAddr:开始读数的地址
//返回值  :读到的数据
u8 RX8010S_ReadOneByte(u8 ReadAddr)
{
	u8 temp = 0;
	
    IIC_Start();
	IIC_Send_Byte(RX8010_Write_Address);
	IIC_Wait_Ack();

    IIC_Send_Byte(ReadAddr);   //发送地址
	IIC_Wait_Ack();

	IIC_Start();
	IIC_Send_Byte(RX8010_Read_Address);
	IIC_Wait_Ack();
	
    temp = IIC_Read_Byte(1);
	
    IIC_Stop();//产生一个停止条件

	return temp;
}

//指定地址写入一个数据
//WriteAddr  :写入数据的目的地址
//Data:要写入的数据
void RX8010S_WriteOneByte(u8 WriteAddr,u8 Data)
{
    IIC_Start();
	IIC_Send_Byte(RX8010_Write_Address);
	IIC_Wait_Ack();

	IIC_Send_Byte(WriteAddr);
	IIC_Wait_Ack();
	IIC_Send_Byte(Data);     //发送字节
	IIC_Wait_Ack();
    IIC_Stop();//产生一个停止条件
}
u8 DATA_TO_BCD(u8 data)
{
	u8 temp = 0;
	u8 bcddata = 0;
	
	temp = (data / 10);
	bcddata = (temp << 4);
	temp = (data % 10);
	bcddata |= temp;
	
	return bcddata;
}

u8 BCD_TO_DATA(u8 bcd)
{
	u8 data = 0;
	
	data = ((bcd & 0xf0) >> 4) * 10 + (bcd & 0x0f);
	
	return data;
}

void RX8010S_Set_Time(u8 syear,u8 smonth,u8 sday,u8 shour,u8 smin,u8 ssecond)
{
	u8 sweek = 0;
	
	if(xSchedulerRunning == 1)
		xSemaphoreTake(xMutex_RTC, portMAX_DELAY);
	
	sweek = RTC_Get_Week(syear + 2000,smonth,sday);
	
	syear = DATA_TO_BCD(syear);
	smonth = DATA_TO_BCD(smonth);
	sday = DATA_TO_BCD(sday);
	shour = DATA_TO_BCD(shour);
	smin = DATA_TO_BCD(smin);
	ssecond = DATA_TO_BCD(ssecond);

	RX8010S_WriteOneByte(RX8010_REG_YEAR,syear);
	RX8010S_WriteOneByte(RX8010_REG_MONTH,smonth);
	RX8010S_WriteOneByte(RX8010_REG_DAY,sday);
	RX8010S_WriteOneByte(RX8010_REG_HOUR,shour);
	RX8010S_WriteOneByte(RX8010_REG_MIN,smin);
	RX8010S_WriteOneByte(RX8010_REG_SEC,ssecond);
	RX8010S_WriteOneByte(RX8010_REG_WEEK,sweek);
	
	if(xSchedulerRunning == 1)
		xSemaphoreGive(xMutex_RTC);
}
u8 RX8010S_Get_Time(void)
{
	u8 year = 0, month = 0, day = 0, hour = 0, min = 0, second = 0, week = 0;
	u8 cnt = 0;
	
	if(xSchedulerRunning == 1)
		xSemaphoreTake(xMutex_RTC, portMAX_DELAY);
	
	year = RX8010S_ReadOneByte(RX8010_REG_YEAR);
	month = RX8010S_ReadOneByte(RX8010_REG_MONTH);
	day = RX8010S_ReadOneByte(RX8010_REG_DAY);
	hour = RX8010S_ReadOneByte(RX8010_REG_HOUR);
	min = RX8010S_ReadOneByte(RX8010_REG_MIN);
	second = RX8010S_ReadOneByte(RX8010_REG_SEC);
	week = RX8010S_ReadOneByte(RX8010_REG_WEEK);

	calendar.w_year = BCD_TO_DATA(year) + 2000;
	calendar.w_month = BCD_TO_DATA(month);
	calendar.w_date = BCD_TO_DATA(day);
	calendar.hour = BCD_TO_DATA(hour);
	calendar.min = BCD_TO_DATA(min);
	calendar.sec = BCD_TO_DATA(second);
	
	while(week)
	{
		week = week / 2;
		cnt++;
	}
	
	calendar.week = (cnt - 1);
	
	if(xSchedulerRunning == 1)
		xSemaphoreGive(xMutex_RTC);
	
	return 0;
}

u8 Is_Leap_Year(u16 year)
{
	if((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

u8 const table_week[12] = {0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{
	u16 temp2;
	u8 yearH ,yearL;

	yearH = year / 100;	
	yearL = year % 100;
	
	// 如果为21世纪,年份数加100
	if (yearH > 19)
	{
		yearL += 100;
	}
	
	// 所过闰年数只算1900年之后的
	temp2 = yearL + yearL / 4;
	temp2 = temp2 % 7;
	temp2 = temp2 + day + table_week[month - 1];
	
	if(yearL % 4 == 0 && month < 3)
	{
		temp2 --;
	}
	
	return(temp2 % 7);
}

const u8 mon_table[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
u8 SyncTimeFromNet(u32 sec_num)
{
	u8 ret = 0;
	u32 temp = 0;
	u16 temp1 = 0;
	_calendar_obj calen;

	temp1 = 1970;
	temp = sec_num / 86400;

	while(temp >= 365)
	{
		if(Is_Leap_Year(temp1))
		{
			if(temp >= 366)
				temp -= 366;
			else
			{
//				temp1 ++;	//???????,???12?31????????1?1?
				break;
			}
		}
		else
		{
			temp -= 365;
		}
		temp1 ++;
	}

	calen.w_year = temp1;
	temp1 = 0;

	while(temp >= 28)
	{
		if(Is_Leap_Year(calen.w_year) && temp1 == 1)
		{
			if(temp >= 29)
				temp -= 29;
			else
				break;
		}
		else
		{
			if(temp >= mon_table[temp1])
				temp -= mon_table[temp1];
			else
				break;
		}
		temp1 ++;
	}
	calen.w_month = temp1 + 1;
	calen.w_date = temp + 1;

	temp = sec_num % 86400;
	calen.hour = temp / 3600;
	calen.min = (temp % 3600) / 60;
	calen.sec = (temp % 3600) % 60;

	RX8010S_Set_Time(calen.w_year - 2000,calen.w_month,calen.w_date,calen.hour,calen.min,calen.sec);

	return ret;
}


















