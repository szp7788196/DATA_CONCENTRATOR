#ifndef __RELAY_CONF_H
#define __RELAY_CONG_H

#include "sys.h"


#define MAX_RELAY_MODULE_CONF_NUM					5	//最多配置5个继电器模块
#define MAX_RELAY_MODULE_STRATEGY_GROUP_NUM			10	//最多10个策略模式(组)
#define MAX_RELAY_MODULE_APPOINTMENT_NUM			10	//最多10个预约控制
#define MAX_RELAY_MODULE_STRATEGY_NUM				300	//最多300策略
#define MAX_RELAY_MODULE_GROUP_STRATEGY_NUM			30	//每个模式(组)最多30个策略
#define MAX_RELAY_MODULE_APPOINTMENT_TIME_RANGE_NUM	10	//每个预约控制最多10个时间段



typedef enum
{
	TYPE_FIXED_TIME			= 1,			//定时
	TYPE_FIXED_TIME_LIGHT 	= 2,			//定时光控
	TYPE_LOCATION 			= 3,			//经纬度
	TYPE_LOCATION_LIGHT 	= 4,			//经纬度光控
	TYPE_CHRONOLOGY			= 5,			//年表
	TYPE_CHRONOLOGY_LIGHT	= 6,			//年表光控

} STRATEGY_TYPE_E;

typedef struct	Uint32TypeNumber			//整形数据
{
	u32 number;								//数字
	
	u16 crc16;								//校验码 存储用
	
}__attribute__((packed))Uint32TypeNumber_S;

typedef struct	RelayModuleConfig			//继电器模块配置
{
	u8 address;								//通讯地址
	u8 channel;								//通讯通道
	u8 loop_num;							//回路总数
	u16 interval_time;						//动作间隔延时
	u8 module[16];							//模块型号
	u16 loop_enable;						//回路启用状态
	u8 loop_group[12][16];					//回路组号
	u8 loop_alarm_thre[12][5];				//回路触点异常阈值

	u16 crc16;								//校验码 存储用

}__attribute__((packed))RelayModuleConfig_S;

typedef struct	RelayModuleState			//继电器模块状态
{
	u8 address;								//通讯地址
	u8 channel;								//通讯通道
	u16 loop_channel;						//回路通道
	u16 loop_current_state;					//回路当前状态
	u16 loop_task_state;					//回路任务状态
	u8 controller;							//控制来源
	u8 control_time[15];					//控制时间
	u16 abnormal_loop;						//异常回路
	u8 loop_alarm[12][8];					//回路告警

}__attribute__((packed))RelayModuleState_S;

typedef struct	DefaultSwitchTime			//默认开关灯时间
{
	u8 on_hour;								//开灯时
	u8 on_minute;							//开灯分
	u8 off_hour;							//关灯时
	u8 off_minute;							//关灯分
	
	u16 crc16;								//校验码 存储用
	
}__attribute__((packed))DefaultSwitchTime_S;

typedef struct	RelayAlarmConfig			//继电器模块告警配置参数
{
	u8 relay_abnormal_alarm_enable;			//继电器异常使能
	u8 contact_abnormal_alarm_enable;		//触点异常使能
	u8 task_abnormal_alarm_enable;			//任务内状态异常使能

	u16 crc16;								//校验码 存储用

}__attribute__((packed))RelayAlarmConfig_S;

typedef struct	TimeRange					//时间段
{
	u8 s_month;								//起始月
	u8 s_date;								//起始日
	u8 s_hour;								//起始时
	u8 s_minute;							//起始分
	
	u8 e_month;								//终止月
	u8 e_date;								//终止日
	u8 e_hour;								//终止时
	u8 e_minute;							//终止分
	
	u8 week_enable;							//星期限制

}__attribute__((packed))TimeRange_S;

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






extern Uint32TypeNumber_S RelayModuleConfigNum;									//继电器模块配置数量
extern RelayModuleConfig_S RelayModuleConfig[MAX_RELAY_MODULE_CONF_NUM];		//继电器模块配置
extern DefaultSwitchTime_S DefaultSwitchTime;									//默认开关灯时间
extern RelayAlarmConfig_S RelayAlarmConfig;										//继电器模块告警参数配置
extern Uint32TypeNumber_S RelayAppointmentNum;									//继电器模块预约控制数量
extern Uint32TypeNumber_S RelayStrategyNum;										//继电器模块策略配置数量

extern pRelayAppointment RelayAppointmentGroup;									//预约控制组
extern pRelayStrategy RelayStrategyGroup[MAX_RELAY_MODULE_STRATEGY_GROUP_NUM];	//策略组
extern RelayStrategyGroupSwitch_S RelayStrategyGroupSwitch;						//策略组(模式)切换配置
extern pRelayStrategy CurrentRelayStrategyGroup;								//当前策略组(模式)






void ReadRelayModuleConfigNum(void);
void WriteRelayModuleConfigNum(u8 reset,u8 write_enable);
void ReadRelayModuleConfig(void);
void WriteRelayModuleConfig(u8 i,u8 reset,u8 write_enable);
void ReadDefaultSwitchTime(void);
void WriteDefaultSwitchTime(u8 reset,u8 write_enable);
void ReadRelayAlarmConfig(void);
void WriteRelayAlarmConfig(u8 reset,u8 write_enable);
void ReadRelayAppointmentNum(void);
void WriteRelayAppointmentNum(u8 reset,u8 write_enable);
void ReadRelayStrategyNum(void);
void WriteRelayStrategyNum(u8 reset,u8 write_enable);
void ReadRelayAppointmentGroup(void);
void ReadRelayStrategyGroups(void);
void ReadRelayStrategyGroupSwitch(void);
void WriteRelayStrategyGroupSwitch(u8 reset,u8 write_enable);

void RefreshRelayStrategyActionTime(pRelayStrategy strategy);
void RefreshRelayStrategyGroupActionTime(u8 group_id);
void RelayStrategyAdd(pRelayStrategy strategy);
void RelayStrategyDelete(u8 group_id);
void RelayAppointmentGroupAdd(pRelayAppointment appointment);
void RelayAppointmentGroupDelete(void);
pRelayStrategy GetRelayStrategyGroupByGroupID(u8 group_id);
pRelayStrategy RefreshCurrentRelayStrategyGroup(void);




















#endif
