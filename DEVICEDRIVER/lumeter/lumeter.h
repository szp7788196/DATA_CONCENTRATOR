#ifndef _KC_H
#define _KC_H
#include "sys.h"
#include "lumeter_comm.h"
#include "task_rs485.h"
#include "server_protocol.h"
#include "common.h"









void GetBuiltOutLumeterState(LumeterConfig_S config);
u16 PackBuiltOutLumeterFrame(u8 address,u8 mode,u8 *outbuf);
void AnalysisBuiltOutLumeterFrame(u8 *buf,u16 len,LumeterCollectState_S *lumeter_state);

































#endif
