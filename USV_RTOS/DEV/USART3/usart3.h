#ifndef __UART3_H
#define __UART3_H

#include "sys.h"
#include "stdio.h"
#include <String.h>
#include <stdarg.h>
#include <stdio.h>
#include "stm32f4xx.h"                  // Device header





#define USART3_MAX_RECV_LEN		600					//最大接收缓存字节数
#define EN_USART3_RX 	1  //使能（1）/禁止（0）串口3接收



void usart3_init(uint32_t pclk1);





#endif
