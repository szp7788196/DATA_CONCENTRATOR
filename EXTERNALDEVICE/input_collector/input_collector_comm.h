#ifndef __INPUT_COLLECTOR_COMM_H
#define __INPUT_COLLECTOR_COMM_H

#include "input_collector_conf.h"
#include "server_protocol.h"



extern u8 InputCollectorStateChangesReportResponse;










void InputCollectorSendStateChangesReportFrameToServer(InputCollectorState_S *module_state);
void InputCollectorSendStateChangesReportToServer(void);

void InputCollectorRecvAndHandleFrameStruct(void);

u8 InputCollectorGetCurrentState(ServerFrameStruct_S *server_frame_struct);
u8 InputCollectorSetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 InputCollectorGetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 InputCollectorGetAlarmReportHistory(ServerFrameStruct_S *server_frame_struct);
u8 InputCollectorSetBasicConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 InputCollectorGetBasicConfiguration(ServerFrameStruct_S *server_frame_struct);






















#endif
