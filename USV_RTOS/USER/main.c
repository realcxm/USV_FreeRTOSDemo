/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* ��׼���ļ� */
#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include <string.h>
#include "uart5.h"
#include "usart2.h"
#include "Usartlin.h"
////�������ȼ�
//#define START_TASK_PRIO		1
////�����ջ��С	
//#define START_STK_SIZE 		128  
////������
//TaskHandle_t StartTask_Handler;
////������
//void start_task(void *pvParameters);


////�������ȼ�
//#define LED1_TASK_PRIO		2
////�����ջ��С	
//#define LED1_STK_SIZE 		50  
////������
//TaskHandle_t LED1Task_Handler;
////������
//void led1_task(void *pvParameters);


////�������ȼ�
//#define FLOAT_TASK_PRIO		3
////�����ջ��С	
//#define FLOAT_STK_SIZE 		128
////������
//TaskHandle_t FLOATTask_Handler;
////������
//void float_task(void *pvParameters);


//int main(void)
//{ 
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
//	delay_init(168);		//��ʼ����ʱ����
//	uart_init(115200);     	//��ʼ������
//	LED_Init();		        //��ʼ��LED�˿�
//	
//	//������ʼ����
//    xTaskCreate((TaskFunction_t )start_task,            //������
//                (const char*    )"start_task",          //��������
//                (uint16_t       )START_STK_SIZE,        //�����ջ��С
//                (void*          )NULL,                  //���ݸ��������Ĳ���
//                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
//                (TaskHandle_t*  )&StartTask_Handler);   //������              
//    vTaskStartScheduler();          //�����������
//}
// 
////��ʼ����������
//void start_task(void *pvParameters)
//{
//    taskENTER_CRITICAL();           //�����ٽ���
//    //����LED0����
//    xTaskCreate((TaskFunction_t )led1_task,     	
//                (const char*    )"led1_task",   	
//                (uint16_t       )LED1_STK_SIZE, 
//                (void*          )NULL,				
//                (UBaseType_t    )LED1_TASK_PRIO,	
//                (TaskHandle_t*  )&LED1Task_Handler);         
//    //�����������
//    xTaskCreate((TaskFunction_t )float_task,     
//                (const char*    )"float_task",   
//                (uint16_t       )FLOAT_STK_SIZE, 
//                (void*          )NULL,
//                (UBaseType_t    )FLOAT_TASK_PRIO,
//                (TaskHandle_t*  )&FLOATTask_Handler);  
//    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
//    taskEXIT_CRITICAL();            //�˳��ٽ���
//}

////LED1������ 
//void led1_task(void *pvParameters)
//{
//    while(1)
//    {
//		ledRun();
//        vTaskDelay(500);
//    }
//}   


////�����������
//void float_task(void *pvParameters)
//{
//	static float float_num=0.00;
//	while(1)
//	{
//		float_num+=0.01f;
//		printf("float_num��ֵΪ: %.4f\r\n",float_num);
//        vTaskDelay(1000);
//	}
//}



/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
static TaskHandle_t Uart_Task_Handle = NULL;/* UART2������ */
/********************************** �ں˶����� *********************************/
SemaphoreHandle_t BinarySem_Handle =NULL;

/******************************* ȫ�ֱ������� ************************************/
/*

 */
 
extern uint8_t Recv2;//���ڽ��ջ���
/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void);/* ���ڴ������� */
static void Uart_Task(void* pvParameters);/*����2����*/
static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */
/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/


int main(void){
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
/* Ӳ����ʼ�� */
  BSP_Init();
	 /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
/* ����������� */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  while(1);/* ��������ִ�е����� */    

};

/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  taskENTER_CRITICAL();           //�����ٽ���
	
  /* ���� BinarySem */
  BinarySem_Handle = xSemaphoreCreateBinary();	 
  
	if(NULL != BinarySem_Handle)
    printf("BinarySem_Handle��ֵ�ź��������ɹ�!\n");
	
  /* ����Uart_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )Uart_Task,  /* ������ں��� */
                        (const char*    )"Uart_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )3, /* ��������ȼ� */
                        (TaskHandle_t*  )&Uart_Task_Handle);/* ������ƿ�ָ�� */ 
  if(pdPASS == xReturn)
    printf("����Uart_Task����ɹ�!\n");
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

/**********************************************************************
  * @ ������  �� LED_Task
  * @ ����˵���� LED_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void Uart_Task(void* parameter)
{	
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  while (1)
  {
    //��ȡ��ֵ�ź��� xSemaphore,û��ȡ����һֱ�ȴ�
		xReturn = xSemaphoreTake(BinarySem_Handle,/* ��ֵ�ź������ */
                              portMAX_DELAY); /* �ȴ�ʱ�� */
    if(pdPASS == xReturn)
    {
			ledRun();
      printf("�յ�����:%d\n",Recv2);
      memset((void *)Recv2,0,sizeof(Recv2));/* ���� */
    }
  }
}
/***********************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	delay_init(168);  		//��ʼ����ʱ����
								
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200        ���Դ��ڣ�PA9��PA10��
								
	usart2_init(115200);	//��ʼ������2������Ϊ115200       ���տ���ָ�PA2��PA3��
	
	LED_Init();//��ʼ��		

//	usart3_init(9600);	//��ʼ������3������Ϊ115200  	    GPS   ��PD8��PD9��
//	
//	uart5_init(115200);   //��ʼ������5                     �ϴ�����GPS��PC12 TX,PD2 RX��

	
}