#ifndef __LAMP_CONF_H
#define __LAMP_CONF_H

#include "sys.h"
#include "common.h"
#include "concentrator_conf.h"

/********************************固件升级***************************************/
//#define LAMP_FIRMWARE_FREE					0
//#define LAMP_FIRMWARE_DOWNLOADING			1
//#define LAMP_FIRMWARE_DOWNLOAD_WAIT			2
//#define LAMP_FIRMWARE_DOWNLOADED			3
//#define LAMP_FIRMWARE_DOWNLOAD_FAILED		4
//#define LAMP_FIRMWARE_UPDATING				5
//#define LAMP_FIRMWARE_UPDATE_SUCCESS		6
//#define LAMP_FIRMWARE_UPDATE_FAILED			7
//#define LAMP_FIRMWARE_ERASE_SUCCESS			8
//#define LAMP_FIRMWARE_ERASE_FAIL			9
//#define LAMP_FIRMWARE_ERASEING				10
#define LAMP_FIRMWARE_BAG_SIZE				258
#define LAMP_FIRMWARE_BUCKUP_FLASH_BASE_ADD	0x08020000	//扇区5
#define LAMP_FIRMWARE_SIZE					131072		//128K
#define LAMP_FIRMWARE_BAG_SEND_SIZE			128


#define MAX_LAMP_AUTO_SYNC_TIME_CYCLE		604800	//单灯自动同步时间周期 最大一周

#define MAX_LAMP_CONF_NUM					512		//最多可配置的单灯数量
#define MAX_LAMP_CH_NUM						2		//最多通道数
#define MAX_LAMP_GROUP_NUM					10		//最多组数
#define MAX_LAMP_ALARM_E_PARA_NUM			3		//可配置告警的电参数种类

#define MAX_LAMP_STRATEGY_NUM				512
#define MAX_LAMP_APPOINTMENT_NUM			10
#define MAX_LAMP_APPOINTMENT_TIME_RANGE_NUM	10		//每个预约控制最多10个时间段



typedef struct	LampListNum							//用于统计单灯配置个数
{
	u16 number;										//个数
	
	u8 list[64];									//对照表 每一个bit表示一个单灯
	
	u16 crc16;										//校验码 存储用
	
}__attribute__((packed))LampListNum_S;

typedef struct	LampGroupListNum					//用于统计每组单灯配置个数
{
	u16 list[MAX_LAMP_GROUP_NUM];					//存放每组单灯数量
	
	u16 crc16;										//校验码 存储用
	
}__attribute__((packed))LampGroupListNum_S;

typedef struct	ElectriccalParaAlarm
{
	u8 channel;									//灯头通道
	u8 para_id;									//参数ID
	u16 min_value;								//最小值
	u8 min_range;								//最小值告警抑制波动范围百分比
	u16 max_value;								//最大值
	u8 max_range;								//最大值告警抑制波动范围百分比
	u16 duration_time;							//持续确认时长
}__attribute__((packed))ElectriccalParaAlarm_S;

typedef struct	LampAlarmConf
{	
	u8 lamp_fault_alarm_enable;						//灯具故障告警使能
	u8 power_module_fault_alarm_enable;				//电源故障告警使能
	u8 capacitor_fault_alarm_enable;				//补偿电容故障告警使能
	u8 relay_fault_alarm_enable;					//继电器故障告警使能
	
	u8 temperature_alarm_enable;					//高低温告警使能
	s8 temperature_alarm_low_thre;					//高低温告警低温阈值
	u8 temperature_alarm_high_thre;					//高低温告警高温阈值
	u16 temperature_alarm_duration;					//高低温告警检测时长
	
	u8 leakage_alarm_enable;						//漏电告警使能
	u16 leakage_alarm_c_thre;						//漏电告警电流阈值
	u8 leakage_alarm_v_thre;						//漏电告警电压阈值
	u16 leakage_alarm_duration;						//漏电告警检测时长
	
	u8 gate_magnetism_alarm_enable;					//门磁告警使能
	u8 gate_magnetism_alarm_type;					//门磁告警检测类型
	
	u8 post_tilt_alarm_enable;						//灯杆倾斜告警使能
	s8 post_tilt_alarm_thre;						//灯杆倾斜告警阈值
	u16 post_tilt_alarm_duration;					//灯杆倾斜告警检测时长
	
	u8 electrical_para_alarm_enable;				//电参数越限告警使能
	ElectriccalParaAlarm_S electrical_para_alarm_thre[MAX_LAMP_CH_NUM][MAX_LAMP_ALARM_E_PARA_NUM];
	
	u8 abnormal_light_on_alarm_enable;				//异常开灯告警使能
	u16 abnormal_light_on_alarm_c_thre;				//异常开灯告警电流阈值
	u16 abnormal_light_on_alarm_p_thre;				//异常开灯告警功率阈值
	u16 abnormal_light_on_alarm_duration;			//异常开灯告警检测时长
	
	u8 abnormal_light_off_alarm_enable;				//异常关灯告警使能
	u16 abnormal_light_off_alarm_c_thre;			//异常关灯告警电流阈值
	u16 abnormal_light_off_alarm_p_thre;			//异常关灯告警功率阈值
	u16 abnormal_light_off_alarm_duration;			//异常关灯告警检测时长
	
	u8 light_on_fault_alarm_enable;							//亮灯异常告警使能
	u8 light_on_fault_alarm_rated_power[MAX_LAMP_CH_NUM];	//灯具1额定功率
	u8 light_on_fault_alarm_low_thre[MAX_LAMP_CH_NUM];		//灯具1最小功率超限百分比
	u8 light_on_fault_alarm_high_thre[MAX_LAMP_CH_NUM];		//灯具1最小功率超限百分比
	u16 light_on_fault_alarm_duration[MAX_LAMP_CH_NUM];		//亮灯异常告警检测时长
	
	u8 task_light_state_fault_alarm_enhable;		//任务亮灯状态异常告警使能
	
	u16 crc16;												//校验码 存储用

}__attribute__((packed))LampAlarmConf_S;

typedef struct	LampBasicConfig
{
	u32 auto_sync_time_cycle;								//自动对时周期 秒
	u8 auto_report_plc_state;								//自动上报PLC状态
	u16 state_collection_cycle;								//灯具状态采集周期 min
	s16 state_collection_offset;							//灯具状态采集偏移量 min
	u8 state_recording_time;								//灯具状态记录时长 day
	u16 response_timeout;									//灯具响应等待时长 sec
	u8 retransmission_times;								//灯具超时重发次数
	u8 broadcast_times;										//灯具广播发送次数
	u16 broadcast_interval_time;							//灯具广播间隔时长 ms

	u16 crc16;												//校验码 存储用

}__attribute__((packed))LampBasicConfig_S;

typedef struct	LampConfig
{
	u32 address;											//设备地址
	s8 advance_time;										//光控提前亮灯分钟数
	s8 delay_time;											//光控延后亮灯分钟数
	float longitude;										//经度
	float latitude;											//纬度
	u8 light_wane;											//光衰
	u8 auto_report;											//单灯自动上报使能
	u8 adjust_type;											//调光方式
	u8 node_loss_check_times;								//节点丢失检测次数
	u8 default_brightness;									//各通初始道亮度 高4字节代表1灯头 低4字节表示灯头2
	u8 group[MAX_LAMP_GROUP_NUM];							//设备所属组号

	u16 crc16;												//校验码 存储用

}__attribute__((packed))LampConfig_S;

typedef struct	LampTask									//单灯任务内容
{
	u8 group_id;											//模式(组)编号
	u8 type;												//任务类型
	u8 executor;											//执行者
	s16 time;												//动作时间
	u8 time_option;											//时间选项
	u8 brightness[MAX_LAMP_CH_NUM];							//灯头亮度
	u8 ctrl_mode;											//配置模式 0配置所有 1配置指定组 2配置指定地址
	u32 group_add[MAX_LAMP_GROUP_NUM];						//组号或地址
	
	u16 crc16;												//校验码 存储用
	
}__attribute__((packed))LampTask_S;

typedef struct	LampSenceConfig									//单灯场景模式配置
{
	u8 group_id;												//模式(组)编号
	u8 priority;												//优先级
	u8 time_range_num;											//有效时间范围数量
	TimeRange_S range[MAX_LAMP_APPOINTMENT_TIME_RANGE_NUM];		//场景时间段
	
	u16 crc16;													//校验码 存储用

}__attribute__((packed))LampSenceConfig_S;

typedef struct	LampStrategyGroupSwitch							//模式切换配置
{
	u8 group_num;												//策略组数量
	u8 group_id[MAX_LAMP_GROUP_NUM];							//策略组号
	u8 type;													//切换方式
	u8 time[15];												//切换时间
	
	u16 crc16;													//校验码 存储用

}__attribute__((packed))LampStrategyGroupSwitch_S;




extern LampBasicConfig_S LampBasicConfig;				//单灯自动对时周期

extern LampGroupListNum_S LampGroupListNum;						//每组单灯数量
extern LampListNum_S LampStrategyNumList;						//已配置的任务数量
extern LampListNum_S LampNumList;								//已配置的单灯数量
extern Uint32TypeNumber_S LampAppointmentNum;					//单灯预约控制数量
extern Uint32TypeNumber_S LampStrategyNum;						//单灯策略配置数量
extern FrameWareState_S LampFrameWareState;						//固件升级状态




void ReadLampBasicConfig(void);
void WriteLampBasicConfig(u8 reset,u8 write_enable);
void ReadLampNumList(void);
void WriteLampNumList(u8 reset,u8 write_enable);
u8 ReadSpecifyLampNumList(u16 i);
void WriteSpecifyLampNumList(u16 i,u8 mode);
void ReadLampStrategyNumList(void);
void WriteLampStrategyNumList(u8 reset,u8 write_enable);
u8 ReadSpecifyLampStrategyNumList(u16 i);
void WriteSpecifyLampStrategyNumList(u16 i,u8 mode);
void ReadLampAppointmentNum(void);
void WriteLampAppointmentNum(u8 reset,u8 write_enable);
u8 ReadLampAppointment(u8 i,LampSenceConfig_S *appointment);
void WriteLampAppointment(u8 i,LampSenceConfig_S *appointment,u8 reset,u8 write_enable);
void WriteSpecifyLampNumList(u16 i,u8 mode);
void ReadLampGroupListNum(void);
void WriteLampGroupListNum(u8 reset,u8 write_enable);
u8 ReadLampConfig(u16 i,LampConfig_S *config);
void WriteLampConfig(u8 i,u8 reset,LampConfig_S config);
u8 ReadLampTaskConfig(u16 i,LampTask_S *task);
void WriteLampTaskConfig(u8 i,u8 reset,LampTask_S task);
u8 ReadLampFrameWareState(void);
void WriteLampFrameWareState(u8 reset,u8 write_enable);












#endif
