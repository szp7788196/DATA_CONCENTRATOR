#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 


#define	RS485_DIR			PDout(15)


#define RS485_RX			0
#define RS485_TX			1


#define USART1_MAX_RX_LN	32	
#define USART2_MAX_RX_LN	256
#define USART4_MAX_RX_LN	256	
#define USART5_MAX_RX_LN	256	



extern u16 Usart1RxCnt;
extern u16 Usart1FrameLen;
extern u8 Usart1RxBuf[USART1_MAX_RX_LN];
extern u8 Usart1RecvEnd;

extern u16 Usart2RxCnt;
extern u16 Usart2FrameLen;
extern u8 Usart2RxBuf[USART2_MAX_RX_LN];
extern u8 Usart2RecvEnd;

extern u16 Usart4RxCnt;
extern u16 Usart4FrameLen;
extern u8 Usart4RxBuf[USART4_MAX_RX_LN];
extern u8 Usart4RecvEnd;

extern u16 Usart5RxCnt;
extern u16 Usart5FrameLen;
extern u8 Usart5RxBuf[USART5_MAX_RX_LN];
extern u8 Usart5RecvEnd;




void USART1_Init(u32 BaudRate);
void USART2_Init(u32 BaudRate);
void USART4_Init(u32 BaudRate);
void USART5_Init(u32 BaudRate,u16 check_mode);
u8 UsartSendString(USART_TypeDef* USARTx,u8 *str, u16 len);



void TIM2_Init(u16 TIM2_Interval_xus);

#endif


