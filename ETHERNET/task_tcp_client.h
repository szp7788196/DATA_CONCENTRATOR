#ifndef __TASK_TCP_CLIENT_H
#define __TASK_TCP_CLIENT_H


#include "common.h"



extern TaskHandle_t xHandleTaskTCP_CLIENT;

void vTaskTCP_CLIENT(void *pvParameters);
void tcp_client_init(void);  //tcp�ͻ��˳�ʼ��(����tcp�ͻ����߳�)

void PullEthTxQueueAndSendFrame(void);





































#endif
