#include "input_collector_conf.h"
#include "cat25x.h"


InputCollectorBasicConfig_S InputCollectorBasicConfig;
Uint32TypeNumber_S InputCollectorConfigNum;
InputCollectorConfig_S InputCollectorConfig[MAX_INPUT_COLLECTOR_CONF_NUM];
InputCollectorAlarmConfig_S InputCollectorAlarmConfig;

InputCollectorState_S InputCollectorState[MAX_INPUT_COLLECTOR_CONF_NUM];



//读取输入量采集模块基本配置
void ReadInputCollectorBasicConfig(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&InputCollectorBasicConfig,
	            INPUT_COLLECTOR_BASIC_CONF_ADD,
	            sizeof(InputCollectorBasicConfig_S));

	crc16_cal = CRC16((u8 *)&InputCollectorBasicConfig,INPUT_COLLECTOR_BASIC_CONF_LEN - 2);

	if(crc16_cal != InputCollectorBasicConfig.crc16)
	{
		WriteInputCollectorBasicConfig(1,0);
	}
}

void WriteInputCollectorBasicConfig(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		InputCollectorBasicConfig.detect_interval = 5000;
		InputCollectorBasicConfig.auto_report = 0;

		InputCollectorBasicConfig.crc16 = 0;
	}

	if(write_enable == 1)
	{
		InputCollectorBasicConfig.crc16 = CRC16((u8 *)&InputCollectorBasicConfig,INPUT_COLLECTOR_BASIC_CONF_LEN - 2);

		CAT25X_Write((u8 *)&InputCollectorBasicConfig,
		             INPUT_COLLECTOR_BASIC_CONF_ADD,
		             INPUT_COLLECTOR_BASIC_CONF_LEN);
	}
}

//读取输入量采集模块配置数量
void ReadInputCollectorConfigNum(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&InputCollectorConfigNum,
	            INPUT_COLLECTOR_CONF_NUM_ADD,
	            sizeof(Uint32TypeNumber_S));

	crc16_cal = CRC16((u8 *)&InputCollectorConfigNum,INPUT_COLLECTOR_CONF_NUM_LEN - 2);

	if(crc16_cal != InputCollectorConfigNum.crc16 ||
	   InputCollectorConfigNum.number > MAX_INPUT_COLLECTOR_CONF_NUM)
	{
		WriteInputCollectorConfigNum(1,0);
	}
}

void WriteInputCollectorConfigNum(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		InputCollectorConfigNum.number = 0;

		InputCollectorConfigNum.crc16 = 0;
	}

	if(write_enable == 1)
	{
		InputCollectorConfigNum.crc16 = CRC16((u8 *)&InputCollectorConfigNum,INPUT_COLLECTOR_CONF_NUM_LEN - 2);

		CAT25X_Write((u8 *)&InputCollectorConfigNum,
		             INPUT_COLLECTOR_CONF_NUM_ADD,
		             INPUT_COLLECTOR_CONF_NUM_LEN);
	}
}

//读取输入量采集模块配置
void ReadInputCollectorConfig(void)
{
	u16 crc16_cal = 0;
	u8 i = 0;
	u8 j = 0;

	ReadInputCollectorConfigNum();

	for(i = 0; i < InputCollectorConfigNum.number; i ++)
	{
		CAT25X_Read((u8 *)&InputCollectorConfig[i],
		            INPUT_COLLECTOR_CONF_ADD + i * INPUT_COLLECTOR_CONF_LEN,
		            sizeof(InputCollectorConfig_S));

		crc16_cal = CRC16((u8 *)&InputCollectorConfig[i],INPUT_COLLECTOR_CONF_LEN - 2);

		if(crc16_cal != InputCollectorConfig[i].crc16)
		{
			WriteInputCollectorConfig(i,1,0);
		}
		else
		{
			memset(&InputCollectorState[i],0,sizeof(InputCollectorState_S));
			
			InputCollectorState[i].address = InputCollectorConfig[i].address;
			InputCollectorState[i].channel = InputCollectorConfig[i].channel;
			
			if(InputCollectorConfig[i].d_loop_num <= MAX_INPUT_COLLECTOR_D_LOOP_CH_NUM)
			{
				for(j = 0; j < InputCollectorConfig[i].d_loop_num; j ++)
				{
					InputCollectorState[i].d_channel_bit |= (1 << j);
				}
			}
			else
			{
				InputCollectorState[i].d_channel_bit = 0;
			}
			
			if(InputCollectorConfig[i].a_loop_num <= MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM)
			{
				for(j = 0; j < InputCollectorConfig[i].a_loop_num; j ++)
				{
					InputCollectorState[i].a_channel_bit |= (1 << j);
				}
			}
			else
			{
				InputCollectorState[i].a_channel_bit = 0;
			}
		}
		
	}
}

void WriteInputCollectorConfig(u8 i,u8 reset,u8 write_enable)
{
	u8 j = 0;
	u8 k = 0;
	
	if(reset == 1)
	{
		memset(&InputCollectorConfig[i],0,sizeof(InputCollectorState_S));
		
		InputCollectorConfig[i].address = 0xFF;
		InputCollectorConfig[i].channel = 0xFF;
		
		for(k = 0; k < InputCollectorConfig[i].d_loop_num; k ++)
		{
			InputCollectorConfig[i].d_alarm_thre[k].confirm_time = 3;
		}
	}

	if(write_enable == 1)
	{
		InputCollectorConfig[i].crc16 = CRC16((u8 *)&InputCollectorConfig[i],INPUT_COLLECTOR_CONF_LEN - 2);

		CAT25X_Write((u8 *)&InputCollectorConfig[i],
		             INPUT_COLLECTOR_CONF_ADD + i * INPUT_COLLECTOR_CONF_LEN,
		             INPUT_COLLECTOR_CONF_LEN);
	}
	
	memset(&InputCollectorState[i],0,sizeof(InputCollectorState_S));
			
	InputCollectorState[i].address = InputCollectorConfig[i].address;
	InputCollectorState[i].channel = InputCollectorConfig[i].channel;
	
	if(InputCollectorConfig[i].d_loop_num <= MAX_INPUT_COLLECTOR_D_LOOP_CH_NUM)
	{
		for(j = 0; j < InputCollectorConfig[i].d_loop_num; j ++)
		{
			InputCollectorState[i].d_channel_bit |= (1 << j);
		}
	}
	else
	{
		InputCollectorState[i].d_channel_bit = 0;
	}
	
	if(InputCollectorConfig[i].a_loop_num <= MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM)
	{
		for(j = 0; j < InputCollectorConfig[i].a_loop_num; j ++)
		{
			InputCollectorState[i].a_channel_bit |= (1 << j);
		}
	}
	else
	{
		InputCollectorState[i].a_channel_bit = 0;
	}
}

//读取告警参数配置
void ReadInputCollectorAlarmConfig(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&InputCollectorAlarmConfig,
	            INPUT_COLLECTOR_ALARM_CONF_ADD,
	            sizeof(InputCollectorAlarmConfig_S));

	crc16_cal = CRC16((u8 *)&InputCollectorAlarmConfig,INPUT_COLLECTOR_ALARM_CONF_LEN - 2);

	if(crc16_cal != InputCollectorAlarmConfig.crc16)
	{
		WriteInputCollectorAlarmConfig(1,0);
	}
}

void WriteInputCollectorAlarmConfig(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		InputCollectorAlarmConfig.d_quantity_abnormal_alarm_enable = 0;
		InputCollectorAlarmConfig.a_quantity_abnormal_alarm_enable = 0;

		InputCollectorAlarmConfig.crc16 = 0;
	}

	if(write_enable == 1)
	{
		InputCollectorAlarmConfig.crc16 = CRC16((u8 *)&InputCollectorAlarmConfig,INPUT_COLLECTOR_ALARM_CONF_LEN - 2);

		CAT25X_Write((u8 *)&InputCollectorAlarmConfig,
		             INPUT_COLLECTOR_ALARM_CONF_ADD,
		             INPUT_COLLECTOR_ALARM_CONF_LEN);
	}
}







































