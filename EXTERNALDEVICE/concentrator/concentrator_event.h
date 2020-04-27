#ifndef __CONCENTRATOR_EVENT_H
#define __CONCENTRATOR_EVENT_H

#include "sys.h"




/********************************�澯����***************************************/
#define CONCENTRATOR_POWER_OFF			1		//�ϵ�澯
#define CONCENTRATOR_LEAKAGE			2		//©��澯
#define CONCENTRATOR_LOW_BATTERY		3		//��ص����͸澯
#define CONCENTRATOR_ABNORMAL_CHARGE	4		//�쳣���澯
#define CONCENTRATOR_ABNORMAL_COMM		5		//�쳣ͨѶ�澯







void ConcentratorEventCheckPolling(void);

void ConcentratorAlarmPowerFailure(void);



























#endif
