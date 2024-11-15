#include "uart5.h"
#include "sys.h"
////////////////////////////////////////////////////////////////////////////////// 	 

//初始化IO 串口5
//bound:波特率	  
void uart5_init(u32 bound)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOD时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOC时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE); //使能UART5时钟

	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5); //GPIOD2复用为UART5 RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //GPIOD2初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化GPIOD2
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5); //GPIOC12复用为UART5 TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //GPIOD2初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化GPIOC12	

	
	USART_InitStructure.USART_BaudRate = bound;//波特率 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(UART5, &USART_InitStructure); //初始化串口3 
	USART_Cmd(UART5, ENABLE);               //使能串口 

}


////=======================================
////串口5中断服务程序
////=======================================
//uint8_t Recv_uart5[128];//串口接收缓存
//u8 rx5_cnt;//接收数据个数计数变量
//u8 isReceiveUart5Cmd = 0;
//u8 rx5_flag=0;

//void UART5_IRQHandler(void)                	
//{
//	uint8_t data;//接收数据暂存变量

//	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //接收中断
//	{
//		data = USART_ReceiveData(UART5);   			
//		Recv_uart5[rx5_cnt++]=data;//接收的数据存入接收数组 
//		rx5_flag = 1;
//		USART_ClearITPendingBit(UART5,USART_IT_RXNE);
//	} 
//	
//	if(USART_GetITStatus(UART5, USART_IT_IDLE) != RESET)//空闲中断
//	{
//		isReceiveUart5Cmd=1;
//		data = USART1->SR;//串口空闲中断的中断标志只能通过先读SR寄存器，再读DR寄存器清除！
//		data = USART1->DR;		
//	}
//} 

//uart5发送数据
void uart5_send(u8*data, u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		while(USART_GetFlagStatus(UART5,USART_FLAG_TC)==RESET); 
		USART_SendData(UART5,data[i]);   
	}
}

