/*
 * spi.h
 *
 *  Created on: Oct 7, 2026
 *      Author: asus
 */

#ifndef CUSTOM_DRIVER_SPI_SPI_H_
#define CUSTOM_DRIVER_SPI_SPI_H_


#include <stdint.h>

extern volatile uint8_t SP2_send_data_flag;

void GPIOB_4_SPI2_init(void);
void SPI2_Init(void);
void SPI2_Enable(void);
void SPI2_Disable(void);
void SPI2_CS(uint8_t arg);
void SPI2_Transmit(uint8_t * data, int size);


#endif /* CUSTOM_DRIVER_SPI_SPI_H_ */
