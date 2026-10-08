/*
 * app.c
 *
 *  Created on: Oct 3, 2026
 *      Author: asus
 */



#include "gpio.h"
#include "app.h"
#include "mpu6500.h"
#include "motion_and_temp.h"
#include "tim.h"
#include "dma.h"
#include "i2c.h"
#include "uart.h"
#include "rtc.h"
#include "logging.h"
#include "spi.h"
#include <stdio.h>
#include "TFT_ILI9341.h"




void APP(void) {
    GPIO_init();

    SPI2_Init();
    SPI2_Enable(); // enable spi2

    TFT_Init();

    I2C1_init();
    UART_init();
    TIM2_Init_10Hz();
    MPU6500_Init();
    DMA_init();
    RTC_Init();

    // Initialize SD Card Logger
    if (Logger_Init("mpu_log.csv") != FR_OK) {
        printf("SD Logger initialization failed!\r\n");
    } else {
        printf("SD Logger initialized successfully.\r\n");
    }

    while (1) {
        if (trigger_mpu_read_timer) {
            trigger_mpu_read_timer = 0;
            MPU6500_Start_Read_DMA();
        }

        if (mpu_data_ready) {
            mpu_data_ready = 0;
            process_mpu6500_motion_and_temp();

            uint16_t year;
            uint8_t month, day, hour, minute, second;
            RTC_GetTime(&year, &month, &day, &hour, &minute, &second);

            // Log single line
            FRESULT result = Logger_WriteData(year, month, day, hour, minute, second,
                             	 Ax, Ay, Az, Gx, Gy, Gz, Tempr);


			// print over UART using string formatting (%s)
			printf("Ax:%s Ay:%s Az:%s | Gx:%s Gy:%s Gz:%s | Temp:%s | SD:%d\r\n",
					str_ax, str_ay, str_az, str_gx, str_gy, str_gz, str_temp, result);

            TFT_DisplayData();
        }

    }

}





