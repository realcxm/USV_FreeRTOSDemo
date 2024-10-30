#include "lora_usart.h"
#include "Usartlin.h"
#include "sys.h"
#include "string.h"

//DMA���泤������
uint8_t UART2_DMA_Tx_Buff[500]={0};
uint8_t UART2_DMA_Rx_Buff[500]={0};
uint8_t buf[500]={0};
void Receive(uint8_t bytedata);

//void USARTLink_Init(void)
//{
//	NVIC_InitTypeDef NVIC_InitStructure;
//	GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;

//	//1.����ʱ�Ӻͺ�GPIOʱ��ʹ�ܡ�
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
//	//2.�������Ÿ�����ӳ�� 
//	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2����ΪUSART2
//	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3����ΪUSART2  
//	//3.GPIO�˿�ģʽ����
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2��GPIOA3��ʼ��
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //�ٶ�50MHz
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
//	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��GPIOA2����GPIOA3
//	//4.���ڲ�����ʼ�������ò����ʣ��ֳ�����żУ��Ȳ���
//	USART_InitStructure.USART_BaudRate = 115200;//������һ������Ϊ115200;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
//	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
//	USART_Init(USART2, &USART_InitStructure); //��ʼ������2����
//	//5.��ʼ��NVIC
//	

//#if EN_USART2_RX	
//	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���������ж�
//	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //�����ȼ�3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ��ʹ��
//	NVIC_Init(&NVIC_InitStructure); //����ָ���Ĳ�����ʼ��VIC�Ĵ���
////	//6.�����ж�
////	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//���������ж�  
////	//7.ʹ�ܴ���
////	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
//#endif
//}


// 
//void uart2_dma_rx_configuration(void)
//{
//	/*����DMA��ʼ���ṹ��*/
//	DMA_InitTypeDef  DMA_InitStructure;
//	/*�����жϽṹ��*/
//	NVIC_InitTypeDef NVIC_InitStructure;
//	/*ʹ��DMA����ʱ��*/
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
//	
//	/*UART2 DMA1 Stream5 Channel_4 ��������*/
//	DMA_DeInit(DMA1_Stream5);
//	DMA_InitStructure.DMA_Channel = DMA_Channel_4;							//DMAͨ������
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);		//DMA�������ַ
//	DMA_InitStructure.DMA_Memory0BaseAddr = (uint8_t)UART2_DMA_Rx_Buff;	    //DMA�ڴ����ַ,�ѽ��յ������ݷŵ��Ķ�
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;					//DMA���ݴ��䷽�򣬴������ȡд�뵽�ڴ棬������Ϊ������Դ
//	DMA_InitStructure.DMA_BufferSize = UART2_DMA_Rx_LEN;					//DMAͨ���Ļ���Ĵ�С��һ�ν��յ�����ֽ���
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//����DMA���������ģʽ�������ַ�Ĵ���������
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//����DMA���ڴ����ģʽ���ڴ��ַ�Ĵ�������
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//���������ֳ������ݿ��Ϊ8λ
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//�ڴ������ֳ������ݿ��Ϊ8λ
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//����DMA�Ĵ���ģʽ������������ģʽ�������˲��ٽ��գ�������ѭ������
//	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;					//����DMA�����ȼ��𣬷ǳ��ߵ����ȼ�
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;					//ָ�����FIFOģʽ��ֱ��ģʽ������ָ������ �� ��ʹ��FIFOģʽ
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;		//ָ����FIFO��ֵˮƽ
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;				//ָ����Burstת�������ڴ洫��
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;		//ָ����Burstת��������Χת��
//	DMA_Init(DMA1_Stream5, &DMA_InitStructure);								//����DMA1��ͨ��
//	
//	/*����UART2 �жϴ���ģʽ*/
//	//�����ж�  
//	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
//        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;               //ͨ������Ϊ�����ж�  
//        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;       //�ж�ռ�ȵȼ�
//        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;              //�ж���Ӧ���ȼ�
//        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 //���ж�  
//        NVIC_Init(&NVIC_InitStructure);
//	//�ж�����
////	USART_ITConfig(USART2,USART_IT_TC,DISABLE);		// Transmit Complete����������жϡ��������ڷ�������жϣ�����һ���ֽھͻ�����жϣ�ֻ��Ҫ����жϱ�־λ������Ҫ�ر��ж�
////	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);	//RX NO Empty��RX�ǿգ�RX�������жϡ��������ڽ����жϣ�ʹ��DMA����ʱҪʧ�������ʹ�ܿ����ж�
////	USART_ITConfig(USART2,USART_IT_TXE,DISABLE);	//TX Empty��TXΪ�գ����ͼĴ���DR���㡣���ͼĴ��������жϣ�������һ���ֽں󣬱���ر�����жϣ�����ֻҪ�Ĵ���Ϊ��ֵ���ͻᷴ�������ж�
//	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);	//�������ڿ����ж�
//	
//	/*����DMA����*/
//	DMA_Cmd(DMA1_Stream5,ENABLE);							//ʹ��DMA����ͨ��
//	//USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);			//����DMA��ʽ����
//	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);			//����DMA��ʽ����
//}



//void uart2_dma_tx_configuration(void)
//{
//	/*����DMA��ʼ���ṹ��*/
//	DMA_InitTypeDef  DMA_InitStructure;
//	/*�����жϽṹ��*/
//	NVIC_InitTypeDef NVIC_InitStructure;
//	/*ʹ��DMA����ʱ��*/
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
//	
//	/*UART2 DMA1 Stream6 Channel_4 ��������*/
//	DMA_DeInit(DMA1_Stream6);
//	DMA_InitStructure.DMA_Channel = DMA_Channel_4;							//DMAͨ������
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);		//DMA�������ַ
//	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)UART2_DMA_Tx_Buff;	//DMA�ڴ����ַ,�ѽ��յ������ݷŵ��Ķ�
//	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;					//DMA���ݴ��䷽�򣬴��ڴ浽���跢�ͣ�������Ϊ���ݴ���Ŀ�ĵ�
//	DMA_InitStructure.DMA_BufferSize = UART2_DMA_Tx_LEN;					//DMAͨ���Ļ���Ĵ�С��һ�η��͵�����ֽ���
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//����DMA���������ģʽ�������ַ�Ĵ���������
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//����DMA���ڴ����ģʽ���ڴ��ַ�Ĵ�������
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//���������ֳ������ݿ��Ϊ8λ
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;		//�ڴ������ֳ������ݿ��Ϊ8λ
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//����DMA�Ĵ���ģʽ������������ģʽ�������˲��ٽ��գ�������ѭ������
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;						//����DMA�����ȼ��𣬷ǳ��ߵ����ȼ�
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;					//ָ�����FIFOģʽ��ֱ��ģʽ������ָ������ �� ��ʹ��FIFOģʽ
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;		//ָ����FIFO��ֵˮƽ
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;				//ָ����Burstת�������ڴ洫��
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;		//ָ����Burstת��������Χת��      
//	DMA_Init(DMA1_Stream6, &DMA_InitStructure);								//����DMA1��ͨ��
//	
//	/*����UART2 �жϴ���ģʽ*/
//	//DMA�����ж�����
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	//�ж�����
//	USART_ITConfig(USART2,USART_IT_TC,DISABLE);		// Transmit Complete����������жϡ��������ڷ�������жϣ�����һ���ֽھͻ�����жϣ�ֻ��Ҫ����жϱ�־λ������Ҫ�ر��ж�
//	USART_ITConfig(USART2,USART_IT_TXE,DISABLE);	//TX Empty��TXΪ�գ����ͼĴ���DR���㡣���ͼĴ��������жϣ�������һ���ֽں󣬱���ر�����жϣ�����ֻҪ�Ĵ���Ϊ��ֵ���ͻᷴ�������ж�
//	//ʹ���ж�
//	DMA_ITConfig(DMA1_Stream6,DMA_IT_TC,ENABLE);
//	
//	/*����DMA����*/
//	DMA_Cmd(DMA1_Stream6,ENABLE);							//ʹ��DMA����ͨ��
//	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);			//����DMA��ʽ����
//}
// 


//uint16_t len = 0;    //��֡���յ����ֽڳ���

//void USART2_IRQHandler(void)
//{
//    //��������жϴ���
//    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
//    {
//	USART2->SR;
//	USART2->DR; //��USART_IT_IDLE��־
//	//�ر�DMA
//	DMA_Cmd(DMA1_Stream5,DISABLE);
//	//�����־λ
//	DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5);
//	//��ý���֡֡��
//	len = UART2_DMA_Rx_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);
//		
//	/* �Խ������ݴ��� */
//	memset(buf,0,UART2_DMA_Rx_LEN);
//	memcpy(buf,UART2_DMA_Rx_Buff,len);
//    printf("���յ������ݣ�%d\r\n",buf); 
//	/* ��ʼ��һ֡����׼�� */
//	//���ô������ݳ���
//	DMA_SetCurrDataCounter(DMA1_Stream5,UART2_DMA_Rx_LEN);
//	//��DMA
//	DMA_Cmd(DMA1_Stream5,ENABLE);
//    }
//}


/////**
//// * @brief  ���� 2 �жϷ�����.
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
////		DMA_Cmd(DMA1_Stream5,DISABLE);; //�ر�DMA���ڽ��մ���ͨ��5
////		
////		RX_len = UART2_DMA_Rx_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);//���ν������ݳ�����-����-�ã�=��
////		for (uint8_t i = 0; i < RX_len; i++) {
////			
////		 Receive(UART2_DMA_Rx_Buff[i]);//�������ڷ�����������

////		}
////		DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5);            //���DMA�жϱ�־λ
////		DMA_SetCurrDataCounter(DMA1_Stream5,UART2_DMA_Rx_LEN); //�����´���Ҫ���յ��ֽڳ���
////		DMA_Cmd(DMA1_Stream5,ENABLE);                          //��DMA	
////  }
////}

                        

