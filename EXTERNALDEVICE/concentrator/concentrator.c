#include "concentrator.h"
#include "common.h"
#include "cat25x.h"
#include "server_protocol.h"
#include "task_handle_server_frame.h"
#include "rx8010s.h"
#include "ec20.h"

//��Ҫ�洢������
RUN_MODE_E RunMode = MODE_AUTO;							//����ģʽ
ConcentratorBasicConfig_S ConcentratorBasicConfig;		//����������Ϣ
ConcentratorAlarmConfig_S ConcentratorAlarmConfig;		//�澯���ò���


//����Ҫ�洢������
u8 FlagSystemReBoot = 0;								//ϵͳ������ʶ
u8 FlagReConnectToServer = 0;							//����������־




//��ȡ����ģʽ
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

//�洢������������������
//reset 0����λ 1��λ
//write_enable 0��д��EEPROM 1д��EEPROM
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

//��ȡ������������������
void ReadConcentratorAlarmConfig(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&ConcentratorAlarmConfig,CONCENTRATOR_ALARM_CONF_ADD,sizeof(ConcentratorAlarmConfig_S));

	crc16_cal = CRC16((u8 *)&ConcentratorAlarmConfig,sizeof(ConcentratorAlarmConfig_S) - 2);

	if(crc16_cal != ConcentratorAlarmConfig.crc16)
	{
		WriteConcentratorAlarmConfig(1,0);
	}
}

//�洢������������������
//reset 0����λ 1��λ
//write_enable 0��д��EEPROM 1д��EEPROM
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


void RecvAndHandleFrameStruct(void)
{
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
				ReConnectToServer(server_frame_struct);
			break;

			case 0x0005:	//���Կ��л�
				SetRunMode(server_frame_struct);
			break;

			case 0x0070:	//��־��ѯ

			break;

			case 0x0071:	//״̬��ѯ
				QueryState(server_frame_struct);
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
				SetAlarmConfiguration(server_frame_struct);
			break;

			case 0x00A4:	//�澯���ò�ѯ
				GetAlarmConfiguration(server_frame_struct);
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

		DeleteServerFrameStruct(server_frame_struct);
	}
}

//͸��ָ��
u8 TransparentTransmission(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 type = 0;
	DEVICE_TYPE_E device_type = RELAY;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//������Ӧ������
	ServerFrameStruct_S *tran_server_frame_struct = NULL;		//����͸���������豸

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));
	tran_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//��Ӧ����������
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

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,1);

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

	FlagSystemReBoot = 1;

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//��Ӧ����������
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

//�������ӷ�����
u8 ReConnectToServer(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//������Ӧ������

	FlagReConnectToServer = 1;

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//��Ӧ����������
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

//��������ģʽ
u8 SetRunMode(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 run_mode = 0;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//������Ӧ������

	run_mode = atoi((char *)server_frame_struct->para[0].value);

	if(run_mode >= (u8)MODE_AUTO && run_mode <= (u8)MODE_STOP)
	{
		RunMode = (RUN_MODE_E)run_mode;
	}

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//��Ӧ����������
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

//״̬��ѯ
u8 QueryState(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 ret = 0;
	char buf[25] = {0};

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//������Ӧ������

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//��Ӧ����������
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

//���ø澯���ò���
u8 SetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	char tmp[10];
	char *msg = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//������Ӧ������

	ConcentratorAlarmConfig.power_off_alarm_enable = atoi((char *)server_frame_struct->para[0].value);
	ConcentratorAlarmConfig.power_off_alarm_thre = atoi((char *)server_frame_struct->para[1].value);
	ConcentratorAlarmConfig.electric_leakage_alarm_enable = atoi((char *)server_frame_struct->para[2].value);

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

	ConcentratorAlarmConfig.low_battery_alarm_enable = atoi((char *)server_frame_struct->para[4].value);
	ConcentratorAlarmConfig.low_battery_alarm_thre = atoi((char *)server_frame_struct->para[5].value);

	ConcentratorAlarmConfig.abnormal_charge_alarm_enable = atoi((char *)server_frame_struct->para[6].value);
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

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//��Ӧ����������
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	WriteConcentratorAlarmConfig(0,1);

	return ret;
}

//��ȡ�澯���ò���
u8 GetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 ret = 0;
	char tmp[10] = {0};
	char buf[25] = {0};

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//������Ӧ������

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//��Ӧ����������
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









































