/*
 * TFT_ILI9341.h
 *
 *  Created on: Oct 7, 2026
 *      Author: asus
 */

#ifndef CUSTOM_DRIVER_LCD_TFT_ILI9341_H_
#define CUSTOM_DRIVER_LCD_TFT_ILI9341_H_


#include <stdint.h>

#define HIGH        1
#define LOW         0

void TFT_Init(void);
void TFT_DC(uint8_t state);
void TFT_Write_Command(uint8_t cmd);
void TFT_Write_Data(uint8_t *data, int size);
void TFT_Reset(void);
void TFT_FillScreen(uint16_t color);

void TFT_SetAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void TFT_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void TFT_DrawChar(uint16_t x, uint16_t y, char c);
void TFT_PrintString(uint16_t x, uint16_t y, char *str);
void TFT_DisplayData(void);
void TFT_PrintFloat(uint16_t x, uint16_t y, const char *name, float value);


#endif /* CUSTOM_DRIVER_LCD_TFT_ILI9341_H_ */
