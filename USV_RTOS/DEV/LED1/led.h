#ifndef __LED_H
#define __LED_H
#include "sys.h"


//LED�˿ڶ���
#define RCC_AHB1Periph_LED RCC_AHB1Periph_GPIOC
#define GPIO_LED GPIOC

#define GPIO_Pin_LEDa GPIO_Pin_13
#define digitalHigh(p,i)     {p->BSRRH=i;}  //��������ߵ�ƽ       
#define digitalLow(p,i)      {p->BSRRL=i;}   //��������͵�ƽ
#define digitalToggle(p,i)   {p->ODR ^=i;} //��ת
#define LEDa_OFF      digitalLow(GPIO_LED,GPIO_Pin_LEDa)
#define LEDa_ON     digitalHigh(GPIO_LED,GPIO_Pin_LEDa)
#define LEDa_Toggle  digitalToggle(GPIO_LED,GPIO_Pin_LEDa)


void LED_Init(void);//��ʼ��		
void ledRun(void);
#endif
