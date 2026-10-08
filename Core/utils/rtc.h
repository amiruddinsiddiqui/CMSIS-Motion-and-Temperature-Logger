/*
 * rtc.h
 *
 *  Created on: Oct 5, 2026
 *      Author: asus
 */

#ifndef UTILS_RTC_H_
#define UTILS_RTC_H_


#include "stm32f4xx.h"
#include <stdint.h>

void RTC_Init(void);

void RTC_GetTime(
    uint16_t *year,
    uint8_t *month,
    uint8_t *day,
    uint8_t *hour,
    uint8_t *minute,
    uint8_t *second
);


#endif /* UTILS_RTC_H_ */
