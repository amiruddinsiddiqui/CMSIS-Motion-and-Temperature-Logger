/*
 * i2c.h
 *
 *  Created on: Oct 3, 2026
 *      Author: asus
 */

#ifndef CUSTOM_DRIVER_I2C_I2C_H_
#define CUSTOM_DRIVER_I2C_I2C_H_


#include <stdint.h>

void GPIOB_4_I2C1_init(void);
void I2C1_init(void);
void I2C_start(void);
void I2C_stop(void);
void I2C_address(uint8_t slave_address);
void I2C_write(uint8_t data);
uint8_t I2C_read(uint8_t address, uint8_t * buffer, uint8_t size);



#endif /* CUSTOM_DRIVER_I2C_I2C_H_ */
