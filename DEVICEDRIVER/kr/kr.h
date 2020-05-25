#ifndef __KR_H
#define __KR_H

#include "sys.h"
#include "input_collector_conf.h"


#define KR1                  	PFin(8) 
#define KR2                  	PFin(7)
#define KR3                  	PFin(6) 
#define KR4                  	PFin(5)
#define KR5                  	PFin(4) 
#define KR6                  	PFin(3)
#define KR7                  	PFin(2)




void KR_Init(void);



void GetAllBuiltInInputCollectorState(InputCollectorState_S *state);
void GetBuiltOutInputCollectorState(InputCollectorState_S state);
u16 PackBuiltOutInputCollectorFrame(u8 address,u8 fun_code,u8 *inbuf,u16 inbuf_len,u8 *outbuf);
void AnalysisBuiltOutInputCollectorFrame(u8 *buf,u16 len,InputCollectorCollectState_S *collect_state);
















#endif
