#include "sys.h"
#include "usart.h"
#include "string.h"
#include "FreeRTOS.h"					//FreeRTOS使用
#include "common.h"
#include "rx8010s.h"

#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
	int handle;
};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
	x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕
	USART1->DR = (u8) ch;
	return ch;
}
#endif


u16 Usart1RxCnt = 0;				//串口1当前收到的字节数（计数）
u16 Usart1FrameLen = 0;				//串口1收到的报文的长度
u8 Usart1RxBuf[USART1_MAX_RX_LN];	//串口1接收缓存
u8 Usart1RecvEnd = 0;				//一帧数据接收完成标志

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



//串口1初始化
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

	/*配置USART1的发送管脚TXD(PA9)为复用推挽输出*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;                                       //设置初始化GPIO为PIN9
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;                               //设置GPIO的速度为50MHz
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;                                    //设置GPIO模式为复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;								  //设置GPIO输出类型为推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						          //设置GPIO为连接上拉电阻
	GPIO_Init(GPIOA,&GPIO_InitStructure);                                         //初始化GPIOA的PIN9
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;                                      //设置初始化GPIO为PIN10
	/*配置USART1的接收管脚RXD(PA10)为浮空输入*/
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;                                    //设置GPIO的模式为复用模式
	GPIO_Init(GPIOA,&GPIO_InitStructure);                                         //初始化GPIOA的PIN10

    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate =BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_IDLE,ENABLE);	//开启空闲中断

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
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOD时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟

	//串口2对应引脚复用映射
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); 

	//USART2端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); 

	//USART2 初始化设置
	USART_InitStructure.USART_BaudRate = BaudRate;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口1
	
	//DMA1通道4配置
	DMA_DeInit(DMA1_Stream5);

	while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE);

	DMA_InitStructure.DMA_Channel = DMA_Channel_4;

	//外设地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);
	//内存地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Usart2RxBuf;
	//dma传输方向单向
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	//设置DMA在传输时缓冲区的长度
	DMA_InitStructure.DMA_BufferSize = USART2_MAX_RX_LN;
	//设置DMA的外设递增模式，一个外设
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//设置DMA的内存递增模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//外设数据字长
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	//内存数据字长
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	//设置DMA的传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//设置DMA的优先级别
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;

	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输

	DMA_Init(DMA1_Stream5,&DMA_InitStructure);

	DMA_Cmd(DMA1_Stream5,ENABLE);
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);

	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//开启空闲中断

	USART_Cmd(USART2, ENABLE);  //使能串口2
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ClearFlag(USART2, USART_FLAG_TXE);

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、	
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
	
	//DMA1通道4配置
	DMA_DeInit(DMA1_Stream2);

	while (DMA_GetCmdStatus(DMA1_Stream2) != DISABLE);

	DMA_InitStructure.DMA_Channel = DMA_Channel_4;

	//外设地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&UART4->DR);
	//内存地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Usart4RxBuf;
	//dma传输方向单向
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	//设置DMA在传输时缓冲区的长度
	DMA_InitStructure.DMA_BufferSize = USART4_MAX_RX_LN;
	//设置DMA的外设递增模式，一个外设
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//设置DMA的内存递增模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//外设数据字长
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	//内存数据字长
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	//设置DMA的传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//设置DMA的优先级别
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;

	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输

	DMA_Init(DMA1_Stream2,&DMA_InitStructure);

	DMA_Cmd(DMA1_Stream2,ENABLE);
	USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE);

	USART_ITConfig(UART4,USART_IT_IDLE,ENABLE);//开启空闲中断

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
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	
	USART_DeInit(UART5);
	
	USART_Cmd(UART5, DISABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);//使能UART5时钟

	//串口2对应引脚复用映射
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5); 

	//USART5 TX端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	//USART5 RX端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); 	

	//USART2 初始化设置
	USART_InitStructure.USART_BaudRate = BaudRate;//波特率设置
	if(check_mode == USART_Parity_No)
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	}
	else if(check_mode == USART_Parity_Even)
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;//字长为8位数据格式
	}
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = check_mode;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(UART5, &USART_InitStructure); //初始化串口5
	
	//方向控制管脚初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	RS485_DIR = RS485_RX;
	
	//DMA1通道4配置
	DMA_DeInit(DMA1_Stream0);

	while (DMA_GetCmdStatus(DMA1_Stream0) != DISABLE);

	DMA_InitStructure.DMA_Channel = DMA_Channel_4;

	//外设地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&UART5->DR);
	//内存地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Usart5RxBuf;
	//dma传输方向单向
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	//设置DMA在传输时缓冲区的长度
	DMA_InitStructure.DMA_BufferSize = USART5_MAX_RX_LN;
	//设置DMA的外设递增模式，一个外设
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//设置DMA的内存递增模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//外设数据字长
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	//内存数据字长
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	//设置DMA的传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//设置DMA的优先级别
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;

	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输

	DMA_Init(DMA1_Stream0,&DMA_InitStructure);

	DMA_Cmd(DMA1_Stream0,ENABLE);
	USART_DMACmd(UART5,USART_DMAReq_Rx,ENABLE);

	USART_ITConfig(UART5,USART_IT_IDLE,ENABLE);//开启空闲中断
	
	USART_Cmd(UART5, ENABLE);  //使能串口5
	USART_ClearFlag(UART5, USART_FLAG_TC);
	USART_ClearFlag(UART5, USART_FLAG_TXE);
	
	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、	
}

//用查询的方式发送数据（暂时没用上）
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

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8  rxdata;
	
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)//接收中断标志位
	{
		USART_ClearFlag(USART1,USART_FLAG_RXNE);
		
		rxdata = USART_ReceiveData(USART1);
		
		if(Usart1RxCnt < USART1_MAX_RX_LN - 1) 
		{
			Usart1RxBuf[Usart1RxCnt++] = rxdata;
		}	
	}
	if(USART_GetITStatus(USART1,USART_IT_IDLE)!=RESET)//空闲帧标志位
	{
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);         //清除中断标志
		
		rxdata = USART1->SR;
		rxdata = USART_ReceiveData(USART1);
		
		USART_ClearFlag(USART1,USART_FLAG_IDLE);//清楚空闲标志位
		
		Usart1FrameLen = Usart1RxCnt;
		
		Usart1RxCnt = 0;
		
		Usart1RecvEnd = 0xAA;
	}
}

void USART2_IRQHandler(void)                	//串口1中断服务程序
{
	u8 data;
	
	if(USART2->SR & USART_FLAG_IDLE)
	{
		data = USART2->SR;
		data = USART2->DR;
		
		data = data;

		DMA_Cmd(DMA1_Stream5, DISABLE);  //先停止DMA才行设置缓冲区大小
		Usart2FrameLen = USART2_MAX_RX_LN - DMA_GetCurrDataCounter(DMA1_Stream5);
		
		Usart2RecvEnd = 0xAA;
	}

	DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5 | DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5);//清除DMA2_Steam7传输完成标志
	DMA_SetCurrDataCounter(DMA1_Stream5,USART2_MAX_RX_LN);   //重新设置DMA的读取缓冲区长度
	DMA_Cmd(DMA1_Stream5, ENABLE);  //开启DMA
}

void UART4_IRQHandler(void)                	//串口1中断服务程序
{
	u8 data;
	
	if(UART4->SR & USART_FLAG_IDLE)
	{
		data = UART4->SR;
		data = UART4->DR;
		
		data = data;

		DMA_Cmd(DMA1_Stream2, DISABLE);  //先停止DMA才行设置缓冲区大小
		Usart4FrameLen = USART4_MAX_RX_LN - DMA_GetCurrDataCounter(DMA1_Stream2);
		
		Usart4RecvEnd = 0xAA;
	}

	DMA_ClearFlag(DMA1_Stream2,DMA_FLAG_TCIF2 | DMA_FLAG_FEIF2 | DMA_FLAG_DMEIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_HTIF2);//清除DMA2_Steam7传输完成标志
	DMA_SetCurrDataCounter(DMA1_Stream2,USART4_MAX_RX_LN);   //重新设置DMA的读取缓冲区长度
	DMA_Cmd(DMA1_Stream2, ENABLE);  //开启DMA
}

void UART5_IRQHandler(void)                	//串口1中断服务程序
{
	u8 data;
	
	if(UART5->SR & USART_FLAG_IDLE)
	{
		data = UART5->SR;
		data = UART5->DR;
		
		data = data;

		DMA_Cmd(DMA1_Stream0, DISABLE);  //先停止DMA才行设置缓冲区大小
		Usart5FrameLen = USART5_MAX_RX_LN - DMA_GetCurrDataCounter(DMA1_Stream0);
		
		Usart5RecvEnd = 0xAA;
	}

	DMA_ClearFlag(DMA1_Stream0,DMA_FLAG_TCIF0 | DMA_FLAG_FEIF0 | DMA_FLAG_DMEIF0 | DMA_FLAG_TEIF0 | DMA_FLAG_HTIF0);//清除DMA2_Steam7传输完成标志
	DMA_SetCurrDataCounter(DMA1_Stream0,USART5_MAX_RX_LN);   //重新设置DMA的读取缓冲区长度
	DMA_Cmd(DMA1_Stream0, ENABLE);  //开启DMA
}

//定时器2初始化，主要用于监测串口是否接收完一帧数据
void TIM2_Init(u16 TIM2_Interval_xus)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;//设置中断优先级
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);	
	  
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//外设时钟使能  				
  	TIM_TimeBaseStructure.TIM_Period = 8400 - 1;
  	TIM_TimeBaseStructure.TIM_Prescaler = TIM2_Interval_xus-1;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);  		
}

//定时器2中断入口
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





