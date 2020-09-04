#ifndef __LUMETER_EVENT_H
#define __LUMETER_EVENT_H


#include "sys.h"



#define LUMETER_OVER_UNDER_VALUE			1		//照度值超限告警
#define LUMETER_DEVICE_ABNORMAL				2		//设备异常告警








void LumeterEventCheckPolling(void);


void LumeterOverValueAbnormal(void);
void LumeterUnderValueAbnormal(void);
void LumeterNoResponseAlarm(void);
void Lumeter24HourUnchangedAlarm(void);













































#endif
