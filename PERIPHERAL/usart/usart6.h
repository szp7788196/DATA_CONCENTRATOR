#ifndef __USART6_H
#define __USART6_H

#include "sys.h"
#include "ringbuf.h"
#include "fifo.h"



#define NET_BUF_MAX_LEN		4096
#define CMD_BUF_MAX_LEN		512
#define DL_BUF_MAX_LEN		1536


extern RingBuf ring_fifo;
extern int8_t dl_buf_id;


void USART6_Init(u32 BaudRate);
void USART6_Write(uint8_t *Data, uint32_t len);


































#endif
