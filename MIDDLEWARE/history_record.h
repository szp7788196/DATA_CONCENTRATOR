#ifndef __HISTORY_RECORD_H
#define __HISTORY_RECORD_H

#include "sys.h"
#include "server_protocol.h"


#define EVENT_NUM_OF_BATCH	5				//ÿ�����ϴ����¼���


typedef enum
{
	TYPE_ALARM 			= 0,				//�澯����
	TYPE_EVENT 			= 1,				//�¼�����
	TYPE_JOURNAL		= 2,				//��־����
	TYPE_STATE			= 3,				//״̬����

} EVENT_TYPE_E;


typedef struct	EventReport					//�¼���¼�ṹ��
{
	u8 device_type;							//�豸����
	u8 *device_address;						//�豸��ַ
	u8 *device_channel;						//�豸ͨ��
	u8 event_type;							//�¼�����
	u8 *event_value;						//�¼�ֵ
	u8 occur_time[15];						//����ʱ��
}__attribute__((packed))EventReport_S;

typedef struct	AlarmReport					//�¼���¼�ṹ��
{
	u8 record_type;							//��¼���� 1:���� 0:����
	u8 device_type;							//�豸����
	u8 alarm_type;							//�澯����
	u8 *device_address;						//�豸��ַ
	u8 *device_channel;						//�豸ͨ��
	u8 *current_value;						//��ǰֵ
	u8 *set_value;							//����ֵ
	u8 *reference_value;					//�ο�ֵ
	u8 occur_time[15];						//����ʱ��
}__attribute__((packed))AlarmReport_S;

typedef struct	EventHistory				//�¼���ʷ��¼
{
	u8 device_type;							//�豸����
	u8 event_type;							//�¼�����
	u8 *start_date;							//��ʼʱ��
	u8 *end_date;							//����ʱ��
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
