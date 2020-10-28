#ifndef __RELAY_COMM_H
#define __RELAY_COMM_H

#include "relay_conf.h"
#include "server_protocol.h"











void RelaySendStateChangesReportFrameToServer(RelayModuleState_S *module_state);
void RelaySendStateChangesReportToServer(void);


void RelayRecvAndHandleFrameStruct(void);





u8 RelayTransparentTransmission(ServerFrameStruct_S *server_frame_struct);
u8 RelayLoopControl(ServerFrameStruct_S *server_frame_struct);
u8 RelayLoopGroupControl(ServerFrameStruct_S *server_frame_struct);
u8 RelaySetStrategyGroupSwitch(ServerFrameStruct_S *server_frame_struct);
u8 RelayGetCurrentState(ServerFrameStruct_S *server_frame_struct);
u8 RelaySetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 RelayGetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 RelayGetAlarmReportHistory(ServerFrameStruct_S *server_frame_struct);
u8 RelaySetBasicConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 RelayGetBasicConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 RelaySetRelayAppointment(ServerFrameStruct_S *server_frame_struct);
u8 RelayGetRelayAppointment(ServerFrameStruct_S *server_frame_struct);
u8 RelaySetRelayStrategy(ServerFrameStruct_S *server_frame_struct);
u8 RelayGetRelayStrategy(ServerFrameStruct_S *server_frame_struct);
















#endif
