#ifndef __TASK_INPUT_COLLECTOR_H
#define __TASK_INPUT_COLLECTOR_H

#include "sys.h"
#include "rtos_task.h"
#include "server_protocol.h"
#include "task_rs485.h"
#include "input_collector_conf.h"




extern TaskHandle_t xHandleTaskINPUT_COLLECTOR;


void vTaskINPUT_COLLECTOR(void *pvParameters);


void InputCollectorCollectCurrentState(void);
void InputCollectorRecvAndHandleDeviceFrame(void);


































#endif
