#ifndef __INPUT_COLLECTOR_CONF_H
#define __INPUT_COLLECTOR_CONF_H

#include "sys.h"
#include "common.h"

#define MAX_INPUT_COLLECTOR_CONF_NUM				10	//最多配置10个输入量采集模块
#define MAX_INPUT_COLLECTOR_MODULE_NAME_LEN			16	//模块型号长度
#define MAX_INPUT_COLLECTOR_D_LOOP_CH_NUM			8	//数字量通道数
#define MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM			8	//模拟量通道数
#define MAX_INPUT_COLLECTOR_ALARM_TYPE_NUM			8	//告警类型数





typedef struct	InputCollectorD_Alarm										//数字通道告警配置
{
	u8 channel;																//检测端子
	u8 alarm_level;															//告警电平
	u8 confirm_time;														//确认次数
	u8 last_run_mode;														//上个运行状态
	u8 switch_run_mode;														//手自控切换
	u8 resume_run_mode;														//手自控恢复
	u8 relay_action;														//回路动作

}__attribute__((packed))InputCollectorD_Alarm_S;

typedef struct	InputCollectorA_Alarm										//模拟通道告警配置
{
	u8 channel;																//检测端子
	double min_value;														//最小值
	double max_value;														//最大值
	u8 confirm_time;														//确认次数
	u8 last_run_mode;														//上个运行状态
	u8 switch_run_mode;														//手自控切换
	u8 resume_run_mode;														//手自控恢复
	u8 relay_action;														//回路动作

}__attribute__((packed))InputCollectorA_Alarm_S;

typedef struct	InputCollectorBasicConfig									//输入量采集模块配置
{
	u16 detect_interval;													//检测间隔
	u8 auto_report;															//自动上报

	u16 crc16;																//校验码 存储用

}__attribute__((packed))InputCollectorBasicConfig_S;

typedef struct	InputCollectorConfig										//输入量采集模块配置
{
	u8 address;																//通讯地址
	u8 channel;																//通讯通道
	u8 d_loop_num;															//数字量回路总数
	u8 a_loop_num;															//模拟量回路总数
	float a_quantity_range;													//模拟量变化幅度
	u8 confirm_time;														//确认次数
	u8 module[MAX_INPUT_COLLECTOR_MODULE_NAME_LEN];							//模块型号
	InputCollectorD_Alarm_S d_alarm_thre[MAX_INPUT_COLLECTOR_D_LOOP_CH_NUM];//数字量告警阈值
	InputCollectorA_Alarm_S a_alarm_thre[MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM];//数字量告警阈值

	u16 crc16;																//校验码 存储用

}__attribute__((packed))InputCollectorConfig_S;

typedef struct	InputCollectorAlarmConfig	//输入量采集模块告警参数配置
{
	u8 d_quantity_abnormal_alarm_enable;	//数字量异常使能
	u8 a_quantity_abnormal_alarm_enable;	//模拟量异常使能

	u16 crc16;								//校验码 存储用

}__attribute__((packed))InputCollectorAlarmConfig_S;

typedef struct	InputCollectorState														//输入量检测模块状态
{
	u8 address;																			//通讯地址
	u8 channel;																			//通讯通道
	
	u8 d_channel_bit;																	//模块拥有数字量通道状态
	u8 a_channel_bit;																	//模块拥有模拟量通道状态
	u8 d_mirror_state;																	//数字量通道上个状态
	u8 d_current_state;																	//数字量通道当前状态
	double a_mirror_state[MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM];							//模拟量通道上个状态
	double a_current_state[MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM];							//模拟量通道当前状态
	u16 d_abnormal_loop;																//数字异常回路
	u16 a_abnormal_loop;																//模拟异常回路
	u8 d_alarm[MAX_INPUT_COLLECTOR_D_LOOP_CH_NUM][MAX_INPUT_COLLECTOR_ALARM_TYPE_NUM];	//数字回路告警
	u8 a_alarm[MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM][MAX_INPUT_COLLECTOR_ALARM_TYPE_NUM];	//模拟回路告警

}__attribute__((packed))InputCollectorState_S;

typedef struct	InputCollectorCollectState											//输入量采集模块采集状态
{
	u8 address;																		//通讯地址
	u8 channel;																		//通讯通道
	u8 update;																		//更新标志
	u16 d_collect_state;															//数字量状态
	double a_collect_state[MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM];						//模拟量状态

}__attribute__((packed))InputCollectorCollectState_S;







extern InputCollectorBasicConfig_S InputCollectorBasicConfig;
extern Uint32TypeNumber_S InputCollectorConfigNum;
extern InputCollectorConfig_S InputCollectorConfig[MAX_INPUT_COLLECTOR_CONF_NUM];
extern InputCollectorAlarmConfig_S InputCollectorAlarmConfig;

extern InputCollectorState_S InputCollectorState[MAX_INPUT_COLLECTOR_CONF_NUM];










void ReadInputCollectorBasicConfig(void);
void WriteInputCollectorBasicConfig(u8 reset,u8 write_enable);
void ReadInputCollectorConfigNum(void);
void WriteInputCollectorConfigNum(u8 reset,u8 write_enable);
void ReadInputCollectorConfig(void);
void WriteInputCollectorConfig(u8 i,u8 reset,u8 write_enable);
void ReadInputCollectorAlarmConfig(void);
void WriteInputCollectorAlarmConfig(u8 reset,u8 write_enable);

























#endif
