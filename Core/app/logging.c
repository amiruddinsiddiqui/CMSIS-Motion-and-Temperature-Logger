/*
 * logging.c
 *
 *  Created on: Oct 5, 2026
 *      Author: asus
 */


#include "main.h"
#include "logging.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static FATFS fs;
static FIL fil;
static char log_filename[32];
static uint8_t is_initialized = 0;

char str_ax[12], str_ay[12], str_az[12];
char str_gx[12], str_gy[12], str_gz[12], str_temp[12];

extern SPI_HandleTypeDef hspi1;


/* Safe float to string conversion without float printf dependencies */
void float_to_str(float val, char *out_buf, size_t buf_size) {
    if (val < 0.0f) {
        val = -val;
        int32_t int_part = (int32_t)val;
        int32_t frac_part = (int32_t)((val - (float)int_part) * 100.0f);
        snprintf(out_buf, buf_size, "-%ld.%02ld", (long)int_part, (long)frac_part);
    } else {
        int32_t int_part = (int32_t)val;
        int32_t frac_part = (int32_t)((val - (float)int_part) * 100.0f);
        snprintf(out_buf, buf_size, "%ld.%02ld", (long)int_part, (long)frac_part);
    }
}


FRESULT Logger_Init(const char *filename) {
    FRESULT fres;

    strncpy(log_filename, filename, sizeof(log_filename) - 1);

    // 1. Mount SD Card at low clock speed (~328 kHz)
    fres = f_mount(&fs, "", 1);
    printf("f_mount status: %d\r\n", fres);
    if (fres != FR_OK) return fres;

    // 2. Switch SPI1 to High Speed (~10.5 MHz) for fast operations
    __HAL_SPI_DISABLE(&hspi1);
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; // 84MHz / 8 = 10.5 MHz
    HAL_SPI_Init(&hspi1);
    __HAL_SPI_ENABLE(&hspi1);

    // 3. Open or create log file
    fres = f_open(&fil, log_filename, FA_OPEN_APPEND | FA_WRITE);
    printf("Header f_open status: %d\r\n", fres);

    if (fres == FR_OK) {
        if (f_size(&fil) == 0) {
            f_puts("Timestamp,Ax,Ay,Az,Gx,Gy,Gz,Temp\r\n", &fil);
        }
        f_sync(&fil);
        is_initialized = 1;
    }

    return fres;
}



static uint16_t sync_counter = 0;

FRESULT Logger_WriteData(uint16_t year, uint8_t month, uint8_t day,
                         uint8_t hour, uint8_t minute, uint8_t second,
                         float ax, float ay, float az,
                         float gx, float gy, float gz,
                         float temp) {

    if (!is_initialized) return FR_NOT_READY;

    FRESULT fres;
    UINT bytes_written;
    char csv_buffer[160];


    float_to_str(ax, str_ax, sizeof(str_ax));
    float_to_str(ay, str_ay, sizeof(str_ay));
    float_to_str(az, str_az, sizeof(str_az));
    float_to_str(gx, str_gx, sizeof(str_gx));
    float_to_str(gy, str_gy, sizeof(str_gy));
    float_to_str(gz, str_gz, sizeof(str_gz));
    float_to_str(temp, str_temp, sizeof(str_temp));

    snprintf(csv_buffer, sizeof(csv_buffer),
             "%04d-%02d-%02d %02d:%02d:%02d,%s,%s,%s,%s,%s,%s,%s\r\n",
             year, month, day, hour, minute, second,
             str_ax, str_ay, str_az,
             str_gx, str_gy, str_gz,
             str_temp);

    fres = f_write(&fil, csv_buffer, strlen(csv_buffer), &bytes_written);

    if (fres == FR_OK) {
        // Sync to SD physical flash every 50 iterations (~5s at 10Hz), 50 samples every 5 seconds
        if (++sync_counter >= 50) {
            sync_counter = 0;
            f_sync(&fil);
        }
    } else {
        // Recovery attempt if write fails
        f_open(&fil, log_filename, FA_OPEN_APPEND | FA_WRITE);
    }

    return fres;
}

void Logger_Close(void) {
    if (is_initialized) {
        f_sync(&fil);
        f_close(&fil);
        is_initialized = 0;
    }
}



