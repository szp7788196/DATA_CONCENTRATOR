#include "lamp_conf.h"
#include "cat25x.h"



LampBasicConfig_S LampBasicConfig;						//���ƻ�������


LampListNum_S LampNumList;								//�����õĵ�������
LampGroupListNum_S LampGroupListNum;					//ÿ�鵥������
Uint32TypeNumber_S LampAppointmentNum;					//����ԤԼ��������
Uint32TypeNumber_S LampStrategyNum;						//���Ʋ�����������
FrameWareState_S LampFrameWareState;					//�̼�����״̬



//��ȡ�����Զ�ͬ��ʱ������
void ReadLampBasicConfig(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&LampBasicConfig,
	            LAMP_BASIC_CONG_ADD,
	            sizeof(Uint32TypeNumber_S));

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
		LampBasicConfig.broadcast_interval_time = 3000;

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

//��ȡ���õ���������
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
		LampNumList.number = 0;

		LampNumList.crc16 = 0;
	}

	if(write_enable == 1)
	{
		LampNumList.crc16 = CRC16((u8 *)&LampNumList,LAMP_LIST_NUM_LEN - 2);

		CAT25X_Write((u8 *)&LampNumList,
		             LAMP_LIST_NUM_ADD,
		             LAMP_LIST_NUM_LEN);
	}
}

//�ж�ָ������ĵ����Ƿ�����
u8 ReadSpecifyLampNumList(u16 i)
{
	u8 ret = 0;
	u8 divisor = 0;
	u8 remainder = 0;
	
	if(i >= LampNumList.number)
	{
		return 0;
	}
	
	divisor = i / 8;
	remainder = i % 8;
	
	if(LampNumList.list[divisor] & (1 << remainder))
	{
		ret = 1;
	}
	
	return ret;
}

//�򵥵����������б��ָ��λ��д������
//mode 0ɾ�� 1���
void WriteSpecifyLampNumList(u16 i,u8 mode)
{
	u8 divisor = 0;
	u8 remainder = 0;
	
	if(i >= LampNumList.number)
	{
		return;
	}
	
	divisor = i / 8;
	remainder = i % 8;
	
	if(mode == 0)
	{
		if(LampNumList.list[divisor] & (1 << remainder))
		{
			LampNumList.number --;
		}
		
		LampNumList.list[divisor] &= ~(1 << remainder);
	}
	else
	{
		if((LampNumList.list[divisor] & (1 << remainder)) == 0)
		{
			LampNumList.number ++;
		}
		
		LampNumList.list[divisor] |= (1 << remainder);
	}
}

//��ȡÿ�鵥��������
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
		memset(LampGroupListNum.list,0,MAX_LAMP_GROUP_NUM);
		LampGroupListNum.crc16 = 0;
	}

	if(write_enable == 1)
	{
		LampGroupListNum.crc16 = CRC16((u8 *)&LampGroupListNum,LAMP_GROUP_LIST_NUM_LEN - 2);

		CAT25X_Write((u8 *)&LampGroupListNum,
		             LAMP_GROUP_LIST_NUM_ADD,
		             LAMP_GROUP_LIST_NUM_LEN);
	}
}

//��ȡ���ƻ�������
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

//��ȡ������������
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

//��ȡ���Ƴ�������
u8 ReadLampSenceConfig(u16 i,LampSenceConfig_S *sence)
{
	u8 ret = 1;
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)sence,
	            LAMP_APPOINTMENT_ADD + i * LAMP_APPOINTMENT_LEN,
	            sizeof(LampSenceConfig_S));

	crc16_cal = CRC16((u8 *)sence,LAMP_APPOINTMENT_LEN - 2);

	if(crc16_cal != sence->crc16)
	{
		ret = 0;
	}
	
	return ret;
}

void WriteLampSenceConfig(u8 i,u8 reset,LampSenceConfig_S sence)
{
	if(reset == 1)
	{
		sence.crc16 = 0;
	}
	else
	{
		sence.crc16 = CRC16((u8 *)&sence,LAMP_APPOINTMENT_LEN - 2);
	}
	
	CAT25X_Write((u8 *)&sence,
		         LAMP_APPOINTMENT_ADD + i * LAMP_APPOINTMENT_LEN,
		         LAMP_APPOINTMENT_LEN);
}

//��ȡ�̼�����״̬
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
		FLASH_Unlock();										//����
		FLASH_DataCacheCmd(DISABLE);						//FLASH�����ڼ�,�����ֹ���ݻ���

		FLASH_EraseSector(FLASH_Sector_5,VoltageRange_3);	//VCC=2.7~3.6V֮��!!

		FLASH_DataCacheCmd(ENABLE);							//FLASH��������,�������ݻ���
		FLASH_Lock();										//����
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






















