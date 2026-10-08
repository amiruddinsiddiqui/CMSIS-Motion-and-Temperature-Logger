/*
 * dma.h
 *
 *  Created on: Oct 3, 2026
 *      Author: asus
 */

#ifndef CUSTOM_DRIVER_DMA_DMA_H_
#define CUSTOM_DRIVER_DMA_DMA_H_


void DMA_init(void);
void DMA_config(uint32_t srcAddr, uint32_t desAddr, uint16_t size);


#endif /* CUSTOM_DRIVER_DMA_DMA_H_ */
