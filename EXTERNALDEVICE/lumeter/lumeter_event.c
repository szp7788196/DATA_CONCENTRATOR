#include "lumeter_event.h"
#include "lumeter_conf.h"
#include "concentrator_conf.h"
#include "relay_conf.h"
#include "history_record.h"
#include "rx8010s.h"
#include <math.h>


void LumeterEventCheckPolling(void)
{
	LumeterOverValueAbnormal();
	LumeterUnderValueAbnormal();
	LumeterNoResponseAlarm();
	Lumeter24HourUnchangedAlarm();
}


void LumeterOverValueAbnormal(void)
{
	u8 i = 0;
	u16 para_len = 0;
	u32 max_value = 0;
	char tmp[25] = {0};
	static time_t time_s = 0;
	static s8 cnt[MAX_LUMETER_CONF_NUM] = {0};
	static u8 occur[MAX_LUMETER_CONF_NUM] = {0};
	static u8 record[MAX_LUMETER_CONF_NUM] = {0};

	if(LumeterAlarmConfig.lumeter_thre_over_alarm_enable == 1)
	{
		if(GetSysTick10ms() - time_s >= (LumeterBasicConfig.collect_cycle / 10))
		{
			time_s = GetSysTick10ms();

			for(i = 0; i < LumeterConfigNum.number; i ++)
			{
				memcpy(&max_value,(void *)&LumeterConfig[i].max_valid_value,4);

				if(max_value != 0xFFFFFFFF && LumeterConfig[i].max_valid_value != 0)
				{
					if(occur[i] == 0)
					{
						if(LumeterState[i].value >= LumeterConfig[i].max_valid_value)
						{
							cnt[i] ++;

							if(cnt[i] == LumeterConfig[i].valid_value_confirm_time)
							{
								cnt[i] = LumeterConfig[i].valid_value_confirm_time;

								occur[i] = 1;

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
						if((s32)(LumeterConfig[i].max_valid_value - LumeterState[i].value) >= (s32)LumeterConfig[i].max_valid_value_range)
						{
							cnt[i] --;

							if(cnt[i] == (0 - LumeterConfig[i].valid_value_confirm_time))
							{
								cnt[i] = (0 - LumeterConfig[i].valid_value_confirm_time);

								occur[i] = 0;

								record[i] = 1;
							}
						}
						else
						{
							cnt[i] = 0;
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
						buf = (char *)pvPortMalloc(128 * sizeof(char));

						if(buf != NULL)
						{
							if(occur[i] != 0)
							{
								alarm_report->record_type = 1;
							}
							else if(occur[i] == 0)
							{
								alarm_report->record_type = 0;
							}

							alarm_report->device_type = (u8)LUMETER;
							alarm_report->alarm_type = LUMETER_OVER_UNDER_VALUE;

							memset(buf,0,25);
							sprintf(buf, "%X",LumeterState[i].address);
							para_len = strlen(buf);
							alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->device_address != NULL)
							{
								memcpy(alarm_report->device_address,buf,para_len + 1);
							}

							memset(buf,0,25);
							sprintf(buf, "%d",LumeterState[i].channel);
							para_len = strlen(buf);
							alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->device_channel != NULL)
							{
								memcpy(alarm_report->device_channel,buf,para_len + 1);
							}

							memset(buf,0,25);
							sprintf(buf, "%d",LumeterState[i].value);
							para_len = strlen(buf);
							alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->current_value != NULL)
							{
								memcpy(alarm_report->current_value,buf,para_len + 1);
							}

							memset(buf,0,128);

							memset(tmp,0,10);
							sprintf(tmp, "%X",LumeterConfig[i].address);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",LumeterConfig[i].channel);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,25);
							memcpy(&max_value,(void *)&LumeterConfig[i].min_valid_value,4);
							if(max_value == 0xFFFFFFFF)
							{
								strcat(buf,"NA,");
							}
							else
							{
								sprintf(tmp, "%d",LumeterConfig[i].min_valid_value);
								strcat(buf,tmp);
								strcat(buf,",");
							}

							memset(tmp,0,10);
							sprintf(tmp, "%d",LumeterConfig[i].min_valid_value_range);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,25);
							memcpy(&max_value,(void *)&LumeterConfig[i].max_valid_value,4);
							if(max_value == 0xFFFFFFFF)
							{
								strcat(buf,"NA,");
							}
							else
							{
								sprintf(tmp, "%d",LumeterConfig[i].max_valid_value);
								strcat(buf,tmp);
								strcat(buf,",");
							}

							memset(tmp,0,10);
							sprintf(tmp, "%d",LumeterConfig[i].max_valid_value_range);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",LumeterConfig[i].valid_value_confirm_time);
							strcat(buf,tmp);

							para_len = strlen(buf);
							alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->set_value != NULL)
							{
								memcpy(alarm_report->set_value,buf,para_len + 1);
							}

							memset(buf,0,32);
							memset(tmp,0,25);
							memcpy(&max_value,(void *)&LumeterConfig[i].min_valid_value,4);
							if(max_value == 0xFFFFFFFF)
							{
								strcat(buf,"NA,");
							}
							else
							{
								sprintf(tmp, "%d",LumeterConfig[i].min_valid_value);
								strcat(buf,tmp);
								strcat(buf,",");
							}

							memset(tmp,0,25);
							memcpy(&max_value,(void *)&LumeterConfig[i].max_valid_value,4);
							if(max_value == 0xFFFFFFFF)
							{
								strcat(buf,"NA,");
							}
							else
							{
								sprintf(tmp, "%d",LumeterConfig[i].max_valid_value);
								strcat(buf,tmp);
								strcat(buf,",");
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

void LumeterUnderValueAbnormal(void)
{
	u8 i = 0;
	u16 para_len = 0;
	u32 min_value = 0;
	char tmp[25] = {0};
	static time_t time_s = 0;
	static s8 cnt[MAX_LUMETER_CONF_NUM] = {0};
	static u8 occur[MAX_LUMETER_CONF_NUM] = {0};
	static u8 record[MAX_LUMETER_CONF_NUM] = {0};

	if(LumeterAlarmConfig.lumeter_thre_over_alarm_enable == 1)
	{
		if(GetSysTick10ms() - time_s >= (LumeterBasicConfig.collect_cycle / 10))
		{
			time_s = GetSysTick10ms();

			for(i = 0; i < LumeterConfigNum.number; i ++)
			{
				memcpy(&min_value,(void *)&LumeterConfig[i].min_valid_value,4);

				if(min_value != 0xFFFFFFFF && LumeterConfig[i].min_valid_value != 0)
				{
					if(occur[i] == 0)
					{
						if(LumeterState[i].value <= LumeterConfig[i].min_valid_value)
						{
							cnt[i] ++;

							if(cnt[i] == LumeterConfig[i].valid_value_confirm_time)
							{
								cnt[i] = LumeterConfig[i].valid_value_confirm_time;

								occur[i] = 1;

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
						if((s32)(LumeterState[i].value - LumeterConfig[i].max_valid_value) >= (s32)LumeterConfig[i].min_valid_value_range)
						{
							cnt[i] --;

							if(cnt[i] == (0 - LumeterConfig[i].valid_value_confirm_time))
							{
								cnt[i] = (0 - LumeterConfig[i].valid_value_confirm_time);

								occur[i] = 0;

								record[i] = 1;
							}
						}
						else
						{
							cnt[i] = 0;
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
						buf = (char *)pvPortMalloc(128 * sizeof(char));

						if(buf != NULL)
						{
							if(occur[i] != 0)
							{
								alarm_report->record_type = 1;
							}
							else if(occur[i] == 0)
							{
								alarm_report->record_type = 0;
							}

							alarm_report->device_type = (u8)LUMETER;
							alarm_report->alarm_type = LUMETER_OVER_UNDER_VALUE;

							memset(buf,0,25);
							sprintf(buf, "%X",LumeterState[i].address);
							para_len = strlen(buf);
							alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->device_address != NULL)
							{
								memcpy(alarm_report->device_address,buf,para_len + 1);
							}

							memset(buf,0,25);
							sprintf(buf, "%d",LumeterState[i].channel);
							para_len = strlen(buf);
							alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->device_channel != NULL)
							{
								memcpy(alarm_report->device_channel,buf,para_len + 1);
							}

							memset(buf,0,25);
							sprintf(buf, "%d",LumeterState[i].value);
							para_len = strlen(buf);
							alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->current_value != NULL)
							{
								memcpy(alarm_report->current_value,buf,para_len + 1);
							}

							memset(buf,0,128);

							memset(tmp,0,10);
							sprintf(tmp, "%X",LumeterConfig[i].address);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",LumeterConfig[i].channel);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,25);
							memcpy(&min_value,(void *)&LumeterConfig[i].min_valid_value,4);
							if(min_value == 0xFFFFFFFF)
							{
								strcat(buf,"NA,");
							}
							else
							{
								sprintf(tmp, "%d",LumeterConfig[i].min_valid_value);
								strcat(buf,tmp);
								strcat(buf,",");
							}

							memset(tmp,0,10);
							sprintf(tmp, "%d",LumeterConfig[i].min_valid_value_range);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,25);
							memcpy(&min_value,(void *)&LumeterConfig[i].max_valid_value,4);
							if(min_value == 0xFFFFFFFF)
							{
								strcat(buf,"NA,");
							}
							else
							{
								sprintf(tmp, "%d",LumeterConfig[i].max_valid_value);
								strcat(buf,tmp);
								strcat(buf,",");
							}

							memset(tmp,0,10);
							sprintf(tmp, "%d",LumeterConfig[i].max_valid_value_range);
							strcat(buf,tmp);
							strcat(buf,",");

							memset(tmp,0,10);
							sprintf(tmp, "%d",LumeterConfig[i].valid_value_confirm_time);
							strcat(buf,tmp);

							para_len = strlen(buf);
							alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
							if(alarm_report->set_value != NULL)
							{
								memcpy(alarm_report->set_value,buf,para_len + 1);
							}

							memset(buf,0,32);
							memset(tmp,0,25);
							memcpy(&min_value,(void *)&LumeterConfig[i].min_valid_value,4);
							if(min_value == 0xFFFFFFFF)
							{
								strcat(buf,"NA,");
							}
							else
							{
								sprintf(tmp, "%d",LumeterConfig[i].min_valid_value);
								strcat(buf,tmp);
								strcat(buf,",");
							}

							memset(tmp,0,25);
							memcpy(&min_value,(void *)&LumeterConfig[i].max_valid_value,4);
							if(min_value == 0xFFFFFFFF)
							{
								strcat(buf,"NA,");
							}
							else
							{
								sprintf(tmp, "%d",LumeterConfig[i].max_valid_value);
								strcat(buf,tmp);
								strcat(buf,",");
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

void LumeterNoResponseAlarm(void)
{
	u8 i = 0;
	u16 para_len = 0;
	char tmp[25] = {0};
	static u8 occur[MAX_LUMETER_CONF_NUM] = {0};
	static u8 record[MAX_LUMETER_CONF_NUM] = {0};

	if(LumeterAlarmConfig.lumeter_abnormal_alarm_enable == 1)
	{
		for(i = 0; i < LumeterConfigNum.number; i ++)
		{
			if(occur[i] == 0)
			{
				if(LumeterState[i].no_resp_time >= LumeterConfig[i].no_response_time)
				{
					occur[i] = 1;

					record[i] = 1;
				}
			}
			else
			{
				if(LumeterState[i].no_resp_time < LumeterConfig[i].no_response_time)
				{
					occur[i] = 0;

					record[i] = 1;
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
					buf = (char *)pvPortMalloc(128 * sizeof(char));

					if(buf != NULL)
					{
						if(occur[i] != 0)
						{
							alarm_report->record_type = 1;
						}
						else if(occur[i] == 0)
						{
							alarm_report->record_type = 0;
						}

						alarm_report->device_type = (u8)LUMETER;
						alarm_report->alarm_type = LUMETER_DEVICE_ABNORMAL;

						memset(buf,0,25);
						sprintf(buf, "%X",LumeterState[i].address);
						para_len = strlen(buf);
						alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
						if(alarm_report->device_address != NULL)
						{
							memcpy(alarm_report->device_address,buf,para_len + 1);
						}

						memset(buf,0,25);
						sprintf(buf, "%d",LumeterState[i].channel);
						para_len = strlen(buf);
						alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
						if(alarm_report->device_channel != NULL)
						{
							memcpy(alarm_report->device_channel,buf,para_len + 1);
						}

						memset(buf,0,3);
						sprintf(buf, "NA");
						para_len = strlen(buf);
						alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
						if(alarm_report->current_value != NULL)
						{
							memcpy(alarm_report->current_value,buf,para_len + 1);
						}

						memset(buf,0,32);
						memset(tmp,0,10);
						sprintf(tmp, "%X",LumeterConfig[i].address);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,10);
						sprintf(tmp, "%d",LumeterConfig[i].channel);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,10);
						sprintf(tmp, "%d",LumeterConfig[i].value_unchanged_range);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,10);
						sprintf(tmp, "%d",LumeterConfig[i].no_response_time);
						strcat(buf,tmp);
						strcat(buf,",");

						para_len = strlen(buf);
						alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
						if(alarm_report->set_value != NULL)
						{
							memcpy(alarm_report->set_value,buf,para_len + 1);
						}

						memset(buf,0,32);
						memset(tmp,0,10);
						sprintf(tmp, "%d",LumeterConfig[i].value_unchanged_range);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,10);
						sprintf(tmp, "%d",LumeterConfig[i].no_response_time);
						strcat(buf,tmp);
						strcat(buf,",");
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

void Lumeter24HourUnchangedAlarm(void)
{
	u8 i = 0;
	u16 para_len = 0;
	char tmp[25] = {0};
	static u8 occur[MAX_LUMETER_CONF_NUM] = {0};
	static u8 record[MAX_LUMETER_CONF_NUM] = {0};

	if(LumeterAlarmConfig.lumeter_abnormal_alarm_enable == 1)
	{
		for(i = 0; i < LumeterConfigNum.number; i ++)
		{
			if(LumeterState[i].collect_time >= LumeterBasicConfig.stack_depth * 2)
			{
				if(occur[i] == 0)
				{
					if(abs((s32)(LumeterState[i].value - LumeterState[i].ref_value)) >= LumeterConfig[i].value_unchanged_range)
					{
						if(GetSysTick1s() - LumeterState[i].unchange_time >= 86400)
						{
							occur[i] = 1;

							record[i] = 1;
						}
					}
					else
					{
						LumeterState[i].collect_time = 0;
					}
				}
				else
				{
					if(abs((s32)(LumeterState[i].value - LumeterState[i].ref_value)) < LumeterConfig[i].value_unchanged_range)
					{
						LumeterState[i].collect_time = 0;

						occur[i] = 0;

						record[i] = 1;
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
					buf = (char *)pvPortMalloc(128 * sizeof(char));

					if(buf != NULL)
					{
						if(occur[i] != 0)
						{
							alarm_report->record_type = 1;
						}
						else if(occur[i] == 0)
						{
							alarm_report->record_type = 0;
						}

						alarm_report->device_type = (u8)LUMETER;
						alarm_report->alarm_type = LUMETER_DEVICE_ABNORMAL;

						memset(buf,0,25);
						sprintf(buf, "%X",LumeterState[i].address);
						para_len = strlen(buf);
						alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
						if(alarm_report->device_address != NULL)
						{
							memcpy(alarm_report->device_address,buf,para_len + 1);
						}

						memset(buf,0,25);
						sprintf(buf, "%d",LumeterState[i].channel);
						para_len = strlen(buf);
						alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
						if(alarm_report->device_channel != NULL)
						{
							memcpy(alarm_report->device_channel,buf,para_len + 1);
						}

						memset(buf,0,3);
						sprintf(buf, "%d",LumeterState[i].value);
						para_len = strlen(buf);
						alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
						if(alarm_report->current_value != NULL)
						{
							memcpy(alarm_report->current_value,buf,para_len + 1);
						}

						memset(buf,0,32);
						memset(tmp,0,10);
						sprintf(tmp, "%X",LumeterConfig[i].address);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,10);
						sprintf(tmp, "%d",LumeterConfig[i].channel);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,10);
						sprintf(tmp, "%d",LumeterConfig[i].value_unchanged_range);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,10);
						sprintf(tmp, "%d",LumeterConfig[i].no_response_time);
						strcat(buf,tmp);
						strcat(buf,",");

						para_len = strlen(buf);
						alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
						if(alarm_report->set_value != NULL)
						{
							memcpy(alarm_report->set_value,buf,para_len + 1);
						}

						memset(buf,0,32);
						memset(tmp,0,10);
						sprintf(tmp, "%d",LumeterConfig[i].value_unchanged_range);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,10);
						sprintf(tmp, "%d",LumeterConfig[i].no_response_time);
						strcat(buf,tmp);
						strcat(buf,",");
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



































































