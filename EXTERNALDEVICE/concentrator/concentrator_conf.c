#include "concentrator_conf.h"
#include "common.h"
#include "cat25x.h"
#include "server_protocol.h"
#include "relay_conf.h"


//需要存储的数据
Uint32TypeNumber_S ConcentratorGateWayID;					//网关ID

RUN_MODE_E RunMode = MODE_AUTO;								//运行模式
ConcentratorLocalNetConfig_S ConcentratorLocalNetConfig;	//本地网络参数
ConcentratorBasicConfig_S ConcentratorBasicConfig;			//基本配置信息
ConcentratorAlarmConfig_S ConcentratorAlarmConfig;			//告警配置参数
ConcentratorLocationConfig_S ConcentratorLocationConfig;	//经纬度年表配置
FrameWareState_S FrameWareState;							//固件升级状态


//读取网关ID
void ReadConcentratorGateWayID(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&ConcentratorGateWayID,CONCENTRATOR_GATE_WAY_ADD,sizeof(Uint32TypeNumber_S));

	crc16_cal = CRC16((u8 *)&ConcentratorGateWayID,CONCENTRATOR_GATE_WAY_LEN - 2);

	if(crc16_cal != ConcentratorGateWayID.crc16)
	{
		WriteConcentratorGateWayID(1,0);
	}
}

void WriteConcentratorGateWayID(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		ConcentratorGateWayID.number = 0x0000000A;

		ConcentratorGateWayID.crc16 = 0;
	}

	if(write_enable == 1)
	{
		ConcentratorGateWayID.crc16 = CRC16((u8 *)&ConcentratorGateWayID,CONCENTRATOR_GATE_WAY_LEN - 2);

		CAT25X_Write((u8 *)&ConcentratorGateWayID,CONCENTRATOR_GATE_WAY_ADD,CONCENTRATOR_GATE_WAY_LEN);
	}
}

//读取运行模式
void ReadRunMode(void)
{
	u8 buf[3] = {0};
	u16 crc16_read = 0;
	u16 crc16_cal = 0;

	CAT25X_Read(buf,CONCENTRATOR_RUN_MODE_ADD,CONCENTRATOR_RUN_MODE_LEN);

	crc16_read = (((u16)buf[1]) << 8) + (u16)buf[2];

	crc16_cal = CRC16(buf,CONCENTRATOR_RUN_MODE_LEN - 2);

	if(crc16_cal == crc16_read)
	{
		RunMode = (RUN_MODE_E)buf[0];
	}

	if(crc16_cal != crc16_read)
	{
		WriteRunMode(1,0);
	}
}

void WriteRunMode(u8 reset,u8 write_enable)
{
	u8 buf[3] = {0};
	u16 crc16 = 0;

	if(reset == 1)
	{
		RunMode = MODE_AUTO;
	}

	if(write_enable == 1)
	{
		crc16 = CRC16(buf,CONCENTRATOR_RUN_MODE_LEN - 2);

		buf[1] = (u8)((crc16 >> 8) & 0x00FF);
		buf[2] = (u8)((crc16 >> 0) & 0x00FF);

		CAT25X_Write(buf,CONCENTRATOR_RUN_MODE_ADD,CONCENTRATOR_RUN_MODE_LEN);
	}
}


//读取集控器基本参数配置
void ReadConcentratorBasicConfig(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&ConcentratorBasicConfig,CONCENTRATOR_BASIC_CONF_ADD,sizeof(ConcentratorBasicConfig_S));

	crc16_cal = CRC16((u8 *)&ConcentratorBasicConfig,CONCENTRATOR_BASIC_CONF_LEN - 2);

	if(crc16_cal != ConcentratorBasicConfig.crc16)
	{
		WriteConcentratorBasicConfig(1,0);
	}
}

void WriteConcentratorBasicConfig(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		ConcentratorBasicConfig.connection_mode = (u8)MODE_4G;

		memset(ConcentratorBasicConfig.server_ip,0,31);
		memcpy(ConcentratorBasicConfig.server_ip,"103.48.232.119",14);

		memset(ConcentratorBasicConfig.server_port,0,6);
		memcpy(ConcentratorBasicConfig.server_port,"7703",4);

		ConcentratorBasicConfig.heartbeat_cycle = 60;

		ConcentratorBasicConfig.electric_energy_collection_cycle = 0;
		ConcentratorBasicConfig.electric_energy_recording_time = 0;

		ConcentratorBasicConfig.loop_state_monitoring_cycle = 0;
		ConcentratorBasicConfig.loop_state_recording_time = 0;

		ConcentratorBasicConfig.cupboard_alarm_collection_cycle = 0;
		ConcentratorBasicConfig.cupboard_alarm_recording_time = 0;

		ConcentratorBasicConfig.lamp_state_collection_cycle = 0;
		ConcentratorBasicConfig.lamp_state_collection_offset = 0;
		ConcentratorBasicConfig.lamp_state_recording_time = 0;

		ConcentratorBasicConfig.command_response_timeout = 60;
		ConcentratorBasicConfig.command_retransmission_times = 3;

		ConcentratorBasicConfig.heartbeat_response_timeout = 30;
		ConcentratorBasicConfig.heartbeat_retransmission_times = 2;

		ConcentratorBasicConfig.lamp_response_timeout = 30;
		ConcentratorBasicConfig.lamp_retransmission_times = 1;

		ConcentratorBasicConfig.lamp_broadcast_times = 10;
		ConcentratorBasicConfig.lamp_broadcast_interval_time = 3000;

		memset(ConcentratorBasicConfig.operation_password,0,7);
		memcpy(ConcentratorBasicConfig.operation_password,"000000",6);

		memset(ConcentratorBasicConfig.manufacturer_name,0,33);
		memcpy(ConcentratorBasicConfig.manufacturer_name,"linktech",8);

		memset(ConcentratorBasicConfig.manufacturer_website,0,33);
		memcpy(ConcentratorBasicConfig.manufacturer_website,"www.bjlinktech.com",18);

		ConcentratorBasicConfig.crc16 = 0;
	}

	if(write_enable == 1)
	{
		ConcentratorBasicConfig.crc16 = CRC16((u8 *)&ConcentratorBasicConfig,CONCENTRATOR_BASIC_CONF_LEN - 2);

		CAT25X_Write((u8 *)&ConcentratorBasicConfig,CONCENTRATOR_BASIC_CONF_ADD,CONCENTRATOR_BASIC_CONF_LEN);
	}
}

void ReadConcentratorLocalNetConfig(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&ConcentratorLocalNetConfig,CONCENTRATOR_LOCAL_NET_CONF_ADD,sizeof(ConcentratorLocalNetConfig_S));

	crc16_cal = CRC16((u8 *)&ConcentratorLocalNetConfig,CONCENTRATOR_LOCAL_NET_CONF_LEN - 2);

	if(crc16_cal != ConcentratorLocalNetConfig.crc16)
	{
		WriteConcentratorLocalNetConfig(1,0);
	}
}

void WriteConcentratorLocalNetConfig(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		ConcentratorLocalNetConfig.dhcp_enable = 0;
		ConcentratorLocalNetConfig.local_ip[0] = 192;
		ConcentratorLocalNetConfig.local_ip[1] = 168;
		ConcentratorLocalNetConfig.local_ip[2] = 0;
		ConcentratorLocalNetConfig.local_ip[3] = 10;
		
		ConcentratorLocalNetConfig.local_msak[0] = 255;
		ConcentratorLocalNetConfig.local_msak[1] = 255;
		ConcentratorLocalNetConfig.local_msak[2] = 255;
		ConcentratorLocalNetConfig.local_msak[3] = 0;
		
		ConcentratorLocalNetConfig.local_gate[0] = 192;
		ConcentratorLocalNetConfig.local_gate[1] = 168;
		ConcentratorLocalNetConfig.local_gate[2] = 0;
		ConcentratorLocalNetConfig.local_gate[3] = 1;
		
		ConcentratorLocalNetConfig.local_dns[0] = 192;
		ConcentratorLocalNetConfig.local_dns[1] = 168;
		ConcentratorLocalNetConfig.local_dns[2] = 0;
		ConcentratorLocalNetConfig.local_dns[3] = 1;
		
		ConcentratorLocalNetConfig.remote_ip[0] = 103;
		ConcentratorLocalNetConfig.remote_ip[1] = 48;
		ConcentratorLocalNetConfig.remote_ip[2] = 232;
		ConcentratorLocalNetConfig.remote_ip[3] = 119;
		
		ConcentratorLocalNetConfig.remote_port = 7703;

		ConcentratorLocalNetConfig.crc16 = 0;
	}

	if(write_enable == 1)
	{
		ConcentratorLocalNetConfig.crc16 = CRC16((u8 *)&ConcentratorLocalNetConfig,CONCENTRATOR_LOCAL_NET_CONF_LEN - 2);

		CAT25X_Write((u8 *)&ConcentratorLocalNetConfig,CONCENTRATOR_LOCAL_NET_CONF_ADD,CONCENTRATOR_LOCAL_NET_CONF_LEN);
	}
}

//读取集控器告警配置参数
void ReadConcentratorAlarmConfig(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&ConcentratorAlarmConfig,CONCENTRATOR_ALARM_CONF_ADD,sizeof(ConcentratorAlarmConfig_S));

	crc16_cal = CRC16((u8 *)&ConcentratorAlarmConfig,CONCENTRATOR_ALARM_CONF_LEN - 2);

	if(crc16_cal != ConcentratorAlarmConfig.crc16)
	{
		WriteConcentratorAlarmConfig(1,0);
	}
}

void WriteConcentratorAlarmConfig(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		ConcentratorAlarmConfig.power_off_alarm_enable = 1;
		ConcentratorAlarmConfig.power_off_alarm_thre = 100;
		ConcentratorAlarmConfig.power_off_alarm_rm_percent = 10;
		ConcentratorAlarmConfig.electric_leakage_alarm_enable = 1;
		ConcentratorAlarmConfig.electric_leakage_alarm_v_thre = 30;
		ConcentratorAlarmConfig.electric_leakage_alarm_v_rm_percent = 10;
		ConcentratorAlarmConfig.electric_leakage_alarm_c_thre = 100;
		ConcentratorAlarmConfig.electric_leakage_alarm_c_rm_percent = 10;
		ConcentratorAlarmConfig.low_battery_alarm_enable = 1;
		ConcentratorAlarmConfig.low_battery_alarm_thre = 20;
		ConcentratorAlarmConfig.low_battery_alarm_rm_percent = 10;
		ConcentratorAlarmConfig.abnormal_charge_alarm_enable = 1;
		ConcentratorAlarmConfig.abnormal_charge_alarm_v_thre = 20;
		ConcentratorAlarmConfig.abnormal_charge_alarm_v_rm_percent = 10;
		ConcentratorAlarmConfig.abnormal_charge_alarm_c_thre = 800;
		ConcentratorAlarmConfig.abnormal_charge_alarm_c_rm_percent = 10;
		ConcentratorAlarmConfig.crc16 = 0;
	}

	if(write_enable == 1)
	{
		ConcentratorAlarmConfig.crc16 = CRC16((u8 *)&ConcentratorAlarmConfig,CONCENTRATOR_ALARM_CONF_LEN - 2);

		CAT25X_Write((u8 *)&ConcentratorAlarmConfig,CONCENTRATOR_ALARM_CONF_ADD,CONCENTRATOR_ALARM_CONF_LEN);
	}
}

//读取集控器经纬度年表
void ReadConcentratorLocationConfig(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&ConcentratorLocationConfig,CONCENTRATOR_LOCATION_CONF_ADD,sizeof(ConcentratorLocationConfig_S));

	crc16_cal = CRC16((u8 *)&ConcentratorLocationConfig,CONCENTRATOR_LOCATION_CONF_LEN - 2);

	if(crc16_cal != ConcentratorLocationConfig.crc16)
	{
		WriteConcentratorLocationConfig(1,0);
	}
}

void WriteConcentratorLocationConfig(u8 reset,u8 write_enable)
{
	u8 i = 0;
	u8 j = 0;

	if(reset == 1)
	{
		ConcentratorLocationConfig.longitude = 116.397128;
		ConcentratorLocationConfig.latitude = 39.916527;

		for(i = 0; i < 12; i ++)
		{
			for(j = 0; j < 31; j ++)
			{
				ConcentratorLocationConfig.switch_time_month_table[i].switch_time[j].on_hour = DefaultSwitchTime.on_hour;
				ConcentratorLocationConfig.switch_time_month_table[i].switch_time[j].on_minute = DefaultSwitchTime.on_minute;
				ConcentratorLocationConfig.switch_time_month_table[i].switch_time[j].off_hour = DefaultSwitchTime.off_hour;
				ConcentratorLocationConfig.switch_time_month_table[i].switch_time[j].off_minute = DefaultSwitchTime.off_minute;
			}
		}

		ConcentratorLocationConfig.crc16 = 0;
	}

	if(write_enable == 1)
	{
		ConcentratorLocationConfig.crc16 = CRC16((u8 *)&ConcentratorLocationConfig,CONCENTRATOR_LOCATION_CONF_LEN - 2);

		CAT25X_Write((u8 *)&ConcentratorLocationConfig,CONCENTRATOR_LOCATION_CONF_ADD,CONCENTRATOR_LOCATION_CONF_LEN);
	}
}

//读取固件升级状态
u8 ReadFrameWareState(void)
{
	u8 ret = 1;
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&FrameWareState,FW_STATE_ADD,sizeof(FrameWareState_S));

	crc16_cal = CRC16((u8 *)&FrameWareState,FW_STATE_LEN - 2);

	if(crc16_cal != FrameWareState.crc16)
	{
		WriteFrameWareState(1,1);
	}

	if(FrameWareState.state == FIRMWARE_DOWNLOADING ||
	   FrameWareState.state == FIRMWARE_DOWNLOAD_WAIT)
	{
		FLASH_Unlock();										//解锁
		FLASH_DataCacheCmd(DISABLE);						//FLASH擦除期间,必须禁止数据缓存

		FLASH_EraseSector(FLASH_Sector_9,VoltageRange_3);	//VCC=2.7~3.6V之间!!
		FLASH_EraseSector(FLASH_Sector_10,VoltageRange_3);	//VCC=2.7~3.6V之间!!
		FLASH_EraseSector(FLASH_Sector_11,VoltageRange_3);	//VCC=2.7~3.6V之间!!

		FLASH_DataCacheCmd(ENABLE);							//FLASH擦除结束,开启数据缓存
		FLASH_Lock();										//上锁
	}

	return ret;
}

void WriteFrameWareState(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		FrameWareState.state 			= FIRMWARE_FREE;
		FrameWareState.total_bags 		= 0;
		FrameWareState.current_bag_cnt 	= 0;
		FrameWareState.bag_size 		= 0;
		FrameWareState.last_bag_size 	= 0;
		FrameWareState.total_size 		= 0;
		FrameWareState.crc16 			= 0;
//		memset(FrameWareState.file_name,0,31);
		memset(FrameWareState.md5,0,33);
	}

	if(write_enable == 1)
	{
		FrameWareState.crc16 = CRC16((u8 *)&FrameWareState,FW_STATE_LEN - 2);

		CAT25X_Write((u8 *)&FrameWareState,FW_STATE_ADD,FW_STATE_LEN);
	}
}

































