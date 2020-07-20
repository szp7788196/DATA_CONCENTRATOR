#ifndef __ELECTRICITY_METER_CONF_H
#define __ELECTRICITY_METER_CONF_H

#include "sys.h"
#include "common.h"


#define MAX_ELECTRICITY_METER_CONF_NUM					6	//最多配置6个电表
#define MAX_ELECTRICITY_METER_MODULE_NAME_LEN			16	//模块型号长度
#define MAX_ELECTRICITY_METER_CH_NUM					7	//通道数
#define MAX_ELECTRICITY_METER_ALARM_TYPE_NUM			1	//告警类型数
#define MAX_ELECTRICITY_METER_PER_CH_PARA_NUM			8	//每隔通道参数数量
#define MAX_ELECTRICITY_METER_ALARM_PARA_NUM			8	//可配置告警的参数数量


typedef struct	ElectricityMeter_Para										//数字通道告警配置
{
	float voltage;										//相电压					
	float current;										//相电流
	float active_power;									//有功功率
	double active_energy;								//有功电能
	double reactive_energy;								//无功电能
	float power_factor;									//功率因数
	float frequency;									//频率
	float line_voltage;									//相电压

}__attribute__((packed))ElectricityMeter_Para_S;

typedef struct	ElectricityMeter_Alarm		//数字通道告警配置
{
	u8 channel;								//检测端子
	u8 para_id;								//告警电平
	float min_value;						//最小值
	u8 min_range;							//最小值抑制波动范围百分比
	float max_value;						//最大值
	u8 max_range;							//最大值抑制波动范围百分比
	u8 confirm_time;						//确认次数
	u8 last_run_mode;						//上个运行状态
	u8 switch_run_mode;						//手自控切换
	u8 resume_run_mode;						//手自控恢复
	u8 relay_action;						//回路动作

}__attribute__((packed))ElectricityMeter_Alarm_S;

typedef struct	ElectricityMeterBasicConfig			//电表基本配置
{
	u16 detect_interval;							//检测间隔
	u8 auto_report;									//自动上报

	u16 crc16;										//校验码 存储用

}__attribute__((packed))ElectricityMeterBasicConfig_S;

typedef struct	ElectricityMeterConfig					//电表配置
{
	u8 address;											//通讯地址
	u8 channel;											//通讯通道
	u8 ch_num;											//测量通道数
	float voltage_ratio;								//电压倍率
	float current_ratio;								//电流倍率
	float voltage_range_of_change;						//电压变化范围
	float current_range_of_change;						//电流变化范围
	float pf_range_of_change;							//功率因数变化范围
	u8 confirm_time;									//确认次数
	u8 module[MAX_ELECTRICITY_METER_MODULE_NAME_LEN];	//模块型号
	ElectricityMeter_Alarm_S alarm_thre[MAX_ELECTRICITY_METER_CH_NUM][MAX_ELECTRICITY_METER_ALARM_PARA_NUM];//数字量告警阈值

	u16 crc16;											//校验码 存储用

}__attribute__((packed))ElectricityMeterConfig_S;

typedef struct	ElectricityMeterAlarmConfig				//电表告警参数配置
{
	u8 electrical_parameters_thre_over_alarm_enable;	//电参数越线告警

	u16 crc16;											//校验码 存储用

}__attribute__((packed))ElectricityMeterAlarmConfig_S;

typedef struct	ElectricityMeterState					//电表实时采集数据
{
	u8 address;											//通讯地址
	u8 channel;											//通讯通道
	
	u8 ch_num;											//测量通道数
	
	u8 abnormal_ch;										//存在异常的回路
	
	ElectricityMeter_Para_S mirror_para[MAX_ELECTRICITY_METER_CH_NUM];
	ElectricityMeter_Para_S current_para[MAX_ELECTRICITY_METER_CH_NUM];

}__attribute__((packed))ElectricityMeterState_S;

typedef struct	ElectricityMeterCollectState			//电表实时采集数据
{
	u8 address;											//通讯地址
	u8 channel;											//通讯通道
	
	u8 update;

	ElectricityMeter_Para_S collect_para[MAX_ELECTRICITY_METER_CH_NUM];

}__attribute__((packed))ElectricityMeterCollectState_S;




extern ElectricityMeterBasicConfig_S ElectricityMeterBasicConfig;
extern Uint32TypeNumber_S ElectricityMeterConfigNum;
extern ElectricityMeterConfig_S ElectricityMeterConfig[MAX_ELECTRICITY_METER_CONF_NUM];
extern ElectricityMeterAlarmConfig_S ElectricityMeterAlarmConfig;

extern ElectricityMeterState_S ElectricityMeterState[MAX_ELECTRICITY_METER_CONF_NUM];








void ReadElectricityMeterBasicConfig(void);
void WriteElectricityMeterBasicConfig(u8 reset,u8 write_enable);
void ReadElectricityMeterConfigNum(void);
void WriteElectricityMeterConfigNum(u8 reset,u8 write_enable);
void ReadElectricityMeterConfig(void);
void WriteElectricityMeterConfig(u8 i,u8 reset,u8 write_enable);
void ReadElectricityMeterAlarmConfig(void);
void WriteElectricityMeterAlarmConfig(u8 reset,u8 write_enable);































#endif
