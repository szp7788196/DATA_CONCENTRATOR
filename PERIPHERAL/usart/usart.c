#include "sys.h"
#include "usart.h"
#include "string.h"
#include "FreeRTOS.h"					//FreeRTOSʹ��
#include "common.h"

#if 1
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
	int handle;
};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
	x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������
	USART1->DR = (u8) ch;
	return ch;
}
#endif


u16 Usart1RxCnt = 0;				//����1��ǰ�յ����ֽ�����������
u16 OldUsart1RxCnt = 0;				//����1�ϴ��յ����ֽ��������������ڶ�֡
u16 Usart1FrameLen = 0;				//����1�յ��ı��ĵĳ���
u8 Usart1RxBuf[Usart1RxLen];		//����1���ջ���
u8 Usart1TxBuf[Usart1TxLen];		//����1���ͻ���
u8 Usart1RecvEnd = 0;				//һ֡���ݽ�����ɱ�־
u8 Usart1Busy = 0;					//����æ��־
u16 Usart1SendLen = 0;				//��Ҫ���͵����ֽ���
u16 Usart1SendNum = 0;				//����1��ǰ�������ֽ�����������
u8 Usart1RecvCnt = 0;				//�����յ����ֽ���������ָʾ����ʱ
u8 Usart1SendCnt = 0;				//���ڷ��͵��ֽ���������ָʾ����ʱ


//����1��ʼ��
void usart1_config(u32 BaudRate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

	USART_Cmd(USART1, DISABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	/*����USART1�ķ��͹ܽ�TXD(PA9)Ϊ�����������*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;                                       //���ó�ʼ��GPIOΪPIN9
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;                               //����GPIO���ٶ�Ϊ50MHz
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;                                    //����GPIOģʽΪ����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;								  //����GPIO�������Ϊ�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						          //����GPIOΪ������������
	GPIO_Init(GPIOA,&GPIO_InitStructure);                                         //��ʼ��GPIOA��PIN9
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;                                      //���ó�ʼ��GPIOΪPIN10
	/*����USART1�Ľ��չܽ�RXD(PA10)Ϊ��������*/
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;                                    //����GPIO��ģʽΪ����ģʽ
	GPIO_Init(GPIOA,&GPIO_InitStructure);                                         //��ʼ��GPIOA��PIN10

    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate =BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    USART_Cmd(USART1, ENABLE);
    USART_ClearFlag(USART1, USART_FLAG_TC);
}

//�ò�ѯ�ķ�ʽ�������ݣ���ʱû���ϣ�
u8 UsartSendString(USART_TypeDef* USARTx,u8 *str, u16 len)
{
	u16 i;
	for(i=0; i<len; i++)
    {
		USART_ClearFlag(USARTx,USART_FLAG_TC);
		USART_SendData(USARTx, str[i]);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
		USART_ClearFlag(USARTx,USART_FLAG_TC);
	}
	return 1;
}

//����1��֡�ж�
void Usart1ReciveFrameEnd(void)
{
	if(Usart1RxCnt)
	{
		if(OldUsart1RxCnt == Usart1RxCnt)
		{
			Usart1FrameLen = Usart1RxCnt;
			Usart1RecvCnt = Usart1RxCnt;
			OldUsart1RxCnt = 0;
			Usart1RxCnt = 0;
			Usart1RecvEnd = 0xAA;
		}
		else
		{
			OldUsart1RxCnt = Usart1RxCnt;
		}
	}
}

//����1�жϷ�ʽ��������
void Usart1FrameSend(void)
{
	u8 send_data = 0;
	
	if(Usart1SendNum == 0)
	{
		Usart1SendCnt = Usart1SendLen;
	}
	
	send_data = Usart1TxBuf[Usart1SendNum];
	USART_SendData(USART1,send_data);
	Usart1SendNum ++;
	if(Usart1SendNum >= Usart1SendLen)
	{
		Usart1Busy = 0;
		Usart1SendLen = 0;
		Usart1SendNum = 0;
		USART_ITConfig(USART1, USART_IT_TC, DISABLE);
		memset(Usart1TxBuf,0,Usart1TxLen);
	}
}

//����1�ж����
void USART1_IRQHandler(void)
{
	u8 rxdata;

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  	{
		rxdata =USART_ReceiveData(USART1);

		if(Usart1RxCnt<Usart1RxLen && Usart1Busy == 0)
		{
			Usart1RxBuf[Usart1RxCnt]=rxdata;
			Usart1RxCnt++;
		}
  	}

	if(USART_GetITStatus(USART1,USART_IT_TC)!=RESET)
	{
		Usart1FrameSend();
	}

	else if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
	{
		rxdata = USART_ReceiveData(USART1);
		rxdata = rxdata;
		USART_ClearFlag(USART1, USART_FLAG_ORE);
	}
	else if(USART_GetFlagStatus(USART1, USART_FLAG_NE) != RESET)
	{
		USART_ClearFlag(USART1, USART_FLAG_NE);
	}
	else if(USART_GetFlagStatus(USART1, USART_FLAG_FE) != RESET)
	{
		USART_ClearFlag(USART1, USART_FLAG_FE);
	}
	else if(USART_GetFlagStatus(USART1, USART_FLAG_PE) != RESET)
	{
		USART_ClearFlag(USART1, USART_FLAG_PE);
	}
}

//��ʱ��2��ʼ������Ҫ���ڼ�⴮���Ƿ������һ֡����
void TIM2_Config(u16 TIM2_Interval_xus)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;//�����ж����ȼ�
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);	
	  
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//����ʱ��ʹ��  				
  	TIM_TimeBaseStructure.TIM_Period = 8400 - 1;
  	TIM_TimeBaseStructure.TIM_Prescaler = TIM2_Interval_xus-1;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);  		
}

//��ʱ��2�ж����
void TIM2_IRQHandler(void)
{
	static u16 cnt = 0;
	
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) 
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); 
		
		Usart1ReciveFrameEnd();
		
		if((cnt ++) >= 100)
		{
			cnt = 0;
			
			SysTick1s ++;
		}
	}
}





