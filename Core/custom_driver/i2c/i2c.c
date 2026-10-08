/*
 * i2c.c
 *
 *  Created on: Oct 3, 2026
 *      Author: asus
 */


#include "stm32f4xx.h"
#include <stdint.h>
#include "i2c.h"


// MotionSense: 3-Axis Motion & Temperature Monitoring & Data Logger


void GPIOB_4_I2C1_init(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	GPIOB->MODER &= ~(3U << (2*6));
	GPIOB->MODER |= (2U << (2*6)); // scl
	GPIOB->MODER &= ~(3U << (2*7));
	GPIOB->MODER |= (2U << (2*7)); // sda

	GPIOB->OTYPER |= GPIO_OTYPER_OT6; // 1: Output open-drain
	GPIOB->OTYPER |= GPIO_OTYPER_OT7; // 1: Output open-drain

	// switching speed
	GPIOB->OSPEEDR &= ~(3U << (2*6));
	GPIOB->OSPEEDR |= (2U << (2*6)); // scl, 10: High speed
	GPIOB->OSPEEDR &= ~(3U << (2*7));
	GPIOB->OSPEEDR |= (2U << (2*7)); // sda, 10: High speed

	// 00: no pullup and pulldown
	GPIOB->PUPDR &= ~(3U << (2*6));
	GPIOB->PUPDR &= ~(3U << (2*7));

	// GPIOB as alternate function 0100: AF4
	GPIOB->AFR[0] &= ~(15U << (4*6));
	GPIOB->AFR[0] |= (4U << (4*6)); // scl
	GPIOB->AFR[0] &= ~(15U << (4*7));
	GPIOB->AFR[0] |= (4U << (4*7)); // sda
}

void I2C1_init(void) {
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	I2C1->CR1 |= I2C_CR1_SWRST;
	I2C1->CR1 &= ~I2C_CR1_SWRST;

	I2C1->CR2 |= (42U<<0);

	I2C1->CCR &= ~I2C_CCR_FS;
	I2C1->CCR &= ~I2C_CCR_DUTY;

	// (1000 + 4*1000)/23.81 = ~210
	I2C1->CCR |= (210U<<0); // SCL = 100KHz
	I2C1->TRISE |= (43U<<0);


	I2C1->CR1 |= I2C_CR1_PE;
}

void I2C_start(void) {
	I2C1->CR1 |= I2C_CR1_ACK; // repeated start ack
	I2C1->CR1 |= I2C_CR1_START;

	while(!(I2C1->SR1 & I2C_SR1_SB));
}

void I2C_stop(void) {
	I2C1->CR1 |= I2C_CR1_STOP;
}

void I2C_address(uint8_t slave_address) {
	I2C1->DR = slave_address;

	while(!(I2C1->SR1 & I2C_SR1_ADDR));

	// reading to clear bit
	(void)I2C1->SR1;
	(void)I2C1->SR2;
}


void I2C_write(uint8_t data) {
	while(!(I2C1->SR1 & I2C_SR1_TXE));
	I2C1->DR = data;
	while(!(I2C1->SR1 & I2C_SR1_BTF));
}



uint8_t I2C_read(uint8_t address, uint8_t * buffer, uint8_t size) {
	uint8_t remaining = size;

	if(size == 1) {
		I2C1->DR = address;
		while((I2C1->SR1 & I2C_SR1_ADDR) == 0); // wait until ADDR bit set to 1

		I2C1->CR1 &= ~I2C_CR1_ACK; // if 1 Byte only is received then Master needs to send NACK to Slave

		// read sr1 and sr2 to clear the ADDR bit
		(void)I2C1->SR1;
		(void)I2C1->SR2;

		I2C1->CR1 |= I2C_CR1_STOP; // set stop bit to 1 to stop the i2c

		while(!(I2C1->SR1 & I2C_SR1_RXNE)); // wait for RXNE to set

		buffer[size-remaining] = I2C1->DR;
	}
	else {
		I2C1->DR = address;
		while(!(I2C1->SR1 & I2C_SR1_ADDR));
//		uint8_t temp = I2C1->SR1 | I2C1->SR2;
		volatile uint32_t temp;
		temp = I2C1->SR1;
		temp = I2C1->SR2;
		(void)temp; // suppress unused variable warning

		while(remaining > 2) {
			while(!(I2C1->SR1 & I2C_SR1_RXNE));
			buffer[size-remaining] = I2C1->DR;
			I2C1->CR1 |= I2C_CR1_ACK;
			remaining--;
		}

		// read 2nd last byte
		while(!(I2C1->SR1 & I2C_SR1_RXNE));
		buffer[size-remaining] = I2C1->DR;
		I2C1->CR1 &= ~I2C_CR1_ACK;
		I2C1->CR1 |= I2C_CR1_STOP;

		remaining--;

		// last byte
		while(!(I2C1->SR1 & I2C_SR1_RXNE));
		buffer[size-remaining] = I2C1->DR;

	}

}

