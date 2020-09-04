#ifndef __PLC_PROTOCOL_H
#define __PLC_PROTOCOL_H


#include "task_plc.h"
#include "lamp_conf.h"



typedef struct	PlcTask									//单灯任务内容
{
	u8 group_id;											//模式(组)编号
	u8 type;												//任务类型
	u8 executor;											//执行者
	s16 time;												//动作时间
	u8 time_option;											//时间选项
	u8 brightness[MAX_LAMP_CH_NUM];							//灯头亮度
	
	u16 crc16;												//校验码 存储用
	
}__attribute__((packed))PlcTask_S;



u8 CombinePlcUserFrame(LampPlcExecuteTask_S *task,PlcFrame_S *frame,LampConfig_S lamp_config);
u8 CombinePlcBottomFrame(PlcFrame_S *frame,u8 *outbuf);
EXECUTE_STATE_E SendPlcFrameToDeviceAndWaitResponse(PlcFrame_S in_frame);
void AnalysisPlcUserFrame(PlcFrame_S *user_frame);
PlcFrame_S* RecvAndAnalysisPlcBottomFrame(u8 mode);


void PlcTransparentTransmission(PlcFrame_S *user_frame);
void PlcQueryLampState(PlcFrame_S *user_frame);
void PlcReportLampAlarm(PlcFrame_S *user_frame,u8 record_type);
void PlcGetLampAlarmConfig(PlcFrame_S *user_frame);

void PlcGetLampBasicConfig(PlcFrame_S *user_frame);
void PlcGetLampAppointment(PlcFrame_S *user_frame);

void PlcGetLampFrameWareVersion(PlcFrame_S *user_frame);



































#endif
