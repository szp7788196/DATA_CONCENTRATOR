#include "lumeter.h"
#include "delay.h"






void GetBuiltOutLumeterState(LumeterConfig_S config)
{
	u8 i = 0;
	u8 module_type = 0;
	u8 outbuf[10] = {0};
	Rs485Frame_S *frame1 = NULL;

	frame1 = (Rs485Frame_S *)pvPortMalloc(sizeof(Rs485Frame_S));

	if(frame1 != NULL)
	{
		for(i = 0; i < MAX_LUMETER_CONF_NUM; i ++)
		{
			if(config.address == LumeterState[i].address &&
			   config.channel == LumeterState[i].channel)
			{
				if(LumeterState[i].waitting_resp == 1)
				{
					if(LumeterState[i].no_resp_time < config.no_response_time)
					{
						LumeterState[i].no_resp_time ++;
					}
				}
				else if(LumeterState[i].waitting_resp == 0)
				{
					LumeterState[i].waitting_resp = 1;
				}
			}
		}
		
		if(search_str((unsigned char *)config.module, "MT201") != -1)
		{
			module_type = 0;
		}
		else if(search_str((unsigned char *)config.module, "QY150A") != -1)
		{
			module_type = 2;
		}
		else if(search_str((unsigned char *)config.module, "QY150B") != -1)
		{
			module_type = 1;
		}

		frame1->device_type = LUMETER;

		frame1->len = PackBuiltOutLumeterFrame(config.address,module_type,outbuf);

		frame1->buf = (u8 *)pvPortMalloc(frame1->len * sizeof(u8));

		if(frame1->buf != NULL)
		{
			memcpy(frame1->buf,outbuf,frame1->len);

			xSemaphoreTake(xMutex_Rs485Rs485Frame, portMAX_DELAY);

			if(xQueueSend(xQueue_Rs485Rs485Frame,(void *)&frame1,(TickType_t)10) != pdPASS)
			{
#ifdef DEBUG_LOG
				printf("meter send xQueue_Rs485Rs485Frame fail.\r\n");
#endif
				DeleteRs485Frame(frame1);
			}

			xSemaphoreGive(xMutex_Rs485Rs485Frame);
		}
		else
		{
			DeleteRs485Frame(frame1);
		}
	}
}

u16 PackBuiltOutLumeterFrame(u8 address,u8 mode,u8 *outbuf)
{
	u16 len = 0;
	u16 crc16 = 0;

	if(mode == 0)
	{
		*(outbuf + 0) = 0xFF;
		*(outbuf + 1) = address;
		*(outbuf + 2) = 0xAC;
		*(outbuf + 3) = 0x00;
		*(outbuf + 4) = 0x00;
		*(outbuf + 5) = 0x00;
		*(outbuf + 6) = 0x04;
		
		len = 7;
	}
	else if(mode == 1)
	{
		*(outbuf + 0) = address;
		*(outbuf + 1) = 0x03;
		*(outbuf + 2) = 0x00;
		*(outbuf + 3) = 0x00;
		*(outbuf + 4) = 0x00;
		*(outbuf + 5) = 0x02;
		
		crc16 = CRC16(outbuf,6);

		*(outbuf + 6) = (u8)((crc16 >> 8) & 0x00FF);
		*(outbuf + 7) = (u8)(crc16 & 0x00FF);
		
		len = 8;
	}
	else if(mode == 2)
	{
		*(outbuf + 0) = address;
		*(outbuf + 1) = 0x03;
		*(outbuf + 2) = 0x00;
		*(outbuf + 3) = 0x00;
		*(outbuf + 4) = 0x00;
		*(outbuf + 5) = 0x01;
		
		crc16 = CRC16(outbuf,6);

		*(outbuf + 6) = (u8)((crc16 >> 8) & 0x00FF);
		*(outbuf + 7) = (u8)(crc16 & 0x00FF);
		
		len = 8;
	}

	return len;
}

u8 AnalysisBuiltOutLumeterFrame(u8 *buf,u16 len,LumeterCollectState_S *lumeter_state)
{
	u8 ret = 0;
	u8 *data = NULL;
	u16 data_len = 0;
	u16 crc16_read = 0;
	u16 crc16_cal = 0;
	u8 address = 0;
	u8 tmp[4] = {0};
	
	if(len >= 2)
	{
		crc16_read = ((((u16)(*(buf + len - 2))) << 8) & 0xFF00) + (((u16)(*(buf + len - 1))) & 0x00FF);

		crc16_cal = CRC16(buf,len - 2);

		if(crc16_read == crc16_cal && *(buf + 1) == 0x03)			//MODBUS协议
		{
			address = *(buf + 0);
			data_len = *(buf + 2);
			data = buf + 3;
			
			lumeter_state->address = address;
			lumeter_state->channel = 1;

			if(data_len == 4)
			{
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&lumeter_state->value,tmp,4);
				
				lumeter_state->update = 1;
				
				ret = 1;
			}
			else if(data_len == 2)
			{
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&lumeter_state->value,tmp,2);
				
				lumeter_state->value *= 100;
				
				lumeter_state->update = 1;
				
				ret = 1;
			}
		}
		else
		{
			if(len == 6)											//曼德克 MT201
			{
				if(*(buf + 1) == 0xBC && *(buf + 4) == 0x00 && *(buf + 5) == 0x04)
				{
					address = *(buf + 0);
					
					lumeter_state->address = address;
					lumeter_state->channel = 1;
					
					data = buf + 2;
					
					tmp[1] = *(data ++);
					tmp[0] = *(data ++);
					memcpy((void *)&lumeter_state->value,tmp,4);
					
					lumeter_state->update = 1;
					
					ret = 1;
				}
			}
		}
	}

	return ret;
}






















































