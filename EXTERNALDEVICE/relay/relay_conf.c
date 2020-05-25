#include "relay_conf.h"
#include "common.h"
#include "cat25x.h"
#include "concentrator_conf.h"
#include "sun_rise_set.h"
#include "rx8010s.h"

u8 RelayForceSwitchOffAllRelays = 0;												//ǿ�ƶϿ����м̵���
u8 RelayRefreshStrategyGroup = 1;													//�̵�������ˢ�±�־

Uint32TypeNumber_S RelayModuleConfigNum;											//�̵���ģ����������
RelayModuleConfig_S RelayModuleConfig[MAX_RELAY_MODULE_CONF_NUM];					//�̵���ģ������
DefaultSwitchTime_S DefaultSwitchTime;												//Ĭ�Ͽ��ص�ʱ��
RelayAlarmConfig_S RelayAlarmConfig;												//�̵���ģ��澯��������
Uint32TypeNumber_S RelayAppointmentNum;												//�̵���ģ��ԤԼ��������
Uint32TypeNumber_S RelayStrategyNum;												//�̵���ģ�������������
RelayModuleState_S RelayModuleState[MAX_RELAY_MODULE_CONF_NUM];						//�̵���ģ�鵱ǰ״̬

pRelayAppointment RelayAppointmentGroup = NULL;										//ԤԼ������
pRelayStrategy RelayStrategyGroup[MAX_RELAY_MODULE_STRATEGY_GROUP_NUM] = {NULL};	//������
RelayStrategyGroupSwitch_S RelayStrategyGroupSwitch;								//������(ģʽ)�л�����
RelayStrategyGroupSwitch_S RelayStrategyGroupSwitchTemp;							//������(ģʽ)�л�����(��ʱ)
pRelayStrategy CurrentRelayStrategyGroup = NULL;									//��ǰ������(ģʽ)
pRelayStrategy CurrentRelayStrategyGroupTemp = NULL;								//��ǰ������(ģʽ)(��ʱ)



//��ȡ�̵���ģ����������
void ReadRelayModuleConfigNum(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&RelayModuleConfigNum,
	            RELAY_MODULE_CONF_NUM_ADD,
	            sizeof(Uint32TypeNumber_S));

	crc16_cal = CRC16((u8 *)&RelayModuleConfigNum,RELAY_MODULE_CONF_NUM_LEN - 2);

	if(crc16_cal != RelayModuleConfigNum.crc16 ||
	   RelayModuleConfigNum.number > MAX_RELAY_MODULE_CONF_NUM)
	{
		WriteRelayModuleConfigNum(1,0);
	}
}

void WriteRelayModuleConfigNum(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		RelayModuleConfigNum.number = 0;

		RelayModuleConfigNum.crc16 = 0;
	}

	if(write_enable == 1)
	{
		RelayModuleConfigNum.crc16 = CRC16((u8 *)&RelayModuleConfigNum,RELAY_MODULE_CONF_NUM_LEN - 2);

		CAT25X_Write((u8 *)&RelayModuleConfigNum,
		             RELAY_MODULE_CONF_NUM_ADD,
		             RELAY_MODULE_CONF_NUM_LEN);
	}
}

//��ȡ�̵���ģ������
void ReadRelayModuleConfig(void)
{
	u16 crc16_cal = 0;
	u8 i = 0;
	u8 j = 0;

	ReadRelayModuleConfigNum();

	for(i = 0; i < RelayModuleConfigNum.number; i ++)
	{
		CAT25X_Read((u8 *)&RelayModuleConfig[i],
		            RELAY_MODULE_CONF_ADD + i * RELAY_MODULE_CONF_LEN,
		            sizeof(RelayModuleConfig_S));

		crc16_cal = CRC16((u8 *)&RelayModuleConfig[i],RELAY_MODULE_CONF_LEN - 2);

		if(crc16_cal != RelayModuleConfig[i].crc16)
		{
			WriteRelayModuleConfig(i,1,0);
		}
		else
		{
			memset(&RelayModuleState[i],0,sizeof(RelayModuleState_S));
			
			RelayModuleState[i].address = RelayModuleConfig[i].address;
			RelayModuleState[i].channel = RelayModuleConfig[i].channel;
			RelayModuleState[i].interval_time = RelayModuleConfig[i].interval_time;
			
			if(RelayModuleConfig[i].loop_num <= MAX_RELAY_MODULE_LOOP_CH_NUM)
			{
				for(j = 0; j < RelayModuleConfig[i].loop_num; j ++)
				{
					RelayModuleState[i].loop_channel_bit |= (1 << j);
				}
			}
			else
			{
				RelayModuleState[i].loop_channel_bit = 0;
			}
		}
		
	}
}

void WriteRelayModuleConfig(u8 i,u8 reset,u8 write_enable)
{
	u8 j = 0; 
	
	if(reset == 1)
	{
		RelayModuleConfig[i].address = 0xFF;
		RelayModuleConfig[i].channel = 0xFF;
		RelayModuleConfig[i].loop_num = 0;
		RelayModuleConfig[i].interval_time = 0;
		memset(RelayModuleConfig[i].module,0,MAX_RELAY_MODULE_MODULE_NAME_LEN);
		RelayModuleConfig[i].loop_enable = 0xFFFF;
		memset(RelayModuleConfig[i].loop_group,0,MAX_RELAY_MODULE_LOOP_CH_NUM * MAX_RELAY_MODULE_LOOP_GROUP_NUM);
		memset(RelayModuleConfig[i].loop_alarm_thre,0,MAX_RELAY_MODULE_LOOP_CH_NUM * 5);

		RelayModuleConfig[i].crc16 = 0;
	}

	if(write_enable == 1)
	{
		RelayModuleConfig[i].crc16 = CRC16((u8 *)&RelayModuleConfig[i],RELAY_MODULE_CONF_LEN - 2);

		CAT25X_Write((u8 *)&RelayModuleConfig[i],
		             RELAY_MODULE_CONF_ADD + i * RELAY_MODULE_CONF_LEN,
		             RELAY_MODULE_CONF_LEN);
	}
	
	memset(&RelayModuleState[i],0,sizeof(RelayModuleState_S));
	
	RelayModuleState[i].address = RelayModuleConfig[i].address;
	RelayModuleState[i].channel = RelayModuleConfig[i].channel;
	RelayModuleState[i].interval_time = RelayModuleConfig[i].interval_time;
	
	if(RelayModuleConfig[i].loop_num <= MAX_RELAY_MODULE_LOOP_CH_NUM)
	{
		for(j = 0; j < RelayModuleConfig[i].loop_num; j ++)
		{
			RelayModuleState[i].loop_channel_bit |= (1 << j);
		}
	}
	else
	{
		RelayModuleState[i].loop_channel_bit = 0;
	}
}

//��ȡĬ�Ͽ��ص�ʱ��
void ReadDefaultSwitchTime(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&DefaultSwitchTime,
	            DEFAULT_SWITCH_TIME_ADD,
	            sizeof(DefaultSwitchTime_S));

	crc16_cal = CRC16((u8 *)&DefaultSwitchTime,DEFAULT_SWITCH_TIME_LEN - 2);

	if(crc16_cal != DefaultSwitchTime.crc16)
	{
		WriteDefaultSwitchTime(1,0);
	}
}

void WriteDefaultSwitchTime(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		DefaultSwitchTime.on_hour = 18;
		DefaultSwitchTime.on_minute = 0;
		DefaultSwitchTime.off_hour = 6;
		DefaultSwitchTime.off_minute = 0;

		DefaultSwitchTime.crc16 = 0;
	}

	if(write_enable == 1)
	{
		DefaultSwitchTime.crc16 = CRC16((u8 *)&DefaultSwitchTime,DEFAULT_SWITCH_TIME_LEN - 2);

		CAT25X_Write((u8 *)&DefaultSwitchTime,
		             DEFAULT_SWITCH_TIME_ADD,
		             DEFAULT_SWITCH_TIME_LEN);
	}
}

//��ȡ�澯��������
void ReadRelayAlarmConfig(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&RelayAlarmConfig,
	            RELAY_MODULE_ALARM_CONF_ADD,
	            sizeof(RelayAlarmConfig_S));

	crc16_cal = CRC16((u8 *)&RelayAlarmConfig,RELAY_MODULE_ALARM_CONF_LEN - 2);

	if(crc16_cal != RelayAlarmConfig.crc16)
	{
		WriteRelayAlarmConfig(1,0);
	}
}

void WriteRelayAlarmConfig(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		RelayAlarmConfig.relay_abnormal_alarm_enable = 0;
		RelayAlarmConfig.contact_abnormal_alarm_enable = 0;
		RelayAlarmConfig.task_abnormal_alarm_enable = 0;

		RelayAlarmConfig.crc16 = 0;
	}

	if(write_enable == 1)
	{
		RelayAlarmConfig.crc16 = CRC16((u8 *)&RelayAlarmConfig,RELAY_MODULE_ALARM_CONF_LEN - 2);

		CAT25X_Write((u8 *)&RelayAlarmConfig,
		             RELAY_MODULE_ALARM_CONF_ADD,
		             RELAY_MODULE_ALARM_CONF_LEN);
	}
}

//��ȡ�̵���ģ��ԤԼ��������
void ReadRelayAppointmentNum(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&RelayAppointmentNum,
	            RELAY_MODULE_APPOINTMENT_NUM_ADD,
	            sizeof(Uint32TypeNumber_S));

	crc16_cal = CRC16((u8 *)&RelayAppointmentNum,RELAY_MODULE_APPOINTMENT_NUM_LEN - 2);

	if(crc16_cal != RelayAppointmentNum.crc16 ||
	   RelayAppointmentNum.number > MAX_RELAY_MODULE_APPOINTMENT_NUM)
	{
		WriteRelayAppointmentNum(1,0);
	}
}

void WriteRelayAppointmentNum(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		RelayAppointmentNum.number = 0;

		RelayAppointmentNum.crc16 = 0;
	}

	if(write_enable == 1)
	{
		RelayAppointmentNum.crc16 = CRC16((u8 *)&RelayAppointmentNum,RELAY_MODULE_APPOINTMENT_NUM_LEN - 2);

		CAT25X_Write((u8 *)&RelayAppointmentNum,
		             RELAY_MODULE_APPOINTMENT_NUM_ADD,
		             RELAY_MODULE_APPOINTMENT_NUM_LEN);
	}
}

//��ȡ�̵���ģ��ԤԼ����
u8 ReadRelayAppointment(u8 i,RelaySenceConfig_S *appointment)
{
	u8 ret = 0;
	u16 crc16_cal = 0;
	
	if(appointment == NULL)
	{
		return ret;
	}

	CAT25X_Read((u8 *)appointment,
	            RELAY_MODULE_APPOINTMENT_ADD + RELAY_MODULE_APPOINTMENT_LEN * i,
	            sizeof(RelaySenceConfig_S));

	crc16_cal = CRC16((u8 *)appointment,RELAY_MODULE_APPOINTMENT_LEN - 2);

	if(crc16_cal == appointment->crc16)
	{
		ret = 1;
	}
	
	return ret;
}

void WriteRelayAppointment(u8 i,RelaySenceConfig_S *appointment,u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		memset(appointment,0,sizeof(RelaySenceConfig_S));
	}

	if(write_enable == 1)
	{
		appointment->crc16 = CRC16((u8 *)appointment,RELAY_MODULE_APPOINTMENT_LEN - 2);

		CAT25X_Write((u8 *)appointment,
		             RELAY_MODULE_APPOINTMENT_ADD + RELAY_MODULE_APPOINTMENT_LEN * i,
		             RELAY_MODULE_APPOINTMENT_LEN);
	}
}

//��ȡ�̵���ģ���������
void ReadRelayStrategyNum(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&RelayStrategyNum,
	            RELAY_MODULE_STRATEGY_NUM_ADD,
	            sizeof(Uint32TypeNumber_S));

	crc16_cal = CRC16((u8 *)&RelayStrategyNum,RELAY_MODULE_STRATEGY_NUM_LEN - 2);

	if(crc16_cal != RelayStrategyNum.crc16 ||
	   RelayStrategyNum.number > MAX_RELAY_MODULE_STRATEGY_NUM)
	{
		WriteRelayStrategyNum(1,0);
	}
}

void WriteRelayStrategyNum(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		RelayStrategyNum.number = 0;

		RelayStrategyNum.crc16 = 0;
	}

	if(write_enable == 1)
	{
		RelayStrategyNum.crc16 = CRC16((u8 *)&RelayStrategyNum,RELAY_MODULE_STRATEGY_NUM_LEN - 2);

		CAT25X_Write((u8 *)&RelayStrategyNum,
		             RELAY_MODULE_STRATEGY_NUM_ADD,
		             RELAY_MODULE_STRATEGY_NUM_LEN);
	}
}

//��ȡ�̵���ģ�鵥������
u8 ReadRelayStrategy(u8 i,RelayTask_S *strategy)
{
	u8 ret = 0;
	u16 crc16_cal = 0;
	
	if(strategy == NULL)
	{
		return ret;
	}

	CAT25X_Read((u8 *)strategy,
	            RELAY_MODULE_STRATEGY_ADD + RELAY_MODULE_STRATEGY_LEN * i,
	            sizeof(RelayTask_S));

	crc16_cal = CRC16((u8 *)strategy,RELAY_MODULE_STRATEGY_LEN - 2);

	if(crc16_cal == strategy->crc16)
	{
		ret = 1;
	}
	
	return ret;
}

void WriteRelayStrategy(u8 i,RelayTask_S *strategy,u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		memset(strategy,0,sizeof(RelayTask_S));
	}

	if(write_enable == 1)
	{
		strategy->crc16 = CRC16((u8 *)strategy,RELAY_MODULE_STRATEGY_LEN - 2);

		CAT25X_Write((u8 *)strategy,
		             RELAY_MODULE_STRATEGY_ADD + RELAY_MODULE_STRATEGY_LEN * i,
		             RELAY_MODULE_STRATEGY_LEN);
	}
}

//��ȡԤԼ��������
void ReadRelayAppointmentGroup(void)
{
	u8 i = 0;
	u16 crc16_cal = 0;
	RelaySenceConfig_S relay_sence_config;

	ReadRelayAppointmentNum();

	RelayAppointmentGroup = (pRelayAppointment)pvPortMalloc(sizeof(RelayAppointment_S));

	if(RelayAppointmentGroup == NULL)
	{
		return;
	}
	
	memset(RelayAppointmentGroup,0,sizeof(RelayAppointment_S));
	
	if(RelayAppointmentNum.number == 0)	//��ԤԼ����
	{
		return;
	}

	for(i = 0; i < RelayAppointmentNum.number; i ++)
	{
		memset(&relay_sence_config,0,sizeof(RelaySenceConfig_S));

		CAT25X_Read((u8 *)&relay_sence_config,
	            RELAY_MODULE_APPOINTMENT_ADD + i * RELAY_MODULE_APPOINTMENT_LEN,
	            sizeof(RelaySenceConfig_S));

		crc16_cal = CRC16((u8 *)&relay_sence_config,RELAY_MODULE_APPOINTMENT_LEN - 2);

		if(crc16_cal == relay_sence_config.crc16)
		{
			pRelayAppointment appointment = NULL;

			appointment = (pRelayAppointment)pvPortMalloc(sizeof(RelayAppointment_S));

			if(appointment != NULL)
			{
				memset(appointment,0,sizeof(RelayAppointment_S));

				appointment->group_id 		= relay_sence_config.group_id;
				appointment->priority 		= relay_sence_config.priority;
				appointment->time_range_num = relay_sence_config.time_range_num;

				memcpy(appointment->range,
				       relay_sence_config.range,
				       appointment->time_range_num * sizeof(TimeRange_S));

				RelayAppointmentGroupAdd(appointment);
			}
		}
	}
}

//��ȡ��������
void ReadRelayStrategyGroups(void)
{
	u8 i = 0;
	u16 crc16_cal = 0;
	RelayTask_S relay_task;

	ReadRelayStrategyNum();

	for(i = 0; i < MAX_RELAY_MODULE_STRATEGY_GROUP_NUM; i ++)
	{
		RelayStrategyGroup[i] = (pRelayStrategy)pvPortMalloc(sizeof(RelayStrategy_S));

		if(RelayStrategyGroup[i] != NULL)
		{
			memset(RelayStrategyGroup[i],0,sizeof(RelayStrategy_S));
			
			RelayStrategyGroup[i]->group_id = i + 1;
		}
	}
	
	if(RelayStrategyNum.number == 0)	//�޲���
	{
		return;
	}

	for(i = 0; i < RelayStrategyNum.number; i ++)
	{
		memset(&relay_task,0,sizeof(RelayTask_S));

		CAT25X_Read((u8 *)&relay_task,
	                RELAY_MODULE_STRATEGY_ADD + i * RELAY_MODULE_STRATEGY_LEN,
	                sizeof(RelayTask_S));

		crc16_cal = CRC16((u8 *)&relay_task,RELAY_MODULE_STRATEGY_LEN - 2);

		if(crc16_cal == relay_task.crc16)
		{
			pRelayStrategy strategy = NULL;

			strategy = (pRelayStrategy)pvPortMalloc(sizeof(RelayStrategy_S));

			if(strategy != NULL)
			{
				memset(strategy,0,sizeof(RelayStrategy_S));

				strategy->group_id = relay_task.group_id;
				strategy->type = relay_task.type;
				strategy->time_option = relay_task.time_option;
				strategy->action_num = relay_task.action_num;
				strategy->offset_min = relay_task.time;

				memcpy(strategy->action,
				       relay_task.action,
				       strategy->action_num* sizeof(RelayTaskAction_S));

				RefreshRelayStrategyActionTime(strategy);

				RelayStrategyAdd(strategy);
			}
		}
	}
}

//��ȡ�̵���ģ������(ģʽ)�л�����
void ReadRelayStrategyGroupSwitch(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&RelayStrategyGroupSwitch,
	            RELAY_MODULE_STRATEGY_SWITCH_ADD,
	            sizeof(RelayStrategyGroupSwitch_S));

	crc16_cal = CRC16((u8 *)&RelayStrategyGroupSwitch,RELAY_MODULE_STRATEGY_SWITCH_LEN - 2);

	if(crc16_cal != RelayStrategyGroupSwitch.crc16)
	{
		WriteRelayStrategyGroupSwitch(1,0);
	}
}

void WriteRelayStrategyGroupSwitch(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		memset(&RelayStrategyGroupSwitch,0,sizeof(RelayStrategyGroupSwitch_S));
	}

	if(write_enable == 1)
	{
		RelayStrategyGroupSwitch.crc16 = CRC16((u8 *)&RelayStrategyGroupSwitch,RELAY_MODULE_STRATEGY_SWITCH_LEN - 2);

		CAT25X_Write((u8 *)&RelayStrategyGroupSwitch,
		             RELAY_MODULE_STRATEGY_SWITCH_ADD,
		             RELAY_MODULE_STRATEGY_SWITCH_LEN);
	}
}

//ˢ�¼�����Զ���ʱ��
void RefreshRelayStrategyActionTime(pRelayStrategy strategy)
{
	if(strategy == NULL)
	{
		return;
	}

	switch(strategy->type)
	{
		case (u8)TYPE_FIXED_TIME:
			strategy->action_time = strategy->offset_min;
		break;

		case (u8)TYPE_FIXED_TIME_LIGHT:
			strategy->action_time = strategy->offset_min;
		break;

		case (u8)TYPE_LOCATION:
			if(strategy->time_option == 1)					//����ʱ��
			{
				strategy->action_time = SunRiseSetTime.set_h * 60 +
				                        SunRiseSetTime.set_m +
				                        strategy->offset_min;
			}
			else if(strategy->time_option == 2)				//�ճ�ʱ��
			{
				strategy->action_time = SunRiseSetTime.rise_h * 60 +
				                        SunRiseSetTime.rise_m +
				                        strategy->offset_min;
			}
		break;

		case (u8)TYPE_LOCATION_LIGHT:
			if(strategy->time_option == 1)					//����ʱ��
			{
				strategy->action_time = SunRiseSetTime.set_h * 60 +
				                        SunRiseSetTime.set_m +
				                        strategy->offset_min;
			}
			else if(strategy->time_option == 2)				//�ճ�ʱ��
			{
				strategy->action_time = SunRiseSetTime.rise_h * 60 +
				                        SunRiseSetTime.rise_m +
				                        strategy->offset_min;
			}
		break;

		case (u8)TYPE_CHRONOLOGY:
			if(strategy->time_option == 1)					//����ʱ��
			{
				strategy->action_time = ConcentratorLocationConfig.switch_time_month_table[calendar.w_month].switch_time[calendar.w_date].off_hour * 60 +
				                        ConcentratorLocationConfig.switch_time_month_table[calendar.w_month].switch_time[calendar.w_date].off_minute +
				                        strategy->offset_min;
			}
			else if(strategy->time_option == 2)				//�ճ�ʱ��
			{
				strategy->action_time = ConcentratorLocationConfig.switch_time_month_table[calendar.w_month].switch_time[calendar.w_date].on_hour * 60 +
				                        ConcentratorLocationConfig.switch_time_month_table[calendar.w_month].switch_time[calendar.w_date].on_minute +
				                        strategy->offset_min;
			}
		break;

		case (u8)TYPE_CHRONOLOGY_LIGHT:
			if(strategy->time_option == 1)					//����ʱ��
			{
				strategy->action_time = ConcentratorLocationConfig.switch_time_month_table[calendar.w_month].switch_time[calendar.w_date].off_hour * 60 +
				                        ConcentratorLocationConfig.switch_time_month_table[calendar.w_month].switch_time[calendar.w_date].off_minute +
				                        strategy->offset_min;
			}
			else if(strategy->time_option == 2)				//�ճ�ʱ��
			{
				strategy->action_time = ConcentratorLocationConfig.switch_time_month_table[calendar.w_month].switch_time[calendar.w_date].on_hour * 60 +
				                        ConcentratorLocationConfig.switch_time_month_table[calendar.w_month].switch_time[calendar.w_date].on_minute +
				                        strategy->offset_min;
			}
		break;

		default:
		break;
	}
}

//ˢ�¼�������鶯��ʱ��
void RefreshRelayStrategyGroupActionTime(u8 group_id)
{
	u8 i = 0;
	pRelayStrategy head_strategy = NULL;
	pRelayStrategy temp_strategy = NULL;

	if(xSchedulerRunning == 1)
	{
		xSemaphoreTake(xMutex_RelayStrategy, portMAX_DELAY);
	}

	if(group_id >= 1 && group_id <= MAX_RELAY_MODULE_STRATEGY_GROUP_NUM)
	{
		for(i = 0; i < MAX_RELAY_MODULE_STRATEGY_GROUP_NUM; i ++)
		{
			if(group_id == RelayStrategyGroup[i]->group_id)
			{
				head_strategy = RelayStrategyGroup[i];

				break;
			}
		}
	}

	if(head_strategy != NULL)
	{
		for(temp_strategy = head_strategy->next; temp_strategy != NULL; temp_strategy = temp_strategy->next)
		{
			RefreshRelayStrategyActionTime(temp_strategy);
		}
	}

	if(xSchedulerRunning == 1)
	{
		xSemaphoreGive(xMutex_RelayStrategy);
	}
}

//������б��м����²���
void RelayStrategyAdd(pRelayStrategy strategy)
{
	u8 i = 0;
	pRelayStrategy head_strategy = NULL;
	pRelayStrategy temp_strategy = NULL;

	if(xSchedulerRunning == 1)
	{
		xSemaphoreTake(xMutex_RelayStrategy, portMAX_DELAY);
	}

	if(strategy->group_id >= 1 && strategy->group_id <= MAX_RELAY_MODULE_STRATEGY_GROUP_NUM)
	{
		for(i = 0; i < MAX_RELAY_MODULE_STRATEGY_GROUP_NUM; i ++)
		{
			if(strategy->group_id == RelayStrategyGroup[i]->group_id)
			{
				head_strategy = RelayStrategyGroup[i];

				break;
			}
		}
	}

	if(head_strategy != NULL)
	{
		for(temp_strategy = head_strategy; temp_strategy != NULL; temp_strategy = temp_strategy->next)
		{
			if(temp_strategy->next == NULL)
			{
				temp_strategy->next = strategy;
				temp_strategy->next->prev = temp_strategy;

				break;
			}
			else
			{
				if(temp_strategy->action_time <= strategy->action_time &&
				   strategy->action_time < temp_strategy->next->action_time)	//����ʱ���Ⱥ�˳������
				{
					strategy->next = temp_strategy->next;
					temp_strategy->next = strategy;

					strategy->prev = strategy->next->prev;
					strategy->next->prev = strategy;
					
					break;
				}
			}
		}
	}

	if(xSchedulerRunning == 1)
	{
		xSemaphoreGive(xMutex_RelayStrategy);
	}
}

//ɾ�������� ���group_id
void RelayStrategyGroupDelete(u8 group_id)
{
	u8 i = 0;
	pRelayStrategy head_strategy = NULL;
	pRelayStrategy temp_strategy = NULL;

	if(xSchedulerRunning == 1)
	{
		xSemaphoreTake(xMutex_RelayStrategy, portMAX_DELAY);
	}

	if(group_id >= 1 && group_id <= MAX_RELAY_MODULE_STRATEGY_GROUP_NUM)
	{
		for(i = 0; i < MAX_RELAY_MODULE_STRATEGY_GROUP_NUM; i ++)
		{
			if(group_id == RelayStrategyGroup[i]->group_id)
			{
				head_strategy = RelayStrategyGroup[i];

				break;
			}
		}
	}

	if(head_strategy != NULL)
	{
		for(temp_strategy = head_strategy->next; temp_strategy != NULL; temp_strategy = temp_strategy->next)
		{
			if(temp_strategy->next != NULL)
			{
				temp_strategy->prev->next = temp_strategy->next;
				temp_strategy->next->prev = temp_strategy->prev;
			}
			else
			{
				temp_strategy->prev->next = NULL;
			}

			vPortFree(temp_strategy);
		}
	}

	if(xSchedulerRunning == 1)
	{
		xSemaphoreGive(xMutex_RelayStrategy);
	}
}

//ɾ�����в�����
void RelayAllStrategyGroupDelete(void)
{
	u8 i = 0;
	u8 group_id = 0;
	
	for(i = 0; i < MAX_RELAY_MODULE_STRATEGY_GROUP_NUM; i ++)
	{
		if(RelayStrategyGroup[i] != NULL)
		{
			group_id = RelayStrategyGroup[i]->group_id;
		
			RelayStrategyGroupDelete(group_id);
		}
	}
}

//��ԤԼ�����б��м����µ�ԤԼ����
void RelayAppointmentGroupAdd(pRelayAppointment appointment)
{
	pRelayAppointment temp_appointment = NULL;

	if(xSchedulerRunning == 1)
	{
		xSemaphoreTake(xMutex_RelayAppointment, portMAX_DELAY);
	}

	if(RelayAppointmentGroup != NULL)
	{
		for(temp_appointment = RelayAppointmentGroup; temp_appointment != NULL; temp_appointment = temp_appointment->next)
		{
			if(temp_appointment->next == NULL)
			{
				temp_appointment->next = appointment;
				temp_appointment->next->prev = temp_appointment;

				break;
			}
		}
	}

	if(xSchedulerRunning == 1)
	{
		xSemaphoreGive(xMutex_RelayAppointment);
	}
}

//ɾ�������� ���group_id
void RelayAppointmentGroupDelete(void)
{
	pRelayAppointment temp_appointment = NULL;

	if(xSchedulerRunning == 1)
	{
		xSemaphoreTake(xMutex_RelayAppointment, portMAX_DELAY);
	}

	if(RelayAppointmentGroup != NULL)
	{
		for(temp_appointment = RelayAppointmentGroup->next; temp_appointment != NULL; temp_appointment = temp_appointment->next)
		{
			if(temp_appointment->next != NULL)
			{
				temp_appointment->prev->next = temp_appointment->next;
				temp_appointment->next->prev = temp_appointment->prev;
			}
			else
			{
				temp_appointment->prev->next = NULL;
			}

			vPortFree(temp_appointment);
		}
	}

	if(xSchedulerRunning == 1)
	{
		xSemaphoreGive(xMutex_RelayAppointment);
	}
}

//ͨ��������Ż�ȡ������
pRelayStrategy GetRelayStrategyGroupByGroupID(u8 group_id)
{
	u8 i = 0;
	pRelayStrategy strategy = NULL;

	if(group_id >= 1 && group_id <= MAX_RELAY_MODULE_STRATEGY_GROUP_NUM)
	{
		for(i = 0; i < MAX_RELAY_MODULE_STRATEGY_GROUP_NUM; i ++)
		{
			if(group_id == RelayStrategyGroup[i]->group_id)
			{
				strategy = RelayStrategyGroup[i];

				break;
			}
		}
	}

	return strategy;
}

//��ȡ��ǰӦ��ִ�еĲ�����
pRelayStrategy RefreshCurrentRelayStrategyGroup(void)
{
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 index = 0;
	u8 max_priority = 0;
	u8 group_id = 0;
	u32 start_min = 0;
	u32 end_min = 0;
	u32 current_min = 0;
	RelayStrategyGroupID_Priority_S group_id_priority[MAX_RELAY_MODULE_STRATEGY_GROUP_NUM];
	pRelayStrategy strategy = NULL;
	pRelayAppointment temp_appointment = NULL;

	if(RelayStrategyGroupSwitch.group_num == 0)
	{
		group_id = 0;
	}
	else
	{
		switch(RelayStrategyGroupSwitch.type)
		{
			case 1:
				memset(&group_id_priority,0,
			           MAX_RELAY_MODULE_STRATEGY_GROUP_NUM * sizeof(RelayStrategyGroupID_Priority_S));

				for(i = 0; i < RelayStrategyGroupSwitch.group_num; i ++)	//��ģʽ����
				{
					if(RelayAppointmentGroup != NULL)						//ԤԼ���Ʋ�Ϊ��
					{
						for(temp_appointment = RelayAppointmentGroup->next;
						    temp_appointment != NULL;
						    temp_appointment = temp_appointment->next)		//ͨ��group_id����ԤԼ��������
						{
							if(temp_appointment->group_id == RelayStrategyGroupSwitch.group_id[i])	//�ҵ���group_idƥ���ԤԼ��������
							{
								current_min = get_minutes_by_calendar(calendar.w_month,
								                                      calendar.w_date,
								                                      calendar.hour,
								                                      calendar.min);				//��ȡ��ǰϵͳ������

								for(j = 0; j < temp_appointment->time_range_num; j ++)				//��ѵ�жϵ�ǰԤԼ���Ƶ�ʱ������
								{
									if(temp_appointment->range[j].s_month  == 0 &&
									   temp_appointment->range[j].s_date   == 0 &&
									   temp_appointment->range[j].s_hour   == 0 &&
									   temp_appointment->range[j].s_minute == 0 &&
									   temp_appointment->range[j].e_month  == 0 &&
									   temp_appointment->range[j].e_date   == 0 &&
									   temp_appointment->range[j].e_hour   == 0 &&
									   temp_appointment->range[j].e_minute == 0)					//��ʱ��û������
									{
										if(temp_appointment->range[j].week_enable == 0)				//������û������
										{
											group_id_priority[k].group_id = temp_appointment->group_id;
											group_id_priority[k].priority = temp_appointment->priority;
											
											k ++;
										}
										else														//������������
										{
											if(temp_appointment->range[j].week_enable & (1 << (calendar.week - 1)) != 0)
											{
												group_id_priority[k].group_id = temp_appointment->group_id;
												group_id_priority[k].priority = temp_appointment->priority;
												
												k ++;
											}
										}
									}
									else															//��ʱ��������
									{
										start_min = get_minutes_by_calendar(temp_appointment->range[j].s_month,
								                                            temp_appointment->range[j].s_date,
								                                            temp_appointment->range[j].s_hour,
								                                            temp_appointment->range[j].s_minute);	//��ȡ��ǰϵͳ������

										end_min = get_minutes_by_calendar(temp_appointment->range[j].e_month,
								                                          temp_appointment->range[j].e_date,
								                                          temp_appointment->range[j].e_hour,
								                                          temp_appointment->range[j].e_minute);		//��ȡ��ǰϵͳ������

										if(start_min <= end_min)													//��ʼʱ��С�ڵ��ڽ���ʱ�� û�п���
										{
											if(start_min <= current_min && current_min <= end_min)					//ʱ��Ϸ�
											{
												if(temp_appointment->range[j].week_enable == 0)				//������û������
												{
													group_id_priority[k].group_id = temp_appointment->group_id;
													group_id_priority[k].priority = temp_appointment->priority;
													
													k ++;
												}
												else														//������������
												{
													if(temp_appointment->range[j].week_enable & (1 << (calendar.week - 1)) != 0)
													{
														group_id_priority[k].group_id = temp_appointment->group_id;
														group_id_priority[k].priority = temp_appointment->priority;
														
														k ++;
													}
												}
											}
										}
										else																//��ʼʱ����ڽ���ʱ�� ������
										{
											if(start_min <= current_min && current_min <= 527040)			//����������� һ����527040����
											{
												if(temp_appointment->range[i].week_enable == 0)				//������û������
												{
													group_id_priority[k].group_id = temp_appointment->group_id;
													group_id_priority[k].priority = temp_appointment->priority;
													
													k ++;
												}
												else														//������������
												{
													if(temp_appointment->range[i].week_enable & (1 << (calendar.week - 1)) != 0)
													{
														group_id_priority[k].group_id = temp_appointment->group_id;
														group_id_priority[k].priority = temp_appointment->priority;
														
														k ++;
													}
												}
											}
											else if(1 <= current_min && current_min <= end_min)
											{
												if(temp_appointment->range[i].week_enable == 0)				//������û������
												{
													group_id_priority[k].group_id = temp_appointment->group_id;
													group_id_priority[k].priority = temp_appointment->priority;
													
													k ++;
												}
												else														//������������
												{
													if(temp_appointment->range[i].week_enable & (1 << (calendar.week - 1)) != 0)
													{
														group_id_priority[k].group_id = temp_appointment->group_id;
														group_id_priority[k].priority = temp_appointment->priority;
														
														k ++;
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

				if(k > 0)
				{
					if(k == 1)		//ɸѡ��һ��������
					{
						group_id = group_id_priority[0].group_id;
					}
					else			//ɸѡ�����������,��Ա�ԤԼ�������ȼ�
					{
						max_priority = group_id_priority[0].priority;

						for(i = 0; i < k; i ++)
						{
							if(max_priority < group_id_priority[i].priority)
							{
								max_priority = group_id_priority[i].priority;

								index = i;
							}
						}

						group_id = group_id_priority[index].group_id;
					}
				}
			break;

			case 2:
				if(RelayStrategyGroupSwitch.group_num == 1)
				{
					group_id = RelayStrategyGroupSwitch.group_id[0];
				}
			break;

			default:
			break;
		}
	}

	strategy = GetRelayStrategyGroupByGroupID(group_id);

	return strategy;
}



























