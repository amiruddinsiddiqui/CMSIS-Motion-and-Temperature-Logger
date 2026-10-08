/*
 * rtc.c
 *
 *  Created on: Oct 5, 2026
 *      Author: asus
 */

#include "rtc.h"


void RTC_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_DBP;

    /* Enable LSE */
    RCC->BDCR |= RCC_BDCR_LSEON;
    while (!(RCC->BDCR & RCC_BDCR_LSERDY));

    /* Select LSE */
    RCC->BDCR &= ~RCC_BDCR_RTCSEL;
    RCC->BDCR |= RCC_BDCR_RTCSEL_0;

    /* Enable RTC */
    RCC->BDCR |= RCC_BDCR_RTCEN;

    /*
     * Configure RTC only once.
     * BKP0R survives MCU reset.
     */
    if (RTC->BKP0R != 0x1234) {
        RTC->WPR = 0xCA;
        RTC->WPR = 0x53;

        /* Enter initialization mode */
        RTC->ISR |= RTC_ISR_INIT;
        while (!(RTC->ISR & RTC_ISR_INITF));

        /* 32768 Hz / 128 / 256 = 1 Hz */
        RTC->PRER = (127U << 16) | 255U;

        /*
         * Set initial time:
         * 21:35:00
         *
         * BCD:
         * 21 = 0x21
         * 35 = 0x35
         * 00 = 0x00
         */
        RTC->TR =
            (0x21 << 16) |
            (0x35 << 8)  |
            (0x00);

        /*
         * Set initial date:
         * 05/10/2026
         *
         * year = 26 = 0x26
         * month = 10 = 0x10
         * day = 05 = 0x05
         */
        RTC->DR =
            (0x26 << 16) |
            (0x10 << 8)  |
            (0x05);

        /* Leave initialization mode */
        RTC->ISR &= ~RTC_ISR_INIT;

        while (!(RTC->ISR & RTC_ISR_RSF));

        /* Mark initialized */
        RTC->BKP0R = 0x1234;

        RTC->WPR = 0xFF;
    }

}

void RTC_GetTime(
    uint16_t *year,
    uint8_t *month,
    uint8_t *day,
    uint8_t *hour,
    uint8_t *minute,
    uint8_t *second)
{


    uint32_t tr = RTC->TR;
    uint32_t dr = RTC->DR;

    uint8_t h10 = (tr >> 20) & 0x03;
    uint8_t h1  = (tr >> 16) & 0x0F;

    uint8_t m10 = (tr >> 12) & 0x07;
    uint8_t m1  = (tr >> 8)  & 0x0F;

    uint8_t s10 = (tr >> 4) & 0x07;
    uint8_t s1  = tr & 0x0F;

    uint8_t y10 = (dr >> 20) & 0x0F;
    uint8_t y1  = (dr >> 16) & 0x0F;

    uint8_t mo10 = (dr >> 12) & 0x01;
    uint8_t mo1  = (dr >> 8) & 0x0F;

    uint8_t d10 = (dr >> 4) & 0x03;
    uint8_t d1  = dr & 0x0F;

    *hour   = h10 * 10 + h1;
    *minute = m10 * 10 + m1;
    *second = s10 * 10 + s1;

    *year  = 2000 + y10 * 10 + y1;
    *month = mo10 * 10 + mo1;
    *day   = d10 * 10 + d1;
}


