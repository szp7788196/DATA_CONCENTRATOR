#ifndef __HISTORY_RECORD_H
#define __HISTORY_RECORD_H

#include "sys.h"
#include "server_protocol.h"


#define EVENT_NUM_OF_BATCH	5				//每批次上传的事件数


typedef enum
{
	TYPE_ALARM 			= 0,				//告警类型
	TYPE_EVENT 			= 1,				//事件类型
	TYPE_JOURNAL		= 2,				//日志类型
	TYPE_STATE			= 3,				//状态类型

} EVENT_TYPE_E;


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
	u8 occur_time[15];						//发生时间
}__attribute__((packed))AlarmReport_S;

typedef struct	EventHistory				//事件历史记录
{
	u8 device_type;							//设备类型
	u8 event_type;							//事件类型
	u8 *start_date;							//起始时间
	u8 *end_date;							//结束时间
}__attribute__((packed))EventHistory_S;




extern u32 SysAlarmState;



u8 StoreAlarmToSpiFlash(AlarmReport_S *alarm_report);


u16 GetAlarmEventNumFromDateSegment(EventHistory_S event_history);
void GetAlarmEventContentFromDateSegmentAndSendToServer(EventHistory_S event_history);
void DeleteAlarmReport(AlarmReport_S *alarm_report);
void CopyAlarmReport(AlarmReport_S *s_alarm_report,AlarmReport_S *d_alarm_report);
void DeleteEventHistory(EventHistory_S *event_history);


void PushAlarmReportToAlarmQueue(AlarmReport_S *alarm_report);





















#endif
