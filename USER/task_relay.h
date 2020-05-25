#ifndef __TASK_RELAY_H
#define __TASK_RELAY_H

#include "sys.h"
#include "rtos_task.h"
#include "server_protocol.h"





extern TaskHandle_t xHandleTaskRELAY;



void vTaskRELAY(void *pvParameters);


void RelayExecuteStrategyGroup(void);
void RelayExecuteTemporaryStrategyGroup(void);
void RelayCheckForceSwitchOffAllRelays(void);
void RelayExecuteActions(void);
void RelayCollectCurrentState(void);
void RelayRecvAndHandleDeviceFrame(void);





































#endif
