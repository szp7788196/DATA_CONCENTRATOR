#ifndef __ELECTRICITY_METER_COMM_H
#define __ELECTRICITY_METER_COMM_H

#include "electricity_meter_conf.h"
#include "server_protocol.h"




extern u8 ElectricityMeterStateChangesReportResponse;





void ElectricityMeterSendStateChangesReportFrameToServer(ElectricityMeterState_S *module_state);
void ElectricityMeterSendStateChangesReportToServer(void);
void ElectricityMeterRecvAndHandleFrameStruct(void);
u8 ElectricityMeterGetCurrentState(ServerFrameStruct_S *server_frame_struct);


u8 ElectricityMeterSetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 ElectricityMeterGetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 ElectricityMeterGetAlarmReportHistory(ServerFrameStruct_S *server_frame_struct);
u8 ElectricityMeterSetBasicConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 ElectricityMeterGetBasicConfiguration(ServerFrameStruct_S *server_frame_struct);





























#endif
