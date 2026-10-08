/*
 * mpu6500.c
 *
 *  Created on: Oct 3, 2026
 *      Author: asus
 */

#include "stm32f4xx.h"
#include "i2c.h"
#include <stdint.h>
#include "mpu6500.h"
#include "dma.h"


#define MPU6500_ADDR       0xD0
#define SMPLRT_DIV_REG     0x19
#define GYRO_CONFIG_REG    0x1B
#define ACCEL_CONFIG_REG   0x1C
#define ACCEL_XOUT_H_REG   0x3B
#define TEMP_OUT_H_REG     0x41
#define GYRO_XOUT_H_REG    0x43
#define PWR_MGMT_1_REG     0x6B
#define WHO_AM_I_REG       0x75





volatile uint8_t mpu_data_ready = 0;


void MPU6500_Init (void) {
	uint8_t check;
	uint8_t Data;

	// check device ID WHO_AM_I
	MPU_read(MPU6500_ADDR, WHO_AM_I_REG, &check, 1);

	if (check == 0x70) {  // 0x68 will be returned by the sensor if everything goes well
		// power management register 0X6B we should write all 0's to wake the sensor up
		Data = 0;
		MPU_write (MPU6500_ADDR, PWR_MGMT_1_REG, Data);

		// Set DATA RATE of 1KHz by writing SMPLRT_DIV register
		Data = 0x07;
		MPU_write(MPU6500_ADDR, SMPLRT_DIV_REG, Data);

		// Set accelerometer configuration in ACCEL_CONFIG Register
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> ? 2g
		Data = 0x00;
		MPU_write(MPU6500_ADDR, ACCEL_CONFIG_REG, Data);

		// Set Gyroscopic configuration in GYRO_CONFIG Register
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> ? 250 ?/s
		Data = 0x00;
		MPU_write(MPU6500_ADDR, GYRO_CONFIG_REG, Data);
	}

}


void MPU6500_Start_Read_DMA(void) {
    // prepare DMA to listen on I2C1 Data Register
    DMA_config((uint32_t)&(I2C1->DR), (uint32_t)Rx_data, 14);

    // set MPU register pointer to 0x3B (ACCEL_XOUT_H)
    I2C_start();
    I2C_address(MPU6500_ADDR);
    I2C_write(ACCEL_XOUT_H_REG);
    I2C_start(); // repeated Start -> Switch to Read mode
    I2C_address(MPU6500_ADDR + 1); // 0xD1 Read address

    // hand off byte reception to DMA hardware
    I2C1->CR2 |= I2C_CR2_LAST;   // automatically NACK final byte
    I2C1->CR1 |= I2C_CR1_ACK;    // enable ACK generation
    I2C1->CR2 |= I2C_CR2_DMAEN;  // enable I2C DMA Requests
}


void MPU_write (uint8_t Address, uint8_t Reg, uint8_t Data) {
    I2C_start();
    I2C_address(Address);
    I2C_write(Reg);
    I2C_write(Data);
    I2C_stop();
}

void MPU_read (uint8_t Address, uint8_t Reg, uint8_t *buffer, uint8_t size) {
    I2C_start();
    I2C_address(Address);
    I2C_write(Reg);
    I2C_start();  // repeated start
    I2C_read(Address + 0x01, buffer, size);
    I2C_stop();
}



// DMA Interrupt Handler (Fires after all 14 bytes arrive ~350 µs later)
void DMA1_Stream0_IRQHandler(void) {
    if (DMA1->LISR & DMA_LISR_TCIF0) {
        DMA1->LIFCR |= DMA_LIFCR_CTCIF0; // clear flag

        // Terminate I2C transaction
        I2C1->CR2 &= ~I2C_CR2_DMAEN;
        I2C_stop();

        mpu_data_ready = 1;
    }
}



