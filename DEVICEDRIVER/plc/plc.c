#include "FreeRTOS.h"
#include "task.h"
#include "plc.h"
#include "usart.h"
#include "common.h"


void PLC_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	PLC_RST = 1;
}


u8 at_csum(u8 *buf,u16 len)
{
	u8 csum = 0,i = 0;

	for(i = 0; i < len; i ++)
	{
		csum += *buf++;
	}

	return csum;
}

u8 at_cxor(u8 *buf,u16 len)
{
	u8 csum = 0,i = 0;

	for(i = 0; i < len; i ++)
	{
		csum ^= *buf++;
	}

	return csum;
}
//cmd:发送命令，buff:发送数据缓存，len:发送数据长度
u16 plc_combine_data(u8 cmd,u8 *buff,u16 len,u8 *outbuf)
{
	outbuf[0] = 0x79;
	outbuf[1] = (u8)len;
	outbuf[2] = (u8)(len >> 8);
	outbuf[3] = 0x58;
	outbuf[4] = cmd;

	memcpy(outbuf + 5,buff,len);

	outbuf[5 + len] = at_csum(outbuf + 1,(4 + len));
	outbuf[6 + len] = at_cxor(outbuf + 1,(4 + len));

	return len + 5 + 2;
}

//将数据打包成用户数据格式
u16 plc_pack_user_data(u8 *dis_add,u8 *inbuf,u16 inbuf_len,u8 *outbuf)
{
	u16 out_len = 0;

	*(outbuf + 0) = 0x44;
	*(outbuf + 1) = 0x00;

	memcpy(outbuf + 2,dis_add,6);

	*(outbuf + 8) = (u8)(inbuf_len & 0x00FF);
	*(outbuf + 9) = (u8)(inbuf_len >> 8);

	memcpy(outbuf + 10,inbuf,inbuf_len);

	out_len = 10 + inbuf_len;

	return out_len;
}

void plc_get_addr(u8 *addr)
{
	u8 send_len = 0;
	u8 send_buf[32];
	
	xSemaphoreTake(xMutex_USART2, portMAX_DELAY);
	
	send_len = plc_combine_data(0x0B,NULL,0,send_buf);
	
	UsartSendString(USART2,send_buf, send_len);
	
	send_len = 0;
	
	delay_ms(200);
	
	if(Usart2RecvEnd == 0xAA)
	{
		if(Usart2FrameLen == 0x14)
		{
			memcpy(addr,&Usart2RxBuf[12],6);
		}
		else if(Usart2FrameLen == 0x0D)
		{
			memcpy(addr,&Usart2RxBuf[5],6);
		}
	}
	
	Usart2RecvEnd = 0;
	Usart2FrameLen = 0;
	
	xSemaphoreGive(xMutex_USART2);
}

void plc_set_addr(u8 *addr)
{
	u8 send_len = 0;
	u8 send_buf[32];
	
	xSemaphoreTake(xMutex_USART2, portMAX_DELAY);
	
	send_len = plc_combine_data(0x0C,addr,6,send_buf);
	
	UsartSendString(USART2,send_buf, send_len);
	
	send_len = 0;
	
	xSemaphoreGive(xMutex_USART2);
}




















