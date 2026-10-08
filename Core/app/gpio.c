/*
 * gpio.c
 *
 *  Created on: Oct 3, 2026
 *      Author: asus
 */


#include "spi.h"
#include "i2c.h"
#include "gpio.h"


void GPIO_init(void) {
	GPIOB_4_I2C1_init();
	GPIOB_4_SPI2_init();
}

