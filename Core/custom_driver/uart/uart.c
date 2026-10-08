/*
 * uart.c
 *
 *  Created on: Oct 5, 2026
 *      Author: asus
 */

#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "uart.h"

#define BAUD_RATE 			115200
#define PERIPHERAL_CLOCK	42000000


static uint16_t compute_baud_rate(uint32_t p_clock, uint32_t baud_rate);


void UART_init(void) {
	// enable GPIOA and UART2, mode for AF for usart2, select USART2 tx rx in AF
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Give clock to USART2 so its hardware/registers can operate


	GPIOA->MODER &= ~(3U << (2*2));
	GPIOA->MODER |= (2U << (2*2));   // Tx
	GPIOA->MODER &= ~(3U << (2*3));
	GPIOA->MODER |= (2U << (2*3));   // Rx

	GPIOA->AFR[0] &= ~(15U << (4*2));
	GPIOA->AFR[0] |= (7U << (4*2));   // Tx
	GPIOA->AFR[0] &= ~(15U << (4*3));
	GPIOA->AFR[0] |= (7U << (4*3));   // Rx


	USART2->CR1 &= ~USART_CR1_OVER8; // 0 : oversampling by 16
	// Calculate the USART BRR value when i put USART2->BRR=365:
	// Internally, USARTDIV is calculated first:
	// USARTDIV = PCLK / (16 × Baud)
	//          = 42,000,000 / (16 × 115,200) = 22.7865
	//
	// Then split USARTDIV into:
	// Mantissa = 22
	// Fraction = 0.7865
	//
	// Convert fraction to 4-bit value:
	// Fraction = 0.7865 × 16 ≈ 13
	//
	// Store them in BRR:
	// BRR = (Mantissa << 4) | Fraction
	//     = (22 << 4) | 13
	//     = 352 + 13 = 365
	USART2->BRR = compute_baud_rate(PERIPHERAL_CLOCK, BAUD_RATE);

	USART2->CR1 |= USART_CR1_TE; // enable tx
	USART2->CR1 |= USART_CR1_RE; // enable rx

	USART2->CR1 |= USART_CR1_UE; // enable usart2 module // Enable USART2 to start UART communication
}

 // static: this function is not accessible outside this .c file
static uint16_t compute_baud_rate(uint32_t p_clock, uint32_t baud_rate) {
	// this formula is just as baud_rate = peripheralclock/baudrate
	// but the below formula handles the floor & ceiling of integer value
	return ((p_clock + (baud_rate/2))/baud_rate);
}

void send_char(uint8_t data) {
	while(!(USART2->SR & USART_SR_TXE));
	USART2->DR = data;
}

int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        send_char((uint8_t)ptr[i]);
    }
    return len;
}

/* Redirect printf char output to USART2 */
int __io_putchar(int ch) {
    send_char((uint8_t)ch);
    return ch;
}

