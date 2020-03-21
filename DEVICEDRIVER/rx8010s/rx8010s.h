#ifndef _RX8010S_H
#define _RX8010S_H
#include "sys.h"
#include "common.h"


#define RX8010_Write_Address 0x64
#define RX8010_Read_Address 0x65



#define RX8010_REG_SEC		0x10
#define RX8010_REG_MIN		0x11
#define RX8010_REG_HOUR		0x12
#define RX8010_REG_WEEK		0x13
#define RX8010_REG_DAY		0x14
#define RX8010_REG_MONTH	0x15
#define RX8010_REG_YEAR		0x16
// 0x17 is reserved
#define RX8010_REG_ALMIN	0x18
#define RX8010_REG_ALHOUR	0x19
#define RX8010_REG_ALWDAY	0x1A
#define RX8010_REG_TCOUNT0	0x1B
#define RX8010_REG_TCOUNT1	0x1C
#define RX8010_REG_EXT		0x1D
#define RX8010_REG_FLAG		0x1E
#define RX8010_REG_CTRL		0x1F
#define RX8010_REG_USER0	0x20
#define RX8010_REG_USER1	0x21
#define RX8010_REG_USER2	0x22
#define RX8010_REG_USER3	0x23
#define RX8010_REG_USER4	0x24
#define RX8010_REG_USER5	0x25
#define RX8010_REG_USER6	0x26
#define RX8010_REG_USER7	0x27
#define RX8010_REG_USER8	0x28
#define RX8010_REG_USER9	0x29
#define RX8010_REG_USERA	0x2A
#define RX8010_REG_USERB	0x2B
#define RX8010_REG_USERC	0x2C
#define RX8010_REG_USERD	0x2D
#define RX8010_REG_USERE	0x2E
#define RX8010_REG_USERF	0x2F
// 0x30 is reserved
// 0x31 is reserved
#define RX8010_REG_IRQ		0x32


//时间结构体
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//公历日月年周
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;		 
}__attribute__((packed))_calendar_obj;	


extern _calendar_obj calendar;


void RX8010S_Init(void);
u8 RX8010S_Get_Time(void);
u8 RTC_Get_Week(u16 year,u8 month,u8 day);
u8 RX8010S_ReadOneByte(u8 ReadAddr);
void RX8010S_WriteOneByte(u8 WriteAddr,u8 Data);
void RX8010S_Set_Time(u8 syear,u8 smonth,u8 sday,u8 shour,u8 smin,u8 ssecond,u8 sweek);
u8 SyncTimeFromNet(u32 sec_num);
#endif
