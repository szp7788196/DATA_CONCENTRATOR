#include "server_protocol.h"
#include "task_handle_server_frame.h"
#include <string.h>
#include "common.h"
#include "ec20.h"



void ServerFrameHandle(ServerFrame_S *rx_frame)
{
	s8 ret = 0;
	
	ServerFrameStruct_S *server_frame_struct = NULL;

	server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(server_frame_struct != NULL)
	{
		ret = GetServerFrameStruct(server_frame_struct,rx_frame);

		if(ret >= 0)		//初步解析成功，发送至各个任务进行进一步处理
		{
			TransServerFrameStructToOtherTask(server_frame_struct,UNKNOW_DEVICE);
		}
		else	//释放server_frame_struct缓存
		{
			DeleteServerFrameStruct(server_frame_struct);
		}
	}
}

u8 TransServerFrameStructToOtherTask(ServerFrameStruct_S *server_frame_struct,DEVICE_TYPE_E device_type)
{
	u8 ret = 1;
	QueueHandle_t xQueue_DeviceXxFrameStruct = NULL;
	DEVICE_TYPE_E _device_type = CONCENTRATOR;
	
	if(device_type == UNKNOW_DEVICE)
	{
		_device_type = (DEVICE_TYPE_E)(server_frame_struct->msg_type & 0xFF00);
	}
	
	switch((u16)_device_type)
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
			DeleteServerFrameStruct(server_frame_struct);
		break;
	}

	if(xQueue_DeviceXxFrameStruct != NULL)
	{
		if(xQueueSend(xQueue_DeviceXxFrameStruct,(void *)&server_frame_struct,(TickType_t)10) != pdPASS)
		{
#ifdef DEBUG_LOG
			printf("send xQueue_DeviceXxFrameStruct fail.\r\n");
#endif
			DeleteServerFrameStruct(server_frame_struct);
			
			ret = 0;
		}
	}
	
	return ret;
}

u8 GetParameterNum(ServerFrame_S *rx_frame)
{
	u16 i = 0;
	
	u8 cnt = 0;//参数个数
	
	for(i = 27; i < rx_frame->len; i ++)//29为第一个参数值起始位置
	{
		if(rx_frame->buf[i] == 0)
		{
			cnt ++;
		}
	}
	
	return cnt;
}

//获取帧数据中的参数个数和参数
u8 GetParameters(ServerFrameStruct_S *server_frame_struct,ServerFrame_S *rx_frame)
{
	u8 i = 0;
	u8 para_num = 0;
	u16 pos = 27;
	u8 *msg = NULL;
	
	para_num = GetParameterNum(rx_frame);
	
	server_frame_struct->para_num = para_num;
	
	server_frame_struct->para = (Parameter_S *)pvPortMalloc(para_num * sizeof(Parameter_S));
	
	for(i = 0; i < para_num; i ++)
	{
		if(pos < rx_frame->len - 1)		//偏移指针不得超过帧长度
		{
			server_frame_struct->para[i].type = (((u16)rx_frame->buf[pos + 0] << 8) +
												(u16)rx_frame->buf[pos + 1]);

			pos += 2;

			msg = rx_frame->buf + pos;

			server_frame_struct->para[i].len = strlen((char *)msg);

			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));

			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,msg,server_frame_struct->para[i].len + 1);
			}

			pos += (server_frame_struct->para[i].len + 1);
		}
	}

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

	if(crc32_cal == server_frame_struct->crc32)
	{
		if(server_frame_struct->msg_type == (u8)SERVER_REQUEST_DOWN ||
	       server_frame_struct->msg_type == (u8)SERVER_RESPONSE_DOWN ||
	       server_frame_struct->msg_type == (u8)SYNC_RESPONSE)
		{
			ret = GetParameters(server_frame_struct,rx_frame);
		}
	}
	else
	{
		return -2;
	}

	return ret;
}

//将帧结构体转换为数据帧
u8 ConvertFrameStructToFrame(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u16 origin_frame_len = 28;
	u16 final_frame_len = 28;
	u16 offset = 27;
	u32 crc32_cal = 0;
	u8 frame_num = 0;
	u8 *origin_buf = NULL;
	u8 *final_buf = NULL;

	if(server_frame_struct == NULL)
	{
		return 0;
	}

	for(i = 0; i < server_frame_struct->para_num; i ++)
	{
		origin_frame_len += 3;

		origin_frame_len += server_frame_struct->para[i].len;
	}

	origin_buf = (u8 *)pvPortMalloc(origin_frame_len * sizeof(u8));

	if(origin_buf != NULL)
	{
		*(origin_buf + 0)  = server_frame_struct->start;										//帧起始符
		*(origin_buf + 1)  = server_frame_struct->msg_type;

		*(origin_buf + 2)  = (u8)((server_frame_struct->serial_num >> 24) & 0x000000FF);		//帧流水号
		*(origin_buf + 3)  = (u8)((server_frame_struct->serial_num >> 16) & 0x000000FF);
		*(origin_buf + 4)  = (u8)((server_frame_struct->serial_num >>  8) & 0x000000FF);
		*(origin_buf + 5)  = (u8)((server_frame_struct->serial_num >>  0) & 0x000000FF);

		*(origin_buf + 6)  = (u8)(((origin_frame_len - 18) >> 8) & 0x00FF);						//消息体长度
		*(origin_buf + 7)  = (u8)(((origin_frame_len - 18) >> 0) & 0x00FF);

		*(origin_buf + 8)  = server_frame_struct->err_code;										//错误码

		*(origin_buf + 9)  = 0;																	//保留位
		*(origin_buf + 10) = 0;
		*(origin_buf + 11) = 0;
		*(origin_buf + 12) = 0;

		*(origin_buf + 13) = 0;																	//CRC32校验
		*(origin_buf + 14) = 0;
		*(origin_buf + 15) = 0;
		*(origin_buf + 16) = 0;

		*(origin_buf + 17) = (u8)((server_frame_struct->msg_id >> 8) & 0x00FF);					//消息ID
		*(origin_buf + 18) = (u8)((server_frame_struct->msg_id >> 0) & 0x00FF);

		*(origin_buf + 19) = (u8)((server_frame_struct->gateway_id >> 24) & 0x000000FF);		//网关ID
		*(origin_buf + 20) = (u8)((server_frame_struct->gateway_id >> 16) & 0x000000FF);
		*(origin_buf + 21) = (u8)((server_frame_struct->gateway_id >>  8) & 0x000000FF);
		*(origin_buf + 22) = (u8)((server_frame_struct->gateway_id >>  0) & 0x000000FF);

		*(origin_buf + 23) = (u8)((server_frame_struct->device_id >> 24) & 0x000000FF);			//设备ID
		*(origin_buf + 24) = (u8)((server_frame_struct->device_id >> 16) & 0x000000FF);
		*(origin_buf + 25) = (u8)((server_frame_struct->device_id >>  8) & 0x000000FF);
		*(origin_buf + 26) = (u8)((server_frame_struct->device_id >>  0) & 0x000000FF);

		*(origin_buf + origin_frame_len - 1) = server_frame_struct->stop;						//帧结束符

		for(i = 0; i < server_frame_struct->para_num; i ++)
		{
			*(origin_buf + offset + 0) = ((server_frame_struct->para[i].type >> 8) & 0x00FF);	//参数类型
			*(origin_buf + offset + 1) = ((server_frame_struct->para[i].type >> 0) & 0x00FF);

			offset += 2;

			memcpy(origin_buf + offset,server_frame_struct->para[i].value,server_frame_struct->para[i].len + 1);

			offset += (server_frame_struct->para[i].len + 1);
		}

		crc32_cal = CRC32(origin_buf + 17,origin_frame_len - 18);

		*(origin_buf + 13) = (u8)((crc32_cal >> 24) & 0x000000FF);								//CRC32校验
		*(origin_buf + 14) = (u8)((crc32_cal >> 16) & 0x000000FF);
		*(origin_buf + 15) = (u8)((crc32_cal >>  8) & 0x000000FF);
		*(origin_buf + 16) = (u8)((crc32_cal >>  0) & 0x000000FF);

		final_frame_len = GetFinalFrameLen(origin_buf,origin_frame_len);

		final_buf = (u8 *)pvPortMalloc(final_frame_len * sizeof(u8));

		if(final_buf != NULL)
		{
			final_frame_len = EscapeSymbolAdd(origin_buf,origin_frame_len,final_buf);

			frame_num = final_frame_len / EC20_MAX_RECV_SEND_BUF_LEN;

			for(i = 0; i < frame_num + 1; i ++)
			{
				ServerFrame_S *tx_frame = NULL;

				tx_frame = (ServerFrame_S *)pvPortMalloc(sizeof(ServerFrame_S));

				if(tx_frame != NULL)
				{
					tx_frame->connection_mode = server_frame_struct->connection_mode;

					if(i == frame_num)
					{
						tx_frame->len = final_frame_len % EC20_MAX_RECV_SEND_BUF_LEN;
					}
					else
					{
						tx_frame->len = EC20_MAX_RECV_SEND_BUF_LEN;
					}

					tx_frame->buf = (u8 *)pvPortMalloc(tx_frame->len * sizeof(u8));

					if(tx_frame->buf != NULL)
					{
						memcpy(tx_frame->buf,&final_buf[i * EC20_MAX_RECV_SEND_BUF_LEN],tx_frame->len);

						PushTheFrameToTxQueue(tx_frame);
						
						if(frame_num > 1)		//多帧数据之间需要加延时
						{
							delay_ms(1000);
						}

						ret ++;
					}
					else
					{
						DeleteServerFrame(tx_frame);
					}
				}
			}

			vPortFree(final_buf);
			final_buf = NULL;
		}

		vPortFree(origin_buf);
		origin_buf = NULL;
	}

	DeleteServerFrameStruct(server_frame_struct);

	return ret;
}

//复制帧结构体 s原件 d副本
//mode 0不复制参数 1复制整个结构体
u8 CopyServerFrameStruct(ServerFrameStruct_S *s_server_frame_struct,ServerFrameStruct_S *d_server_frame_struct,u8 mode)
{
	u8 ret = 1;
	u8 i = 0;

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
		d_server_frame_struct->para_num = s_server_frame_struct->para_num;
		
		d_server_frame_struct->para = (Parameter_S *)pvPortMalloc(d_server_frame_struct->para_num * sizeof(Parameter_S));

		if(d_server_frame_struct->para != NULL)
		{
			for(i = 0; i < d_server_frame_struct->para_num; i ++)
			{
				d_server_frame_struct->para[i].type = s_server_frame_struct->para[i].type;

				d_server_frame_struct->para[i].len = s_server_frame_struct->para[i].len;

				d_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((d_server_frame_struct->para[i].len + 1) * sizeof(u8));

				if(d_server_frame_struct->para[i].value != NULL)
				{
					memcpy(d_server_frame_struct->para[i].value,s_server_frame_struct->para[i].value,d_server_frame_struct->para[i].len + 1);
				}
			}
		}
		else
		{
			d_server_frame_struct->para_num = 0;
		}
	}
	else
	{
		d_server_frame_struct->para_num = 0;
	}

	return ret;
}


//获取原始帧转义后的帧长度
u16 GetFinalFrameLen(u8 *buf,u16 len)
{
	u16 i = 0;
	u16 final_len = 0;

	for(i = 0; i < len; i ++)
	{
		if(*(buf + i) == 0x02 || *(buf + i) == 0x03 || *(buf + i) == 0x1B)
		{
			final_len += 1;
		}
	}

	final_len += len;

	return final_len;
}

//释放ServerFrameStruct结构体申请的内存
void DeleteServerFrameStruct(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	
	if(server_frame_struct != NULL)
	{
		if(server_frame_struct->para != NULL)
		{
			if(server_frame_struct->para_num != 0)
			{
				for(i = 0; i < server_frame_struct->para_num; i ++)		//释放server_frame_struct缓存
				{
					vPortFree(server_frame_struct->para[i].value);
					server_frame_struct->para[i].value = NULL;
				}
			}

			vPortFree(server_frame_struct->para);
			server_frame_struct->para = NULL;
		}

		vPortFree(server_frame_struct);
		server_frame_struct = NULL;
	}
}

//释放ServerFrame结构体申请的内存
void DeleteServerFrame(ServerFrame_S *server_frame)
{
	if(server_frame != NULL)
	{
		if(server_frame->buf != NULL)
		{
			vPortFree(server_frame->buf);
			server_frame->buf = NULL;
		}
		
		vPortFree(server_frame);
		server_frame = NULL;
	}
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
			outbuf[++ j] = 0x00;

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

































