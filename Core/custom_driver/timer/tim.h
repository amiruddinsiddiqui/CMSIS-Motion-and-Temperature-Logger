/*
 * tim.h
 *
 *  Created on: Oct 3, 2026
 *      Author: asus
 */

#ifndef CUSTOM_DRIVER_TIMER_TIM_H_
#define CUSTOM_DRIVER_TIMER_TIM_H_

extern volatile uint8_t trigger_mpu_read_timer;


void TIM2_Init_10Hz(void);
void TIM2_IRQHandler(void);


#endif /* CUSTOM_DRIVER_TIMER_TIM_H_ */
