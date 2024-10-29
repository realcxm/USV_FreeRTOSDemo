#ifndef __LED_H
#define __LED_H
#include "sys.h"


//LED端口定义
#define RCC_AHB1Periph_LED RCC_AHB1Periph_GPIOC
#define GPIO_LED GPIOC

#define GPIO_Pin_LEDa GPIO_Pin_13
#define digitalHigh(p,i)     {p->BSRRH=i;}  //引脚输出高电平       
#define digitalLow(p,i)      {p->BSRRL=i;}   //引脚输出低电平
#define digitalToggle(p,i)   {p->ODR ^=i;} //反转
#define LEDa_OFF      digitalLow(GPIO_LED,GPIO_Pin_LEDa)
#define LEDa_ON     digitalHigh(GPIO_LED,GPIO_Pin_LEDa)
#define LEDa_Toggle  digitalToggle(GPIO_LED,GPIO_Pin_LEDa)


void LED_Init(void);//初始化		
void ledRun(void);
#endif
