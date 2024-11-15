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
#include "remotecontrol.h"
#include "stdio.h"
#include "usart3.h"
#include "nmea_gps.h"
/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
/* LED������ */
static TaskHandle_t LED_Task_Handle = NULL;
/* ����ָ��������(lora_Usart2)*/
static TaskHandle_t ReceiveOrder_Task_Handle = NULL;/* UART2������ */
/* �ϴ�GPS������(lora_Usart2)*/
static TaskHandle_t SendGPS_Task_Handle = NULL;

/********************************** �ں˶����� *********************************/

SemaphoreHandle_t xSemaphore = NULL;
/******************************* ȫ�ֱ������� ************************************/

extern uint8_t Recv2[128];  // ����2���ջ���
extern u8 rx2_cnt;  // �������ݸ�����������
extern u8 isReceiveUart2Cmd;



extern uint8_t Recv3[USART3_MAX_RECV_LEN];//���ڽ��ջ���
extern u8 rx3_cnt;//�������ݸ�����������
extern u8 isReceiveUart3Cmd ;

nmea_msg gpsx;
 /******************************* �궨�� ************************************/
 

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void);/* ���ڴ������� */
static void LED_Task(void* pvParameters);/* LED_Task����ʵ�� */
static void ReceiveOrder_Task(void* pvParameters);/*����2_���տ���ָ������*/
static void SendGPS_Task(void* pvParameters);/* SendGPS_Task����ʵ�� */
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
	
  xSemaphore = xSemaphoreCreateBinary();
	 /* ����LED_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )LED_Task, /* ������ں��� */
                        (const char*    )"LED_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )1,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&LED_Task_Handle);/* ������ƿ�ָ�� */
 if(pdPASS == xReturn)
    printf("����LED_Task����ɹ�!\n");
	
  /* ����ReceiveOrder_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )ReceiveOrder_Task,  /* ������ں��� */
                        (const char*    )"ReceiveOrder_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )2, /* ��������ȼ� */
                        (TaskHandle_t*  )&ReceiveOrder_Task_Handle);/* ������ƿ�ָ�� */ 
 
 if(pdPASS == xReturn)
    printf("����ReceiveOrder_Task����ɹ�!\n");
 
   /* ����SendGPS_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )SendGPS_Task,  /* ������ں��� */
                        (const char*    )"SendGPS_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )2, /* ��������ȼ� */
                        (TaskHandle_t*  )&SendGPS_Task_Handle);/* ������ƿ�ָ�� */ 
 
 if(pdPASS == xReturn)
    printf("����SendGPS_Task����ɹ�!\n");
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

/**********************************************************************
  * @ ������  �� LED_Task
  * @ ����˵���� LED_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void LED_Task(void* parameter)
{	
    while (1)
    {
	ledRun();
    }

}

/**********************************************************************
  * @ ������  �� ReceiveOrder_Task
  * @ ����˵���� ReceiveOrder_Task�������� ���տ���ָ�����������ң��
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void ReceiveOrder_Task(void* parameter)
{	
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
    while (1) {
//		printf("ִ��ReceiveOrder_Task������\r\n");
        if (isReceiveUart2Cmd) {
			//ң��
            remotecontroller();
			printf("����ң��ֵ!!!\r\n");
        }
		vTaskDelay(500);
    }
}

/**********************************************************************
  * @ ������  �� SendGPS_Task
  * @ ����˵���� SendGPS_Task�������� ����GPSָ������ϴ�
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void SendGPS_Task(void* parameter)
{
	while(1){	
		if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE){
				if(isReceiveUart3Cmd){
	         // �����յ������� GPS ����ʱ����ʼ����
					GPS_Analysis(&gpsx, Recv3);  //  GPS_Analysis �������� Recv3 �����е�����
					printf("ִ��SendGPS_Task\r\n");
					int temp_lon=(double)gpsx.longitude/10;  		/* temp_lonΪ��ʵ���ȣ���ȷ��С�������λ */     
					int temp_lat= (double)gpsx.latitude ;/* temp_latΪ��ʵγ�ȣ���ȷ��С�������λ */ 
//					double real_hdop=(double)gpsx.hdop/10; 								/* ˮƽ��������,��Ӧʵ��ֵ0~50.0 */     
//					double real_pdop=(double)gpsx.pdop/10;  							/* λ�þ������ӣ���Ӧʵ��ֵ0~50.0 */          
//					double real_vdop=(double)gpsx.vdop/10;							  /* ��ֱ��������,��Ӧʵ��ֵ0~50.0 */                        
//					double num_of_weixing =(double)gpsx.posslnum; 				/* �ɼ��������� */                               
//					double real_altitude = (double)gpsx.altitude;					/* ���θ߶� */           
//					double real_speed = (double)gpsx.speed;  							/* �����ٶ� */        
//					double real_fixmode = (double)gpsx.fixmode;						/* ��λ���� */                   
//					char dingweiyouxiao= (char)gpsx.pvi;  								/* ��λ�Ƿ���Ч */                                
//					char dingweimoshi = (char)gpsx.pmf;   							/* ��λģʽ��־ */                                   
//					double time_sec = (double)gpsx.utc.sec;
//					double time_min = (double)gpsx.utc.min;
//					double time_hour = (double)gpsx.utc.hour+8;
					printf("��ʵ����Ϊ�� %d\r\n",temp_lon);
					printf("��ʵγ��Ϊ�� %d\r\n",temp_lat);
//					printf("��λ��Ч��־��%c\r\n",dingweiyouxiao);
//					printf("ʱ��Ϊ�� %.0f:%.0f:%.0f\r\n",time_hour,time_min,time_sec);	
            // ������ձ�־������
            rx3_cnt = 0;  // ���ý��ջ���������
            isReceiveUart3Cmd = 0;  // ������ձ�־
		}

	}

	  vTaskDelay(500);
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
	
	LED_Init();//��ʼ��	
								
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200        ���Դ��ڣ�PA9��PA10��
								
	usart2_init(115200);	//��ʼ������2������Ϊ115200       ���տ���ָ�PA2��PA3��
	
//	printf("��ʼ����ɣ�����\r\n");
	usart3_init(9600);	//��ʼ������3������Ϊ115200  	    GPS   ��PD8��PD9��
	
}




