#ifndef __TASK_LUMETER_H
#define __TASK_LUMETER_H

#include "sys.h"
#include "rtos_task.h"
#include "server_protocol.h"
#include "task_rs485.h"
#include "lumeter_conf.h"




extern TaskHandle_t xHandleTaskLUMETER;


void vTaskLUMETER(void *pvParameters);

void LumeterPushNewDateToStack(u32 *stack,u8 depth,u32 new_date,u8 *mode);
u32 LumeterGetAverageValue(u32 *stack,u8 depth,u8 ignore_num);
void LumeterGetAppValue(void);
void LumeterCollectCurrentState(void);
void LumeterRecvAndHandleDeviceFrame(void);


































#endif
