#include "kr.h"
#include "task_rs485.h"

u8 BuiltInInputCollectorState = 0;

void KR_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);	//使能GPIOF时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |
	                              GPIO_Pin_5 | GPIO_Pin_6 |
	                              GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
}


void GetKrState(void)
{
	if(KR1 == 1)
	{
		BuiltInInputCollectorState |= (1 << 0);
	}
	else
	{
		BuiltInInputCollectorState &= ~(1 << 0);
	}

	if(KR2 == 1)
	{
		BuiltInInputCollectorState |= (1 << 1);
	}
	else
	{
		BuiltInInputCollectorState &= ~(1 << 1);
	}

	if(KR3 == 1)
	{
		BuiltInInputCollectorState |= (1 << 2);
	}
	else
	{
		BuiltInInputCollectorState &= ~(1 << 2);
	}

	if(KR4 == 1)
	{
		BuiltInInputCollectorState |= (1 << 3);
	}
	else
	{
		BuiltInInputCollectorState &= ~(1 << 3);
	}

	if(KR5 == 1)
	{
		BuiltInInputCollectorState |= (1 << 4);
	}
	else
	{
		BuiltInInputCollectorState &= ~(1 << 4);
	}

	if(KR6 == 1)
	{
		BuiltInInputCollectorState |= (1 << 5);
	}
	else
	{
		BuiltInInputCollectorState &= ~(1 << 5);
	}
}

//控制集控内部继电器
void GetAllBuiltInInputCollectorState(InputCollectorState_S *state)
{
	state->d_current_state = BuiltInInputCollectorState;
}

//采集集控外部继电器模块状态
void GetBuiltOutInputCollectorState(InputCollectorState_S state)
{
	u8 outbuf[48] = {0};
	Rs485Frame_S *frame = NULL;

	frame = (Rs485Frame_S *)pvPortMalloc(sizeof(Rs485Frame_S));

	if(frame != NULL)
	{
		frame->device_type = INPUT_COLLECTOR;

		frame->len = PackBuiltOutInputCollectorFrame(state.address,0xE0,NULL,0,outbuf);

		frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));

		if(frame->buf != NULL)
		{
			memcpy(frame->buf,outbuf,frame->len);

			xSemaphoreTake(xMutex_Rs485Rs485Frame, portMAX_DELAY);

			if(xQueueSend(xQueue_Rs485Rs485Frame,(void *)&frame,(TickType_t)10) != pdPASS)
			{
#ifdef DEBUG_LOG
				printf("send xQueue_Rs485Rs485Frame fail.\r\n");
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

//合并外部输入量检测模块报文
u16 PackBuiltOutInputCollectorFrame(u8 address,u8 fun_code,u8 *inbuf,u16 inbuf_len,u8 *outbuf)
{
	u16 len = 0;
	u8 address_area[6] = {0x00,0x00,0x00,0x00,0x00,0x06};
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

void AnalysisBuiltOutInputCollectorFrame(u8 *buf,u16 len,InputCollectorCollectState_S *collect_state)
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
					case 0xE0:
						if(data_len == 21) //数字量2  模拟量16 时间戳3
						{
							collect_state->d_collect_state = (((u16)(*(data + 0))) << 8) + (u16)(*(data + 1));
							memcpy((void *)&collect_state->a_collect_state[0],data + 2,8);
							memcpy((void *)&collect_state->a_collect_state[1],data + 10,8);
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


























