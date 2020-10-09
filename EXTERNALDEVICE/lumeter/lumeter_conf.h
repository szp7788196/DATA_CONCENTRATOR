#ifndef __LUMETER_CONF_H
#define __LUMETER_CONF_H

#include "sys.h"
#include "common.h"


#define MAX_LUMETER_CONF_NUM					4			//�������4�����ռ�
#define MAX_LUMETER_NAME_LEN					16
#define MAX_LUMETER_STACK_LEN					100



typedef struct	LumeterBasicConfig							//���ռƻ�������
{
	u16 collect_cycle;										//�ɼ�����
	u8 stack_depth;											//��ջ���
	u8 ignore_num;											//��������
	u16 auto_report_cycle;									//�Զ��ϱ�����
	u8 data_source;											//������Դ
	u32 light_on_thre;										//���ƹ���ֵ
	u16 light_on_advance_time;								//��ǰ��Чʱ��
	u16 light_on_delay_time;								//�Ӻ���Чʱ��
	u32 light_off_thre;										//�صƹ���ֵ
	u16 light_off_advance_time;								//��ǰ��Чʱ��
	u16 light_off_delay_time;								//�Ӻ���Чʱ��
	
	u16 crc16;												//У���� �洢��

}__attribute__((packed))LumeterBasicConfig_S;

typedef struct	LumeterConfig								//���ռ�����
{
	u8 address;												//ͨѶ��ַ
	u8 channel;												//ͨѶͨ��
	u8 module[MAX_LUMETER_NAME_LEN];						//ģ���ͺ�
	
	u32 min_valid_value;									//��С��Ч����ֵ
	u32 min_valid_value_range;								//��С�������Ʒ�Χ
	u32 max_valid_value;									//�����Ч����ֵ
	u32 max_valid_value_range;								//��󲨶����Ʒ�Χ
	u8 valid_value_confirm_time;							//ȷ�ϴ���
	
	u32 value_unchanged_range;								//�����ޱ仯����ֵ
	u8 no_response_time;									//�豸����ӦͨѶ����

	u16 crc16;												//У���� �洢��

}__attribute__((packed))LumeterConfig_S;

typedef struct	LumeterAlarmConfig							//���ռƸ澯����
{
	u8 lumeter_thre_over_alarm_enable;						//����Խ��ʹ��
	u8 lumeter_abnormal_alarm_enable;						//�豸�쳣ʹ��

	u16 crc16;												//У���� �洢��

}__attribute__((packed))LumeterAlarmConfig_S;

typedef struct	LumeterState								//���ռ�״̬
{
	u8 address;												//ͨѶ��ַ
	u8 channel;												//ͨѶͨ��
	u8 waitting_resp;										//�ȴ��豸��Ӧ
	u8 no_resp_time;										//����Ӧ����
	u8 collect_time;										//�ɼ�����
	time_t unchange_time;									//���ݲ��仯ʱ��
	u32 ref_value;											//24Сʱ�����׼ֵ
	
	u32 stack[MAX_LUMETER_STACK_LEN];						//��ջ
	
	u32 value;												//��Чֵ

}LumeterState_S;

typedef struct	LumeterCollectState							//���ղɼ���״̬
{
	u8 address;												//ͨѶ��ַ
	u8 channel;												//ͨѶͨ��
	u8 update;												//���±�־
	
	u32 value;												//��Чֵ

}__attribute__((packed))LumeterCollectState_S;




extern u32 LumeterAppValue;											//�����߼�����Ĺ���ֵ
extern u32 LumeterValueFromServer;									//��λ���·����ն�ֵ
extern Uint32TypeNumber_S LumeterConfigNum;							//�նȼ���������
extern LumeterConfig_S LumeterConfig[MAX_LUMETER_CONF_NUM];			//�նȼ�����
extern LumeterBasicConfig_S LumeterBasicConfig;						//�նȼƻ�������
extern LumeterAlarmConfig_S LumeterAlarmConfig;						//�նȼƸ澯��������

extern LumeterState_S LumeterState[MAX_LUMETER_CONF_NUM];			//�նȼƵ�ǰ״̬












void ReadLumeterConfigNum(void);
void WriteLumeterConfigNum(u8 reset,u8 write_enable);
void ReadLumeterBasicConfig(void);
void WriteLumeterBasicConfig(u8 reset,u8 write_enable);
void ReadLumeterAlarmConfig(void);
void WriteLumeterAlarmConfig(u8 reset,u8 write_enable);
void ReadLumeterConfig(void);
void WriteLumeterConfig(u8 i,u8 reset,u8 write_enable);



































#endif
