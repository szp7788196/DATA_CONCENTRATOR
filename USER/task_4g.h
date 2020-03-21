#ifndef __TASK_4G_H
#define __TASK_4G_H

#include "sys.h"
#include "rtos_task.h"
#include <time.h>
#include "server_protocol.h"





extern TaskHandle_t xHandleTask4G;

void vTask4G(void *pvParameters);

void Pull4gTxQueueAndSendFrame(void);
u8 SyncDataTimeFormEC20Module(time_t sync_cycle);





































#endif
