#ifndef __EVENT_ALARM_H
#define __EVENT_ALARM_H

#include "sys.h"





typedef struct	EventReport					//�¼���¼�ṹ��
{
	u8 device_type;							//�豸����
	u8 device_address;						//�豸��ַ
	u8 device_channel;						//�豸ͨ��
	u8 event_type;							//�¼�����
	u8 *event_value;						//�¼�ֵ
	u8 occur_time[15];						//����ʱ��
}EventReport_S;

typedef struct	AlarmReport					//�¼���¼�ṹ��
{
	u8 record_type;							//��¼���� 1:���� 0:����
	u8 device_type;							//�豸����
	u8 alarm_type;							//�澯����
	u8 device_address;						//�豸��ַ
	u8 device_channel;						//�豸ͨ��
	u8 *current_value;						//��ǰֵ
	u8 *set_value;							//����ֵ
	u8 *reference_value;					//�ο�ֵ
	u8 occur_time[15];						//����ʱ��
}AlarmReport_S;





































#endif
