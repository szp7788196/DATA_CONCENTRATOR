#ifndef __TASK_TCP_CLIENT_H
#define __TASK_TCP_CLIENT_H


#include "common.h"



#define TCP_CLIENT_RX_BUFSIZE	2000	//接收缓冲区长度
#define TCP_CLIENT_TX_BUFSIZE	2000	//接收缓冲区长度
#define REMOTE_PORT				16200	//定义远端主机的IP地址
#define LWIP_SEND_DATA			0X80    //定义有数据发送


extern u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP客户端接收数据缓冲区
extern u8 tcp_client_flag;		//TCP客户端数据发送标志位

void vTaskTCP_CLIENT(void *pvParameters);
void tcp_client_init(void);  //tcp客户端初始化(创建tcp客户端线程)





































#endif
