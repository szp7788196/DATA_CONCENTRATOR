#ifndef __CONCENTRATOR_H
#define __CONCENTRATOR_H

#include "sys.h"
#include "server_protocol.h"




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






extern RUN_MODE_E RunMode;
extern ConcentratorBasicConfig_S ConcentratorBasicConfig;




extern u8 FlagSystemReBoot;										//系统重启标识
extern u8 FlagReConnectToServer;								//断网重连标志











void ReadRunMode(void);
void WriteRunMode(u8 reset,u8 write_enable);
void ReadConcentratorBasicConfig(void);
void WriteConcentratorBasicConfig(u8 reset,u8 write_enable);
void ReadConcentratorAlarmConfig(void);
void WriteConcentratorAlarmConfig(u8 reset,u8 write_enable);


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

































#endif
