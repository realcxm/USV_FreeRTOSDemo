#ifndef __UART3_H
#define __UART3_H

#include "sys.h"
#include "stdio.h"
#include <String.h>
#include <stdarg.h>
#include <stdio.h>
#include "stm32f4xx.h"                  // Device header





#define USART3_MAX_RECV_LEN		600					//�����ջ����ֽ���
#define EN_USART3_RX 	1  //ʹ�ܣ�1��/��ֹ��0������3����



void usart3_init(uint32_t pclk1);





#endif
