#include "electricity_meter_event.h"
#include "electricity_meter_conf.h"
#include "concentrator_conf.h"
#include "relay_conf.h"
#include "server_protocol.h"
#include "time.h"
#include "rx8010s.h"
#include "history_record.h"



void ElectricityMeterEventCheckPolling(void)
{
	ElectricityMeterDetectParasStateChanges();
	ElectricityMeterDetectParasThreOver();
	ElectricityMeterDetectParasThreUnder();
}

//检测模拟量变化
void ElectricityMeterDetectParasStateChanges(void)
{
	u8 i = 0;
	u8 j = 0;
	static time_t time_s = 0;
	static MultipleCnt1_S cnt[MAX_ELECTRICITY_METER_CONF_NUM][MAX_ELECTRICITY_METER_CH_NUM] = {0};
	static u8 occur[MAX_ELECTRICITY_METER_CONF_NUM] = {0};

	if(ElectricityMeterBasicConfig.auto_report == 1)
	{
		if(GetSysTick10ms() - time_s >= (ElectricityMeterBasicConfig.detect_interval / 10))
		{
			time_s = GetSysTick10ms();

			for(i = 0; i < ElectricityMeterConfigNum.number; i ++)
			{
				for(j = 0; j < ElectricityMeterConfig[i].ch_num; j ++)
				{
					if(ElectricityMeterConfig[i].voltage_range_of_change != 0.0f)
					{
						if(abs(ElectricityMeterState[i].current_para[j].voltage - ElectricityMeterState[i].mirror_para[j].voltage) >= 
						   ElectricityMeterConfig[i].voltage_range_of_change)
						{
							if(cnt[i][j].v < ElectricityMeterConfig[i].confirm_time)
							{
								cnt[i][j].v ++;
							}

							if(cnt[i][j].v == ElectricityMeterConfig[i].confirm_time)									//低电压持续约30秒
							{
								ElectricityMeterState[i].mirror_para[j].voltage = ElectricityMeterState[i].current_para[j].voltage;
								
								occur[i] = 1;
							}
						}
						else
						{
							cnt[i][j].v = 0;
							occur[i] = 0;
						}
					}
					else
					{
						cnt[i][j].v = 0;
						occur[i] = 0;
					}

					if(ElectricityMeterConfig[i].current_range_of_change != 0.0f)
					{
						if(abs(ElectricityMeterState[i].current_para[j].current - ElectricityMeterState[i].mirror_para[j].current) >= 
						   ElectricityMeterConfig[i].current_range_of_change)
						{
							if(cnt[i][j].c < ElectricityMeterConfig[i].confirm_time)
							{
								cnt[i][j].c ++;
							}

							if(cnt[i][j].c == ElectricityMeterConfig[i].confirm_time)									//低电压持续约30秒
							{
								ElectricityMeterState[i].mirror_para[j].current = ElectricityMeterState[i].current_para[j].current;
								
								occur[i] = 1;
							}
						}
						else
						{
							cnt[i][j].c = 0;
							occur[i] = 0;
						}
					}
					else
					{
						cnt[i][j].c = 0;
						occur[i] = 0;
					}
					
					if(ElectricityMeterConfig[i].pf_range_of_change != 0.0f)
					{
						if(abs(ElectricityMeterState[i].current_para[j].power_factor - ElectricityMeterState[i].mirror_para[j].power_factor) >= 
						   ElectricityMeterConfig[i].pf_range_of_change)
						{
							if(cnt[i][j].pf < ElectricityMeterConfig[i].confirm_time)
							{
								cnt[i][j].pf ++;
							}

							if(cnt[i][j].pf == ElectricityMeterConfig[i].confirm_time)									//低电压持续约30秒
							{
								ElectricityMeterState[i].mirror_para[j].power_factor = ElectricityMeterState[i].current_para[j].power_factor;
								
								occur[i] = 1;
							}
						}
						else
						{
							cnt[i][j].pf = 0;
							occur[i] = 0;
						}
					}
					else
					{
						cnt[i][j].pf = 0;
						occur[i] = 0;
					}
				}

				if(occur[i] == 1)
				{
					ElectricityMeterState_S *module_state = NULL;

					occur[i] = 0;

					module_state = (ElectricityMeterState_S *)pvPortMalloc(sizeof(ElectricityMeterState_S));

					if(module_state != NULL)
					{
						memcpy(module_state,&ElectricityMeterState[i],sizeof(ElectricityMeterState_S));

						if(xQueueSend(xQueue_ElectricityMeterState,(void *)&module_state,(TickType_t)10) != pdPASS)
						{
#ifdef DEBUG_LOG
							printf("send xQueue_ElectricityMeterState fail.\r\n");
#endif
							vPortFree(module_state);
						}
					}
				}
			}
		}
	}
}

void ElectricityMeterCombineParasThreOverAlarm(u8 i,u8 j,u8 m,double reference_value)
{
	u16 para_len = 0;
	long long max_value = 0;
	char tmp[25] = {0};
	char *buf = NULL;
	AlarmReport_S *alarm_report = NULL;

	alarm_report = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));

	if(alarm_report != NULL)
	{
		buf = (char *)pvPortMalloc(100 * sizeof(char));

		if(buf != NULL)
		{
			if(ElectricityMeterState[i].abnormal_ch != 0)
			{
				alarm_report->record_type = 1;
			}
			else if(ElectricityMeterState[i].abnormal_ch == 0)
			{
				alarm_report->record_type = 0;
			}
			
			alarm_report->device_type = (u8)ELECTRICITY_METER;
			alarm_report->alarm_type = ELECTRICITY_METER_PARAS_THRE_OVER;
			
			memset(buf,0,25);
			sprintf(buf, "%d",ElectricityMeterState[i].address);
			para_len = strlen(buf);
			alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
			if(alarm_report->device_address != NULL)
			{
				memcpy(alarm_report->device_address,buf,para_len + 1);
			}
			
			memset(buf,0,25);
			sprintf(buf, "%d",ElectricityMeterState[i].channel);
			para_len = strlen(buf);
			alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
			if(alarm_report->device_channel != NULL)
			{
				memcpy(alarm_report->device_channel,buf,para_len + 1);
			}
			
			memset(buf,0,25);
			sprintf(buf, "%04x",ElectricityMeterState[i].abnormal_ch);
			para_len = strlen(buf);
			alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
			if(alarm_report->current_value != NULL)
			{
				memcpy(alarm_report->current_value,buf,para_len + 1);
			}
			
			memset(buf,0,100);
			memset(tmp,0,10);
			sprintf(tmp, "%x",ElectricityMeterConfig[i].address);
			strcat(buf,tmp);
			strcat(buf,",");
			
			memset(tmp,0,10);
			sprintf(tmp, "%x",ElectricityMeterConfig[i].channel);
			strcat(buf,tmp);
			strcat(buf,",");

			memset(tmp,0,10);
			sprintf(tmp, "%d",ElectricityMeterConfig[i].alarm_thre[j][m].channel);
			strcat(buf,tmp);
			strcat(buf,",");

			memset(tmp,0,10);
			sprintf(tmp, "%d",ElectricityMeterConfig[i].alarm_thre[j][m].para_id);
			strcat(buf,tmp);
			strcat(buf,",");
			
			memset(tmp,0,25);
			memcpy(&max_value,(void *)&ElectricityMeterConfig[i].alarm_thre[j][m].min_value,4);
			if(max_value == 0xFFFFFFFFFFFFFFFF)
			{
				strcat(buf,"NA,");
			}
			else
			{
				sprintf(tmp, "%f",ElectricityMeterConfig[i].alarm_thre[j][m].min_value);
				strcat(buf,tmp);
				strcat(buf,",");
			}
			
			memset(tmp,0,25);
			memcpy(&max_value,(void *)&ElectricityMeterConfig[i].alarm_thre[j][m].max_value,4);
			if(max_value == 0xFFFFFFFFFFFFFFFF)
			{
				strcat(buf,"NA,");
			}
			else
			{
				sprintf(tmp, "%f",ElectricityMeterConfig[i].alarm_thre[j][m].max_value);
				strcat(buf,tmp);
				strcat(buf,",");
			}
			
			memset(tmp,0,10);
			sprintf(tmp, "%d",ElectricityMeterConfig[i].alarm_thre[j][m].confirm_time);
			strcat(buf,tmp);
			strcat(buf,",");
			
			memset(tmp,0,10);
			sprintf(tmp, "%d",ElectricityMeterConfig[i].alarm_thre[j][m].switch_run_mode);
			strcat(buf,tmp);
			strcat(buf,",");
			
			memset(tmp,0,10);
			sprintf(tmp, "%d",ElectricityMeterConfig[i].alarm_thre[j][m].resume_run_mode);
			strcat(buf,tmp);
			strcat(buf,",");

			memset(tmp,0,10);
			sprintf(tmp, "%d",ElectricityMeterConfig[i].alarm_thre[j][m].relay_action);
			strcat(buf,tmp);
			
			para_len = strlen(buf);
			alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
			if(alarm_report->set_value != NULL)
			{
				memcpy(alarm_report->set_value,buf,para_len + 1);
			}
			
			memset(buf,0,25);
			sprintf(tmp, "%0.3f",reference_value);
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

void ElectricityMeterDetectParasThreOver(void)
{
	u8 i = 0;
	u8 j = 0;
	u8 m = 0;
	float value = 0.0f;
	long long max_value = 0;
	static time_t time_s = 0;
	static MultipleCnt2_S cnt[MAX_ELECTRICITY_METER_CONF_NUM][MAX_ELECTRICITY_METER_CH_NUM] = {0};
	static u8 occur[MAX_ELECTRICITY_METER_CONF_NUM][MAX_ELECTRICITY_METER_CH_NUM] = {0};

	if(ElectricityMeterAlarmConfig.electrical_parameters_thre_over_alarm_enable == 1)
	{
		if(GetSysTick10ms() - time_s >= (ElectricityMeterBasicConfig.detect_interval / 10))
		{
			time_s = GetSysTick10ms();

			for(i = 0; i < ElectricityMeterConfigNum.number; i ++)
			{
				for(j = 0; j < ElectricityMeterConfig[i].ch_num; j ++)
				{
					for(m = 0; m < MAX_ELECTRICITY_METER_ALARM_PARA_NUM; m ++)
					{
						if(ElectricityMeterConfig[i].alarm_thre[j][m].channel  < MAX_ELECTRICITY_METER_CH_NUM && 
						   ElectricityMeterConfig[i].alarm_thre[j][m].para_id != 0)
						{
							memcpy(&max_value,(void *)&ElectricityMeterConfig[i].alarm_thre[j][m].max_value,8);
							
							if(max_value != 0xFFFFFFFFFFFFFFFF && ElectricityMeterConfig[i].alarm_thre[j][m].max_value != 0.0f)
							{
								switch(m)
								{
									case 0:
										value = ElectricityMeterState[i].current_para[j].voltage;
									break;
									
									case 1:
										value = ElectricityMeterState[i].current_para[j].current;
									break;
									
									case 2:
										value = ElectricityMeterState[i].current_para[j].active_power;
									break;
									
									case 3:
										value = ElectricityMeterState[i].current_para[j].active_energy;
									break;
									
									case 4:
										value = ElectricityMeterState[i].current_para[j].reactive_energy;
									break;
									
									case 5:
										value = ElectricityMeterState[i].current_para[j].power_factor;
									break;
									
									case 6:
										value = ElectricityMeterState[i].current_para[j].frequency;
									break;
									
									case 7:
										value = ElectricityMeterState[i].current_para[j].line_voltage;
									break;
									
									default:
									break;
								}
								
								if(occur[i][j] == 0)
								{
									if(value >= ElectricityMeterConfig[i].alarm_thre[j][m].max_value)
									{
										cnt[i][j].cnt[m] ++;

										if(cnt[i][j].cnt[m] == ElectricityMeterConfig[i].alarm_thre[j][m].confirm_time)
										{
											cnt[i][j].cnt[m] = ElectricityMeterConfig[i].alarm_thre[j][m].confirm_time;

											occur[i][j] = 1;
											
											ElectricityMeterState[i].abnormal_ch |= (1 << j);
											
											if(ElectricityMeterConfig[i].alarm_thre[j][m].switch_run_mode != 0)
											{
												ElectricityMeterConfig[i].alarm_thre[j][m].last_run_mode = (u8)RunMode;
												RunMode = (RUN_MODE_E)ElectricityMeterConfig[i].alarm_thre[j][m].switch_run_mode;
											}
											
											if(ElectricityMeterConfig[i].alarm_thre[j][m].relay_action == 1)
											{
												RelayForceSwitchOffAllRelays = 1;
											}
											
											ElectricityMeterCombineParasThreOverAlarm(i,j,m,value);
										}
									}
									else
									{
										cnt[i][j].cnt[m] = 0;
									}
								}
								else
								{
									if((value - ElectricityMeterConfig[i].alarm_thre[j][m].max_value) / 100.0f >= 0.05f)
									{
										cnt[i][j].cnt[m] --;

										if(cnt[i][j].cnt[m] == (0 - ElectricityMeterConfig[i].alarm_thre[j][m].confirm_time))
										{
											cnt[i][j].cnt[m] = (0 - ElectricityMeterConfig[i].alarm_thre[j][m].confirm_time);

											occur[i][j] = 1;
											
											ElectricityMeterState[i].abnormal_ch &= ~(1 << j);
											
											if(ElectricityMeterConfig[i].alarm_thre[j][m].switch_run_mode != 0)
											{
												if(ElectricityMeterConfig[i].alarm_thre[j][m].resume_run_mode == 1)
												{
													RunMode = (RUN_MODE_E)ElectricityMeterConfig[i].alarm_thre[j][m].last_run_mode;
												}
											}
											
											ElectricityMeterCombineParasThreOverAlarm(i,j,m,value);
										}
									}
									else
									{
										cnt[i][j].cnt[m] = 0;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void ElectricityMeterDetectParasThreUnder(void)
{
	u8 i = 0;
	u8 j = 0;
	u8 m = 0;
	float value = 0.0f;
	long long min_value = 0;
	static time_t time_s = 0;
	static MultipleCnt2_S cnt[MAX_ELECTRICITY_METER_CONF_NUM][MAX_ELECTRICITY_METER_CH_NUM] = {0};
	static u8 occur[MAX_ELECTRICITY_METER_CONF_NUM][MAX_ELECTRICITY_METER_CH_NUM] = {0};

	if(ElectricityMeterAlarmConfig.electrical_parameters_thre_over_alarm_enable == 1)
	{
		if(GetSysTick10ms() - time_s >= (ElectricityMeterBasicConfig.detect_interval / 10))
		{
			time_s = GetSysTick10ms();

			for(i = 0; i < ElectricityMeterConfigNum.number; i ++)
			{
				for(j = 0; j < ElectricityMeterConfig[i].ch_num; j ++)
				{
					for(m = 0; m < MAX_ELECTRICITY_METER_ALARM_PARA_NUM; m ++)
					{
						if(ElectricityMeterConfig[i].alarm_thre[j][m].channel  < MAX_ELECTRICITY_METER_CH_NUM && 
						   ElectricityMeterConfig[i].alarm_thre[j][m].para_id != 0)
						{
							memcpy(&min_value,(void *)&ElectricityMeterConfig[i].alarm_thre[j][m].max_value,8);
							
							if(min_value != 0xFFFFFFFFFFFFFFFF && ElectricityMeterConfig[i].alarm_thre[j][m].max_value != 0.0f)
							{
								switch(m)
								{
									case 0:
										value = ElectricityMeterState[i].current_para[j].voltage;
									break;
									
									case 1:
										value = ElectricityMeterState[i].current_para[j].current;
									break;
									
									case 2:
										value = ElectricityMeterState[i].current_para[j].active_power;
									break;
									
									case 3:
										value = ElectricityMeterState[i].current_para[j].active_energy;
									break;
									
									case 4:
										value = ElectricityMeterState[i].current_para[j].reactive_energy;
									break;
									
									case 5:
										value = ElectricityMeterState[i].current_para[j].power_factor;
									break;
									
									case 6:
										value = ElectricityMeterState[i].current_para[j].frequency;
									break;
									
									case 7:
										value = ElectricityMeterState[i].current_para[j].line_voltage;
									break;
									
									default:
									break;
								}
								
								if(occur[i][j] == 0)
								{
									if(value <= ElectricityMeterConfig[i].alarm_thre[j][m].min_value)
									{
										cnt[i][j].cnt[m] ++;

										if(cnt[i][j].cnt[m] == ElectricityMeterConfig[i].alarm_thre[j][m].confirm_time)
										{
											cnt[i][j].cnt[m] = ElectricityMeterConfig[i].alarm_thre[j][m].confirm_time;

											occur[i][j] = 1;
											
											ElectricityMeterState[i].abnormal_ch |= (1 << j);
											
											if(ElectricityMeterConfig[i].alarm_thre[j][m].switch_run_mode != 0)
											{
												ElectricityMeterConfig[i].alarm_thre[j][m].last_run_mode = (u8)RunMode;
												RunMode = (RUN_MODE_E)ElectricityMeterConfig[i].alarm_thre[j][m].switch_run_mode;
											}
											
											if(ElectricityMeterConfig[i].alarm_thre[j][m].relay_action == 1)
											{
												RelayForceSwitchOffAllRelays = 1;
											}
											
											ElectricityMeterCombineParasThreOverAlarm(i,j,m,value);
										}
									}
									else
									{
										cnt[i][j].cnt[m] = 0;
									}
								}
								else
								{
									if((ElectricityMeterConfig[i].alarm_thre[j][m].min_value - value) / 100.0f >= 0.05f)
									{
										cnt[i][j].cnt[m] --;

										if(cnt[i][j].cnt[m] == (0 - ElectricityMeterConfig[i].alarm_thre[j][m].confirm_time))
										{
											cnt[i][j].cnt[m] = (0 - ElectricityMeterConfig[i].alarm_thre[j][m].confirm_time);

											occur[i][j] = 1;
											
											ElectricityMeterState[i].abnormal_ch &= ~(1 << j);
											
											if(ElectricityMeterConfig[i].alarm_thre[j][m].switch_run_mode != 0)
											{
												if(ElectricityMeterConfig[i].alarm_thre[j][m].resume_run_mode == 1)
												{
													RunMode = (RUN_MODE_E)ElectricityMeterConfig[i].alarm_thre[j][m].last_run_mode;
												}
											}
											
											ElectricityMeterCombineParasThreOverAlarm(i,j,m,value);
										}
									}
									else
									{
										cnt[i][j].cnt[m] = 0;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}








































