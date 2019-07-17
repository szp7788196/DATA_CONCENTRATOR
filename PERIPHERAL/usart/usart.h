#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 



#define Usart1RxLen	4096	
#define Usart1TxLen	4096



extern u16 Usart1RxCnt;
extern u16 OldUsart1RxCnt;
extern u16 Usart1FrameLen;
extern u8 Usart1RxBuf[Usart1RxLen];
extern u8 Usart1TxBuf[Usart1TxLen];
extern u8 Usart1RecvEnd;
extern u8 Usart1Busy;
extern u16 Usart1SendLen;
extern u16 Usart1SendNum;
extern u8 Usart1RecvCnt;
extern u8 Usart1SendCnt;




void usart1_config(u32 BaudRate);


void Usart1FrameSend(void);


void TIM2_Config(u16 TIM2_Interval_xus);

#endif


