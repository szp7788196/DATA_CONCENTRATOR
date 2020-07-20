#ifndef __LAMP_COMM_H
#define __LAMP_COMM_H

#include "lamp_conf.h"
#include "server_protocol.h"
#include "task_plc.h"
#include "lamp_event.h"


void LampSendExecuteLampPlcExecuteTaskFrameToServer(LampPlcExecuteTask_S *recv_task);
void LampSendExecuteLampPlcExecuteTaskToServer(void);
void LampRecvLampStateAndSendToServer(void);
void LampSendOtaRequestFrameToServer(FrameWareState_S frame_ware_state);
void LampSendOtaCompleteFrameToServer(void);
void LampSendOtaCompleteNoticeToServer(void);
void LampSendOtaRequestToServer(void);





void LampRecvAndHandleFrameStruct(void);
u8 LampSynchronizeTime(ServerFrameStruct_S *server_frame_struct);
u8 LampResetConfigParameters(ServerFrameStruct_S *server_frame_struct);
u8 LampAdjustBrightness(ServerFrameStruct_S *server_frame_struct);
u8 LampFlashTest(ServerFrameStruct_S *server_frame_struct);
u8 LampSetStrategyGroupSwitch(ServerFrameStruct_S *server_frame_struct);
u8 LampSetRunMode(ServerFrameStruct_S *server_frame_struct);
u8 LampSetExecuteState(ServerFrameStruct_S *server_frame_struct);
u8 LampGetCurrentState(ServerFrameStruct_S *server_frame_struct);
u8 LampSetBasicConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 LampGetBasicConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 LampReSetDeviceAddress(ServerFrameStruct_S *server_frame_struct);



u8 LampRequestFrameWareUpDate(ServerFrameStruct_S *server_frame_struct);
u8 LampRecvFrameWareBag(ServerFrameStruct_S *server_frame_struct);
u8 LampStartFirmWareUpdate(ServerFrameStruct_S *server_frame_struct);
u8 LampSuspendFirmWareUpdate(ServerFrameStruct_S *server_frame_struct);
u8 LampStopFirmWareUpdate(ServerFrameStruct_S *server_frame_struct);
u8 LampGetFirmWareVersion(ServerFrameStruct_S *server_frame_struct);









#endif
