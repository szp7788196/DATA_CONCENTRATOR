#ifndef __TASK_LED_H
#define __TASK_LED_H

#include "sys.h"
#include "rtos_task.h"

extern TaskHandle_t xHandleTaskLED;


void vTaskLED(void *pvParameters);







u16 HCI_DataAnalysis(u8 *inbuf,u16 inbuf_len,u8 *outbuf);

u8 CombineMMI_Frame(u8 cmd_code,u8 *data,u8 len,u8 *outbuf);
u8 HandleMMI_Frame(u8 *inbuf,u8 len,u8 *outbuf);

u8 GetLinkTypeLinkState(u8 cmd_code,u8 *outbuf);
u8 GetSysDateTime(u8 cmd_code,u8 *outbuf);
u8 SetSysDateTime(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf);
u8 GetDeviceInfo(u8 cmd_code,u8 *outbuf);
u8 GetDI_State(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf);
u8 GetDO_State(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf);
u8 SetDO_State(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf);
u8 SetLamp_State(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf);
u8 GetGateWayID(u8 cmd_code,u8 *outbuf);
u8 SetGateWayID(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf);
u8 GetLocalNetConfig(u8 cmd_code,u8 *outbuf);
u8 SetLocalNetConfig(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf);
u8 GetElectricRatio(u8 cmd_code,u8 *outbuf);
u8 SetElectricRatio(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf);
u8 SetSysReset(u8 cmd_code,u8 *data,u8 data_len,u8 *outbuf);


























#endif
