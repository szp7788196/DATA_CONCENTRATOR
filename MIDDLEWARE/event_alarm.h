#ifndef __EVENT_ALARM_H
#define __EVENT_ALARM_H

#include "sys.h"





typedef struct	EventReport					//事件记录结构体
{
	u8 device_type;							//设备类型
	u8 *device_address;						//设备地址
	u8 *device_channel;						//设备通道
	u8 event_type;							//事件类型
	u8 *event_value;						//事件值
	u8 occur_time[15];						//发生时间
}__attribute__((packed))EventReport_S;

typedef struct	AlarmReport					//事件记录结构体
{
	u8 record_type;							//记录类型 1:发生 0:消除
	u8 device_type;							//设备类型
	u8 alarm_type;							//告警类型
	u8 *device_address;						//设备地址
	u8 *device_channel;						//设备通道
	u8 *current_value;						//当前值
	u8 *set_value;							//设置值
	u8 *reference_value;					//参考值
	u8 *occur_time;						//发生时间
}__attribute__((packed))AlarmReport_S;






u8 StoreAlarmToSpiFlash(AlarmReport_S *alarm_report);



void DeleteAlarmReport(AlarmReport_S *alarm_report);
void CopyAlarmReport(AlarmReport_S *s_alarm_report,AlarmReport_S *d_alarm_report);

























#endif
