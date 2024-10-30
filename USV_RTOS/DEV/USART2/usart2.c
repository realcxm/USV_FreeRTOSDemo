#include "usart2.h"
#include "sys.h"
#include "Usartlin.h"

/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//#define 	UART2_MAXBUFFERSIZE		256

uint8_t Recv2[128];//���ڽ��ջ���
u8 rx2_cnt;//�������ݸ�����������
u8 isReceiveUart2Cmd = 0;
u8 rx2_flag=0;
extern SemaphoreHandle_t BinarySem_Handle;

void USART2_RX_IRQHandler(void)                	
{
	BaseType_t pxHigherPriorityTaskWoken;
	uint8_t data;//���������ݴ����

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
	{
		data = USART_ReceiveData(USART2);   			
		rx2_flag = 1;
		Recv2[rx2_cnt++]=data;//���յ����ݴ���������� 
//		revSolv_handle(&data);
//		printf("%c",data);
 // ������ֵ�ź�����֪ͨǰ̨�����������ݿ���
    xSemaphoreGiveFromISR(BinarySem_Handle, &pxHigherPriorityTaskWoken);
		
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	} 
	
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)//�����ж�
	{
		isReceiveUart2Cmd=1;
		data = USART2->SR;//���ڿ����жϵ��жϱ�־ֻ��ͨ���ȶ�SR�Ĵ������ٶ�DR�Ĵ��������
		data = USART2->DR;
		
	}
	// ����и������ȼ�������Ҫ���ѣ������һ���������л�
	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);	
} 


void usart2_init(u32 bound)
{  
NVIC_InitTypeDef NVIC_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;

//1.����ʱ�Ӻͺ�GPIOʱ��ʹ�ܡ�
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
//2.�������Ÿ�����ӳ�� 
GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2����ΪUSART2
GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3����ΪUSART2  
//3.GPIO�˿�ģʽ����
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2��GPIOA3��ʼ��
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //�ٶ�50MHz
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��GPIOA2����GPIOA3
//4.���ڲ�����ʼ�������ò����ʣ��ֳ�����żУ��Ȳ���
USART_InitStructure.USART_BaudRate = bound;//������һ������Ϊ9600;
USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
USART_Init(USART2, &USART_InitStructure); //��ʼ������2����

//5.ʹ�ܴ���
USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 

#if EN_USART2_RX	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);

	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����2�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

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

