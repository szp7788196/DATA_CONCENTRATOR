#ifndef __USART6_H
#define __USART6_H

#include "sys.h"



#define NET_BUF_MAX_LEN		4096
#define CMD_BUF_MAX_LEN		512



extern int8_t dl_buf_id;


void USART6_Init(u32 BaudRate);
void USART6_Write(uint8_t *Data, uint32_t len);


































#endif
