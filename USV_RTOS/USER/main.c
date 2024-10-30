/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* 标准库文件 */
#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include <string.h>
#include "uart5.h"
#include "usart2.h"
#include "Usartlin.h"
////任务优先级
//#define START_TASK_PRIO		1
////任务堆栈大小	
//#define START_STK_SIZE 		128  
////任务句柄
//TaskHandle_t StartTask_Handler;
////任务函数
//void start_task(void *pvParameters);


////任务优先级
//#define LED1_TASK_PRIO		2
////任务堆栈大小	
//#define LED1_STK_SIZE 		50  
////任务句柄
//TaskHandle_t LED1Task_Handler;
////任务函数
//void led1_task(void *pvParameters);


////任务优先级
//#define FLOAT_TASK_PRIO		3
////任务堆栈大小	
//#define FLOAT_STK_SIZE 		128
////任务句柄
//TaskHandle_t FLOATTask_Handler;
////任务函数
//void float_task(void *pvParameters);


//int main(void)
//{ 
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
//	delay_init(168);		//初始化延时函数
//	uart_init(115200);     	//初始化串口
//	LED_Init();		        //初始化LED端口
//	
//	//创建开始任务
//    xTaskCreate((TaskFunction_t )start_task,            //任务函数
//                (const char*    )"start_task",          //任务名称
//                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
//                (void*          )NULL,                  //传递给任务函数的参数
//                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
//                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
//    vTaskStartScheduler();          //开启任务调度
//}
// 
////开始任务任务函数
//void start_task(void *pvParameters)
//{
//    taskENTER_CRITICAL();           //进入临界区
//    //创建LED0任务
//    xTaskCreate((TaskFunction_t )led1_task,     	
//                (const char*    )"led1_task",   	
//                (uint16_t       )LED1_STK_SIZE, 
//                (void*          )NULL,				
//                (UBaseType_t    )LED1_TASK_PRIO,	
//                (TaskHandle_t*  )&LED1Task_Handler);         
//    //浮点测试任务
//    xTaskCreate((TaskFunction_t )float_task,     
//                (const char*    )"float_task",   
//                (uint16_t       )FLOAT_STK_SIZE, 
//                (void*          )NULL,
//                (UBaseType_t    )FLOAT_TASK_PRIO,
//                (TaskHandle_t*  )&FLOATTask_Handler);  
//    vTaskDelete(StartTask_Handler); //删除开始任务
//    taskEXIT_CRITICAL();            //退出临界区
//}

////LED1任务函数 
//void led1_task(void *pvParameters)
//{
//    while(1)
//    {
//		ledRun();
//        vTaskDelay(500);
//    }
//}   


////浮点测试任务
//void float_task(void *pvParameters)
//{
//	static float float_num=0.00;
//	while(1)
//	{
//		float_num+=0.01f;
//		printf("float_num的值为: %.4f\r\n",float_num);
//        vTaskDelay(1000);
//	}
//}



/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* 创建任务句柄 */
static TaskHandle_t Uart_Task_Handle = NULL;/* UART2任务句柄 */
/********************************** 内核对象句柄 *********************************/
SemaphoreHandle_t BinarySem_Handle =NULL;

/******************************* 全局变量声明 ************************************/
/*

 */
 
extern uint8_t Recv2;//串口接收缓存
/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void AppTaskCreate(void);/* 用于创建任务 */
static void Uart_Task(void* pvParameters);/*串口2任务*/
static void BSP_Init(void);/* 用于初始化板载相关资源 */
/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：硬件初始化 
            第二步：创建APP应用任务
            第三步：启动FreeRTOS，开始多任务调度
  ****************************************************************/


int main(void){
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
/* 硬件初始化 */
  BSP_Init();
	 /* 创建AppTaskCreate任务 */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
                        (const char*    )"AppTaskCreate",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )1, /* 任务的优先级 */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
/* 启动任务调度 */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* 启动任务，开启调度 */
  else
    return -1;  
  while(1);/* 正常不会执行到这里 */    

};

/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  taskENTER_CRITICAL();           //进入临界区
	
  /* 创建 BinarySem */
  BinarySem_Handle = xSemaphoreCreateBinary();	 
  
	if(NULL != BinarySem_Handle)
    printf("BinarySem_Handle二值信号量创建成功!\n");
	
  /* 创建Uart_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )Uart_Task,  /* 任务入口函数 */
                        (const char*    )"Uart_Task",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )3, /* 任务的优先级 */
                        (TaskHandle_t*  )&Uart_Task_Handle);/* 任务控制块指针 */ 
  if(pdPASS == xReturn)
    printf("创建Uart_Task任务成功!\n");
  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}

/**********************************************************************
  * @ 函数名  ： LED_Task
  * @ 功能说明： LED_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void Uart_Task(void* parameter)
{	
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  while (1)
  {
    //获取二值信号量 xSemaphore,没获取到则一直等待
		xReturn = xSemaphoreTake(BinarySem_Handle,/* 二值信号量句柄 */
                              portMAX_DELAY); /* 等待时间 */
    if(pdPASS == xReturn)
    {
			ledRun();
      printf("收到数据:%d\n",Recv2);
      memset((void *)Recv2,0,sizeof(Recv2));/* 清零 */
    }
  }
}
/***********************************************************************
  * @ 函数名  ： BSP_Init
  * @ 功能说明： 板级外设初始化，所有板子上的初始化均可放在这个函数里面
  * @ 参数    ：   
  * @ 返回值  ： 无
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
	 * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
	 * 都统一用这个优先级分组，千万不要再分组，切忌。
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	delay_init(168);  		//初始化延时函数
								
	uart_init(115200);		//初始化串口波特率为115200        调试串口（PA9、PA10）
								
	usart2_init(115200);	//初始化串口2波特率为115200       接收控制指令（PA2、PA3）
	
	LED_Init();//初始化		

//	usart3_init(9600);	//初始化串口3波特率为115200  	    GPS   （PD8、PD9）
//	
//	uart5_init(115200);   //初始化串口5                     上传数据GPS（PC12 TX,PD2 RX）

	
}