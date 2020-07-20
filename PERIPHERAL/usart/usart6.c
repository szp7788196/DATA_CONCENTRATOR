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



void USART6_Init(u32 BaudRate)
{
	static u8 first_init = 1;
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	USART_Cmd(USART6, DISABLE);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //ʹ��GPIODʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//ʹ��USART2ʱ��
	
	//����2��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); 

	//USART2�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOC,&GPIO_InitStructure); 

	//USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = BaudRate;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART6, &USART_InitStructure); //��ʼ������1
	
	if(first_init == 1)
	{
		first_init = 0;
		
		ringbuf_init(&ring_fifo, rx_fifo, sizeof(rx_fifo));
		ringbuf_init(&ring_fifo1, rx_fifo1, sizeof(rx_fifo1));
		dl_buf_id=fifo_init(&dl_buf);
		register_cmd_handler(USART6_Write,&ring_fifo1,&rsp_ok);
	}
	
	USART_Cmd(USART6, ENABLE);  //ʹ�ܴ���2
	USART_ClearFlag(USART6, USART_FLAG_TC);
	USART_ClearFlag(USART6, USART_FLAG_TXE);

	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//��������ж�
	USART_ITConfig(USART6,USART_IT_IDLE,ENABLE);//���������ж�
	
	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//����6�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);
}


void USART6_Write(uint8_t *Data, uint32_t len)
{
	UsartSendString(USART6,Data,len);
}

void USART6_IRQHandler(void)                	//����1�жϷ������
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
	else if(USART6->SR & USART_FLAG_RXNE)
	{
		data = USART6->DR;
		
		ringbuf_put(&ring_fifo,data);
		
		if(ringbuf_elements(&ring_fifo) == 1)
			USART_ITConfig(USART6,USART_IT_IDLE,ENABLE);
	}
	else if(USART6->SR & USART_FLAG_IDLE)
	{
		data = USART6->SR;
		data = USART6->DR;
		
		USART_ITConfig(USART6, USART_IT_IDLE, DISABLE);	
		
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
}





































