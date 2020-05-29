#ifndef __HT7038_H
#define __HT7038_H
#include "sys.h"


#define HT7038_CS                    	PAout(15)


#define PHASE_A							0
#define PHASE_B							1
#define PHASE_C							2
#define PHASE_ABC						3



/*****************计量芯片寄存器******************/
#define HT7038_REG_ACTIVE_POWER_A		0x01		//有功功率
#define HT7038_REG_ACTIVE_POWER_B		0x02
#define HT7038_REG_ACTIVE_POWER_C		0x03
#define HT7038_REG_ACTIVE_POWER_ABC		0x04

#define HT7038_REG_REACTIVE_POWER_A		0x05		//无功功率
#define HT7038_REG_REACTIVE_POWER_B		0x06
#define HT7038_REG_REACTIVE_POWER_C		0x07
#define HT7038_REG_REACTIVE_POWER_ABC	0x08

#define HT7038_REG_APPARENT_POWER_A		0x09		//视在功率
#define HT7038_REG_APPARENT_POWER_B		0x0A
#define HT7038_REG_APPARENT_POWER_C		0x0B
#define HT7038_REG_APPARENT_POWER_ABC	0x0C

#define HT7038_REG_RMS_VOL_A			0x0D		//电压有效值
#define HT7038_REG_RMS_VOL_B			0x0E
#define HT7038_REG_RMS_VOL_C			0x0F

#define HT7038_REG_RMS_CURRENT_A		0x10		//电流有效值
#define HT7038_REG_RMS_CURRENT_B		0x11
#define HT7038_REG_RMS_CURRENT_C		0x12
#define HT7038_REG_RMS_CURRENT_ABC		0x13

#define HT7038_REG_PFACTOR_A			0x14		//功率因数
#define HT7038_REG_PFACTOR_B			0x15
#define HT7038_REG_PFACTOR_C			0x16
#define HT7038_REG_PFACTOR_ABC			0x17

#define HT7038_REG_ACTIVE_ENERGY_A		0x1E		//有功电能
#define HT7038_REG_ACTIVE_ENERGY_B		0x1F
#define HT7038_REG_ACTIVE_ENERGY_C		0x20
#define HT7038_REG_ACTIVE_ENERGY_ABC	0x21

#define HT7038_REG_REACTIVE_ENERGY_A	0x22		//无功电能
#define HT7038_REG_REACTIVE_ENERGY_B	0x23
#define HT7038_REG_REACTIVE_ENERGY_C	0x24
#define HT7038_REG_REACTIVE_ENERGY_ABC	0x25

#define HT7038_REG_APPARENT_ENERGY_A	0x35		//视在电能
#define HT7038_REG_APPARENT_ENERGY_B	0x36
#define HT7038_REG_APPARENT_ENERGY_C	0x37
#define HT7038_REG_APPARENT_ENERGY_ABC	0x38

#define HT7038_REG_FREQUENCY			0x1C		//频率



#define HT7038_REG_MODE_CONFIG			0x01
#define HT7038_REG_ADC_CONFIG			0x02
#define HT7038_REG_EMU_CONFIG			0x03
#define HT7038_REG_PGAIN_A				0x04
#define HT7038_REG_PGAIN_B				0x05
#define HT7038_REG_PGAIN_C				0x06
#define HT7038_REG_QGAIN_A				0x07
#define HT7038_REG_QGAIN_B				0x08
#define HT7038_REG_QGAIN_C				0x09
#define HT7038_REG_SGAIN_A				0x0A
#define HT7038_REG_SGAIN_B				0x0B
#define HT7038_REG_SGAIN_C				0x0C
#define HT7038_REG_UGAIN_A				0x17
#define HT7038_REG_UGAIN_B				0x18
#define HT7038_REG_UGAIN_C				0x19
#define HT7038_REG_IGAIN_A				0x1A
#define HT7038_REG_IGAIN_B				0x1B
#define HT7038_REG_IGAIN_C				0x1C
#define HT7038_REG_HF_CONST				0x1E
#define HT7038_REG_INT_EN				0x30
#define HT7038_REG_ANMODULE_EN			0x31
#define HT7038_REG_ALGOFG				0x70


#define HF_CONST						0x0C


typedef struct
{
	float voltage[6];						//六路检测模块 最多六路
	float current[6];
	float active_power[6];
	float reactive_power[6];
	float apparent_power[6];
	float power_factor[6];
	double active_energy[6];
	double reactive_energy[6];	
	double apparent_energy[6];
	float frequency;
}__attribute__((packed))ELECTRICITY_METER_INFO;


extern float HT7038_K;



extern ELECTRICITY_METER_INFO BuiltInMeter;




void HT7038_Init(void);
u32 HT7038ReadDeviceID(void);
void HT7038ConfigClibration(void);
u32 HT7038GetSingleParameter(u8 ReadAddr);
//void HT7038GetParameters(void);




















#endif

