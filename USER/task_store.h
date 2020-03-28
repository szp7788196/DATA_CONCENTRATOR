#ifndef __TASK_STORE_H
#define __TASK_STORE_H

#include "sys.h"
#include "rtos_task.h"




extern TaskHandle_t xHandleTaskSTORE;



void vTaskSTORE(void *pvParameters);








void RecvAndStoreAlarmReport(void);


































#endif
