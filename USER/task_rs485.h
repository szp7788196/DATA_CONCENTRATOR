#ifndef __TASK_RS485_H
#define __TASK_RS485_H

#include "sys.h"
#include "rtos_task.h"
#include "server_protocol.h"




typedef struct	Rs485Frame
{
	DEVICE_TYPE_E device_type;
	u16 len;
	
	u8 *buf;
}__attribute__((packed))Rs485Frame_S;







extern TaskHandle_t xHandleTaskRS485;
extern s16 Rs485RecvCnt;
extern s16 Rs485SendCnt;



void vTaskRS485(void *pvParameters);
void DeleteRs485Frame(Rs485Frame_S *rs485_frame);
void RecvRs485FrameQueueAndSendToDeviceAndWaitResponse(void);
void RecvAnsAnalysisHCI_Frame(void);




























#endif
