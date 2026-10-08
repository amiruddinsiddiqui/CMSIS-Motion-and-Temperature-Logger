/*
 * spi.c
 *
 *  Created on: Oct 4, 2026
 *      Author: asus
 */


#include "stm32f4xx.h"
#include "spi.h"




void GPIOB_4_SPI2_init(void) {
    // GPIOB clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    // SPI2 clock
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    GPIOB->MODER &= ~(3U << (2*12));
    GPIOB->MODER |= (1U << (2*12)); // CS

    GPIOB->MODER &= ~(3U << (2*8));
    GPIOB->MODER |= (1U << (2*8)); // DC

    // PB9 = TFT RESET
    GPIOB->MODER &= ~(3U << (2 * 9));
    GPIOB->MODER |=  (1U << (2 * 9)); // Output

    GPIOB->OSPEEDR |= (3U << (2*13)) |
                      (3U << (2*14)) |
                      (3U << (2*15));

    GPIOB->MODER &= ~((3U<<(2*13)) | (3U<<(2*14)) | (3U<<(2*15)));
    GPIOB->MODER |= (2U<<(2*13)) | (2U<<(2*14)) | (2U<<(2*15));   // sck, miso, mosi

    GPIOB->AFR[1] &= ~((15U << 20) | (15U << 24) | (15U << 28));
    GPIOB->AFR[1] |= ((5U << 20) | (5U << 24) | (5U << 28));
}

void SPI2_Init(void) {
    // SPI2 clock
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    SPI2->CR1 &= ~SPI_CR1_CPHA;
    SPI2->CR1 &= ~SPI_CR1_CPOL;
    SPI2->CR1 |= SPI_CR1_MSTR;

    SPI2->CR1 &= ~SPI_CR1_DFF; // 0 -> 8 bit data frame

    SPI2->CR1 &= ~SPI_CR1_BR;
    SPI2->CR1 |= SPI_CR1_BR_0;    // 001 = PCLK1/4 = 10.5 MHz

    SPI2->CR1 |= SPI_CR1_SSI;
    SPI2->CR1 |= SPI_CR1_SSM;

//    SPI2->CR1 |= SPI_CR1_SPE;

//    SPI2->CR2 |= SPI_CR2_TXEIE;
//    NVIC_SetPriority(SPI2_IRQn, 3);
//    NVIC_EnableIRQ(SPI2_IRQn);
}

void SPI2_Enable(void) {
    SPI2->CR1 |= SPI_CR1_SPE;
}
void SPI2_Disable(void) {
    SPI2->CR1 &= ~SPI_CR1_SPE;
}

void SPI2_CS(uint8_t state) {
	if(state == 1) {
		GPIOB->ODR |= (1U << 12);
	} else if(state == 0) {
		GPIOB->ODR &= ~(1U << 12);
	}
}



void SPI2_Transmit(uint8_t * data, int size) {

    for (int i = 0; i < size; i++) {
        while (!(SPI2->SR & SPI_SR_TXE));
        SPI2->DR = data[i];
    }
    while (SPI2->SR & SPI_SR_BSY) {} // currently busy with a transfer
}

//volatile uint8_t SP2_send_data_flag = 0;

//void SPI2_IRQHandler(void) {
//	if(SPI2->SR & SPI_SR_TXE) {
//		SP2_send_data_flag = 1;
//		SPI2->CR2 &= ~SPI_CR2_TXEIE;
//	}
//}


