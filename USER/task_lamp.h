#ifndef __TASK_LAMP_H
#define __TASK_LAMP_H

#include "sys.h"
#include "rtos_task.h"
#include "server_protocol.h"





extern TaskHandle_t xHandleTaskLAMP;



void vTaskLAMP(void *pvParameters);







































#endif
