#include "relay_comm.h"
#include "common.h"
#include "concentrator_comm.h"
#include "rx8010s.h"

u8 RelayStateChangesReportResponse = 0;						//继电器状态变化上报响应标志


//发送告警事件
void RelaySendStateChangesReportFrameToServer(RelayModuleState_S *module_state)
{
	u8 i = 0;
	u8 m = 0;
	u8 n = 0;
	char tmp[10] = {0};
	char *buf = NULL;
	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

	buf = (char *)mymalloc(400 * sizeof(char));

	if(buf != NULL)
	{
		server_frame_struct = (ServerFrameStruct_S *)mymalloc(sizeof(ServerFrameStruct_S));

		if(server_frame_struct != NULL && module_state != NULL)
		{
			InitServerFrameStruct(server_frame_struct);

			server_frame_struct->msg_type 	= (u8)DEVICE_REQUEST_UP;	//响应服务器类型
			server_frame_struct->msg_len 	= 10;
			server_frame_struct->err_code 	= (u8)NO_ERR;
			server_frame_struct->msg_id		= 0x0271;
			server_frame_struct->para_num	= 7;

			server_frame_struct->para = (Parameter_S *)mymalloc(server_frame_struct->para_num * sizeof(Parameter_S));

			if(server_frame_struct->para != NULL)
			{
				server_frame_struct->para[i].type = 0x3001;
				memset(buf,0,25);
				sprintf(buf, "%x",module_state->address);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)mymalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x4002;
				memset(buf,0,25);
				sprintf(buf, "%d",module_state->channel);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)mymalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x3003;
				memset(buf,0,25);
				sprintf(buf, "%04x",module_state->loop_channel_bit);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)mymalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x3004;
				memset(buf,0,25);
				sprintf(buf, "%04x",module_state->loop_current_state);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)mymalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x8005;
				memset(buf,0,25);
				sprintf(buf, "%d",module_state->controller);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)mymalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0xA006;
				memset(buf,0,25);
				memcpy(buf,module_state->control_time,14);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)mymalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x4007;
				if(module_state->abnormal_loop == 0)
				{
					server_frame_struct->para[i].len = 0;
					memset(buf,0,2);
				}
				else
				{
					for(m = 0; m < MAX_RELAY_MODULE_LOOP_CH_NUM; m ++)
					{
						if(module_state->abnormal_loop & (1 << m))
						{
							for(n = 0; n < MAX_RELAY_MODULE_ALARM_TYPE_NUM; n ++)
							{
								if(module_state->loop_alarm[m][n] != 0)
								{
									memset(tmp,0,10);
									sprintf(tmp, "%d",module_state->loop_alarm[m][n]);
									strcat(buf,tmp);
									strcat(buf,",");
								}
							}

							buf[strlen(buf) - 1] = '\0';
						}
						else
						{
							memset(tmp,0,10);
							sprintf(tmp, "%d",0);
							strcat(buf,tmp);
						}

						if(m < MAX_RELAY_MODULE_LOOP_CH_NUM - 1)
						{
							strcat(buf,"|");
						}
					}
					
					server_frame_struct->para[i].len = strlen(buf);
				}
				server_frame_struct->para[i].value = (u8 *)mymalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;
			}

			ConvertFrameStructToFrame(server_frame_struct);
		}

		myfree(buf);
	}
}

//向服务器发送状态变化事件
void RelaySendStateChangesReportToServer(void)
{
	BaseType_t xResult;
	static RelayModuleState_S *module_state = NULL;
	static time_t time_5 = 0;
	static u8 retry_times5 = 0;

	if(RelayStateChangesReportResponse == 0)
	{
		if(module_state != NULL)
		{
			myfree(module_state);
			module_state = NULL;
		}

		if(module_state == NULL)
		{
			xResult = xQueueReceive(xQueue_RelayModuleState,(void *)&module_state,(TickType_t)pdMS_TO_TICKS(1));

			if(xResult == pdPASS)
			{
				RelayStateChangesReportResponse = 1;

				time_5 = GetSysTick1s();
				retry_times5 = 0;

				RE_SEND_STATE_CHANGES_REPORT:
				RelaySendStateChangesReportFrameToServer(module_state);
			}
		}
	}

	if(RelayStateChangesReportResponse == 1)
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

				RelayStateChangesReportResponse = 0;

				myfree(module_state);
				module_state = NULL;
			}
		}
	}
}

//处理接收到的报文
void RelayRecvAndHandleFrameStruct(void)
{
	BaseType_t xResult;
	ServerFrameStruct_S *server_frame_struct = NULL;

	xResult = xQueueReceive(xQueue_RelayFrameStruct,(void *)&server_frame_struct,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS )
	{
		switch(server_frame_struct->msg_id)
		{
			case 0x0000:	//数据透传

			break;

			case 0x0201:	//回路控制
				RelayLoopControl(server_frame_struct);
			break;

			case 0x0202:	//分组控制
				RelayLoopGroupControl(server_frame_struct);
			break;

			case 0x0203:	//模式切换
				RelaySetStrategyGroupSwitch(server_frame_struct);
			break;

			case 0x0270:	//状态查询
				RelayGetCurrentState(server_frame_struct);
			break;

			case 0x0271:	//状态上报
				RelayStateChangesReportResponse = 0;
			break;

			case 0x0272:	//状态历史查询

			break;

			case 0x02A0:	//告警上报
				AlarmReportResponse = 0;
			break;

			case 0x02A1:	//告警解除
				AlarmReportResponse = 0;
			break;

			case 0x02A2:	//事件上报

			break;

			case 0x02A3:	//告警配置
				RelaySetAlarmConfiguration(server_frame_struct);
			break;

			case 0x02A4:	//告警配置查询
				RelayGetAlarmConfiguration(server_frame_struct);
			break;

			case 0x02A5:	//告警历史查询
				RelayGetAlarmReportHistory(server_frame_struct);
			break;

			case 0x02D0:	//基础配置
				RelaySetBasicConfiguration(server_frame_struct);
			break;

			case 0x02D1:	//查询基础配置
				RelayGetBasicConfiguration(server_frame_struct);
			break;

			case 0x02D2:	//场景配置
				RelaySetRelayAppointment(server_frame_struct);
			break;

			case 0x02D3:	//查询场景配置
				RelayGetRelayAppointment(server_frame_struct);
			break;

			case 0x02D4:	//任务配置
				RelaySetRelayStrategy(server_frame_struct);
			break;

			case 0x02D5:	//查询任务配置
				RelayGetRelayStrategy(server_frame_struct);
			break;

			default:
			break;
		}

		DeleteServerFrameStruct(server_frame_struct);
	}
}

//回路控制
u8 RelayLoopControl(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 n = 0;
	u8 tmp_h = 0;
	u8 tmp_l = 0;
	u16 loop_ch = 0;
	u16 loop_state = 0;
	u8 ret = 0;
	u8 module_num = 0;
	u8 add = 0;
	u8 ch = 0;
	char *msg = NULL;
	char tmp[5] = {0};

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)mymalloc(sizeof(ServerFrameStruct_S));

	if(server_frame_struct->para_num % 4 == 0)
	{
		if(resp_server_frame_struct != NULL)				//参数个数是4的倍数
		{
			CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

			resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
			resp_server_frame_struct->msg_len 	= 10;
			resp_server_frame_struct->err_code 	= (u8)NO_ERR;

			ret = ConvertFrameStructToFrame(resp_server_frame_struct);

			module_num = server_frame_struct->para_num / 4;

			for(k = 0; k < module_num; k ++)
			{
				for(j = 0; j < RelayModuleConfigNum.number; j ++)
				{
					n = 0;
						
					msg = (char *)server_frame_struct->para[k * 4 + 0].value;
					
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
					
					ch = myatoi((char *)server_frame_struct->para[k * 4 + 1].value);

					if(add == RelayModuleState[j].address && ch == RelayModuleState[j].channel)
					{
						if(server_frame_struct->para[k * 4 + 2].len == 1 ||
						   server_frame_struct->para[k * 4 + 2].len == 2)
						{
							msg = (char *)server_frame_struct->para[k * 4 + 2].value;
								
							while(*msg != '\0')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							
							if(i == 1)
							{
								tmp[1] = tmp[0];
								tmp[0] = '0';
							}

							StrToHex((u8 *)&loop_ch,tmp,1);
						}
						else if(server_frame_struct->para[k * 4 + 2].len == 3 ||
							    server_frame_struct->para[k * 4 + 2].len == 4)
						{
							msg = (char *)server_frame_struct->para[k * 4 + 2].value;
								
							while(*msg != '\0')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							
							if(i == 3)
							{
								tmp[3] = tmp[2];
								tmp[2] = tmp[1];
								tmp[1] = tmp[0];
								tmp[0] = '0';
							}

							StrToHex(&tmp_h,tmp + 0,1);
							StrToHex(&tmp_l,tmp + 2,1);

							loop_ch = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
						}

						if(server_frame_struct->para[k * 4 + 3].len == 1 ||
						   server_frame_struct->para[k * 4 + 3].len == 2)
						{
							msg = (char *)server_frame_struct->para[k * 4 + 3].value;
								
							while(*msg != '\0')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							
							if(i == 1)
							{
								tmp[1] = tmp[0];
								tmp[0] = '0';
							}

							StrToHex((u8 *)&loop_ch,tmp,1);
						}
						else if(server_frame_struct->para[k * 4 + 3].len == 3 ||
							    server_frame_struct->para[k * 4 + 3].len == 4)
						{
							msg = (char *)server_frame_struct->para[k * 4 + 3].value;
								
							while(*msg != '\0')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							
							if(i == 3)
							{
								tmp[3] = tmp[2];
								tmp[2] = tmp[1];
								tmp[1] = tmp[0];
								tmp[0] = '0';
							}

							StrToHex(&tmp_h,tmp + 0,1);
							StrToHex(&tmp_l,tmp + 2,1);

							loop_state = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
						}

						loop_ch &= RelayModuleConfig[j].loop_enable;
						loop_state &= RelayModuleConfig[j].loop_enable;

						RelayModuleState[j].loop_current_channel = loop_ch;
						RelayModuleState[j].loop_current_state = loop_state;
						RelayModuleState[j].controller = 1;
						memset(RelayModuleState[j].control_time,0,15);
						TimeToString(RelayModuleState[j].control_time,
						             calendar.w_year,
						             calendar.w_month,
						             calendar.w_date,
						             calendar.hour,
						             calendar.min,
						             calendar.sec);
						
						RelayModuleState[j].execute_immediately = 1;
						
						break;
					}
				}
			}
		}
	}

	return ret;
}

//回路分组控制
u8 RelayLoopGroupControl(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 m = 0;
	u8 n = 0;
	u16 loop_ch = 0;
	u16 loop_state = 0;
	u8 ret = 0;
	u8 group_num = 0;
	u8 group[MAX_RELAY_MODULE_LOOP_GROUP_NUM] = {0};
	u8 on_off = 0;
	char *msg = NULL;
	char tmp[5] = {0};

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)mymalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		if(server_frame_struct->para_num  == 2)				//参数个数是4的倍数
		{
			CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

			resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
			resp_server_frame_struct->msg_len 	= 10;
			resp_server_frame_struct->err_code 	= (u8)NO_ERR;

			ret = ConvertFrameStructToFrame(resp_server_frame_struct);

			msg = (char *)server_frame_struct->para[0].value;

			while(*msg != '\0')
			{
				if(*(msg ++) == ',')
				{
					group_num ++;
				}
			}

			group_num ++;

			if(group_num > MAX_RELAY_MODULE_LOOP_GROUP_NUM)
			{
				group_num = MAX_RELAY_MODULE_LOOP_GROUP_NUM;
			}

			msg = (char *)server_frame_struct->para[0].value;
			
			for(n = 0; n < group_num; n ++)
			{
				while(*msg != ',' && *msg != '\0')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				group[n] = myatoi(tmp);
			}

			on_off = myatoi((char *)server_frame_struct->para[1].value);

			if(group_num == 1 && group[0] == 0)
			{
				for(i = 0; i < RelayModuleConfigNum.number; i ++)
				{
					loop_ch = 0;
					loop_state = 0;

					for(k = 0; k < RelayModuleConfig[i].loop_num; k ++)
					{
						for(m = 0; m < MAX_RELAY_MODULE_LOOP_GROUP_NUM; m ++)
						{
							if(RelayModuleConfig[i].loop_group[k][m] != 0)
							{
								loop_ch |= (1 << k);

								if(on_off == 1)
								{
									loop_state |= (1 << k);
								}
								else if(on_off == 0)
								{
									loop_state &= ~(1 << k);
								}
								
								break;
							}
						}
					}

					if(loop_ch != 0)
					{
						loop_ch &= RelayModuleConfig[i].loop_enable;
						loop_state &= RelayModuleConfig[i].loop_enable;

						RelayModuleState[i].loop_current_channel = loop_ch;
						RelayModuleState[i].loop_current_state = loop_state;
						RelayModuleState[i].controller = 1;
						memset(RelayModuleState[i].control_time,0,15);
						TimeToString(RelayModuleState[i].control_time,
						             calendar.w_year,
						             calendar.w_month,
						             calendar.w_date,
						             calendar.hour,
						             calendar.min,
						             calendar.sec);
						
						RelayModuleState[i].execute_immediately = 1;
					}
				}
			}
			else
			{
				for(i = 0; i < RelayModuleConfigNum.number; i ++)
				{
					loop_ch = 0;
					loop_state = 0;

					for(k = 0; k < RelayModuleConfig[i].loop_num; k ++)
					{
						for(m = 0; m < MAX_RELAY_MODULE_LOOP_GROUP_NUM; m ++)
						{
							for(j = 0; j < group_num; j ++)
							{
								if(RelayModuleConfig[i].loop_group[k][m] == group[j])
								{
									loop_ch |= (1 << k);

									if(on_off == 1)
									{
										loop_state |= (1 << k);
									}
									else if(on_off == 0)
									{
										loop_state &= ~(1 << k);
									}
									
									m = MAX_RELAY_MODULE_LOOP_GROUP_NUM;
									
									break;
								}
							}
						}
					}

					if(loop_ch != 0)
					{
						loop_ch &= RelayModuleConfig[i].loop_enable;
						loop_state &= RelayModuleConfig[i].loop_enable;

						RelayModuleState[i].loop_current_channel = loop_ch;
						RelayModuleState[i].loop_current_state = loop_state;
						RelayModuleState[i].controller = 1;
						memset(RelayModuleState[i].control_time,0,15);
						TimeToString(RelayModuleState[i].control_time,
						             calendar.w_year,
						             calendar.w_month,
						             calendar.w_date,
						             calendar.hour,
						             calendar.min,
						             calendar.sec);
						
						RelayModuleState[i].execute_immediately = 1;
					}
				}
			}
		}
	}

	return ret;
}

//状态查询
u8 RelayGetCurrentState(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 m = 0;
	u8 n = 0;
	u8 ret = 0;
	u8 module_num = 0;
	u8 add = 0;
	u8 ch = 0;
	char tmp[10] = {0};
	char *buf = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)mymalloc(sizeof(ServerFrameStruct_S));

	if(server_frame_struct->para_num % 2 == 0)
	{
		if(resp_server_frame_struct != NULL)				//参数个数是2的倍数
		{
			CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

			resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
			resp_server_frame_struct->msg_len 	= 10;
			resp_server_frame_struct->err_code 	= (u8)NO_ERR;

			ret = ConvertFrameStructToFrame(resp_server_frame_struct);

			buf = (char *)mymalloc(400 * sizeof(char));

			if(buf != NULL)
			{
				module_num = server_frame_struct->para_num / 2;

				for(k = 0; k < module_num; k ++)
				{
					for(j = 0; j < RelayModuleConfigNum.number; j ++)
					{
						add = myatoi((char *)server_frame_struct->para[k * 2 + 0].value);
						ch = myatoi((char *)server_frame_struct->para[k * 2 + 1].value);

						if(add == RelayModuleState[j].address && ch == RelayModuleState[j].channel)
						{
							ServerFrameStruct_S *state_server_frame_struct = NULL;		//用于响应服务器

							state_server_frame_struct = (ServerFrameStruct_S *)mymalloc(sizeof(ServerFrameStruct_S));

							if(state_server_frame_struct != NULL)
							{
								CopyServerFrameStruct(server_frame_struct,state_server_frame_struct,0);

								state_server_frame_struct->msg_type = (u8)SYNC_RESPONSE;	//响应服务器类型
								state_server_frame_struct->msg_len 	= 10;
								state_server_frame_struct->err_code = (u8)NO_ERR;

								state_server_frame_struct->para_num = 7;

								state_server_frame_struct->para = (Parameter_S *)mymalloc(state_server_frame_struct->para_num * sizeof(Parameter_S));

								if(state_server_frame_struct->para != NULL)
								{
									i = 0;

									state_server_frame_struct->para[i].type = 0x3001;
									memset(buf,0,25);
									sprintf(buf, "%x",RelayModuleState[j].address);
									state_server_frame_struct->para[i].len = strlen(buf);
									state_server_frame_struct->para[i].value = (u8 *)mymalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
									if(state_server_frame_struct->para[i].value != NULL)
									{
										memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
									}
									i ++;

									state_server_frame_struct->para[i].type = 0x4002;
									memset(buf,0,25);
									sprintf(buf, "%d",RelayModuleState[j].channel);
									state_server_frame_struct->para[i].len = strlen(buf);
									state_server_frame_struct->para[i].value = (u8 *)mymalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
									if(state_server_frame_struct->para[i].value != NULL)
									{
										memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
									}
									i ++;

									state_server_frame_struct->para[i].type = 0x3003;
									memset(buf,0,25);
									sprintf(buf, "%04x",RelayModuleState[j].loop_channel_bit);
									state_server_frame_struct->para[i].len = strlen(buf);
									state_server_frame_struct->para[i].value = (u8 *)mymalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
									if(state_server_frame_struct->para[i].value != NULL)
									{
										memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
									}
									i ++;

									state_server_frame_struct->para[i].type = 0x3004;
									memset(buf,0,25);
									sprintf(buf, "%04x",RelayModuleState[j].loop_current_state);
									state_server_frame_struct->para[i].len = strlen(buf);
									state_server_frame_struct->para[i].value = (u8 *)mymalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
									if(state_server_frame_struct->para[i].value != NULL)
									{
										memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
									}
									i ++;

									state_server_frame_struct->para[i].type = 0x8005;
									memset(buf,0,25);
									sprintf(buf, "%d",RelayModuleState[j].controller);
									state_server_frame_struct->para[i].len = strlen(buf);
									state_server_frame_struct->para[i].value = (u8 *)mymalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
									if(state_server_frame_struct->para[i].value != NULL)
									{
										memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
									}
									i ++;

									state_server_frame_struct->para[i].type = 0xA006;
									memset(buf,0,25);
									memcpy(buf,RelayModuleState[j].control_time,14);
									state_server_frame_struct->para[i].len = strlen(buf);
									state_server_frame_struct->para[i].value = (u8 *)mymalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
									if(state_server_frame_struct->para[i].value != NULL)
									{
										memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
									}
									i ++;

									state_server_frame_struct->para[i].type = 0x4007;
									if(RelayModuleState[j].abnormal_loop == 0)
									{
										state_server_frame_struct->para[i].len = 0;
										memset(buf,0,2);
									}
									else
									{
										memset(buf,0,400);

										for(m = 0; m < MAX_RELAY_MODULE_LOOP_CH_NUM; m ++)
										{
											if(RelayModuleState[j].abnormal_loop & (1 << m))
											{
												for(n = 0; n < MAX_RELAY_MODULE_ALARM_TYPE_NUM; n ++)
												{
													if(RelayModuleState[j].loop_alarm[m][n] != 0)
													{
														memset(tmp,0,10);
														sprintf(tmp, "%d",RelayModuleState[j].loop_alarm[m][n]);
														strcat(buf,tmp);
														strcat(buf,",");
													}
												}

												buf[strlen(buf) - 1] = '\0';
											}
											else
											{
												memset(tmp,0,10);
												sprintf(tmp, "%d",0);
												strcat(buf,tmp);
											}

											if(m < MAX_RELAY_MODULE_LOOP_CH_NUM - 1)
											{
												strcat(buf,"|");
											}
										}
									}
									state_server_frame_struct->para[i].len = strlen(buf);
									state_server_frame_struct->para[i].value = (u8 *)mymalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
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

				myfree(buf);
			}
		}
	}

	return ret;
}

//模式切换
u8 RelaySetStrategyGroupSwitch(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 group_num = 0;
	char tmp[10];
	char *msg = NULL;
	RelayStrategyGroupSwitch_S strategy_group_switch;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	memset(&strategy_group_switch,0,sizeof(RelayStrategyGroupSwitch_S));

	for(j = 0; j < server_frame_struct->para_num; j ++)
	{
		switch(server_frame_struct->para[j].type)
		{
			case 0x4001:
				if(server_frame_struct->para[j].len != 0)
				{
					msg = (char *)server_frame_struct->para[j].value;

					while(*msg != '\0')
					{
						if(*(msg ++) == ',')
						{
							group_num ++;
						}
					}

					group_num ++;

					strategy_group_switch.group_num = group_num;

					msg = (char *)server_frame_struct->para[j].value;

					for(k = 0; k < group_num; k ++)
					{
						while(*msg != ',' && *msg != '\0')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						strategy_group_switch.group_id[k] = myatoi(tmp);
					}
				}
			break;

			case 0x8002:
				strategy_group_switch.type = myatoi((char *)server_frame_struct->para[j].value);
			break;

			case 0xA003:
				if(server_frame_struct->para[j].len == 14)
				{
					memcpy(strategy_group_switch.time,server_frame_struct->para[j].value,14);
				}
			break;

			default:
			break;
		}
	}

	if(strategy_group_switch.type == 3)
	{
		memcpy(&RelayStrategyGroupSwitchTemp,&strategy_group_switch,sizeof(RelayStrategyGroupSwitch_S));
	}
	else if(strategy_group_switch.type == 1 ||
		    strategy_group_switch.type == 2)
	{
		memcpy(&RelayStrategyGroupSwitch,&strategy_group_switch,sizeof(RelayStrategyGroupSwitch_S));

		WriteRelayStrategyGroupSwitch(0,1);
		
		RelayRefreshStrategyGroup = 1;		//置位策略刷新标志
	}

	resp_server_frame_struct = (ServerFrameStruct_S *)mymalloc(sizeof(ServerFrameStruct_S));

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

//设置告警配置参数
u8 RelaySetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct)
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
				RelayAlarmConfig.relay_abnormal_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
			break;

			case 0x4002:

			break;

			case 0x6003:
				RelayAlarmConfig.contact_abnormal_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
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

						for(m = 0; m < RelayModuleConfigNum.number; m ++)
						{
							if(add == RelayModuleConfig[m].address &&
							   ch == RelayModuleConfig[m].channel)
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
						
						if(loop_ch < MAX_RELAY_MODULE_LOOP_CH_NUM)
						{
							if(loop_ch == 0)
							{
								loop_ch = 1;
							}
							RelayModuleConfig[m].loop_alarm_thre[loop_ch - 1][0] = loop_ch;

							while(*msg != ',')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							RelayModuleConfig[m].loop_alarm_thre[loop_ch - 1][1] = myatoi(tmp);

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
							RelayModuleConfig[m].loop_alarm_thre[loop_ch - 1][2] = add;

							while(*msg != ',')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							RelayModuleConfig[m].loop_alarm_thre[loop_ch - 1][3] = myatoi(tmp);

							while(*msg != ',' && *msg != '|' && *msg != '\0')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							RelayModuleConfig[m].loop_alarm_thre[loop_ch - 1][4] = myatoi(tmp);
						}
					}

					for(k = 0; k < RelayModuleConfigNum.number; k ++)
					{
						WriteRelayModuleConfig(k,0,1);
					}
				}
			break;

			case 0x6005:
				RelayAlarmConfig.task_abnormal_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
			break;

			case 0x4006:

			break;

			default:
			break;
		}
	}

	resp_server_frame_struct = (ServerFrameStruct_S *)mymalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	WriteRelayAlarmConfig(0,1);

	return ret;
}

//获取告警配置参数
u8 RelayGetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 j = 0;
	u8 m = 0;
	u8 ret = 0;
	char tmp[10] = {0};
	char *buf = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)mymalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		buf = (char *)mymalloc(1100 * sizeof(char));

		if(buf != NULL)
		{
			CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

			resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
			resp_server_frame_struct->msg_len 	= 10;
			resp_server_frame_struct->err_code 	= (u8)NO_ERR;
			resp_server_frame_struct->para_num 	= 6;

			resp_server_frame_struct->para = (Parameter_S *)mymalloc(resp_server_frame_struct->para_num * sizeof(Parameter_S));

			if(resp_server_frame_struct->para != NULL)
			{
				resp_server_frame_struct->para[i].type = 0x6101;
				memset(buf,0,2);
				sprintf(buf, "%d",RelayAlarmConfig.relay_abnormal_alarm_enable);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)mymalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x4102;
				memset(buf,0,2);
				resp_server_frame_struct->para[i].len = 0;
				resp_server_frame_struct->para[i].value = (u8 *)mymalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x6103;
				memset(buf,0,2);
				sprintf(buf, "%d",RelayAlarmConfig.contact_abnormal_alarm_enable);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)mymalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x4104;
				memset(buf,0,1100);
				memset(tmp,0,10);

				for(m = 0; m < RelayModuleConfigNum.number; m ++)
				{
					for(j = 0; j < RelayModuleConfig[m].loop_num; j ++)
					{
						if(RelayModuleConfig[m].loop_alarm_thre[j][0] == j + 1)
						{
							memset(tmp,0,10);
							sprintf(tmp, "%x",RelayModuleConfig[m].address);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",RelayModuleConfig[m].channel);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",RelayModuleConfig[m].loop_alarm_thre[j][0]);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",RelayModuleConfig[m].loop_alarm_thre[j][1]);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%x",RelayModuleConfig[m].loop_alarm_thre[j][2]);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",RelayModuleConfig[m].loop_alarm_thre[j][3]);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",RelayModuleConfig[m].loop_alarm_thre[j][4]);
							strcat(buf,tmp);
							strcat(buf,"|");
						}
					}
				}

				buf[strlen(buf) - 1] = 0;	//去掉最后一个'|'
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)mymalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x6105;
				memset(buf,0,25);
				sprintf(buf, "%d",RelayAlarmConfig.task_abnormal_alarm_enable);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)mymalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x4106;
				memset(buf,0,2);
				resp_server_frame_struct->para[i].len = 0;
				resp_server_frame_struct->para[i].value = (u8 *)mymalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;
			}

			ret = ConvertFrameStructToFrame(resp_server_frame_struct);

			myfree(buf);
		}
		else
		{
			DeleteServerFrameStruct(resp_server_frame_struct);
		}
	}

	return ret;
}

//告警历史查询
u8 RelayGetAlarmReportHistory(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u16 record_num = 0;
	char buf[10];

	EventHistory_S *event_history = NULL;
	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	event_history = (EventHistory_S *)mymalloc(sizeof(EventHistory_S));

	if(event_history != NULL)
	{
		event_history->device_type = (u8)RELAY;
		event_history->event_type = (u8)TYPE_ALARM;

		for(j = 0; j < server_frame_struct->para_num; j ++)
		{
			switch(server_frame_struct->para[j].type)
			{
				case 0xA001:
					event_history->start_date = (u8 *)mymalloc((server_frame_struct->para[j].len + 1) * sizeof(u8));

					if(event_history->start_date != NULL)
					{
						memcpy(event_history->start_date,server_frame_struct->para[j].value,server_frame_struct->para[j].len + 1);
					}
				break;

				case 0xA002:
					event_history->end_date = (u8 *)mymalloc((server_frame_struct->para[j].len + 1) * sizeof(u8));

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

	resp_server_frame_struct = (ServerFrameStruct_S *)mymalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;
		resp_server_frame_struct->para_num  = 2;

		resp_server_frame_struct->para = (Parameter_S *)mymalloc(resp_server_frame_struct->para_num * sizeof(Parameter_S));

		if(resp_server_frame_struct->para != NULL)
		{
			resp_server_frame_struct->para[i].type = 0x8101;
			memset(buf,0,10);
			sprintf(buf, "%d",record_num);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)mymalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
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
			resp_server_frame_struct->para[i].value = (u8 *)mymalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
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
u8 RelaySetBasicConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u16 time = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 m = 0;
	u8 n = 0;
	u8 add = 0;
	u8 ch = 0;
	u8 en = 0;
	u8 loop_ch = 0;
	u8 seg_num = 0;
	u8 group_num = 0;
	u16 pos = 0;
	char tmp[17];
	char *msg = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	for(j = 0; j < server_frame_struct->para_num; j ++)
	{
		switch(server_frame_struct->para[j].type)
		{
			case 0x4001:
				msg = (char *)server_frame_struct->para[j].value;
				while(*msg != ',')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				time = myatoi(tmp);
				DefaultSwitchTime.on_hour = time / 100;
				DefaultSwitchTime.on_minute = time % 100;
				while(*msg != '\0')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 2;
				time = myatoi(tmp);
				DefaultSwitchTime.off_hour = time / 100;
				DefaultSwitchTime.off_minute = time % 100;

				WriteDefaultSwitchTime(0,1);
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

					if(seg_num > MAX_RELAY_MODULE_CONF_NUM)
					{
						seg_num = MAX_RELAY_MODULE_CONF_NUM;
					}

					RelayModuleConfigNum.number = seg_num;

					WriteRelayModuleConfigNum(0,1);
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
					StrToHex(&RelayModuleConfig[k].address,tmp,1);
					i = 0;

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					RelayModuleConfig[k].channel = myatoi(tmp);

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					RelayModuleConfig[k].loop_num = myatoi(tmp);

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					RelayModuleConfig[k].interval_time = myatoi(tmp);

					while(*msg != ',' && *msg != '|' && *msg != '\0')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					msg = msg + 1;
					memset(RelayModuleConfig[k].module,0,MAX_RELAY_MODULE_MODULE_NAME_LEN);

					if(i < MAX_RELAY_MODULE_MODULE_NAME_LEN)
					{
						memcpy(RelayModuleConfig[k].module,tmp,i);
					}

					i = 0;

					WriteRelayModuleConfig(k,0,1);
				}
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
					StrToHex(&add,tmp,1);
					i = 0;

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					ch = myatoi(tmp);

					for(m = 0; m < RelayModuleConfigNum.number; m ++)
					{
						if(add == RelayModuleConfig[m].address &&
						   ch == RelayModuleConfig[m].channel)
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

					while(*msg != ',' && *msg != '\0' && *msg != '|')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					en = myatoi(tmp);

					if(en == 1)
					{
						RelayModuleConfig[m].loop_enable |= (1 << (loop_ch - 1));
					}
					else if(en == 0)
					{
						RelayModuleConfig[m].loop_enable &= ~(1 << (loop_ch - 1));
					}
					
					group_num = 0;
					pos = 0;
					if(*(msg - 1) != '\0' && *(msg - 1) != '|')
					{
						while(*msg != '\0' && *msg != '|')
						{
							if(*(msg ++) == ',')
							{
								group_num ++;
							}
							
							pos ++;
						}

						group_num ++;

						msg -= pos;
					}

					for(n = 0; n < group_num; n ++)
					{
						while(*msg != ',' && *msg != '\0' && *msg != '|')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						RelayModuleConfig[m].loop_group[loop_ch - 1][n] = myatoi(tmp);
					}

					WriteRelayModuleConfig(m,0,1);
				}
			break;

			default:
			break;
		}
	}

	resp_server_frame_struct = (ServerFrameStruct_S *)mymalloc(sizeof(ServerFrameStruct_S));

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
u8 RelayGetBasicConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 m = 0;
	u8 en = 0;
	u8 ret = 0;
	u16 time = 0;
	u8 group_num = 0;
	char tmp[10] = {0};
	char *buf = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)mymalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		buf = (char *)mymalloc(3100 * sizeof(char));

		if(buf != NULL)
		{
			CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

			resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
			resp_server_frame_struct->msg_len 	= 10;
			resp_server_frame_struct->err_code 	= (u8)NO_ERR;
			resp_server_frame_struct->para_num 	= 3;

			resp_server_frame_struct->para = (Parameter_S *)mymalloc(resp_server_frame_struct->para_num * sizeof(Parameter_S));

			if(resp_server_frame_struct->para != NULL)
			{
				resp_server_frame_struct->para[i].type = 0x4101;
				memset(buf,0,3100);
				memset(tmp,0,10);
				time = (((u16)DefaultSwitchTime.on_hour) * 100) +
					   ((u16)DefaultSwitchTime.on_minute);

				Int4BitToString((u8 *)buf,time);

				strcat(buf,",");

				memset(tmp,0,10);
				time = (((u16)DefaultSwitchTime.off_hour) * 100) +
					   ((u16)DefaultSwitchTime.off_minute);

				Int4BitToString((u8 *)tmp,time);
				strcat(buf,tmp);

				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)mymalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x4102;
				memset(buf,0,3100);
				memset(tmp,0,10);

				for(m = 0; m < RelayModuleConfigNum.number; m ++)
				{
					memset(tmp,0,10);
					sprintf(tmp, "%x",RelayModuleConfig[m].address);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",RelayModuleConfig[m].channel);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",RelayModuleConfig[m].loop_num);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",RelayModuleConfig[m].interval_time);
					strcat(buf,tmp);
					strcat(buf,",");

					strcat(buf,(char *)RelayModuleConfig[m].module);

					if(m < RelayModuleConfigNum.number - 1)
					{
						strcat(buf,"|");
					}
				}

				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)mymalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x4103;
				memset(buf,0,3100);
				memset(tmp,0,10);

				for(m = 0; m < RelayModuleConfigNum.number; m ++)
				{
					for(j = 0; j < RelayModuleConfig[m].loop_num; j ++)
					{
						memset(tmp,0,10);
						sprintf(tmp, "%x",RelayModuleConfig[m].address);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,10);
						sprintf(tmp, "%d",RelayModuleConfig[m].channel);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,10);
						sprintf(tmp, "%d",j + 1);
						strcat(buf,tmp);
						strcat(buf,",");

						if(RelayModuleConfig[m].loop_enable & (1 << j))
						{
							en = 1;
						}
						else
						{
							en = 0;
						}
						memset(tmp,0,10);
						sprintf(tmp, "%d",en);
						strcat(buf,tmp);
						
						for(group_num = 0; group_num < MAX_RELAY_MODULE_LOOP_GROUP_NUM; group_num ++)
						{
							if(RelayModuleConfig[m].loop_group[j][group_num] == 0)
							{
								break;
							}
						}
						
						if(group_num != 0)
						{
							strcat(buf,",");
						}
						else
						{
							strcat(buf,"|");
						}

						for(k = 0; k < group_num; k ++)
						{
							if(RelayModuleConfig[m].loop_group[j][k] != 0)
							{
								memset(tmp,0,10);
								sprintf(tmp, "%d",RelayModuleConfig[m].loop_group[j][k]);
								strcat(buf,tmp);

								if(k == group_num - 1)
								{
									strcat(buf,"|");
								}
								else
								{
									if(RelayModuleConfig[m].loop_group[j][k + 1] != 0)
									{
										strcat(buf,",");
									}
									else
									{
										if(j < RelayModuleConfig[m].loop_num - 1)
										{
											strcat(buf,"|");
										}
									}
								}
							}
							else
							{
								break;
							}
						}
					}
				}

				buf[strlen(buf) - 1] = 0;
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)mymalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;
			}

			ret = ConvertFrameStructToFrame(resp_server_frame_struct);

			myfree(buf);
		}
		else
		{
			DeleteServerFrameStruct(resp_server_frame_struct);
		}

	}

	return ret;
}

//预约控制配置
u8 RelaySetRelayAppointment(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u16 pos = 0;
	u8 para_num = 0;
	u32 time = 0;
	u8 week = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 seg_num = 0;
	char tmp[16];
	char *msg = NULL;
	RelaySenceConfig_S relay_sence_config;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	para_num = server_frame_struct->para_num;

	if(para_num > MAX_RELAY_MODULE_APPOINTMENT_NUM)
	{
		para_num = MAX_RELAY_MODULE_APPOINTMENT_NUM;
	}

	RelayAppointmentNum.number = para_num;

	WriteRelayAppointmentNum(0,1);

	for(j = 0; j < para_num; j ++)
	{
		memset(&relay_sence_config,0,sizeof(RelaySenceConfig_S));
		
		msg = (char *)server_frame_struct->para[j].value;

		if(server_frame_struct->para[j].len != 0)
		{
			while(*msg != ',')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			relay_sence_config.group_id = myatoi(tmp);

			while(*msg != '|')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			relay_sence_config.priority = myatoi(tmp);

			seg_num = 0;
			pos = 0;

			while(*msg != '\0')
			{
				if(*(msg ++) == '|')
				{
					seg_num ++;
				}
				
				pos ++;
			}

			seg_num ++;

			if(seg_num > MAX_RELAY_MODULE_APPOINTMENT_TIME_RANGE_NUM)
			{
				seg_num = MAX_RELAY_MODULE_APPOINTMENT_TIME_RANGE_NUM;
			}

			relay_sence_config.time_range_num = seg_num;

			msg -= pos;
			
			for(k = 0; k < seg_num; k ++)
			{
				while(*msg != ',')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				time = myatoi(tmp);
				relay_sence_config.range[k].s_month = time / 1000000;
				relay_sence_config.range[k].s_date = (time / 10000) % 100;
				relay_sence_config.range[k].s_hour = (time / 100) % 100;
				relay_sence_config.range[k].s_minute = time % 100;

				while(*msg != ',')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				time = myatoi(tmp);
				relay_sence_config.range[k].e_month = time / 1000000;
				relay_sence_config.range[k].e_date = (time / 10000) % 100;
				relay_sence_config.range[k].e_hour = (time / 100) % 100;
				relay_sence_config.range[k].e_minute = time % 100;

				while(*msg != ',' && *msg != '|' && *msg != '\0')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				msg = msg + 1;
				if(i == 1)
				{
					tmp[1] = tmp[0];
					tmp[0] = '0';
				}
				StrToHex(&week,tmp,1);
				i = 0;
				relay_sence_config.range[k].week_enable = week;
			}

			WriteRelayAppointment(j,&relay_sence_config,0,1);
		}
	}

	resp_server_frame_struct = (ServerFrameStruct_S *)mymalloc(sizeof(ServerFrameStruct_S));

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

//读取预约控制配置
u8 RelayGetRelayAppointment(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 j = 0;
	u8 ret = 0;
	char tmp[10] = {0};
	char *buf = NULL;
	RelaySenceConfig_S appointment;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)mymalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		buf = (char *)mymalloc(220 * sizeof(char));

		if(buf != NULL)
		{
			CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

			resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
			resp_server_frame_struct->msg_len 	= 10;
			resp_server_frame_struct->err_code 	= (u8)NO_ERR;
			resp_server_frame_struct->para_num 	= RelayAppointmentNum.number;

			resp_server_frame_struct->para = (Parameter_S *)mymalloc(resp_server_frame_struct->para_num * sizeof(Parameter_S));

			if(resp_server_frame_struct->para != NULL)
			{
				for(i = 0; i < resp_server_frame_struct->para_num; i ++)
				{
					memset(buf,0,220);
					memset(&appointment,0,sizeof(RelaySenceConfig_S));

					ret = ReadRelayAppointment(i,&appointment);

					if(ret == 1)
					{
						resp_server_frame_struct->para[i].type = 0x4101 + i;

						memset(tmp,0,10);
						sprintf(tmp, "%d",appointment.group_id);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,10);
						sprintf(tmp, "%d",appointment.priority);
						strcat(buf,tmp);
						strcat(buf,"|");

						for(j = 0; j < appointment.time_range_num; j ++)
						{
							if(appointment.range[j].s_month  == 0 &&
							   appointment.range[j].s_date   == 0 &&
							   appointment.range[j].s_hour   == 0 &&
							   appointment.range[j].s_minute == 0 &&
							   appointment.range[j].e_month  == 0 &&
							   appointment.range[j].e_date   == 0 &&
							   appointment.range[j].e_hour   == 0 &&
							   appointment.range[j].e_minute == 0)
							{
								strcat(buf,"0,0");
							}
							else
							{
								memset(tmp,0,10);
								sprintf(tmp, "%02d",appointment.range[j].s_month);
								strcat(buf,tmp);
								memset(tmp,0,10);
								sprintf(tmp, "%02d",appointment.range[j].s_date);
								strcat(buf,tmp);
								memset(tmp,0,10);
								sprintf(tmp, "%02d",appointment.range[j].s_hour);
								strcat(buf,tmp);
								memset(tmp,0,10);
								sprintf(tmp, "%02d",appointment.range[j].s_minute);
								strcat(buf,tmp);
								strcat(buf,",");
								memset(tmp,0,10);
								sprintf(tmp, "%02d",appointment.range[j].e_month);
								strcat(buf,tmp);
								memset(tmp,0,10);
								sprintf(tmp, "%02d",appointment.range[j].e_date);
								strcat(buf,tmp);
								memset(tmp,0,10);
								sprintf(tmp, "%02d",appointment.range[j].e_hour);
								strcat(buf,tmp);
								memset(tmp,0,10);
								sprintf(tmp, "%02d",appointment.range[j].e_minute);
								strcat(buf,tmp);
							}
							strcat(buf,",");
							memset(tmp,0,10);
							sprintf(tmp, "%02x",appointment.range[j].week_enable);
							strcat(buf,tmp);
							if(j < appointment.time_range_num - 1)
							{
								strcat(buf,"|");
							}
						}
					}

					resp_server_frame_struct->para[i].len = strlen(buf);
					resp_server_frame_struct->para[i].value = (u8 *)mymalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
					if(resp_server_frame_struct->para[i].value != NULL)
					{
						memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
					}
				}
			}

			ret = ConvertFrameStructToFrame(resp_server_frame_struct);

			myfree(buf);
		}
		else
		{
			DeleteServerFrameStruct(resp_server_frame_struct);
		}
	}

	return ret;
}

//策略配置
u8 RelaySetRelayStrategy(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u16 pos = 0;
	u8 seg_num = 0;
	u8 i = 0;
	u16 j = 0;
	u8 k = 0;
	u16 loop_ch = 0;
	u16 loop_action = 0;
	u16 strategy_num = 0;
	u8 add = 0;
	u8 ch = 0;
	u8 tmp_h = 0;
	u8 tmp_l = 0;
	char tmp[16];
	char *msg = NULL;
	RelayTask_S strategy;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	strategy_num = server_frame_struct->para_num;

	if(strategy_num > MAX_RELAY_MODULE_STRATEGY_NUM)
	{
		strategy_num = MAX_RELAY_MODULE_STRATEGY_NUM;
	}

	RelayStrategyNum.number = strategy_num;

	WriteRelayStrategyNum(0,1);

	for(j = 0; j < strategy_num; j ++)
	{
		memset(&strategy,0,sizeof(RelayTask_S));
		
		msg = (char *)server_frame_struct->para[j].value;

		if(server_frame_struct->para[j].len != 0)
		{
			while(*msg != ',')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			strategy.group_id = myatoi(tmp);

			while(*msg != ',')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			strategy.type = myatoi(tmp);

			while(*msg != ',')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			strategy.time = myatoi(tmp);

			while(*msg != '|')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			strategy.time_option = myatoi(tmp);

			seg_num = 0;
			pos = 0;
			
			while(*msg != '\0')
			{
				if(*(msg ++) == '|')
				{
					seg_num ++;
				}
				
				pos ++;
			}

			seg_num ++;

			if(seg_num > MAX_RELAY_MODULE_CONF_NUM)
			{
				seg_num = MAX_RELAY_MODULE_CONF_NUM;
			}

			strategy.action_num = seg_num;

			msg -= pos;
			
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
				StrToHex(&add,tmp,1);
				i = 0;
				strategy.action[k].module_address = add;

				while(*msg != ',')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				ch = myatoi(tmp);
				strategy.action[k].module_channel = ch;
				
				while(*msg != ',')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				msg = msg + 1;
				if(i == 1 || i == 2)
				{
					if(i == 1)
					{
						tmp[1] = tmp[0];
						tmp[0] = '0';
					}

					StrToHex((u8 *)&loop_ch,tmp,1);
				}
				else if(i == 3 || i == 4)
				{
					if(i == 3)
					{
						tmp[3] = tmp[2];
						tmp[2] = tmp[1];
						tmp[1] = tmp[0];
						tmp[0] = '0';
					}

					StrToHex(&tmp_h,tmp + 0,1);
					StrToHex(&tmp_l,tmp + 2,1);

					loop_ch = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
				}
				strategy.action[k].loop_channel = loop_ch;
				i = 0;

				while(*msg != ',' && *msg != '|' && *msg != '\0')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				msg = msg + 1;
				if(i == 1 || i == 2)
				{
					if(i == 1)
					{
						tmp[1] = tmp[0];
						tmp[0] = '0';
					}

					StrToHex((u8 *)&loop_action,tmp,1);
				}
				else if(i == 3 || i == 4)
				{
					if(i == 3)
					{
						tmp[3] = tmp[2];
						tmp[2] = tmp[1];
						tmp[1] = tmp[0];
						tmp[0] = '0';
					}

					StrToHex(&tmp_h,tmp + 0,1);
					StrToHex(&tmp_l,tmp + 2,1);

					loop_action = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
				}
				strategy.action[k].loop_action = loop_action;
				i = 0;
			}

			WriteRelayStrategy(j,&strategy,0,1);
		}
	}

	resp_server_frame_struct = (ServerFrameStruct_S *)mymalloc(sizeof(ServerFrameStruct_S));

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

//读取策略配置
u8 RelayGetRelayStrategy(ServerFrameStruct_S *server_frame_struct)
{
	u16 i = 0;
	u16 j = 0;
	u8 ret = 0;
	char tmp[10] = {0};
	char buf[110] = {0};
	RelayTask_S strategy;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)mymalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;
		resp_server_frame_struct->para_num 	= RelayStrategyNum.number;

		resp_server_frame_struct->para = (Parameter_S *)mymalloc(resp_server_frame_struct->para_num * sizeof(Parameter_S));

		if(resp_server_frame_struct->para != NULL)
		{
			for(i = 0; i < resp_server_frame_struct->para_num; i ++)
			{
				memset(buf,0,110);
				memset(&strategy,0,sizeof(RelayTask_S));

				ret = ReadRelayStrategy(i,&strategy);

				if(ret == 1)
				{
					resp_server_frame_struct->para[i].type = 0x4101 + i;

					memset(tmp,0,10);
					sprintf(tmp, "%d",strategy.group_id);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",strategy.type);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",strategy.time);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",strategy.time_option);
					strcat(buf,tmp);
					strcat(buf,"|");

					for(j = 0; j < strategy.action_num; j ++)
					{
						memset(tmp,0,10);
						sprintf(tmp, "%x",strategy.action[j].module_address);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,10);
						sprintf(tmp, "%d",strategy.action[j].module_channel);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,10);
						sprintf(tmp, "%04x",strategy.action[j].loop_channel);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,10);
						sprintf(tmp, "%04x",strategy.action[j].loop_action);
						strcat(buf,tmp);
						if(j < strategy.action_num - 1)
						{
							strcat(buf,"|");
						}
					}
				}

				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)mymalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
			}
		}

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

















































