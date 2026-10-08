/*
 * mpu6500.h
 *
 *  Created on: Oct 3, 2026
 *      Author: asus
 */

#ifndef CUSTOM_DRIVER_MPU6500_MPU6500_H_
#define CUSTOM_DRIVER_MPU6500_MPU6500_H_
#include <stdint.h>

// Flag to notify main thread that new raw I2C data has arrived
extern volatile uint8_t mpu_data_ready;
extern uint8_t Rx_data[14];

void MPU_write (uint8_t Address, uint8_t Reg, uint8_t Data);
void MPU_read (uint8_t Address, uint8_t Reg, uint8_t *buffer, uint8_t size);
void MPU6500_Init (void);
void MPU6500_Start_Read_DMA(void);
void DMA1_Stream0_IRQHandler(void);


#endif /* CUSTOM_DRIVER_MPU6500_MPU6500_H_ */
