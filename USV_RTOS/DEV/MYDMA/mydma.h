#ifndef __MYDMA_H__
#define __MYDMA_H__
#include "sys.h"
//#include "stm32f4xx.h"     



void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u32 mar,u16 ndtr,u32 dir,u32 priority);//����DMAx_CHx
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr);	//ʹ��һ��DMA����	
void MYDMA_Init(void);

#endif

