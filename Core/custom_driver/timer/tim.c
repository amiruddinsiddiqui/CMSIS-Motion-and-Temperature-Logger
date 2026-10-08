/*
 * tim.c
 *
 *  Created on: Oct 3, 2026
 *      Author: asus
 */


#include "stm32f4xx.h"
#include "tim.h"

volatile uint8_t trigger_mpu_read_timer = 0;

void TIM2_Init_10Hz(void) {

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Prescaler: 42 MHz / 4200 = 10 kHz (0.1 ms per tick)
    TIM2->PSC = 4200 - 1;

    // CHANGED: 2,500 counts = 250 ms (4 Hz)
    TIM2->ARR = 1000 - 1;

    // Enable update interrupt
    TIM2->DIER |= TIM_DIER_UIE;

    // Enable TIM2 IRQ in NVIC
    NVIC_SetPriority(TIM2_IRQn, 2);
    NVIC_EnableIRQ(TIM2_IRQn);

    // Start timer
    TIM2->CR1 |= TIM_CR1_CEN;
}

// TIM2 Interrupt Handler (Fires 4 times every second)
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF; // Clear interrupt flag

        trigger_mpu_read_timer = 1;
    }
}



