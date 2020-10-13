#include "sys.h"
#include "usart.h"
#include "string.h"
#include "FreeRTOS.h"					//FreeRTOSʹ��
#include "common.h"
#include "rx8010s.h"

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
u16 Usart1FrameLen = 0;				//����1�յ��ı��ĵĳ���
u8 Usart1RxBuf[USART1_MAX_RX_LN];	//����1���ջ���
u8 Usart1RecvEnd = 0;				//һ֡���ݽ�����ɱ�־

u16 Usart2RxCnt = 0;
u16 Usart2FrameLen = 0;
u8 Usart2RxBuf[USART2_MAX_RX_LN];
u8 Usart2RecvEnd = 0;

u16 Usart4RxCnt = 0;
u16 Usart4FrameLen = 0;
u8 Usart4RxBuf[USART4_MAX_RX_LN];
u8 Usart4RecvEnd = 0;

u16 Usart5RxCnt = 0;
u16 Usart5FrameLen = 0;
u8 Usart5RxBuf[USART5_MAX_RX_LN];
u8 Usart5RecvEnd = 0;



//����1��ʼ��
void USART1_Init(u32 BaudRate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

	USART_DeInit(USART1);
	
	USART_Cmd(USART1, DISABLE);

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
	USART_ITConfig(USART1, USART_IT_IDLE,ENABLE);	//���������ж�

    USART_Cmd(USART1, ENABLE);
    USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_ClearFlag(USART1, USART_FLAG_TXE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void USART2_Init(u32 BaudRate)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIODʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��

	//����2��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); 

	//USART2�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOD,&GPIO_InitStructure); 

	//USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = BaudRate;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������1
	
	//DMA1ͨ��4����
	DMA_DeInit(DMA1_Stream5);

	while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE);

	DMA_InitStructure.DMA_Channel = DMA_Channel_4;

	//�����ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);
	//�ڴ��ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Usart2RxBuf;
	//dma���䷽����
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	//����DMA�ڴ���ʱ�������ĳ���
	DMA_InitStructure.DMA_BufferSize = USART2_MAX_RX_LN;
	//����DMA���������ģʽ��һ������
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//����DMA���ڴ����ģʽ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//���������ֳ�
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	//�ڴ������ֳ�
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	//����DMA�Ĵ���ģʽ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//����DMA�����ȼ���
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;

	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���

	DMA_Init(DMA1_Stream5,&DMA_InitStructure);

	DMA_Cmd(DMA1_Stream5,ENABLE);
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);

	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//���������ж�

	USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���2
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ClearFlag(USART2, USART_FLAG_TXE);

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����	
}

void USART4_Init(u32 BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);
	
	//DMA1ͨ��4����
	DMA_DeInit(DMA1_Stream2);

	while (DMA_GetCmdStatus(DMA1_Stream2) != DISABLE);

	DMA_InitStructure.DMA_Channel = DMA_Channel_4;

	//�����ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&UART4->DR);
	//�ڴ��ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Usart4RxBuf;
	//dma���䷽����
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	//����DMA�ڴ���ʱ�������ĳ���
	DMA_InitStructure.DMA_BufferSize = USART4_MAX_RX_LN;
	//����DMA���������ģʽ��һ������
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//����DMA���ڴ����ģʽ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//���������ֳ�
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	//�ڴ������ֳ�
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	//����DMA�Ĵ���ģʽ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//����DMA�����ȼ���
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;

	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���

	DMA_Init(DMA1_Stream2,&DMA_InitStructure);

	DMA_Cmd(DMA1_Stream2,ENABLE);
	USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE);

	USART_ITConfig(UART4,USART_IT_IDLE,ENABLE);//���������ж�

	USART_Cmd(UART4, ENABLE);
	USART_ClearFlag(UART4, USART_FLAG_TC);
	USART_ClearFlag(UART4, USART_FLAG_TXE);
	
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void USART5_Init(u32 BaudRate,u16 check_mode)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	
	USART_DeInit(UART5);
	
	USART_Cmd(UART5, DISABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);//ʹ��UART5ʱ��

	//����2��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5); 

	//USART5 TX�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	//USART5 RX�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOD,&GPIO_InitStructure); 	

	//USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = BaudRate;//����������
	if(check_mode == USART_Parity_No)
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	}
	else if(check_mode == USART_Parity_Even)
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;//�ֳ�Ϊ8λ���ݸ�ʽ
	}
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = check_mode;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(UART5, &USART_InitStructure); //��ʼ������5
	
	//������ƹܽų�ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	RS485_DIR = RS485_RX;
	
	//DMA1ͨ��4����
	DMA_DeInit(DMA1_Stream0);

	while (DMA_GetCmdStatus(DMA1_Stream0) != DISABLE);

	DMA_InitStructure.DMA_Channel = DMA_Channel_4;

	//�����ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&UART5->DR);
	//�ڴ��ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Usart5RxBuf;
	//dma���䷽����
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	//����DMA�ڴ���ʱ�������ĳ���
	DMA_InitStructure.DMA_BufferSize = USART5_MAX_RX_LN;
	//����DMA���������ģʽ��һ������
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//����DMA���ڴ����ģʽ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//���������ֳ�
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	//�ڴ������ֳ�
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	//����DMA�Ĵ���ģʽ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//����DMA�����ȼ���
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;

	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���

	DMA_Init(DMA1_Stream0,&DMA_InitStructure);

	DMA_Cmd(DMA1_Stream0,ENABLE);
	USART_DMACmd(UART5,USART_DMAReq_Rx,ENABLE);

	USART_ITConfig(UART5,USART_IT_IDLE,ENABLE);//���������ж�
	
	USART_Cmd(UART5, ENABLE);  //ʹ�ܴ���5
	USART_ClearFlag(UART5, USART_FLAG_TC);
	USART_ClearFlag(UART5, USART_FLAG_TXE);
	
	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����	
}

//�ò�ѯ�ķ�ʽ�������ݣ���ʱû���ϣ�
u8 UsartSendString(USART_TypeDef* USARTx,u8 *str, u16 len)
{
	u16 i;
	
	if(USARTx == UART5)
	{
		RS485_DIR = RS485_TX;
	}
	
	for(i = 0; i < len; i++)
    {
		USART_ClearFlag(USARTx,USART_FLAG_TC);
		
		USART_SendData(USARTx, str[i]);
		
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
		
		USART_ClearFlag(USARTx,USART_FLAG_TC);
	}
	
	if(USARTx == UART5)
	{
		RS485_DIR = RS485_RX;
	}
	
	return 1;
}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8  rxdata;
	
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)//�����жϱ�־λ
	{
		USART_ClearFlag(USART1,USART_FLAG_RXNE);
		
		rxdata = USART_ReceiveData(USART1);
		
		if(Usart1RxCnt < USART1_MAX_RX_LN - 1) 
		{
			Usart1RxBuf[Usart1RxCnt++] = rxdata;
		}	
	}
	if(USART_GetITStatus(USART1,USART_IT_IDLE)!=RESET)//����֡��־λ
	{
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);         //����жϱ�־
		
		rxdata = USART1->SR;
		rxdata = USART_ReceiveData(USART1);
		
		USART_ClearFlag(USART1,USART_FLAG_IDLE);//������б�־λ
		
		Usart1FrameLen = Usart1RxCnt;
		
		Usart1RxCnt = 0;
		
		Usart1RecvEnd = 0xAA;
	}
}

void USART2_IRQHandler(void)                	//����1�жϷ������
{
	u8 data;
	
	if(USART2->SR & USART_FLAG_IDLE)
	{
		data = USART2->SR;
		data = USART2->DR;
		
		data = data;

		DMA_Cmd(DMA1_Stream5, DISABLE);  //��ֹͣDMA�������û�������С
		Usart2FrameLen = USART2_MAX_RX_LN - DMA_GetCurrDataCounter(DMA1_Stream5);
		
		Usart2RecvEnd = 0xAA;
	}

	DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5 | DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5);//���DMA2_Steam7������ɱ�־
	DMA_SetCurrDataCounter(DMA1_Stream5,USART2_MAX_RX_LN);   //��������DMA�Ķ�ȡ����������
	DMA_Cmd(DMA1_Stream5, ENABLE);  //����DMA
}

void UART4_IRQHandler(void)                	//����1�жϷ������
{
	u8 data;
	
	if(UART4->SR & USART_FLAG_IDLE)
	{
		data = UART4->SR;
		data = UART4->DR;
		
		data = data;

		DMA_Cmd(DMA1_Stream2, DISABLE);  //��ֹͣDMA�������û�������С
		Usart4FrameLen = USART4_MAX_RX_LN - DMA_GetCurrDataCounter(DMA1_Stream2);
		
		Usart4RecvEnd = 0xAA;
	}

	DMA_ClearFlag(DMA1_Stream2,DMA_FLAG_TCIF2 | DMA_FLAG_FEIF2 | DMA_FLAG_DMEIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_HTIF2);//���DMA2_Steam7������ɱ�־
	DMA_SetCurrDataCounter(DMA1_Stream2,USART4_MAX_RX_LN);   //��������DMA�Ķ�ȡ����������
	DMA_Cmd(DMA1_Stream2, ENABLE);  //����DMA
}

void UART5_IRQHandler(void)                	//����1�жϷ������
{
	u8 data;
	
	if(UART5->SR & USART_FLAG_IDLE)
	{
		data = UART5->SR;
		data = UART5->DR;
		
		data = data;

		DMA_Cmd(DMA1_Stream0, DISABLE);  //��ֹͣDMA�������û�������С
		Usart5FrameLen = USART5_MAX_RX_LN - DMA_GetCurrDataCounter(DMA1_Stream0);
		
		Usart5RecvEnd = 0xAA;
	}

	DMA_ClearFlag(DMA1_Stream0,DMA_FLAG_TCIF0 | DMA_FLAG_FEIF0 | DMA_FLAG_DMEIF0 | DMA_FLAG_TEIF0 | DMA_FLAG_HTIF0);//���DMA2_Steam7������ɱ�־
	DMA_SetCurrDataCounter(DMA1_Stream0,USART5_MAX_RX_LN);   //��������DMA�Ķ�ȡ����������
	DMA_Cmd(DMA1_Stream0, ENABLE);  //����DMA
}

//��ʱ��2��ʼ������Ҫ���ڼ�⴮���Ƿ������һ֡����
void TIM2_Init(u16 TIM2_Interval_xus)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;//�����ж����ȼ�
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
		
		SysTick10ms ++;
		
		if((cnt ++) >= 100)
		{
			cnt = 0;
			
			SysTick1s ++;
		}
	}
}





