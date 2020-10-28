#include "lumeter_comm.h"
#include "concentrator_comm.h"
#include "common.h"
#include "task_rs485.h"


u8 LumeterStateChangesReportResponse = 0;

//发送状态变化事件
void LumeterSendStateChangesReportFrameToServer(void)
{
	u8 i = 0;
	u8 j = 0;
	char *buf = NULL;
	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

	buf = (char *)pvPortMalloc(25 * sizeof(char));
	
	if(buf != NULL)
	{
		server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

		if(server_frame_struct != NULL)
		{
			InitServerFrameStruct(server_frame_struct);

			server_frame_struct->msg_type 	= (u8)DEVICE_REQUEST_UP;	//响应服务器类型
			server_frame_struct->msg_len 	= 10;
			server_frame_struct->err_code 	= (u8)NO_ERR;
			server_frame_struct->msg_id		= 0x0571;
			server_frame_struct->para_num	= LumeterConfigNum.number * 2;

			server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

			if(server_frame_struct->para != NULL)
			{
				for(j = 0; j < LumeterConfigNum.number; j ++)
				{
					server_frame_struct->para[i].type = 0x3001 + i;
					memset(buf,0,25);
					sprintf(buf, "%X",LumeterState[j].address);
					server_frame_struct->para[i].len = strlen(buf);
					server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
					if(server_frame_struct->para[i].value != NULL)
					{
						memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
					}
					i ++;

					server_frame_struct->para[i].type = 0x4001 + i;
					memset(buf,0,25);
					sprintf(buf, "%d",LumeterState[j].value);
					server_frame_struct->para[i].len = strlen(buf);
					server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
					if(server_frame_struct->para[i].value != NULL)
					{
						memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
					}
					i ++;
				}
				
				ConvertFrameStructToFrame(server_frame_struct);
			}
			else
			{
				DeleteServerFrameStruct(server_frame_struct);
			}
		}

		vPortFree(buf);
	}
}

//向服务器发送状态变化事件
void LumeterSendStateChangesReportToServer(void)
{
	static time_t time_4 = 0;
	static time_t time_5 = 0;
	static u8 retry_times5 = 0;

	if(GetSysTick1s() -  time_4 >= LumeterBasicConfig.auto_report_cycle)
	{
		LumeterStateChangesReportResponse = 1;

		time_4 = GetSysTick1s();
		time_5 = GetSysTick1s();
		retry_times5 = 0;

		RE_SEND_STATE_CHANGES_REPORT:
		LumeterSendStateChangesReportFrameToServer();
	}

	if(LumeterStateChangesReportResponse == 1)
	{
		if(GetSysTick1s() - time_5 >= ConcentratorBasicConfig.command_response_timeout)
		{
			time_5 = GetSysTick1s();

			if((retry_times5 ++) < ConcentratorBasicConfig.command_retransmission_times)
			{
				goto RE_SEND_STATE_CHANGES_REPORT;
			}
			else
			{
				retry_times5 = 0;

				LumeterStateChangesReportResponse = 0;
			}
		}
	}
}

//处理接收到的报文
void LumeterRecvAndHandleFrameStruct(void)
{
	BaseType_t xResult;
	ServerFrameStruct_S *server_frame_struct = NULL;

	xResult = xQueueReceive(xQueue_LumeterFrameStruct,(void *)&server_frame_struct,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS )
	{
		switch(server_frame_struct->msg_id)
		{
			case 0x0500:	//数据透传
				LumeterTransparentTransmission(server_frame_struct);
			break;
			
			case 0x0501:	//照度计数据
				LumeterSetILluminanceValue(server_frame_struct);
			break;

			case 0x0570:	//状态查询
				LumeterGetCurrentState(server_frame_struct);
			break;

			case 0x0571:	//状态上报
				LumeterStateChangesReportResponse = 0;
			break;

			case 0x05A0:	//告警上报
				AlarmReportResponse = 0;
			break;

			case 0x05A1:	//告警解除
				AlarmReportResponse = 0;
			break;

			case 0x05A2:	//事件上报

			break;

			case 0x05A3:	//告警配置
				LumeterSetAlarmConfiguration(server_frame_struct);
			break;

			case 0x05A4:	//告警配置查询
				LumeterGetAlarmConfiguration(server_frame_struct);
			break;

			case 0x05A5:	//告警历史查询
				LumeterGetAlarmReportHistory(server_frame_struct);
			break;

			case 0x05D0:	//基础配置
				LumeterSetBasicConfiguration(server_frame_struct);
			break;

			case 0x05D1:	//查询基础配置
				LumeterGetBasicConfiguration(server_frame_struct);
			break;

			default:
			break;
		}

		DeleteServerFrameStruct(server_frame_struct);
	}
}

//透传指令
u8 LumeterTransparentTransmission(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 j = 0;
	Rs485Frame_S *frame = NULL;
	
	if(server_frame_struct->para_num  == 5)				//参数个数是4的倍数
	{
		for(j = 0; j < server_frame_struct->para_num; j ++)
		{
			if(server_frame_struct->para[j].type == 0x3005)
			{
				if(server_frame_struct->para[j].len % 2 == 0)
				{
					frame = (Rs485Frame_S *)pvPortMalloc(sizeof(Rs485Frame_S));
					
					if(frame != NULL)
					{
						frame->device_type = LUMETER;
						
						frame->len = server_frame_struct->para[j].len / 2;
						
						frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));
						
						if(frame->buf != NULL)
						{
							StrToHex(frame->buf, (char *)server_frame_struct->para[j].value, frame->len);
							
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
			}
		}
	}

	return ret;
}

//设置照度值
u8 LumeterSetILluminanceValue(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;

	u8 j = 0;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	for(j = 0; j < server_frame_struct->para_num; j ++)
	{
		switch(server_frame_struct->para[j].type)
		{
			case 0x7001:
				LumeterValueFromServer = myatoi((char *)server_frame_struct->para[j].value);
			break;

			default:
			break;
		}
	}

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

//状态查询
u8 LumeterGetCurrentState(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 n = 0;
	u8 ret = 0;
	u8 para_num = 0;
	u8 module_num = 0;
	u8 add = 0;
	u8 ch = 0;
	char tmp[25] = {0};
	char *buf = NULL;
	char *msg = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器
	ServerFrameStruct_S *state_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(server_frame_struct->para_num % 2 == 0)
	{
		if(resp_server_frame_struct != NULL)				//参数个数是2的倍数
		{
			CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

			resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
			resp_server_frame_struct->msg_len 	= 10;
			resp_server_frame_struct->err_code 	= (u8)NO_ERR;

			ret = ConvertFrameStructToFrame(resp_server_frame_struct);

			buf = (char *)pvPortMalloc(25 * sizeof(char));
			
			if(buf != NULL)
			{
				module_num = server_frame_struct->para_num / 2;

				for(k = 0; k < module_num; k ++)
				{
					for(j = 0; j < LumeterConfigNum.number; j ++)
					{
						n = 0;
						
						msg = (char *)server_frame_struct->para[k * 2 + 0].value;
						
						while(*msg != '\0')
						tmp[n ++] = *(msg ++);
						tmp[n] = '\0';
						msg = msg + 1;
						if(n == 1)
						{
							tmp[1] = tmp[0];
							tmp[0] = '0';
						}
						StrToHex(&add,tmp,1);

						ch = myatoi((char *)server_frame_struct->para[k * 2 + 1].value);

						if(add == LumeterState[j].address && ch == LumeterState[j].channel)
						{
							para_num += 2;
						}
					}
				}
				
				if(para_num != 0)
				{
					state_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));
					
					if(state_server_frame_struct != NULL)
					{
						CopyServerFrameStruct(server_frame_struct,state_server_frame_struct,0);

						state_server_frame_struct->msg_type = (u8)SYNC_RESPONSE;	//响应服务器类型
						state_server_frame_struct->msg_len 	= 10;
						state_server_frame_struct->err_code = (u8)NO_ERR;

						state_server_frame_struct->para_num = 4;

						state_server_frame_struct->para = (Parameter_S *)pvPortMalloc(state_server_frame_struct->para_num * sizeof(Parameter_S));
						
						if(state_server_frame_struct->para != NULL)
						{
							for(k = 0; k < module_num; k ++)
							{
								for(j = 0; j < LumeterConfigNum.number; j ++)
								{
									n = 0;
									
									msg = (char *)server_frame_struct->para[k * 2 + 0].value;
									
									while(*msg != '\0')
									tmp[n ++] = *(msg ++);
									tmp[n] = '\0';
									msg = msg + 1;
									if(n == 1)
									{
										tmp[1] = tmp[0];
										tmp[0] = '0';
									}
									StrToHex(&add,tmp,1);

									ch = myatoi((char *)server_frame_struct->para[k * 2 + 1].value);

									if(add == LumeterState[j].address && ch == LumeterState[j].channel)
									{
										state_server_frame_struct->para[i].type = 0x3001 + i;
										memset(buf,0,25);
										sprintf(buf, "%X",LumeterState[j].address);
										state_server_frame_struct->para[i].len = strlen(buf);
										state_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
										if(state_server_frame_struct->para[i].value != NULL)
										{
											memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
										}
										i ++;

										state_server_frame_struct->para[i].type = 0x4001 + i;
										memset(buf,0,25);
										sprintf(buf, "%d",LumeterState[j].value);
										state_server_frame_struct->para[i].len = strlen(buf);
										state_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
										if(state_server_frame_struct->para[i].value != NULL)
										{
											memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
										}
										i ++;
									}
								}
							}
							
							ret = ConvertFrameStructToFrame(state_server_frame_struct);
						}
						else
						{
							DeleteServerFrameStruct(state_server_frame_struct);
						}
					}
				}

				vPortFree(buf);
			}
		}
	}

	return ret;
}

//设置告警配置参数
u8 LumeterSetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 m = 0;
	u8 n = 0;
	u8 add = 0;
	u8 ch = 0;
	u8 seg_num = 0;
	char tmp[10];
	char *msg = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	for(j = 0; j < server_frame_struct->para_num; j ++)
	{
		switch(server_frame_struct->para[j].type)
		{
			case 0x6001:
				LumeterAlarmConfig.lumeter_thre_over_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
			break;

			case 0x4002:
				msg = (char *)server_frame_struct->para[j].value;

				if(server_frame_struct->para[j].len != 0)
				{
					seg_num = 0;

					while(*msg != '\0')
					{
						if(*(msg ++) == '|')
						{
							seg_num ++;
						}
					}

					seg_num ++;

					msg = (char *)server_frame_struct->para[j].value;

					i = 0;
					
					for(n = 0; n < seg_num; n ++)
					{
						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						msg = msg + 1;
						if(i == 1)
						{
							tmp[1] = tmp[0];
							tmp[0] = '0';
						}
						StrToHex(&add,tmp,1);
						i = 0;

						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						ch = myatoi(tmp);

						for(m = 0; m < LumeterConfigNum.number; m ++)
						{
							if(add == LumeterConfig[m].address &&
							   ch == LumeterConfig[m].channel)
							{
								break;
							}
						}
						
						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						if(tmp[0] == 'N' && tmp[1] == 'A')
						{
							memset((void *)&LumeterConfig[m].min_valid_value,0xFF,4);
						}
						else
						{
							LumeterConfig[m].min_valid_value = myatoi(tmp);
						}

						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						LumeterConfig[m].min_valid_value_range = myatoi(tmp);

						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						if(tmp[0] == 'N' && tmp[1] == 'A')
						{
							memset((void *)&LumeterConfig[m].max_valid_value,0xFF,4);
						}
						else
						{
							LumeterConfig[m].max_valid_value = myatoi(tmp);
						}

						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						LumeterConfig[m].max_valid_value_range = myatoi(tmp);
						
						while(*msg != ',' && *msg != '|' && *msg != '\0')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						LumeterConfig[m].valid_value_confirm_time = myatoi(tmp);
					}

					for(k = 0; k < LumeterConfigNum.number; k ++)
					{
						WriteLumeterConfig(k,0,1);
					}
				}
			break;

			case 0x6003:
				LumeterAlarmConfig.lumeter_abnormal_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
			break;

			case 0x4004:
				msg = (char *)server_frame_struct->para[j].value;

				if(server_frame_struct->para[j].len != 0)
				{
					seg_num = 0;

					while(*msg != '\0')
					{
						if(*(msg ++) == '|')
						{
							seg_num ++;
						}
					}

					seg_num ++;

					msg = (char *)server_frame_struct->para[j].value;

					i = 0;
					
					for(n = 0; n < seg_num; n ++)
					{
						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						msg = msg + 1;
						if(i == 1)
						{
							tmp[1] = tmp[0];
							tmp[0] = '0';
						}
						StrToHex(&add,tmp,1);
						i = 0;

						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						ch = myatoi(tmp);

						for(m = 0; m < LumeterConfigNum.number; m ++)
						{
							if(add == LumeterConfig[m].address &&
							   ch == LumeterConfig[m].channel)
							{
								break;
							}
						}

						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						LumeterConfig[m].value_unchanged_range = myatoi(tmp);
						
						while(*msg != ',' && *msg != '|' && *msg != '\0')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						LumeterConfig[m].no_response_time = myatoi(tmp);
					}

					for(k = 0; k < LumeterConfigNum.number; k ++)
					{
						WriteLumeterConfig(k,0,1);
					}
				}
			break;

			default:
			break;
		}
	}
	
	WriteLumeterAlarmConfig(0,1);

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

//获取告警配置参数
u8 LumeterGetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 m = 0;
	u8 ret = 0;
	u16 str_len = 0;
	long long thre = {0};
	char tmp[25] = {0};
	char *buf = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		buf = (char *)pvPortMalloc(64 * sizeof(char));
		
		if(buf != NULL)
		{
			CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

			resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
			resp_server_frame_struct->msg_len 	= 10;
			resp_server_frame_struct->err_code 	= (u8)NO_ERR;
			resp_server_frame_struct->para_num 	= 4;

			resp_server_frame_struct->para = (Parameter_S *)pvPortMalloc(resp_server_frame_struct->para_num * sizeof(Parameter_S));

			if(resp_server_frame_struct->para != NULL)
			{
				resp_server_frame_struct->para[i].type = 0x6101;
				memset(buf,0,2);
				sprintf(buf, "%d",LumeterAlarmConfig.lumeter_thre_over_alarm_enable);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x4102;
				memset(buf,0,64);
				memset(tmp,0,10);
				
				for(m = 0; m < LumeterConfigNum.number; m ++)
				{
					memset(tmp,0,10);
					sprintf(tmp, "%X",LumeterConfig[m].address);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",LumeterConfig[m].channel);
					strcat(buf,tmp);
					strcat(buf,",");
					
					memset(tmp,0,25);
					memcpy(&thre,(void *)&LumeterConfig[m].min_valid_value,4);
					if(thre == 0xFFFFFFFF)
					{
						strcat(buf,"NA,");
					}
					else
					{
						sprintf(tmp, "%d",LumeterConfig[m].min_valid_value);
						strcat(buf,tmp);
						strcat(buf,",");
					}

					memset(tmp,0,10);
					sprintf(tmp, "%d",LumeterConfig[m].min_valid_value_range);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,25);
					memcpy(&thre,(void *)&LumeterConfig[m].max_valid_value,4);
					if(thre == 0xFFFFFFFF)
					{
						strcat(buf,"NA,");
					}
					else
					{
						sprintf(tmp, "%d",LumeterConfig[m].max_valid_value);
						strcat(buf,tmp);
						strcat(buf,",");
					}
					
					memset(tmp,0,10);
					sprintf(tmp, "%d",LumeterConfig[m].max_valid_value_range);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",LumeterConfig[m].valid_value_confirm_time);
					strcat(buf,tmp);
					strcat(buf,"|");
				}

				str_len = strlen(buf);
				if(str_len != 0)
				{
					buf[str_len - 1] = 0;
				}
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x6103;
				memset(buf,0,2);
				sprintf(buf, "%d",LumeterAlarmConfig.lumeter_abnormal_alarm_enable);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x4104;
				memset(buf,0,64);
				memset(tmp,0,10);

				for(m = 0; m < LumeterConfigNum.number; m ++)
				{
					memset(tmp,0,10);
					sprintf(tmp, "%X",LumeterConfig[m].address);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",LumeterConfig[m].channel);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",LumeterConfig[m].value_unchanged_range);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",LumeterConfig[m].no_response_time);
					strcat(buf,tmp);
					strcat(buf,"|");
				}

				str_len = strlen(buf);
				if(str_len != 0)
				{
					buf[str_len - 1] = 0;
				}
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;
				
				ret = ConvertFrameStructToFrame(resp_server_frame_struct);
			}
			else
			{
				DeleteServerFrameStruct(resp_server_frame_struct);
			}

			vPortFree(buf);
		}
		else
		{
			DeleteServerFrameStruct(resp_server_frame_struct);
		}
	}

	return ret;
}

//告警历史查询
u8 LumeterGetAlarmReportHistory(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u16 record_num = 0;
	char buf[10];

	EventHistory_S *event_history = NULL;
	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	event_history = (EventHistory_S *)pvPortMalloc(sizeof(EventHistory_S));

	if(event_history != NULL)
	{
		event_history->device_type = (u8)LUMETER;
		event_history->event_type = (u8)TYPE_ALARM;

		for(j = 0; j < server_frame_struct->para_num; j ++)
		{
			switch(server_frame_struct->para[j].type)
			{
				case 0xA001:
					event_history->start_date = (u8 *)pvPortMalloc((server_frame_struct->para[j].len + 1) * sizeof(u8));

					if(event_history->start_date != NULL)
					{
						memcpy(event_history->start_date,server_frame_struct->para[j].value,server_frame_struct->para[j].len + 1);
					}
				break;

				case 0xA002:
					event_history->end_date = (u8 *)pvPortMalloc((server_frame_struct->para[j].len + 1) * sizeof(u8));

					if(event_history->end_date != NULL)
					{
						memcpy(event_history->end_date,server_frame_struct->para[j].value,server_frame_struct->para[j].len + 1);
					}
				break;

				default:
				break;
			}
		}

		if(server_frame_struct->para_num != 2 ||
		   event_history->start_date == NULL ||
		   event_history->end_date == NULL )
		{
			DeleteEventHistory(event_history);

			return ret;
		}

		record_num = GetAlarmEventNumFromDateSegment(*event_history);

		if(record_num == 0)
		{
			DeleteEventHistory(event_history);
		}
		else
		{
			if(xQueueSend(xQueue_HistoryRecordRead,(void *)&event_history,(TickType_t)10) != pdPASS)
			{
#ifdef DEBUG_LOG
				printf("send xQueue_HistoryRecordRead fail.\r\n");
#endif
				DeleteEventHistory(event_history);
			}
		}
	}

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;
		resp_server_frame_struct->para_num  = 2;

		resp_server_frame_struct->para = (Parameter_S *)pvPortMalloc(resp_server_frame_struct->para_num * sizeof(Parameter_S));

		if(resp_server_frame_struct->para != NULL)
		{
			resp_server_frame_struct->para[i].type = 0x8101;
			memset(buf,0,10);
			sprintf(buf, "%d",record_num);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8102;
			memset(buf,0,10);
			if(record_num >= EVENT_NUM_OF_BATCH)
			{
				sprintf(buf, "%d",EVENT_NUM_OF_BATCH);
			}
			else
			{
				sprintf(buf, "%d",record_num);
			}
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;
			
			ret = ConvertFrameStructToFrame(resp_server_frame_struct);
		}
		else
		{
			DeleteServerFrameStruct(resp_server_frame_struct);
		}
	}

	return ret;
}

//基本配置
u8 LumeterSetBasicConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 seg_num = 0;
	char tmp[17];
	char *msg = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	for(j = 0; j < server_frame_struct->para_num; j ++)
	{
		switch(server_frame_struct->para[j].type)
		{
			case 0x4001:
				msg = (char *)server_frame_struct->para[j].value;

				if(server_frame_struct->para[j].len != 0)
				{
					seg_num = 0;
					
					while(*msg != '\0')
					{
						if(*(msg ++) == '|')
						{
							seg_num ++;
						}
					}

					seg_num ++;

					if(seg_num > MAX_LUMETER_CONF_NUM)
					{
						seg_num = MAX_LUMETER_CONF_NUM;
					}

					LumeterConfigNum.number = seg_num;

					WriteLumeterConfigNum(0,1);
				}

				msg = (char *)server_frame_struct->para[j].value;

				for(k = 0; k < seg_num; k ++)
				{
					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					msg = msg + 1;
					if(i == 1)
					{
						tmp[1] = tmp[0];
						tmp[0] = '0';
					}
					StrToHex(&LumeterConfig[k].address,tmp,1);
					i = 0;

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					LumeterConfig[k].channel = myatoi(tmp);

					while(*msg != ',' && *msg != '|' && *msg != '\0')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					msg = msg + 1;
					memset(LumeterConfig[k].module,0,MAX_LUMETER_NAME_LEN);

					if(i < MAX_LUMETER_NAME_LEN)
					{
						memcpy(LumeterConfig[k].module,tmp,i);
					}

					i = 0;

					WriteLumeterConfig(k,0,1);
				}
			break;
				
			case 0x8002:
				LumeterBasicConfig.collect_cycle = myatoi((char *)server_frame_struct->para[j].value);

				WriteLumeterBasicConfig(0,1);
			break;

			case 0x4003:
				msg = (char *)server_frame_struct->para[j].value;
			
				while(*msg != ',')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				LumeterBasicConfig.stack_depth = myatoi(tmp);
				
				while(*msg != '\0')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				LumeterBasicConfig.ignore_num = myatoi(tmp);
			
				WriteLumeterBasicConfig(0,1);
			break;
				
			case 0x8004:
				LumeterBasicConfig.auto_report_cycle = myatoi((char *)server_frame_struct->para[j].value);

				WriteLumeterBasicConfig(0,1);
			break;
			
			case 0x8005:
				LumeterBasicConfig.data_source = myatoi((char *)server_frame_struct->para[j].value);

				WriteLumeterBasicConfig(0,1);
			break;
			
			case 0x4006:
				msg = (char *)server_frame_struct->para[j].value;
			
				while(*msg != ',')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				LumeterBasicConfig.light_on_thre = myatoi(tmp);
				
				while(*msg != ',')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				LumeterBasicConfig.light_on_advance_time = myatoi(tmp);
				
				while(*msg != '\0')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				LumeterBasicConfig.light_on_delay_time = myatoi(tmp);
			

				WriteLumeterBasicConfig(0,1);
			break;
				
			case 0x4007:
				msg = (char *)server_frame_struct->para[j].value;
			
				while(*msg != ',')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				LumeterBasicConfig.light_off_thre = myatoi(tmp);
				
				while(*msg != ',')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				LumeterBasicConfig.light_off_advance_time = myatoi(tmp);
				
				while(*msg != '\0')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				LumeterBasicConfig.light_off_delay_time = myatoi(tmp);
			
				WriteLumeterBasicConfig(0,1);
			break;

			default:
			break;
		}
	}

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

//读取基本配置
u8 LumeterGetBasicConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 m = 0;
	u8 ret = 0;
	char tmp[20] = {0};
	char *buf = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		buf = (char *)pvPortMalloc(128 * sizeof(char));
		
		if(buf != NULL)
		{
			CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

			resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
			resp_server_frame_struct->msg_len 	= 10;
			resp_server_frame_struct->err_code 	= (u8)NO_ERR;
			resp_server_frame_struct->para_num 	= 7;

			resp_server_frame_struct->para = (Parameter_S *)pvPortMalloc(resp_server_frame_struct->para_num * sizeof(Parameter_S));

			if(resp_server_frame_struct->para != NULL)
			{
				resp_server_frame_struct->para[i].type = 0x4101;
				memset(buf,0,128);
				memset(tmp,0,10);
				for(m = 0; m < LumeterConfigNum.number; m ++)
				{
					memset(tmp,0,10);
					sprintf(tmp, "%X",LumeterConfig[m].address);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",LumeterConfig[m].channel);
					strcat(buf,tmp);
					strcat(buf,",");

					strcat(buf,(char *)LumeterConfig[m].module);

					if(m < LumeterConfigNum.number - 1)
					{
						strcat(buf,"|");
					}
				}
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;
				
				resp_server_frame_struct->para[i].type = 0x8102;
				memset(buf,0,10);
				sprintf(buf, "%d",LumeterBasicConfig.collect_cycle);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;
				
				resp_server_frame_struct->para[i].type = 0x8103;
				memset(buf,0,20);
				sprintf(buf, "%d",LumeterBasicConfig.stack_depth);
				strcat(buf,",");
				memset(tmp,0,10);
				sprintf(tmp, "%d",LumeterBasicConfig.ignore_num);
				strcat(buf,tmp);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x8104;
				memset(buf,0,10);
				sprintf(buf, "%d",LumeterBasicConfig.auto_report_cycle);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;
				
				resp_server_frame_struct->para[i].type = 0x8105;
				memset(buf,0,10);
				sprintf(buf, "%d",LumeterBasicConfig.data_source);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;
				
				resp_server_frame_struct->para[i].type = 0x4106;
				memset(buf,0,20);
				sprintf(buf, "%d",LumeterBasicConfig.light_on_thre);
				strcat(buf,",");
				memset(tmp,0,10);
				sprintf(tmp, "%d",LumeterBasicConfig.light_on_advance_time);
				strcat(buf,tmp);
				strcat(buf,",");
				sprintf(tmp, "%d",LumeterBasicConfig.light_on_delay_time);
				strcat(buf,tmp);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;
				
				resp_server_frame_struct->para[i].type = 0x4107;
				memset(buf,0,20);
				sprintf(buf, "%d",LumeterBasicConfig.light_off_thre);
				strcat(buf,",");
				memset(tmp,0,10);
				sprintf(tmp, "%d",LumeterBasicConfig.light_off_advance_time);
				strcat(buf,tmp);
				strcat(buf,",");
				sprintf(tmp, "%d",LumeterBasicConfig.light_off_delay_time);
				strcat(buf,tmp);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;
				
				ret = ConvertFrameStructToFrame(resp_server_frame_struct);
			}
			else
			{
				DeleteServerFrameStruct(resp_server_frame_struct);
			}

			vPortFree(buf);
		}
		else
		{
			DeleteServerFrameStruct(resp_server_frame_struct);
		}
	}

	return ret;
}
































































