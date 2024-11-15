#include "uart5.h"
#include "sys.h"
////////////////////////////////////////////////////////////////////////////////// 	 

//��ʼ��IO ����5
//bound:������	  
void uart5_init(u32 bound)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIODʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //ʹ��GPIOCʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE); //ʹ��UART5ʱ��

	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5); //GPIOD2����ΪUART5 RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //GPIOD2��ʼ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOD,&GPIO_InitStructure); //��ʼ��GPIOD2
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5); //GPIOC12����ΪUART5 TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //GPIOD2��ʼ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��GPIOC12	

	
	USART_InitStructure.USART_BaudRate = bound;//������ 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(UART5, &USART_InitStructure); //��ʼ������3 
	USART_Cmd(UART5, ENABLE);               //ʹ�ܴ��� 

}


////=======================================
////����5�жϷ������
////=======================================
//uint8_t Recv_uart5[128];//���ڽ��ջ���
//u8 rx5_cnt;//�������ݸ�����������
//u8 isReceiveUart5Cmd = 0;
//u8 rx5_flag=0;

//void UART5_IRQHandler(void)                	
//{
//	uint8_t data;//���������ݴ����

//	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //�����ж�
//	{
//		data = USART_ReceiveData(UART5);   			
//		Recv_uart5[rx5_cnt++]=data;//���յ����ݴ���������� 
//		rx5_flag = 1;
//		USART_ClearITPendingBit(UART5,USART_IT_RXNE);
//	} 
//	
//	if(USART_GetITStatus(UART5, USART_IT_IDLE) != RESET)//�����ж�
//	{
//		isReceiveUart5Cmd=1;
//		data = USART1->SR;//���ڿ����жϵ��жϱ�־ֻ��ͨ���ȶ�SR�Ĵ������ٶ�DR�Ĵ��������
//		data = USART1->DR;		
//	}
//} 

//uart5��������
void uart5_send(u8*data, u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		while(USART_GetFlagStatus(UART5,USART_FLAG_TC)==RESET); 
		USART_SendData(UART5,data[i]);   
	}
}

