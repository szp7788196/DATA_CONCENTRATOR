#ifndef __CONCENTRATOR_CONF_H
#define __CONCENTRATOR_CONF_H

#include "sys.h"
#include "common.h"

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
#define FIRMWARE_DOWNLOADED_SUCCESS		11
#define FIRMWARE_BAG_SIZE				258
#define FIRMWARE_RUN_FLASH_BASE_ADD		0x08040000
#define FIRMWARE_BUCKUP_FLASH_BASE_ADD	0x080A0000
#define FIRMWARE_MAX_FLASH_ADD			0x08100000
#define FIRMWARE_SIZE					FIRMWARE_BUCKUP_FLASH_BASE_ADD - FIRMWARE_RUN_FLASH_BASE_ADD

/********************************事件类型***************************************/
#define CONCENTRATOR_RESET				1		//复位事件
#define CONCENTRATOR_REBOOT				2		//重启事件


typedef enum
{
	MODE_AUTO 	= 1,
	MODE_MANUAL = 2,
	MODE_TASK 	= 3,
	MODE_STOP 	= 4,

} RUN_MODE_E;


typedef struct	ConcentratorLocalNetConfig	//集控器本地链接参数
{
	u8 connection_mode;						//连接方式
	u8 dhcp_enable;							//DHCP使能
	u8 local_ip[4];							//本地IP
	u8 local_msak[4];						//本地MASK
	u8 local_gate[4];						//本地网关
	u8 local_dns[4];						//本地DNS
	u8 remote_ip[4];						//远端IP
	u16 remote_port;						//远端端口

	u16 crc16;								//校验码 存储用

}__attribute__((packed))ConcentratorLocalNetConfig_S;

typedef struct	ConcentratorBasicConfig		//集控器基础配置参数
{
	u8 connection_mode;						//连接方式
	u8 server_ip[31];						//服务器地址
	u8 server_port[6];						//服务器端口
	u16 heartbeat_cycle;					//心跳周期 s
	u16 electric_energy_collection_cycle;	//电能采集周期 min
	u8 electric_energy_recording_time;		//电能记录时长 day
	u16 cupboard_alarm_collection_cycle;	//柜控告警采集周期 min
	u8 cupboard_alarm_recording_time;		//柜控告警记录时长 day
	u16 command_response_timeout;			//指令响应等待时长 sec
	u8 command_retransmission_times;		//指令超时重发次数
	u16 heartbeat_response_timeout;			//指令响应等待时长 sec
	u8 heartbeat_retransmission_times;		//心跳超时重发次数
	u8 operation_password[7];				//设备工程维护密码
	u8 manufacturer_name[33];				//厂家名称
	u8 manufacturer_website[33];			//厂家网址
	u16 crc16;								//校验码 存储用

}__attribute__((packed))ConcentratorBasicConfig_S;


typedef struct	ConcentratorAlarmConfig		//集控器告警配置参数
{
	u8 power_off_alarm_enable;				//断电告警使能
	u16 power_off_alarm_thre;				//断电告警电压阈值
	u16 power_off_alarm_rm_percent;			//告警解除百分比
	u8 electric_leakage_alarm_enable;		//漏电告警使能
	u16 electric_leakage_alarm_v_thre;		//漏电电压阈值
	u8 electric_leakage_alarm_v_rm_percent;	//告警解除百分比
	u16 electric_leakage_alarm_c_thre;		//漏电电流阈值
	u8 electric_leakage_alarm_c_rm_percent;	//告警解除百分比
	u8 low_battery_alarm_enable;			//电池电量低告警使能
	u8 low_battery_alarm_thre;				//电池电量低告警阈值
	u8 low_battery_alarm_rm_percent;		//告警解除百分比
	u8 abnormal_charge_alarm_enable;		//电池充电异常告警
	u16 abnormal_charge_alarm_v_thre;		//电池充电异常电压阈值
	u8 abnormal_charge_alarm_v_rm_percent;	//告警解除百分比
	u16 abnormal_charge_alarm_c_thre;		//电池充电异常电流阈值
	u8 abnormal_charge_alarm_c_rm_percent;	//告警解除百分比
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





extern Uint32TypeNumber_S ConcentratorGateWayID;					//网关ID
extern RUN_MODE_E RunMode;										//运行模式
extern ConcentratorLocalNetConfig_S ConcentratorLocalNetConfig;	//本地网络参数
extern ConcentratorBasicConfig_S ConcentratorBasicConfig;		//基本配置信息
extern ConcentratorAlarmConfig_S ConcentratorAlarmConfig;		//告警配置参数
extern ConcentratorLocationConfig_S ConcentratorLocationConfig;	//经纬度年表配置
extern FrameWareState_S FrameWareState;							//固件升级状态






void ReadConcentratorGateWayID(void);
void WriteConcentratorGateWayID(u8 reset,u8 write_enable);
void ReadRunMode(void);
void WriteRunMode(u8 reset,u8 write_enable);
void ReadConcentratorBasicConfig(void);
void ReadConcentratorLocalNetConfig(void);
void WriteConcentratorLocalNetConfig(u8 reset,u8 write_enable);
void WriteConcentratorBasicConfig(u8 reset,u8 write_enable);
void ReadConcentratorAlarmConfig(void);
void WriteConcentratorAlarmConfig(u8 reset,u8 write_enable);
void ReadConcentratorLocationConfig(void);
void WriteConcentratorLocationConfig(u8 reset,u8 write_enable);
u8 ReadFrameWareState(void);
void WriteFrameWareState(u8 reset,u8 write_enable);




































#endif
