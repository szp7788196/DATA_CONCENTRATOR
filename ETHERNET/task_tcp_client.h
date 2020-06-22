#ifndef __TASK_TCP_CLIENT_H
#define __TASK_TCP_CLIENT_H


#include "common.h"

#define ETH_UNKNOW		0
#define ETH_CONNECTED	1


extern u8 ETH_ConnectState;

extern TaskHandle_t xHandleTaskTCP_CLIENT;

void vTaskTCP_CLIENT(void *pvParameters);
void tcp_client_init(void);  //tcp�ͻ��˳�ʼ��(����tcp�ͻ����߳�)

void PullEthTxQueueAndSendFrame(void);





































#endif
