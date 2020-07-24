#include "lamp_comm.h"
#include "common.h"
#include "concentrator_comm.h"
#include "rx8010s.h"
#include "stmflash.h"


u8 LampStateChangesReportResponse = 0;						//单灯状态变化上报响应标志
u8 LampPlcExecuteTaskReportResponse = 0;					//执行状态上报响应标志


//发送执行状态到服务器
void LampSendExecuteLampPlcExecuteTaskFrameToServer(LampPlcExecuteTask_S *recv_task)
{
	u8 i = 0;
	char tmp[10] = {0};
	char buf[20] = {0};

	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

	server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(server_frame_struct != NULL && recv_task != NULL)
	{
		InitServerFrameStruct(server_frame_struct);

		server_frame_struct->msg_type 	= (u8)DEVICE_REQUEST_UP;	//响应服务器类型
		server_frame_struct->msg_len 	= 10;
		server_frame_struct->err_code 	= (u8)NO_ERR;
		server_frame_struct->msg_id		= recv_task->cmd_code;

		if(recv_task->state == STATE_START)		//开始执行
		{
			server_frame_struct->para_num = 3;

			server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

			if(server_frame_struct->para != NULL)
			{
				server_frame_struct->para[i].type = 0x4001;
				memset(buf,0,10);
				sprintf(buf, "%04x",recv_task->cmd_code);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x8002;
				memset(buf,0,10);
				sprintf(buf, "%d",recv_task->dev_num);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x6003;
				memset(buf,0,10);
				sprintf(buf, "%d",recv_task->execute_type);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x4004;
				memset(buf,0,10);
				memset(tmp,0,10);
				sprintf(tmp, "%d",recv_task->executed_num);
				strcat(buf,tmp);
				strcat(buf,"/");
				memset(tmp,0,10);
				sprintf(tmp, "%d",recv_task->execute_total_num);
				strcat(buf,tmp);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;
			}
		}
		else if(recv_task->state == STATE_FINISHED)		//执行结束
		{
			server_frame_struct->para_num = 3;

			server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

			if(server_frame_struct->para != NULL)
			{
				server_frame_struct->para[i].type = 0x4001;
				memset(buf,0,10);
				sprintf(buf, "%04x",recv_task->cmd_code);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x8002;
				memset(buf,0,10);
				sprintf(buf, "%d",recv_task->success_num);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x8003;
				memset(buf,0,10);
				sprintf(buf, "%d",recv_task->failed_num);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x8004;
				memset(buf,0,10);
				sprintf(buf, "%d",recv_task->timeout_num);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x8005;
				memset(buf,0,10);
				sprintf(buf, "%d",recv_task->spent_time);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;
			}
		}

		ConvertFrameStructToFrame(server_frame_struct);
	}
}

//发送执行状态到服务器
void LampRecvLampStateAndSendToServer(void)
{
	u8 i = 0;
	char tmp[16] = {0};
	char buf[200] = {0};
	BaseType_t xResult;
	LampState_S *state = NULL;

	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

	xResult = xQueueReceive(xQueue_LampState,(void *)&state,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS)
	{
		server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

		if(server_frame_struct != NULL && state != NULL)
		{
			InitServerFrameStruct(server_frame_struct);

			server_frame_struct->msg_type 	= (u8)DEVICE_REQUEST_UP;	//响应服务器类型
			server_frame_struct->msg_len 	= 10;
			server_frame_struct->err_code 	= (u8)NO_ERR;
			server_frame_struct->msg_id		= 0x0170;
			server_frame_struct->para_num 	= 5;

			server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

			if(server_frame_struct->para != NULL)
			{
				server_frame_struct->para[i].type = 0x4001;
				memset(buf,0,10);
				sprintf(buf, "%04x",state->address);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x4002;
				memset(buf,0,200);
				memset(tmp,0,16);
				sprintf(tmp, "%d",state->lamp_paras[0].brightness);
				strcat(buf,tmp);
				strcat(buf,",");

				memset(tmp,0,16);
				sprintf(tmp, "%d",state->lamp_paras[0].light_up_day);
				strcat(buf,tmp);
				strcat(buf,":");

				memset(tmp,0,16);
				sprintf(tmp, "%d",state->lamp_paras[0].light_up_total);
				strcat(buf,tmp);
				strcat(buf,",");

				memset(tmp,0,16);
				sprintf(tmp, "%03f",state->lamp_paras[0].active_energy_day);
				strcat(buf,tmp);
				strcat(buf,":");

				memset(tmp,0,16);
				sprintf(tmp, "%03f",state->lamp_paras[0].active_energy_total);
				strcat(buf,tmp);
				strcat(buf,",");

				memset(tmp,0,16);
				sprintf(tmp, "%03f",state->lamp_paras[0].reactive_energy_day);
				strcat(buf,tmp);
				strcat(buf,":");

				memset(tmp,0,16);
				sprintf(tmp, "%03f",state->lamp_paras[0].reactive_energy_total);
				strcat(buf,tmp);
				strcat(buf,",");

				memset(tmp,0,16);
				sprintf(tmp, "%02f",state->lamp_paras[0].voltage);
				strcat(buf,tmp);
				strcat(buf,",");

				memset(tmp,0,16);
				sprintf(tmp, "%03f",state->lamp_paras[0].current);
				strcat(buf,tmp);
				strcat(buf,",");

				memset(tmp,0,16);
				sprintf(tmp, "%03f",state->lamp_paras[0].active_power);
				strcat(buf,tmp);
				strcat(buf,",");

				memset(tmp,0,16);
				sprintf(tmp, "%02f",state->lamp_paras[0].power_factor);
				strcat(buf,tmp);
				strcat(buf,",");

				memset(tmp,0,16);
				sprintf(tmp, "%02f",state->lamp_paras[0].frequency);
				strcat(buf,tmp);

				if(state->lamp_num == 2)
				{
					strcat(buf,"|");

					memset(tmp,0,16);
					sprintf(tmp, "%d",state->lamp_paras[1].brightness);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,16);
					sprintf(tmp, "%d",state->lamp_paras[1].light_up_day);
					strcat(buf,tmp);
					strcat(buf,":");

					memset(tmp,0,16);
					sprintf(tmp, "%d",state->lamp_paras[1].light_up_total);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,16);
					sprintf(tmp, "%03f",state->lamp_paras[1].active_energy_day);
					strcat(buf,tmp);
					strcat(buf,":");

					memset(tmp,0,16);
					sprintf(tmp, "%03f",state->lamp_paras[1].active_energy_total);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,16);
					sprintf(tmp, "%03f",state->lamp_paras[1].reactive_energy_day);
					strcat(buf,tmp);
					strcat(buf,":");

					memset(tmp,0,16);
					sprintf(tmp, "%03f",state->lamp_paras[1].reactive_energy_total);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,16);
					sprintf(tmp, "%02f",state->lamp_paras[1].voltage);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,16);
					sprintf(tmp, "%03f",state->lamp_paras[1].current);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,16);
					sprintf(tmp, "%03f",state->lamp_paras[1].active_power);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,16);
					sprintf(tmp, "%02f",state->lamp_paras[1].power_factor);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,16);
					sprintf(tmp, "%02f",state->lamp_paras[1].frequency);
					strcat(buf,tmp);
				}
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x4003;
				memset(buf,0,30);
				memset(tmp,0,16);
				sprintf(tmp, "%d",(u8)state->run_mode);
				strcat(buf,tmp);
				strcat(buf,",");

				memset(tmp,0,16);
				sprintf(tmp, "%d",state->controller);
				strcat(buf,tmp);
				strcat(buf,",");

				strcat(buf,(char *)state->control_time);
				strcat(buf,",");

				memset(tmp,0,16);
				sprintf(tmp, "%d",state->temperature);
				strcat(buf,tmp);
				strcat(buf,",");

				memset(tmp,0,16);
				sprintf(tmp, "%d",state->tilt_angle);
				strcat(buf,tmp);
				server_frame_struct->para[i].len = strlen(buf);
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x4004;
				memset(buf,0,1);
				server_frame_struct->para[i].len = 0;
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0x4005;
				memset(buf,0,1);
				server_frame_struct->para[i].len = 0;
				server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(server_frame_struct->para[i].value != NULL)
				{
					memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
				}
				i ++;

				server_frame_struct->para[i].type = 0xA006;
				memset(buf,0,20);
				sprintf(buf, "%s",state->dev_time);
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

		vPortFree(state);
		state = NULL;
	}
}

//向服务器发送状态变化事件
void LampSendExecuteLampPlcExecuteTaskToServer(void)
{
	BaseType_t xResult;
	static LampPlcExecuteTask_S *recv_task = NULL;
	static time_t time_5 = 0;
	static u8 retry_times5 = 0;

	if(LampPlcExecuteTaskReportResponse == 0)
	{
		if(recv_task != NULL)
		{
			DeleteLampPlcExecuteTask(recv_task);
			recv_task = NULL;
		}

		if(recv_task == NULL)
		{
			xResult = xQueueReceive(xQueue_LampPlcExecuteTaskFromPlc,(void *)&recv_task,(TickType_t)pdMS_TO_TICKS(1));

			if(xResult == pdPASS)
			{
				LampPlcExecuteTaskReportResponse = 1;

				time_5 = GetSysTick1s();
				retry_times5 = 0;

				RE_SEND_PLC_EXECUTETASK_REPORT:
				LampSendExecuteLampPlcExecuteTaskFrameToServer(recv_task);
			}
		}
	}

	if(LampPlcExecuteTaskReportResponse == 1)
	{
		if(GetSysTick1s() - time_5 >= ConcentratorBasicConfig.command_response_timeout)
		{
			time_5 = GetSysTick1s();

			if((retry_times5 ++) < ConcentratorBasicConfig.command_retransmission_times)
			{
				goto RE_SEND_PLC_EXECUTETASK_REPORT;
			}
			else
			{
				retry_times5 = 0;

				LampPlcExecuteTaskReportResponse = 0;

				DeleteLampPlcExecuteTask(recv_task);
				recv_task = NULL;
			}
		}
	}
}

//发送OTA请求包
void LampSendOtaRequestFrameToServer(FrameWareState_S frame_ware_state)
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
		server_frame_struct->msg_id		= 0x01F1;
		server_frame_struct->para_num	= 3;

		server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

		if(server_frame_struct->para != NULL)
		{
			server_frame_struct->para[i].type = 0x4001;
			server_frame_struct->para[i].len = strlen((char *)LampFrameWareState.md5);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,LampFrameWareState.md5,server_frame_struct->para[i].len + 1);
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
void LampSendOtaCompleteFrameToServer(void)
{
	u8 i = 0;
	u8 tmp = 0;
	char buf[32];
	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

	server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(server_frame_struct != NULL)
	{
		InitServerFrameStruct(server_frame_struct);

		server_frame_struct->msg_type 	= (u8)DEVICE_REQUEST_UP;	//响应服务器类型
		server_frame_struct->msg_len 	= 10;
		server_frame_struct->err_code 	= (u8)NO_ERR;
		server_frame_struct->msg_id		= 0x01F2;
		server_frame_struct->para_num	= 3;

		server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

		if(server_frame_struct->para != NULL)
		{
			server_frame_struct->para[i].type = 0x4001;
			server_frame_struct->para[i].len = strlen((char *)LampFrameWareState.md5);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,LampFrameWareState.md5,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x6002;
			if(LampFrameWareState.state == FIRMWARE_DOWNLOADED)
			{
				tmp = 1;
			}
			else if(LampFrameWareState.state == FIRMWARE_DOWNLOAD_FAILED)
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
			memset(buf,0,32);
			sprintf(buf, "%s",LampFrameWareState.file_name);
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

//向服务器发送OTA完成通知
void LampSendOtaCompleteNoticeToServer(void)
{
	static time_t time_3 = 0;
	static u8 retry_times3 = 0;

	if(LampFrameWareState.state == FIRMWARE_DOWNLOADED ||
	   LampFrameWareState.state == FIRMWARE_DOWNLOAD_FAILED)
	{
		if(GetSysTick1s() - time_3 >= ConcentratorBasicConfig.command_response_timeout)
		{
			time_3 = GetSysTick1s();

			if((retry_times3 ++) < ConcentratorBasicConfig.command_retransmission_times)
			{
				LampSendOtaCompleteFrameToServer();
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
}

void LampSendOtaRequestToServer(void)
{
	static time_t time_4 = 0;
	static u8 retry_times4 = 0;

	if(LampFrameWareState.state == FIRMWARE_DOWNLOADING)
	{
		time_4 = GetSysTick1s();
		retry_times4 = 0;

		RE_SEND_OTA_REQUEST:
		LampSendOtaRequestFrameToServer(LampFrameWareState);

		LampFrameWareState.state = FIRMWARE_DOWNLOAD_WAIT;
	}

	if(LampFrameWareState.state == FIRMWARE_DOWNLOAD_WAIT)
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

				LampFrameWareState.state = FIRMWARE_DOWNLOAD_FAILED;
			}
		}
	}
	else
	{
		retry_times4 = 0;
	}
}

//处理接收到的报文
void LampRecvAndHandleFrameStruct(void)
{
	BaseType_t xResult;
	ServerFrameStruct_S *server_frame_struct = NULL;

	xResult = xQueueReceive(xQueue_LampFrameStruct,(void *)&server_frame_struct,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS )
	{
		switch(server_frame_struct->msg_id)
		{
			case 0x0100:	//数据透传

			break;

			case 0x0101:	//校时
				LampSynchronizeTime(server_frame_struct);
			break;

			case 0x0102:	//复位
				LampResetConfigParameters(server_frame_struct);
			break;

			case 0x0103:	//灯具调光
				LampAdjustBrightness(server_frame_struct);
			break;

			case 0x0104:	//灯具闪测
				LampFlashTest(server_frame_struct);
			break;

			case 0x0105:	//模式切换
				LampSetStrategyGroupSwitch(server_frame_struct);
			break;

			case 0x0106:	//手自控切换
				LampSetRunMode(server_frame_struct);
			break;

			case 0x0170:	//状态查询
				LampGetCurrentState(server_frame_struct);
			break;

			case 0x0171:	//状态上报
				LampStateChangesReportResponse = 0;
			break;

			case 0x0172:	//状态历史查询

			break;

			case 0x0173:	//上报开始执行
				LampPlcExecuteTaskReportResponse = 0;
				LampSetExecuteState(server_frame_struct);
			break;

			case 0x0174:	//上报执行完成
				LampPlcExecuteTaskReportResponse = 0;
			break;

			case 0x01A0:	//告警上报
				AlarmReportResponse = 0;
			break;

			case 0x01A1:	//告警解除
				AlarmReportResponse = 0;
			break;

			case 0x01A2:	//事件上报

			break;

			case 0x01A3:	//告警配置
				LampSetAlarmConfiguration(server_frame_struct);
			break;

			case 0x01A4:	//告警配置查询
				LampGetAlarmConfiguration(server_frame_struct);
			break;

			case 0x01A5:	//告警历史查询

			break;

			case 0x01D0:	//基础配置
				LampSetBasicConfiguration(server_frame_struct);
			break;

			case 0x01D1:	//查询基础配置
				LampGetBasicConfiguration(server_frame_struct);
			break;

			case 0x01D2:	//重新写址
				LampReSetDeviceAddress(server_frame_struct);
			break;

			case 0x01D3:	//场景配置
				LampSetLampAppointment(server_frame_struct);
			break;

			case 0x01D4:	//查询场景配置
				LampGetLampAppointment(server_frame_struct);
			break;

			case 0x01D5:	//任务配置

			break;

			case 0x01D6:	//查询任务配置

			break;

			case 0x01D7:	//配置同步

			break;

			case 0x01D8:	//节点搜索
				LampNodeSearch(server_frame_struct);
			break;

			case 0x01F0:	//请求下载固件包
				LampRequestFrameWareUpDate(server_frame_struct);
			break;

			case 0x01F1:	//下载固件包
				LampRecvFrameWareBag(server_frame_struct);
			break;

			case 0x01F2:	//完成下载
				WriteLampFrameWareState(2,1);
			break;

			case 0x01F3:	//开始升级
				LampStartFirmWareUpdate(server_frame_struct);
			break;

			case 0x01F4:	//暂停升级
				LampSuspendFirmWareUpdate(server_frame_struct);
			break;

			case 0x01F5:	//停止升级
				LampStopFirmWareUpdate(server_frame_struct);
			break;

			case 0x01F6:	//版本查询
				LampGetFirmWareVersion(server_frame_struct);
			break;

			default:
			break;
		}

		DeleteServerFrameStruct(server_frame_struct);
	}
}

void LampGetLampPlcExecuteTaskInfo(LampPlcExecuteTask_S *task)
{
	u8 m = 0;
	
	for(task->group_num = 0; task->group_num < MAX_LAMP_GROUP_NUM; task->group_num ++)
	{
		if(task->group_dev_id[task->group_num] == 0)
		{
			break;
		}
	}
	
	switch(task->broadcast_type)
	{
		case 0:
			task->dev_num = LampNumList.number;
			
			if(task->execute_type == 0)
			{
				task->execute_total_num = 1;
			}
			else
			{
				task->execute_total_num = task->dev_num;
			}
		break;

		case 1:
			for(m = 0; m < task->group_num; m ++)
			{
				task->dev_num += LampGroupListNum.list[task->group_dev_id[m]];
			}
			
			if(task->execute_type == 0)
			{
				task->execute_total_num = 1;
			}
			else
			{
				task->execute_total_num = task->dev_num;
			}
		break;

		case 2:
			task->dev_num = task->group_num;
			task->execute_total_num = task->group_num;
		break;

		default:

		break;
	}
}

u8 LampSynchronizeTime(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 tmp_h = 0;
	u8 tmp_l = 0;
	u16 add = 0;
	u8 *data = NULL;
	char tmp[5];
	char *msg = NULL;

	LampPlcExecuteTask_S *task = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	task = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));

	if(task != NULL)
	{
		memset(task,0,sizeof(LampPlcExecuteTask_S));

		for(j = 0; j < server_frame_struct->para_num; j ++)
		{
			switch(server_frame_struct->para[j].type)
			{
				case 0x8001:
					task->broadcast_type = myatoi((char *)server_frame_struct->para[j].value);
				break;

				case 0x4002:
					msg = (char *)server_frame_struct->para[j].value;

					if(task->broadcast_type == 2)
					{
						while(*msg != '\0')
						{
							while(*msg != ',')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							msg = msg + 1;
							if(i == 1 || i == 2)
							{
								if(i == 1)
								{
									tmp[1] = tmp[0];
									tmp[0] = '0';
								}

								StrToHex((u8 *)&add,tmp,1);
							}
							else if(i == 3 || i == 4)
							{
								if(i == 3)
								{
									tmp[3] = tmp[2];
									tmp[2] = tmp[1];
									tmp[1] = tmp[0];
									tmp[0] = '0';
								}

								StrToHex(&tmp_h,tmp + 0,1);
								StrToHex(&tmp_l,tmp + 2,1);

								add = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
							}

							task->group_dev_id[k ++] = add;

							i = 0;

							if(*msg == '\0')
							{
								break;
							}
						}
					}
					else
					{
						while(*msg != '\0')
						{
							while(*msg != ',' && *msg != '\0')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							task->group_dev_id[k ++] = myatoi(tmp);

							if(*msg == '\0')
							{
								break;
							}
						}
					}
				break;

				case 0xA003:
					if(server_frame_struct->para[j].len == 6)
					{
						msg = (char *)server_frame_struct->para[j].value;

						data = (u8 *)pvPortMalloc(3 * sizeof(u8));

						if(data != NULL)
						{
							*(data + 0) = (*(msg + 0) - 0x30) * 10 + (*(msg + 1) - 0x30);
							*(data + 1) = (*(msg + 2) - 0x30) * 10 + (*(msg + 3) - 0x30);
							*(data + 2) = (*(msg + 4) - 0x30) * 10 + (*(msg + 5) - 0x30);
						}
					}
				break;

				case 0x8004:
					task->execute_type = myatoi((char *)server_frame_struct->para[j].value);
				break;

				default:
				break;
			}
		}

		task->data = data;
		task->data_len = 3;
		task->notify_enable = 1;
		task->cmd_code = 0x0101;

		LampGetLampPlcExecuteTaskInfo(task);

		if(xQueueSend(xQueue_LampPlcExecuteTaskToPlc,(void *)&task,(TickType_t)10) != pdPASS)
		{
#ifdef DEBUG_LOG
			printf("send xQueue_LampPlcFrame fail.\r\n");
#endif
			DeleteLampPlcExecuteTask(task);
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

u8 LampResetConfigParameters(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 tmp_h = 0;
	u8 tmp_l = 0;
	u16 add = 0;
	u8 *data = NULL;
	char tmp[5];
	char *msg = NULL;

	LampPlcExecuteTask_S *task = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	task = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));

	if(task != NULL)
	{
		memset(task,0,sizeof(LampPlcExecuteTask_S));

		data = (u8 *)pvPortMalloc(1 * sizeof(u8));

		if(data != NULL)
		{
			for(j = 0; j < server_frame_struct->para_num; j ++)
			{
				switch(server_frame_struct->para[j].type)
				{
					case 0x8001:
						task->broadcast_type = myatoi((char *)server_frame_struct->para[j].value);
					break;

					case 0x4002:
						msg = (char *)server_frame_struct->para[j].value;

						if(task->broadcast_type == 2)
						{
							while(*msg != '\0')
							{
								while(*msg != ',')
								tmp[i ++] = *(msg ++);
								tmp[i] = '\0';
								msg = msg + 1;
								if(i == 1 || i == 2)
								{
									if(i == 1)
									{
										tmp[1] = tmp[0];
										tmp[0] = '0';
									}

									StrToHex((u8 *)&add,tmp,1);
								}
								else if(i == 3 || i == 4)
								{
									if(i == 3)
									{
										tmp[3] = tmp[2];
										tmp[2] = tmp[1];
										tmp[1] = tmp[0];
										tmp[0] = '0';
									}

									StrToHex(&tmp_h,tmp + 0,1);
									StrToHex(&tmp_l,tmp + 2,1);

									add = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
								}

								task->group_dev_id[k ++] = add;

								i = 0;

								if(*msg == '\0')
								{
									break;
								}
							}
						}
						else
						{
							while(*msg != '\0')
							{
								while(*msg != ',' && *msg != '\0')
								tmp[i ++] = *(msg ++);
								tmp[i] = '\0';
								i = 0;
								msg = msg + 1;
								task->group_dev_id[k ++] = myatoi(tmp);

								if(*msg == '\0')
								{
									break;
								}
							}
						}
					break;

					case 0x8003:
						*(data + 0) = myatoi((char *)server_frame_struct->para[j].value);
					break;

					case 0x8004:
						task->execute_type = myatoi((char *)server_frame_struct->para[j].value);
					break;

					default:
					break;
				}
			}

			if(*(data + 0) == 7)
			{
				WriteLampNumList(1,1);
				
				goto GET_OUT;
			}
			else
			{
				task->data = data;
				task->data_len = 1;
				task->notify_enable = 1;
				task->cmd_code = 0x0102;

				LampGetLampPlcExecuteTaskInfo(task);

				if(xQueueSend(xQueue_LampPlcExecuteTaskToPlc,(void *)&task,(TickType_t)10) != pdPASS)
				{
	#ifdef DEBUG_LOG
					printf("send xQueue_LampPlcFrame fail.\r\n");
	#endif
					DeleteLampPlcExecuteTask(task);
				}
			}
		}
		else
		{
			GET_OUT:
			DeleteLampPlcExecuteTask(task);
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

u8 LampAdjustBrightness(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 n = 0;
	u8 ch = 0;
	u8 tmp_h = 0;
	u8 tmp_l = 0;
	u16 add = 0;
	u8 *data = NULL;
	char tmp[5];
	char *msg = NULL;

	LampPlcExecuteTask_S *task = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	task = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));

	if(task != NULL)
	{
		memset(task,0,sizeof(LampPlcExecuteTask_S));

		data = (u8 *)pvPortMalloc(3 * sizeof(u8));

		if(data != NULL)
		{
			for(j = 0; j < server_frame_struct->para_num; j ++)
			{
				switch(server_frame_struct->para[j].type)
				{
					case 0x8001:
						task->broadcast_type = myatoi((char *)server_frame_struct->para[j].value);
					break;

					case 0x4002:
						msg = (char *)server_frame_struct->para[j].value;

						if(task->broadcast_type == 2)
						{
							while(*msg != '\0')
							{
								while(*msg != ',')
								tmp[i ++] = *(msg ++);
								tmp[i] = '\0';
								msg = msg + 1;
								if(i == 1 || i == 2)
								{
									if(i == 1)
									{
										tmp[1] = tmp[0];
										tmp[0] = '0';
									}

									StrToHex((u8 *)&add,tmp,1);
								}
								else if(i == 3 || i == 4)
								{
									if(i == 3)
									{
										tmp[3] = tmp[2];
										tmp[2] = tmp[1];
										tmp[1] = tmp[0];
										tmp[0] = '0';
									}

									StrToHex(&tmp_h,tmp + 0,1);
									StrToHex(&tmp_l,tmp + 2,1);

									add = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
								}

								task->group_dev_id[k ++] = add;

								i = 0;

								if(*msg == '\0')
								{
									break;
								}
							}
						}
						else
						{
							while(*msg != '\0')
							{
								while(*msg != ',' && *msg != '\0')
								tmp[i ++] = *(msg ++);
								tmp[i] = '\0';
								i = 0;
								msg = msg + 1;
								task->group_dev_id[k ++] = myatoi(tmp);

								if(*msg == '\0')
								{
									break;
								}
							}
						}
					break;

					case 0x3003:
						msg = (char *)server_frame_struct->para[j].value;

						while(*msg != '\0')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						msg = msg + 1;
						if(i == 1 || i == 2)
						{
							if(i == 1)
							{
								tmp[1] = tmp[0];
								tmp[0] = '0';
							}

							StrToHex((u8 *)&ch,tmp,1);
						}
						i = 0;

						*(data + 0) = ch;
					break;

					case 0x4004:
						msg = (char *)server_frame_struct->para[j].value;

						n = 1;

						while(*msg != '\0')
						{
							while(*msg != ',' && *msg != '\0')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							*(data + (n ++)) = myatoi(tmp);

							if(*msg == '\0')
							{
								break;
							}
						}
					break;

					case 0x8005:
						task->execute_type = myatoi((char *)server_frame_struct->para[j].value);
					break;

					default:
					break;
				}
			}

			task->data = data;
			task->data_len = 3;
			task->notify_enable = 1;
			task->cmd_code = 0x0103;

			LampGetLampPlcExecuteTaskInfo(task);

			if(xQueueSend(xQueue_LampPlcExecuteTaskToPlc,(void *)&task,(TickType_t)10) != pdPASS)
			{
#ifdef DEBUG_LOG
				printf("send xQueue_LampPlcFrame fail.\r\n");
#endif
				DeleteLampPlcExecuteTask(task);
			}
		}
		else
		{
			DeleteLampPlcExecuteTask(task);
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

u8 LampFlashTest(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 n = 0;
	u8 ch = 0;
	u8 tmp_h = 0;
	u8 tmp_l = 0;
	u16 add = 0;
	u8 *data = NULL;
	char tmp[5];
	char *msg = NULL;

	LampPlcExecuteTask_S *task = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	task = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));

	if(task != NULL)
	{
		memset(task,0,sizeof(LampPlcExecuteTask_S));

		data = (u8 *)pvPortMalloc(3 * sizeof(u8));

		if(data != NULL)
		{
			for(j = 0; j < server_frame_struct->para_num; j ++)
			{
				switch(server_frame_struct->para[j].type)
				{
					case 0x8001:
						task->broadcast_type = myatoi((char *)server_frame_struct->para[j].value);
					break;

					case 0x4002:
						msg = (char *)server_frame_struct->para[j].value;

						if(task->broadcast_type == 2)
						{
							while(*msg != '\0')
							{
								while(*msg != ',')
								tmp[i ++] = *(msg ++);
								tmp[i] = '\0';
								msg = msg + 1;
								if(i == 1 || i == 2)
								{
									if(i == 1)
									{
										tmp[1] = tmp[0];
										tmp[0] = '0';
									}

									StrToHex((u8 *)&add,tmp,1);
								}
								else if(i == 3 || i == 4)
								{
									if(i == 3)
									{
										tmp[3] = tmp[2];
										tmp[2] = tmp[1];
										tmp[1] = tmp[0];
										tmp[0] = '0';
									}

									StrToHex(&tmp_h,tmp + 0,1);
									StrToHex(&tmp_l,tmp + 2,1);

									add = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
								}

								task->group_dev_id[k ++] = add;

								i = 0;

								if(*msg == '\0')
								{
									break;
								}
							}
						}
						else
						{
							while(*msg != '\0')
							{
								while(*msg != ',' && *msg != '\0')
								tmp[i ++] = *(msg ++);
								tmp[i] = '\0';
								i = 0;
								msg = msg + 1;
								task->group_dev_id[k ++] = myatoi(tmp);

								if(*msg == '\0')
								{
									break;
								}
							}
						}
					break;

					case 0x3003:
						msg = (char *)server_frame_struct->para[j].value;

						while(*msg != '\0')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						msg = msg + 1;
						if(i == 1 || i == 2)
						{
							if(i == 1)
							{
								tmp[1] = tmp[0];
								tmp[0] = '0';
							}

							StrToHex((u8 *)&ch,tmp,1);
						}
						i = 0;

						*(data + 0) = ch;
					break;

					case 0x4004:
						msg = (char *)server_frame_struct->para[j].value;

						n = 1;

						while(*msg != '\0')
						{
							while(*msg != ',' && *msg != '\0')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							*(data + (n ++)) = myatoi(tmp);

							if(*msg == '\0')
							{
								break;
							}
						}
					break;

					case 0x8005:
						task->execute_type = myatoi((char *)server_frame_struct->para[j].value);
					break;

					default:
					break;
				}
			}

			task->data = data;
			task->data_len = 3;
			task->notify_enable = 1;
			task->cmd_code = 0x0104;

			LampGetLampPlcExecuteTaskInfo(task);

			if(xQueueSend(xQueue_LampPlcExecuteTaskToPlc,(void *)&task,(TickType_t)10) != pdPASS)
			{
#ifdef DEBUG_LOG
				printf("send xQueue_LampPlcFrame fail.\r\n");
#endif
				DeleteLampPlcExecuteTask(task);
			}
		}
		else
		{
			DeleteLampPlcExecuteTask(task);
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

u8 LampSetStrategyGroupSwitch(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 tmp_h = 0;
	u8 tmp_l = 0;
	u16 add = 0;
	LampStrategyGroupSwitch_S *data = NULL;
	char tmp[5];
	char *msg = NULL;

	LampPlcExecuteTask_S *task = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	task = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));

	if(task != NULL)
	{
		memset(task,0,sizeof(LampPlcExecuteTask_S));

		data = (LampStrategyGroupSwitch_S *)pvPortMalloc(sizeof(LampStrategyGroupSwitch_S));

		if(data != NULL)
		{
			memset(data,0,sizeof(LampStrategyGroupSwitch_S));

			for(j = 0; j < server_frame_struct->para_num; j ++)
			{
				switch(server_frame_struct->para[j].type)
				{
					case 0x8001:
						task->broadcast_type = myatoi((char *)server_frame_struct->para[j].value);
					break;

					case 0x4002:
						msg = (char *)server_frame_struct->para[j].value;

						if(task->broadcast_type == 2)
						{
							while(*msg != '\0')
							{
								while(*msg != ',')
								tmp[i ++] = *(msg ++);
								tmp[i] = '\0';
								msg = msg + 1;
								if(i == 1 || i == 2)
								{
									if(i == 1)
									{
										tmp[1] = tmp[0];
										tmp[0] = '0';
									}

									StrToHex((u8 *)&add,tmp,1);
								}
								else if(i == 3 || i == 4)
								{
									if(i == 3)
									{
										tmp[3] = tmp[2];
										tmp[2] = tmp[1];
										tmp[1] = tmp[0];
										tmp[0] = '0';
									}

									StrToHex(&tmp_h,tmp + 0,1);
									StrToHex(&tmp_l,tmp + 2,1);

									add = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
								}

								task->group_dev_id[k ++] = add;

								i = 0;

								if(*msg == '\0')
								{
									break;
								}
							}
						}
						else
						{
							while(*msg != '\0')
							{
								while(*msg != ',' && *msg != '\0')
								tmp[i ++] = *(msg ++);
								tmp[i] = '\0';
								i = 0;
								msg = msg + 1;
								task->group_dev_id[k ++] = myatoi(tmp);

								if(*msg == '\0')
								{
									break;
								}
							}
						}
					break;

					case 0x4003:
						msg = (char *)server_frame_struct->para[j].value;

						while(*msg != '\0')
						{
							while(*msg != ',' && *msg != '\0')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							data->group_id[data->group_num ++] = myatoi(tmp);

							if(*msg == '\0')
							{
								break;
							}
						}
					break;

					case 0x8004:
						data->type = myatoi((char *)server_frame_struct->para[j].value);
					break;

					case 0xA005:
						if(server_frame_struct->para[j].len == 14)
						{
							memcpy(data->time,server_frame_struct->para[j].value,14);
						}
					break;

					case 0x8006:
						task->execute_type = myatoi((char *)server_frame_struct->para[j].value);
					break;

					default:
					break;
				}
			}

			task->data = data;

			task->notify_enable = 1;
			task->cmd_code = 0x0105;

			LampGetLampPlcExecuteTaskInfo(task);

			if(xQueueSend(xQueue_LampPlcExecuteTaskToPlc,(void *)&task,(TickType_t)10) != pdPASS)
			{
#ifdef DEBUG_LOG
				printf("send xQueue_LampPlcFrame fail.\r\n");
#endif
				DeleteLampPlcExecuteTask(task);
			}
		}
		else
		{
			DeleteLampPlcExecuteTask(task);
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

u8 LampSetRunMode(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 tmp_h = 0;
	u8 tmp_l = 0;
	u16 add = 0;
	u8 *data = NULL;
	char tmp[5];
	char *msg = NULL;

	LampPlcExecuteTask_S *task = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	task = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));

	if(task != NULL)
	{
		memset(task,0,sizeof(LampPlcExecuteTask_S));

		data = (u8 *)pvPortMalloc(1 * sizeof(u8));

		if(data != NULL)
		{
			for(j = 0; j < server_frame_struct->para_num; j ++)
			{
				switch(server_frame_struct->para[j].type)
				{
					case 0x8001:
						task->broadcast_type = myatoi((char *)server_frame_struct->para[j].value);
					break;

					case 0x4002:
						msg = (char *)server_frame_struct->para[j].value;

						if(task->broadcast_type == 2)
						{
							while(*msg != '\0')
							{
								while(*msg != ',')
								tmp[i ++] = *(msg ++);
								tmp[i] = '\0';
								msg = msg + 1;
								if(i == 1 || i == 2)
								{
									if(i == 1)
									{
										tmp[1] = tmp[0];
										tmp[0] = '0';
									}

									StrToHex((u8 *)&add,tmp,1);
								}
								else if(i == 3 || i == 4)
								{
									if(i == 3)
									{
										tmp[3] = tmp[2];
										tmp[2] = tmp[1];
										tmp[1] = tmp[0];
										tmp[0] = '0';
									}

									StrToHex(&tmp_h,tmp + 0,1);
									StrToHex(&tmp_l,tmp + 2,1);

									add = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
								}

								task->group_dev_id[k ++] = add;

								i = 0;

								if(*msg == '\0')
								{
									break;
								}
							}
						}
						else
						{
							while(*msg != '\0')
							{
								while(*msg != ',' && *msg != '\0')
								tmp[i ++] = *(msg ++);
								tmp[i] = '\0';
								i = 0;
								msg = msg + 1;
								task->group_dev_id[k ++] = myatoi(tmp);

								if(*msg == '\0')
								{
									break;
								}
							}
						}
					break;

					case 0x8003:
						*(data + 0) = myatoi((char *)server_frame_struct->para[j].value);
					break;

					case 0x8004:
						task->execute_type = myatoi((char *)server_frame_struct->para[j].value);
					break;

					default:
					break;
				}
			}

			task->data = data;
			task->data_len = 1;
			task->notify_enable = 1;
			task->cmd_code = 0x0106;

			LampGetLampPlcExecuteTaskInfo(task);

			if(xQueueSend(xQueue_LampPlcExecuteTaskToPlc,(void *)&task,(TickType_t)10) != pdPASS)
			{
#ifdef DEBUG_LOG
				printf("send xQueue_LampPlcFrame fail.\r\n");
#endif
				DeleteLampPlcExecuteTask(task);
			}
		}
		else
		{
			DeleteLampPlcExecuteTask(task);
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

//基本配置
u8 LampSetExecuteState(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 j = 0;
	LampPlcExecuteTaskState_S state;

	for(j = 0; j < server_frame_struct->para_num; j ++)
	{
		switch(server_frame_struct->para[j].type)
		{
			case 0x4001:
				state.cmd_code = myatoi((char *)server_frame_struct->para[j].value);
			break;

			case 0x8002:
				state.state = myatoi((char *)server_frame_struct->para[j].value);
			break;
		}
	}

	if(state.state == 1)
	{
		if(xQueueSend(xQueue_LampPlcExecuteTaskState,(void *)&state,(TickType_t)10) != pdPASS)
		{
#ifdef DEBUG_LOG
			printf("send xQueue_LampPlcExecuteTaskState fail 1.\r\n");
#endif
		}
	}

	return ret;
}

u8 LampGetCurrentState(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 tmp_h = 0;
	u8 tmp_l = 0;
	u16 add = 0;
	char tmp[5];
	char *msg = NULL;

	LampPlcExecuteTask_S *task = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	task = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));

	if(task != NULL)
	{
		memset(task,0,sizeof(LampPlcExecuteTask_S));

		for(j = 0; j < server_frame_struct->para_num; j ++)
		{
			switch(server_frame_struct->para[j].type)
			{
				case 0x8001:
					task->broadcast_type = myatoi((char *)server_frame_struct->para[j].value);
				break;

				case 0x4002:
					msg = (char *)server_frame_struct->para[j].value;

					if(task->broadcast_type == 2)
					{
						while(*msg != '\0')
						{
							while(*msg != ',')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							msg = msg + 1;
							if(i == 1 || i == 2)
							{
								if(i == 1)
								{
									tmp[1] = tmp[0];
									tmp[0] = '0';
								}

								StrToHex((u8 *)&add,tmp,1);
							}
							else if(i == 3 || i == 4)
							{
								if(i == 3)
								{
									tmp[3] = tmp[2];
									tmp[2] = tmp[1];
									tmp[1] = tmp[0];
									tmp[0] = '0';
								}

								StrToHex(&tmp_h,tmp + 0,1);
								StrToHex(&tmp_l,tmp + 2,1);

								add = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
							}

							task->group_dev_id[k ++] = add;

							i = 0;

							if(*msg == '\0')
							{
								break;
							}
						}
					}
					else
					{
						while(*msg != '\0')
						{
							while(*msg != ',' && *msg != '\0')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							task->group_dev_id[k ++] = myatoi(tmp);

							if(*msg == '\0')
							{
								break;
							}
						}
					}
				break;

				default:
				break;
			}
		}
		
		task->notify_enable = 1;
		task->cmd_code = 0x0170;
		task->execute_type = 2;

		LampGetLampPlcExecuteTaskInfo(task);

		if(xQueueSend(xQueue_LampPlcExecuteTaskToPlc,(void *)&task,(TickType_t)10) != pdPASS)
		{
#ifdef DEBUG_LOG
			printf("send xQueue_LampPlcFrame fail.\r\n");
#endif
			DeleteLampPlcExecuteTask(task);
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

//设置告警配置参数
u8 LampSetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 n = 0;
	u8 tmp_h = 0;
	u8 tmp_l = 0;
	u16 add = 0;
	u8 para_id = 0;
	u8 loop_ch = 0;
	u8 seg_num = 0;
	char tmp[10];
	char *msg = NULL;
	LampAlarm_S *lamp_alarm = NULL;
	LampPlcExecuteTask_S *task = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器
	
	lamp_alarm = (LampAlarm_S *)pvPortMalloc(sizeof(LampAlarm_S));
	task = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));
	
	if(lamp_alarm != NULL && task != NULL)
	{
		for(j = 0; j < server_frame_struct->para_num; j ++)
		{
			switch(server_frame_struct->para[j].type)
			{
				case 0x8001:
					task->broadcast_type = myatoi((char *)server_frame_struct->para[j].value);
				break;

				case 0x4002:
					msg = (char *)server_frame_struct->para[j].value;

					if(task->broadcast_type == 2)
					{
						while(*msg != '\0')
						{
							while(*msg != ',')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							msg = msg + 1;
							if(i == 1 || i == 2)
							{
								if(i == 1)
								{
									tmp[1] = tmp[0];
									tmp[0] = '0';
								}

								StrToHex((u8 *)&add,tmp,1);
							}
							else if(i == 3 || i == 4)
							{
								if(i == 3)
								{
									tmp[3] = tmp[2];
									tmp[2] = tmp[1];
									tmp[1] = tmp[0];
									tmp[0] = '0';
								}

								StrToHex(&tmp_h,tmp + 0,1);
								StrToHex(&tmp_l,tmp + 2,1);

								add = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
							}

							task->group_dev_id[k ++] = add;

							i = 0;

							if(*msg == '\0')
							{
								break;
							}
						}
					}
					else
					{
						while(*msg != '\0')
						{
							while(*msg != ',' && *msg != '\0')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							task->group_dev_id[k ++] = myatoi(tmp);

							if(*msg == '\0')
							{
								break;
							}
						}
					}
				break;
				
				case 0x8003:
					LampNodeLossAlarmConfig.enable = myatoi((char *)server_frame_struct->para[i].value);
					
					WriteLampNodeLossAlarmConfig(0,1);
				break;

				case 0x8004:
					lamp_alarm->lamp_fault_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
				break;
				
				case 0x8005:
					lamp_alarm->power_module_fault_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
				break;
				
				case 0x8006:
					lamp_alarm->capacitor_fault_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
				break;
				
				case 0x8007:
					lamp_alarm->relay_fault_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
				break;
				
				case 0x8008:
					lamp_alarm->temperature_alarm_duration = myatoi((char *)server_frame_struct->para[i].value);
				break;
				
				case 0x4009:
					msg = (char *)server_frame_struct->para[j].value;
				
					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					lamp_alarm->temperature_alarm_low_thre = myatoi(tmp);

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					lamp_alarm->temperature_alarm_high_thre = myatoi(tmp);

					while(*msg != '\0')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					lamp_alarm->temperature_alarm_duration = atof(tmp);
				break;
					
				case 0x800A:
					lamp_alarm->leakage_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
				break;
				
				case 0x400B:
					msg = (char *)server_frame_struct->para[j].value;
				
					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					lamp_alarm->leakage_alarm_c_thre = myatoi(tmp);

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					lamp_alarm->leakage_alarm_v_thre = myatoi(tmp);

					while(*msg != '\0')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					lamp_alarm->leakage_alarm_duration = atof(tmp);
				break;
					
				case 0x800C:
					lamp_alarm->gate_magnetism_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
				break;
				
				case 0x800D:
					lamp_alarm->gate_magnetism_alarm_type = myatoi((char *)server_frame_struct->para[i].value);
				break;
				
				case 0x800E:
					lamp_alarm->post_tilt_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
				break;
				
				case 0x400F:
					msg = (char *)server_frame_struct->para[j].value;

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					lamp_alarm->post_tilt_alarm_thre = myatoi(tmp);

					while(*msg != '\0')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					lamp_alarm->post_tilt_alarm_duration = atof(tmp);
				break;
					
				case 0x8010:
					lamp_alarm->electrical_para_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
				break;
				
				case 0x4011:
					msg = (char *)server_frame_struct->para[j].value;
				
					seg_num = 0;

					while(*msg != '\0')
					{
						if(*(msg ++) == '|')
						{
							seg_num ++;
						}
					}

					seg_num ++;

					msg = (char *)server_frame_struct->para[j].value;
					
					for(n = 0; n < seg_num; n ++)
					{
						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						loop_ch = myatoi(tmp);
						
						if(loop_ch <= MAX_LAMP_CH_NUM)
						{
							if(loop_ch == 0)
							{
								loop_ch = 1;
							}
							
							while(*msg != ',')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							para_id = myatoi(tmp);
							
							if(para_id <= MAX_LAMP_ALARM_E_PARA_NUM)
							{
								if(para_id == 0)
								{
									para_id = 1;
								}
								
								lamp_alarm->electrical_para_alarm_thre[loop_ch - 1][para_id - 1].channel = loop_ch;
								lamp_alarm->electrical_para_alarm_thre[loop_ch - 1][para_id - 1].para_id = para_id;
								
								while(*msg != ',')
								tmp[i ++] = *(msg ++);
								tmp[i] = '\0';
								i = 0;
								msg = msg + 1;
								lamp_alarm->electrical_para_alarm_thre[loop_ch - 1][para_id - 1].min_value = myatoi(tmp);
								
								while(*msg != ',')
								tmp[i ++] = *(msg ++);
								tmp[i] = '\0';
								i = 0;
								msg = msg + 1;
								lamp_alarm->electrical_para_alarm_thre[loop_ch - 1][para_id - 1].min_range = myatoi(tmp);
								
								while(*msg != ',')
								tmp[i ++] = *(msg ++);
								tmp[i] = '\0';
								i = 0;
								msg = msg + 1;
								lamp_alarm->electrical_para_alarm_thre[loop_ch - 1][para_id - 1].max_value = myatoi(tmp);
								
								while(*msg != ',')
								tmp[i ++] = *(msg ++);
								tmp[i] = '\0';
								i = 0;
								msg = msg + 1;
								lamp_alarm->electrical_para_alarm_thre[loop_ch - 1][para_id - 1].max_range = myatoi(tmp);
								
								while(*msg != '|' && *msg != '\0')
								tmp[i ++] = *(msg ++);
								tmp[i] = '\0';
								i = 0;
								msg = msg + 1;
								lamp_alarm->electrical_para_alarm_thre[loop_ch - 1][para_id - 1].duration_time = myatoi(tmp);
							}
						}
					}
				break;
					
				case 0x8012:
					lamp_alarm->abnormal_light_on_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
				break;
				
				case 0x4013:
					msg = (char *)server_frame_struct->para[j].value;
				
					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					lamp_alarm->abnormal_light_on_alarm_p_thre = myatoi(tmp);

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					lamp_alarm->abnormal_light_on_alarm_c_thre = myatoi(tmp);

					while(*msg != '\0')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					lamp_alarm->abnormal_light_on_alarm_duration = atof(tmp);
				break;
					
				case 0x8014:
					lamp_alarm->abnormal_light_off_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
				break;
				
				case 0x4015:
					msg = (char *)server_frame_struct->para[j].value;
				
					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					lamp_alarm->abnormal_light_off_alarm_p_thre = myatoi(tmp);

					while(*msg != ',')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					lamp_alarm->abnormal_light_off_alarm_c_thre = myatoi(tmp);

					while(*msg != '\0')
					tmp[i ++] = *(msg ++);
					tmp[i] = '\0';
					i = 0;
					msg = msg + 1;
					lamp_alarm->abnormal_light_off_alarm_duration = atof(tmp);
				break;
					
				case 0x8016:
					lamp_alarm->light_on_fault_alarm_enable = myatoi((char *)server_frame_struct->para[i].value);
				break;
				
				case 0x4017:
					msg = (char *)server_frame_struct->para[j].value;
				
					seg_num = 0;

					while(*msg != '\0')
					{
						if(*(msg ++) == '|')
						{
							seg_num ++;
						}
					}

					seg_num ++;

					msg = (char *)server_frame_struct->para[j].value;
					
					for(n = 0; n < seg_num; n ++)
					{
						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						lamp_alarm->light_on_fault_alarm_rated_power[n] = myatoi(tmp);
						
						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						lamp_alarm->light_on_fault_alarm_low_thre[n] = myatoi(tmp);
						
						while(*msg != ',')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						lamp_alarm->light_on_fault_alarm_high_thre[n] = myatoi(tmp);
						
						while(*msg != '|' && *msg != '\0')
						tmp[i ++] = *(msg ++);
						tmp[i] = '\0';
						i = 0;
						msg = msg + 1;
						lamp_alarm->light_on_fault_alarm_duration[n] = myatoi(tmp);
					}
				break;
					
				case 0x8018:
					lamp_alarm->task_light_state_fault_alarm_enhable = myatoi((char *)server_frame_struct->para[i].value);
				break;
				
				case 0x8019:
					task->execute_type = myatoi((char *)server_frame_struct->para[j].value);
				break;

				default:
				break;
			}
		}
		
		task->data = lamp_alarm;
		task->data_len = sizeof(LampAlarm_S);
		task->notify_enable = 1;
		task->cmd_code = 0x01A3;

		LampGetLampPlcExecuteTaskInfo(task);

		if(xQueueSend(xQueue_LampPlcExecuteTaskToPlc,(void *)&task,(TickType_t)10) != pdPASS)
		{
#ifdef DEBUG_LOG
			printf("send xQueue_LampPlcFrame fail.\r\n");
#endif
			DeleteLampPlcExecuteTask(task);
		}
	}
	else
	{
		if(lamp_alarm != NULL)
		{
			vPortFree(lamp_alarm);
		}
		
		if(task != NULL)
		{
			DeleteLampPlcExecuteTask(task);
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

u8 LampGetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 tmp_h = 0;
	u8 tmp_l = 0;
	u16 add = 0;
	char tmp[5];
	char *msg = NULL;

	LampPlcExecuteTask_S *task = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	task = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));

	if(task != NULL)
	{
		memset(task,0,sizeof(LampPlcExecuteTask_S));

		for(j = 0; j < server_frame_struct->para_num; j ++)
		{
			switch(server_frame_struct->para[j].type)
			{
				case 0x8001:
					task->broadcast_type = myatoi((char *)server_frame_struct->para[j].value);
				break;

				case 0x4002:
					msg = (char *)server_frame_struct->para[j].value;

					if(task->broadcast_type == 2)
					{
						while(*msg != '\0')
						{
							while(*msg != ',')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							msg = msg + 1;
							if(i == 1 || i == 2)
							{
								if(i == 1)
								{
									tmp[1] = tmp[0];
									tmp[0] = '0';
								}

								StrToHex((u8 *)&add,tmp,1);
							}
							else if(i == 3 || i == 4)
							{
								if(i == 3)
								{
									tmp[3] = tmp[2];
									tmp[2] = tmp[1];
									tmp[1] = tmp[0];
									tmp[0] = '0';
								}

								StrToHex(&tmp_h,tmp + 0,1);
								StrToHex(&tmp_l,tmp + 2,1);

								add = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
							}

							task->group_dev_id[k ++] = add;

							i = 0;

							if(*msg == '\0')
							{
								break;
							}
						}
					}
					else
					{
						while(*msg != '\0')
						{
							while(*msg != ',' && *msg != '\0')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							task->group_dev_id[k ++] = myatoi(tmp);

							if(*msg == '\0')
							{
								break;
							}
						}
					}
				break;

				default:
				break;
			}
		}
		
		task->notify_enable = 1;
		task->cmd_code = 0x01A4;
		task->execute_type = 2;

		LampGetLampPlcExecuteTaskInfo(task);

		if(xQueueSend(xQueue_LampPlcExecuteTaskToPlc,(void *)&task,(TickType_t)10) != pdPASS)
		{
#ifdef DEBUG_LOG
			printf("send xQueue_LampPlcFrame fail.\r\n");
#endif
			DeleteLampPlcExecuteTask(task);
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

//基本配置
u8 LampSetBasicConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u16 lamp_num = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 tmp_h = 0;
	u8 tmp_l = 0;
	u16 add = 0;
	u8 brightness = 0;
	u8 group_id = 0;
	char tmp[17];
	char *msg = NULL;
	LampConfig_S lamp_conf;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	for(j = 0; j < server_frame_struct->para_num; j ++)
	{
		if(server_frame_struct->para[j].type == 0x7001)
		{
			LampBasicConfig.auto_sync_time_cycle = myatoi((char *)server_frame_struct->para[j].value);
		}
		else if(server_frame_struct->para[j].type == 0x8002)
		{
			LampBasicConfig.auto_report_plc_state = myatoi((char *)server_frame_struct->para[j].value);
		}
		else if(server_frame_struct->para[j].type == 0x8003)
		{
			LampBasicConfig.state_collection_cycle = myatoi((char *)server_frame_struct->para[j].value);
		}
		else if(server_frame_struct->para[j].type == 0x8004)
		{
			LampBasicConfig.state_collection_offset = myatoi((char *)server_frame_struct->para[j].value);
		}
		else if(server_frame_struct->para[j].type == 0x8005)
		{
			LampBasicConfig.state_recording_time = myatoi((char *)server_frame_struct->para[j].value);
		}
		else if(server_frame_struct->para[j].type == 0x8006)
		{
			LampBasicConfig.response_timeout = myatoi((char *)server_frame_struct->para[j].value);
		}
		else if(server_frame_struct->para[j].type == 0x8007)
		{
			LampBasicConfig.retransmission_times = myatoi((char *)server_frame_struct->para[j].value);
		}
		else if(server_frame_struct->para[j].type == 0x8008)
		{
			LampBasicConfig.broadcast_times = myatoi((char *)server_frame_struct->para[j].value);
		}
		else if(server_frame_struct->para[j].type == 0x8009)
		{
			LampBasicConfig.broadcast_interval_time = myatoi((char *)server_frame_struct->para[j].value);

			WriteLampBasicConfig(0,1);
		}
		else
		{
			memset(&lamp_conf,0,sizeof(LampConfig_S));

			lamp_num = server_frame_struct->para[j].type - 0x400A;

			msg = (char *)server_frame_struct->para[j].value;

			while(*msg != ',')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			msg = msg + 1;
			if(i == 1 || i == 2)
			{
				if(i == 1)
				{
					tmp[1] = tmp[0];
					tmp[0] = '0';
				}

				StrToHex((u8 *)&add,tmp,1);
			}
			else if(i == 3 || i == 4)
			{
				if(i == 3)
				{
					tmp[3] = tmp[2];
					tmp[2] = tmp[1];
					tmp[1] = tmp[0];
					tmp[0] = '0';
				}

				StrToHex(&tmp_h,tmp + 0,1);
				StrToHex(&tmp_l,tmp + 2,1);

				add = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
			}
			lamp_conf.address = add;
			i = 0;

			while(*msg != ',')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			lamp_conf.advance_time = myatoi(tmp);

			while(*msg != ',')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			lamp_conf.delay_time = myatoi(tmp);

			while(*msg != ',')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			lamp_conf.longitude = atof(tmp);

			while(*msg != ',')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			lamp_conf.latitude = atof(tmp);

			while(*msg != ',')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			lamp_conf.light_wane = myatoi(tmp);

			while(*msg != ',')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			lamp_conf.auto_report = myatoi(tmp);

			while(*msg != ',')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			lamp_conf.adjust_type = myatoi(tmp);

			while(*msg != '|')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			lamp_conf.node_loss_check_times = myatoi(tmp);

			while(*msg != ',')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			brightness = 0;
			brightness = myatoi(tmp);
			lamp_conf.default_brightness = (brightness << 4) & 0xF0;

			while(*msg != '|')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			brightness = 0;
			brightness = myatoi(tmp);
			lamp_conf.default_brightness += brightness;

			while(*msg != '\0')
			{
				while(*msg != ',' && *msg != '\0')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				group_id = myatoi(tmp);
				lamp_conf.group[k ++] = group_id;

				LampGroupListNum.list[group_id - 1] ++;

				if(*msg == '\0')
				{
					break;
				}
			}

			if(lamp_num < MAX_LAMP_CONF_NUM)
			{
				WriteLampConfig(lamp_num,0,lamp_conf);

				WriteSpecifyLampNumList(lamp_num,1);
			}
		}
	}

	WriteLampNumList(0,1);
	WriteLampGroupListNum(0,1);

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

u8 LampGetBasicConfiguration(ServerFrameStruct_S *server_frame_struct)
{
	u16 i = 0;
	u16 j = 0;
	u8 k = 0;
	u8 m = 0;
	u8 n = 0;
	u8 ret = 0;
	char tmp[16] = {0};
	char buf[80] = {0};
	LampConfig_S conf;
	LampConfig_S lamp_conf[10];
	u16 index[10] = {0};

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;
		resp_server_frame_struct->para_num 	= 2;

		resp_server_frame_struct->para = (Parameter_S *)pvPortMalloc(resp_server_frame_struct->para_num * sizeof(Parameter_S));

		if(resp_server_frame_struct->para != NULL)
		{
			resp_server_frame_struct->para[i].type = 0x7101;
			memset(buf,0,25);
			sprintf(buf, "%d",LampBasicConfig.auto_sync_time_cycle);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8102;
			memset(buf,0,25);
			sprintf(buf, "%d",LampBasicConfig.auto_report_plc_state);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8103;
			memset(buf,0,25);
			sprintf(buf, "%d",LampBasicConfig.state_collection_cycle);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8104;
			memset(buf,0,25);
			sprintf(buf, "%d",LampBasicConfig.state_collection_offset);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8105;
			memset(buf,0,25);
			sprintf(buf, "%d",LampBasicConfig.state_recording_time);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8106;
			memset(buf,0,25);
			sprintf(buf, "%d",LampBasicConfig.response_timeout);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8107;
			memset(buf,0,25);
			sprintf(buf, "%d",LampBasicConfig.retransmission_times);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8108;
			memset(buf,0,25);
			sprintf(buf, "%d",LampBasicConfig.broadcast_times);
			resp_server_frame_struct->para[i].len = strlen(buf);
			resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(resp_server_frame_struct->para[i].value != NULL)
			{
				memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
			}
			i ++;

			resp_server_frame_struct->para[i].type = 0x8109;
			memset(buf,0,25);
			sprintf(buf, "%d",LampBasicConfig.broadcast_interval_time);
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

	for(j = 0; j < MAX_LAMP_CONF_NUM; j ++)
	{
		ret = ReadLampConfig(j,&conf);

		if(ret == 1)
		{
			memcpy(&lamp_conf[k],&conf,sizeof(LampConfig_S));

			index[k] = j;

			k ++;
		}

		if(k == 10 || (k >= 1 && j == MAX_LAMP_CONF_NUM -1))
		{
			ServerFrameStruct_S *resp_server_frame_struct1 = NULL;		//用于响应服务器

			resp_server_frame_struct1 = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

			if(resp_server_frame_struct1 != NULL)
			{
				CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct1,0);

				resp_server_frame_struct1->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
				resp_server_frame_struct1->msg_len 		= 10;
				resp_server_frame_struct1->err_code 	= (u8)NO_ERR;
				resp_server_frame_struct1->para_num 	= k;

				resp_server_frame_struct1->para = (Parameter_S *)pvPortMalloc(resp_server_frame_struct1->para_num * sizeof(Parameter_S));

				if(resp_server_frame_struct1->para != NULL)
				{
					i = 0;

					for(m = 0; m < k; m ++)
					{
						resp_server_frame_struct->para[i].type = 0x4103 + index[m];
						memset(buf,0,80);
						sprintf(buf, "%d",lamp_conf[m].address);

						memset(tmp,0,16);
						sprintf(tmp, "%d",lamp_conf[m].advance_time);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,16);
						sprintf(tmp, "%d",lamp_conf[m].delay_time);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,16);
						sprintf(tmp, "%f",lamp_conf[m].longitude);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,16);
						sprintf(tmp, "%f",lamp_conf[m].latitude);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,16);
						sprintf(tmp, "%d",lamp_conf[m].light_wane);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,16);
						sprintf(tmp, "%d",lamp_conf[m].auto_report);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,16);
						sprintf(tmp, "%d",lamp_conf[m].adjust_type);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,16);
						sprintf(tmp, "%d",lamp_conf[m].node_loss_check_times);
						strcat(buf,tmp);
						strcat(buf,"|");

						memset(tmp,0,16);
						sprintf(tmp, "%d",((lamp_conf[m].default_brightness) >> 4) & 0x0F);
						strcat(buf,tmp);
						strcat(buf,",");

						memset(tmp,0,16);
						sprintf(tmp, "%d",lamp_conf[m].default_brightness & 0x0F);
						strcat(buf,tmp);
						strcat(buf,"|");

						for(n = 0; n < MAX_LAMP_GROUP_NUM; n ++)
						{
							if(lamp_conf[m].group[n] != 0)
							{
								memset(tmp,0,16);
								sprintf(tmp, "%d",lamp_conf[m].group[n]);
								strcat(buf,tmp);
								strcat(buf,",");
							}
						}

						resp_server_frame_struct->para[i].len = strlen(buf);
						buf[resp_server_frame_struct->para[i].len - 1] = '\0';
						resp_server_frame_struct->para[i].len -= 1;
						resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
						if(resp_server_frame_struct->para[i].value != NULL)
						{
							memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
						}
						i ++;
					}
				}

				ret = ConvertFrameStructToFrame(resp_server_frame_struct1);
			}

			k = 0;
		}
	}

	return ret;
}

u8 LampReSetDeviceAddress(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 tmp_h = 0;
	u8 tmp_l = 0;
	u16 add = 0;
	u16 *data = NULL;
	char tmp[5];
	char *msg = NULL;

	LampPlcExecuteTask_S *task = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	task = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));

	if(task != NULL)
	{
		memset(task,0,sizeof(LampPlcExecuteTask_S));

		data = (u16 *)pvPortMalloc(2 * sizeof(u16));

		if(data != NULL)
		{
			for(j = 0; j < server_frame_struct->para_num; j ++)
			{
				switch(server_frame_struct->para[j].type)
				{
					case 0x3001:
						msg = (char *)server_frame_struct->para[j].value;

						while(*msg != '\0')
						{
							while(*msg != ',')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							msg = msg + 1;
							if(i == 1 || i == 2)
							{
								if(i == 1)
								{
									tmp[1] = tmp[0];
									tmp[0] = '0';
								}

								StrToHex((u8 *)&add,tmp,1);
							}
							else if(i == 3 || i == 4)
							{
								if(i == 3)
								{
									tmp[3] = tmp[2];
									tmp[2] = tmp[1];
									tmp[1] = tmp[0];
									tmp[0] = '0';
								}

								StrToHex(&tmp_h,tmp + 0,1);
								StrToHex(&tmp_l,tmp + 2,1);

								add = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
							}

							*(data + 0) = add;

							i = 0;

							if(*msg == '\0')
							{
								break;
							}
						}
					break;

					case 0x3002:
						msg = (char *)server_frame_struct->para[j].value;

						while(*msg != '\0')
						{
							while(*msg != ',')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							msg = msg + 1;
							if(i == 1 || i == 2)
							{
								if(i == 1)
								{
									tmp[1] = tmp[0];
									tmp[0] = '0';
								}

								StrToHex((u8 *)&add,tmp,1);
							}
							else if(i == 3 || i == 4)
							{
								if(i == 3)
								{
									tmp[3] = tmp[2];
									tmp[2] = tmp[1];
									tmp[1] = tmp[0];
									tmp[0] = '0';
								}

								StrToHex(&tmp_h,tmp + 0,1);
								StrToHex(&tmp_l,tmp + 2,1);

								add = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
							}

							*(data + 1) = add;

							i = 0;

							if(*msg == '\0')
							{
								break;
							}
						}
					break;

					default:
					break;
				}
			}

			task->data = data;

			task->notify_enable = 1;
			task->cmd_code = 0x01D2;
			task->execute_type = 1;
			task->broadcast_type = 2;
			task->dev_num = 1;
			task->execute_total_num = 1;
			

			if(xQueueSend(xQueue_LampPlcExecuteTaskToPlc,(void *)&task,(TickType_t)10) != pdPASS)
			{
#ifdef DEBUG_LOG
				printf("send xQueue_LampPlcFrame fail.\r\n");
#endif
				DeleteLampPlcExecuteTask(task);
			}
		}
		else
		{
			DeleteLampPlcExecuteTask(task);
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

u8 LampSetLampAppointment(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u16 pos = 0;
	u8 para_num = 0;
	u32 time = 0;
	u8 week = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 seg_num = 0;
	char tmp[16];
	char *msg = NULL;
	LampSenceConfig_S lamp_sence_config;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	para_num = server_frame_struct->para_num;

	if(para_num > MAX_LAMP_APPOINTMENT_NUM)
	{
		para_num = MAX_LAMP_APPOINTMENT_NUM;
	}

	LampAppointmentNum.number = para_num;

	WriteLampAppointmentNum(0,1);

	for(j = 0; j < para_num; j ++)
	{
		memset(&lamp_sence_config,0,sizeof(LampSenceConfig_S));

		msg = (char *)server_frame_struct->para[j].value;

		if(server_frame_struct->para[j].len != 0)
		{
			while(*msg != ',')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			lamp_sence_config.group_id = myatoi(tmp);

			while(*msg != '|')
			tmp[i ++] = *(msg ++);
			tmp[i] = '\0';
			i = 0;
			msg = msg + 1;
			lamp_sence_config.priority = myatoi(tmp);

			seg_num = 0;
			pos = 0;

			while(*msg != '\0')
			{
				if(*(msg ++) == '|')
				{
					seg_num ++;
				}

				pos ++;
			}

			seg_num ++;

			if(seg_num > MAX_LAMP_APPOINTMENT_TIME_RANGE_NUM)
			{
				seg_num = MAX_LAMP_APPOINTMENT_TIME_RANGE_NUM;
			}

			lamp_sence_config.time_range_num = seg_num;

			msg -= pos;

			for(k = 0; k < seg_num; k ++)
			{
				while(*msg != ',')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				time = myatoi(tmp);
				lamp_sence_config.range[k].s_month = time / 1000000;
				lamp_sence_config.range[k].s_date = (time / 10000) % 100;
				lamp_sence_config.range[k].s_hour = (time / 100) % 100;
				lamp_sence_config.range[k].s_minute = time % 100;

				while(*msg != ',')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				i = 0;
				msg = msg + 1;
				time = myatoi(tmp);
				lamp_sence_config.range[k].e_month = time / 1000000;
				lamp_sence_config.range[k].e_date = (time / 10000) % 100;
				lamp_sence_config.range[k].e_hour = (time / 100) % 100;
				lamp_sence_config.range[k].e_minute = time % 100;

				while(*msg != ',' && *msg != '|' && *msg != '\0')
				tmp[i ++] = *(msg ++);
				tmp[i] = '\0';
				msg = msg + 1;
				if(i == 1)
				{
					tmp[1] = tmp[0];
					tmp[0] = '0';
				}
				StrToHex(&week,tmp,1);
				i = 0;
				lamp_sence_config.range[k].week_enable = week;
			}

			WriteLampAppointment(j,&lamp_sence_config,0,1);
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

u8 LampGetLampAppointment(ServerFrameStruct_S *server_frame_struct)
{
	u8 i = 0;
	u8 j = 0;
	u8 ret = 0;
	char tmp[10] = {0};
	char buf[220];
	LampSenceConfig_S appointment;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;
		resp_server_frame_struct->para_num 	= LampAppointmentNum.number;

		resp_server_frame_struct->para = (Parameter_S *)pvPortMalloc(resp_server_frame_struct->para_num * sizeof(Parameter_S));

		if(resp_server_frame_struct->para != NULL)
		{
			for(i = 0; i < resp_server_frame_struct->para_num; i ++)
			{
				memset(buf,0,220);
				memset(&appointment,0,sizeof(LampSenceConfig_S));

				ret = ReadLampAppointment(i,&appointment);

				if(ret == 1)
				{
					resp_server_frame_struct->para[i].type = 0x4101 + i;

					memset(tmp,0,10);
					sprintf(tmp, "%d",appointment.group_id);
					strcat(buf,tmp);
					strcat(buf,",");

					memset(tmp,0,10);
					sprintf(tmp, "%d",appointment.priority);
					strcat(buf,tmp);
					strcat(buf,"|");

					for(j = 0; j < appointment.time_range_num; j ++)
					{
						if(appointment.range[j].s_month  == 0 &&
						   appointment.range[j].s_date   == 0 &&
						   appointment.range[j].s_hour   == 0 &&
						   appointment.range[j].s_minute == 0 &&
						   appointment.range[j].e_month  == 0 &&
						   appointment.range[j].e_date   == 0 &&
						   appointment.range[j].e_hour   == 0 &&
						   appointment.range[j].e_minute == 0)
						{
							strcat(buf,"0,0");
						}
						else
						{
							memset(tmp,0,10);
							sprintf(tmp, "%02d",appointment.range[j].s_month);
							strcat(buf,tmp);
							memset(tmp,0,10);
							sprintf(tmp, "%02d",appointment.range[j].s_date);
							strcat(buf,tmp);
							memset(tmp,0,10);
							sprintf(tmp, "%02d",appointment.range[j].s_hour);
							strcat(buf,tmp);
							memset(tmp,0,10);
							sprintf(tmp, "%02d",appointment.range[j].s_minute);
							strcat(buf,tmp);
							strcat(buf,",");
							memset(tmp,0,10);
							sprintf(tmp, "%02d",appointment.range[j].e_month);
							strcat(buf,tmp);
							memset(tmp,0,10);
							sprintf(tmp, "%02d",appointment.range[j].e_date);
							strcat(buf,tmp);
							memset(tmp,0,10);
							sprintf(tmp, "%02d",appointment.range[j].e_hour);
							strcat(buf,tmp);
							memset(tmp,0,10);
							sprintf(tmp, "%02d",appointment.range[j].e_minute);
							strcat(buf,tmp);
						}
						strcat(buf,",");
						memset(tmp,0,10);
						sprintf(tmp, "%02x",appointment.range[j].week_enable);
						strcat(buf,tmp);
						if(j < appointment.time_range_num - 1)
						{
							strcat(buf,"|");
						}
					}
				}

				resp_server_frame_struct->para[i].len = strlen(buf);
				resp_server_frame_struct->para[i].value = (u8 *)pvPortMalloc((resp_server_frame_struct->para[i].len + 1) * sizeof(u8));
				if(resp_server_frame_struct->para[i].value != NULL)
				{
					memcpy(resp_server_frame_struct->para[i].value,buf,resp_server_frame_struct->para[i].len + 1);
				}
			}
		}

		ret = ConvertFrameStructToFrame(resp_server_frame_struct);
	}

	return ret;
}

u8 LampNodeSearch(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;

	LampPlcExecuteTask_S *task = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	task = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));

	if(task != NULL)
	{
		memset(task,0,sizeof(LampPlcExecuteTask_S));

		task->broadcast_type = 0;
		task->notify_enable = 1;
		task->cmd_code = 0x01A4;
		task->execute_type = 2;
		task->dev_num = 0;
		task->execute_total_num = 1;

		if(xQueueSend(xQueue_LampPlcExecuteTaskToPlc,(void *)&task,(TickType_t)10) != pdPASS)
		{
#ifdef DEBUG_LOG
			printf("send xQueue_LampPlcFrame fail.\r\n");
#endif
			DeleteLampPlcExecuteTask(task);
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

//请求固件下载
u8 LampRequestFrameWareUpDate(ServerFrameStruct_S *server_frame_struct)
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
				memset(LampFrameWareState.md5,0,33);
				memcpy(LampFrameWareState.md5,server_frame_struct->para[j].value,server_frame_struct->para[j].len);
			break;

			case 0x9003:
				LampFrameWareState.total_size = myatoi((char *)server_frame_struct->para[j].value);
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

	if(LampFrameWareState.total_size > LAMP_FIRMWARE_SIZE ||
	   LampFrameWareState.total_size < LAMP_FIRMWARE_BAG_SIZE ||
	   strlen((char *)LampFrameWareState.md5) != 32)
	{
		WriteLampFrameWareState(1,1);
	}
	else
	{
		LampFrameWareState.state 			= FIRMWARE_DOWNLOADING;
		LampFrameWareState.total_bags 		= LampFrameWareState.total_size % LAMP_FIRMWARE_BAG_SIZE != 0 ?
										      LampFrameWareState.total_size / LAMP_FIRMWARE_BAG_SIZE + 1 : LampFrameWareState.total_size / LAMP_FIRMWARE_BAG_SIZE;
		LampFrameWareState.current_bag_cnt 	= 1;
		LampFrameWareState.bag_size 		= LAMP_FIRMWARE_BAG_SIZE;
		LampFrameWareState.last_bag_size 	= LampFrameWareState.total_size % LAMP_FIRMWARE_BAG_SIZE != 0 ?
										      LampFrameWareState.total_size % LAMP_FIRMWARE_BAG_SIZE : LAMP_FIRMWARE_BAG_SIZE;

		FLASH_Unlock();										//解锁
		FLASH_DataCacheCmd(DISABLE);						//FLASH擦除期间,必须禁止数据缓存

		FLASH_EraseSector(FLASH_Sector_5,VoltageRange_3);	//VCC=2.7~3.6V之间!!

		FLASH_DataCacheCmd(ENABLE);							//FLASH擦除结束,开启数据缓存
		FLASH_Lock();										//上锁

		WriteLampFrameWareState(0,1);
	}

	return ret;
}

//接收固件包
u8 LampRecvFrameWareBag(ServerFrameStruct_S *server_frame_struct)
{
	u16 ret = 0;
	u8 j = 0;
	u16 i = 0;
	u16 current_bags = 0;
	u16 bag_size = 0;
	u8 msg[LAMP_FIRMWARE_BAG_SIZE];
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
				current_bags = myatoi((char *)server_frame_struct->para[j].value);

				if(current_bags > LampFrameWareState.total_bags)	//包数错误
				{
					return 0;
				}
			break;

			case 0x3103:
				if(search_str(md5, LampFrameWareState.md5) == -1)		//MD5校验失败
				{
					return 0;
				}

				if(server_frame_struct->para[j].len > LAMP_FIRMWARE_BAG_SIZE * 2)	//包长度错误
				{
					return 0;
				}

				StrToHex(msg,(char *)server_frame_struct->para[j].value,LAMP_FIRMWARE_BAG_SIZE);

				crc_read = (((u16)(*(msg + LAMP_FIRMWARE_BAG_SIZE - 2))) << 8) +
				           (u16)(*(msg + LAMP_FIRMWARE_BAG_SIZE - 1));

				crc_cal = CRC16(msg,bag_size - 2);

				if(crc_cal == crc_read)
				{
					if(current_bags == LampFrameWareState.current_bag_cnt)
					{
						if(current_bags < LampFrameWareState.total_bags)
						{
							FLASH_Unlock();										//解锁
							FLASH_DataCacheCmd(DISABLE);						//写FLASH期间,必须禁止数据缓存

							for(i = 0; i < (LAMP_FIRMWARE_BAG_SIZE - 2) / 2; i ++)
							{
								temp = ((u16)(*(msg + i * 2 + 1)) << 8) + (u16)(*(msg + i * 2));

								FLASH_ProgramHalfWord(LAMP_FIRMWARE_BUCKUP_FLASH_BASE_ADD + (current_bags - 1) * (LAMP_FIRMWARE_BAG_SIZE - 2) + i * 2,temp);
							}

							FLASH_DataCacheCmd(ENABLE);							//写FLASH结束,开启数据缓存
							FLASH_Lock();										//上锁

							LampFrameWareState.current_bag_cnt ++;

							LampFrameWareState.state = FIRMWARE_DOWNLOADING;	//当前包下载完成
						}
						else if(current_bags == LampFrameWareState.total_bags)
						{
							crc32_read = (((u32)(*(msg + 0))) << 24) +
										 (((u32)(*(msg + 1))) << 16) +
										 (((u32)(*(msg + 2))) << 8) +
										 (((u32)(*(msg + 3))));

							file_len = (LAMP_FIRMWARE_BAG_SIZE - 2) * (LampFrameWareState.total_bags - 1);

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
									STMFLASH_ReadBytes(LAMP_FIRMWARE_BUCKUP_FLASH_BASE_ADD + 1024 * i,crc32_cal_buf,1024);
									crc32_cal = CRC32Extend(crc32_cal_buf,1024,crc32_cal,0);
								}
								if(i == k_num - 1)
								{
									if(last_k_byte_num == 0)
									{
										STMFLASH_ReadBytes(LAMP_FIRMWARE_BUCKUP_FLASH_BASE_ADD + 1024 * i,crc32_cal_buf,1024);
										crc32_cal = CRC32Extend(crc32_cal_buf,1024,crc32_cal,1);
									}
									else if(last_k_byte_num > 0)
									{
										STMFLASH_ReadBytes(LAMP_FIRMWARE_BUCKUP_FLASH_BASE_ADD + 1024 * i,crc32_cal_buf,last_k_byte_num);
										crc32_cal = CRC32Extend(crc32_cal_buf,last_k_byte_num,crc32_cal,1);
									}
								}
							}

							if(crc32_read == crc32_cal)
							{
								LampFrameWareState.state = FIRMWARE_DOWNLOADED;
							}
							else
							{
								LampFrameWareState.state = FIRMWARE_DOWNLOAD_FAILED;
							}

							WriteLampFrameWareState(0,1);
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

u8 LampStartFirmWareUpdate(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 m = 0;
	u8 tmp_h = 0;
	u8 tmp_l = 0;
	u16 add = 0;
	u8 state = 0;
	u8 md5[33] = {0};
	char tmp[5];
	char buf[4] = {0};
	char *msg = NULL;

	LampPlcExecuteTask_S *task = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	task = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));

	if(task != NULL)
	{
		memset(task,0,sizeof(LampPlcExecuteTask_S));

		for(j = 0; j < server_frame_struct->para_num; j ++)
		{
			switch(server_frame_struct->para[j].type)
			{
				case 0x8001:
					task->broadcast_type = myatoi((char *)server_frame_struct->para[j].value);
				break;

				case 0x4002:
					msg = (char *)server_frame_struct->para[j].value;

					if(task->broadcast_type == 2)
					{
						while(*msg != '\0')
						{
							while(*msg != ',')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							msg = msg + 1;
							if(i == 1 || i == 2)
							{
								if(i == 1)
								{
									tmp[1] = tmp[0];
									tmp[0] = '0';
								}

								StrToHex((u8 *)&add,tmp,1);
							}
							else if(i == 3 || i == 4)
							{
								if(i == 3)
								{
									tmp[3] = tmp[2];
									tmp[2] = tmp[1];
									tmp[1] = tmp[0];
									tmp[0] = '0';
								}

								StrToHex(&tmp_h,tmp + 0,1);
								StrToHex(&tmp_l,tmp + 2,1);

								add = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
							}

							task->group_dev_id[k ++] = add;

							i = 0;

							if(*msg == '\0')
							{
								break;
							}
						}
					}
					else
					{
						while(*msg != '\0')
						{
							while(*msg != ',' && *msg != '\0')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							task->group_dev_id[k ++] = myatoi(tmp);

							if(*msg == '\0')
							{
								break;
							}
						}
					}
				break;

				case 0x4003:
					memcpy(md5,server_frame_struct->para[0].value,server_frame_struct->para[0].len);

					if(search_str(md5, FrameWareState.md5) == -1)		//md5校验失败
					{
						DeleteLampPlcExecuteTask(task);

						state = 2;

						goto GET_OUT;
					}
					else if(LampFrameWareState.state != FIRMWARE_DOWNLOADED_SUCCESS)
					{
						DeleteLampPlcExecuteTask(task);

						state = 3;

						goto GET_OUT;
					}
				break;

				default:
				break;
			}
		}

		task->notify_enable = 1;
		task->cmd_code = 0x01F3;
		task->execute_total_num = FrameWareState.total_bags;

		switch(task->broadcast_type)
		{
			case 0:
				task->dev_num = LampNumList.number;
			break;

			case 1:
				task->group_num = k;
				for(m = 0; m < k; m ++)
				{
					task->dev_num += LampGroupListNum.list[task->group_dev_id[m]];
				}
			break;

			case 2:
				task->execute_type = 1;
				task->dev_num = k;
				task->execute_total_num = k * FrameWareState.total_bags;
			break;

			default:

			break;
		}

		if(xQueueSend(xQueue_LampPlcExecuteTaskToPlc,(void *)&task,(TickType_t)10) != pdPASS)
		{
#ifdef DEBUG_LOG
			printf("send xQueue_LampPlcFrame fail.\r\n");
#endif
			DeleteLampPlcExecuteTask(task);
		}
	}

	GET_OUT:
	resp_server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(resp_server_frame_struct != NULL)
	{
		CopyServerFrameStruct(server_frame_struct,resp_server_frame_struct,0);

		resp_server_frame_struct->msg_type 	= (u8)DEVICE_RESPONSE_UP;	//响应服务器类型
		resp_server_frame_struct->msg_len 	= 10;
		resp_server_frame_struct->err_code 	= (u8)NO_ERR;
		resp_server_frame_struct->para_num = 1;

		resp_server_frame_struct->para = (Parameter_S *)pvPortMalloc(resp_server_frame_struct->para_num * sizeof(Parameter_S));

		if(resp_server_frame_struct->para != NULL)
		{
			resp_server_frame_struct->para[i].type = 0x8101;
			memset(buf,0,4);
			sprintf(buf, "%d",state);
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

u8 LampSuspendFirmWareUpdate(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;

	LampPlcExecuteTaskState_S state;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	state.cmd_code = 0x01F3;

	state.state = 2;

	if(xQueueSend(xQueue_LampPlcExecuteTaskState,(void *)&state,(TickType_t)10) != pdPASS)
	{
#ifdef DEBUG_LOG
		printf("send xQueue_LampPlcExecuteTaskState fail 1.\r\n");
#endif
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

u8 LampStopFirmWareUpdate(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;

	LampPlcExecuteTaskState_S state;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	state.cmd_code = 0x01F3;

	state.state = 1;

	if(xQueueSend(xQueue_LampPlcExecuteTaskState,(void *)&state,(TickType_t)10) != pdPASS)
	{
#ifdef DEBUG_LOG
		printf("send xQueue_LampPlcExecuteTaskState fail 1.\r\n");
#endif
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

u8 LampGetFirmWareVersion(ServerFrameStruct_S *server_frame_struct)
{
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 tmp_h = 0;
	u8 tmp_l = 0;
	u16 add = 0;
	char tmp[5];
	char *msg = NULL;

	LampPlcExecuteTask_S *task = NULL;

	ServerFrameStruct_S *resp_server_frame_struct = NULL;		//用于响应服务器

	task = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));

	if(task != NULL)
	{
		memset(task,0,sizeof(LampPlcExecuteTask_S));

		for(j = 0; j < server_frame_struct->para_num; j ++)
		{
			switch(server_frame_struct->para[j].type)
			{
				case 0x8001:
					task->broadcast_type = myatoi((char *)server_frame_struct->para[j].value);
				break;

				case 0x4002:
					msg = (char *)server_frame_struct->para[j].value;

					if(task->broadcast_type == 2)
					{
						while(*msg != '\0')
						{
							while(*msg != ',')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							msg = msg + 1;
							if(i == 1 || i == 2)
							{
								if(i == 1)
								{
									tmp[1] = tmp[0];
									tmp[0] = '0';
								}

								StrToHex((u8 *)&add,tmp,1);
							}
							else if(i == 3 || i == 4)
							{
								if(i == 3)
								{
									tmp[3] = tmp[2];
									tmp[2] = tmp[1];
									tmp[1] = tmp[0];
									tmp[0] = '0';
								}

								StrToHex(&tmp_h,tmp + 0,1);
								StrToHex(&tmp_l,tmp + 2,1);

								add = ((((u16)tmp_h) << 8) & 0xFF00) + (u16)tmp_l;
							}

							task->group_dev_id[k ++] = add;

							i = 0;

							if(*msg == '\0')
							{
								break;
							}
						}
					}
					else
					{
						while(*msg != '\0')
						{
							while(*msg != ',' && *msg != '\0')
							tmp[i ++] = *(msg ++);
							tmp[i] = '\0';
							i = 0;
							msg = msg + 1;
							task->group_dev_id[k ++] = myatoi(tmp);

							if(*msg == '\0')
							{
								break;
							}
						}
					}
				break;

				default:
				break;
			}
		}
		
		task->notify_enable = 1;
		task->cmd_code = 0x01F6;
		task->execute_type = 2;

		LampGetLampPlcExecuteTaskInfo(task);

		if(xQueueSend(xQueue_LampPlcExecuteTaskToPlc,(void *)&task,(TickType_t)10) != pdPASS)
		{
#ifdef DEBUG_LOG
			printf("send xQueue_LampPlcFrame fail.\r\n");
#endif
			DeleteLampPlcExecuteTask(task);
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































































