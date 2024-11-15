#include "usart3.h"
#include "sys.h"
#include "Usartlin.h"


/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"      //FreeRTOS ʹ��  
#endif

//��ʼ��IO ����3
//bound:������	  
void usart3_init(u32 bound)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIODʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��

 	USART_DeInit(USART3);  //��λ����3
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3); //GPIOD8����ΪUSART3
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3); //GPIOD9����ΪUSART3	
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; //GPIOD8��GPIOD9��ʼ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOD,&GPIO_InitStructure); //��ʼ��GPIOD8����GPIOD9
	
	USART_InitStructure.USART_BaudRate = bound;//������ 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  
	USART_Init(USART3, &USART_InitStructure); //��ʼ������3 
	USART_Cmd(USART3, ENABLE);               //ʹ�ܴ��� 
//	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);  //ʹ��DMA����	
	
//  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�   
#if EN_USART3_RX	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//��������ж�
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);

	//Usart3 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//����2�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6;//��ռ���ȼ�6
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif
}


uint8_t Recv3[USART3_MAX_RECV_LEN];//���ڽ��ջ���
u8 rx3_cnt;//�������ݸ�����������
u8 isReceiveUart3Cmd = 0;
// �����ź���
extern SemaphoreHandle_t xSemaphore;
void USART3_IRQHandler(void)                	
{
	uint8_t data;//���������ݴ����

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�
	{
		data = USART_ReceiveData(USART3);   			
//		Recv3[rx3_cnt++]=data;//���յ����ݴ���������� 
//		revSolv_handle(&data);
		
		        // �����ݴ�����ջ�����
        if (rx3_cnt < USART3_MAX_RECV_LEN) {
            Recv3[rx3_cnt++] = data;  // �����ݴ������鲢���Ӽ���
        }

		
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	} 
	
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)//�����ж�
	{
		isReceiveUart3Cmd=1;
		data = USART3->SR;//���ڿ����жϵ��жϱ�־ֻ��ͨ���ȶ�SR�Ĵ������ٶ�DR�Ĵ��������
		data = USART3->DR;
		        // �ͷ��ź�����֪ͨ������д���
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
		
		        // ����Ƿ���Ҫ���������л�
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        // ��������жϱ�־
        USART_ClearITPendingBit(USART3, USART_IT_IDLE);
	}
} 

