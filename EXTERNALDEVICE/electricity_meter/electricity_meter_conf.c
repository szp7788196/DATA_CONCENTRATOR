#include "electricity_meter_conf.h"
#include "cat25x.h"


ElectricityMeterBasicConfig_S ElectricityMeterBasicConfig;
Uint32TypeNumber_S ElectricityMeterConfigNum;
ElectricityMeterConfig_S ElectricityMeterConfig[MAX_ELECTRICITY_METER_CONF_NUM];
ElectricityMeterAlarmConfig_S ElectricityMeterAlarmConfig;

ElectricityMeterState_S ElectricityMeterState[MAX_ELECTRICITY_METER_CONF_NUM];



//电表基本配置
void ReadElectricityMeterBasicConfig(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&ElectricityMeterBasicConfig,
	            ELECTRICITY_METER_BASIC_CONF_ADD,
	            sizeof(ElectricityMeterBasicConfig_S));

	crc16_cal = CRC16((u8 *)&ElectricityMeterBasicConfig,ELECTRICITY_METER_BASIC_CONF_LEN - 2);

	if(crc16_cal != ElectricityMeterBasicConfig.crc16)
	{
		WriteElectricityMeterBasicConfig(1,0);
	}
}

void WriteElectricityMeterBasicConfig(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		ElectricityMeterBasicConfig.detect_interval = 5000;
		ElectricityMeterBasicConfig.auto_report = 0;

		ElectricityMeterBasicConfig.crc16 = 0;
	}

	if(write_enable == 1)
	{
		ElectricityMeterBasicConfig.crc16 = CRC16((u8 *)&ElectricityMeterBasicConfig,ELECTRICITY_METER_BASIC_CONF_LEN - 2);

		CAT25X_Write((u8 *)&ElectricityMeterBasicConfig,
		             ELECTRICITY_METER_BASIC_CONF_ADD,
		             ELECTRICITY_METER_BASIC_CONF_LEN);
	}
}

//读取电表配置数量
void ReadElectricityMeterConfigNum(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&ElectricityMeterConfigNum,
	            ELECTRICITY_METER_CONF_NUM_ADD,
	            sizeof(Uint32TypeNumber_S));

	crc16_cal = CRC16((u8 *)&ElectricityMeterConfigNum,ELECTRICITY_METER_CONF_NUM_LEN - 2);

	if(crc16_cal != ElectricityMeterConfigNum.crc16 ||
	   ElectricityMeterConfigNum.number > MAX_ELECTRICITY_METER_CONF_NUM)
	{
		WriteElectricityMeterConfigNum(1,0);
	}
}

void WriteElectricityMeterConfigNum(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		ElectricityMeterConfigNum.number = 0;

		ElectricityMeterConfigNum.crc16 = 0;
	}

	if(write_enable == 1)
	{
		ElectricityMeterConfigNum.crc16 = CRC16((u8 *)&ElectricityMeterConfigNum,ELECTRICITY_METER_CONF_NUM_LEN - 2);

		CAT25X_Write((u8 *)&ElectricityMeterConfigNum,
		             ELECTRICITY_METER_CONF_NUM_ADD,
		             ELECTRICITY_METER_CONF_NUM_LEN);
	}
}

//读取电表配置
void ReadElectricityMeterConfig(void)
{
	u16 crc16_cal = 0;
	u8 i = 0;

	ReadElectricityMeterConfigNum();

	for(i = 0; i < ElectricityMeterConfigNum.number; i ++)
	{
		CAT25X_Read((u8 *)&ElectricityMeterConfig[i],
		            ELECTRICITY_METER_CONF_ADD + i * ELECTRICITY_METER_CONF_LEN,
		            sizeof(ElectricityMeterConfig_S));

		crc16_cal = CRC16((u8 *)&ElectricityMeterConfig[i],ELECTRICITY_METER_CONF_LEN - 2);

		if(crc16_cal != ElectricityMeterConfig[i].crc16)
		{
			WriteElectricityMeterConfig(i,1,0);
		}
		else
		{
			memset(&ElectricityMeterState[i],0,sizeof(ElectricityMeterState_S));
			
			ElectricityMeterState[i].address = ElectricityMeterConfig[i].address;
			ElectricityMeterState[i].channel = ElectricityMeterConfig[i].channel;
			ElectricityMeterState[i].ch_num = ElectricityMeterConfig[i].ch_num;
		}
	}
}

void WriteElectricityMeterConfig(u8 i,u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		memset(&ElectricityMeterConfig[i],0,sizeof(ElectricityMeterState_S));
		
		ElectricityMeterConfig[i].address = 0xFF;
		ElectricityMeterConfig[i].channel = 0xFF;
		ElectricityMeterConfig[i].voltage_ratio = 1.0f;
		ElectricityMeterConfig[i].current_ratio = 1.0f;
	}

	if(write_enable == 1)
	{
		ElectricityMeterConfig[i].crc16 = CRC16((u8 *)&ElectricityMeterConfig[i],ELECTRICITY_METER_CONF_LEN - 2);

		CAT25X_Write((u8 *)&ElectricityMeterConfig[i],
		             ELECTRICITY_METER_CONF_ADD + i * ELECTRICITY_METER_CONF_LEN,
		             ELECTRICITY_METER_CONF_LEN);
	}
	
	memset(&ElectricityMeterState[i],0,sizeof(ElectricityMeterState_S));
			
	ElectricityMeterState[i].address = ElectricityMeterConfig[i].address;
	ElectricityMeterState[i].channel = ElectricityMeterConfig[i].channel;
	ElectricityMeterState[i].ch_num = ElectricityMeterConfig[i].ch_num;
}

//读取告警参数配置
void ReadElectricityMeterAlarmConfig(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&ElectricityMeterAlarmConfig,
	            ELECTRICITY_METER_ALARM_CONF_ADD,
	            sizeof(ElectricityMeterAlarmConfig_S));

	crc16_cal = CRC16((u8 *)&ElectricityMeterAlarmConfig,ELECTRICITY_METER_ALARM_CONF_LEN - 2);

	if(crc16_cal != ElectricityMeterAlarmConfig.crc16)
	{
		WriteElectricityMeterAlarmConfig(1,0);
	}
}

void WriteElectricityMeterAlarmConfig(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		ElectricityMeterAlarmConfig.electrical_parameters_thre_over_alarm_enable = 0;

		ElectricityMeterAlarmConfig.crc16 = 0;
	}

	if(write_enable == 1)
	{
		ElectricityMeterAlarmConfig.crc16 = CRC16((u8 *)&ElectricityMeterAlarmConfig,ELECTRICITY_METER_ALARM_CONF_LEN - 2);

		CAT25X_Write((u8 *)&ElectricityMeterAlarmConfig,
		             ELECTRICITY_METER_ALARM_CONF_ADD,
		             ELECTRICITY_METER_ALARM_CONF_LEN);
	}
}








































