#ifndef __TASK_TCP_CLIENT_H
#define __TASK_TCP_CLIENT_H


#include "common.h"



extern TaskHandle_t xHandleTaskTCP_CLIENT;

void vTaskTCP_CLIENT(void *pvParameters);
void tcp_client_init(void);  //tcp客户端初始化(创建tcp客户端线程)

void PullEthTxQueueAndSendFrame(void);





































#endif
