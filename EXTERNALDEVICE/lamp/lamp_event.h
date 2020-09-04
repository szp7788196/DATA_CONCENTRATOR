#ifndef __LAMP_EVENT_H
#define __LAMP_EVENT_H

#include "sys.h"
#include "lamp_conf.h"
#include "concentrator_conf.h"
#include "task_plc.h"
#include "history_record.h"


#define LAMP_NODE_LOSS_ALARM					1		//节点丢失告警
#define LAMP_LAMP_FAULT_ALARM					2		//灯具故障告警
#define LAMP_POWER_MODULE_FAULT_ALARM			3		//电源模块故障告警
#define LAMP_CAPACITOR_FAULT_ALARM				4		//补偿电容故障告警
#define LMAP_RELAY_FAULT_ALARM					5		//继电器故障告警
#define LAMP_OVRE_LOW_TEMPERATURE_ALARM			6		//高低温告警
#define LAMP_LEAKAGE_ALARM						7		//漏电告警
#define LAMP_GATE_MAGNETISM_ALARM				8		//门磁告警
#define LAMP_POLE_TILT_ALARM					9		//灯杆倾斜告警
#define LAMP_ELECTRICAL_PARA_OVER_THRE_ALARM	10		//电参数越限告警
#define LAMP_ABNORMAL_LIGHT_ON_ALARM			11		//异常开灯告警
#define LAMP_ABNORMAL_LIGHT_OFF_ALARM			12		//异常关灯告警
#define LAMP_LIGHT_ON_ABNORMAL_ALARM			13		//开灯异常告警
#define LAMP_TASK_STATE_ABNORMAL_ALARM			14		//任务内亮灯状态异常告警

typedef struct	LampParas
{
	u8 brightness;
	u16 light_up_day;
	u32 light_up_total;
	u32 active_energy_day;				//单位：Wh
	u32 active_energy_total;
	u32 reactive_energy_day;
	u32 reactive_energy_total;
	u16 voltage;						//单位：V
	u16 current;						//单位：mA
	u16 active_power;					//单位：W
	u16 power_factor;					//单位：0.01
	u16 frequency;						//单位：0.01Hz

}__attribute__((packed))LampParas_S;


typedef struct	LampState
{
	u8 report_type;								//上报类型 0被动 1主动
	u32 address;								//设备地址
	u8 channel;									//灯头通道
	
	LampParas_S lamp_paras[MAX_LAMP_CH_NUM];	//灯头参数
	
	RUN_MODE_E run_mode;						//手自控模式
	u8 controller;								//控制来源
	u8 control_time[3];							//控制时间
	s8 temperature;								//温度
	u16 tilt_angle;								//倾斜角度
	u8 dev_time[6];								//设备当前时间
	
}__attribute__((packed))LampState_S;

















void LampEventCheckPolling(void);


void LampLamp_Power_CapacitorFaultAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampRelayFaultAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampOverLowTemperatureAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampLeakageAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampGateMagnetismAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampPoleTiltAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampElectricalParaOverThreAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampAbnormalLightOnOffAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampLightOnAbnormalAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampTaskStateAbnormalAlarm(PlcFrame_S *user_frame,u8 record_type);











































#endif
