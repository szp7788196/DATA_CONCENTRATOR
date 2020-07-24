#ifndef __TASK_PLC_H
#define __TASK_PLC_H

#include "sys.h"
#include "rtos_task.h"
#include "server_protocol.h"
#include "lamp_conf.h"

typedef enum
{
	STATE_START 	= 0,
	STATE_STOP		= 1,
	STATE_SUSPEND	= 2,
	STATE_SUCCESS 	= 3,
	STATE_FAIL 		= 4,
	STATE_TIMEOUT 	= 5,
	STATE_FINISHED 	= 6,

} EXECUTE_STATE_E;

typedef struct	LampPlcExecuteTask
{
	u8 notify_enable;							//完成后是否需要通知服务器
	u16 cmd_code;								//命令ID
	EXECUTE_STATE_E state;						//执行状态
	u16 dev_num;								//设备数量
	u16 execute_total_num;						//执行数
	u16 executed_num;							//已执行数
	u16 executed_index;							//执行索引号
	u8 broadcast_state;							//广播完成状态
	u8 broadcast_type;							//播送方式 0广播 1组播 2单播
	u8 execute_type;							//执行方式 0广播通讯 1逐个通讯 2逐个通讯并返回结果
	u8 group_num;								//组数量
	u16 group_dev_id[MAX_LAMP_GROUP_NUM + 1];	//组号或设备号
	u8 content_type;							//内容类别
	void *data;									//发送数据
	u8 data_len;								//数据长度
	u16 success_num;							//成功数
	u16 failed_num;								//失败数
	u16 timeout_num;							//超时数
	u16 spent_time;								//耗时 秒
	
}__attribute__((packed))LampPlcExecuteTask_S;

typedef struct	LampPlcExecuteTaskState
{
	u16 cmd_code;								//命令ID
	u8 state;									//执行状态 0继续执行 1停止执行 2暂停执行

}__attribute__((packed))LampPlcExecuteTaskState_S;




typedef struct	PlcFrame
{
	u8 type;								//播送类型  0：广播 1：单播
	u16 address;							//设备地址
	u8 cmd_code;							//命令字
	u8 group_num;							//组数量
	u8 group_id[MAX_LAMP_GROUP_NUM + 1];	//组号
	u8 wait_ack;							//等待返回
	u8 resp_ack;							//返回ACK
	u16 len;								//数据长度
	u8 *buf;								//数据
}__attribute__((packed))PlcFrame_S;







extern TaskHandle_t xHandleTaskPLC;



void vTaskPLC(void *pvParameters);

void RecvAndExecuteLampPlcExecuteTask(void);
u8 CombinePlcUserFrame(LampPlcExecuteTask_S *task,PlcFrame_S *frame);
u8 CombinePlcBottomFrame(PlcFrame_S *frame,u8 *outbuf);
EXECUTE_STATE_E SendPlcFrameToDeviceAndWaitResponse(PlcFrame_S in_frame);
void AnalysisPlcUserFrame(PlcFrame_S *user_frame);
PlcFrame_S* RecvAndAnalysisPlcBottomFrame(u8 mode);

u8 CopyLampPlcExecuteTask(LampPlcExecuteTask_S *s_task,LampPlcExecuteTask_S *d_task);
void DeletePlcFrame(PlcFrame_S *plc_frame);
void DeleteLampPlcExecuteTask(LampPlcExecuteTask_S *task);




























#endif
