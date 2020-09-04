#include "relay_event.h"
#include "relay_conf.h"
#include "history_record.h"
#include "common.h"
#include "rx8010s.h"
#include "input_collector_conf.h"


void RelayEventCheckPolling(void)
{
	RelayDetectStateChanges();
	RelayAlarmTaskAbnormal();
//	RelayAlarmContactAbnormal();
}

//继电器模块检测状态变化
void RelayDetectStateChanges(void)
{
	u8 i = 0;

	for(i = 0; i < RelayModuleConfigNum.number; i ++)
	{
		if(RelayModuleState[i].loop_mirror_state != RelayModuleState[i].loop_current_state)
		{
			RelayModuleState_S *module_state = NULL;

			RelayModuleState[i].loop_mirror_state = RelayModuleState[i].loop_current_state;

			module_state = (RelayModuleState_S *)pvPortMalloc(sizeof(RelayModuleState_S));

			if(module_state != NULL)
			{
				memcpy(module_state,&RelayModuleState[i],sizeof(RelayModuleState_S));

				if(xQueueSend(xQueue_RelayModuleState,(void *)&module_state,(TickType_t)10) != pdPASS)
				{
#ifdef DEBUG_LOG
					printf("send xQueue_RelayModuleState fail.\r\n");
#endif
					vPortFree(module_state);
				}
			}
		}
	}
}

//继电器任务内状态异常告警
void RelayAlarmTaskAbnormal(void)
{
	static s16 cnt[MAX_RELAY_MODULE_CONF_NUM] = {0};
	u8 i = 0;
	u8 para_len = 0;
	char tmp[10] = {0};
	char buf[25] = {0};
	static u8 occur[MAX_RELAY_MODULE_CONF_NUM] = {0};
	static u8 record[MAX_RELAY_MODULE_CONF_NUM] = {0};

	if(RelayAlarmConfig.task_abnormal_alarm_enable == 1)		//告警以使能
	{
		for(i = 0; i < RelayModuleConfigNum.number; i ++)
		{
			if(occur[i] == 0)											//告警未发生或已解除
			{
				if(RelayModuleState[i].loop_current_state != RelayModuleState[i].loop_task_state)
				{
					cnt[i] ++;

					if(cnt[i] == 10)									//低电压持续约30秒
					{
						cnt[i] = 10;

						occur[i] = 1;									//发生断电告警

						record[i] = 1;
					}
				}
				else
				{
					cnt[i] = 0;
				}
			}
			else
			{
				if(RelayModuleState[i].loop_current_state == RelayModuleState[i].loop_task_state)
				{
					cnt[i] --;

					if(cnt[i] == -10)									//低电压持续约30秒
					{
						cnt[i] = -10;

						occur[i] = 0;									//发生断电告警

						record[i] = 1;
					}
				}
				else
				{
					cnt[i] = 0;
				}
			}

			if(record[i] == 1)
			{
				AlarmReport_S *alarm_report = NULL;

				record[i] = 0;

				alarm_report = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));

				if(alarm_report != NULL)
				{
					if(cnt[i] == 10)
					{
						alarm_report->record_type = 1;
					}
					else if(cnt[i] == -10)
					{
						alarm_report->record_type = 0;
					}
					alarm_report->device_type = (u8)RELAY;
					alarm_report->alarm_type = RELAY_TASK_ABNORMAL;

					memset(buf,0,25);
					sprintf(buf, "%d",RelayModuleState[i].address);
					para_len = strlen(buf);
					alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
					if(alarm_report->device_address != NULL)
					{
						memcpy(alarm_report->device_address,buf,para_len + 1);
					}

					memset(buf,0,25);
					sprintf(buf, "%d",RelayModuleState[i].channel);
					para_len = strlen(buf);
					alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
					if(alarm_report->device_channel != NULL)
					{
						memcpy(alarm_report->device_channel,buf,para_len + 1);
					}

					memset(buf,0,25);
					sprintf(buf, "%04X",RelayModuleState[i].abnormal_loop);
					para_len = strlen(buf);
					alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
					if(alarm_report->current_value != NULL)
					{
						memcpy(alarm_report->current_value,buf,para_len + 1);
					}

					memset(buf,0,25);
					sprintf(buf, "%d",0);
					para_len = strlen(buf);
					alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
					if(alarm_report->set_value != NULL)
					{
						memcpy(alarm_report->set_value,buf,para_len + 1);
					}

					memset(buf,0,25);
					sprintf(tmp, "%04X",RelayModuleState[i].loop_task_state);
					strcat(buf,tmp);
					strcat(buf,",");
					sprintf(tmp, "%04X",RelayModuleState[i].loop_current_state);
					strcat(buf,tmp);
					strcat(buf,",");
					sprintf(tmp, "%d",RelayModuleState[i].controller);
					strcat(buf,tmp);
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
				}
			}
		}
	}
}

//继电器触点状态异常告警
void RelayAlarmContactAbnormal(void)
{
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u16 str_len = 0;
	u16 para_len = 0;
	u8 out_state = 0;
	u8 in_state = 0;
	u8 in_add = 0;
	u8 in_ch = 0;
	u8 in_loop_ch = 0;
	u8 cmp_mode = 0;
	u8 cmpared = 0;
	char tmp[10] = {0};
	char buf[1300];
	static s8 cnt[MAX_RELAY_MODULE_CONF_NUM][MAX_RELAY_MODULE_LOOP_CH_NUM] = {0};
	static u8 occur[MAX_RELAY_MODULE_CONF_NUM][MAX_RELAY_MODULE_LOOP_CH_NUM] = {0};
	static u8 record[MAX_RELAY_MODULE_CONF_NUM] = {0};

	if(RelayAlarmConfig.contact_abnormal_alarm_enable == 1)		//告警以使能
	{
		for(i = 0; i < RelayModuleConfigNum.number; i ++)
		{
			for(j = 0; j < RelayModuleConfig[i].loop_num; j ++)
			{
				if(RelayModuleConfig[i].loop_alarm_thre[j][0] != 0)
				{
					if(RelayModuleState[i].loop_current_state & (1 << j))
					{
						out_state = 1;
					}
					else
					{
						out_state = 0;
					}
					
					cmp_mode = RelayModuleConfig[i].loop_alarm_thre[j][1];
					in_add = RelayModuleConfig[i].loop_alarm_thre[j][2];
					in_ch = RelayModuleConfig[i].loop_alarm_thre[j][3];
					in_loop_ch = RelayModuleConfig[i].loop_alarm_thre[j][4];

					cmpared = 0;

					for(k = 0; k < InputCollectorConfigNum.number; k ++)
					{
						if(in_add == InputCollectorConfig[k].address &&
						   in_ch == InputCollectorConfig[k].channel)
						{
							cmpared = 1;

							break;
						}
					}

					if(cmpared == 1)
					{
						if(InputCollectorState[k].d_current_state & (1 << (in_loop_ch - 1)))
						{
							if(cmp_mode == 0)
							{
								in_state = 1;
							}
							else
							{
								in_state = 0;
							}
						}
						else
						{
							if(cmp_mode == 0)
							{
								in_state = 0;
							}
							else
							{
								in_state = 1;
							}
						}

						if(occur[i][j] == 0)
						{
							if(out_state != in_state)
							{
								cnt[i][j] ++;

								if(cnt[i][j] == 10)
								{
									cnt[i][j] = 10;

									occur[i][j] = 1;

									record[i] = 1;

									RelayModuleState[i].abnormal_loop |= (1 << j);
								}
							}
							else
							{
								cnt[i][j] = 0;
							}
						}
						else
						{
							if(out_state == in_state)
							{
								cnt[i][j] --;

								if(cnt[i][j] == -10)
								{
									cnt[i][j] = -10;

									occur[i][j] = 1;

									record[i] = 1;

									RelayModuleState[i].abnormal_loop &= ~(1 << j);
								}
							}
							else
							{
								cnt[i][j] = 0;
							}
						}
					}
				}
			}

			if(record[i] == 1)
			{
				AlarmReport_S *alarm_report = NULL;

				record[i] = 0;

				alarm_report = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));

				if(alarm_report != NULL)
				{
					if(RelayModuleState[i].abnormal_loop != 0)
					{
						alarm_report->record_type = 1;
					}
					else if(RelayModuleState[i].abnormal_loop == 0)
					{
						alarm_report->record_type = 0;
					}

					alarm_report->device_type = (u8)RELAY;
					alarm_report->alarm_type = RELAY_CONTACT_ABNORMAL;

					memset(buf,0,25);
					sprintf(buf, "%d",RelayModuleState[i].address);
					para_len = strlen(buf);
					alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
					if(alarm_report->device_address != NULL)
					{
						memcpy(alarm_report->device_address,buf,para_len + 1);
					}

					memset(buf,0,25);
					sprintf(buf, "%d",RelayModuleState[i].channel);
					para_len = strlen(buf);
					alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
					if(alarm_report->device_channel != NULL)
					{
						memcpy(alarm_report->device_channel,buf,para_len + 1);
					}

					memset(buf,0,25);
					sprintf(buf, "%04X",RelayModuleState[i].abnormal_loop);
					para_len = strlen(buf);
					alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
					if(alarm_report->current_value != NULL)
					{
						memcpy(alarm_report->current_value,buf,para_len + 1);
					}

					memset(buf,0,1300);
					for(k = 0; k < RelayModuleConfig[i].loop_num; k ++)
					{
						if(RelayModuleConfig[i].loop_alarm_thre[k][0] == k + 1 &&
						  (RelayModuleState[i].abnormal_loop & (1 << k)) != 0)
						{
							memset(tmp,0,10);
							sprintf(tmp, "%X",RelayModuleConfig[i].address);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",RelayModuleConfig[i].channel);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",RelayModuleConfig[i].loop_alarm_thre[k][0]);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",RelayModuleConfig[i].loop_alarm_thre[k][1]);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",RelayModuleConfig[i].loop_alarm_thre[k][2]);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",RelayModuleConfig[i].loop_alarm_thre[k][3]);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",RelayModuleConfig[i].loop_alarm_thre[k][4]);
							strcat(buf,tmp);
							strcat(buf,"|");
						}
					}
					str_len = strlen(buf);
					if(str_len != 0)
					{
						buf[str_len - 1] = 0;
					}
					para_len = strlen(buf);
					alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
					if(alarm_report->set_value != NULL)
					{
						memcpy(alarm_report->set_value,buf,para_len + 1);
					}

					memset(buf,0,25);
					sprintf(buf, "%04X",RelayModuleState[i].loop_current_state);
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
				}
			}
		}
	}
}



































