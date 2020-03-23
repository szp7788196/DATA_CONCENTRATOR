#include "concentrator.h"
#include "common.h"
#include "cat25x.h"
#include "server_protocol.h"
#include "task_handle_server_frame.h"
#include "rx8010s.h"


ConcentratorBasicConfig_S ConcentratorBasicConfig;
u8 SystemReBoot = 0;								//ϵͳ������ʶ





//��ȡ������������������
void ReadConcentratorBasicConfig(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&ConcentratorBasicConfig,CONCENTRATOR_BASIC_CONF_ADD,sizeof(ConcentratorBasicConfig_S));

	crc16_cal = CRC16((u8 *)&ConcentratorBasicConfig,sizeof(ConcentratorBasicConfig_S) - 2);

	if(crc16_cal != ConcentratorBasicConfig.crc16)
	{
		WriteConcentratorBasicConfig(1,0);
	}
}

//�洢������������������
//reset 0����λ 1��λ
//write_enable 0��д��EEPROM 1д��EEPROM
void WriteConcentratorBasicConfig(u8 reset,u8 write_enable)
{
	u8 len = 0;
	
	if(reset == 1)
	{
		ConcentratorBasicConfig.conncetion_mode = (u8)MODE_4G;

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
		len = sizeof(ConcentratorBasicConfig_S);
		
		ConcentratorBasicConfig.crc16 = CRC16((u8 *)&ConcentratorBasicConfig,len - 2);
		
		CAT25X_Write((u8 *)&ConcentratorBasicConfig,CONCENTRATOR_BASIC_CONF_ADD,len);
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
			case 0x0000:	//����͸��
				TransparentTransmission(server_frame_struct);
			break;
			
			case 0x0001:	//Уʱ
				SynchronizeTime(server_frame_struct);
			break;
			
			case 0x0002:	//��λ
				ResetConfigParameters(server_frame_struct);
			break;
			
			case 0x0003:	//����
				RebootTheSystem(server_frame_struct);
			break;
			
			case 0x0004:	//����
				
			break;
			
			case 0x0005:	//���Կ��л�
				
			break;
			
			case 0x0070:	//��־��ѯ
				
			break;
			
			case 0x0071:	//״̬��ѯ
				
			break;
			
			case 0x0072:	//״̬��ʷ��ѯ
				
			break;
			
			case 0x00A0:	//�澯�ϱ�
				
			break;
			
			case 0x00A1:	//�澯���
				
			break;
			
			case 0x00A2:	//�¼��ϱ�
				
			break;
			
			case 0x00A3:	//�澯����
				
			break;
			
			case 0x00A4:	//�澯���ò�ѯ
				
			break;
			
			case 0x00A5:	//�澯��ʷ��ѯ
				
			break;
			
			case 0x00D0:	//��������
				
			break;
			
			case 0x00D1:	//�������ò�ѯ
				
			break;
			
			case 0x00D2:	//��γ���������
				
			break;
			
			case 0x00D3:	//��γ������ѯ
				
			break;
			
			case 0x00E0:	//��¼
				
			break;
			
			case 0x00E1:	//��������
				
			break;
			
			case 0x00E2:	//��Կ����
				
			break;
			
			case 0x00E3:	//����
				
			break;
			
			case 0x00F0:	//��������
				
			break;
			
			case 0x00F1:	//��ȡ������
				
			break;
			
			case 0x00F2:	//�������
				
			break;
			
			case 0x00F3:	//ֹͣ����
				
			break;
			
			default:
			break;
		}

		if(server_frame_struct->para != NULL || server_frame_struct->para_num != 0)
		{
			for(i = 0; i < server_frame_struct->para_num; i ++)		//�ͷ�server_frame_struct����
			{
				vPortFree(server_frame_struct->para[i].value);
				server_frame_struct->para[i].value = NULL;
			}
			
			vPortFree(server_frame_struct->para);
			server_frame_struct->para = NULL;
			
			vPortFree(server_frame_struct);
			server_frame_struct = NULL;
		}
	}
}

//͸��ָ��
u8 TransparentTransmission(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 type = 0;
	DEVICE_TYPE_E device_type = RELAY;
	
	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//������Ӧ������
	ServerFrameStruct_S *tran_server_frame_struct = NULL;		//����͸���������豸
	
	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));
	tran_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));
	
	CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);
	CopyServerFrameStruct(server_frame_struct,tran_server_frame_struct,1);
	
	if(resp_server_frame_struct != NULL)
	{
		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//��Ӧ����������
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;
		
		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}
	
	if(tran_server_frame_struct != NULL)
	{
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
			if(tran_server_frame_struct->para != NULL || tran_server_frame_struct->para_num != 0)
			{
				for(i = 0; i < tran_server_frame_struct->para_num; i ++)		//�ͷ�server_frame_struct����
				{
					vPortFree(tran_server_frame_struct->para[i].value);
					tran_server_frame_struct->para[i].value = NULL;
				}
				
				vPortFree(tran_server_frame_struct->para);
				tran_server_frame_struct->para = NULL;
				
				vPortFree(tran_server_frame_struct);
				tran_server_frame_struct = NULL;
			}
		}
	}

	return ret;
}

//Уʱָ��
u8 SynchronizeTime(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	char buf[5];
	_calendar_obj cal;
	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//������Ӧ������
	
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
	
	CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,1);
	
	if(resp_server_frame_struct != NULL)
	{
		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//��Ӧ����������
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

//�ָ���������
u8 ResetConfigParameters(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 reset_type = 0;
	u8 reboot_type = 0;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//������Ӧ������
	
	reset_type = atoi((char *)server_frame_struct->para[0].value);
	reboot_type = atoi((char *)server_frame_struct->para[1].value);
	
	if(reset_type == 1)
	{
		WriteConcentratorBasicConfig(1,1);
	}
	else if(reset_type == 2)
	{
		WriteConcentratorBasicConfig(1,1);
	}
	
	SystemReBoot = reboot_type;
	
	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));
	
	CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);
	
	if(resp_server_frame_struct != NULL)
	{
		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//��Ӧ����������
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;
		
		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

//����ϵͳ
u8 RebootTheSystem(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//������Ӧ������
	
	SystemReBoot = 1;
	
	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));
	
	CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);
	
	if(resp_server_frame_struct != NULL)
	{
		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//��Ӧ����������
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;
		
		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}















































