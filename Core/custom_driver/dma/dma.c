/*
 * dma.c
 *
 *  Created on: Oct 3, 2026
 *      Author: asus
 */

#include "stm32f4xx.h"
#include <stdint.h>
#include "dma.h"


void DMA_init(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

	DMA1_Stream0->CR &= ~DMA_SxCR_EN;
	while(DMA1_Stream0->CR & DMA_SxCR_EN); // Wait for disable

	DMA1_Stream0->CR = 0; // Reset configuration

	DMA1_Stream0->CR &= ~DMA_SxCR_DIR;    // 00 : Peripheral to Memory
//	DMA1_Stream0->CR |= DMA_SxCR_CIRC;	  // 1: circular mode
	DMA1_Stream0->CR |= DMA_SxCR_CHSEL_0; // 001 : channel 1 + stream 0   I2C1_RX
	DMA1_Stream0->CR |= DMA_SxCR_MINC;    // memory increment
	DMA1_Stream0->CR &= ~DMA_SxCR_PSIZE;  // 00: Half-word (8-bit)
	DMA1_Stream0->CR &= ~DMA_SxCR_MSIZE;  // 00: Half-word (8-bit)

	// enable transfer complete interrupt
	DMA1_Stream0->CR |= DMA_SxCR_TCIE;

	// enable interrupt in NVIC
	NVIC_SetPriority(DMA1_Stream0_IRQn, 1);
	NVIC_EnableIRQ(DMA1_Stream0_IRQn);
}


void DMA_config(uint32_t srcAddr, uint32_t desAddr, uint16_t size) {
	DMA1_Stream0->CR &= ~DMA_SxCR_EN;
	while(DMA1_Stream0->CR & DMA_SxCR_EN);

    // clear old DMA flags
    DMA1->LIFCR =
          DMA_LIFCR_CFEIF0
        | DMA_LIFCR_CDMEIF0
        | DMA_LIFCR_CTEIF0
        | DMA_LIFCR_CHTIF0
        | DMA_LIFCR_CTCIF0;

	DMA1_Stream0->NDTR = size;
	DMA1_Stream0->PAR = srcAddr;
	DMA1_Stream0->M0AR = desAddr;

	DMA1_Stream0->CR |= DMA_SxCR_EN; // 1: Stream enable / DMA1 enable
}
