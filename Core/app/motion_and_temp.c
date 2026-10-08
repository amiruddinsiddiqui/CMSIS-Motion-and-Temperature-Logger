/*
 * motion_and_temp.c
 *
 *  Created on: Oct 3, 2026
 *      Author: asus
 */

#include "mpu6500.h"
#include "motion_and_temp.h"
#include <stdio.h>


volatile int16_t Accel_X_RAW = 0;
volatile int16_t Accel_Y_RAW = 0;
volatile int16_t Accel_Z_RAW = 0;

volatile int16_t Gyro_X_RAW = 0;
volatile int16_t Gyro_Y_RAW = 0;
volatile int16_t Gyro_Z_RAW = 0;

volatile int16_t Temperature_RAW = 0;

volatile float Ax, Ay, Az, Gx, Gy, Gz, Tempr;


uint8_t Rx_data[14];
//float processed_MPU_data[7];

void process_mpu6500_motion_and_temp(void) {
    Accel_X_RAW = (int16_t)((Rx_data[0] << 8) | Rx_data[1]);
    Accel_Y_RAW = (int16_t)((Rx_data[2] << 8) | Rx_data[3]);
    Accel_Z_RAW = (int16_t)((Rx_data[4] << 8) | Rx_data[5]);

    Temperature_RAW = (int16_t)((Rx_data[6] << 8) | Rx_data[7]);

    Gyro_X_RAW = (int16_t)((Rx_data[8] << 8) | Rx_data[9]);
    Gyro_Y_RAW = (int16_t)((Rx_data[10] << 8) | Rx_data[11]);
    Gyro_Z_RAW = (int16_t)((Rx_data[12] << 8) | Rx_data[13]);

    // Conversion factors for FS_SEL = 0
    Ax = (float)Accel_X_RAW / 16384.0f;
    Ay = (float)Accel_Y_RAW / 16384.0f;
    Az = (float)Accel_Z_RAW / 16384.0f;

    Gx = (float)Gyro_X_RAW / 131.0f;
    Gy = (float)Gyro_Y_RAW / 131.0f;
    Gz = (float)Gyro_Z_RAW / 131.0f;

    // formula for MPU6500 temperature sensor in Celsius
    Tempr = ((float)Temperature_RAW / 333.87f) + 21.0f;
}



//void uart_send_data(void) {
//	printf("\n");
//	for(int i = 0; i<7; i++) {
//		printf("data: %d", processed_MPU_data[i]);
//	}
//	printf("\r\n");
//}

