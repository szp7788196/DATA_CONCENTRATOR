#ifndef __TASK_HANDLE_SERVER_FRAME_H
#define __TASK_HANDLE_SERVER_FRAME_H

#include "sys.h"
#include "rtos_task.h"
#include "server_protocol.h"




extern TaskHandle_t xHandleTaskHANDLE_SERVER_FRAME;



void vTaskHANDLE_SERVER_FRAME(void *pvParameters);



void PushTheFrameToTxQueue(ServerFrame_S *tx_frame);

void RecvNetFrameAndPushToRxQueue(CONNECTION_MODE_E conncetion_mode);
































#endif
