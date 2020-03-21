#include "concentrator.h"
#include "common.h"
#include "cat25x.h"
#include "server_protocol.h"
#include "task_handle_server_frame.h"


ConcentratorBasicConfig_S ConcentratorBasicConfig;





//读取集控器基本参数配置
void ReadConcentratorBasicConfig(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&ConcentratorBasicConfig,CONCENTRATOR_BASIC_CONF_ADD,sizeof(ConcentratorBasicConfig_S));

	crc16_cal = CRC16((u8 *)&ConcentratorBasicConfig,sizeof(ConcentratorBasicConfig_S) - 2);

	if(crc16_cal != ConcentratorBasicConfig.crc16)
	{
		ConcentratorBasicConfig.conncetion_mode = (u8)MODE_4G;

		memset(ConcentratorBasicConfig.server_ip,0,31);
		memcpy(ConcentratorBasicConfig.server_ip,"118.178.253.220",15);

		memset(ConcentratorBasicConfig.server_port,0,6);
		memcpy(ConcentratorBasicConfig.server_port,"40901",5);

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
}


void RecvAndHandleFrameStruct(void)
{
	u8 i = 0;
	BaseType_t xResult;
	ServerFrameStruct_S *server_frame_struct = NULL;
	
	xResult = xQueueReceive(xQueue_ConcentratorFrameStruct,(void *)&server_frame_struct,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS )
	{
		switch(server_frame_struct->msg_id)
		{
			case 0x0000:	//数据透传
				
			break;
			
			case 0x0001:	//校时
				
			break;
			
			case 0x0002:	//复位
				
			break;
			
			case 0x0003:	//重启
				
			break;
			
			case 0x0004:	//重连
				
			break;
			
			case 0x0005:	//手自控切换
				
			break;
			
			case 0x0070:	//日志查询
				
			break;
			
			case 0x0071:	//状态查询
				
			break;
			
			case 0x0072:	//状态历史查询
				
			break;
			
			case 0x00A0:	//告警上报
				
			break;
			
			case 0x00A1:	//告警解除
				
			break;
			
			case 0x00A2:	//事件上报
				
			break;
			
			case 0x00A3:	//告警配置
				
			break;
			
			case 0x00A4:	//告警配置查询
				
			break;
			
			case 0x00A5:	//告警历史查询
				
			break;
			
			case 0x00D0:	//基础配置
				
			break;
			
			case 0x00D1:	//基础配置查询
				
			break;
			
			case 0x00D2:	//经纬度年表配置
				
			break;
			
			case 0x00D3:	//经纬度年表查询
				
			break;
			
			case 0x00E0:	//登录
				
			break;
			
			case 0x00E1:	//加密请求
				
			break;
			
			case 0x00E2:	//密钥更新
				
			break;
			
			case 0x00E3:	//心跳
				
			break;
			
			case 0x00F0:	//请求升级
				
			break;
			
			case 0x00F1:	//获取升级包
				
			break;
			
			case 0x00F2:	//升级完成
				
			break;
			
			case 0x00F3:	//停止升级
				
			break;
			
			default:
			break;
		}

		for(i = 0; i < server_frame_struct->para_num; i ++)		//释放server_frame_struct缓存
		{
			vPortFree(server_frame_struct->para[i].value);
			server_frame_struct->para[i].value = NULL;
		}

		vPortFree(server_frame_struct);
		server_frame_struct = NULL;
	}
}

u8 TransparentTransmission(ServerFrameStruct_S *in_server_frame_struct)
{
	u8 ret = 0;
	
	u8 device_type = 0;
	
	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器
	ServerFrameStruct_S *tran_server_frame_struct = NULL;		//用于透传到其他设备
	
	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));
	tran_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));
	
	CopyServerFrameStruct(in_server_frame_struct,resp_server_frame_struct,0);
	CopyServerFrameStruct(in_server_frame_struct,tran_server_frame_struct,1);
	
	resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
	resp_server_frame_struct->msg_len 	= 10;
	resp_server_frame_struct->err_code 	= (u8)NO_ERR;
	
	
	return ret;
}



























