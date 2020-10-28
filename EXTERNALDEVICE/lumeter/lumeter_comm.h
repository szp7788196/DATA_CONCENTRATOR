#ifndef __LUMETER_COMM_H
#define __LUMETER_COMM_H

#include "lumeter_conf.h"
#include "server_protocol.h"



extern u8 LumeterStateChangesReportResponse;










void LumeterSendStateChangesReportFrameToServer(void);
void LumeterSendStateChangesReportToServer(void);

void LumeterRecvAndHandleFrameStruct(void);
u8 LumeterTransparentTransmission(ServerFrameStruct_S *server_frame_struct);
u8 LumeterGetCurrentState(ServerFrameStruct_S *server_frame_struct);
u8 LumeterSetILluminanceValue(ServerFrameStruct_S *server_frame_struct);
u8 LumeterGetCurrentState(ServerFrameStruct_S *server_frame_struct);
u8 LumeterSetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 LumeterGetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 LumeterGetAlarmReportHistory(ServerFrameStruct_S *server_frame_struct);
u8 LumeterSetBasicConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 LumeterGetBasicConfiguration(ServerFrameStruct_S *server_frame_struct);






















#endif



