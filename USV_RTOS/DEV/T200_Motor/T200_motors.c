#include "sys.h"
#include "delay.h"
#include "stdbool.h"
#include "T200_motors.h"
#include "stdio.h"
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_rcc.h>
//�����ƽ����ͳ�
#define THRUST_MAX 2200
#define THRUST_MIN 800
#define THRUST_MIDDLE 1500
// //���������ٶȳ���
// #define SPEED_LOW  1
// #define SPEED_MEDIUM 2
// #define SPEED_HIGH 3



/********************************************************************************	 
 * ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 * ATKflight�ɿع̼�
 * �����������	
 * ����ԭ��@ALIENTEK
 * ������̳:www.openedv.com
 * ��������:2018/5/2
 * �汾��V1.0
 * ��Ȩ���У�����ؾ���
 * Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
 * All rights reserved
********************************************************************************/
static bool isInit = false;
u32 motor_ratios[] = {0, 0, 0, 0};
const u32 MOTORS[] = { MOTOR_M1, MOTOR_M2, MOTOR_M3, MOTOR_M4 };


//�����ʼ�ٶ�
int currentSpeed = SPEED_LOW;

//���尴��ֵ
int buttonValue ;

void motorsInit(void)	/*�����ʼ��*/
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  
	
	TIM_DeInit(TIM3);
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM3);//PC6 ����ΪTIM3 CH1	MOTOR1
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM3);//PC7 ����ΪTIM3 CH2	MOTOR2
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM3);//PC8 ����ΪTIM3 CH3	MOTOR3
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM3);//PC9 ����ΪTIM3 CH4	MOTOR4
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;		
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;      
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure); 	
	
	TIM_TimeBaseStructure.TIM_Period = 20000-1;			//�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 84-1;		//��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//���ϼ���ģʽ	
	TIM_TimeBaseStructure.TIM_ClockDivision=0; 						//ʱ�ӷ�Ƶ
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;					//�ظ���������
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);					//��ʼ��TIM3
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2;				//PWMģʽ2
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;	//ʹ�����
	TIM_OCInitStructure.TIM_Pulse=0;							//CCRx
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;		//�ߵ�ƽ��Ч
	TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;	//���иߵ�ƽ
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  	//��ʼ��TIM3 CH1����Ƚ�
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  	//��ʼ��TIM3 CH2����Ƚ�
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);  	//��ʼ��TIM3 CH3����Ƚ�
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);  	//��ʼ��TIM3 CH4����Ƚ�
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR3�ϵ�Ԥװ�ؼĴ���
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR4�ϵ�Ԥװ�ؼĴ���
 
	TIM_ARRPreloadConfig(TIM3,ENABLE);	//TIM3	ARPEʹ�� 
	TIM_Cmd(TIM3, ENABLE);
	
	TIM_SetCompare1(TIM3,2200); 
	TIM_SetCompare2(TIM3,2200); 
	delay_ms(1800);
	delay_ms(1800);

	TIM_SetCompare1(TIM3,1480);
	TIM_SetCompare2(TIM3,1480);
	delay_ms(1800);
	delay_ms(1800);
	
//	TIM_SetCompare1(TIM3,800);
//	TIM_SetCompare2(TIM3,800);
//	
//	delay_ms(1800);
//	delay_ms(1800);

	isInit = true;
}


/*���õ��PWMռ�ձ�*/
/*ithrust:0-1000*/
void motorsSetRatio(u32 id, u16 ithrust)
{
	if (isInit) 
	{
		switch(id)
		{
			case 0:		/*MOTOR_M1*/
				TIM_SetCompare1(TIM3,ithrust);
				break;
			case 1:		/*MOTOR_M2*/
				TIM_SetCompare2(TIM3,ithrust);
				break;
			case 2:		/*MOTOR_M3*/
				TIM_SetCompare3(TIM3,ithrust);
				break;
			case 3:		/*MOTOR_M4*/	
				TIM_SetCompare4(TIM3,ithrust);
				break;
			default: break;
		}	
	}
}
// //����ӳ�䵽PS2
// float LineTrust(float PS2_num){
// 	float Max1,Min1,Max2,Min2;
// 	float Cal_num;
// 	Max1 = 2200, Min1 = 800, Max2 = 255, Min2 = 0;
// 	Cal_num = (Max1 - Min1)/(Max2 - Min2)*(PS2_num - Min2)+800;
//     return Cal_num;
// }
//����ӳ�䵽PS2
float LineTrust(float PS2_num, float x1, float y1, float x2, float y2 ){
	
    return 	 y1 + ((y2 - y1) * (PS2_num - x1)) / (x2 - x1);
}


int16_t Value_Limit(int16_t Min,int16_t Max,int16_t Data)
{
    if(Data>=Max) Data=Max;
    else if(Data<=Min) Data=Min;
    return Data;
}







