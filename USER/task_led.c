//指示灯处理任务
#include "task_led.h"
#include "delay.h"
#include "led.h"
#include "watch_dog.h"
#include "rx8010s.h"
#include "exfuns.h"
#include "ff.h"
#include "ht7038.h"
#include "history_record.h"
#include "common.h"
#include "sun_rise_set.h"
#include "concentrator_conf.h"
#include "e_meter.h"
#include "ec20.h"
#include "kr.h"
#include "kc.h"
#include "plc.h"
#include "concentrator_comm.h"
#include "task_tcp_client.h"
#include "task_plc.h"
#include "cpu_utils.h"
#include "dp83848.h"
#include "battery.h"
#include "task_handle_server_frame.h"
#include "history_record.h"




TaskHandle_t xHandleTaskLED = NULL;
unsigned portBASE_TYPE SatckLED;

u32 FreeHeapSize = 0;
//u16 CPU_Usage = 0;


void vTaskLED(void *pvParameters)
{
	u32 cnt = 0;
	u8 date = 0;
	u8 mmi_len = 0;
	u8 mmi_outbuf[256] = {0};
	u8 hci_len = 0;
	u8 hci_outbuf[32] = {0};

	u8 led_run_state = 0;
	u8 led_eth_state = 0;
	u8 led_4g_state = 0;
	u8 led_485_state = 0;
	u8 led_recv_state = 0;
	u8 led_send_state = 0;

	while(1)											//循环一次延时约100ms
	{
		if(Usart4RecvEnd == 0xAA)						//处理屏幕发过来的数据
		{
			Usart4RecvEnd = 0;

			mmi_len = HandleMMI_Frame(Usart4RxBuf,Usart4FrameLen,mmi_outbuf);

			memset(Usart4RxBuf,0,Usart4FrameLen);

			Usart4FrameLen = 0;

			if(mmi_len >= 5)
			{
				UsartSendString(UART4,mmi_outbuf, mmi_len);
			}
		}

		if(Usart1RecvEnd == 0xAA)						//处理屏幕发过来的数据
		{
			Usart1RecvEnd = 0;

			hci_len = HCI_DataAnalysis(Usart1RxBuf,Usart1FrameLen,hci_outbuf);

			memset(Usart1RxBuf,0,Usart1FrameLen);

			Usart1FrameLen = 0;

			if(hci_len >= 1)
			{
				UsartSendString(USART1,hci_outbuf, hci_len);
			}
		}

		if(date != calendar.w_date)						//计算新一天的日出日落时间
		{
			date = calendar.w_date;

			SunRiseSetTime = GetSunTime(calendar.w_year,
			                            calendar.w_month,
			                            calendar.w_date,
			                            ConcentratorLocationConfig.longitude,
			                            ConcentratorLocationConfig.latitude);
		}

		if(cnt % 50 == 0)								//每1秒喂一次看门狗
		{
			IWDG_Feed_Thread();

			RX8010S_Get_Time();
		}

		if(cnt % 250 == 0)
		{
			FreeHeapSize = xPortGetFreeHeapSize();

#ifdef DEBUG_LOG
			printf("FreeHeapSize:%d\r\n",FreeHeapSize);
#endif
		}

		if(cnt % 20 == 0)								//运行灯以1秒的周期闪烁
		{
			led_run_state = ~led_run_state;
		}

		if(NetCableState == 1)
		{
			if(ETH_ConnectState == ETH_CONNECTED)
			{
				if(cnt % 150 == 0)								//运行灯以1秒的周期闪烁
				{
					led_eth_state = 1;
				}
				else
				{
					led_eth_state = 0;
				}
			}
			else
			{
				if(cnt % 25 == 0)								//运行灯以1秒的周期闪烁
				{
					led_eth_state = ~led_eth_state;
				}
			}
		}
		else
		{
			led_eth_state = 0;
		}

		if(EC20ConnectState == CONNECTED)
		{
			if(cnt % 150 == 0)								//运行灯以1秒的周期闪烁
			{
				led_4g_state = 1;
			}
			else
			{
				led_4g_state = 0;
			}
		}
		else
		{
			if(cnt % 25 == 0)								//运行灯以1秒的周期闪烁
			{
				led_4g_state = ~led_4g_state;
			}
		}

		if(Rs485RecvCnt != 0 || Rs485SendCnt != 0)
		{
			if(Rs485RecvCnt > 0)
			{
				Rs485RecvCnt -= 2;
			}
			if(Rs485RecvCnt < 0)
			{
				Rs485RecvCnt = 0;
			}

			if(Rs485SendCnt > 0)
			{
				Rs485SendCnt -= 2;
			}
			if(Rs485SendCnt < 0)
			{
				Rs485SendCnt = 0;
			}

			led_485_state = ~led_485_state;
		}
		else
		{
			led_485_state = 0;
		}

		if(NetWorkRecvCnt != 0)
		{
			if(NetWorkRecvCnt > 0)
			{
				NetWorkRecvCnt -= 2;
			}
			if(NetWorkRecvCnt < 0)
			{
				NetWorkRecvCnt = 0;
			}

			led_recv_state = ~led_recv_state;
		}
		else
		{
			led_recv_state = 0;
		}

		if(NetWorkSendCnt != 0)
		{
			if(NetWorkSendCnt > 0)
			{
				NetWorkSendCnt -= 2;
			}
			if(NetWorkSendCnt < 0)
			{
				NetWorkSendCnt = 0;
			}

			led_send_state = ~led_send_state;
		}
		else
		{
			led_send_state = 0;
		}

		if(led_run_state)
		{
			LED_RUN = 0;
			LED_STATE = 0;
		}
		else
		{
			LED_RUN = 1;
			LED_STATE = 1;
		}

		if(led_eth_state)
		{
			LED_ETH = 1;
		}
		else
		{
			LED_ETH = 0;
		}

		if(led_4g_state)
		{
			LED_4G = 1;
		}
		else
		{
			LED_4G = 0;
		}

		if(led_485_state)
		{
			LED_RS485 = 1;
		}
		else
		{
			LED_RS485 = 0;
		}

		if(led_recv_state)
		{
			LED_DOWN = 1;
		}
		else
		{
			LED_DOWN = 0;
		}

		if(led_send_state)
		{
			LED_UP = 1;
		}
		else
		{
			LED_UP = 0;
		}

		if(SysAlarmState)
		{
			LED_ALARM = 1;
		}
		else
		{
			LED_ALARM = 0;
		}

		if(BatteryManagement.discharge_voltage >= BATTERY_LOW_BATTERY)
		{
			LED_12V = 1;
		}
		else
		{
			LED_12V = 0;
		}

		cnt = (cnt + 1) & 0xFFFFFFFF;

		delay_ms(20);									//循环一次延时约20ms

		SatckLED = uxTaskGetStackHighWaterMark(NULL);
	}
}

//人机交互数据解析
u16 HCI_DataAnalysis(u8 *inbuf,u16 inbuf_len,u8 *outbuf)
{
	u16 ret = 0;
	u8 buf[4] = {0};
	u8 tmp[9] = {0};
	u8 plc_addr_w[6] = {0};
	u8 plc_addr_r[6] = {0};

	if(inbuf_len == 17)
	{
		if(MyStrstr(inbuf, "AT+ADD=", inbuf_len, 7) != 0xFFFF)
		{
			StrToHex(buf, (char*)inbuf + 7, 4);

			ConcentratorGateWayID.number = (((u32)buf[0] << 24) +
										   ((u32)buf[1] << 16) +
										   ((u32)buf[2] << 8) +
										   (u32)buf[3]);

			WriteConcentratorGateWayID(0,1);

			memcpy(&plc_addr_w[2],buf,4);

			plc_set_addr(plc_addr_w);

			delay_ms(100);

			plc_get_addr(plc_addr_r);

			if(MyStrstr(plc_addr_w,plc_addr_r, 6, 6) != 0xFFFF)
			{
				HexToStr((char *)tmp, buf, 4);

				sprintf((char*)outbuf,"{\"LTUAddr\":\"%s\"}\r\n",tmp);

				ret = strlen((char *)outbuf);
			}
			else
			{
				sprintf((char*)outbuf,"set address failed\r\n");

				ret = strlen((char *)outbuf);
			}
		}
	}
	else if(inbuf_len == 8)
	{
		if(MyStrstr(inbuf, "AT+ADD", inbuf_len, 6) != 0xFFFF)
		{
			buf[0] = (u8)((ConcentratorGateWayID.number >> 24) & 0x000000FF);		//网关ID
			buf[1] = (u8)((ConcentratorGateWayID.number >> 16) & 0x000000FF);
			buf[2] = (u8)((ConcentratorGateWayID.number >>  8) & 0x000000FF);
			buf[3] = (u8)((ConcentratorGateWayID.number >>  0) & 0x000000FF);

			plc_get_addr(plc_addr_r);

			if(MyStrstr(&plc_addr_r[2],buf, 4, 4) != 0xFFFF)
			{
				HexToStr((char*)tmp,&plc_addr_r[2],4);

				sprintf((char*)outbuf,"{\"LTUAddr\":\"%s\"}\r\n",tmp);

				ret = strlen((char *)outbuf);
			}
			else
			{
				sprintf((char*)outbuf,"not configured!\r\n");

				ret = strlen((char *)outbuf);
			}
		}
	}

	return ret;
}


u8 CombineMMI_Frame(u8 cmd_code,u8 *data,u8 len,u8 *outbuf)
{
	*(outbuf + 0) = 0x68;
	*(outbuf + 1) = cmd_code;
	*(outbuf + 2) = len;

	memcpy(outbuf + 3,data,len);

	*(outbuf + 3 + len + 0) = CalCheckSum(outbuf, len + 3);

	*(outbuf + 3 + len + 1) = 0x16;

	return len + 5;
}

//处理MMI数据
u8 HandleMMI_Frame(u8 *inbuf,u8 len,u8 *outbuf)
{
	u8 ret = 0;
	u8 cmd_code = 0;
	u16 data_len = 0;
	u8 *data = NULL;
	u8 read_check_sum = 0;
	u8 cal_check_sum = 0;

	if(*(inbuf + 0) != 0x68 || *(inbuf + len - 1) != 0x16)
	{
		return 0;
	}

	read_check_sum = *(inbuf + len - 2);
	cal_check_sum = CalCheckSum(inbuf, len - 2);

	if(read_check_sum != cal_check_sum)
	{
		return 0;
	}

	cmd_code = *(inbuf + 1);
	data_len = *(inbuf + 2);
	data = inbuf + 3;

	switch(cmd_code)
	{
		case 0x00:
			ret = CombineMMI_Frame(cmd_code,(u8 *)&BulitInMeterParas,sizeof(BulitInMeterParas_S),outbuf);
		break;

		case 0x01:
			ret = GetDI_State(cmd_code,data,data_len,outbuf);
		break;

		case 0x02:
			ret = SetDO_State(cmd_code,data,data_len,outbuf);
		break;

		case 0x03:
			ret = SetLamp_State(cmd_code,data,data_len,outbuf);
		break;

		case 0x04:
			ret = GetDeviceInfo(cmd_code,outbuf);
		break;

		case 0x05:
			ret = GetLocalNetConfig(cmd_code,outbuf);
		break;

		case 0x06:
			ret = SetLocalNetConfig(cmd_code,data,data_len,outbuf);
		break;

		case 0x07:
			ret = GetGateWayID(cmd_code,outbuf);
		break;

		case 0x08:
			ret = SetGateWayID(cmd_code,data,data_len,outbuf);
		break;

		case 0x09:
			ret = GetElectricRatio(cmd_code,outbuf);
		break;

		case 0x0A:
			ret = SetElectricRatio(cmd_code,data,data_len,outbuf);
		break;

		case 0x0B:
			ret = GetSysDateTime(cmd_code,outbuf);
		break;

		case 0x0C:
			ret = SetSysDateTime(cmd_code,data,data_len,outbuf);
		break;

		case 0x0D:

		break;

		case 0x0E:

		break;

		case 0x0F:

		break;

		case 0x10:
			ret = GetLinkTypeLinkState(cmd_code,outbuf);
		break;

		case 0x11:
			ret = GetDO_State(cmd_code,data,data_len,outbuf);
		break;

		case 0x12:
			ret = SetSysReset(cmd_code,data,data_len,outbuf);
		break;

		default:
		break;
	}

	return ret;
}

u8 GetLinkTypeLinkState(u8 cmd_code,u8 *outbuf)
{
	u8 ret = 0;
	u8 buf[3] = {0};

	CONNECTION_MODE_E conn_mode = MODE_4G;

	if(ConcentratorBasicConfig.connection_mode == (u8)MODE_INSIDE)
	{
		if(EC20ConnectState == CONNECTED && ETH_ConnectState == ETH_CONNECTED)
		{
			conn_mode = MODE_4G;
		}
		else if(EC20ConnectState == CONNECTED && ETH_ConnectState != ETH_CONNECTED)
		{
			conn_mode = MODE_4G;
		}
		else if(EC20ConnectState != CONNECTED && ETH_ConnectState == ETH_CONNECTED)
		{
			conn_mode = MODE_ETH;
		}
		else
		{
			conn_mode = MODE_INSIDE;
		}
	}
	else
	{
		conn_mode = (CONNECTION_MODE_E)ConcentratorBasicConfig.connection_mode;
	}

	buf[0] = (u8)conn_mode;

	if(conn_mode == MODE_4G)
	{
		if(EC20ConnectState == (u8)CONNECTED)
		{
			buf[1] = 1;
		}

		buf[2] = 0 - EC20Info.csq;
	}
	else if(conn_mode == MODE_ETH)
	{
		if(ETH_ConnectState == (u8)ETH_CONNECTED)
		{
			buf[1] = 1;
		}

		buf[2] = 0;
	}

	ret = CombineMMI_Frame(cmd_code,buf,3,outbuf);

	return ret;
}

u8 GetSysDateTime(u8 cmd_code,u8 *outbuf)
{
	u8 ret = 0;
	u8 buf[6] = {0};

	buf[0] = calendar.w_year - 2000;
	buf[1] = calendar.w_month;
	buf[2] = calendar.w_date;
	buf[3] = calendar.hour;
	buf[4] = calendar.min;
	buf[5] = calendar.sec;

	ret = CombineMMI_Frame(cmd_code,buf,6,outbuf);

	return ret;
}

u8 GetDeviceInfo(u8 cmd_code,u8 *outbuf)
{
	u8 ret = 0;
	u8 buf[6] = {0};

	buf[0] = SOFT_WARE_VRESION / 100;
	buf[1] = SOFT_WARE_VRESION % 100;

	buf[2] = (u8)((ConcentratorGateWayID.number >> 24) & 0x000000FF);		//网关ID
	buf[3] = (u8)((ConcentratorGateWayID.number >> 16) & 0x000000FF);
	buf[4] = (u8)((ConcentratorGateWayID.number >>  8) & 0x000000FF);
	buf[5] = (u8)((ConcentratorGateWayID.number >>  0) & 0x000000FF);

	ret = CombineMMI_Frame(cmd_code,buf,6,outbuf);

	return ret;
}

u8 GetDI_State(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf)
{
	u8 i = 0;
	u8 ret = 0;
	u8 buf[2] = {0};

	if(*(data + 0) == 0 && *(data + 1) == 0)
	{
		buf[0] = (u8)((BuiltInInputCollectorState >>  8) & 0x00FF);
		buf[1] = (u8)((BuiltInInputCollectorState >>  0) & 0x00FF);
	}
	else
	{
		for(i = 0; i < InputCollectorConfigNum.number; i ++)
		{
			if(InputCollectorState[i].address == *(data + 0) &&
			   InputCollectorState[i].channel == *(data + 1))
			{
				buf[0] = (u8)((InputCollectorState[i].d_current_state >>  8) & 0x00FF);
				buf[1] = (u8)((InputCollectorState[i].d_current_state >>  0) & 0x00FF);

				break;
			}
		}
	}

	ret = CombineMMI_Frame(cmd_code,buf,2,outbuf);

	return ret;
}

u8 GetDO_State(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf)
{
	u8 i = 0;
	u8 ret = 0;
	u8 buf[2] = {0};

	if(*(data + 0) == 0 && *(data + 1) == 0)
	{
		buf[0] = (u8)((BuiltInRelayState >>  8) & 0x00FF);
		buf[1] = (u8)((BuiltInRelayState >>  0) & 0x00FF);
	}
	else
	{
		for(i = 0; i < RelayModuleConfigNum.number; i ++)
		{
			if(RelayModuleState[i].address == *(data + 0) &&
			   RelayModuleState[i].channel == *(data + 1))
			{
				buf[0] = (u8)((RelayModuleState[i].loop_current_state >>  8) & 0x00FF);
				buf[1] = (u8)((RelayModuleState[i].loop_current_state >>  0) & 0x00FF);

				break;
			}
		}
	}

	ret = CombineMMI_Frame(cmd_code,buf,2,outbuf);

	return ret;
}

u8 SetDO_State(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf)
{
	u8 i = 0;
	u8 ret = 0;
	u16 loop_ch = 0xFFFF;
	u16 loop_state = 0;
	u8 err_code = 0;

	if(data_len == 4)
	{
		if(RelayModuleConfigNum.number == 0)
		{
			if(*(data + 0) == 0 && *(data + 1) == 0)
			{
				RelayModuleState_S state;

				state.loop_current_channel = 0xFFFF;
				state.loop_current_state = (((u16)(*(data + 2))) << 8) + *(data + 3);

				ControlAllBuiltInRelay(state);
			}
		}
		else
		{
			for(i = 0; i < RelayModuleConfigNum.number; i ++)
			{
				if(RelayModuleState[i].address == *(data + 0) &&
				   RelayModuleState[i].channel == *(data + 1))
				{
//					if(*(data + 0) == 0 && *(data + 1) == 0)
//					{
//						loop_ch = 0xFFFF;
//						loop_state = (((u16)(*(data + 2))) << 8) + *(data + 3);
//					}
//					else
//					{
//						loop_ch &= RelayModuleConfig[i].loop_enable;
//						loop_state &= RelayModuleConfig[i].loop_enable;
//					}

					loop_ch = 0xFFFF;
					loop_state = (((u16)(*(data + 2))) << 8) + *(data + 3);

//					loop_ch &= RelayModuleConfig[i].loop_enable;
//					loop_state &= RelayModuleConfig[i].loop_enable;

					RelayModuleState[i].loop_current_channel = loop_ch;
					RelayModuleState[i].loop_current_state = loop_state;
					RelayModuleState[i].controller = 4;
					memset(RelayModuleState[i].control_time,0,15);
					TimeToString(RelayModuleState[i].control_time,
								 calendar.w_year,
								 calendar.w_month,
								 calendar.w_date,
								 calendar.hour,
								 calendar.min,
								 calendar.sec);

					RelayModuleState[i].execute_immediately = 1;

					break;
				}
			}
		}
	}

	ret = CombineMMI_Frame(cmd_code,&err_code,1,outbuf);

	return ret;
}

u8 SetLamp_State(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf)
{
	u8 ret = 0;
	u8 err_code = 0;
	u8 *_data = NULL;
	u32 add = 0;
	LampPlcExecuteTask_S *task = NULL;

	if(data_len == 7)
	{
		if(*(data + 0) <= 2)
		{
			_data = (u8 *)pvPortMalloc(4 * sizeof(u8));

			if(_data != NULL)
			{
				task = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));

				if(task != NULL)
				{
					memset(task,0,sizeof(LampPlcExecuteTask_S));

					task->cmd_code = 0x0103;
					task->broadcast_type = *(data + 0);

					if(LampBasicConfig.auto_report_plc_state & (1 << 3))
					{
						task->notify_enable = 1;
					}

					*(_data + 0) = 0x03;
					*(_data + 1) = *(data + 1);
					*(_data + 2) = *(data + 2);
					*(_data + 3) = 0;

					task->data = _data;
					task->data_len = 4;

					add = ((((u32)(*(data + 3))) << 24) & 0xFF000000) +
						  ((((u32)(*(data + 4))) << 16) & 0x00FF0000) +
						  ((((u32)(*(data + 5))) <<  8) & 0x0000FF00) +
						  ((((u32)(*(data + 6))) <<  0) & 0x000000FF);

					task->group_dev_id[0] = add;

					switch(task->broadcast_type)
					{
						case 0:
							task->execute_type = 0;
							task->dev_num = LampNumList.number;
							task->execute_total_num = 1;
						break;

						case 1:
							task->execute_type = 0;
							task->group_num = 1;
							task->dev_num = LampGroupListNum.list[task->group_dev_id[0]];
							task->execute_total_num = 1;
						break;

						case 2:
							task->execute_type = 1;
							task->dev_num = 1;
							task->execute_total_num = 1;
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
			}
		}
	}

	ret = CombineMMI_Frame(cmd_code,&err_code,1,outbuf);

	return ret;
}

u8 GetGateWayID(u8 cmd_code,u8 *outbuf)
{
	u8 ret = 0;
	u8 buf[4] = {0};

	buf[0] = (u8)((ConcentratorGateWayID.number >> 24) & 0x000000FF);		//网关ID
	buf[1] = (u8)((ConcentratorGateWayID.number >> 16) & 0x000000FF);
	buf[2] = (u8)((ConcentratorGateWayID.number >>  8) & 0x000000FF);
	buf[3] = (u8)((ConcentratorGateWayID.number >>  0) & 0x000000FF);

	ret = CombineMMI_Frame(cmd_code,buf,4,outbuf);

	return ret;
}

u8 SetGateWayID(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf)
{
	u8 ret = 0;
	u8 err_code = 0;
	u8 plc_addr_w[6] = {0};

	if(data_len == 4)
	{
		ConcentratorGateWayID.number = (((u32)data[0] << 24) +
									   ((u32)data[1] << 16) +
									   ((u32)data[2] << 8) +
									   (u32)data[3]);

		WriteConcentratorGateWayID(0,1);

		memcpy(&plc_addr_w[2],data,4);

		plc_set_addr(plc_addr_w);
	}

	ret = CombineMMI_Frame(cmd_code,&err_code,1,outbuf);

	return ret;
}

u8 GetLocalNetConfig(u8 cmd_code,u8 *outbuf)
{
	u8 ret = 0;

	ret = CombineMMI_Frame(cmd_code,(u8 *)&ConcentratorLocalNetConfig,sizeof(ConcentratorLocalNetConfig_S),outbuf);

	return ret;
}

u8 SetLocalNetConfig(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf)
{
	u8 ret = 0;
	u8 err_code = 0;
	char tmp[10] = {0};

	if(data_len == sizeof(ConcentratorLocalNetConfig_S))
	{
		memcpy(&ConcentratorLocalNetConfig,data,data_len);

		WriteConcentratorLocalNetConfig(0,1);

		ConcentratorBasicConfig.connection_mode = ConcentratorLocalNetConfig.connection_mode;

		memset(ConcentratorBasicConfig.server_ip,0,31);
		memset(ConcentratorBasicConfig.server_port,0,6);

		sprintf(tmp, "%d",ConcentratorLocalNetConfig.remote_ip[0]);
		strcat((char *)ConcentratorBasicConfig.server_ip,tmp);
		strcat((char *)ConcentratorBasicConfig.server_ip,".");

		sprintf(tmp, "%d",ConcentratorLocalNetConfig.remote_ip[1]);
		strcat((char *)ConcentratorBasicConfig.server_ip,tmp);
		strcat((char *)ConcentratorBasicConfig.server_ip,".");

		sprintf(tmp, "%d",ConcentratorLocalNetConfig.remote_ip[2]);
		strcat((char *)ConcentratorBasicConfig.server_ip,tmp);
		strcat((char *)ConcentratorBasicConfig.server_ip,".");

		sprintf(tmp, "%d",ConcentratorLocalNetConfig.remote_ip[3]);
		strcat((char *)ConcentratorBasicConfig.server_ip,tmp);

		sprintf((char *)ConcentratorBasicConfig.server_port, "%d",ConcentratorLocalNetConfig.remote_port);

		WriteConcentratorBasicConfig(0,1);
	}

	ret = CombineMMI_Frame(cmd_code,&err_code,1,outbuf);

	return ret;
}

u8 GetElectricRatio(u8 cmd_code,u8 *outbuf)
{
	u8 ret = 0;
	u8 buf[4] = {0};

	buf[0] = (u8)(((u16)BulitInMeterRatio.voltage_ratio >> 8) & 0x00FF);		//网关ID
	buf[1] = (u8)(((u16)BulitInMeterRatio.voltage_ratio >> 0) & 0x00FF);
	buf[2] = (u8)(((u16)BulitInMeterRatio.current_ratio >> 8) & 0x00FF);
	buf[3] = (u8)(((u16)BulitInMeterRatio.current_ratio >> 0) & 0x00FF);

	ret = CombineMMI_Frame(cmd_code,buf,4,outbuf);

	return ret;
}

u8 SetElectricRatio(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf)
{
	u8 i = 0;
	u8 ret = 0;
	u8 err_code = 0;

	if(data_len == 4)
	{
		BulitInMeterRatio.voltage_ratio = (float)(((u16)data[0] << 8) +(u16)data[1]);
		BulitInMeterRatio.current_ratio = (float)(((u16)data[2] << 8) +(u16)data[3]);

		WriteBulitInMeterRatio(0,1);

		for(i = 0; i < ElectricityMeterConfigNum.number; i ++)
		{
			if(ElectricityMeterConfig[i].address == 0 &&
			   ElectricityMeterConfig[i].channel == 0)
			{
				ElectricityMeterConfig[i].voltage_ratio = BulitInMeterRatio.voltage_ratio;
				ElectricityMeterConfig[i].current_ratio = BulitInMeterRatio.current_ratio;

				WriteElectricityMeterConfig(i,0,1);

				break;
			}
		}
	}

	ret = CombineMMI_Frame(cmd_code,&err_code,1,outbuf);

	return ret;
}

u8 SetSysDateTime(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf)
{
	u8 ret = 0;
	u8 err_code = 0;

	if(data_len == 6)
	{
		RX8010S_Set_Time(*(data + 0),*(data + 1),*(data + 2),*(data + 3),*(data + 4),*(data + 5));
	}

	ret = CombineMMI_Frame(cmd_code,&err_code,1,outbuf);

	return ret;
}

u8 SetSysReset(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf)
{
	u8 ret = 0;
	u8 err_code = 0;
	u8 type = 0;

	if(data_len == 1)
	{
		type = *(data + 0);

		switch(type)
		{
			case 0:		//重启
				FlagSystemReBoot = 1;
			break;

			case 1:		//重启 + 恢复出厂

			break;

			case 2:		//重链接
				FlagReConnectToServer = 1;
			break;

			default:
			break;
		}
	}

	ret = CombineMMI_Frame(cmd_code,&err_code,1,outbuf);

	return ret;
}







































