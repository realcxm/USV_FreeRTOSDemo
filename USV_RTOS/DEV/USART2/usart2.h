#ifndef __USART2_H__
#define __USART2_H__
#include "stdio.h"
#include "stm32f4xx.h"

#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口1接收
void usart2_init(u32 bound);
void usart2_send(u8*data, u8 len);

#endif













