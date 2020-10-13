#include "usart6.h"
#include "usart.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
//#include "utils.h"
#include "at_cmd.h"
#include "ec20.h"
#include "common.h"


volatile char rsp_ok=0;
RingBuf ring_fifo;
RingBuf ring_fifo1;
uint8_t rx_fifo[NET_BUF_MAX_LEN];
uint8_t rx_fifo1[CMD_BUF_MAX_LEN];
int8_t dl_buf_id = -1;

FIFO(dl_buf,3,DL_BUF_MAX_LEN);

#define USART6_DMA


void USART6_Write(uint8_t *Data, uint32_t len)
{
//#ifndef USART6_DMA
	UsartSendString(USART6,Data,len);
//#else
//	DMA_InitTypeDef DMA_InitStruct;
//    DMA_Cmd(DMA2_Stream6,DISABLE);
//
//	DMA_ClearFlag(DMA2_Stream6,DMA_FLAG_TCIF6);
//	while (DMA_GetCmdStatus(DMA2_Stream6) != DISABLE);
//	DMA_InitStruct.DMA_Channel = DMA_Channel_5;
//    DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)(&USART6->DR);
//    DMA_InitStruct.DMA_Memory0BaseAddr  = (u32)Data;
//    DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//    DMA_InitStruct.DMA_BufferSize = len;
//    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
//    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
//    DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;
//	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
//	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
//	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
//	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
//    DMA_Init(DMA2_Stream6,&DMA_InitStruct);
//    DMA_Cmd(DMA2_Stream6,ENABLE);
//#endif
}


void USART6_Init(u32 BaudRate)
{
	static u8 first_init = 1;
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

#ifdef USART6_DMA
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
#endif

	USART_Cmd(USART6, DISABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOD时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//使能USART2时钟

	//串口2对应引脚复用映射
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6);

	//USART2端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	//USART2 初始化设置
	USART_InitStructure.USART_BaudRate = BaudRate;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART6, &USART_InitStructure); //初始化串口1

	if(first_init == 1)
	{
		first_init = 0;

		ringbuf_init(&ring_fifo, rx_fifo, sizeof(rx_fifo));
		ringbuf_init(&ring_fifo1, rx_fifo1, sizeof(rx_fifo1));
		dl_buf_id=fifo_init(&dl_buf);
		register_cmd_handler(USART6_Write,&ring_fifo1,&rsp_ok);
	}

#ifdef USART6_DMA
	//DMA1通道4配置
	DMA_DeInit(DMA2_Stream1);

	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE);

	DMA_InitStructure.DMA_Channel = DMA_Channel_5;

	//外设地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART6->DR);
	//内存地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)rx_fifo;
	//dma传输方向单向
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	//设置DMA在传输时缓冲区的长度
	DMA_InitStructure.DMA_BufferSize = NET_BUF_MAX_LEN;
	//设置DMA的外设递增模式，一个外设
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//设置DMA的内存递增模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//外设数据字长
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	//内存数据字长
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	//设置DMA的传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//设置DMA的优先级别
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;

	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输

	DMA_Init(DMA2_Stream1,&DMA_InitStructure);

	DMA_Cmd(DMA2_Stream1,ENABLE);
	USART_DMACmd(USART6,USART_DMAReq_Rx,ENABLE);
//	USART_DMACmd(USART6,USART_DMAReq_Tx,ENABLE);

	USART_ITConfig(USART6,USART_IT_IDLE,ENABLE);//开启空闲中断
#else
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//开启相关中断
#endif

	USART_Cmd(USART6, ENABLE);  //使能串口2
	USART_ClearFlag(USART6, USART_FLAG_TC);
	USART_ClearFlag(USART6, USART_FLAG_TXE);

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//串口6中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);
}

void USART6_IRQHandler(void)                	//串口1中断服务程序
{
	u8 data;
	u16 pos = 0;
	char temp_buf[8] = {0};
	u16 recv_len = 0;
	u8 len_len = 0;

	if(USART6->SR & 0x0F)
	{
		data = USART6->DR;

		data = data;
	}
#ifndef USART6_DMA
	else if(USART6->SR & USART_FLAG_RXNE)
	{
		data = USART6->DR;

		ringbuf_put(&ring_fifo,data);

		if(ringbuf_elements(&ring_fifo) == 1)
			USART_ITConfig(USART6,USART_IT_IDLE,ENABLE);
	}
#endif
	else if(USART6->SR & USART_FLAG_IDLE)
	{
		data = USART6->SR;
		data = USART6->DR;

#ifndef USART6_DMA
		USART_ITConfig(USART6, USART_IT_IDLE, DISABLE);
#else
		DMA_Cmd(DMA2_Stream1, DISABLE);  //先停止DMA才行设置缓冲区大小
		ring_fifo.put_ptr = NET_BUF_MAX_LEN - DMA_GetCurrDataCounter(DMA2_Stream1);
#endif

		ANALYSIS_LOOP:
		if(strstr((const char *)(ring_fifo.data + pos), "+QIURC: ") != NULL)
		{
			if(strstr((const char *)(ring_fifo.data + pos), "\"recv\"") != NULL)
			{
				get_str1((u8 *)(ring_fifo.data + pos), ",", 2, "\r\n", 2, (unsigned char *)temp_buf);

				recv_len = atoi(temp_buf);
				len_len = strlen(temp_buf);

				fifo_put(dl_buf_id,recv_len,ring_fifo.data + pos + 21 + len_len);

				pos += (21 + len_len + recv_len + 2);

				if(pos < ring_fifo.put_ptr)
				{
					goto ANALYSIS_LOOP;
				}
				else
				{
					goto GET_OUT;
				}
			}
			else if(strstr((const char *)ring_fifo.data, "\"closed\"") != NULL)
			{
				EC20ConnectState = CLOSING;
			}
		}
		else
		{
			rsp_ok = 1;
			ring_fifo1.get_ptr = ring_fifo.get_ptr;
			ring_fifo1.put_ptr = ring_fifo.put_ptr;
			memcpy(ring_fifo1.data,ring_fifo.data,ringbuf_elements(&ring_fifo));
		}

		GET_OUT:
		ringbuf_clear(&ring_fifo);
	}

#ifdef USART6_DMA
	DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_TCIF1 | DMA_FLAG_FEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1);//清除DMA2_Steam7传输完成标志
	DMA_SetCurrDataCounter(DMA2_Stream1,NET_BUF_MAX_LEN);   //重新设置DMA的读取缓冲区长度
	DMA_Cmd(DMA2_Stream1, ENABLE);  //开启DMA
#endif
}





































