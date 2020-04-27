#ifndef __CONCENTRATOR_EVENT_H
#define __CONCENTRATOR_EVENT_H

#include "sys.h"




/********************************告警类型***************************************/
#define CONCENTRATOR_POWER_OFF			1		//断电告警
#define CONCENTRATOR_LEAKAGE			2		//漏电告警
#define CONCENTRATOR_LOW_BATTERY		3		//电池电量低告警
#define CONCENTRATOR_ABNORMAL_CHARGE	4		//异常充电告警
#define CONCENTRATOR_ABNORMAL_COMM		5		//异常通讯告警







void ConcentratorEventCheckPolling(void);

void ConcentratorAlarmPowerFailure(void);



























#endif
