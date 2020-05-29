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

u16 Usart5RxCnt = 0;				//串口1当前收到的字节数（计数）
u16 Usart5FrameLen = 0;				//串口1收到的报文的长度
u8 Usart5RxBuf[USART5_MAX_RX_LN];	//串口1接收缓存
u8 Usart5RecvEnd = 0;				//一帧数据接收完成标志



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
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
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

	USART_Cmd(UART5, ENABLE);  //使能串口5
	USART_ClearFlag(UART5, USART_FLAG_TC);
	USART_ClearFlag(UART5, USART_FLAG_TXE);

	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启相关中断
	USART_ITConfig(UART5, USART_IT_IDLE,ENABLE);//开启空闲中断
	
	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;//抢占优先级3
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

void UART5_IRQHandler(void)                	//串口1中断服务程序
{
	u8  rxdata;
	
	if(USART_GetITStatus(UART5,USART_IT_RXNE)!=RESET)//接收中断标志位
	{
		USART_ClearFlag(UART5,USART_FLAG_RXNE);
		
		rxdata = USART_ReceiveData(UART5);
		
		if(Usart5RxCnt < USART5_MAX_RX_LN - 1) 
		{
			Usart5RxBuf[Usart5RxCnt++] = rxdata;
		}	
	}
	if(USART_GetITStatus(UART5,USART_IT_IDLE)!=RESET)//空闲帧标志位
	{
		USART_ClearITPendingBit(UART5, USART_IT_IDLE);         //清除中断标志
		
		rxdata = UART5->SR;
		rxdata = USART_ReceiveData(UART5);
		
		USART_ClearFlag(UART5,USART_FLAG_IDLE);//清楚空闲标志位
		
		Usart5FrameLen = Usart5RxCnt;
		
		Usart5RxCnt = 0;
		
		Usart5RecvEnd = 0xAA;
	}
}

//定时器2初始化，主要用于监测串口是否接收完一帧数据
void TIM2_Init(u16 TIM2_Interval_xus)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;//设置中断优先级
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





