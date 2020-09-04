#ifndef __RELAY_CONF_H
#define __RELAY_CONG_H

#include "sys.h"
#include "common.h"


#define MAX_RELAY_MODULE_CONF_NUM					5	//最多配置5个继电器模块
#define MAX_RELAY_MODULE_STRATEGY_GROUP_NUM			10	//最多10个策略模式(组)
#define MAX_RELAY_MODULE_APPOINTMENT_NUM			10	//最多10个预约控制
#define MAX_RELAY_MODULE_STRATEGY_NUM				300	//最多300策略
#define MAX_RELAY_MODULE_GROUP_STRATEGY_NUM			30	//每个模式(组)最多30个策略
#define MAX_RELAY_MODULE_APPOINTMENT_TIME_RANGE_NUM	10	//每个预约控制最多10个时间段

#define MAX_RELAY_MODULE_MODULE_NAME_LEN			16
#define MAX_RELAY_MODULE_LOOP_CH_NUM				12
#define MAX_RELAY_MODULE_LOOP_GROUP_NUM				16
#define MAX_RELAY_MODULE_ALARM_TYPE_NUM				8

#define MAX_RELAY_MODULES_LOOP_CH_NUM				MAX_RELAY_MODULE_LOOP_CH_NUM * MAX_RELAY_MODULE_CONF_NUM	//系统可配置的回路个数最大值



typedef enum
{
	TYPE_FIXED_TIME			= 1,			//定时
	TYPE_FIXED_TIME_LIGHT 	= 2,			//定时光控
	TYPE_LOCATION 			= 3,			//经纬度
	TYPE_LOCATION_LIGHT 	= 4,			//经纬度光控
	TYPE_CHRONOLOGY			= 5,			//年表
	TYPE_CHRONOLOGY_LIGHT	= 6,			//年表光控

} STRATEGY_TYPE_E;

typedef struct	RelayModuleConfig													//继电器模块配置
{
	u8 address;																		//通讯地址
	u8 channel;																		//通讯通道
	u8 loop_num;																	//回路总数
	u16 interval_time;																//动作间隔延时
	u8 module[MAX_RELAY_MODULE_MODULE_NAME_LEN];									//模块型号
	u16 loop_enable;																//回路启用状态
	u8 loop_group[MAX_RELAY_MODULE_LOOP_CH_NUM][MAX_RELAY_MODULE_LOOP_GROUP_NUM];	//回路组号
	u8 loop_alarm_thre[MAX_RELAY_MODULE_LOOP_CH_NUM][5];							//回路触点异常阈值

	u16 crc16;																		//校验码 存储用

}__attribute__((packed))RelayModuleConfig_S;

typedef struct	RelayModuleState													//继电器模块状态
{
	u8 address;																		//通讯地址
	u8 channel;																		//通讯通道
	
	u16 interval_time;																//动作间隔延时
	
	u8 execute_immediately;															//立即执行
	
	u16 loop_channel_bit;															//模块拥有回路状态
	u16 loop_last_channel;															//回路上个控制通道
	u16 loop_current_channel;														//回路当前控制通道
	u16 loop_last_task_channel;														//回路上个任务控制通道
	u16 loop_task_channel;															//回路当前任务控制通道
	
	u16 loop_mirror_state;															//对比回路状态
	u16 loop_last_state;															//回路上个状态
	u16 loop_current_state;															//回路当前状态
	u16 loop_collect_state;															//回路采集状态
	u16 loop_last_task_state;														//回路上个任务状态
	u16 loop_task_state;															//回路当前任务状态
	
	u8 controller;																	//控制来源
	u8 control_time[15];															//控制时间
	
	u16 abnormal_loop;																//异常回路
	u8 loop_alarm[MAX_RELAY_MODULE_LOOP_CH_NUM][MAX_RELAY_MODULE_ALARM_TYPE_NUM];	//回路告警

}__attribute__((packed))RelayModuleState_S;

typedef struct	RelayModuleCollectState												//继电器模块采集状态
{
	u8 address;																		//通讯地址
	u8 channel;																		//通讯通道
	u8 update;																		//更新标志
	u16 loop_collect_state;															//回路采集状态

}__attribute__((packed))RelayModuleCollectState_S;

typedef struct	RelayModuleBasicConfig		//继电器模块基础配置
{
	u8 on_hour;								//开灯时
	u8 on_minute;							//开灯分
	u8 off_hour;							//关灯时
	u8 off_minute;							//关灯分
	
	u16 state_monitoring_cycle;				//回路状态监测周期 min
	u8 state_recording_time;				//回路状态记录时长 day
	
	u16 crc16;								//校验码 存储用
	
}__attribute__((packed))RelayModuleBasicConfig_S;

typedef struct	RelayAlarmConfig			//继电器模块告警配置参数
{
	u8 relay_abnormal_alarm_enable;			//继电器异常使能
	u8 contact_abnormal_alarm_enable;		//触点异常使能
	u8 task_abnormal_alarm_enable;			//任务内状态异常使能

	u16 crc16;								//校验码 存储用

}__attribute__((packed))RelayAlarmConfig_S;

typedef struct	RelayTaskAction				//继电器任务动作
{
	u8 module_address;						//模块地址
	u8 module_channel;						//模块回路
	u16 loop_channel;						//回路通道
	u16 loop_action;						//回路动作

}__attribute__((packed))RelayTaskAction_S;

typedef struct	RelayTask									//继电器任务内容
{
	u8 group_id;											//模式(组)编号
	u8 type;												//任务类型
	s16 time;												//动作时间
	u8 time_option;											//时间选项
	u8 action_num;											//动作数量
	
	RelayTaskAction_S action[MAX_RELAY_MODULE_CONF_NUM];	//任务动作
	
	u16 crc16;												//校验码 存储用
	
}__attribute__((packed))RelayTask_S;

typedef struct	RelaySenceConfig									//继电器模块场景模式配置
{
	u8 group_id;													//模式(组)编号
	u8 priority;													//优先级
	u8 time_range_num;												//有效时间范围数量
	TimeRange_S range[MAX_RELAY_MODULE_APPOINTMENT_TIME_RANGE_NUM];	//任务动作
	
	u16 crc16;														//校验码 存储用

}__attribute__((packed))RelaySenceConfig_S;

typedef struct	RelayStrategyGroupSwitch							//模式切换配置
{
	u8 group_num;													//策略组数量
	u8 group_id[MAX_RELAY_MODULE_STRATEGY_GROUP_NUM];				//策略组号
	u8 type;														//切换方式
	u8 time[15];													//切换时间
	
	u16 crc16;														//校验码 存储用

}__attribute__((packed))RelayStrategyGroupSwitch_S;


#define RelayStrategyGroupID_Priority_S struct RelayStrategyGroupID_Priority
typedef struct RelayStrategyGroupID_Priority *RelayStrategyGroupID_Priority;
struct RelayStrategyGroupID_Priority								//策略组号和优先级
{
	u8 group_id;													//模式(组)编号
	u8 priority;													//优先级
	
}__attribute__((packed));

#define RelayAppointment_S struct RelayAppointment
typedef struct RelayAppointment *pRelayAppointment;
struct RelayAppointment												//场景配置(预约控制)
{
	u8 group_id;													//模式(组)编号
	u8 priority;													//优先级
	
	u8 time_range_num;												//有效时间范围数量
	TimeRange_S range[MAX_RELAY_MODULE_APPOINTMENT_TIME_RANGE_NUM];	//任务动作

	pRelayAppointment prev;
	pRelayAppointment next;
}__attribute__((packed));

#define RelayStrategy_S struct RelayStrategy
typedef struct RelayStrategy *pRelayStrategy;
struct RelayStrategy										//模式配置(策略配置)
{
	u8 group_id;											//模式(组)编号
	u8 type;												//任务类型

	u16 action_time;										//动作时间
	
	s16 offset_min;											//偏移时间
	u8 time_option;											//时间选项

	u8 action_num;											//动作个数
	RelayTaskAction_S action[MAX_RELAY_MODULE_CONF_NUM];	//任务动作

	pRelayStrategy prev;
	pRelayStrategy next;
}__attribute__((packed));





extern u8 RelayForceSwitchOffAllRelays;											//强制断开所有继电器
extern u8 RelayRefreshStrategyGroup;											//继电器策略刷新标志

extern Uint32TypeNumber_S RelayModuleConfigNum;									//继电器模块配置数量
extern RelayModuleConfig_S RelayModuleConfig[MAX_RELAY_MODULE_CONF_NUM];		//继电器模块配置
extern RelayModuleBasicConfig_S RelayModuleBasicConfig;							//默认开关灯时间
extern RelayAlarmConfig_S RelayAlarmConfig;										//继电器模块告警参数配置
extern Uint32TypeNumber_S RelayAppointmentNum;									//继电器模块预约控制数量
extern Uint32TypeNumber_S RelayStrategyNum;										//继电器模块策略配置数量
extern RelayModuleState_S RelayModuleState[MAX_RELAY_MODULE_CONF_NUM];			//继电器模块当前状态

extern pRelayAppointment RelayAppointmentGroup;									//预约控制组
extern pRelayStrategy RelayStrategyGroup[MAX_RELAY_MODULE_STRATEGY_GROUP_NUM];	//策略组
extern RelayStrategyGroupSwitch_S RelayStrategyGroupSwitch;						//策略组(模式)切换配置
extern RelayStrategyGroupSwitch_S RelayStrategyGroupSwitchTemp;					//策略组(模式)切换配置(临时)
extern pRelayStrategy CurrentRelayStrategyGroup;								//当前策略组(模式)
extern pRelayStrategy CurrentRelayStrategyGroupTemp;							//当前策略组(模式)(临时)






void ReadRelayModuleConfigNum(void);
void WriteRelayModuleConfigNum(u8 reset,u8 write_enable);
void ReadRelayModuleConfig(void);
void WriteRelayModuleConfig(u8 i,u8 reset,u8 write_enable);
void ReadRelayModuleBasicConfig(void);
void WriteRelayModuleBasicConfig(u8 reset,u8 write_enable);
void ReadRelayAlarmConfig(void);
void WriteRelayAlarmConfig(u8 reset,u8 write_enable);
void ReadRelayAppointmentNum(void);
void WriteRelayAppointmentNum(u8 reset,u8 write_enable);
u8 ReadRelayAppointment(u8 i,RelaySenceConfig_S *appointment);
void WriteRelayAppointment(u8 i,RelaySenceConfig_S *appointment,u8 reset,u8 write_enable);
void ReadRelayStrategyNum(void);
void WriteRelayStrategyNum(u8 reset,u8 write_enable);
u8 ReadRelayStrategy(u8 i,RelayTask_S *strategy);
void WriteRelayStrategy(u8 i,RelayTask_S *strategy,u8 reset,u8 write_enable);
void ReadRelayAppointmentGroup(void);
void ReadRelayStrategyGroups(void);
void ReadRelayStrategyGroupSwitch(void);
void WriteRelayStrategyGroupSwitch(u8 reset,u8 write_enable);

void RefreshRelayStrategyActionTime(pRelayStrategy strategy,u32 illuminance_value);
void RefreshRelayStrategyGroupActionTime(u8 group_id,u32 illuminance_value);
void RelayStrategyAdd(pRelayStrategy strategy);
void RelayStrategyGroupDelete(u8 group_id);
void RelayAllStrategyGroupDelete(void);
void RelayAppointmentGroupAdd(pRelayAppointment appointment);
void RelayAppointmentGroupDelete(void);
pRelayStrategy GetRelayStrategyGroupByGroupID(u8 group_id);
pRelayStrategy RefreshCurrentRelayStrategyGroup(void);




















#endif
