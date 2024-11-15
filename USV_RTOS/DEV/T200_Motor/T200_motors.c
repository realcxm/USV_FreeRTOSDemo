#include "sys.h"
#include "delay.h"
#include "stdbool.h"
#include "T200_motors.h"
#include "stdio.h"
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_rcc.h>
//定义推进器油程
#define THRUST_MAX 2200
#define THRUST_MIN 800
#define THRUST_MIDDLE 1500
// //定义三档速度常量
// #define SPEED_LOW  1
// #define SPEED_MEDIUM 2
// #define SPEED_HIGH 3



/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ATKflight飞控固件
 * 电机驱动代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/5/2
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/
static bool isInit = false;
u32 motor_ratios[] = {0, 0, 0, 0};
const u32 MOTORS[] = { MOTOR_M1, MOTOR_M2, MOTOR_M3, MOTOR_M4 };


//定义初始速度
int currentSpeed = SPEED_LOW;

//定义按键值
int buttonValue ;

void motorsInit(void)	/*电机初始化*/
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  
	
	TIM_DeInit(TIM3);
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM3);//PC6 复用为TIM3 CH1	MOTOR1
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM3);//PC7 复用为TIM3 CH2	MOTOR2
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM3);//PC8 复用为TIM3 CH3	MOTOR3
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM3);//PC9 复用为TIM3 CH4	MOTOR4
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;		
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;      
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure); 	
	
	TIM_TimeBaseStructure.TIM_Period = 20000-1;			//自动重装载值
	TIM_TimeBaseStructure.TIM_Prescaler = 84-1;		//定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//向上计数模式	
	TIM_TimeBaseStructure.TIM_ClockDivision=0; 						//时钟分频
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;					//重复计数次数
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);					//初始化TIM3
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2;				//PWM模式2
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;	//使能输出
	TIM_OCInitStructure.TIM_Pulse=0;							//CCRx
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;		//高电平有效
	TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;	//空闲高电平
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  	//初始化TIM3 CH1输出比较
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  	//初始化TIM3 CH2输出比较
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);  	//初始化TIM3 CH3输出比较
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);  	//初始化TIM3 CH4输出比较
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR1上的预装载寄存器
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR3上的预装载寄存器
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR4上的预装载寄存器
 
	TIM_ARRPreloadConfig(TIM3,ENABLE);	//TIM3	ARPE使能 
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


/*设置电机PWM占空比*/
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
// //油门映射到PS2
// float LineTrust(float PS2_num){
// 	float Max1,Min1,Max2,Min2;
// 	float Cal_num;
// 	Max1 = 2200, Min1 = 800, Max2 = 255, Min2 = 0;
// 	Cal_num = (Max1 - Min1)/(Max2 - Min2)*(PS2_num - Min2)+800;
//     return Cal_num;
// }
//油门映射到PS2
float LineTrust(float PS2_num, float x1, float y1, float x2, float y2 ){
	
    return 	 y1 + ((y2 - y1) * (PS2_num - x1)) / (x2 - x1);
}


int16_t Value_Limit(int16_t Min,int16_t Max,int16_t Data)
{
    if(Data>=Max) Data=Max;
    else if(Data<=Min) Data=Min;
    return Data;
}







