#include "input_collector_comm.h"
#include "concentrator_comm.h"
#include "common.h"


u8 InputCollectorStateChangesReportResponse = 0;

//发送告警事件
void InputCollectorSendStateChangesReportFrameToServer(InputCollectorState_S *module_state)
{
	u8 i = 0;
	u8 m = 0;
	u8 n = 0;
	u8 a_loop_num = 0;
	char tmp[25] = {0};
	char *buf = NULL;
	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

	buf = (char *)pvPortMalloc(400 * sizeof(char));

	if(buf != NULL)
	{
		server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

		if(server_frame_struct != NULL && module_state != NULL)
		{
			InitServerFrameStruct(server_frame_struct);

			server_frame_struct->msg_type 	= (u8)DEVICE_REQUEST_UP;	//响应服务器类型
			server_frame_struct->msg_len 	= 10;
			server_frame_struct->err_code 	= (u8)NO_ERR;
			server_frame_struct->msg_id		= 0x0371;
			server_frame_struct->para_num	= 6;

			server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

			if(server_frame_struct->para != NULL)
			{
				i = 0;

				server_frame_struct->para[i].type = 0x3001;
				memset(buf,0,25);
				sprintf(buf, "%x",module_state->address);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x4002;
				memset(buf,0,25);
				sprintf(buf, "%d",module_state->channel);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x3003;
				memset(buf,0,25);
				if(module_state->d_channel_bit != 0)
				{
					sprintf(buf, "%04x",module_state->d_channel_bit);
					server_frame_struct->para[i].len = strlen(buf);
				}
				else
				{
					server_frame_struct->para[i].len = 0;
				}
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x3004;
				memset(buf,0,25);
				sprintf(buf, "%04x",module_state->d_current_state);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x3005;
				memset(buf,0,25);
				if(module_state->a_channel_bit != 0)
				{
					sprintf(buf, "%04x",module_state->a_channel_bit);
					server_frame_struct->para[i].len = strlen(buf);
				}
				else
				{
					server_frame_struct->para[i].len = 0;
				}
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x4006;
				memset(buf,0,400);
				
				for(n = 0; n < MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM; n ++)
				{
					if(module_state->a_channel_bit & (1 << n))
					{
						a_loop_num ++;
					}
				}
				
				for(m = 0; m < a_loop_num; m ++)
				{
					memset(tmp,0,25);
					sprintf(tmp, "%f",module_state->a_current_state[m]);
					strcat(buf,tmp);
					
					if(m < a_loop_num - 1)
					{
						strcat(buf,",");
					}
				}
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

		vPortFree(buf);
	}
}

//向服务器发送状态变化事件
void InputCollectorSendStateChangesReportToServer(void)
{
	BaseType_t xResult;
	static InputCollectorState_S *module_state = NULL;
	static time_t time_5 = 0;
	static u8 retry_times5 = 0;

	if(InputCollectorStateChangesReportResponse == 0)
	{
		if(module_state != NULL)
		{
			vPortFree(module_state);
			module_state = NULL;
		}

		if(module_state == NULL)
		{
			xResult = xQueueReceive(xQueue_InputCollectorState,(void *)&module_state,(TickType_t)pdMS_TO_TICKS(1));

			if(xResult == pdPASS)
			{
				InputCollectorStateChangesReportResponse = 1;

				time_5 = GetSysTick1s();
				retry_times5 = 0;

				RE_SEND_STATE_CHANGES_REPORT:
				InputCollectorSendStateChangesReportFrameToServer(module_state);
			}
		}
	}

	if(InputCollectorStateChangesReportResponse == 1)
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

				InputCollectorStateChangesReportResponse = 0;

				vPortFree(module_state);
				module_state = NULL;
			}
		}
	}
}

//处理接收到的报文
void InputCollectorRecvAndHandleFrameStruct(void)
{
	BaseType_t xResult;
	ServerFrameStruct_S *server_frame_struct = NULL;

	xResult = xQueueReceive(xQueue_InputCollectorFrameStruct,(void *)&server_frame_struct,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS )
	{
		switch(server_frame_struct->msg_id)
		{
			case 0x0000:	//数据透传

			break;

			case 0x0370:	//状态查询
				InputCollectorGetCurrentState(server_frame_struct);
			break;

			case 0x0371:	//状态上报
				InputCollectorStateChangesReportResponse = 0;
			break;

			case 0x0372:	//状态历史查询

			break;

			case 0x03A0:	//告警上报
				AlarmReportResponse = 0;
			break;

			case 0x03A1:	//告警解除
				AlarmReportResponse = 0;
			break;

			case 0x03A2:	//事件上报

			break;

			case 0x03A3:	//告警配置
				InputCollectorSetAlarmConfiguration(server_frame_struct);
			break;

			case 0x03A4:	//告警配置查询
				InputCollectorGetAlarmConfiguration(server_frame_struct);
			break;

			case 0x03A5:	//告警历史查询
				InputCollectorGetAlarmReportHistory(server_frame_struct);
			break;

			case 0x03D0:	//基础配置
				InputCollectorSetBasicConfiguration(server_frame_struct);
			break;

			case 0x03D1:	//查询基础配置
				InputCollectorGetBasicConfiguration(server_frame_struct);
			break;

			default:
			break;
		}

		DeleteServerFrameStruct(server_frame_struct);
	}
}

//状态查询
u8 InputCollectorGetCurrentState(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 m = 0;
	u8 ret = 0;
	u8 module_num = 0;
	u8 add = 0;
	u8 ch = 0;
	char tmp[25] = {0};
	char *buf = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

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

			buf = (char *)pvPortMalloc(400 * sizeof(char));

			if(buf != NULL)
			{
				module_num = server_frame_struct->para_num / 2;

				for(k = 0; k < module_num; k ++)
				{
					for(j = 0; j < InputCollectorConfigNum.number; j ++)
					{
						add = myatoi((char *)server_frame_struct->para[k * 2 + 0].value);
						ch = myatoi((char *)server_frame_struct->para[k * 2 + 1].value);

						if(add == InputCollectorState[j].address && ch == InputCollectorState[j].channel)
						{
							ServerFrameStruct_S *state_server_frame_struct = NULL;		//用于响应服务器

							state_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

							if(state_server_frame_struct != NULL)
							{
								CopyServerFrameStruct(server_frame_struct,state_server_frame_struct,0);

								state_server_frame_struct->msg_type = (u8)SYNC_RESPONSE;	//响应服务器类型
								state_server_frame_struct->msg_len 	= 10;
								state_server_frame_struct->err_code = (u8)NO_ERR;

								state_server_frame_struct->para_num = 6;

								state_server_frame_struct->para = (Parameter_S *)pvPortMalloc(state_server_frame_struct->para_num * sizeof(Parameter_S));

								if(state_server_frame_struct->para != NULL)
								{
									i = 0;

									state_server_frame_struct->para[i].type = 0x3001;
									memset(buf,0,25);
									sprintf(buf, "%x",InputCollectorState[j].address);
									state_server_frame_struct->para[i].len = strlen(buf);
									state_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
									if(state_server_frame_struct->para[i].value != NULL)
									{
										memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
									}
									i ++;

									state_server_frame_struct->para[i].type = 0x4002;
									memset(buf,0,25);
									sprintf(buf, "%d",InputCollectorState[j].channel);
									state_server_frame_struct->para[i].len = strlen(buf);
									state_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
									if(state_server_frame_struct->para[i].value != NULL)
									{
										memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
									}
									i ++;

									state_server_frame_struct->para[i].type = 0x3003;
									memset(buf,0,25);
									if(InputCollectorState[j].d_channel_bit != 0)
									{
										sprintf(buf, "%04x",InputCollectorState[j].d_channel_bit);
										state_server_frame_struct->para[i].len = strlen(buf);
									}
									else
									{
										state_server_frame_struct->para[i].len = 0;
									}
									state_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
									if(state_server_frame_struct->para[i].value != NULL)
									{
										memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
									}
									i ++;

									state_server_frame_struct->para[i].type = 0x3004;
									memset(buf,0,25);
									sprintf(buf, "%04x",InputCollectorState[j].d_current_state);
									state_server_frame_struct->para[i].len = strlen(buf);
									state_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
									if(state_server_frame_struct->para[i].value != NULL)
									{
										memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
									}
									i ++;

									state_server_frame_struct->para[i].type = 0x3005;
									memset(buf,0,25);
									if(InputCollectorState[j].a_channel_bit != 0)
									{
										sprintf(buf, "%04x",InputCollectorState[j].a_channel_bit);
										state_server_frame_struct->para[i].len = strlen(buf);
									}
									else
									{
										state_server_frame_struct->para[i].len = 0;
									}
									state_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
									if(state_server_frame_struct->para[i].value != NULL)
									{
										memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
									}
									i ++;

									state_server_frame_struct->para[i].type = 0x4006;
									memset(buf,0,400);
									for(m = 0; m < InputCollectorConfig[j].a_loop_num; m ++)
									{
										memset(tmp,0,25);
										sprintf(tmp, "%f",InputCollectorState[j].a_current_state[m]);
										strcat(buf,tmp);
										
										if(m < InputCollectorConfig[j].a_loop_num - 1)
										{
											strcat(buf,",");
										}
									}
									state_server_frame_struct->para[i].len = strlen(buf);
									state_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
									if(state_server_frame_struct->para[i].value != NULL)
									{
										memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
									}
									i ++;
								}
								
								ret = ConvertFrameStructToFrame(state_server_frame_struct);
							}
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
u8 InputCollectorSetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 m = 0;
	u8 n = 0;
	u8 add = 0;
	u8 ch = 0;
	u8 loop_ch = 0;
	u8 seg_num = 0;
	char tmp[10];
	char *msg = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	for(j = 0; j < server_frame_struct->para_num; j ++)
	{
		switch(server_frame_struct->para[j].type)
		{
			case 0x6001:
				InputCollectorAlarmConfig.d_quantity_abnormal_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
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

						for(m = 0; m < InputCollectorConfigNum.number; m ++)
						{
							if(add == InputCollectorConfig[m].address &&
							   ch == InputCollectorConfig[m].channel)
							{
								break;
							}
						}

						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						loop_ch = myatoi(tmp);
						if(loop_ch == 0)
						{
							loop_ch = 1;
						}
						InputCollectorConfig[m].d_alarm_thre[loop_ch - 1].channel = loop_ch;

						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						InputCollectorConfig[m].d_alarm_thre[loop_ch - 1].alarm_level = myatoi(tmp);

						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						InputCollectorConfig[m].d_alarm_thre[loop_ch - 1].confirm_time = myatoi(tmp);

						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						InputCollectorConfig[m].d_alarm_thre[loop_ch - 1].switch_run_mode = myatoi(tmp);

						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						InputCollectorConfig[m].d_alarm_thre[loop_ch - 1].resume_run_mode = myatoi(tmp);
						
						while(*msg != ',' && *msg != '|' && *msg != '\0')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						InputCollectorConfig[m].d_alarm_thre[loop_ch - 1].relay_action = myatoi(tmp);
					}

					for(k = 0; k < InputCollectorConfigNum.number; k ++)
					{
						WriteInputCollectorConfig(k,0,1);
					}
				}
			break;

			case 0x6003:
				InputCollectorAlarmConfig.a_quantity_abnormal_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
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

						for(m = 0; m < InputCollectorConfigNum.number; m ++)
						{
							if(add == InputCollectorConfig[m].address &&
							   ch == InputCollectorConfig[m].channel)
							{
								break;
							}
						}

						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						loop_ch = myatoi(tmp);
						if(loop_ch == 0)
						{
							loop_ch = 1;
						}
						InputCollectorConfig[m].a_alarm_thre[loop_ch - 1].channel = loop_ch;

						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						if(tmp[0] == 'N' && tmp[1] == 'A')
						{
							memset((void *)&InputCollectorConfig[m].a_alarm_thre[loop_ch - 1].min_value,0xFF,8);
						}
						else
						{
							InputCollectorConfig[m].a_alarm_thre[loop_ch - 1].min_value = atof(tmp);
						}
						
						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						if(tmp[0] == 'N' && tmp[1] == 'A')
						{
							memset((void *)&InputCollectorConfig[m].a_alarm_thre[loop_ch - 1].max_value,0xFF,8);
						}
						else
						{
							InputCollectorConfig[m].a_alarm_thre[loop_ch - 1].max_value = atof(tmp);
						}
						
						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						InputCollectorConfig[m].a_alarm_thre[loop_ch - 1].confirm_time = myatoi(tmp);

						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						InputCollectorConfig[m].a_alarm_thre[loop_ch - 1].switch_run_mode = myatoi(tmp);

						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						InputCollectorConfig[m].a_alarm_thre[loop_ch - 1].resume_run_mode = myatoi(tmp);
						
						while(*msg != ',' && *msg != '|' && *msg != '\0')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						InputCollectorConfig[m].a_alarm_thre[loop_ch - 1].relay_action = myatoi(tmp);
					}

					for(k = 0; k < InputCollectorConfigNum.number; k ++)
					{
						WriteInputCollectorConfig(k,0,1);
					}
				}
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

	WriteInputCollectorAlarmConfig(0,1);

	return ret;
}

//获取告警配置参数
u8 InputCollectorGetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 j = 0;
	u8 m = 0;
	u8 ret = 0;
	long long thre = {0};
	char tmp[25] = {0};
	char *buf = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		buf = (char *)pvPortMalloc(1700 * sizeof(char));

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
				sprintf(buf, "%d",InputCollectorAlarmConfig.d_quantity_abnormal_alarm_enable);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x4102;
				memset(buf,0,1700);
				memset(tmp,0,10);
				
				for(m = 0; m < InputCollectorConfigNum.number; m ++)
				{
					for(j = 0; j < InputCollectorConfig[m].d_loop_num; j ++)
					{
						if(InputCollectorConfig[m].d_alarm_thre[j].channel == j + 1)
						{
							memset(tmp,0,10);
							sprintf(tmp, "%x",InputCollectorConfig[m].address);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",InputCollectorConfig[m].channel);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",InputCollectorConfig[m].d_alarm_thre[j].channel);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",InputCollectorConfig[m].d_alarm_thre[j].alarm_level);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",InputCollectorConfig[m].d_alarm_thre[j].confirm_time);
							strcat(buf,tmp);
							strcat(buf,",");
							
							memset(tmp,0,10);
							sprintf(tmp, "%d",InputCollectorConfig[m].d_alarm_thre[j].switch_run_mode);
							strcat(buf,tmp);
							strcat(buf,",");
							
							memset(tmp,0,10);
							sprintf(tmp, "%d",InputCollectorConfig[m].d_alarm_thre[j].resume_run_mode);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",InputCollectorConfig[m].d_alarm_thre[j].relay_action);
							strcat(buf,tmp);
							strcat(buf,"|");
						}
					}
				}

				buf[strlen(buf) - 1] = 0;	//去掉最后一个'|'
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x6103;
				memset(buf,0,2);
				sprintf(buf, "%d",InputCollectorAlarmConfig.a_quantity_abnormal_alarm_enable);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x4104;
				memset(buf,0,1700);
				memset(tmp,0,10);

				for(m = 0; m < InputCollectorConfigNum.number; m ++)
				{
					for(j = 0; j < InputCollectorConfig[m].a_loop_num; j ++)
					{
						if(InputCollectorConfig[m].a_alarm_thre[j].channel == j + 1)
						{
							memset(tmp,0,10);
							sprintf(tmp, "%x",InputCollectorConfig[m].address);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",InputCollectorConfig[m].channel);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",InputCollectorConfig[m].a_alarm_thre[j].channel);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,25);
							memcpy(&thre,(void *)&InputCollectorConfig[m].a_alarm_thre[j].min_value,4);
							if(thre == 0xFFFFFFFFFFFFFFFF)
							{
								strcat(buf,"NA,");
							}
							else
							{
								sprintf(tmp, "%f",InputCollectorConfig[m].a_alarm_thre[j].min_value);
								strcat(buf,tmp);
								strcat(buf,",");
							}

							memset(tmp,0,25);
							memcpy(&thre,(void *)&InputCollectorConfig[m].a_alarm_thre[j].max_value,4);
							if(thre == 0xFFFFFFFFFFFFFFFF)
							{
								strcat(buf,"NA,");
							}
							else
							{
								sprintf(tmp, "%f",InputCollectorConfig[m].a_alarm_thre[j].max_value);
								strcat(buf,tmp);
								strcat(buf,",");
							}
	
							memset(tmp,0,10);
							sprintf(tmp, "%d",InputCollectorConfig[m].a_alarm_thre[j].confirm_time);
							strcat(buf,tmp);
							strcat(buf,",");
							
							memset(tmp,0,10);
							sprintf(tmp, "%d",InputCollectorConfig[m].a_alarm_thre[j].switch_run_mode);
							strcat(buf,tmp);
							strcat(buf,",");
							
							memset(tmp,0,10);
							sprintf(tmp, "%d",InputCollectorConfig[m].a_alarm_thre[j].resume_run_mode);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",InputCollectorConfig[m].a_alarm_thre[j].relay_action);
							strcat(buf,tmp);
							strcat(buf,"|");
						}
					}
				}

				buf[strlen(buf) - 1] = 0;	//去掉最后一个'|'
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;
			}

			ret = ConvertFrameStructToFrame(resp_server_frame_struct);

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
u8 InputCollectorGetAlarmReportHistory(ServerFrameStruct_S *server_frame_struct)
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
		event_history->device_type = (u8)INPUT_COLLECTOR;
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
		}

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

//基本配置
u8 InputCollectorSetBasicConfiguration(ServerFrameStruct_S *server_frame_struct)
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
			case 0x8001:
				InputCollectorBasicConfig.detect_interval = myatoi((char *)server_frame_struct->para[j].value);

				WriteInputCollectorBasicConfig(0,1);
			break;

			case 0x6002:
				InputCollectorBasicConfig.auto_report = myatoi((char *)server_frame_struct->para[j].value);

				WriteInputCollectorBasicConfig(0,1);
			break;

			case 0x4003:
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

					if(seg_num > MAX_INPUT_COLLECTOR_CONF_NUM)
					{
						seg_num = MAX_INPUT_COLLECTOR_CONF_NUM;
					}

					InputCollectorConfigNum.number = seg_num;

					WriteInputCollectorConfigNum(0,1);
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
					StrToHex(&InputCollectorConfig[k].address,tmp,1);
					i = 0;

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					InputCollectorConfig[k].channel = myatoi(tmp);

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					InputCollectorConfig[k].d_loop_num = myatoi(tmp);
					
					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					InputCollectorConfig[k].a_loop_num = myatoi(tmp);

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					InputCollectorConfig[k].a_quantity_range = atof(tmp);
					
					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					InputCollectorConfig[k].confirm_time = myatoi(tmp);

					while(*msg != ',' && *msg != '|' && *msg != '\0')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					msg = msg + 1;
					memset(InputCollectorConfig[k].module,0,MAX_INPUT_COLLECTOR_MODULE_NAME_LEN);

					if(i < MAX_INPUT_COLLECTOR_MODULE_NAME_LEN)
					{
						memcpy(InputCollectorConfig[k].module,tmp,i);
					}

					i = 0;

					WriteInputCollectorConfig(k,0,1);
				}
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
u8 InputCollectorGetBasicConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 m = 0;
	u8 ret = 0;
	char tmp[10] = {0};
	char *buf = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		buf = (char *)pvPortMalloc(600 * sizeof(char));

		if(buf != NULL)
		{
			CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

			resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
			resp_server_frame_struct->msg_len 	= 10;
			resp_server_frame_struct->err_code 	= (u8)NO_ERR;
			resp_server_frame_struct->para_num 	= 3;

			resp_server_frame_struct->para = (Parameter_S *)pvPortMalloc(resp_server_frame_struct->para_num * sizeof(Parameter_S));

			if(resp_server_frame_struct->para != NULL)
			{
				resp_server_frame_struct->para[i].type = 0x8101;
				memset(buf,0,10);
				sprintf(buf, "%d",InputCollectorBasicConfig.detect_interval);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;
				
				resp_server_frame_struct->para[i].type = 0x6102;
				memset(buf,0,10);
				sprintf(buf, "%d",InputCollectorBasicConfig.auto_report);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x4103;
				memset(buf,0,600);
				memset(tmp,0,10);

				for(m = 0; m < InputCollectorConfigNum.number; m ++)
				{
					memset(tmp,0,10);
					sprintf(tmp, "%x",InputCollectorConfig[m].address);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",InputCollectorConfig[m].channel);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",InputCollectorConfig[m].d_loop_num);
					strcat(buf,tmp);
					strcat(buf,",");
					
					memset(tmp,0,10);
					sprintf(tmp, "%d",InputCollectorConfig[m].a_loop_num);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%f",InputCollectorConfig[m].a_quantity_range);
					strcat(buf,tmp);
					strcat(buf,",");
					
					memset(tmp,0,10);
					sprintf(tmp, "%d",InputCollectorConfig[m].confirm_time);
					strcat(buf,tmp);
					strcat(buf,",");

					strcat(buf,(char *)InputCollectorConfig[m].module);

					if(m < InputCollectorConfigNum.number - 1)
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
			}

			ret = ConvertFrameStructToFrame(resp_server_frame_struct);

			vPortFree(buf);
		}
		else
		{
			DeleteServerFrameStruct(resp_server_frame_struct);
		}

	}

	return ret;
}
































































