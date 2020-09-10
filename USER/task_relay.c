#include "task_relay.h"
#include "delay.h"
#include "common.h"
#include "server_protocol.h"
#include "relay_comm.h"
#include "relay_event.h"
#include "sun_rise_set.h"
#include "rx8010s.h"
#include "kc.h"
#include "concentrator_conf.h"
#include "lumeter_conf.h"




TaskHandle_t xHandleTaskRELAY = NULL;
unsigned portBASE_TYPE SatckRELAY;

void vTaskRELAY(void *pvParameters)
{
	while(1)
	{
		RelayRecvAndHandleFrameStruct();				//报文解析

		RelayEventCheckPolling();						//告警等事件轮训

		RelayExecuteStrategyGroup();					//轮训普通策略
		
		RelayExecuteTemporaryStrategyGroup();			//轮训临时策略
		
		RelayCheckForceSwitchOffAllRelays();			//检测是否有告警联动

		RelayExecuteActions();							//执行继电器动作

		RelayCollectCurrentState();						//采集继电器模块状态

		RelayRecvAndHandleDeviceFrame();				//接收并处理外置继电器模块返回的数据

		delay_ms(100);
		
		SatckRELAY = uxTaskGetStackHighWaterMark(NULL);
	}
}

//执行策略
void RelayExecuteStrategyGroup(void)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	pRelayStrategy strategy = NULL;
	u16 gate0 = 0;
	u16 gate24 = 1440;	//24*60;
	u16 gate1 = 0;
	u16 gate2 = 0;
	u16 gate_n = 0;
	static u32 Illuminance_value = 0;
	static time_t time_s = 0;

	static SunRiseSetTime_S sun_rise_set_time;

	if(GetSysTick1s() - time_s >= 1)
	{
		time_s = GetSysTick1s();

		if(RelayRefreshStrategyGroup == 1)
		{
			RelayRefreshStrategyGroup = 0;

			RelayAllStrategyGroupDelete();
			RelayAppointmentGroupDelete();

			CurrentRelayStrategyGroup = NULL;
			CurrentRelayStrategyGroupTemp = NULL;

			ReadRelayStrategyGroups();
			ReadRelayAppointmentGroup();
			ReadRelayStrategyGroupSwitch();

			CurrentRelayStrategyGroup = RefreshCurrentRelayStrategyGroup();
		}

		if(sun_rise_set_time.rise_h != SunRiseSetTime.rise_h ||
		   sun_rise_set_time.rise_m != SunRiseSetTime.rise_m ||
		   sun_rise_set_time.set_h != SunRiseSetTime.set_h ||
		   sun_rise_set_time.set_m != SunRiseSetTime.set_m ||
		   Illuminance_value != LumeterAppValue)
		{
			sun_rise_set_time.rise_h = SunRiseSetTime.rise_h;
			sun_rise_set_time.rise_m = SunRiseSetTime.rise_m;
			sun_rise_set_time.set_h = SunRiseSetTime.set_h;
			sun_rise_set_time.set_m = SunRiseSetTime.set_m;
			Illuminance_value = LumeterAppValue;

			RefreshRelayStrategyGroupActionTime(CurrentRelayStrategyGroup->group_id,Illuminance_value);
		}

		if(CurrentRelayStrategyGroup != NULL && CurrentRelayStrategyGroup->next != NULL)		//判断策略列表是否不为空
		{
			for(strategy = CurrentRelayStrategyGroup->next; strategy != NULL; strategy = strategy->next)	//轮训策略列表
			{
				gate_n = calendar.hour * 60 + calendar.min;

				if(gate_n == strategy->action_time)
				{
					ret = 1;
				}
				else if(strategy->next != NULL)
				{
					if(gate_n == strategy->next->action_time)
					{
						strategy = strategy->next;

						ret = 1;
					}
					else
					{
						gate1 = strategy->action_time;					//该条策略的分钟数
						gate2 = strategy->next->action_time;			//该条策略的next的分钟数

						if(gate1 < gate2)								//该条策略时间早于next的时间
						{
							if(gate1 <= gate_n && gate_n <= gate2)		//判断当前时间是否在两条策略时间段中间
							{
								ret = 1;
							}
						}
						else if(gate1 > gate2)							//该条策略时间晚于next的时间
						{
							if(gate1 <= gate_n && gate_n <= gate24)		//判断当前时间是否在该条策略时间和24点时间段中间
							{
								ret = 1;
							}
							else if(gate0 <= gate_n && gate_n <= gate2)	//判断当前时间是否在0点和next的时间段中间
							{
								ret = 1;
							}
						}
					}
				}
				else
				{
					ret = 1;
				}

				if(ret == 1)
				{
					for(i = 0; i < strategy->action_num; i ++)
					{
						for(j = 0; j < RelayModuleConfigNum.number; j ++)
						{
							if(strategy->action[i].module_address == RelayModuleState[j].address &&
							   strategy->action[i].module_channel == RelayModuleState[j].channel)
							{
								RelayModuleState[j].loop_task_channel = strategy->action[i].loop_channel & RelayModuleConfig[j].loop_enable;
								RelayModuleState[j].loop_task_state = strategy->action[i].loop_action;

								j = RelayModuleConfigNum.number;			//跳出循环
							}
						}
					}

					goto UPDATE_RELAY_STATE;
				}
			}
		}
	}

	UPDATE_RELAY_STATE:
	for(i = 0; i < RelayModuleConfigNum.number; i ++)
	{
		if(RelayModuleState[i].loop_last_task_channel != RelayModuleState[i].loop_task_channel ||
		   RelayModuleState[i].loop_last_task_state != RelayModuleState[i].loop_task_state)
		{
			RelayModuleState[i].loop_last_task_channel = RelayModuleState[i].loop_task_channel;
			RelayModuleState[i].loop_last_task_state = RelayModuleState[i].loop_task_state;

			RelayModuleState[i].loop_current_channel = RelayModuleState[i].loop_task_channel;
			RelayModuleState[i].loop_current_state = RelayModuleState[i].loop_task_state;

			RelayModuleState[i].controller = 6;

			TimeToString(RelayModuleState[j].control_time,
						 calendar.w_year,
						 calendar.w_month,
						 calendar.w_date,
						 calendar.hour,
						 calendar.min,
						 calendar.sec);
		}
	}
}

//执行临时的策略(临时模式控制)
void RelayExecuteTemporaryStrategyGroup(void)
{
	u8 i = 0;
	u8 j = 0;
	pRelayStrategy strategy = NULL;
	u16 gate_n = 0;
	u8 hour = 0;
	u8 min = 0;
	char tmp[5] = {0};

	if(RelayStrategyGroupSwitchTemp.type == 3)
	{
		CurrentRelayStrategyGroupTemp = GetRelayStrategyGroupByGroupID(RelayStrategyGroupSwitchTemp.group_id[0]);

		memset(tmp,0,5);
		memcpy(tmp,&RelayStrategyGroupSwitchTemp.time[8],2);
		hour = myatoi(tmp);

		memset(tmp,0,5);
		memcpy(tmp,&RelayStrategyGroupSwitchTemp.time[10],2);
		min = myatoi(tmp);

		gate_n = hour * 60 + min;

		if(CurrentRelayStrategyGroupTemp != NULL && CurrentRelayStrategyGroupTemp->next != NULL)		//判断策略列表是否不为空
		{
			for(strategy = CurrentRelayStrategyGroupTemp->next; strategy != NULL; strategy = strategy->next)	//轮训策略列表
			{
				if(gate_n == strategy->action_time)
				{
					for(i = 0; i < strategy->action_num; i ++)
					{
						for(j = 0; j < RelayModuleConfigNum.number; j ++)
						{
							if(strategy->action[i].module_address == RelayModuleState[j].address &&
							   strategy->action[i].module_channel == RelayModuleState[j].channel)
							{
								RelayModuleState[j].loop_current_channel = strategy->action[i].loop_channel & RelayModuleConfig[j].loop_enable;
								RelayModuleState[j].loop_current_state = strategy->action[i].loop_action;

								RelayModuleState[i].controller = 3;

								TimeToString(RelayModuleState[j].control_time,
											 calendar.w_year,
											 calendar.w_month,
											 calendar.w_date,
											 calendar.hour,
											 calendar.min,
											 calendar.sec);

								j = RelayModuleConfigNum.number;			//跳出循环
							}
						}
					}
					
					RelayStrategyGroupSwitchTemp.type = 0;
				}
			}
		}

		CurrentRelayStrategyGroupTemp = NULL;
	}
}

//强制关闭所有继电器
void RelayCheckForceSwitchOffAllRelays(void)
{
	u8 i = 0;
	
	if(RelayForceSwitchOffAllRelays == 1)		//强制关闭所有继电器
	{
		RelayForceSwitchOffAllRelays = 0;
		
		for(i = 0; i < RelayModuleConfigNum.number; i ++)
		{
			RelayModuleState[i].loop_last_channel = 0;
			RelayModuleState[i].loop_last_state = 0;
			
			RelayModuleState[i].loop_current_channel = 0xFFFF & RelayModuleState[i].loop_channel_bit;
			RelayModuleState[i].loop_current_state = 0;
			
			RelayModuleState[i].controller = 5;
			
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

//继电器执行动作
void RelayExecuteActions(void)
{
	u8 i = 0;
	u8 execute = 0;
	
	for(i = 0; i < RelayModuleConfigNum.number; i ++)
	{
		if(RelayModuleState[i].loop_last_channel != RelayModuleState[i].loop_current_channel ||
		   RelayModuleState[i].loop_last_state != RelayModuleState[i].loop_current_state ||
		   RelayModuleState[i].execute_immediately == 1)
		{
			RelayModuleState[i].execute_immediately = 0;

			execute = 0;
			
			if(RunMode == MODE_AUTO ||
			  (RunMode == MODE_MANUAL && 
			  (RelayModuleState[i].controller == 1 || 
			   RelayModuleState[i].controller == 2 || 
			   RelayModuleState[i].controller == 4)) ||
			  (RunMode == MODE_TASK && 
			  (RelayModuleState[i].controller == 3 || 
			   RelayModuleState[i].controller == 6)) ||
			   RelayModuleState[i].controller == 5)
			{
				execute = 1;
			}
			else if(RunMode == MODE_STOP)
			{
				execute = 0;
			}
			
			if(execute == 1)
			{
				if(RelayModuleState[i].address == 0 && RelayModuleState[i].channel == 0)	//集控内部继电器
				{
					ControlAllBuiltInRelay(RelayModuleState[i]);
				}
				else if(RelayModuleState[i].channel == 1)									//集控外部继电器模块
				{
					ControlAllBuiltOutRelay(RelayModuleState[i]);
				}
			}

			RelayModuleState[i].loop_last_channel = RelayModuleState[i].loop_current_channel;
			RelayModuleState[i].loop_last_state = RelayModuleState[i].loop_current_state;
		}
	}
}

//采集继电器模块状态
void RelayCollectCurrentState(void)
{
	u8 i = 0;
	static time_t time_s = 0;

	if(RelayModuleBasicConfig.state_monitoring_cycle != 0)
	{
		if(GetSysTick1s() - time_s >= (RelayModuleBasicConfig.state_monitoring_cycle * 60))
		{
			time_s = GetSysTick1s();
			
			for(i = 0; i < RelayModuleConfigNum.number; i ++)
			{
				if(RelayModuleState[i].address == 0 && RelayModuleState[i].channel == 0)	//集控内部继电器
				{
					RelayModuleState[i].loop_collect_state = RelayModuleState[i].loop_current_state;
				}
				else if(RelayModuleState[i].channel == 1)									//集控外部继电器模块
				{
					GetBuiltOutRelayState(RelayModuleState[i]);
				}
			}
		}
	}
}

//接收并处理外置继电器模块返回的数据
void RelayRecvAndHandleDeviceFrame(void)
{
	u8 i = 0;
	Rs485Frame_S *recv_rs485_frame = NULL;
	BaseType_t xResult;
	RelayModuleCollectState_S collect_state;

	xResult = xQueueReceive(xQueue_RelayRs485Frame,(void *)&recv_rs485_frame,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS)
	{
		AnalysisBuiltOutRelayFrame(recv_rs485_frame->buf,recv_rs485_frame->len,&collect_state);

		if(collect_state.update == 1)	//外置继电器模块采集状态有更新
		{
			collect_state.update = 0;

			for(i = 0; i < RelayModuleConfigNum.number; i ++)
			{
				if(RelayModuleState[i].address == collect_state.address &&
				   RelayModuleState[i].channel == collect_state.channel)
				{
					RelayModuleState[i].loop_collect_state = collect_state.loop_collect_state;
					
					break;
				}
			}
		}
		
		DeleteRs485Frame(recv_rs485_frame);
	}
}



















































