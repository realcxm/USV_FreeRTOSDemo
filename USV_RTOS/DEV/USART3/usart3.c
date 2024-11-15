#include "usart3.h"
#include "sys.h"
#include "Usartlin.h"


/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"      //FreeRTOS 使用  
#endif

//初始化IO 串口3
//bound:波特率	  
void usart3_init(u32 bound)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOD时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟

 	USART_DeInit(USART3);  //复位串口3
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3); //GPIOD8复用为USART3
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3); //GPIOD9复用为USART3	
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; //GPIOD8和GPIOD9初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化GPIOD8，和GPIOD9
	
	USART_InitStructure.USART_BaudRate = bound;//波特率 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  
	USART_Init(USART3, &USART_InitStructure); //初始化串口3 
	USART_Cmd(USART3, ENABLE);               //使能串口 
//	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);  //使能DMA接收	
	
//  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断   
#if EN_USART3_RX	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启相关中断
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);

	//Usart3 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6;//抢占优先级6
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

#endif
}


uint8_t Recv3[USART3_MAX_RECV_LEN];//串口接收缓存
u8 rx3_cnt;//接收数据个数计数变量
u8 isReceiveUart3Cmd = 0;
// 声明信号量
extern SemaphoreHandle_t xSemaphore;
void USART3_IRQHandler(void)                	
{
	uint8_t data;//接收数据暂存变量

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
	{
		data = USART_ReceiveData(USART3);   			
//		Recv3[rx3_cnt++]=data;//接收的数据存入接收数组 
//		revSolv_handle(&data);
		
		        // 将数据存入接收缓冲区
        if (rx3_cnt < USART3_MAX_RECV_LEN) {
            Recv3[rx3_cnt++] = data;  // 将数据存入数组并增加计数
        }

		
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	} 
	
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)//空闲中断
	{
		isReceiveUart3Cmd=1;
		data = USART3->SR;//串口空闲中断的中断标志只能通过先读SR寄存器，再读DR寄存器清除！
		data = USART3->DR;
		        // 释放信号量，通知任务进行处理
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
		
		        // 检查是否需要进行任务切换
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        // 清除空闲中断标志
        USART_ClearITPendingBit(USART3, USART_IT_IDLE);
	}
} 

