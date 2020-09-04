#include "lamp_conf.h"
#include "cat25x.h"



LampBasicConfig_S LampBasicConfig;						//单灯基础配置


LampListNum_S LampNumList;								//已配置的单灯数量
LampListNum_S LampStrategyNumList;						//已配置的任务数量
LampGroupListNum_S LampGroupListNum;					//每组单灯数量
Uint32TypeNumber_S LampAppointmentNum;					//单灯预约控制数量
Uint32TypeNumber_S LampStrategyNum;						//单灯策略配置数量
FrameWareState_S LampFrameWareState;					//固件升级状态



//读取单灯自动同步时间周期
void ReadLampBasicConfig(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&LampBasicConfig,
	            LAMP_BASIC_CONG_ADD,
	            sizeof(LampBasicConfig_S));

	crc16_cal = CRC16((u8 *)&LampBasicConfig,LAMP_BASIC_CONG_LEN - 2);

	if(crc16_cal != LampBasicConfig.crc16)
	{
		WriteLampBasicConfig(1,0);
	}
}

void WriteLampBasicConfig(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		LampBasicConfig.auto_sync_time_cycle = 86400;
		LampBasicConfig.auto_report_plc_state = 0;
		LampBasicConfig.state_collection_cycle = 0;
		LampBasicConfig.state_collection_offset = 0;
		LampBasicConfig.state_recording_time = 0;
		LampBasicConfig.response_timeout = 30;
		LampBasicConfig.retransmission_times = 1;
		LampBasicConfig.broadcast_times = 10;
		LampBasicConfig.broadcast_interval_time = 500;

		LampBasicConfig.crc16 = 0;
	}

	if(write_enable == 1)
	{
		LampBasicConfig.crc16 = CRC16((u8 *)&LampBasicConfig,LAMP_BASIC_CONG_LEN - 2);

		CAT25X_Write((u8 *)&LampBasicConfig,
		             LAMP_BASIC_CONG_ADD,
		             LAMP_BASIC_CONG_LEN);
	}
}

//读取配置单灯数量表
void ReadLampNumList(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&LampNumList,
	            LAMP_LIST_NUM_ADD,
	            sizeof(LampListNum_S));

	crc16_cal = CRC16((u8 *)&LampNumList,LAMP_LIST_NUM_LEN - 2);

	if(crc16_cal != LampNumList.crc16 ||
	   LampNumList.number > MAX_LAMP_CONF_NUM)
	{
		WriteLampNumList(1,0);
	}
}

void WriteLampNumList(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		memset(&LampNumList,0,sizeof(LampListNum_S));
	}

	if(write_enable == 1)
	{
		LampNumList.crc16 = CRC16((u8 *)&LampNumList,LAMP_LIST_NUM_LEN - 2);

		CAT25X_Write((u8 *)&LampNumList,
		             LAMP_LIST_NUM_ADD,
		             LAMP_LIST_NUM_LEN);
	}
}

//判断指定号码的单灯是否被配置
u8 ReadSpecifyLampNumList(u16 i)
{
	u8 ret = 0;
	u8 divisor = 0;
	u8 remainder = 0;
	
	if(i >= MAX_LAMP_CONF_NUM)
	{
		return 0;
	}
	
	divisor = i / 8;
	remainder = i % 8;
	
	if((LampNumList.list[divisor] & (1 << remainder)) != 0)
	{
		ret = 1;
	}
	
	return ret;
}

//向单灯配置数量列表的指定位置写入数据
//mode 0删除 1添加
void WriteSpecifyLampNumList(u16 i,u8 mode)
{
	u8 divisor = 0;
	u8 remainder = 0;
	
	if(i >= MAX_LAMP_CONF_NUM)
	{
		return;
	}
	
	divisor = i / 8;
	remainder = i % 8;
	
	if(mode == 0)
	{
		if((LampNumList.list[divisor] & (1 << remainder)) != 0)
		{
			LampNumList.number --;
		}
		
		LampNumList.list[divisor] &= ~(1 << remainder);
	}
	else
	{
		if(((LampNumList.list[divisor] & (1 << remainder))) == 0)
		{
			LampNumList.number ++;
		}
		
		LampNumList.list[divisor] |= (1 << remainder);
	}
}

//读取配置单灯任务数量表
void ReadLampStrategyNumList(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&LampStrategyNumList,
	            LAMP_STRATEGY_LIST_NUM_ADD,
	            sizeof(LampListNum_S));

	crc16_cal = CRC16((u8 *)&LampStrategyNumList,LAMP_STRATEGY_LIST_NUM_LEN - 2);

	if(crc16_cal != LampStrategyNumList.crc16 ||
	   LampStrategyNumList.number > MAX_LAMP_STRATEGY_NUM)
	{
		WriteLampStrategyNumList(1,0);
	}
}

void WriteLampStrategyNumList(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		memset(&LampStrategyNumList,0,sizeof(LampListNum_S));
	}

	if(write_enable == 1)
	{
		LampStrategyNumList.crc16 = CRC16((u8 *)&LampStrategyNumList,LAMP_STRATEGY_LIST_NUM_LEN - 2);

		CAT25X_Write((u8 *)&LampStrategyNumList,
		             LAMP_STRATEGY_LIST_NUM_ADD,
		             LAMP_STRATEGY_LIST_NUM_LEN);
	}
}

//判断指定号码的单灯是否被配置
u8 ReadSpecifyLampStrategyNumList(u16 i)
{
	u8 ret = 0;
	u8 divisor = 0;
	u8 remainder = 0;
	
	if(i >= MAX_LAMP_STRATEGY_NUM)
	{
		return 0;
	}
	
	divisor = i / 8;
	remainder = i % 8;
	
	if((LampStrategyNumList.list[divisor] & (1 << remainder)) != 0)
	{
		ret = 1;
	}
	
	return ret;
}

//向单灯配置数量列表的指定位置写入数据
//mode 0删除 1添加
void WriteSpecifyLampStrategyNumList(u16 i,u8 mode)
{
	u8 divisor = 0;
	u8 remainder = 0;
	
	if(i >= MAX_LAMP_STRATEGY_NUM)
	{
		return;
	}
	
	divisor = i / 8;
	remainder = i % 8;
	
	if(mode == 0)
	{
		if((LampStrategyNumList.list[divisor] & (1 << remainder)) != 0)
		{
			LampStrategyNumList.number --;
		}
		
		LampStrategyNumList.list[divisor] &= ~(1 << remainder);
	}
	else
	{
		if(((LampStrategyNumList.list[divisor] & (1 << remainder))) == 0)
		{
			LampStrategyNumList.number ++;
		}
		
		LampStrategyNumList.list[divisor] |= (1 << remainder);
	}
}

//读取每组单灯数量表
void ReadLampGroupListNum(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&LampGroupListNum,
	            LAMP_GROUP_LIST_NUM_ADD,
	            sizeof(LampGroupListNum_S));

	crc16_cal = CRC16((u8 *)&LampGroupListNum,LAMP_GROUP_LIST_NUM_LEN - 2);

	if(crc16_cal != LampGroupListNum.crc16)
	{
		WriteLampGroupListNum(1,0);
	}
}

void WriteLampGroupListNum(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		memset(&LampGroupListNum,0,sizeof(LampGroupListNum_S));
	}

	if(write_enable == 1)
	{
		LampGroupListNum.crc16 = CRC16((u8 *)&LampGroupListNum,LAMP_GROUP_LIST_NUM_LEN - 2);

		CAT25X_Write((u8 *)&LampGroupListNum,
		             LAMP_GROUP_LIST_NUM_ADD,
		             LAMP_GROUP_LIST_NUM_LEN);
	}
}

void ReadLampAppointmentNum(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&LampAppointmentNum,
	            LAMP_APPOINTMENT_NUM_ADD,
	            sizeof(Uint32TypeNumber_S));

	crc16_cal = CRC16((u8 *)&LampAppointmentNum,LAMP_APPOINTMENT_NUM_LEN - 2);

	if(crc16_cal != LampAppointmentNum.crc16 ||
	   LampAppointmentNum.number > MAX_LAMP_APPOINTMENT_NUM)
	{
		WriteLampAppointmentNum(1,0);
	}
}

void WriteLampAppointmentNum(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		LampAppointmentNum.number = 0;

		LampAppointmentNum.crc16 = 0;
	}

	if(write_enable == 1)
	{
		LampAppointmentNum.crc16 = CRC16((u8 *)&LampAppointmentNum,LAMP_APPOINTMENT_NUM_LEN - 2);

		CAT25X_Write((u8 *)&LampAppointmentNum,
		             LAMP_APPOINTMENT_NUM_ADD,
		             LAMP_APPOINTMENT_NUM_LEN);
	}
}

u8 ReadLampAppointment(u8 i,LampSenceConfig_S *appointment)
{
	u8 ret = 0;
	u16 crc16_cal = 0;
	
	if(appointment == NULL)
	{
		return ret;
	}

	CAT25X_Read((u8 *)appointment,
	            LAMP_APPOINTMENT_ADD + LAMP_APPOINTMENT_LEN * i,
	            sizeof(LampSenceConfig_S));

	crc16_cal = CRC16((u8 *)appointment,LAMP_APPOINTMENT_LEN - 2);

	if(crc16_cal == appointment->crc16)
	{
		ret = 1;
	}
	
	return ret;
}

void WriteLampAppointment(u8 i,LampSenceConfig_S *appointment,u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		memset(appointment,0,sizeof(LampSenceConfig_S));
	}

	if(write_enable == 1)
	{
		appointment->crc16 = CRC16((u8 *)appointment,LAMP_APPOINTMENT_LEN - 2);

		CAT25X_Write((u8 *)appointment,
		             LAMP_APPOINTMENT_ADD + LAMP_APPOINTMENT_LEN * i,
		             LAMP_APPOINTMENT_LEN);
	}
}

//读取单灯基础配置
u8 ReadLampConfig(u16 i,LampConfig_S *config)
{
	u8 ret = 1;
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)config,
	            LAMP_CONF_ADD + i * LAMP_CONF_LEN,
	            sizeof(LampConfig_S));

	crc16_cal = CRC16((u8 *)config,LAMP_CONF_LEN - 2);

	if(crc16_cal != config->crc16)
	{
		ret = 0;
	}
	
	return ret;
}

void WriteLampConfig(u8 i,u8 reset,LampConfig_S config)
{
	if(reset == 1)
	{
		config.crc16 = 0;
	}
	else
	{
		config.crc16 = CRC16((u8 *)&config,LAMP_CONF_LEN - 2);
	}
	
	CAT25X_Write((u8 *)&config,
		         LAMP_CONF_ADD + i * LAMP_CONF_LEN,
		         LAMP_CONF_LEN);
}

//读取单灯任务配置
u8 ReadLampTaskConfig(u16 i,LampTask_S *task)
{
	u8 ret = 1;
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)task,
	            LAMP_STRATEGY_ADD + i * LAMP_STRATEGY_LEN,
	            sizeof(LampTask_S));

	crc16_cal = CRC16((u8 *)task,LAMP_STRATEGY_LEN - 2);

	if(crc16_cal != task->crc16)
	{
		ret = 0;
	}
	
	return ret;
}

void WriteLampTaskConfig(u8 i,u8 reset,LampTask_S task)
{
	if(reset == 1)
	{
		task.crc16 = 0;
	}
	else
	{
		task.crc16 = CRC16((u8 *)&task,LAMP_STRATEGY_LEN - 2);
	}
	
	CAT25X_Write((u8 *)&task,
		         LAMP_STRATEGY_ADD + i * LAMP_STRATEGY_LEN,
		         LAMP_STRATEGY_LEN);
}

//读取固件升级状态
u8 ReadLampFrameWareState(void)
{
	u8 ret = 1;
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&LampFrameWareState,LAMP_FW_STATE_ADD,sizeof(FrameWareState_S));

	crc16_cal = CRC16((u8 *)&LampFrameWareState,FW_STATE_LEN - 2);

	if(crc16_cal != LampFrameWareState.crc16)
	{
		WriteLampFrameWareState(1,1);
	}

	if(LampFrameWareState.state == FIRMWARE_DOWNLOADING ||
	   LampFrameWareState.state == FIRMWARE_DOWNLOAD_WAIT)
	{
		FLASH_Unlock();										//解锁
		FLASH_DataCacheCmd(DISABLE);						//FLASH擦除期间,必须禁止数据缓存

		FLASH_EraseSector(FLASH_Sector_5,VoltageRange_3);	//VCC=2.7~3.6V之间!!

		FLASH_DataCacheCmd(ENABLE);							//FLASH擦除结束,开启数据缓存
		FLASH_Lock();										//上锁
	}

	return ret;
}

void WriteLampFrameWareState(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		RESET:
		LampFrameWareState.state 			= FIRMWARE_FREE;
		LampFrameWareState.total_bags 		= 0;
		LampFrameWareState.current_bag_cnt 	= 0;
		LampFrameWareState.bag_size 		= 0;
		LampFrameWareState.last_bag_size 	= 0;
		LampFrameWareState.total_size 		= 0;
		LampFrameWareState.crc16 			= 0;
//		memset(FrameWareState.file_name,0,31);
		memset(LampFrameWareState.md5,0,33);
	}
	else if(reset == 2)
	{
		if(LampFrameWareState.state == FIRMWARE_DOWNLOADED)
		{
			LampFrameWareState.state 			= FIRMWARE_DOWNLOADED_SUCCESS;
//			LampFrameWareState.total_bags 		= 0;
//			LampFrameWareState.current_bag_cnt 	= 0;
//			LampFrameWareState.bag_size 		= 0;
//			LampFrameWareState.last_bag_size 	= 0;
//			LampFrameWareState.total_size 		= 0;
			LampFrameWareState.crc16 			= 0;
	//		memset(FrameWareState.file_name,0,31);
//			memset(LampFrameWareState.md5,0,33);
		}
		else
		{
			goto RESET;
		}
	}

	if(write_enable == 1)
	{
		LampFrameWareState.crc16 = CRC16((u8 *)&LampFrameWareState,LAMP_FW_STATE_LEN - 2);

		CAT25X_Write((u8 *)&LampFrameWareState,LAMP_FW_STATE_ADD,LAMP_FW_STATE_LEN);
	}
}






















