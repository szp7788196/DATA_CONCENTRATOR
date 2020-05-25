#ifndef __INPUT_COLLECTOR_EVENT_H
#define __INPUT_COLLECTOR_EVENT_H

#include "sys.h"



#define INPUT_COLLECTOR_D_ABNORMAL			1		//数字量异常告警
#define INPUT_COLLECTOR_A_ABNORMAL			2		//模拟量异常告警








void InputCollectorEventCheckPolling(void);

void InputCollectorDetectD_QuantityStateChanges(void);
void InputCollectorDetectA_QuantityStateChanges(void);
void InputCollectorD_QuantityAbnormal(void);
void InputCollectorA_OverQuantityAbnormal(void);
void InputCollectorA_UnderQuantityAbnormal(void);































#endif
