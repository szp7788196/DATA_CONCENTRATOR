#include "electricity_meter_comm.h"
#include "concentrator_comm.h"
#include "common.h"
#include "e_meter.h"

u8 ElectricityMeterStateChangesReportResponse = 0;


void ElectricityMeterSendStateChangesReportFrameToServer(ElectricityMeterState_S *module_state)
{
	u8 i = 0;
	u8 m = 0;
	char tmp[25] = {0};
	char *buf = NULL;
	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

	buf = (char *)pvPortMalloc(500 * sizeof(char));

	if(buf != NULL)
	{
		server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

		if(server_frame_struct != NULL && module_state != NULL)
		{
			InitServerFrameStruct(server_frame_struct);

			server_frame_struct->msg_type 	= (u8)DEVICE_REQUEST_UP;	//响应服务器类型
			server_frame_struct->msg_len 	= 10;
			server_frame_struct->err_code 	= (u8)NO_ERR;
			server_frame_struct->msg_id		= 0x0471;
			server_frame_struct->para_num	= module_state->ch_num + 1 + 3;

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

				server_frame_struct->para[i].type = 0x8003;
				memset(buf,0,25);
				sprintf(buf, "%d",module_state->ch_num);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				for(m = 0; m < module_state->ch_num + 1; m ++)
				{
					server_frame_struct->para[i].type = 0x8004 + m;
					memset(buf,0,500);

					memset(tmp,0,25);
					sprintf(tmp, "%0.2f",module_state->current_para[m].voltage);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,25);
					sprintf(tmp, "%0.3f",module_state->current_para[m].current);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,25);
					sprintf(tmp, "%0.3f",module_state->current_para[m].active_power);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,25);
					sprintf(tmp, "%0.2f",module_state->current_para[m].active_energy);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,25);
					sprintf(tmp, "%0.2f",module_state->current_para[m].reactive_energy);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,25);
					sprintf(tmp, "%0.2f",module_state->current_para[m].power_factor);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,25);
					sprintf(tmp, "%0.2f",module_state->current_para[m].frequency);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,25);
					sprintf(tmp, "%0.2f",module_state->current_para[m].line_voltage);
					strcat(buf,tmp);

					server_frame_struct->para[i].len = strlen(buf);
					server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
					if(server_frame_struct->para[i].value != NULL)
					{
						memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
					}
					i ++;
				}
			}

			ConvertFrameStructToFrame(server_frame_struct);
		}

		vPortFree(buf);
	}
}

//向服务器发送状态变化事件
void ElectricityMeterSendStateChangesReportToServer(void)
{
	BaseType_t xResult;
	static ElectricityMeterState_S *module_state = NULL;
	static time_t time_5 = 0;
	static u8 retry_times5 = 0;

	if(ElectricityMeterStateChangesReportResponse == 0)
	{
		if(module_state != NULL)
		{
			vPortFree(module_state);
			module_state = NULL;
		}

		if(module_state == NULL)
		{
			xResult = xQueueReceive(xQueue_ElectricityMeterState,(void *)&module_state,(TickType_t)pdMS_TO_TICKS(1));

			if(xResult == pdPASS)
			{
				ElectricityMeterStateChangesReportResponse = 1;

				time_5 = GetSysTick1s();
				retry_times5 = 0;

				RE_SEND_STATE_CHANGES_REPORT:
				ElectricityMeterSendStateChangesReportFrameToServer(module_state);
			}
		}
	}

	if(ElectricityMeterStateChangesReportResponse == 1)
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

				ElectricityMeterStateChangesReportResponse = 0;

				vPortFree(module_state);
				module_state = NULL;
			}
		}
	}
}




//处理接收到的报文
void ElectricityMeterRecvAndHandleFrameStruct(void)
{
	BaseType_t xResult;
	ServerFrameStruct_S *server_frame_struct = NULL;

	xResult = xQueueReceive(xQueue_ElectricityMeterFrameStruct,(void *)&server_frame_struct,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS )
	{
		switch(server_frame_struct->msg_id)
		{
			case 0x0000:	//数据透传

			break;

			case 0x0470:	//状态查询
				ElectricityMeterGetCurrentState(server_frame_struct);
			break;

			case 0x0471:	//状态上报
				ElectricityMeterStateChangesReportResponse = 0;
			break;

			case 0x0472:	//状态历史查询

			break;

			case 0x04A0:	//告警上报
				AlarmReportResponse = 0;
			break;

			case 0x04A1:	//告警解除
				AlarmReportResponse = 0;
			break;

			case 0x04A2:	//事件上报

			break;

			case 0x04A3:	//告警配置
				ElectricityMeterSetAlarmConfiguration(server_frame_struct);
			break;

			case 0x04A4:	//告警配置查询
				ElectricityMeterGetAlarmConfiguration(server_frame_struct);
			break;

			case 0x04A5:	//告警历史查询
				ElectricityMeterGetAlarmReportHistory(server_frame_struct);
			break;

			case 0x04D0:	//基础配置
				ElectricityMeterSetBasicConfiguration(server_frame_struct);
			break;

			case 0x04D1:	//查询基础配置
				ElectricityMeterGetBasicConfiguration(server_frame_struct);
			break;

			default:
			break;
		}

		DeleteServerFrameStruct(server_frame_struct);
	}
}

//状态查询
u8 ElectricityMeterGetCurrentState(ServerFrameStruct_S *server_frame_struct)
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
	char *msg = NULL;
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

			buf = (char *)pvPortMalloc(500 * sizeof(char));

			if(buf != NULL)
			{
				module_num = server_frame_struct->para_num / 2;

				for(k = 0; k < module_num; k ++)
				{
					for(j = 0; j < ElectricityMeterConfigNum.number; j ++)
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

						if(add == ElectricityMeterState[j].address && ch == ElectricityMeterState[j].channel)
						{
							ServerFrameStruct_S *state_server_frame_struct = NULL;		//用于响应服务器

							state_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

							if(state_server_frame_struct != NULL)
							{
								CopyServerFrameStruct(server_frame_struct,state_server_frame_struct,0);

								state_server_frame_struct->msg_type = (u8)SYNC_RESPONSE;	//响应服务器类型
								state_server_frame_struct->msg_len 	= 10;
								state_server_frame_struct->err_code = (u8)NO_ERR;

								state_server_frame_struct->para_num = ElectricityMeterState[j].ch_num + 1 + 3;

								state_server_frame_struct->para = (Parameter_S *)pvPortMalloc(state_server_frame_struct->para_num * sizeof(Parameter_S));

								if(state_server_frame_struct->para != NULL)
								{
									i = 0;

									state_server_frame_struct->para[i].type = 0x3001;
									memset(buf,0,25);
									sprintf(buf, "%x",ElectricityMeterState[j].address);
									state_server_frame_struct->para[i].len = strlen(buf);
									state_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
									if(state_server_frame_struct->para[i].value != NULL)
									{
										memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
									}
									i ++;

									state_server_frame_struct->para[i].type = 0x4002;
									memset(buf,0,25);
									sprintf(buf, "%d",ElectricityMeterState[j].channel);
									state_server_frame_struct->para[i].len = strlen(buf);
									state_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
									if(state_server_frame_struct->para[i].value != NULL)
									{
										memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
									}
									i ++;

									state_server_frame_struct->para[i].type = 0x8003;
									memset(buf,0,25);
									sprintf(buf, "%d",ElectricityMeterState[j].ch_num);
									state_server_frame_struct->para[i].len = strlen(buf);
									state_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
									if(state_server_frame_struct->para[i].value != NULL)
									{
										memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
									}
									i ++;

									for(m = 0; m < ElectricityMeterState[j].ch_num + 1; m ++)
									{
										state_server_frame_struct->para[i].type = 0x8004 + m;
										memset(buf,0,500);

										memset(tmp,0,25);
										sprintf(tmp, "%0.2f",ElectricityMeterState[j].current_para[m].voltage);
										strcat(buf,tmp);
										strcat(buf,",");

										memset(tmp,0,25);
										sprintf(tmp, "%0.3f",ElectricityMeterState[j].current_para[m].current);
										strcat(buf,tmp);
										strcat(buf,",");

										memset(tmp,0,25);
										sprintf(tmp, "%0.3f",ElectricityMeterState[j].current_para[m].active_power);
										strcat(buf,tmp);
										strcat(buf,",");

										memset(tmp,0,25);
										sprintf(tmp, "%0.2f",ElectricityMeterState[j].current_para[m].active_energy);
										strcat(buf,tmp);
										strcat(buf,",");

										memset(tmp,0,25);
										sprintf(tmp, "%0.2f",ElectricityMeterState[j].current_para[m].reactive_energy);
										strcat(buf,tmp);
										strcat(buf,",");

										memset(tmp,0,25);
										sprintf(tmp, "%0.2f",ElectricityMeterState[j].current_para[m].power_factor);
										strcat(buf,tmp);
										strcat(buf,",");

										memset(tmp,0,25);
										sprintf(tmp, "%0.2f",ElectricityMeterState[j].current_para[m].frequency);
										strcat(buf,tmp);
										strcat(buf,",");

										memset(tmp,0,25);
										sprintf(tmp, "%0.2f",ElectricityMeterState[j].current_para[m].line_voltage);
										strcat(buf,tmp);

										state_server_frame_struct->para[i].len = strlen(buf);
										state_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((state_server_frame_struct->para[i].len + 1) * sizeof(u8));
										if(state_server_frame_struct->para[i].value != NULL)
										{
											memcpy(state_server_frame_struct->para[i].value,buf,state_server_frame_struct->para[i].len + 1);
										}
										i ++;
									}
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
u8 ElectricityMeterSetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct)
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
	u8 para_id = 0;
	u8 seg_num = 0;
	char tmp[10];
	char *msg = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	for(j = 0; j < server_frame_struct->para_num; j ++)
	{
		switch(server_frame_struct->para[j].type)
		{
			case 0x6001:
				ElectricityMeterAlarmConfig.electrical_parameters_thre_over_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
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

						for(m = 0; m < ElectricityMeterConfigNum.number; m ++)
						{
							if(add == ElectricityMeterConfig[m].address &&
							   ch == ElectricityMeterConfig[m].channel)
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

						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						para_id = myatoi(tmp);

						if(loop_ch < MAX_ELECTRICITY_METER_CH_NUM && para_id < MAX_ELECTRICITY_METER_ALARM_PARA_NUM)
						{
//							if(loop_ch == 0)
//							{
//								loop_ch = 1;
//							}

							if(para_id == 0)
							{
								para_id = 1;
							}

							ElectricityMeterConfig[m].alarm_thre[loop_ch][para_id - 1].channel = loop_ch;
							ElectricityMeterConfig[m].alarm_thre[loop_ch][para_id - 1].para_id = para_id;

							while(*msg != ',')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							if(tmp[0] == 'N' && tmp[1] == 'A')
							{
								memset((void *)&ElectricityMeterConfig[m].alarm_thre[loop_ch][para_id - 1].min_value,0xFF,8);
							}
							else
							{
								ElectricityMeterConfig[m].alarm_thre[loop_ch][para_id - 1].min_value = atof(tmp);
							}

							while(*msg != ',')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							if(tmp[0] == 'N' && tmp[1] == 'A')
							{
								memset((void *)&ElectricityMeterConfig[m].alarm_thre[loop_ch][para_id - 1].max_value,0xFF,8);
							}
							else
							{
								ElectricityMeterConfig[m].alarm_thre[loop_ch][para_id - 1].max_value = atof(tmp);
							}

							while(*msg != ',')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							ElectricityMeterConfig[m].alarm_thre[loop_ch][para_id - 1].confirm_time = myatoi(tmp);

							while(*msg != ',')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							ElectricityMeterConfig[m].alarm_thre[loop_ch][para_id - 1].switch_run_mode = myatoi(tmp);

							while(*msg != ',')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							ElectricityMeterConfig[m].alarm_thre[loop_ch][para_id - 1].resume_run_mode = myatoi(tmp);

							while(*msg != ',' && *msg != '|' && *msg != '\0')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							ElectricityMeterConfig[m].alarm_thre[loop_ch][para_id - 1].relay_action = myatoi(tmp);
						}
					}

					for(k = 0; k < ElectricityMeterConfigNum.number; k ++)
					{
						WriteElectricityMeterConfig(k,0,1);
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

	WriteElectricityMeterAlarmConfig(0,1);

	return ret;
}

//获取告警配置参数
u8 ElectricityMeterGetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 k = 0;
	u8 m = 0;
	u8 n = 0;
	u8 ret = 0;
	long long thre = {0};
	char tmp[25] = {0};
	char *buf = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		buf = (char *)pvPortMalloc(4096 * sizeof(char));

		if(buf != NULL)
		{
			CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

			resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
			resp_server_frame_struct->msg_len 	= 10;
			resp_server_frame_struct->err_code 	= (u8)NO_ERR;
			resp_server_frame_struct->para_num 	= 2;

			resp_server_frame_struct->para = (Parameter_S *)pvPortMalloc(resp_server_frame_struct->para_num * sizeof(Parameter_S));

			if(resp_server_frame_struct->para != NULL)
			{
				resp_server_frame_struct->para[i].type = 0x6101;
				memset(buf,0,2);
				sprintf(buf, "%d",ElectricityMeterAlarmConfig.electrical_parameters_thre_over_alarm_enable);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x4102;
				memset(buf,0,4096);
				memset(tmp,0,25);

				for(m = 0; m < ElectricityMeterConfigNum.number; m ++)
				{
					for(k = 0; k < ElectricityMeterConfig[m].ch_num; k ++)
					{
						for(n = 0; n < MAX_ELECTRICITY_METER_ALARM_PARA_NUM; n ++)
						{
							if(ElectricityMeterConfig[m].alarm_thre[k][n].channel < MAX_ELECTRICITY_METER_CH_NUM &&
							   ElectricityMeterConfig[m].alarm_thre[k][n].para_id != 0)
							{
								memset(tmp,0,10);
								sprintf(tmp, "%x",ElectricityMeterConfig[m].address);
								strcat(buf,tmp);
								strcat(buf,",");

								memset(tmp,0,10);
								sprintf(tmp, "%d",ElectricityMeterConfig[m].channel);
								strcat(buf,tmp);
								strcat(buf,",");

								memset(tmp,0,10);
								sprintf(tmp, "%d",ElectricityMeterConfig[m].alarm_thre[k][n].channel);
								strcat(buf,tmp);
								strcat(buf,",");

								memset(tmp,0,10);
								sprintf(tmp, "%d",ElectricityMeterConfig[m].alarm_thre[k][n].para_id);
								strcat(buf,tmp);
								strcat(buf,",");

								memset(tmp,0,25);
								memcpy(&thre,(void *)&ElectricityMeterConfig[m].alarm_thre[k][n].min_value,4);
								if(thre == 0xFFFFFFFFFFFFFFFF)
								{
									strcat(buf,"NA,");
								}
								else
								{
									sprintf(tmp, "%f",ElectricityMeterConfig[m].alarm_thre[k][n].min_value);
									strcat(buf,tmp);
									strcat(buf,",");
								}

								memset(tmp,0,25);
								memcpy(&thre,(void *)&ElectricityMeterConfig[m].alarm_thre[k][n].max_value,4);
								if(thre == 0xFFFFFFFFFFFFFFFF)
								{
									strcat(buf,"NA,");
								}
								else
								{
									sprintf(tmp, "%f",ElectricityMeterConfig[m].alarm_thre[k][n].max_value);
									strcat(buf,tmp);
									strcat(buf,",");
								}

								memset(tmp,0,10);
								sprintf(tmp, "%d",ElectricityMeterConfig[m].alarm_thre[k][n].confirm_time);
								strcat(buf,tmp);
								strcat(buf,",");

								memset(tmp,0,10);
								sprintf(tmp, "%d",ElectricityMeterConfig[m].alarm_thre[k][n].switch_run_mode);
								strcat(buf,tmp);
								strcat(buf,",");

								memset(tmp,0,10);
								sprintf(tmp, "%d",ElectricityMeterConfig[m].alarm_thre[k][n].resume_run_mode);
								strcat(buf,tmp);
								strcat(buf,",");

								memset(tmp,0,10);
								sprintf(tmp, "%d",ElectricityMeterConfig[m].alarm_thre[k][n].relay_action);
								strcat(buf,tmp);
								strcat(buf,"|");
							}
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
u8 ElectricityMeterGetAlarmReportHistory(ServerFrameStruct_S *server_frame_struct)
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
		event_history->device_type = (u8)ELECTRICITY_METER;
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
u8 ElectricityMeterSetBasicConfiguration(ServerFrameStruct_S *server_frame_struct)
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
				ElectricityMeterBasicConfig.detect_interval = myatoi((char *)server_frame_struct->para[j].value);

				WriteElectricityMeterBasicConfig(0,1);
			break;

			case 0x6002:
				ElectricityMeterBasicConfig.auto_report = myatoi((char *)server_frame_struct->para[j].value);

				WriteElectricityMeterBasicConfig(0,1);
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

					if(seg_num > MAX_ELECTRICITY_METER_CONF_NUM)
					{
						seg_num = MAX_ELECTRICITY_METER_CONF_NUM;
					}

					ElectricityMeterConfigNum.number = seg_num;

					WriteElectricityMeterConfigNum(0,1);
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
					StrToHex(&ElectricityMeterConfig[k].address,tmp,1);
					i = 0;

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					ElectricityMeterConfig[k].channel = myatoi(tmp);

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					ElectricityMeterConfig[k].ch_num = myatoi(tmp);

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					ElectricityMeterConfig[k].voltage_ratio = atof(tmp);

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					ElectricityMeterConfig[k].current_ratio = atof(tmp);

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					ElectricityMeterConfig[k].voltage_range_of_change = atof(tmp);

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					ElectricityMeterConfig[k].current_range_of_change = atof(tmp);

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					ElectricityMeterConfig[k].pf_range_of_change = atof(tmp);

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					ElectricityMeterConfig[k].confirm_time = myatoi(tmp);

					while(*msg != ',' && *msg != '|' && *msg != '\0')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					msg = msg + 1;
					memset(ElectricityMeterConfig[k].module,0,MAX_ELECTRICITY_METER_MODULE_NAME_LEN);

					if(i < MAX_ELECTRICITY_METER_MODULE_NAME_LEN)
					{
						memcpy(ElectricityMeterConfig[k].module,tmp,i);
					}

					i = 0;

					WriteElectricityMeterConfig(k,0,1);
					
					if(ElectricityMeterConfig[k].address == 0 && ElectricityMeterConfig[k].channel == 0)
					{
						BulitInMeterRatio.voltage_ratio = ElectricityMeterConfig[k].voltage_ratio;
						BulitInMeterRatio.current_ratio = ElectricityMeterConfig[k].current_ratio;
						
						WriteBulitInMeterRatio(0,1);
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

	return ret;
}

//读取基本配置
u8 ElectricityMeterGetBasicConfiguration(ServerFrameStruct_S *server_frame_struct)
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
		buf = (char *)pvPortMalloc(500 * sizeof(char));

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
				sprintf(buf, "%d",ElectricityMeterBasicConfig.detect_interval);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x6102;
				memset(buf,0,10);
				sprintf(buf, "%d",ElectricityMeterBasicConfig.auto_report);
				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;

				resp_server_frame_struct->para[i].type = 0x4103;
				memset(buf,0,500);
				memset(tmp,0,10);

				for(m = 0; m < ElectricityMeterConfigNum.number; m ++)
				{
					memset(tmp,0,10);
					sprintf(tmp, "%x",ElectricityMeterConfig[m].address);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",ElectricityMeterConfig[m].channel);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",ElectricityMeterConfig[m].ch_num);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%f",ElectricityMeterConfig[m].voltage_ratio);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%f",ElectricityMeterConfig[m].current_ratio);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%f",ElectricityMeterConfig[m].voltage_range_of_change);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%f",ElectricityMeterConfig[m].current_range_of_change);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%f",ElectricityMeterConfig[m].pf_range_of_change);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",ElectricityMeterConfig[m].confirm_time);
					strcat(buf,tmp);
					strcat(buf,",");

					strcat(buf,(char *)ElectricityMeterConfig[m].module);

					if(m < ElectricityMeterConfigNum.number - 1)
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



































