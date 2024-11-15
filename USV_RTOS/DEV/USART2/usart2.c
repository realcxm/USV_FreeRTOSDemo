#include "usart2.h"
#include "sys.h"
#include "Usartlin.h"

/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

uint8_t Recv2[128];//串口接收缓存
u8 rx2_cnt;//接收数据个数计数变量
u8 isReceiveUart2Cmd = 0;
void USART2_IRQHandler(void)                	
{
	uint8_t data;//接收数据暂存变量

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
	{
		data = USART_ReceiveData(USART2);   			

		revSolv_handle(&data);

		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	} 
	
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)//空闲中断
	{
		isReceiveUart2Cmd=1;
		data = USART2->SR;//串口空闲中断的中断标志只能通过先读SR寄存器，再读DR寄存器清除！
		data = USART2->DR;
		
	}
} 


void usart2_init(u32 bound)
{  
NVIC_InitTypeDef NVIC_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;

//1.串口时钟和和GPIO时钟使能。
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
//2.设置引脚复用器映射 
GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2复用为USART2
GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3复用为USART2  
//3.GPIO端口模式设置
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2和GPIOA3初始化
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //速度50MHz
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化GPIOA2，和GPIOA3
//4.串口参数初始化：设置波特率，字长，奇偶校验等参数
USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为115200;
USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式
USART_Init(USART2, &USART_InitStructure); //初始化串口2参数

//5.使能串口
USART_Cmd(USART2, ENABLE);                    //使能串口 

#if EN_USART2_RX	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启相关中断
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);

	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

#endif

}


void usart2_send(u8*data, u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); 
		USART_SendData(USART2,data[i]);   
	}
}

