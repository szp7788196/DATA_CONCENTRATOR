#ifndef __RELAY_EVENT_H
#define __RELAY_EVENT_H

#include "sys.h"


#define RELAY_CONTACT_ADJOIN			1		//�̵�������ճ���澯
#define RELAY_CONTACT_ABNORMAL			2		//�̵��������쳣�澯
#define RELAY_TASK_ABNORMAL				3		//�̵���������״̬�쳣�澯


















void RelayEventCheckPolling(void);



void RelayDetectStateChanges(void);
void RelayAlarmTaskAbnormal(void);
void RelayAlarmContactAbnormal(void);



















#endif
