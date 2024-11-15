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
#include "remotecontrol.h"
#include "stdio.h"
#include "usart3.h"
#include "nmea_gps.h"
/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* 创建任务句柄 */
/* LED任务句柄 */
static TaskHandle_t LED_Task_Handle = NULL;
/* 控制指令任务句柄(lora_Usart2)*/
static TaskHandle_t ReceiveOrder_Task_Handle = NULL;/* UART2任务句柄 */
/* 上传GPS任务句柄(lora_Usart2)*/
static TaskHandle_t SendGPS_Task_Handle = NULL;

/********************************** 内核对象句柄 *********************************/

SemaphoreHandle_t xSemaphore = NULL;
/******************************* 全局变量声明 ************************************/

extern uint8_t Recv2[128];  // 串口2接收缓存
extern u8 rx2_cnt;  // 接收数据个数计数变量
extern u8 isReceiveUart2Cmd;



extern uint8_t Recv3[USART3_MAX_RECV_LEN];//串口接收缓存
extern u8 rx3_cnt;//接收数据个数计数变量
extern u8 isReceiveUart3Cmd ;

nmea_msg gpsx;
 /******************************* 宏定义 ************************************/
 

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void AppTaskCreate(void);/* 用于创建任务 */
static void LED_Task(void* pvParameters);/* LED_Task任务实现 */
static void ReceiveOrder_Task(void* pvParameters);/*串口2_接收控制指令任务*/
static void SendGPS_Task(void* pvParameters);/* SendGPS_Task任务实现 */
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
	
  xSemaphore = xSemaphoreCreateBinary();
	 /* 创建LED_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )LED_Task, /* 任务入口函数 */
                        (const char*    )"LED_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )1,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&LED_Task_Handle);/* 任务控制块指针 */
 if(pdPASS == xReturn)
    printf("创建LED_Task任务成功!\n");
	
  /* 创建ReceiveOrder_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )ReceiveOrder_Task,  /* 任务入口函数 */
                        (const char*    )"ReceiveOrder_Task",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )2, /* 任务的优先级 */
                        (TaskHandle_t*  )&ReceiveOrder_Task_Handle);/* 任务控制块指针 */ 
 
 if(pdPASS == xReturn)
    printf("创建ReceiveOrder_Task任务成功!\n");
 
   /* 创建SendGPS_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )SendGPS_Task,  /* 任务入口函数 */
                        (const char*    )"SendGPS_Task",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )2, /* 任务的优先级 */
                        (TaskHandle_t*  )&SendGPS_Task_Handle);/* 任务控制块指针 */ 
 
 if(pdPASS == xReturn)
    printf("创建SendGPS_Task任务成功!\n");
  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}

/**********************************************************************
  * @ 函数名  ： LED_Task
  * @ 功能说明： LED_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void LED_Task(void* parameter)
{	
    while (1)
    {
	ledRun();
    }

}

/**********************************************************************
  * @ 函数名  ： ReceiveOrder_Task
  * @ 功能说明： ReceiveOrder_Task任务主体 接收控制指令、解析并开启遥控
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void ReceiveOrder_Task(void* parameter)
{	
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
    while (1) {
//		printf("执行ReceiveOrder_Task！！！\r\n");
        if (isReceiveUart2Cmd) {
			//遥控
            remotecontroller();
			printf("传输遥控值!!!\r\n");
        }
		vTaskDelay(500);
    }
}

/**********************************************************************
  * @ 函数名  ： SendGPS_Task
  * @ 功能说明： SendGPS_Task任务主体 解析GPS指令、并且上传
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void SendGPS_Task(void* parameter)
{
	while(1){	
		if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE){
				if(isReceiveUart3Cmd){
	         // 当接收到完整的 GPS 数据时，开始解析
					GPS_Analysis(&gpsx, Recv3);  //  GPS_Analysis 函数解析 Recv3 数组中的数据
					printf("执行SendGPS_Task\r\n");
					int temp_lon=(double)gpsx.longitude/10;  		/* temp_lon为真实经度，精确到小数点后七位 */     
					int temp_lat= (double)gpsx.latitude ;/* temp_lat为真实纬度，精确到小数点后七位 */ 
//					double real_hdop=(double)gpsx.hdop/10; 								/* 水平精度因子,对应实际值0~50.0 */     
//					double real_pdop=(double)gpsx.pdop/10;  							/* 位置精度因子，对应实际值0~50.0 */          
//					double real_vdop=(double)gpsx.vdop/10;							  /* 垂直精度因子,对应实际值0~50.0 */                        
//					double num_of_weixing =(double)gpsx.posslnum; 				/* 可见卫星数量 */                               
//					double real_altitude = (double)gpsx.altitude;					/* 海拔高度 */           
//					double real_speed = (double)gpsx.speed;  							/* 地面速度 */        
//					double real_fixmode = (double)gpsx.fixmode;						/* 定位类型 */                   
//					char dingweiyouxiao= (char)gpsx.pvi;  								/* 定位是否有效 */                                
//					char dingweimoshi = (char)gpsx.pmf;   							/* 定位模式标志 */                                   
//					double time_sec = (double)gpsx.utc.sec;
//					double time_min = (double)gpsx.utc.min;
//					double time_hour = (double)gpsx.utc.hour+8;
					printf("真实经度为： %d\r\n",temp_lon);
					printf("真实纬度为： %d\r\n",temp_lat);
//					printf("定位有效标志：%c\r\n",dingweiyouxiao);
//					printf("时间为： %.0f:%.0f:%.0f\r\n",time_hour,time_min,time_sec);	
            // 清除接收标志和数据
            rx3_cnt = 0;  // 重置接收缓冲区计数
            isReceiveUart3Cmd = 0;  // 清除接收标志
		}

	}

	  vTaskDelay(500);
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
	
	LED_Init();//初始化	
								
	uart_init(115200);		//初始化串口波特率为115200        调试串口（PA9、PA10）
								
	usart2_init(115200);	//初始化串口2波特率为115200       接收控制指令（PA2、PA3）
	
//	printf("初始化完成！！！\r\n");
	usart3_init(9600);	//初始化串口3波特率为115200  	    GPS   （PD8、PD9）
	
}




