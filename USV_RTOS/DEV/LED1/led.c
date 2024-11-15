#include "led.h" 
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
//===========================================
//LED IO��ʼ��
//��ʼ��PB9Ϊ�����.��ʹ����IO�ڵ�ʱ��		    
//===========================================
void LED_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_LED, ENABLE);//ʹ��GPIOAʱ��

	//GPIOB9��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_LEDa ;//LEDaIO��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIO_LED, &GPIO_InitStructure);//��ʼ��GPIO

}

void ledRun(void){
	
GPIO_WriteBit(GPIO_LED,GPIO_Pin_LEDa,(BitAction)(1-GPIO_ReadOutputDataBit(GPIO_LED,GPIO_Pin_LEDa)));
 vTaskDelay(500);
}
