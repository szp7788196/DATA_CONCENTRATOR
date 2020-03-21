#include "server_protocol.h"
#include "task_handle_server_frame.h"
#include <string.h>
#include "common.h"



void ServerFrameHandle(ServerFrame_S *rx_frame)
{
	u8 i = 0;
	s8 ret = 0;
	DEVICE_TYPE_E device_type = CONCENTRATOR;
	QueueHandle_t xQueue_DeviceXxFrameStruct = NULL;

	ServerFrameStruct_S *server_frame_struct = NULL;

	server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(server_frame_struct != NULL)
	{
		ret = GetServerFrameStruct(server_frame_struct,rx_frame);

		if(ret != -1)		//初步解析成功，发送至各个任务进行进一步处理
		{
			device_type = (DEVICE_TYPE_E)(server_frame_struct->msg_type & 0xFF00);

			switch((u16)device_type)
			{
				case (u16)CONCENTRATOR:
					xQueue_DeviceXxFrameStruct = xQueue_ConcentratorFrameStruct;
				break;

				case (u16)LAMP_CONTROLLER:
					xQueue_DeviceXxFrameStruct = xQueue_LampControllerFrameStruct;
				break;

				case (u16)RELAY:
					xQueue_DeviceXxFrameStruct = xQueue_RelayFrameStruct;
				break;

				case (u16)INPUT_COLLECTOR:
					xQueue_DeviceXxFrameStruct = xQueue_InputCollectorFrameStruct;
				break;

				case (u16)ELECTRIC_METER:
					xQueue_DeviceXxFrameStruct = xQueue_ElectricMeterFrameStruct;
				break;

				case (u16)LUMETER:
					xQueue_DeviceXxFrameStruct = xQueue_LumeterFrameStruct;
				break;

				default:
				break;
			}

			if(xQueue_DeviceXxFrameStruct != NULL)
			{
				if(xQueueSend(xQueue_DeviceXxFrameStruct,(void *)&server_frame_struct,(TickType_t)10) != pdPASS)
				{
#ifdef DEBUG_LOG
					printf("send xQueue_DeviceXxFrameStruct fail.\r\n");
#endif
					for(i = 0; i < server_frame_struct->para_num; i ++)		//释放server_frame_struct缓存
					{
						vPortFree(server_frame_struct->para[i].value);
						server_frame_struct->para[i].value = NULL;
					}

					vPortFree(server_frame_struct);
					server_frame_struct = NULL;
				}
			}
		}
		else	//释放server_frame_struct缓存
		{
			vPortFree(server_frame_struct);
			server_frame_struct = NULL;
		}
	}
}

//获取帧数据中的参数个数和参数
u8 GetParameters(ServerFrameStruct_S *server_frame_struct,ServerFrame_S *rx_frame)
{
	u8 para_num = 0;
	u16 para_len;
	u16 pos = 27;
	u8 *msg = NULL;

	while(pos < rx_frame->len - 1)		//偏移指针不得超过帧长度
	{
		server_frame_struct->para[para_num].type = (((u16)rx_frame->buf[pos + 0] << 8) +
	                                                  (u16)rx_frame->buf[pos + 1]);

		pos += 2;

		msg = rx_frame->buf + pos;

		para_len = strlen((char *)msg);

		server_frame_struct->para[para_num].value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));

		if(server_frame_struct->para[para_num].value != NULL)
		{
			memcpy(server_frame_struct->para[para_num].value,msg,para_len + 1);
		}

		para_num ++;

		pos += (para_len + 1);
	}

	server_frame_struct->para_num = para_num;

	return para_num;
}

//拆分帧结构体
s8 GetServerFrameStruct(ServerFrameStruct_S *server_frame_struct,ServerFrame_S *rx_frame)
{
	u8 ret = 0;
	u32 crc32_cal = 0;

	if(rx_frame->len < 28)
	{
		return -1;
	}

	server_frame_struct->connection_mode 	= rx_frame->connection_mode;
	server_frame_struct->start 				= rx_frame->buf[0];
	server_frame_struct->msg_type 			= rx_frame->buf[1];
	server_frame_struct->serial_num 		= (((u32)rx_frame->buf[2] << 24) +
										      ((u32)rx_frame->buf[3] << 16) +
	                                          ((u32)rx_frame->buf[4] << 8) +
	                                          (u32)rx_frame->buf[5]);
	server_frame_struct->msg_len 			= (((u16)rx_frame->buf[6] << 8) +
	                                          (u16)rx_frame->buf[7]);
	server_frame_struct->err_code 			= rx_frame->buf[8];
	server_frame_struct->crc32 				= (((u32)rx_frame->buf[13] << 24) +
	                                          ((u32)rx_frame->buf[14] << 16) +
	                                          ((u32)rx_frame->buf[15] << 8) +
	                                          (u32)rx_frame->buf[16]);
	server_frame_struct->msg_id 			= (((u16)rx_frame->buf[17] << 8) +
	                                          (u16)rx_frame->buf[18]);
	server_frame_struct->gateway_id 		= (((u32)rx_frame->buf[19] << 24) +
	                                          ((u32)rx_frame->buf[20] << 16) +
											  ((u32)rx_frame->buf[21] << 8) +
											  (u32)rx_frame->buf[22]);
	server_frame_struct->device_id 			= (((u32)rx_frame->buf[23] << 24) +
                                              ((u32)rx_frame->buf[24] << 16) +
											  ((u32)rx_frame->buf[25] << 8) +
											  (u32)rx_frame->buf[26]);
	server_frame_struct->stop				= rx_frame->buf[rx_frame->len - 1];

	crc32_cal = CRC32((u8*)&rx_frame->buf[17],rx_frame->len - 18);

	if(crc32_cal == server_frame_struct->crc32 &&
	  (server_frame_struct->msg_type == (u8)SERVER_REQUEST_DOWN ||
	   server_frame_struct->msg_type == (u8)SERVER_RESPONSE_DOWN ||
	   server_frame_struct->msg_type == (u8)SYNC_RESPONSE))
	{
		ret = GetParameters(server_frame_struct,rx_frame);
	}

	return ret;
}

//将帧结构体转换为数据帧
u8 ConvertFrameStructToFrame(ServerFrameStruct_S *server_frame_struct)
{
	
	if(server_frame_struct == NULL)
	{
		return 0;
	}
	
	
	
	
}

//复制帧结构体 s原件 d副本
//mode 0不复制参数 1复制整个结构体
u8 CopyServerFrameStruct(ServerFrameStruct_S *s_server_frame_struct,ServerFrameStruct_S *d_server_frame_struct,u8 mode)
{
	u8 ret = 1;
	u8 i = 0;
	u16 para_len;

	if(s_server_frame_struct == NULL || d_server_frame_struct == NULL)
	{
		return 0;
	}
	
	d_server_frame_struct->connection_mode 	= s_server_frame_struct->connection_mode;
	d_server_frame_struct->start 			= s_server_frame_struct->start;
	d_server_frame_struct->msg_type 		= s_server_frame_struct->msg_type;
	d_server_frame_struct->serial_num 		= s_server_frame_struct->msg_type;
	d_server_frame_struct->msg_len 			= s_server_frame_struct->msg_len;
	d_server_frame_struct->err_code 		= s_server_frame_struct->err_code;
	d_server_frame_struct->crc32 			= s_server_frame_struct->crc32;
	d_server_frame_struct->msg_id 			= s_server_frame_struct->msg_id;
	d_server_frame_struct->gateway_id 		= s_server_frame_struct->gateway_id;
	d_server_frame_struct->device_id 		= s_server_frame_struct->device_id;
	d_server_frame_struct->para_num			= s_server_frame_struct->para_num;
	d_server_frame_struct->stop				= s_server_frame_struct->stop;
	
	
	if(mode == 1)
	{
		for(i = 0; i < d_server_frame_struct->para_num; i ++)
		{
			d_server_frame_struct->para[i].type = s_server_frame_struct->para[i].type;
			
			para_len = strlen((char *)s_server_frame_struct->para[i].value);
			
			d_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
			
			if(d_server_frame_struct->para[i].value != NULL)
			{
				memcpy(d_server_frame_struct->para[i].value,s_server_frame_struct->para[i].value,para_len + 1);
			}
		}
	}

	return ret;
}


/*
功能：去掉转义字符
参数: inbuf源缓存区，inbuf_len源数据长度，outbuf转换后缓存
0x1B 0xE7转成0x02
0x1B 0xE8转成0x03
0x1B 0x00转成0x1B
返回：返回转换后数据长度
*/
u16 EscapeSymbolDelete(u8* inbuf,u16 inbuf_len,u8* outbuf)
{
	u16 out_len = 0, i = 0, j = 1;

	out_len = inbuf_len;

	if((inbuf[0] == 0x02) && (inbuf[inbuf_len - 1]==0x03))
	{
		outbuf[0] = 0x02;

		for(i = 1; i < inbuf_len - 1; i++)
		{
			if((inbuf[i] == 0x1B) && (inbuf[i + 1] == 0xE7))
			{
				outbuf[j] = 0x02;

				out_len --;

				i ++;
			}
			else if((inbuf[i] == 0x1B) && (inbuf[i + 1] == 0xE8))
			{
				outbuf[j] = 0x03;

				out_len --;

				i ++;
			}
			else if((inbuf[i] == 0x1B) && (inbuf[i + 1] == 0x00))
			{
				outbuf[j] = 0x1B;

				out_len --;

				i ++;
			}
			else
			{
				outbuf[j] = inbuf[i];
			}

			j++;
		}

		outbuf[out_len - 1] = 0x03;

		return out_len;
	}
	else
	{
		return 0;
	}
}

//添加转义字符
u16 EscapeSymbolAdd(u8* inbuf,u16 inbuf_len,u8* outbuf)
{
	u16 out_len = 0, i = 0, j = 1;

	out_len = inbuf_len;

	for(i = 1; i < inbuf_len - 1; i ++)
	{
		if(inbuf[i] == 0x02)
		{
			outbuf[j] = 0x1B;
			outbuf[++ j] = 0xE7;

			out_len ++;
		}
		else if(inbuf[i] == 0x03)
		{
			outbuf[j] = 0x1B;
			outbuf[++ j] = 0xE8;

			out_len ++;
		}
		else if(inbuf[i] == 0x1B)
		{
			outbuf[j] = 0x1B;
			outbuf[++ j] = 0;

			out_len ++;
		}
		else
		{
			outbuf[j] = inbuf[i];
		}

		j++;
	}

	outbuf[0] = 0x02;
	outbuf[out_len-1] = 0x03;

	return out_len;
}

































