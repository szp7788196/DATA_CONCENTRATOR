#ifndef __TASK_LWIP_INIT_H
#define __TASK_LWIP_INIT_H

#include "stm32f4x7_eth.h"
#include "netconf.h"
#include "dp83848.h"
#include "common.h"



extern TaskHandle_t xHandleTaskLWIP_INIT;

void vTaskLWIP_INIT(void *pvParameters);



































#endif
