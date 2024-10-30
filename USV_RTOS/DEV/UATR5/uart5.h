#ifndef __UART5_H
#define __UART5_H

#include "sys.h"
#include "stm32f4xx.h"                  // Device header

#define UART5_MAX_RECV_LEN		600					//�����ջ����ֽ���
#define UART5_MAX_SEND_LEN		300					//����ͻ����ֽ���
#define UART5_RX_EN 			1					//0,������;1,����.



void uart5_init(u32 bound);
void UART5_IRQHandler(void);
void uart5_send(u8*data, u8 len);



#endif
