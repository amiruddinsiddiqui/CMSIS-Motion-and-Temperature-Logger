/*
 * logging.h
 *
 *  Created on: Oct 5, 2026
 *      Author: asus
 */



#ifndef LOGGING_H
#define LOGGING_H

#include <stdint.h>
#include "fatfs.h"


extern char str_ax[12], str_ay[12], str_az[12];
extern char str_gx[12], str_gy[12], str_gz[12], str_temp[12];

FRESULT Logger_Init(const char *filename);
FRESULT Logger_WriteData(uint16_t year, uint8_t month, uint8_t day,
                         uint8_t hour, uint8_t minute, uint8_t second,
                         float ax, float ay, float az,
                         float gx, float gy, float gz,
                         float temp);

void float_to_str(float val, char *out_buf, size_t buf_size);
void Logger_Close(void);

#endif /* LOGGING_H */


