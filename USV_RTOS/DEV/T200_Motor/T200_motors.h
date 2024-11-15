#ifndef __MOTORS_H
#define __MOTORS_H
#include "sys.h"


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

#define TIM_CLOCK_HZ 				84000000
/*电调协议配置（智只能选其一）*/
//#define USE_ESC_PROTOCOL_STANDARD  //标准PWM协议
//#define USE_ESC_PROTOCOL_ONESHOT125	//oneshot125协议
#define USE_ESC_PROTOCOL_T200

#if defined(USE_ESC_PROTOCOL_STANDARD)//STANDARD_PWM
//#define STANDARD_PWM_PERIOD      0.0025// 2.5ms = 400Hz
#define STANDARD_PWM_PERIOD      0.02// 2.5ms = 5Hz
#define MOTOR_TIM_PRESCALE_RAW   (uint32_t)((TIM_CLOCK_HZ/0xFFFF) * STANDARD_PWM_PERIOD + 1) //+1表示必须要分频，0xFFFF表示计数值
#define MOTOR_TIM_PERIOD 		 (uint32_t)(TIM_CLOCK_HZ * STANDARD_PWM_PERIOD / MOTOR_TIM_PRESCALE_RAW)//定时器重装载值
#define MOTOR_TIM_PRESCALE       (uint16_t)(MOTOR_TIM_PRESCALE_RAW - 1)//定时器预分频值
#define MOTOR_TIM_CNT_FOR_HIGH   (uint32_t)(TIM_CLOCK_HZ * 0.001 / MOTOR_TIM_PRESCALE_RAW)//1000us高电平所需计数值

#elif defined(USE_ESC_PROTOCOL_ONESHOT125)//ONESHOT125
#define ONESHOT125_PERIOD        0.0005 // 0.5ms = 2000Hz
#define MOTOR_TIM_PRESCALE_RAW   (uint32_t)((TIM_CLOCK_HZ/0xFFFF) * ONESHOT125_PERIOD + 1) //+1表示必须要分频，0xFFFF表示计数值
#define MOTOR_TIM_PERIOD 		 (uint32_t)(TIM_CLOCK_HZ * ONESHOT125_PERIOD / MOTOR_TIM_PRESCALE_RAW)//定时器重装载值
#define MOTOR_TIM_PRESCALE       (uint16_t)(MOTOR_TIM_PRESCALE_RAW - 1)
#define MOTOR_TIM_CNT_FOR_HIGH   (uint32_t)(TIM_CLOCK_HZ * 0.000125 / MOTOR_TIM_PRESCALE_RAW)//125us高电平所需计数值


/*
PWM波频率：50Hz（20ms）
中点停转：1500us
反转：1100 - 1500us
正转：1500 - 1900us
停转区间：1475us - 1525us
*/
#elif defined(USE_ESC_PROTOCOL_T200)//ONESHOT125
#define ONESHOT200_PERIOD        0.02 // 20ms = 50Hz
#define MOTOR_TIM_PRESCALE_RAW   (uint32_t)((TIM_CLOCK_HZ/0xFFFF) * ONESHOT200_PERIOD + 1) //+1表示必须要分频，0xFFFF表示计数值
#define MOTOR_TIM_PERIOD 		 (uint32_t)(TIM_CLOCK_HZ * ONESHOT200_PERIOD / MOTOR_TIM_PRESCALE_RAW)//定时器重装载值
#define MOTOR_TIM_PRESCALE       (uint16_t)(MOTOR_TIM_PRESCALE_RAW - 1)
#define MOTOR_TIM_CNT_FOR_HIGH   (uint32_t)(TIM_CLOCK_HZ * 0.000125 / MOTOR_TIM_PRESCALE_RAW)//125us高电平所需计数值

#endif


#define NBR_OF_MOTORS 	4

#define MOTOR_M1  		0
#define MOTOR_M2  		1
#define MOTOR_M3  		2
#define MOTOR_M4  		3


//定义三档速度常量
#define SPEED_LOW  1
#define SPEED_MEDIUM 2
#define SPEED_HIGH 3

void motorsInit(void);
void motorsSetRatio(u32 id, u16 ithrust);
uint16_t thrust(int ccr_value);
// float LineTrust(float PS2_num);
//float LineTrust(float PS2_num, float Max1, float Min1);

float LineTrust(float PS2_num, float x1, float y1, float x2, float y2 );
uint16_t thrust_negative(int control_num);
uint16_t thrust_positive(int control_num);
#endif /* __MOTORS_H */

