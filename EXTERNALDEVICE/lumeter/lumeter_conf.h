#ifndef __LUMETER_CONF_H
#define __LUMETER_CONF_H

#include "sys.h"
#include "common.h"


#define MAX_LUMETER_CONF_NUM					4			//最多配置4个光照计
#define MAX_LUMETER_NAME_LEN					16
#define MAX_LUMETER_STACK_LEN					100



typedef struct	LumeterBasicConfig							//光照计基础配置
{
	u16 collect_cycle;										//采集周期
	u8 stack_depth;											//堆栈深度
	u8 ignore_num;											//忽略数量
	u16 auto_report_cycle;									//自动上报周期
	u8 data_source;											//数据来源
	u32 light_on_thre;										//开灯光照值
	u16 light_on_advance_time;								//提前有效时间
	u16 light_on_delay_time;								//延后有效时间
	u32 light_off_thre;										//关灯光照值
	u16 light_off_advance_time;								//提前有效时间
	u16 light_off_delay_time;								//延后有效时间
	
	u16 crc16;												//校验码 存储用

}__attribute__((packed))LumeterBasicConfig_S;

typedef struct	LumeterConfig								//光照计配置
{
	u8 address;												//通讯地址
	u8 channel;												//通讯通道
	u8 module[MAX_LUMETER_NAME_LEN];						//模块型号
	
	u32 min_valid_value;									//最小有效光照值
	u32 min_valid_value_range;								//最小波动抑制范围
	u32 max_valid_value;									//最大有效光照值
	u32 max_valid_value_range;								//最大波动抑制范围
	u8 valid_value_confirm_time;							//确认次数
	
	u32 value_unchanged_range;								//数据无变化波动值
	u8 no_response_time;									//设备无响应通讯次数

	u16 crc16;												//校验码 存储用

}__attribute__((packed))LumeterConfig_S;

typedef struct	LumeterAlarmConfig							//光照计告警配置
{
	u8 lumeter_thre_over_alarm_enable;						//数据越线使能
	u8 lumeter_abnormal_alarm_enable;						//设备异常使能

	u16 crc16;												//校验码 存储用

}__attribute__((packed))LumeterAlarmConfig_S;

typedef struct	LumeterState								//光照计状态
{
	u8 address;												//通讯地址
	u8 channel;												//通讯通道
	u8 waitting_resp;										//等待设备响应
	u8 no_resp_time;										//无响应次数
	u8 collect_time;										//采集次数
	time_t unchange_time;									//数据不变化时间
	u32 ref_value;											//24小时不变基准值
	
	u32 stack[MAX_LUMETER_STACK_LEN];						//堆栈
	
	u32 value;												//有效值

}LumeterState_S;

typedef struct	LumeterCollectState							//光照采集计状态
{
	u8 address;												//通讯地址
	u8 channel;												//通讯通道
	u8 update;												//更新标志
	
	u32 value;												//有效值

}__attribute__((packed))LumeterCollectState_S;




extern u32 LumeterAppValue;											//参与逻辑运算的光照值
extern u32 LumeterValueFromServer;									//上位机下发的照度值
extern Uint32TypeNumber_S LumeterConfigNum;							//照度计配置数量
extern LumeterConfig_S LumeterConfig[MAX_LUMETER_CONF_NUM];			//照度计配置
extern LumeterBasicConfig_S LumeterBasicConfig;						//照度计基础配置
extern LumeterAlarmConfig_S LumeterAlarmConfig;						//照度计告警参数配置

extern LumeterState_S LumeterState[MAX_LUMETER_CONF_NUM];			//照度计当前状态












void ReadLumeterConfigNum(void);
void WriteLumeterConfigNum(u8 reset,u8 write_enable);
void ReadLumeterBasicConfig(void);
void WriteLumeterBasicConfig(u8 reset,u8 write_enable);
void ReadLumeterAlarmConfig(void);
void WriteLumeterAlarmConfig(u8 reset,u8 write_enable);
void ReadLumeterConfig(void);
void WriteLumeterConfig(u8 i,u8 reset,u8 write_enable);



































#endif
