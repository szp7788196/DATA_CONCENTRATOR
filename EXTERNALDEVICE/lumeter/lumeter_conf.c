#include "lumeter_conf.h"
#include "common.h"
#include "cat25x.h"


u32 LumeterAppValue = 0;									//参与逻辑运算的光照值
u32 LumeterValueFromServer = 0;								//上位机下发的照度值
Uint32TypeNumber_S LumeterConfigNum;						//照度计配置数量
LumeterConfig_S LumeterConfig[MAX_LUMETER_CONF_NUM];		//照度计配置
LumeterBasicConfig_S LumeterBasicConfig;					//照度计基础配置
LumeterAlarmConfig_S LumeterAlarmConfig;					//照度计告警参数配置

LumeterState_S LumeterState[MAX_LUMETER_CONF_NUM];			//照度计当前状态





void ReadLumeterConfigNum(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&LumeterConfigNum,
	            LUMETER_CONF_NUM_ADD,
	            sizeof(Uint32TypeNumber_S));

	crc16_cal = CRC16((u8 *)&LumeterConfigNum,LUMETER_CONF_NUM_LEN - 2);

	if(crc16_cal != LumeterConfigNum.crc16 ||
	   LumeterConfigNum.number > MAX_LUMETER_CONF_NUM)
	{
		WriteLumeterConfigNum(1,0);
	}
}

void WriteLumeterConfigNum(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		LumeterConfigNum.number = 0;

		LumeterConfigNum.crc16 = 0;
	}

	if(write_enable == 1)
	{
		LumeterConfigNum.crc16 = CRC16((u8 *)&LumeterConfigNum,LUMETER_CONF_NUM_LEN - 2);

		CAT25X_Write((u8 *)&LumeterConfigNum,
		             LUMETER_CONF_NUM_ADD,
		             LUMETER_CONF_NUM_LEN);
	}
}

void ReadLumeterBasicConfig(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&LumeterBasicConfig,
	            LUMETER_BASIC_CONF_ADD,
	            sizeof(LumeterBasicConfig_S));

	crc16_cal = CRC16((u8 *)&LumeterBasicConfig,LUMETER_BASIC_CONF_LEN - 2);

	if(crc16_cal != LumeterBasicConfig.crc16)
	{
		WriteLumeterBasicConfig(1,0);
	}
}

void WriteLumeterBasicConfig(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		LumeterBasicConfig.collect_cycle = 5000;
		LumeterBasicConfig.stack_depth = 20;
		LumeterBasicConfig.ignore_num = 5;
		LumeterBasicConfig.auto_report_cycle = 60;
		LumeterBasicConfig.data_source = 0;
		LumeterBasicConfig.light_on_thre = 40;
		LumeterBasicConfig.light_on_advance_time = 30;
		LumeterBasicConfig.light_on_delay_time = 30;
		LumeterBasicConfig.light_off_thre = 40;
		LumeterBasicConfig.light_off_advance_time = 30;
		LumeterBasicConfig.light_off_delay_time = 30;

		LumeterBasicConfig.crc16 = 0;
	}

	if(write_enable == 1)
	{
		LumeterBasicConfig.crc16 = CRC16((u8 *)&LumeterBasicConfig,LUMETER_BASIC_CONF_LEN - 2);

		CAT25X_Write((u8 *)&LumeterBasicConfig,
		             LUMETER_BASIC_CONF_ADD,
		             LUMETER_BASIC_CONF_LEN);
	}
}

void ReadLumeterAlarmConfig(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&LumeterAlarmConfig,
	            LUMETER_ALARM_CONF_ADD,
	            sizeof(LumeterAlarmConfig_S));

	crc16_cal = CRC16((u8 *)&LumeterAlarmConfig,LUMETER_ALARM_CONF_LEN - 2);

	if(crc16_cal != LumeterAlarmConfig.crc16)
	{
		WriteLumeterAlarmConfig(1,0);
	}
}

void WriteLumeterAlarmConfig(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		LumeterAlarmConfig.lumeter_thre_over_alarm_enable = 0;
		LumeterAlarmConfig.lumeter_abnormal_alarm_enable = 0;

		LumeterAlarmConfig.crc16 = 0;
	}

	if(write_enable == 1)
	{
		LumeterAlarmConfig.crc16 = CRC16((u8 *)&LumeterAlarmConfig,LUMETER_ALARM_CONF_LEN - 2);

		CAT25X_Write((u8 *)&LumeterAlarmConfig,
		             LUMETER_ALARM_CONF_ADD,
		             LUMETER_ALARM_CONF_LEN);
	}
}


void ReadLumeterConfig(void)
{
	u16 crc16_cal = 0;
	u8 i = 0;

	ReadLumeterConfigNum();

	for(i = 0; i < LumeterConfigNum.number; i ++)
	{
		CAT25X_Read((u8 *)&LumeterConfig[i],
		            LUMETER_CONF_ADD + i * LUMETER_CONF_LEN,
		            sizeof(LumeterConfig_S));

		crc16_cal = CRC16((u8 *)&LumeterConfig[i],LUMETER_CONF_LEN - 2);

		if(crc16_cal != LumeterConfig[i].crc16)
		{
			WriteLumeterConfig(i,1,0);
		}
		else
		{
			memset(&LumeterState[i],0,sizeof(LumeterState_S));
			
			LumeterState[i].address = LumeterConfig[i].address;
			LumeterState[i].channel = LumeterConfig[i].channel;
		}
	}
}

void WriteLumeterConfig(u8 i,u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		LumeterConfig[i].address = 0xFF;
		LumeterConfig[i].channel = 0xFF;
		
		LumeterConfig[i].min_valid_value = 0;
		LumeterConfig[i].min_valid_value_range = 0;
		LumeterConfig[i].max_valid_value = 0;
		LumeterConfig[i].max_valid_value_range = 0;
		LumeterConfig[i].valid_value_confirm_time = 10;

		LumeterConfig[i].value_unchanged_range = 1000;
		LumeterConfig[i].no_response_time = 10;

		LumeterConfig[i].crc16 = 0;
	}

	if(write_enable == 1)
	{
		LumeterConfig[i].crc16 = CRC16((u8 *)&LumeterConfig[i],LUMETER_CONF_LEN - 2);

		CAT25X_Write((u8 *)&LumeterConfig[i],
		             LUMETER_CONF_ADD + i * LUMETER_CONF_LEN,
		             LUMETER_CONF_LEN);
	}
	
	memset(&LumeterState[i],0,sizeof(LumeterState_S));
	
	LumeterState[i].address = LumeterConfig[i].address;
	LumeterState[i].channel = LumeterConfig[i].channel;
}












































