#ifndef __USART2_H__
#define __USART2_H__
#include "stdio.h"
#include "stm32f4xx.h"

#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
void usart2_init(u32 bound);
void usart2_send(u8*data, u8 len);

#endif













