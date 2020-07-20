#ifndef __LAMP_EVENT_H
#define __LAMP_EVENT_H

#include "sys.h"
#include "lamp_conf.h"
#include "concentrator_conf.h"

typedef struct	LampParas
{
	u8 brightness;
	u16 light_up_day;
	u16 light_up_total;
	float active_energy_day;
	float active_energy_total;
	float reactive_energy_day;
	float reactive_energy_total;
	float voltage;
	float current;
	float active_power;
	float power_factor;
	float frequency;

}__attribute__((packed))LampParas_S;


typedef struct	LampState
{
	u16 address;								//设备地址
	u8 lamp_num;								//灯头数
	
	LampParas_S lamp_paras[MAX_LAMP_CH_NUM];	//灯头参数
	
	RUN_MODE_E run_mode;						//手自控模式
	u8 controller;								//控制来源
	u8 control_time[7];							//控制时间
	s8 temperature;								//温度
	u16 tilt_angle;								//倾斜角度
	u8 dev_time[15];							//设备当前时间
	
}__attribute__((packed))LampState_S;

















void LampEventCheckPolling(void);






















#endif
