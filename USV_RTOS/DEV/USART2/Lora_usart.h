#ifndef LORA_USART_H__
#define LORA_USART_H__
#include "stdio.h"
#include "stm32f4xx_conf.h"


#define EN_USART2_RX 	1			//ʹ�ܣ�1��/��ֹ��0������2����
#define UART2_DMA_Tx_LEN 500
#define UART2_DMA_Rx_LEN 500

void USARTLink_Init(void);


void uart2_dma_rx_configuration(void);
void uart2_dma_tx_configuration(void);









#endif


