#include "led.h" 
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
//===========================================
//LED IO初始化
//初始化PB9为输出口.并使能这IO口的时钟		    
//===========================================
void LED_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_LED, ENABLE);//使能GPIOA时钟

	//GPIOB9初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_LEDa ;//LEDaIO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIO_LED, &GPIO_InitStructure);//初始化GPIO

}

void ledRun(void){
	
GPIO_WriteBit(GPIO_LED,GPIO_Pin_LEDa,(BitAction)(1-GPIO_ReadOutputDataBit(GPIO_LED,GPIO_Pin_LEDa)));
 vTaskDelay(500);
}
