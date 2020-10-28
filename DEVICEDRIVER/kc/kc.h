#ifndef _KC_H
#define _KC_H
#include "sys.h"
#include "relay_comm.h"
#include "task_rs485.h"
#include "server_protocol.h"
#include "common.h"



#define KC1                  		PEout(10)          
#define KC2                  		PEout(11)           
#define KC3                  		PEout(12)        
#define KC4                  		PEout(13)       
#define KC5                  		PEout(14)      
#define KC6                  		PEout(15)



extern u16 BuiltInRelayState;

void KC_Init(void);



void ControlAllBuiltInRelay(RelayModuleState_S state);
void ControlAllBuiltOutRelay(RelayModuleState_S state);
void GetBuiltOutRelayState(RelayModuleState_S state);
u16 PackBuiltOutRelayFrame(u8 address,u8 fun_code,u8 *inbuf,u16 inbuf_len,u8 *outbuf);
u8 AnalysisBuiltOutRelayFrame(u8 *buf,u16 len,RelayModuleCollectState_S *collect_state);

































#endif
