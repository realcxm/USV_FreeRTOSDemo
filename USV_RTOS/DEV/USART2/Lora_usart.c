#include "lora_usart.h"
#include "Usartlin.h"
#include "sys.h"
#include "string.h"

//DMA缓存长度配置
uint8_t UART2_DMA_Tx_Buff[500]={0};
uint8_t UART2_DMA_Rx_Buff[500]={0};
uint8_t buf[500]={0};
void Receive(uint8_t bytedata);

//void USARTLink_Init(void)
//{
//	NVIC_InitTypeDef NVIC_InitStructure;
//	GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;

//	//1.串口时钟和和GPIO时钟使能。
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
//	//2.设置引脚复用器映射 
//	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2复用为USART2
//	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3复用为USART2  
//	//3.GPIO端口模式设置
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2和GPIOA3初始化
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //速度50MHz
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
//	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化GPIOA2，和GPIOA3
//	//4.串口参数初始化：设置波特率，字长，奇偶校验等参数
//	USART_InitStructure.USART_BaudRate = 115200;//波特率一般设置为115200;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
//	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式
//	USART_Init(USART2, &USART_InitStructure); //初始化串口2参数
//	//5.初始化NVIC
//	

//#if EN_USART2_RX	
//	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启接受中断
//	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //子优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器
////	//6.开启中断
////	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//开启空闲中断  
////	//7.使能串口
////	USART_Cmd(USART2, ENABLE);                    //使能串口 
//#endif
//}


// 
//void uart2_dma_rx_configuration(void)
//{
//	/*定义DMA初始化结构体*/
//	DMA_InitTypeDef  DMA_InitStructure;
//	/*定义中断结构体*/
//	NVIC_InitTypeDef NVIC_InitStructure;
//	/*使能DMA外设时钟*/
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
//	
//	/*UART2 DMA1 Stream5 Channel_4 接收配置*/
//	DMA_DeInit(DMA1_Stream5);
//	DMA_InitStructure.DMA_Channel = DMA_Channel_4;							//DMA通道配置
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);		//DMA外设基地址
//	DMA_InitStructure.DMA_Memory0BaseAddr = (uint8_t)UART2_DMA_Rx_Buff;	    //DMA内存基地址,把接收到的数据放到哪儿
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;					//DMA数据传输方向，从外设读取写入到内存，外设作为数据来源
//	DMA_InitStructure.DMA_BufferSize = UART2_DMA_Rx_LEN;					//DMA通道的缓存的大小，一次接收的最大字节数
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，外设地址寄存器不递增
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//设置DMA的内存递增模式，内存地址寄存器递增
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据字长，数据宽度为8位
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//内存数据字长，数据宽度为8位
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//设置DMA的传输模式，工作在正常模式，即满了不再接收，而不是循环储存
//	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;					//设置DMA的优先级别，非常高的优先级
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;					//指定如果FIFO模式或直接模式将用于指定的流 ： 不使能FIFO模式
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;		//指定了FIFO阈值水平
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;				//指定的Burst转移配置内存传输
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;		//指定的Burst转移配置外围转移
//	DMA_Init(DMA1_Stream5, &DMA_InitStructure);								//配置DMA1的通道
//	
//	/*设置UART2 中断触发模式*/
//	//配置中断  
//	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
//        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;               //通道设置为串口中断  
//        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;       //中断占先等级
//        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;              //中断响应优先级
//        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 //打开中断  
//        NVIC_Init(&NVIC_InitStructure);
//	//中断配置
////	USART_ITConfig(USART2,USART_IT_TC,DISABLE);		// Transmit Complete，发送完成中断。开启串口发送完成中断，发送一个字节就会进入中断，只需要清除中断标志位，不需要关闭中断
////	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);	//RX NO Empty，RX非空，RX有数据中断。开启串口接受中断，使用DMA接收时要失能这个，使能空闲中断
////	USART_ITConfig(USART2,USART_IT_TXE,DISABLE);	//TX Empty，TX为空，发送寄存器DR清零。发送寄存器空闲中断，发送完一个字节后，必须关闭这个中断，否则只要寄存器为空值，就会反复进入中断
//	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);	//开启串口空闲中断
//	
//	/*开启DMA接收*/
//	DMA_Cmd(DMA1_Stream5,ENABLE);							//使能DMA接收通道
//	//USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);			//采用DMA方式发送
//	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);			//采用DMA方式接收
//}



//void uart2_dma_tx_configuration(void)
//{
//	/*定义DMA初始化结构体*/
//	DMA_InitTypeDef  DMA_InitStructure;
//	/*定义中断结构体*/
//	NVIC_InitTypeDef NVIC_InitStructure;
//	/*使能DMA外设时钟*/
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
//	
//	/*UART2 DMA1 Stream6 Channel_4 发送配置*/
//	DMA_DeInit(DMA1_Stream6);
//	DMA_InitStructure.DMA_Channel = DMA_Channel_4;							//DMA通道配置
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);		//DMA外设基地址
//	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)UART2_DMA_Tx_Buff;	//DMA内存基地址,把接收到的数据放到哪儿
//	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;					//DMA数据传输方向，从内存到外设发送，外设作为数据传输目的地
//	DMA_InitStructure.DMA_BufferSize = UART2_DMA_Tx_LEN;					//DMA通道的缓存的大小，一次发送的最大字节数
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//设置DMA的外设递增模式，外设地址寄存器不递增
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//设置DMA的内存递增模式，内存地址寄存器递增
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据字长，数据宽度为8位
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;		//内存数据字长，数据宽度为8位
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//设置DMA的传输模式，工作在正常模式，即满了不再接收，而不是循环储存
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;						//设置DMA的优先级别，非常高的优先级
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;					//指定如果FIFO模式或直接模式将用于指定的流 ： 不使能FIFO模式
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;		//指定了FIFO阈值水平
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;				//指定的Burst转移配置内存传输
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;		//指定的Burst转移配置外围转移      
//	DMA_Init(DMA1_Stream6, &DMA_InitStructure);								//配置DMA1的通道
//	
//	/*设置UART2 中断触发模式*/
//	//DMA发送中断设置
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	//中断配置
//	USART_ITConfig(USART2,USART_IT_TC,DISABLE);		// Transmit Complete，发送完成中断。开启串口发送完成中断，发送一个字节就会进入中断，只需要清除中断标志位，不需要关闭中断
//	USART_ITConfig(USART2,USART_IT_TXE,DISABLE);	//TX Empty，TX为空，发送寄存器DR清零。发送寄存器空闲中断，发送完一个字节后，必须关闭这个中断，否则只要寄存器为空值，就会反复进入中断
//	//使能中断
//	DMA_ITConfig(DMA1_Stream6,DMA_IT_TC,ENABLE);
//	
//	/*开启DMA发送*/
//	DMA_Cmd(DMA1_Stream6,ENABLE);							//使能DMA接收通道
//	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);			//采用DMA方式发送
//}
// 


//uint16_t len = 0;    //本帧接收到的字节长度

//void USART2_IRQHandler(void)
//{
//    //接收完成中断处理
//    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
//    {
//	USART2->SR;
//	USART2->DR; //清USART_IT_IDLE标志
//	//关闭DMA
//	DMA_Cmd(DMA1_Stream5,DISABLE);
//	//清除标志位
//	DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5);
//	//获得接收帧帧长
//	len = UART2_DMA_Rx_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);
//		
//	/* 对接收数据处理 */
//	memset(buf,0,UART2_DMA_Rx_LEN);
//	memcpy(buf,UART2_DMA_Rx_Buff,len);
//    printf("接收到的数据：%d\r\n",buf); 
//	/* 开始下一帧接收准备 */
//	//设置传输数据长度
//	DMA_SetCurrDataCounter(DMA1_Stream5,UART2_DMA_Rx_LEN);
//	//打开DMA
//	DMA_Cmd(DMA1_Stream5,ENABLE);
//    }
//}


/////**
//// * @brief  串口 2 中断服务函数.
//// * @param None
//// * @return None
//// * @note None
//// */
////void USART2_IRQHandler(void)
////{		
////	u8 RX_len=0;
////	if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
////	{
////		USART2->SR;
////		USART2->DR;
////		
////		DMA_Cmd(DMA1_Stream5,DISABLE);; //关闭DMA串口接收传输通道5
////		
////		RX_len = UART2_DMA_Rx_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);//本次接收数据长度总-（总-用）=用
////		for (uint8_t i = 0; i < RX_len; i++) {
////			
////		 Receive(UART2_DMA_Rx_Buff[i]);//解析串口发送来的数据

////		}
////		DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5);            //清除DMA中断标志位
////		DMA_SetCurrDataCounter(DMA1_Stream5,UART2_DMA_Rx_LEN); //设置下次需要接收的字节长度
////		DMA_Cmd(DMA1_Stream5,ENABLE);                          //打开DMA	
////  }
////}

                        

