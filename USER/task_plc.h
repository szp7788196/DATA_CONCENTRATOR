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
	u8 notify_enable;							//��ɺ��Ƿ���Ҫ֪ͨ������
	u16 cmd_code;								//����ID
	EXECUTE_STATE_E state;						//ִ��״̬
	u16 dev_num;								//�豸����
	u16 execute_total_num;						//ִ����
	u16 executed_num;							//��ִ����
	u16 executed_index;							//ִ��������
	u8 broadcast_state;							//�㲥���״̬
	u8 broadcast_type;							//���ͷ�ʽ 0�㲥 1�鲥 2����
	u8 execute_type;							//ִ�з�ʽ 0�㲥ͨѶ 1���ͨѶ 2���ͨѶ�����ؽ��
	u8 group_num;								//������
	u16 group_dev_id[MAX_LAMP_GROUP_NUM + 1];	//��Ż��豸��
	u8 content_type;							//�������
	void *data;									//��������
	u8 data_len;								//���ݳ���
	u16 success_num;							//�ɹ���
	u16 failed_num;								//ʧ����
	u16 timeout_num;							//��ʱ��
	u16 spent_time;								//��ʱ ��
	
}__attribute__((packed))LampPlcExecuteTask_S;

typedef struct	LampPlcExecuteTaskState
{
	u16 cmd_code;								//����ID
	u8 state;									//ִ��״̬ 0����ִ�� 1ִֹͣ�� 2��ִͣ��

}__attribute__((packed))LampPlcExecuteTaskState_S;




typedef struct	PlcFrame
{
	u8 type;								//��������  0���㲥 1������
	u16 address;							//�豸��ַ
	u8 cmd_code;							//������
	u8 group_num;							//������
	u8 group_id[MAX_LAMP_GROUP_NUM + 1];	//���
	u8 wait_ack;							//�ȴ�����
	u8 resp_ack;							//����ACK
	u16 len;								//���ݳ���
	u8 *buf;								//����
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
