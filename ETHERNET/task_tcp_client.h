#ifndef __TASK_TCP_CLIENT_H
#define __TASK_TCP_CLIENT_H


#include "common.h"



#define TCP_CLIENT_RX_BUFSIZE	2000	//���ջ���������
#define TCP_CLIENT_TX_BUFSIZE	2000	//���ջ���������
#define REMOTE_PORT				16200	//����Զ��������IP��ַ
#define LWIP_SEND_DATA			0X80    //���������ݷ���


extern u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
extern u8 tcp_client_flag;		//TCP�ͻ������ݷ��ͱ�־λ

void vTaskTCP_CLIENT(void *pvParameters);
void tcp_client_init(void);  //tcp�ͻ��˳�ʼ��(����tcp�ͻ����߳�)





































#endif
