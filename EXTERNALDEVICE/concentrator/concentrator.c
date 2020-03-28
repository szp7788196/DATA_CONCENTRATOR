#include "concentrator.h"
#include "common.h"
#include "cat25x.h"
#include "server_protocol.h"
#include "task_handle_server_frame.h"
#include "rx8010s.h"
#include "ec20.h"
#include "stmflash.h"


//需要存储的数据
RUN_MODE_E RunMode = MODE_AUTO;								//运行模式
ConcentratorBasicConfig_S ConcentratorBasicConfig;			//基本配置信息
ConcentratorAlarmConfig_S ConcentratorAlarmConfig;			//告警配置参数
ConcentratorLocationConfig_S ConcentratorLocationConfig;	//经纬度年表配置
FrameWareState_S FrameWareState;							//固件升级状态


//不需要存储的数据
u8 FlagSystemReBoot = 0;								//系统重启标识
u8 FlagReConnectToServer = 0;							//断网重连标志
u8 LoginResponse = 0;									//服务器对登录包的相应
u8 HeartBeatResponse = 0;								//服务器对心跳包的响应
u8 AlarmReportResponse = 0;								//服务器对告警包的相应




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
		ConcentratorAlarmConfig.electric_leakage_alarm_enable = 1;
		ConcentratorAlarmConfig.electric_leakage_alarm_v_thre = 30;
		ConcentratorAlarmConfig.electric_leakage_alarm_c_thre = 100;
		ConcentratorAlarmConfig.low_battery_alarm_enable = 1;
		ConcentratorAlarmConfig.low_battery_alarm_thre = 20;
		ConcentratorAlarmConfig.abnormal_charge_alarm_enable = 1;
		ConcentratorAlarmConfig.abnormal_charge_alarm_v_thre = 20;
		ConcentratorAlarmConfig.abnormal_charge_alarm_c_thre = 800;
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
				ConcentratorLocationConfig.switch_time_month_table[i].switch_time[j].on_hour = 6;
				ConcentratorLocationConfig.switch_time_month_table[i].switch_time[j].on_minute = 0;
				ConcentratorLocationConfig.switch_time_month_table[i].switch_time[j].off_hour = 18;
				ConcentratorLocationConfig.switch_time_month_table[i].switch_time[j].off_minute = 0;
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

//发送登录包
void SendLoginFrameToServer(void)
{
	u8 i = 0;
	char buf[25];
	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

	server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(server_frame_struct != NULL)
	{
		InitServerFrameStruct(server_frame_struct);

		server_frame_struct->msg_type 	= (u8)DEVICE_REQUEST_UP;	//响应服务器类型
		server_frame_struct->msg_len 	= 10;
		server_frame_struct->err_code 	= (u8)NO_ERR;
		server_frame_struct->msg_id		= 0x00E0;
		server_frame_struct->para_num	= 8;

		server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

		if(server_frame_struct->para != NULL)
		{
			server_frame_struct->para[i].type = 0x8001;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.connection_mode);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x8002;
			memset(buf,0,25);
			sprintf(buf, "1");
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x8003;
			memset(buf,0,25);
			sprintf(buf, "%d",EC20Info.csq);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x4004;
			memset(buf,0,25);
			sprintf(buf, "LNK-RTU");
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x4005;
			memset(buf,0,25);
			sprintf(buf, "%d",HARD_WARE_VRESION);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x4006;
			memset(buf,0,25);
			sprintf(buf, "%d",SOFT_WARE_VRESION);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0xA007;
			memset(buf,0,25);
			TimeToString((u8 *)buf,calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x8008;
			memset(buf,0,25);
			sprintf(buf, "0");
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;
		}

		ConvertFrameStructToFrame(server_frame_struct);
	}
}

//发送登录包
void SendHeartBeatFrameToServer(void)
{
	u8 i = 0;
	char buf[25];
	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

	server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(server_frame_struct != NULL)
	{
		InitServerFrameStruct(server_frame_struct);

		server_frame_struct->msg_type 	= (u8)DEVICE_REQUEST_UP;	//响应服务器类型
		server_frame_struct->msg_len 	= 10;
		server_frame_struct->err_code 	= (u8)NO_ERR;
		server_frame_struct->msg_id		= 0x00E3;
		server_frame_struct->para_num	= 2;

		server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

		if(server_frame_struct->para != NULL)
		{
			server_frame_struct->para[i].type = 0x8001;
			memset(buf,0,25);
			sprintf(buf, "%d",EC20Info.csq);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0xA002;
			memset(buf,0,25);
			TimeToString((u8 *)buf,calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;
		}

		ConvertFrameStructToFrame(server_frame_struct);
	}
}

//发送OTA请求包
void SendOtaRequestFrameToServer(FrameWareState_S frame_ware_state)
{
	u8 i = 0;
	char buf[25];
	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

	server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(server_frame_struct != NULL)
	{
		InitServerFrameStruct(server_frame_struct);

		server_frame_struct->msg_type 	= (u8)DEVICE_REQUEST_UP;	//响应服务器类型
		server_frame_struct->msg_len 	= 10;
		server_frame_struct->err_code 	= (u8)NO_ERR;
		server_frame_struct->msg_id		= 0x00F1;
		server_frame_struct->para_num	= 3;

		server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

		if(server_frame_struct->para != NULL)
		{
			server_frame_struct->para[i].type = 0x4001;
			server_frame_struct->para[i].len = strlen((char *)FrameWareState.md5);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,FrameWareState.md5,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x8002;
			memset(buf,0,25);
			sprintf(buf, "%d",FIRMWARE_BAG_SIZE);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x8003;
			memset(buf,0,25);
			sprintf(buf, "%d",frame_ware_state.current_bag_cnt);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;
		}

		ConvertFrameStructToFrame(server_frame_struct);
	}
}

//发送OTA完成包
void SendOtaCompleteFrameToServer(void)
{
	u8 i = 0;
	u8 tmp = 0;
	char buf[25];
	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

	server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(server_frame_struct != NULL)
	{
		InitServerFrameStruct(server_frame_struct);

		server_frame_struct->msg_type 	= (u8)DEVICE_REQUEST_UP;	//响应服务器类型
		server_frame_struct->msg_len 	= 10;
		server_frame_struct->err_code 	= (u8)NO_ERR;
		server_frame_struct->msg_id		= 0x00F2;
		server_frame_struct->para_num	= 3;

		server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

		if(server_frame_struct->para != NULL)
		{
			server_frame_struct->para[i].type = 0x4001;
			server_frame_struct->para[i].len = strlen((char *)FrameWareState.md5);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,FrameWareState.md5,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x6002;
			if(FrameWareState.state == FIRMWARE_UPDATE_SUCCESS)
			{
				tmp = 1;
			}
			else if(FrameWareState.state == FIRMWARE_UPDATE_FAILED || FrameWareState.state == FIRMWARE_DOWNLOAD_FAILED)
			{
				tmp = 0;
			}
			memset(buf,0,25);
			sprintf(buf, "%d",tmp);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x4003;
			memset(buf,0,25);
			sprintf(buf, "%d",SOFT_WARE_VRESION);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;
		}

		ConvertFrameStructToFrame(server_frame_struct);
	}
}

//发送告警事件
void SendAlarmReportFrameToServer(AlarmReport_S *alarm_report)
{
	u8 i = 0;
	char buf[25];
	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

	server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(server_frame_struct != NULL && alarm_report != NULL)
	{
		InitServerFrameStruct(server_frame_struct);

		server_frame_struct->msg_type 	= (u8)DEVICE_REQUEST_UP;	//响应服务器类型
		server_frame_struct->msg_len 	= 10;
		server_frame_struct->err_code 	= (u8)NO_ERR;
//		server_frame_struct->msg_id		= 0x0000;
		server_frame_struct->para_num	= 8;
		
		if(alarm_report->record_type == 1)
		{
			server_frame_struct->msg_id = 0x00A0;
		}
		else if(alarm_report->record_type == 0)
		{
			server_frame_struct->msg_id = 0x00A0;
		}
		
		server_frame_struct->msg_id += ((((u16)alarm_report->device_type) << 8) & 0xFF00);

		server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

		if(server_frame_struct->para != NULL)
		{
			server_frame_struct->para[i].type = 0x8001;
			memset(buf,0,25);
			sprintf(buf, "%d",alarm_report->device_type);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;
			
			server_frame_struct->para[i].type = 0x8002;
			memset(buf,0,25);
			sprintf(buf, "%d",alarm_report->record_type);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;
			
			server_frame_struct->para[i].type = 0x4003;
			server_frame_struct->para[i].len = strlen((char *)alarm_report->device_address);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,alarm_report->device_address,server_frame_struct->para[i].len + 1);
			}
			i ++;
			
			server_frame_struct->para[i].type = 0x4004;
			server_frame_struct->para[i].len = strlen((char *)alarm_report->device_channel);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,alarm_report->device_channel,server_frame_struct->para[i].len + 1);
			}
			i ++;
			
			server_frame_struct->para[i].type = 0x4005;
			server_frame_struct->para[i].len = strlen((char *)alarm_report->current_value);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,alarm_report->current_value,server_frame_struct->para[i].len + 1);
			}
			i ++;
			
			server_frame_struct->para[i].type = 0x4006;
			server_frame_struct->para[i].len = strlen((char *)alarm_report->set_value);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,alarm_report->set_value,server_frame_struct->para[i].len + 1);
			}
			i ++;
			
			server_frame_struct->para[i].type = 0x4007;
			server_frame_struct->para[i].len = strlen((char *)alarm_report->reference_value);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,alarm_report->reference_value,server_frame_struct->para[i].len + 1);
			}
			i ++;
			
			server_frame_struct->para[i].type = 0xA008;
			server_frame_struct->para[i].len = strlen((char *)alarm_report->occur_time);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,alarm_report->occur_time,server_frame_struct->para[i].len + 1);
			}
			i ++;
		}

		ConvertFrameStructToFrame(server_frame_struct);
	}
}

void AutoSendFrameToServer(void)
{
	static time_t time_1 = 0;
	static u8 retry_times1 = 0;
	
	static time_t time_2_1 = 0;
	static time_t time_2_2 = 0;
	static u8 retry_times2 = 0;
	
	static time_t time_3 = 0;
	static u8 retry_times3 = 0;
	
	static time_t time_4 = 0;
	static u8 retry_times4 = 0;
	
	BaseType_t xResult;
	static AlarmReport_S *alarm_report = NULL;
	static time_t time_5 = 0;
	static u8 retry_times5 = 0;


/**********************************发送登录包**********************************/	
	if(LoginResponse == 0)		//未登录状态
	{
		if(GetSysTick1s() - time_1 >= ConcentratorBasicConfig.command_response_timeout)
		{
			time_1 = GetSysTick1s();

			if((retry_times1 ++) < ConcentratorBasicConfig.command_retransmission_times)
			{
				SendLoginFrameToServer();
			}
			else
			{
				retry_times1 = 0;
//				FlagReConnectToServer = 1;		//多次登录失败后断网重连
			}
		}
	}
	else
	{
		retry_times1 = 0;
/**********************************发送心跳包**********************************/
		if(GetSysTick1s() - time_2_1 >= ConcentratorBasicConfig.heartbeat_cycle)
		{
			time_2_1 = GetSysTick1s();
			time_2_2 = GetSysTick1s();

			RE_SEND_HEART_BEAT:
			SendHeartBeatFrameToServer();

			HeartBeatResponse = 0;
		}

		if(HeartBeatResponse == 0)
		{
			if(GetSysTick1s() - time_2_2 >= ConcentratorBasicConfig.heartbeat_response_timeout)
			{
				time_2_2 = GetSysTick1s();

				if((retry_times2 ++) < ConcentratorBasicConfig.heartbeat_retransmission_times)
				{
					goto RE_SEND_HEART_BEAT;
				}
				else
				{
					retry_times2 = 0;
//					LoginResponse = 0;
//					FlagReConnectToServer = 1;		//多次登录失败后断网重连
				}
			}
		}
		else
		{
			retry_times2 = 0;
		}
/**********************************发送升级完成通知包**********************************/
		if(FrameWareState.state == FIRMWARE_UPDATE_SUCCESS ||
		   FrameWareState.state == FIRMWARE_UPDATE_FAILED ||
		   FrameWareState.state == FIRMWARE_DOWNLOAD_FAILED)
		{
			if(GetSysTick1s() - time_3 >= ConcentratorBasicConfig.command_response_timeout)
			{
				time_3 = GetSysTick1s();

				if((retry_times3 ++) < ConcentratorBasicConfig.command_retransmission_times)
				{
					SendOtaCompleteFrameToServer();
				}
				else
				{
					retry_times3 = 0;
				}
			}
		}
		else
		{
			retry_times3 = 0;
		}
/**********************************发送固件请求包**********************************/		
		if(FrameWareState.state == FIRMWARE_DOWNLOADING)
		{
			time_4 = GetSysTick1s();
			retry_times4 = 0;
			
			RE_SEND_OTA_REQUEST:
			SendOtaRequestFrameToServer(FrameWareState);
			
			FrameWareState.state = FIRMWARE_DOWNLOAD_WAIT;
		}

		if(FrameWareState.state == FIRMWARE_DOWNLOAD_WAIT)
		{
			if(GetSysTick1s() - time_4 >= ConcentratorBasicConfig.command_response_timeout)
			{
				time_4 = GetSysTick1s();

				if((retry_times4 ++) < ConcentratorBasicConfig.command_retransmission_times)
				{
					goto RE_SEND_OTA_REQUEST;
				}
				else
				{
					retry_times4 = 0;
					
					FrameWareState.state = FIRMWARE_DOWNLOAD_FAILED;
				}
			}
		}
		else
		{
			retry_times4 = 0;
		}
/**********************************发送告警上报包**********************************/		
		if(AlarmReportResponse == 0)
		{
			if(alarm_report != NULL)
			{
				DeleteAlarmReport(alarm_report);
			}
			
			if(alarm_report == NULL)
			{
				xResult = xQueueReceive(xQueue_AlarmReportSend,(void *)&alarm_report,(TickType_t)pdMS_TO_TICKS(1));
			
				if(xResult == pdPASS)
				{
					if(AlarmReportResponse == 0)
					{
						AlarmReportResponse = 1;
						
						time_5 = GetSysTick1s();
						retry_times5 = 0;
						
						RE_SEND_ALARM_REPORT:
						SendAlarmReportFrameToServer(alarm_report);
					}
				}
			}
		}

		if(AlarmReportResponse == 1)
		{
			if(GetSysTick1s() - time_5 >= ConcentratorBasicConfig.command_response_timeout)
			{
				time_5 = GetSysTick1s();

				if((retry_times5 ++) < ConcentratorBasicConfig.command_retransmission_times)
				{
					goto RE_SEND_ALARM_REPORT;
				}
				else
				{
					retry_times5 = 0;
					
					AlarmReportResponse = 0;
					
					DeleteAlarmReport(alarm_report);
				}
			}
		}
		
	}
}


void RecvAndHandleFrameStruct(void)
{
	BaseType_t xResult;
	ServerFrameStruct_S *server_frame_struct = NULL;

	xResult = xQueueReceive(xQueue_ConcentratorFrameStruct,(void *)&server_frame_struct,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS )
	{
		switch(server_frame_struct->msg_id)
		{
			case 0x0000:	//数据透传
				TransparentTransmission(server_frame_struct);
			break;

			case 0x0001:	//校时
				SynchronizeTime(server_frame_struct);
			break;

			case 0x0002:	//复位
				ResetConfigParameters(server_frame_struct);
			break;

			case 0x0003:	//重启
				RebootTheSystem(server_frame_struct);
			break;

			case 0x0004:	//重连
				ReConnectToServer(server_frame_struct);
			break;

			case 0x0005:	//手自控切换
				SetRunMode(server_frame_struct);
			break;

			case 0x0070:	//日志查询

			break;

			case 0x0071:	//状态查询
				QueryState(server_frame_struct);
			break;

			case 0x0072:	//状态历史查询

			break;

			case 0x00A0:	//告警上报
				AlarmReportResponse = 0;
			break;

			case 0x00A1:	//告警解除
				AlarmReportResponse = 0;
			break;

			case 0x00A2:	//事件上报

			break;

			case 0x00A3:	//告警配置
				SetAlarmConfiguration(server_frame_struct);
			break;

			case 0x00A4:	//告警配置查询
				GetAlarmConfiguration(server_frame_struct);
			break;

			case 0x00A5:	//告警历史查询

			break;

			case 0x00D0:	//基础配置
				SetBasicConfiguration(server_frame_struct);
			break;

			case 0x00D1:	//基础配置查询
				GetBasicConfiguration(server_frame_struct);
			break;

			case 0x00D2:	//经纬度年表配置
				SetLocationConfiguration(server_frame_struct);
			break;

			case 0x00D3:	//经纬度年表查询
				GetLocationConfiguration(server_frame_struct);
			break;

			case 0x00E0:	//登录
				LoginResponse = 1;
			break;

			case 0x00E1:	//加密请求

			break;

			case 0x00E2:	//密钥更新

			break;

			case 0x00E3:	//心跳
				HeartBeatResponse = 1;
			break;

			case 0x00F0:	//请求升级
				RequestFrameWareUpDate(server_frame_struct);
			break;

			case 0x00F1:	//获取升级包
				RecvFrameWareBag(server_frame_struct);
			break;

			case 0x00F2:	//升级完成
				WriteFrameWareState(1,1);
			break;

			case 0x00F3:	//停止升级
				FrameWareUpDateStop(server_frame_struct);
			break;

			default:
			break;
		}

		DeleteServerFrameStruct(server_frame_struct);
	}
}

//透传指令
u8 TransparentTransmission(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 type = 0;
	DEVICE_TYPE_E device_type = RELAY;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器
	ServerFrameStruct_S *tran_server_frame_struct = NULL;		//用于透传到其他设备

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));
	tran_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	if(tran_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,tran_server_frame_struct,1);

		type = atoi((char *)tran_server_frame_struct->para[0].value);

		switch(type)
		{
			case 0:

			break;

			case 1:
				device_type = ELECTRIC_METER;
			break;

			case 2:
				device_type = RELAY;
			break;

			case 3:
				device_type = ELECTRIC_METER;
			break;

			case 4:
				device_type = LUMETER;
			break;

			default:

			break;
		}

		if(device_type != CONCENTRATOR && device_type != UNKNOW_DEVICE)
		{
			TransServerFrameStructToOtherTask(tran_server_frame_struct,device_type);
		}
		else
		{
			DeleteServerFrameStruct(tran_server_frame_struct);
		}
	}

	return ret;
}

//校时指令
u8 SynchronizeTime(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	char buf[5];
	_calendar_obj cal;
	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	memset(buf,0,5);
	memcpy(buf,&server_frame_struct->para[0].value[0],4);
	cal.w_year = atoi(buf);

	memset(buf,0,5);
	memcpy(buf,&server_frame_struct->para[0].value[4],2);
	cal.w_month = atoi(buf);

	memset(buf,0,5);
	memcpy(buf,&server_frame_struct->para[0].value[6],2);
	cal.w_date = atoi(buf);

	memset(buf,0,5);
	memcpy(buf,&server_frame_struct->para[0].value[8],2);
	cal.hour = atoi(buf);

	memset(buf,0,5);
	memcpy(buf,&server_frame_struct->para[0].value[10],2);
	cal.min = atoi(buf);

	memset(buf,0,5);
	memcpy(buf,&server_frame_struct->para[0].value[12],2);
	cal.sec = atoi(buf);

	RX8010S_Set_Time(cal.w_year - 2000,cal.w_month,cal.w_date,cal.hour,cal.min,cal.sec);

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,1);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= server_frame_struct->msg_len;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		resp_server_frame_struct->para[0].type = 0xA101;

		RX8010S_Get_Time();

		TimeToString((u8 *)resp_server_frame_struct->para[0].value,
                     calendar.w_year,
		             calendar.w_month,
		             calendar.w_date,
		             calendar.hour,
		             calendar.min,
					 calendar.sec);

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

//恢复出厂设置
u8 ResetConfigParameters(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 reset_type = 0;
	u8 reboot_type = 0;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	reset_type = atoi((char *)server_frame_struct->para[0].value);
	reboot_type = atoi((char *)server_frame_struct->para[1].value);

	if(reset_type == 1)
	{
		WriteRunMode(1,1);
		WriteConcentratorBasicConfig(1,1);
	}
	else if(reset_type == 2)
	{
		WriteRunMode(1,1);
		WriteConcentratorBasicConfig(1,1);
	}

	FlagSystemReBoot = reboot_type;

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

//重启系统
u8 RebootTheSystem(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	FlagSystemReBoot = 1;

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

//重新连接服务器
u8 ReConnectToServer(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	FlagReConnectToServer = 1;

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

//设置运行模式
u8 SetRunMode(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 run_mode = 0;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	run_mode = atoi((char *)server_frame_struct->para[0].value);

	if(run_mode >= (u8)MODE_AUTO && run_mode <= (u8)MODE_STOP)
	{
		RunMode = (RUN_MODE_E)run_mode;
	}

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		if(run_mode < (u8)MODE_AUTO || run_mode > (u8)MODE_STOP)
		{
			resp_server_frame_struct->err_code 	= (u8)PARA_RANGE_ERR;
		}

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

//状态查询
u8 QueryState(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 ret = 0;
	char buf[25] = {0};

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;
		resp_server_frame_struct->para_num = 9;

		resp_server_frame_struct->para = (Parameter_S *)pvPortMalloc(resp_server_frame_struct->para_num * sizeof(Parameter_S));

		if(resp_server_frame_struct->para != NULL)
		{
			resp_server_frame_struct->para[i].type = 0x8101;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.connection_mode);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8102;
			memset(buf,0,25);
			sprintf(buf, "1");
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8103;
			memset(buf,0,25);
			sprintf(buf, "%d",EC20Info.csq);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8104;
			memset(buf,0,25);
			TimeToString((u8 *)buf,
                     calendar.w_year,
		             calendar.w_month,
		             calendar.w_date,
		             calendar.hour,
		             calendar.min,
					 calendar.sec);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x4105;
			resp_server_frame_struct->para[i].len = strlen((char *)EC20Info.apn);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,(char *)EC20Info.apn,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x4106;
			resp_server_frame_struct->para[i].len = strlen((char *)EC20Info.imei);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,(char *)EC20Info.imei,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x4107;
			resp_server_frame_struct->para[i].len = strlen((char *)EC20Info.iccid);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,(char *)EC20Info.iccid,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x4107;
			resp_server_frame_struct->para[i].len = strlen((char *)EC20Info.imsi);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,(char *)EC20Info.imsi,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x4117;
			resp_server_frame_struct->para[i].len = 0;
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				resp_server_frame_struct->para[i].value[0] = 0;
			}
			i ++;
		}

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

//设置告警配置参数
u8 SetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	char tmp[10];
	char *msg = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	for(j = 0; j < server_frame_struct->para_num; j ++)
	{
		switch(server_frame_struct->para[j].type)
		{
			case 0x6001:
				ConcentratorAlarmConfig.power_off_alarm_enable = atoi((char *)server_frame_struct->para[0].value);
			break;

			case 0x4002:
				ConcentratorAlarmConfig.power_off_alarm_thre = atoi((char *)server_frame_struct->para[1].value);
			break;

			case 0x6003:
				ConcentratorAlarmConfig.electric_leakage_alarm_enable = atoi((char *)server_frame_struct->para[2].value);
			break;

			case 0x4004:
				msg = (char *)server_frame_struct->para[3].value;
				while(*msg != ',')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				ConcentratorAlarmConfig.electric_leakage_alarm_v_thre = atoi(tmp);
				while(*msg != '\0')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				ConcentratorAlarmConfig.electric_leakage_alarm_c_thre = atoi(tmp);
			break;

			case 0x6005:
				ConcentratorAlarmConfig.low_battery_alarm_enable = atoi((char *)server_frame_struct->para[4].value);
			break;

			case 0x4006:
				ConcentratorAlarmConfig.low_battery_alarm_thre = atoi((char *)server_frame_struct->para[5].value);
			break;

			case 0x6007:
				ConcentratorAlarmConfig.abnormal_charge_alarm_enable = atoi((char *)server_frame_struct->para[6].value);
			break;

			case 0x4008:
				msg = (char *)server_frame_struct->para[7].value;
				while(*msg != ',')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				ConcentratorAlarmConfig.abnormal_charge_alarm_v_thre = atoi(tmp);
				while(*msg != '\0')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				ConcentratorAlarmConfig.abnormal_charge_alarm_c_thre = atoi(tmp);
			break;

			default:
			break;
		}
	}

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	WriteConcentratorAlarmConfig(0,1);

	return ret;
}

//获取告警配置参数
u8 GetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 ret = 0;
	char tmp[10] = {0};
	char buf[25] = {0};

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;
		resp_server_frame_struct->para_num = 8;

		resp_server_frame_struct->para = (Parameter_S *)pvPortMalloc(resp_server_frame_struct->para_num * sizeof(Parameter_S));

		if(resp_server_frame_struct->para != NULL)
		{
			resp_server_frame_struct->para[i].type = 0x6101;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorAlarmConfig.power_off_alarm_enable);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x4102;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorAlarmConfig.power_off_alarm_thre);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x6103;
			memset(buf,0,25);
			myitoa(ConcentratorAlarmConfig.electric_leakage_alarm_enable,(char *)buf,10);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x4104;
			memset(buf,0,25);
			sprintf(tmp, "%d",ConcentratorAlarmConfig.electric_leakage_alarm_v_thre);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",ConcentratorAlarmConfig.electric_leakage_alarm_c_thre);
			strcat(buf,tmp);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x6105;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorAlarmConfig.low_battery_alarm_enable);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x4106;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorAlarmConfig.low_battery_alarm_thre);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x6107;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorAlarmConfig.abnormal_charge_alarm_enable);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x4108;
			memset(buf,0,25);
			sprintf(tmp, "%d",ConcentratorAlarmConfig.abnormal_charge_alarm_v_thre);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",ConcentratorAlarmConfig.abnormal_charge_alarm_c_thre);
			strcat(buf,tmp);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;
		}

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

//设置告警配置参数
u8 SetBasicConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 j = 0;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	for(j = 0; j < server_frame_struct->para_num; j ++)
	{
		switch(server_frame_struct->para[j].type)
		{
			case 0x8001:
				ConcentratorBasicConfig.connection_mode = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x4002:
				memset(ConcentratorBasicConfig.server_ip,0,31);
				memcpy(ConcentratorBasicConfig.server_ip,server_frame_struct->para[j].value,server_frame_struct->para[j].len);
			break;

			case 0x7003:
				memset(ConcentratorBasicConfig.server_port,0,6);
				memcpy(ConcentratorBasicConfig.server_port,server_frame_struct->para[j].value,server_frame_struct->para[j].len);
			break;

			case 0x8004:
				ConcentratorBasicConfig.heartbeat_cycle = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x8005:
				ConcentratorBasicConfig.electric_energy_collection_cycle = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x8006:
				ConcentratorBasicConfig.electric_energy_recording_time = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x8007:
				ConcentratorBasicConfig.loop_state_monitoring_cycle = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x8008:
				ConcentratorBasicConfig.loop_state_recording_time = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x8009:
				ConcentratorBasicConfig.cupboard_alarm_collection_cycle = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x800A:
				ConcentratorBasicConfig.cupboard_alarm_recording_time = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x800B:
				ConcentratorBasicConfig.lamp_state_collection_cycle = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x800C:
				ConcentratorBasicConfig.lamp_state_collection_offset = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x800D:
				ConcentratorBasicConfig.lamp_state_recording_time = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x800E:
				ConcentratorBasicConfig.command_response_timeout = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x800F:
				ConcentratorBasicConfig.command_retransmission_times = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x8010:
				ConcentratorBasicConfig.heartbeat_response_timeout = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x8011:
				ConcentratorBasicConfig.heartbeat_retransmission_times = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x8012:
				ConcentratorBasicConfig.lamp_response_timeout = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x8013:
				ConcentratorBasicConfig.lamp_retransmission_times = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x8014:
				ConcentratorBasicConfig.lamp_broadcast_times = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x8015:
				ConcentratorBasicConfig.lamp_broadcast_interval_time = atoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x4016:
				memset(ConcentratorBasicConfig.operation_password,0,7);
				memcpy(ConcentratorBasicConfig.operation_password,server_frame_struct->para[j].value,server_frame_struct->para[j].len);
			break;

			case 0x4017:
				memset(ConcentratorBasicConfig.manufacturer_name,0,33);
				memcpy(ConcentratorBasicConfig.manufacturer_name,server_frame_struct->para[j].value,server_frame_struct->para[j].len);
			break;

			case 0x4018:
				memset(ConcentratorBasicConfig.manufacturer_website,0,33);
				memcpy(ConcentratorBasicConfig.manufacturer_website,server_frame_struct->para[j].value,server_frame_struct->para[j].len);
			break;

			default:
			break;
		}
	}

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	WriteConcentratorBasicConfig(0,1);

	return ret;
}

//获取告警配置参数
u8 GetBasicConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 ret = 0;
	char buf[25] = {0};

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;
		resp_server_frame_struct->para_num = 24;

		resp_server_frame_struct->para = (Parameter_S *)pvPortMalloc(resp_server_frame_struct->para_num * sizeof(Parameter_S));

		if(resp_server_frame_struct->para != NULL)
		{
			resp_server_frame_struct->para[i].type = 0x8101;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.connection_mode);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x4102;
			resp_server_frame_struct->para[i].len = strlen((char *)ConcentratorBasicConfig.server_ip);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,ConcentratorBasicConfig.server_ip,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x7103;
			resp_server_frame_struct->para[i].len = strlen((char *)ConcentratorBasicConfig.server_port);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,ConcentratorBasicConfig.server_port,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8104;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.heartbeat_cycle);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8105;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.electric_energy_collection_cycle);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8106;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.electric_energy_recording_time);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8107;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.loop_state_monitoring_cycle);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8108;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.loop_state_recording_time);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8109;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.cupboard_alarm_collection_cycle);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x810A;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.cupboard_alarm_recording_time);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x810B;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.lamp_state_collection_cycle);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x810C;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.lamp_state_collection_offset);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x810D;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.lamp_state_recording_time);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x810E;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.command_response_timeout);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x810F;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.command_retransmission_times);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8110;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.heartbeat_response_timeout);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8111;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.heartbeat_retransmission_times);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8112;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.lamp_response_timeout);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8113;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.lamp_retransmission_times);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8114;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.lamp_broadcast_times);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8115;
			memset(buf,0,25);
			sprintf(buf, "%d",ConcentratorBasicConfig.lamp_broadcast_interval_time);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x4116;
			resp_server_frame_struct->para[i].len = strlen((char *)ConcentratorBasicConfig.operation_password);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,ConcentratorBasicConfig.operation_password,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x4117;
			resp_server_frame_struct->para[i].len = strlen((char *)ConcentratorBasicConfig.manufacturer_name);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,ConcentratorBasicConfig.manufacturer_name,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x4118;
			resp_server_frame_struct->para[i].len = strlen((char *)ConcentratorBasicConfig.manufacturer_website);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,ConcentratorBasicConfig.manufacturer_website,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;
		}

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

//设置经纬度年表配置参数
u8 SetLocationConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 month = 0;
	u16 time = 0;
	char tmp[16];
	char *msg = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	for(j = 0; j < resp_server_frame_struct->para_num; j ++)
	{
		if(server_frame_struct->para[j].type == 0x2001)
		{
			ConcentratorLocationConfig.longitude = atof((char *)server_frame_struct->para[0].value);
		}
		else if(server_frame_struct->para[j].type == 0x2002)
		{
			ConcentratorLocationConfig.latitude = atof((char *)server_frame_struct->para[1].value);
		}
		else
		{
			month = server_frame_struct->para[j].type - 0x4003;

			msg = (char *)server_frame_struct->para[j].value;

			for(k = 0; k < 31; k ++)
			{
				msg = (char *)server_frame_struct->para[3].value;
				while(*msg != ',')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				time = atoi(tmp);
				ConcentratorLocationConfig.switch_time_month_table[month].switch_time[k].on_hour = time / 100;
				ConcentratorLocationConfig.switch_time_month_table[month].switch_time[k].on_minute = time % 100;
				while(*msg != '\0')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 2;
				time = atoi(tmp);
				ConcentratorLocationConfig.switch_time_month_table[month].switch_time[k].off_hour = time / 100;
				ConcentratorLocationConfig.switch_time_month_table[month].switch_time[k].off_minute = time % 100;
			}
		}
	}

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	WriteConcentratorLocationConfig(0,1);

	return ret;
}

//获取经纬度年表配置
u8 GetLocationConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 ret = 0;
	u16 time = 0;
	char tmp[10] = {0};
	char buf[310] = {0};

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;
		resp_server_frame_struct->para_num = 14;

		resp_server_frame_struct->para = (Parameter_S *)pvPortMalloc(resp_server_frame_struct->para_num * sizeof(Parameter_S));

		if(resp_server_frame_struct->para != NULL)
		{
			resp_server_frame_struct->para[i].type = 0x2001;
			memset(buf,0,25);
			sprintf(buf, "%f",ConcentratorLocationConfig.longitude);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x2002;
			memset(buf,0,25);
			sprintf(buf, "%f",ConcentratorLocationConfig.latitude);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			for(j = 0; j < 12; j ++)
			{
				resp_server_frame_struct->para[i].type = 0x4003 + j;

				memset(buf,0,310);

				for(k = 0; k < 31; k ++)
				{
					memset(tmp,0,10);
					time = (((u16)ConcentratorLocationConfig.switch_time_month_table[j].switch_time[k].on_hour) * 100) +
					       ((u16)ConcentratorLocationConfig.switch_time_month_table[j].switch_time[k].on_minute);

					Int4BitToString((u8 *)tmp,time);

					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					time = (((u16)ConcentratorLocationConfig.switch_time_month_table[j].switch_time[k].off_hour) * 100) +
					       ((u16)ConcentratorLocationConfig.switch_time_month_table[j].switch_time[k].off_minute);

					Int4BitToString((u8 *)tmp,time);
					strcat(buf,tmp);
					if(k < 30)
					{
						strcat(buf,"|");
					}
				}

				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));

				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
				i ++;
			}
		}

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

//请求固件升级
u8 RequestFrameWareUpDate(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 j = 0;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	for(j = 0; j < server_frame_struct->para_num; j ++)
	{
		switch(server_frame_struct->para[j].type)
		{
			case 0x8001:

			break;

			case 0x4002:
				memset(FrameWareState.file_name,0,31);
				memcpy(FrameWareState.file_name,server_frame_struct->para[j].value,server_frame_struct->para[j].len);
			break;

			case 0x4003:
				memset(FrameWareState.md5,0,33);
				memcpy(FrameWareState.md5,server_frame_struct->para[j].value,server_frame_struct->para[j].len);
			break;

			case 0x9004:
				FrameWareState.total_size = atoi((char *)server_frame_struct->para[j].value);
			break;

			default:
			break;
		}
	}

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	if(FrameWareState.total_size > FIRMWARE_SIZE || 
	   FrameWareState.total_size < FIRMWARE_BAG_SIZE ||
	   strlen((char *)FrameWareState.md5) != 32)
	{
		WriteFrameWareState(1,1);
	}
	else
	{
		FrameWareState.state 			= FIRMWARE_DOWNLOADING;
		FrameWareState.total_bags 		= FrameWareState.total_size % FIRMWARE_BAG_SIZE != 0 ?
										  FrameWareState.total_size / FIRMWARE_BAG_SIZE + 1 : FrameWareState.total_size / FIRMWARE_BAG_SIZE;
		FrameWareState.current_bag_cnt 	= 1;
		FrameWareState.bag_size 		= FIRMWARE_BAG_SIZE;
		FrameWareState.last_bag_size 	= FrameWareState.total_size % FIRMWARE_BAG_SIZE != 0 ?
										  FrameWareState.total_size % FIRMWARE_BAG_SIZE : FIRMWARE_BAG_SIZE;

		FLASH_Unlock();										//解锁
		FLASH_DataCacheCmd(DISABLE);						//FLASH擦除期间,必须禁止数据缓存

		FLASH_EraseSector(FLASH_Sector_9,VoltageRange_3);	//VCC=2.7~3.6V之间!!
		FLASH_EraseSector(FLASH_Sector_10,VoltageRange_3);	//VCC=2.7~3.6V之间!!
		FLASH_EraseSector(FLASH_Sector_11,VoltageRange_3);	//VCC=2.7~3.6V之间!!

		FLASH_DataCacheCmd(ENABLE);							//FLASH擦除结束,开启数据缓存
		FLASH_Lock();										//上锁

		WriteFrameWareState(0,1);
	}

	return ret;
}

//接收固件包
u8 RecvFrameWareBag(ServerFrameStruct_S *server_frame_struct)
{
	u16 ret = 0;
	u8 j = 0;
	u16 i = 0;
	u16 current_bags = 0;
	u16 bag_size = 0;
	u8 msg[FIRMWARE_BAG_SIZE];
	u16 crc_read = 0;
	u16 crc_cal = 0;
	u32 crc32_cal = 0xFFFFFFFF;
	u32 crc32_read = 0;
	u8 crc32_cal_buf[1024];
	u32 file_len = 0;
	u16 k_num = 0;
	u16 last_k_byte_num = 0;
	u16 temp = 0;
	u8 md5[33] = {0};

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	for(j = 0; j < resp_server_frame_struct->para_num; j ++)
	{
		switch(server_frame_struct->para[j].type)
		{
			case 0x4101:
				memcpy(md5,server_frame_struct->para[j].value,server_frame_struct->para[j].len);
			break;

			case 0x8102:
				current_bags = atoi((char *)server_frame_struct->para[j].value);

				if(current_bags > FrameWareState.total_bags)	//包数错误
				{
					return 0;
				}
			break;

			case 0x3103:
				if(search_str(md5, FrameWareState.md5) == -1)		//MD5校验失败
				{
					return 0;
				}

				if(server_frame_struct->para[j].len > FIRMWARE_BAG_SIZE * 2)	//包长度错误
				{
					return 0;
				}

				StrToHex(msg,(char *)server_frame_struct->para[j].value,FIRMWARE_BAG_SIZE);

				crc_read = (((u16)(*(msg + FIRMWARE_BAG_SIZE - 2))) << 8) +
				           (u16)(*(msg + FIRMWARE_BAG_SIZE - 1));

				crc_cal = CRC16(msg,bag_size - 2);

				if(crc_cal == crc_read)
				{
					if(current_bags == FrameWareState.current_bag_cnt)
					{
						if(current_bags < FrameWareState.total_bags)
						{
							FLASH_Unlock();										//解锁
							FLASH_DataCacheCmd(DISABLE);						//写FLASH期间,必须禁止数据缓存

							for(i = 0; i < (FIRMWARE_BAG_SIZE - 2) / 2; i ++)
							{
								temp = ((u16)(*(msg + i * 2 + 1)) << 8) + (u16)(*(msg + i * 2));

								FLASH_ProgramHalfWord(FIRMWARE_BUCKUP_FLASH_BASE_ADD + (current_bags - 1) * (FIRMWARE_BAG_SIZE - 2) + i * 2,temp);
							}

							FLASH_DataCacheCmd(ENABLE);							//写FLASH结束,开启数据缓存
							FLASH_Lock();										//上锁

							FrameWareState.current_bag_cnt ++;

							FrameWareState.state = FIRMWARE_DOWNLOADING;	//当前包下载完成
						}
						else if(current_bags == FrameWareState.total_bags)
						{
							crc32_read = (((u32)(*(msg + 0))) << 24) +
										 (((u32)(*(msg + 1))) << 16) +
										 (((u32)(*(msg + 2))) << 8) +
										 (((u32)(*(msg + 3))));

							file_len = 256 * (FrameWareState.total_bags - 1);

							k_num = file_len / 1024;
							last_k_byte_num = file_len % 1024;
							if(last_k_byte_num > 0)
							{
								k_num += 1;
							}

							for(i = 0; i < k_num; i ++)
							{
								memset(crc32_cal_buf,0,1024);
								if(i < k_num - 1)
								{
									STMFLASH_ReadBytes(FIRMWARE_BUCKUP_FLASH_BASE_ADD + 1024 * i,crc32_cal_buf,1024);
									crc32_cal = CRC32Extend(crc32_cal_buf,1024,crc32_cal,0);
								}
								if(i == k_num - 1)
								{
									if(last_k_byte_num == 0)
									{
										STMFLASH_ReadBytes(FIRMWARE_BUCKUP_FLASH_BASE_ADD + 1024 * i,crc32_cal_buf,1024);
										crc32_cal = CRC32Extend(crc32_cal_buf,1024,crc32_cal,1);
									}
									else if(last_k_byte_num > 0)
									{
										STMFLASH_ReadBytes(FIRMWARE_BUCKUP_FLASH_BASE_ADD + 1024 * i,crc32_cal_buf,last_k_byte_num);
										crc32_cal = CRC32Extend(crc32_cal_buf,last_k_byte_num,crc32_cal,1);
									}
								}
							}

							if(crc32_read == crc32_cal)
							{
								FrameWareState.state = FIRMWARE_DOWNLOADED;
							}
							else
							{
								FrameWareState.state = FIRMWARE_DOWNLOAD_FAILED;
							}

							WriteFrameWareState(0,1);
						}
					}
				}
			break;

			default:
			break;
		}
	}

	return ret;
}

//停止升级
u8 FrameWareUpDateStop(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 md5[33] = {0};

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	if(server_frame_struct->para[0].type == 0x4001)
	{
		memcpy(md5,server_frame_struct->para[0].value,server_frame_struct->para[0].len);

		if(search_str(md5, FrameWareState.md5) != -1)
		{
			WriteFrameWareState(1,1);
		}
	}

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}



































