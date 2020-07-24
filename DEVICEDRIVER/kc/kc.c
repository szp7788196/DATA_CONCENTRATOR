#include "kc.h"
#include "delay.h"

u16 BuiltInRelayState = 0;

//RELAY IO初始化
void KC_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	//使能GPIOF时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 |
	                              GPIO_Pin_12 | GPIO_Pin_13 |
	                              GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

//控制集控内部继电器
void ControlAllBuiltInRelay(RelayModuleState_S state)
{
	u8 i = 0;

	for(i = 0; i < 6; i ++)
	{
		if(state.loop_current_channel & (1 << i))
		{
			if(state.loop_current_state & (1 << i))
			{
				switch(i)
				{
					case 0:
						KC1 = 1;
						BuiltInRelayState |= (1 << 0);
					break;

					case 1:
						KC2 = 1;
						BuiltInRelayState |= (1 << 1);
					break;

					case 2:
						KC3 = 1;
						BuiltInRelayState |= (1 << 2);
					break;

					case 3:
						KC4 = 1;
						BuiltInRelayState |= (1 << 3);
					break;

					case 4:
						KC5 = 1;
						BuiltInRelayState |= (1 << 4);
					break;

					case 5:
						KC6 = 1;
						BuiltInRelayState |= (1 << 5);
					break;

					default:
					break;
				}
			}
			else
			{
				switch(i)
				{
					case 0:
						KC1 = 0;
						BuiltInRelayState &= ~(1 << 0);
					break;

					case 1:
						KC2 = 0;
						BuiltInRelayState &= ~(1 << 1);
					break;

					case 2:
						KC3 = 0;
						BuiltInRelayState &= ~(1 << 2);
					break;

					case 3:
						KC4 = 0;
						BuiltInRelayState &= ~(1 << 3);
					break;

					case 4:
						KC5 = 0;
						BuiltInRelayState &= ~(1 << 4);
					break;

					case 5:
						KC6 = 0;
						BuiltInRelayState &= ~(1 << 5);
					break;

					default:
					break;
				}
			}

			if(state.interval_time == 0)
			{
				delay_ms(100);
			}
			else
			{
				delay_ms(state.interval_time);
			}
		}
	}
}

//控制集控外部继电器模块
void ControlAllBuiltOutRelay(RelayModuleState_S state)
{
	u8 inbuf[4] = {0};
	u8 outbuf[48] = {0};
	Rs485Frame_S *frame = NULL;

	inbuf[0] = (u8)((state.loop_current_channel >> 8) & 0x00FF);
	inbuf[1] = (u8)((state.loop_current_channel >> 0) & 0x00FF);
	inbuf[2] = (u8)((state.loop_current_state >> 8) & 0x00FF);
	inbuf[3] = (u8)((state.loop_current_state >> 0) & 0x00FF);
	
	frame = (Rs485Frame_S *)pvPortMalloc(sizeof(Rs485Frame_S));

	if(frame != NULL)
	{
		frame->device_type = RELAY;
		
		frame->len = PackBuiltOutRelayFrame(state.address,0xD1,inbuf,4,outbuf);
		
		frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));
		
		if(frame->buf != NULL)
		{
			memcpy(frame->buf,outbuf,frame->len);
			
			xSemaphoreTake(xMutex_Rs485Rs485Frame, portMAX_DELAY);

			if(xQueueSend(xQueue_Rs485Rs485Frame,(void *)&frame,(TickType_t)10) != pdPASS)
			{
#ifdef DEBUG_LOG
				printf("relay send xQueue_Rs485Rs485Frame fail.\r\n");
#endif
				DeleteRs485Frame(frame);
			}
			
			xSemaphoreGive(xMutex_Rs485Rs485Frame);
		}
		else
		{
			DeleteRs485Frame(frame);
		}
	}	
}

//采集集控外部继电器模块状态
void GetBuiltOutRelayState(RelayModuleState_S state)
{
	u8 outbuf[48] = {0};
	Rs485Frame_S *frame = NULL;
	
	frame = (Rs485Frame_S *)pvPortMalloc(sizeof(Rs485Frame_S));

	if(frame != NULL)
	{
		frame->device_type = RELAY;
		
		frame->len = PackBuiltOutRelayFrame(state.address,0xD0,NULL,0,outbuf);
		
		frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));
		
		if(frame->buf != NULL)
		{
			memcpy(frame->buf,outbuf,frame->len);
			
			xSemaphoreTake(xMutex_Rs485Rs485Frame, portMAX_DELAY);

			if(xQueueSend(xQueue_Rs485Rs485Frame,(void *)&frame,(TickType_t)10) != pdPASS)
			{
#ifdef DEBUG_LOG
				printf("relay send xQueue_Rs485Rs485Frame fail.\r\n");
#endif
				DeleteRs485Frame(frame);
			}
			
			xSemaphoreGive(xMutex_Rs485Rs485Frame);
		}
		else
		{
			DeleteRs485Frame(frame);
		}
	}	
}

//合并外部继电器模块报文
u16 PackBuiltOutRelayFrame(u8 address,u8 fun_code,u8 *inbuf,u16 inbuf_len,u8 *outbuf)
{
	u16 len = 0;
	u8 address_area[6] = {0x00,0x00,0x00,0x00,0x00,0x02};
	u8 imei[17] = {0x00};

	*(outbuf + 0) = 0x68;

	memcpy(outbuf + 1,address_area,6);

	*(outbuf + 7) = 0x68;
	*(outbuf + 8) = 0xFF;
	*(outbuf + 9) = 0xFF;

	imei[16] = address;
	memcpy(outbuf + 10,imei,17);

	*(outbuf + 27) = fun_code;
	
	*(outbuf + 28) = (u8)inbuf_len;
	
	memcpy(outbuf + 29,inbuf,inbuf_len);

	*(outbuf + 29 + inbuf_len + 0) = CalCheckSum(outbuf, 29 + inbuf_len);

	*(outbuf + 29 + inbuf_len + 1) = 0x16;

	*(outbuf + 29 + inbuf_len + 2) = 0xFE;
	*(outbuf + 29 + inbuf_len + 3) = 0xFD;
	*(outbuf + 29 + inbuf_len + 4) = 0xFC;
	*(outbuf + 29 + inbuf_len + 5) = 0xFB;
	*(outbuf + 29 + inbuf_len + 6) = 0xFA;
	*(outbuf + 29 + inbuf_len + 7) = 0xF9;

	len = 29 + inbuf_len + 7 + 1;

	return len;
}


void AnalysisBuiltOutRelayFrame(u8 *buf,u16 len,RelayModuleCollectState_S *collect_state)
{
	u16 pos1 = 0;
	u8 cmd_code = 0;
	u8 *data = NULL;
	u16 data_len = 0;
	u8 read_check_sum = 0;
	u8 cal_check_sum = 0;

	u8 buf_tail[6] = {0xFE,0xFD,0xFC,0xFB,0xFA,0xF9};

	pos1 = MyStrstr(buf,buf_tail,len,6);

	cmd_code = *(buf + 27);

	data_len = *(buf + 28);
	data = buf + 29;

	if(pos1 != 0xFFFF)
	{
		if(*(buf + 0) == 0x68 && 
		   *(buf + 7) == 0x68 && 
		   *(buf + pos1 - 1) == 0x16)
		{
			read_check_sum = *(buf + pos1 - 2);
			cal_check_sum = CalCheckSum(buf, pos1 - 2);
			
			if(read_check_sum == cal_check_sum)
			{
				collect_state->address = *(buf + 26);
				collect_state->channel = 1;
				
				switch(cmd_code)
				{
					case 0xD0:
						if(data_len == 7)
						{
							collect_state->loop_collect_state = (((u16)(*(data + 0))) << 8) + (u16)(*(data + 1));
							collect_state->update = 1;
						}
					break;

					case 0x80:

					break;

					default:

					break;
				}
			}
		}
	}
}
























































