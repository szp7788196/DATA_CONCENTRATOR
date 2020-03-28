#ifndef __CONCENTRATOR_H
#define __CONCENTRATOR_H

#include "sys.h"
#include "server_protocol.h"
#include "event_alarm.h"

/********************************固件升级***************************************/
#define FIRMWARE_FREE					0
#define FIRMWARE_DOWNLOADING			1
#define FIRMWARE_DOWNLOAD_WAIT			2
#define FIRMWARE_DOWNLOADED				3
#define FIRMWARE_DOWNLOAD_FAILED		4
#define FIRMWARE_UPDATING				5
#define FIRMWARE_UPDATE_SUCCESS			6
#define FIRMWARE_UPDATE_FAILED			7
#define FIRMWARE_ERASE_SUCCESS			8
#define FIRMWARE_ERASE_FAIL				9
#define FIRMWARE_ERASEING				10
#define FIRMWARE_BAG_SIZE				258
#define FIRMWARE_RUN_FLASH_BASE_ADD		0x08040000
#define FIRMWARE_BUCKUP_FLASH_BASE_ADD	0x080A0000
#define FIRMWARE_MAX_FLASH_ADD			0x08100000
#define FIRMWARE_SIZE					FIRMWARE_BUCKUP_FLASH_BASE_ADD - FIRMWARE_RUN_FLASH_BASE_ADD

/********************************事件类型***************************************/
#define CONCENTRATOR_RESET				1		//复位事件
#define CONCENTRATOR_REBOOT				2		//重启事件

/********************************告警类型***************************************/
#define CONCENTRATOR_POWER_OFF			1		//断电告警
#define CONCENTRATOR_LEAKAGE			2		//漏电告警
#define CONCENTRATOR_LOW_BATTERY		3		//电池电量低告警
#define CONCENTRATOR_ABNORMAL_CHARGE	4		//异常充电告警
#define CONCENTRATOR_ABNORMAL_COMM		5		//异常通讯告警

typedef enum
{
	MODE_AUTO 	= 1,
	MODE_MANUAL = 2,
	MODE_TASK 	= 3,
	MODE_STOP 	= 4,

} RUN_MODE_E;


typedef struct	ConcentratorBasicConfig		//集控器基础配置参数
{
	u8 connection_mode;						//连接方式
	u8 server_ip[31];						//服务器地址
	u8 server_port[6];						//服务器端口
	u16 heartbeat_cycle;					//心跳周期 s
	u16 electric_energy_collection_cycle;	//电能采集周期 min
	u8 electric_energy_recording_time;		//电能记录时长 day
	u16 loop_state_monitoring_cycle;		//回路状态监测周期 min
	u8 loop_state_recording_time;			//回路状态记录时长 day
	u16 cupboard_alarm_collection_cycle;	//柜控告警采集周期 min
	u8 cupboard_alarm_recording_time;		//柜控告警记录时长 day
	u16 lamp_state_collection_cycle;		//灯具状态采集周期 min
	s16 lamp_state_collection_offset;		//灯具状态采集偏移量 min
	u8 lamp_state_recording_time;			//灯具状态记录时长 day
	u16 command_response_timeout;			//指令响应等待时长 sec
	u8 command_retransmission_times;		//指令超时重发次数
	u16 heartbeat_response_timeout;			//指令响应等待时长 sec
	u8 heartbeat_retransmission_times;		//心跳超时重发次数
	u16 lamp_response_timeout;				//灯具响应等待时长 sec
	u8 lamp_retransmission_times;			//灯具超时重发次数
	u8 lamp_broadcast_times;				//灯具广播发送次数
	u16 lamp_broadcast_interval_time;		//灯具广播间隔时长 ms
	u8 operation_password[7];				//设备工程维护密码
	u8 manufacturer_name[33];				//厂家名称
	u8 manufacturer_website[33];			//厂家网址
	u16 crc16;								//校验码 存储用

}__attribute__((packed))ConcentratorBasicConfig_S;


typedef struct	ConcentratorAlarmConfig		//集控器告警配置参数
{
	u8 power_off_alarm_enable;				//断电告警使能
	u16 power_off_alarm_thre;				//断电告警设置值
	u8 electric_leakage_alarm_enable;		//漏电告警使能
	u16 electric_leakage_alarm_v_thre;		//漏电电压阈值
	u16 electric_leakage_alarm_c_thre;		//漏电电流阈值
	u8 low_battery_alarm_enable;			//电池电量低告警使能
	u8 low_battery_alarm_thre;				//电池电量低告警阈值
	u8 abnormal_charge_alarm_enable;		//电池充电异常告警
	u16 abnormal_charge_alarm_v_thre;		//电池充电异常电压阈值
	u16 abnormal_charge_alarm_c_thre;		//电池充电异常电流阈值
	u16 crc16;								//校验码 存储用

}__attribute__((packed))ConcentratorAlarmConfig_S;

typedef struct	SwitchTime					//每日开关灯时间
{
	u8 on_hour;								//开灯时
	u8 on_minute;							//开灯分
	u8 off_hour;							//关灯时
	u8 off_minute;							//关灯分
}__attribute__((packed))SwitchTime_S;

typedef struct	SwitchTimeMonthTable		//每月开关灯时间
{
	SwitchTime_S switch_time[31];
}__attribute__((packed))SwitchTimeMonthTable_S;

typedef struct	ConcentratorLocationConfig	//集控器经纬度年表配置
{
	double longitude;						//经度
	double latitude;						//纬度
	
	SwitchTimeMonthTable_S switch_time_month_table[12];
	
	u16 crc16;								//校验码 存储用

}__attribute__((packed))ConcentratorLocationConfig_S;

typedef struct FrameWareState
{
	u8 state;								//更新状态
	u16 total_bags;							//总包数
	u16 current_bag_cnt;					//当前包数
	u16 bag_size;							//包大小
	u16 last_bag_size;						//末包大小
	u32 total_size;							//固件总大小
	u8 file_name[31];						//文件名称
	u8 md5[33];								//文件唯一编码
	u16 crc16;								//校验码
	
}__attribute__((packed))FrameWareState_S;




extern RUN_MODE_E RunMode;										//运行模式
extern ConcentratorBasicConfig_S ConcentratorBasicConfig;		//基本配置信息
extern ConcentratorAlarmConfig_S ConcentratorAlarmConfig;		//告警配置参数
extern ConcentratorLocationConfig_S ConcentratorLocationConfig;	//经纬度年表配置
extern FrameWareState_S FrameWareState;							//固件升级状态



extern u8 FlagSystemReBoot;										//系统重启标识
extern u8 FlagReConnectToServer;								//断网重连标志
extern u8 LoginResponse;										//服务器对登录包的相应
extern u8 HeartBeatResponse;									//服务器对心跳包的响应




void ReadRunMode(void);
void WriteRunMode(u8 reset,u8 write_enable);
void ReadConcentratorBasicConfig(void);
void WriteConcentratorBasicConfig(u8 reset,u8 write_enable);
void ReadConcentratorAlarmConfig(void);
void WriteConcentratorAlarmConfig(u8 reset,u8 write_enable);
void ReadConcentratorLocationConfig(void);
void WriteConcentratorLocationConfig(u8 reset,u8 write_enable);
u8 ReadFrameWareState(void);
void WriteFrameWareState(u8 reset,u8 write_enable);

void SendLoginFrameToServer(void);
void SendHeartBeatFrameToServer(void);
void SendOtaRequestFrameToServer(FrameWareState_S frame_ware_state);
void SendOtaCompleteFrameToServer(void);
void SendAlarmReportFrameToServer(AlarmReport_S *alarm_report);
void AutoSendFrameToServer(void);
void RecvAndHandleFrameStruct(void);


u8 TransparentTransmission(ServerFrameStruct_S *server_frame_struct);
u8 SynchronizeTime(ServerFrameStruct_S *server_frame_struct);
u8 ResetConfigParameters(ServerFrameStruct_S *server_frame_struct);
u8 RebootTheSystem(ServerFrameStruct_S *server_frame_struct);
u8 ReConnectToServer(ServerFrameStruct_S *server_frame_struct);
u8 SetRunMode(ServerFrameStruct_S *server_frame_struct);
u8 QueryState(ServerFrameStruct_S *server_frame_struct);
u8 SetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 GetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 SetBasicConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 GetBasicConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 SetLocationConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 GetLocationConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 RequestFrameWareUpDate(ServerFrameStruct_S *server_frame_struct);
u8 RecvFrameWareBag(ServerFrameStruct_S *server_frame_struct);
u8 FrameWareUpDateStop(ServerFrameStruct_S *server_frame_struct);

































#endif
