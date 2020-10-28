#ifndef __E_METER_H
#define __E_METER_H

#include "sys.h"
#include "electricity_meter_conf.h"


#define PHASE_A							0
#define PHASE_B							1
#define PHASE_C							2
#define PHASE_ABC						3



typedef struct	BulitInMeterRatio
{
	float voltage_ratio;					
	float current_ratio;

	u16 crc16;

}__attribute__((packed))BulitInMeterRatio_S;

typedef struct	BulitInMeterParas		//内置电表参数
{
	float voltage[4];
	float current[4];
	float active_power[4];
	float reactive_power[4];
	float apparent_power[4];
	float power_factor[4];
	double active_energy[4];
	double reactive_energy[4];
	double apparent_energy[4];
	float frequency;
	float line_voltage[4];
	
}__attribute__((packed))BulitInMeterParas_S;


extern BulitInMeterParas_S BulitInMeterParas;
extern BulitInMeterRatio_S BulitInMeterRatio;

void ReadBulitInMeterRatio(void);
void WriteBulitInMeterRatio(u8 reset,u8 write_enable);

void HT7038GetParameters(void);
void ElectricityMeterGetBuiltInParas(ElectricityMeterState_S *state);
void ElectricityMeterGetBuiltOutParas(ElectricityMeterState_S state);
u16 PackBuiltOutElectricityMeterFrame(u8 address,u8 mode,u8 *outbuf);
u8 AnalysisBuiltOutElectricityMeterFrame(u8 *buf,u16 len,ElectricityMeterCollectState_S *meter_state);



































#endif
