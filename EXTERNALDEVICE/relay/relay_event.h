#ifndef __RELAY_EVENT_H
#define __RELAY_EVENT_H

#include "sys.h"


#define RELAY_CONTACT_ADJOIN			1		//继电器触点粘连告警
#define RELAY_CONTACT_ABNORMAL			2		//继电器触点异常告警
#define RELAY_TASK_ABNORMAL				3		//继电器任务内状态异常告警


















void RelayEventCheckPolling(void);



void RelayDetectStateChanges(void);
void RelayAlarmTaskAbnormal(void);
void RelayAlarmContactAbnormal(void);



















#endif
