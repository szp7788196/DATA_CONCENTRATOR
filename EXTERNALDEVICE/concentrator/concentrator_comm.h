#ifndef __CONCENTRATOR_COMM_H
#define __CONCENTRATOR_COMM_H

#include "sys.h"
#include "server_protocol.h"
#include "history_record.h"
#include "concentrator_conf.h"





extern u8 FlagSystemReBoot;										//ϵͳ������ʶ
extern u8 FlagReConnectToServer;								//����������־
extern u8 LoginResponse;										//�������Ե�¼������Ӧ
extern u8 HeartBeatResponse;									//������������������Ӧ
extern u8 AlarmReportResponse;									//�������Ը澯������Ӧ




void SendLoginFrameToServer(void);
void SendHeartBeatFrameToServer(void);
void SendOtaRequestFrameToServer(FrameWareState_S frame_ware_state);
void SendOtaCompleteFrameToServer(void);
void SendAlarmReportFrameToServer(AlarmReport_S *alarm_report);
void SendTransTransFrameToServer(TransTransmissionFrame_S  *trans_trans_frame);
void SendHeartBeatToServer(void);
void SendOtaCompleteNoticeToServer(void);
void SendOtaRequestToServer(void);
void SendAlarmReportToServer(void);
void SendTransTransToServer(void);
void AutoSendFrameToServer(void);
void ConcentratorRecvAndHandleFrameStruct(void);


u8 TransparentTransmission(ServerFrameStruct_S *server_frame_struct);
u8 SynchronizeTime(ServerFrameStruct_S *server_frame_struct);
u8 ResetConfigParameters(ServerFrameStruct_S *server_frame_struct);
u8 RebootTheSystem(ServerFrameStruct_S *server_frame_struct);
u8 ReConnectToServer(ServerFrameStruct_S *server_frame_struct);
u8 SetRunMode(ServerFrameStruct_S *server_frame_struct);
u8 QueryState(ServerFrameStruct_S *server_frame_struct);
u8 SetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 GetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 SetBasicConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 GetBasicConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 SetLocationConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 GetLocationConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 RequestFrameWareUpDate(ServerFrameStruct_S *server_frame_struct);
u8 RecvFrameWareBag(ServerFrameStruct_S *server_frame_struct);
u8 FrameWareUpDateStop(ServerFrameStruct_S *server_frame_struct);
u8 GetAlarmReportHistory(ServerFrameStruct_S *server_frame_struct);

































#endif
