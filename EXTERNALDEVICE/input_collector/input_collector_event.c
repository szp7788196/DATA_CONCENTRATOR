#include "input_collector_event.h"
#include "input_collector_conf.h"
#include "concentrator_conf.h"
#include "relay_conf.h"
#include "history_record.h"
#include "rx8010s.h"


void InputCollectorEventCheckPolling(void)
{
	InputCollectorDetectD_QuantityStateChanges();
	InputCollectorDetectA_QuantityStateChanges();
	InputCollectorD_QuantityAbnormal();
	InputCollectorA_OverQuantityAbnormal();
	InputCollectorA_UnderQuantityAbnormal();
}


//检测数字量变化
void InputCollectorDetectD_QuantityStateChanges(void)
{
	u8 i = 0;

	if(InputCollectorBasicConfig.auto_report == 1)
	{
		for(i = 0; i < InputCollectorConfigNum.number; i ++)
		{
			if(InputCollectorState[i].d_mirror_state != InputCollectorState[i].d_current_state)
			{
				InputCollectorState_S *module_state = NULL;

				InputCollectorState[i].d_mirror_state = InputCollectorState[i].d_current_state;

				module_state = (InputCollectorState_S *)pvPortMalloc(sizeof(InputCollectorState_S));

				if(module_state != NULL)
				{
					memcpy(module_state,&InputCollectorState[i],sizeof(InputCollectorState_S));

					if(xQueueSend(xQueue_InputCollectorState,(void *)&module_state,(TickType_t)10) != pdPASS)
					{
#ifdef DEBUG_LOG
						printf("send xQueue_InputCollectorState fail.\r\n");
#endif
						vPortFree(module_state);
					}
				}
			}
		}
	}
}

//检测模拟量变化
void InputCollectorDetectA_QuantityStateChanges(void)
{
	u8 i = 0;
	u8 j = 0;
	static time_t time_s = 0;
	static u8 cnt[MAX_INPUT_COLLECTOR_CONF_NUM][MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM] = {0};
	static u8 occur[MAX_INPUT_COLLECTOR_CONF_NUM] = {0};

	if(InputCollectorBasicConfig.auto_report == 1)
	{
		if(GetSysTick10ms() - time_s >= (InputCollectorBasicConfig.detect_interval / 10))
		{
			time_s = GetSysTick10ms();

			for(i = 0; i < InputCollectorConfigNum.number; i ++)
			{
				for(j = 0; j < InputCollectorConfig[i].a_loop_num; j ++)
				{
					if(InputCollectorConfig[i].a_quantity_range != 0.0f)
					{
						if(abs(InputCollectorState[i].a_current_state[j] - InputCollectorState[i].a_mirror_state[j]) >= InputCollectorConfig[i].a_quantity_range)
						{
							if(cnt[i][j] < InputCollectorConfig[i].confirm_time)
							{
								cnt[i][j] ++;
							}

							if(cnt[i][j] == InputCollectorConfig[i].confirm_time)									//低电压持续约30秒
							{
								InputCollectorState[i].a_current_state[j] = InputCollectorState[i].a_mirror_state[j];
								
								occur[i] = 1;
							}
						}
						else
						{
							cnt[i][j] = 0;
							occur[i] = 0;
						}
					}
					else
					{
						cnt[i][j] = 0;
						occur[i] = 0;
					}
				}

				if(occur[i] == 1)
				{
					InputCollectorState_S *module_state = NULL;

					occur[i] = 0;

					module_state = (InputCollectorState_S *)pvPortMalloc(sizeof(InputCollectorState_S));

					if(module_state != NULL)
					{
						memcpy(module_state,&InputCollectorState[i],sizeof(InputCollectorState_S));

						if(xQueueSend(xQueue_InputCollectorState,(void *)&module_state,(TickType_t)10) != pdPASS)
						{
#ifdef DEBUG_LOG
							printf("send xQueue_InputCollectorState fail.\r\n");
#endif
							vPortFree(module_state);
						}
					}
				}
			}
		}
	}
}

//检测数字量异常告警
void InputCollectorD_QuantityAbnormal(void)
{
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u16 para_len = 0;
	u8 state = 0;
	char tmp[10] = {0};
//	char *buf = NULL;
	static time_t time_s = 0;
	static s8 cnt[MAX_INPUT_COLLECTOR_CONF_NUM][MAX_INPUT_COLLECTOR_D_LOOP_CH_NUM] = {0};
	static u8 occur[MAX_INPUT_COLLECTOR_CONF_NUM][MAX_INPUT_COLLECTOR_D_LOOP_CH_NUM] = {0};
	static u8 record[MAX_INPUT_COLLECTOR_CONF_NUM] = {0};

	if(InputCollectorAlarmConfig.d_quantity_abnormal_alarm_enable == 1)
	{
		if(GetSysTick10ms() - time_s >= (InputCollectorBasicConfig.detect_interval / 10))
		{
			time_s = GetSysTick10ms();

			for(i = 0; i < InputCollectorConfigNum.number; i ++)
			{
				for(j = 0; j < InputCollectorConfig[i].d_loop_num; j ++)
				{
					if(InputCollectorState[i].d_current_state & (1 << j))
					{
						state = 1;
					}
					else
					{
						state = 0;
					}
					
					if(occur[i][j] == 0)
					{
						if(state == InputCollectorConfig[i].d_alarm_thre[j].alarm_level)
						{
							cnt[i][j] ++;

							if(cnt[i][j] == InputCollectorConfig[i].d_alarm_thre[j].confirm_time)
							{
								cnt[i][j] = InputCollectorConfig[i].d_alarm_thre[j].confirm_time;

								occur[i][j] = 1;

								record[i] = 1;
								
								InputCollectorState[i].d_abnormal_loop |= (1 << j);
								
								if(InputCollectorConfig[i].d_alarm_thre[j].switch_run_mode != 0)
								{
									InputCollectorConfig[i].d_alarm_thre[j].last_run_mode = (u8)RunMode;
									RunMode = (RUN_MODE_E)InputCollectorConfig[i].d_alarm_thre[j].switch_run_mode;
								}
								
								if(InputCollectorConfig[i].d_alarm_thre[j].relay_action == 1)
								{
									RelayForceSwitchOffAllRelays = 1;
								}
							}
						}
						else
						{
							cnt[i][j] = 0;
						}
					}
					else
					{
						if(state != InputCollectorConfig[i].d_alarm_thre[j].alarm_level)
						{
							cnt[i][j] --;

							if(cnt[i][j] == (0 - InputCollectorConfig[i].d_alarm_thre[j].confirm_time))
							{
								cnt[i][j] = (0 - InputCollectorConfig[i].d_alarm_thre[j].confirm_time);

								occur[i][j] = 1;

								record[i] = 1;
								
								InputCollectorState[i].d_abnormal_loop &= ~(1 << j);
								
								if(InputCollectorConfig[i].d_alarm_thre[j].switch_run_mode != 0)
								{
									if(InputCollectorConfig[i].d_alarm_thre[j].resume_run_mode == 1)
									{
										RunMode = (RUN_MODE_E)InputCollectorConfig[i].d_alarm_thre[j].last_run_mode;
									}
								}
							}
						}
						else
						{
							cnt[i][j] = 0;
						}
					}
				}

				if(record[i] == 1)
				{
					char *buf = NULL;
					AlarmReport_S *alarm_report = NULL;

					record[i] = 0;

					alarm_report = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));

					if(alarm_report != NULL)
					{
						buf = (char *)pvPortMalloc(400 * sizeof(char));

						if(buf != NULL)
						{
							if(InputCollectorState[i].d_abnormal_loop != 0)
							{
								alarm_report->record_type = 1;
							}
							else if(InputCollectorState[i].d_abnormal_loop == 0)
							{
								alarm_report->record_type = 0;
							}
							
							alarm_report->device_type = (u8)INPUT_COLLECTOR;
							alarm_report->alarm_type = INPUT_COLLECTOR_D_ABNORMAL;
							
							memset(buf,0,25);
							sprintf(buf, "%d",InputCollectorState[i].address);
							para_len = strlen(buf);
							alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->device_address != NULL)
							{
								memcpy(alarm_report->device_address,buf,para_len + 1);
							}
							
							memset(buf,0,25);
							sprintf(buf, "%d",InputCollectorState[i].channel);
							para_len = strlen(buf);
							alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->device_channel != NULL)
							{
								memcpy(alarm_report->device_channel,buf,para_len + 1);
							}
							
							memset(buf,0,25);
							sprintf(buf, "%04x",InputCollectorState[i].d_abnormal_loop);
							para_len = strlen(buf);
							alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->current_value != NULL)
							{
								memcpy(alarm_report->current_value,buf,para_len + 1);
							}
							
							memset(buf,0,400);
							for(k = 0; k < InputCollectorConfig[i].d_loop_num; k ++)
							{
								if(InputCollectorConfig[i].d_alarm_thre[k].channel == k + 1 &&
								  (InputCollectorState[i].d_abnormal_loop & (1 << k)) != 0)
								{
									memset(tmp,0,10);
									sprintf(tmp, "%x",InputCollectorConfig[i].address);
									strcat(buf,tmp);
									strcat(buf,",");

									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].channel);
									strcat(buf,tmp);
									strcat(buf,",");

									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].d_alarm_thre[k].channel);
									strcat(buf,tmp);
									strcat(buf,",");

									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].d_alarm_thre[k].alarm_level);
									strcat(buf,tmp);
									strcat(buf,",");

									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].d_alarm_thre[k].confirm_time);
									strcat(buf,tmp);
									strcat(buf,",");
									
									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].d_alarm_thre[k].switch_run_mode);
									strcat(buf,tmp);
									strcat(buf,",");
									
									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].d_alarm_thre[k].resume_run_mode);
									strcat(buf,tmp);
									strcat(buf,",");

									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].d_alarm_thre[k].relay_action);
									strcat(buf,tmp);
									strcat(buf,"|");
								}
							}
							buf[strlen(buf) - 1] = 0;
							para_len = strlen(buf);
							alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->set_value != NULL)
							{
								memcpy(alarm_report->set_value,buf,para_len + 1);
							}
							
							memset(buf,0,25);
							sprintf(buf, "%04x",InputCollectorState[i].d_current_state);
							para_len = strlen(buf);
							alarm_report->reference_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->reference_value != NULL)
							{
								memcpy(alarm_report->reference_value,buf,para_len + 1);
							}
							
							alarm_report->occur_time[14] = 0;
							TimeToString(alarm_report->occur_time,
										 calendar.w_year,
										 calendar.w_month,
										 calendar.w_date,
										 calendar.hour,
										 calendar.min,calendar.sec);

							PushAlarmReportToAlarmQueue(alarm_report);
							
							vPortFree(buf);
						}
						else
						{
							DeleteAlarmReport(alarm_report);
						}
					}
				}
			}
		}
	}
}

//检测模拟量异常告警 最大值越线
void InputCollectorA_OverQuantityAbnormal(void)
{
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u16 para_len = 0;
	long long max_value = 0;
	char tmp[25] = {0};
//	char *buf = NULL;
	static time_t time_s = 0;
	static s8 cnt[MAX_INPUT_COLLECTOR_CONF_NUM][MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM] = {0};
	static u8 occur[MAX_INPUT_COLLECTOR_CONF_NUM][MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM] = {0};
	static u8 record[MAX_INPUT_COLLECTOR_CONF_NUM] = {0};

	if(InputCollectorAlarmConfig.a_quantity_abnormal_alarm_enable == 1)
	{
		if(GetSysTick10ms() - time_s >= (InputCollectorBasicConfig.detect_interval / 10))
		{
			time_s = GetSysTick10ms();

			for(i = 0; i < InputCollectorConfigNum.number; i ++)
			{
				for(j = 0; j < InputCollectorConfig[i].a_loop_num; j ++)
				{
					memcpy(&max_value,(void *)&InputCollectorConfig[i].a_alarm_thre[j].max_value,8);
					
					if(max_value != 0xFFFFFFFFFFFFFFFF && InputCollectorConfig[i].a_alarm_thre[j].max_value != 0.0f)
					{
						if(occur[i][j] == 0)
						{
							if(InputCollectorState[i].a_current_state[j] >= InputCollectorConfig[i].a_alarm_thre[j].max_value)
							{
								cnt[i][j] ++;

								if(cnt[i][j] == InputCollectorConfig[i].a_alarm_thre[j].confirm_time)
								{
									cnt[i][j] = InputCollectorConfig[i].a_alarm_thre[j].confirm_time;

									occur[i][j] = 1;

									record[i] = 1;
									
									InputCollectorState[i].a_abnormal_loop |= (1 << j);
									
									if(InputCollectorConfig[i].a_alarm_thre[j].switch_run_mode != 0)
									{
										InputCollectorConfig[i].a_alarm_thre[j].last_run_mode = (u8)RunMode;
										RunMode = (RUN_MODE_E)InputCollectorConfig[i].a_alarm_thre[j].switch_run_mode;
									}
									
									if(InputCollectorConfig[i].a_alarm_thre[j].relay_action == 1)
									{
										RelayForceSwitchOffAllRelays = 1;
									}
								}
							}
							else
							{
								cnt[i][j] = 0;
							}
						}
						else
						{
							if((InputCollectorConfig[i].a_alarm_thre[j].max_value - InputCollectorState[i].a_current_state[j]) / 100.0f >= 0.05f)
							{
								cnt[i][j] --;

								if(cnt[i][j] == (0 - InputCollectorConfig[i].a_alarm_thre[j].confirm_time))
								{
									cnt[i][j] = (0 - InputCollectorConfig[i].a_alarm_thre[j].confirm_time);

									occur[i][j] = 1;

									record[i] = 1;
									
									InputCollectorState[i].a_abnormal_loop &= ~(1 << j);
									
									if(InputCollectorConfig[i].a_alarm_thre[j].switch_run_mode != 0)
									{
										if(InputCollectorConfig[i].a_alarm_thre[j].resume_run_mode == 1)
										{
											RunMode = (RUN_MODE_E)InputCollectorConfig[i].a_alarm_thre[j].last_run_mode;
										}
									}
								}
							}
							else
							{
								cnt[i][j] = 0;
							}
						}
					}
				}

				if(record[i] == 1)
				{
					char *buf = NULL;
					AlarmReport_S *alarm_report = NULL;

					record[i] = 0;

					alarm_report = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));

					if(alarm_report != NULL)
					{
						buf = (char *)pvPortMalloc(400 * sizeof(char));

						if(buf != NULL)
						{
							if(InputCollectorState[i].a_abnormal_loop != 0)
							{
								alarm_report->record_type = 1;
							}
							else if(InputCollectorState[i].a_abnormal_loop == 0)
							{
								alarm_report->record_type = 0;
							}
							
							alarm_report->device_type = (u8)INPUT_COLLECTOR;
							alarm_report->alarm_type = INPUT_COLLECTOR_D_ABNORMAL;
							
							memset(buf,0,25);
							sprintf(buf, "%d",InputCollectorState[i].address);
							para_len = strlen(buf);
							alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->device_address != NULL)
							{
								memcpy(alarm_report->device_address,buf,para_len + 1);
							}
							
							memset(buf,0,25);
							sprintf(buf, "%d",InputCollectorState[i].channel);
							para_len = strlen(buf);
							alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->device_channel != NULL)
							{
								memcpy(alarm_report->device_channel,buf,para_len + 1);
							}
							
							memset(buf,0,25);
							sprintf(buf, "%04x",InputCollectorState[i].a_abnormal_loop);
							para_len = strlen(buf);
							alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->current_value != NULL)
							{
								memcpy(alarm_report->current_value,buf,para_len + 1);
							}
							
							memset(buf,0,400);
							for(k = 0; k < InputCollectorConfig[i].a_loop_num; k ++)
							{
								if(InputCollectorConfig[i].a_alarm_thre[k].channel == k + 1 &&
								  (InputCollectorState[i].d_abnormal_loop & (1 << k)) != 0)
								{
									memset(tmp,0,10);
									sprintf(tmp, "%x",InputCollectorConfig[i].address);
									strcat(buf,tmp);
									strcat(buf,",");

									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].channel);
									strcat(buf,tmp);
									strcat(buf,",");

									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].a_alarm_thre[k].channel);
									strcat(buf,tmp);
									strcat(buf,",");
									
									memset(tmp,0,25);
									memcpy(&max_value,(void *)&InputCollectorConfig[i].a_alarm_thre[k].min_value,4);
									if(max_value == 0xFFFFFFFFFFFFFFFF)
									{
										strcat(buf,"NA,");
									}
									else
									{
										sprintf(tmp, "%f",InputCollectorConfig[i].a_alarm_thre[k].min_value);
										strcat(buf,tmp);
										strcat(buf,",");
									}

									memset(tmp,0,25);
									memcpy(&max_value,(void *)&InputCollectorConfig[i].a_alarm_thre[k].max_value,4);
									if(max_value == 0xFFFFFFFFFFFFFFFF)
									{
										strcat(buf,"NA,");
									}
									else
									{
										sprintf(tmp, "%f",InputCollectorConfig[i].a_alarm_thre[k].max_value);
										strcat(buf,tmp);
										strcat(buf,",");
									}

									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].a_alarm_thre[k].confirm_time);
									strcat(buf,tmp);
									strcat(buf,",");
									
									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].a_alarm_thre[k].switch_run_mode);
									strcat(buf,tmp);
									strcat(buf,",");
									
									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].a_alarm_thre[k].resume_run_mode);
									strcat(buf,tmp);
									strcat(buf,",");

									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].a_alarm_thre[k].relay_action);
									strcat(buf,tmp);
									if(k < InputCollectorConfig[i].a_loop_num - 1)
									{
										strcat(buf,"|");
									}
								}
							}
							para_len = strlen(buf);
							alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->set_value != NULL)
							{
								memcpy(alarm_report->set_value,buf,para_len + 1);
							}
							
							memset(buf,0,200);
							for(k = 0; k < InputCollectorConfig[i].a_loop_num; k ++)
							{
								memset(tmp,0,10);
								sprintf(tmp, "%f",InputCollectorState[i].a_current_state[k]);
								strcat(buf,tmp);
								if(k < InputCollectorConfig[i].a_loop_num - 1)
								{
									strcat(buf,",");
								}
							}
							para_len = strlen(buf);
							alarm_report->reference_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->reference_value != NULL)
							{
								memcpy(alarm_report->reference_value,buf,para_len + 1);
							}
							
							alarm_report->occur_time[14] = 0;
							TimeToString(alarm_report->occur_time,
										 calendar.w_year,
										 calendar.w_month,
										 calendar.w_date,
										 calendar.hour,
										 calendar.min,calendar.sec);

							PushAlarmReportToAlarmQueue(alarm_report);
							
							vPortFree(buf);
						}
						else
						{
							DeleteAlarmReport(alarm_report);
						}
					}
				}
			}
		}
	}
}

//检测模拟量异常告警 最小值越线
void InputCollectorA_UnderQuantityAbnormal(void)
{
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u16 para_len = 0;
	long long min_value = 0;
	char tmp[25] = {0};
//	char *buf = NULL;
	static time_t time_s = 0;
	static s8 cnt[MAX_INPUT_COLLECTOR_CONF_NUM][MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM] = {0};
	static u8 occur[MAX_INPUT_COLLECTOR_CONF_NUM][MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM] = {0};
	static u8 record[MAX_INPUT_COLLECTOR_CONF_NUM] = {0};

	if(InputCollectorAlarmConfig.a_quantity_abnormal_alarm_enable == 1)
	{
		if(GetSysTick10ms() - time_s >= (InputCollectorBasicConfig.detect_interval / 10))
		{
			time_s = GetSysTick10ms();

			for(i = 0; i < InputCollectorConfigNum.number; i ++)
			{
				for(j = 0; j < InputCollectorConfig[i].a_loop_num; j ++)
				{
					memcpy(&min_value,(void *)&InputCollectorConfig[i].a_alarm_thre[j].min_value,8);
					
					if(min_value != 0xFFFFFFFFFFFFFFFF && InputCollectorConfig[i].a_alarm_thre[j].min_value != 0.0f)
					{
						if(occur[i][j] == 0)
						{
							if(InputCollectorState[i].a_current_state[j] <= InputCollectorConfig[i].a_alarm_thre[j].min_value)
							{
								cnt[i][j] ++;

								if(cnt[i][j] == InputCollectorConfig[i].a_alarm_thre[j].confirm_time)
								{
									cnt[i][j] = InputCollectorConfig[i].a_alarm_thre[j].confirm_time;

									occur[i][j] = 1;

									record[i] = 1;
									
									InputCollectorState[i].a_abnormal_loop |= (1 << j);
									
									if(InputCollectorConfig[i].a_alarm_thre[j].switch_run_mode != 0)
									{
										InputCollectorConfig[i].a_alarm_thre[j].last_run_mode = (u8)RunMode;
										RunMode = (RUN_MODE_E)InputCollectorConfig[i].a_alarm_thre[j].switch_run_mode;
									}
									
									if(InputCollectorConfig[i].a_alarm_thre[j].relay_action == 1)
									{
										RelayForceSwitchOffAllRelays = 1;
									}
								}
							}
							else
							{
								cnt[i][j] = 0;
							}
						}
						else
						{
							if((InputCollectorState[i].a_current_state[j] - InputCollectorConfig[i].a_alarm_thre[j].min_value) / 100.0f >= 0.05f)
							{
								cnt[i][j] --;

								if(cnt[i][j] == (0 - InputCollectorConfig[i].a_alarm_thre[j].confirm_time))
								{
									cnt[i][j] = (0 - InputCollectorConfig[i].a_alarm_thre[j].confirm_time);

									occur[i][j] = 1;

									record[i] = 1;
									
									InputCollectorState[i].a_abnormal_loop &= ~(1 << j);
									
									if(InputCollectorConfig[i].a_alarm_thre[j].switch_run_mode != 0)
									{
										if(InputCollectorConfig[i].a_alarm_thre[j].resume_run_mode == 1)
										{
											RunMode = (RUN_MODE_E)InputCollectorConfig[i].a_alarm_thre[j].last_run_mode;
										}
									}
								}
							}
							else
							{
								cnt[i][j] = 0;
							}
						}
					}
				}

				if(record[i] == 1)
				{
					char *buf = NULL;
					AlarmReport_S *alarm_report = NULL;

					record[i] = 0;

					alarm_report = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));

					if(alarm_report != NULL)
					{
						buf = (char *)pvPortMalloc(400 * sizeof(char));

						if(buf != NULL)
						{
							if(InputCollectorState[i].a_abnormal_loop != 0)
							{
								alarm_report->record_type = 1;
							}
							else if(InputCollectorState[i].a_abnormal_loop == 0)
							{
								alarm_report->record_type = 0;
							}
							
							alarm_report->device_type = (u8)INPUT_COLLECTOR;
							alarm_report->alarm_type = INPUT_COLLECTOR_D_ABNORMAL;
							
							memset(buf,0,25);
							sprintf(buf, "%d",InputCollectorState[i].address);
							para_len = strlen(buf);
							alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->device_address != NULL)
							{
								memcpy(alarm_report->device_address,buf,para_len + 1);
							}
							
							memset(buf,0,25);
							sprintf(buf, "%d",InputCollectorState[i].channel);
							para_len = strlen(buf);
							alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->device_channel != NULL)
							{
								memcpy(alarm_report->device_channel,buf,para_len + 1);
							}
							
							memset(buf,0,25);
							sprintf(buf, "%04x",InputCollectorState[i].a_abnormal_loop);
							para_len = strlen(buf);
							alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->current_value != NULL)
							{
								memcpy(alarm_report->current_value,buf,para_len + 1);
							}
							
							memset(buf,0,400);
							for(k = 0; k < InputCollectorConfig[i].a_loop_num; k ++)
							{
								if(InputCollectorConfig[i].a_alarm_thre[k].channel == k + 1 &&
								  (InputCollectorState[i].a_abnormal_loop & (1 << k)) != 0)
								{
									memset(tmp,0,10);
									sprintf(tmp, "%x",InputCollectorConfig[i].address);
									strcat(buf,tmp);
									strcat(buf,",");

									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].channel);
									strcat(buf,tmp);
									strcat(buf,",");

									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].a_alarm_thre[k].channel);
									strcat(buf,tmp);
									strcat(buf,",");
									
									memset(tmp,0,25);
									memcpy(&min_value,(void *)&InputCollectorConfig[i].a_alarm_thre[k].min_value,4);
									if(min_value == 0xFFFFFFFFFFFFFFFF)
									{
										strcat(buf,"NA,");
									}
									else
									{
										sprintf(tmp, "%f",InputCollectorConfig[i].a_alarm_thre[k].min_value);
										strcat(buf,tmp);
										strcat(buf,",");
									}

									memset(tmp,0,25);
									memcpy(&min_value,(void *)&InputCollectorConfig[i].a_alarm_thre[k].max_value,4);
									if(min_value == 0xFFFFFFFFFFFFFFFF)
									{
										strcat(buf,"NA,");
									}
									else
									{
										sprintf(tmp, "%f",InputCollectorConfig[i].a_alarm_thre[k].max_value);
										strcat(buf,tmp);
										strcat(buf,",");
									}

									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].a_alarm_thre[k].confirm_time);
									strcat(buf,tmp);
									strcat(buf,",");
									
									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].a_alarm_thre[k].switch_run_mode);
									strcat(buf,tmp);
									strcat(buf,",");
									
									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].a_alarm_thre[k].resume_run_mode);
									strcat(buf,tmp);
									strcat(buf,",");

									memset(tmp,0,10);
									sprintf(tmp, "%d",InputCollectorConfig[i].a_alarm_thre[k].relay_action);
									strcat(buf,tmp);
									if(k < InputCollectorConfig[i].a_loop_num - 1)
									{
										strcat(buf,"|");
									}
								}
							}
							para_len = strlen(buf);
							alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->set_value != NULL)
							{
								memcpy(alarm_report->set_value,buf,para_len + 1);
							}
							
							memset(buf,0,200);
							for(k = 0; k < InputCollectorConfig[i].a_loop_num; k ++)
							{
								memset(tmp,0,10);
								sprintf(tmp, "%f",InputCollectorState[i].a_current_state[k]);
								strcat(buf,tmp);
								if(k < InputCollectorConfig[i].a_loop_num - 1)
								{
									strcat(buf,",");
								}
							}
							para_len = strlen(buf);
							alarm_report->reference_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->reference_value != NULL)
							{
								memcpy(alarm_report->reference_value,buf,para_len + 1);
							}
							
							alarm_report->occur_time[14] = 0;
							TimeToString(alarm_report->occur_time,
										 calendar.w_year,
										 calendar.w_month,
										 calendar.w_date,
										 calendar.hour,
										 calendar.min,calendar.sec);

							PushAlarmReportToAlarmQueue(alarm_report);
							
							vPortFree(buf);
						}
						else
						{
							DeleteAlarmReport(alarm_report);
						}
					}
				}
			}
		}
	}
}




































































