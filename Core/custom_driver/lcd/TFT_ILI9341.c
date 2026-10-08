///*
// * TFT_ILI9341.c
// *
// *  Created on: Oct 7, 2026
// *      Author: asus
// */
//
//#include "TFT_ILI9341.h"
//#include "spi.h"
//#include <stdint.h>
//#include "stm32f4xx.h"
//#include <stdio.h>
//#include "motion_and_temp.h"
//
//
//
//#define Software_Reset              0x01 // Reset controller
//#define Sleep_Out                   0x11 // Wake display
//#define Pixel_Format_Set            0x3A // Select pixel format
//#define Memory_Access_Control       0x36 // Orientation/RGB order
//#define Column_Address_Set          0x2A // X range
//#define Page_Address_Set            0x2B // Y range
//#define Memory_Write                0x2C // Send pixels
//#define Display_ON                  0x29 // Turn display output on
//
//#define Pixel_Format                0x55 // 16 bits / pixel
//#define MADCTL_VALUE 				0x08 // MX=1, BGR=1
//
//
//
//
//static const uint8_t font5x7[128][5] = {
//    ['0'] = {0x3E, 0x51, 0x49, 0x45, 0x3E},
//    ['1'] = {0x00, 0x42, 0x7F, 0x40, 0x00},
//    ['2'] = {0x42, 0x61, 0x51, 0x49, 0x46},
//    ['3'] = {0x21, 0x41, 0x45, 0x4B, 0x31},
//    ['4'] = {0x18, 0x14, 0x12, 0x7F, 0x10},
//    ['5'] = {0x27, 0x45, 0x45, 0x45, 0x39},
//    ['6'] = {0x3C, 0x4A, 0x49, 0x49, 0x30},
//    ['7'] = {0x01, 0x71, 0x09, 0x05, 0x03},
//    ['8'] = {0x36, 0x49, 0x49, 0x49, 0x36},
//    ['9'] = {0x06, 0x49, 0x49, 0x29, 0x1E},
//
//    ['A'] = {0x7E, 0x11, 0x11, 0x11, 0x7E},
//    ['B'] = {0x7F, 0x49, 0x49, 0x49, 0x36},
//    ['C'] = {0x3E, 0x41, 0x41, 0x41, 0x22},
//    ['D'] = {0x7F, 0x41, 0x41, 0x22, 0x1C},
//    ['E'] = {0x7F, 0x49, 0x49, 0x49, 0x41},
//    ['F'] = {0x7F, 0x09, 0x09, 0x09, 0x01},
//    ['G'] = {0x3E, 0x41, 0x49, 0x49, 0x7A},
//    ['T'] = {0x01, 0x01, 0x7F, 0x01, 0x01},
//
//    ['a'] = {0x20, 0x54, 0x54, 0x54, 0x78},
//    ['c'] = {0x38, 0x44, 0x44, 0x44, 0x20},
//    ['d'] = {0x38, 0x44, 0x44, 0x48, 0x7F},
//    ['g'] = {0x18, 0xA4, 0xA4, 0xA4, 0x7C},
//    ['m'] = {0x7C, 0x04, 0x18, 0x04, 0x78},
//    ['p'] = {0xF8, 0x24, 0x24, 0x24, 0x18},
//    ['x'] = {0x44, 0x28, 0x10, 0x28, 0x44},
//
//	['y'] = {0x0C, 0x50, 0x50, 0x50, 0x3C},
//	['z'] = {0x44, 0x64, 0x54, 0x4C, 0x44},
//	['e'] = {0x38, 0x54, 0x54, 0x54, 0x18},
//	['s'] = {0x48, 0x54, 0x54, 0x54, 0x20},
//
//    [':'] = {0x00, 0x36, 0x36, 0x00, 0x00},
//    ['.'] = {0x00, 0x40, 0x60, 0x00, 0x00},
//    ['-'] = {0x08, 0x08, 0x08, 0x08, 0x08},
//    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00}
//};
//
//
//
//void TFT_Init(void) {
//    uint8_t data;
//
//    TFT_Reset();
//
//    TFT_Write_Command(Software_Reset);
//    HAL_Delay(5);
//
//    TFT_Write_Command(Sleep_Out);
//    HAL_Delay(120);
//
//    data = Pixel_Format;
//    TFT_Write_Command(Pixel_Format_Set);
//    TFT_Write_Data(&data, 1);
//
//    data = MADCTL_VALUE;
//    TFT_Write_Command(Memory_Access_Control);
//    TFT_Write_Data(&data, 1);
//
//    TFT_Write_Command(Display_ON);
//    HAL_Delay(5);
//    TFT_FillScreen(0x0000);   // black background
//}
//
//
//void TFT_DC(uint8_t state) {
//    if(state) {
//        GPIOB->BSRR = (1U << 8);         // DC HIGH -> data
//    }
//    else {
//        GPIOB->BSRR = (1U << (8 + 16));  // DC LOW -> command
//    }
//}
//
//
//void TFT_Write_Command(uint8_t cmd) {
//    SPI2_CS(LOW);
//    TFT_DC(LOW); // command mode
//    SPI2_Transmit(&cmd, 1);
//    SPI2_CS(HIGH);
//}
//
//
//void TFT_Write_Data(uint8_t *data, int size) {
//    SPI2_CS(LOW);  // select slave
//    TFT_DC(HIGH);  // data mode
//    SPI2_Transmit(data, size);
//    SPI2_CS(HIGH); // disable slave
//}
//
//
//void TFT_Reset(void) {
//    GPIOB->BSRR = (1U << (9 + 16));   // RESET LOW
//    HAL_Delay(10);
//
//    GPIOB->BSRR = (1U << 9);           // RESET HIGH
//    HAL_Delay(5);
//}
//
//
//void TFT_FillScreen(uint16_t color) {
//    uint8_t data[2];
//
//    data[0] = color >> 8;
//    data[1] = color & 0xFF;
//
//    // Select the entire 240x320 screen
//    TFT_SetAddress(0, 0, 239, 319);
//
//    // Start memory write
//    TFT_Write_Command(Memory_Write);
//
//    // Send color for every pixel
//    for (uint32_t i = 0; i < 240UL * 320UL; i++) {
//        TFT_Write_Data(data, 2);
//    }
//}
//
//
//void TFT_SetAddress(uint16_t x1, uint16_t y1,
//                    uint16_t x2, uint16_t y2) {
//
//    uint8_t data[4];
//
//    // Column Address Set
//    TFT_Write_Command(Column_Address_Set);
//
//    data[0] = x1 >> 8;
//    data[1] = x1 & 0xFF;
//    data[2] = x2 >> 8;
//    data[3] = x2 & 0xFF;
//
//    TFT_Write_Data(data, 4);
//
//
//    // Page Address Set
//    TFT_Write_Command(Page_Address_Set);
//
//    data[0] = y1 >> 8;
//    data[1] = y1 & 0xFF;
//    data[2] = y2 >> 8;
//    data[3] = y2 & 0xFF;
//
//    TFT_Write_Data(data, 4);
//}
//
//
//void TFT_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
//    uint8_t data[2];
//
//    TFT_SetAddress(x, y, x, y);
//
//    TFT_Write_Command(Memory_Write);
//
//    data[0] = color >> 8;
//    data[1] = color & 0xFF;
//
//    TFT_Write_Data(data, 2);
//}
//
//
//void TFT_DrawChar(uint16_t x, uint16_t y, char c) {
//    const uint8_t *bitmap = font5x7[(uint8_t)c];
//
//    uint8_t data[70];
//    uint16_t index = 0;
//
//    for (uint8_t row = 0; row < 7; row++) {
//        for (uint8_t col = 0; col < 5; col++) {
//            if (bitmap[col] & (1U << row)) {
//                // GREEN
//                data[index++] = 0x07;
//                data[index++] = 0xE0;
//            }
//            else {
//                // BLACK
//                data[index++] = 0x00;
//                data[index++] = 0x00;
//            }
//        }
//    }
//
//    TFT_SetAddress(x, y, x + 4, y + 6);
//
//    TFT_Write_Command(Memory_Write);
//
//    TFT_Write_Data(data, 70);
//}
//
//void TFT_PrintString(uint16_t x, uint16_t y, char *str) {
//    while (*str) {
//        TFT_DrawChar(x, y, *str);
//
//        x += 6;       // move 6 pixels to the right for the next character
//        			  // 5 pixels = character width, 1 pixel = spacing
//        str++;
//    }
//}
//
////void TFT_DisplayData(void) {
////
////    TFT_PrintFloat(10, 20,  "Ax: ", Ax);
////    TFT_PrintFloat(10, 40,  "Ay: ", Ay);
////    TFT_PrintFloat(10, 60,  "Az: ", Az);
////
////    TFT_PrintFloat(10, 90,  "Gx: ", Gx);
////    TFT_PrintFloat(10, 110, "Gy: ", Gy);
////    TFT_PrintFloat(10, 130, "Gz: ", Gz);
////
////    TFT_PrintFloat(10, 160, "Temp: ", Tempr);
////}
//
//void TFT_DisplayData(void) {
//    // Only print numbers; keep static labels drawn during TFT_Init()
//    TFT_PrintFloat(50, 20,  "", Ax);
//    TFT_PrintFloat(50, 40,  "", Ay);
//    TFT_PrintFloat(50, 60,  "", Az);
//
//    TFT_PrintFloat(50, 90,  "", Gx);
//    TFT_PrintFloat(50, 110, "", Gy);
//    TFT_PrintFloat(50, 130, "", Gz);
//
//    TFT_PrintFloat(50, 160, "", Tempr);
//}
//
//
//void TFT_PrintFloat(uint16_t x, uint16_t y, const char *name, float value) {
//    char buffer[30];
//
//    int sign = 1;
//
//    if (value < 0) {
//        sign = -1;
//        value = -value;
//    }
//
//    int whole = (int)value;
//    int decimal = (int)((value - whole) * 100);
//
//    if (sign < 0) {
//        sprintf(buffer, "%s-%d.%02d", name, whole, decimal);
//    }
//    else {
//        sprintf(buffer, "%s%d.%02d", name, whole, decimal);
//    }
//
//    TFT_PrintString(x, y, buffer);
//}
//
//




/*
 * TFT_ILI9341.c
 *
 *  Created on: Oct 7, 2026
 *      Author: asus
 */

#include "TFT_ILI9341.h"
#include "spi.h"
#include <stdint.h>
#include "stm32f4xx.h"
#include "logging.h"
#include <stdio.h>
#include <string.h>
#include "motion_and_temp.h"

#define Software_Reset              0x01
#define Sleep_Out                   0x11
#define Pixel_Format_Set            0x3A
#define Memory_Access_Control       0x36
#define Column_Address_Set          0x2A
#define Page_Address_Set            0x2B
#define Memory_Write                0x2C
#define Display_ON                  0x29

#define Pixel_Format                0x55
#define MADCTL_VALUE                0x08

static const uint8_t font5x7[128][5] = {
    ['0'] = {0x3E, 0x51, 0x49, 0x45, 0x3E},
    ['1'] = {0x00, 0x42, 0x7F, 0x40, 0x00},
    ['2'] = {0x42, 0x61, 0x51, 0x49, 0x46},
    ['3'] = {0x21, 0x41, 0x45, 0x4B, 0x31},
    ['4'] = {0x18, 0x14, 0x12, 0x7F, 0x10},
    ['5'] = {0x27, 0x45, 0x45, 0x45, 0x39},
    ['6'] = {0x3C, 0x4A, 0x49, 0x49, 0x30},
    ['7'] = {0x01, 0x71, 0x09, 0x05, 0x03},
    ['8'] = {0x36, 0x49, 0x49, 0x49, 0x36},
    ['9'] = {0x06, 0x49, 0x49, 0x29, 0x1E},

    ['A'] = {0x7E, 0x11, 0x11, 0x11, 0x7E},
    ['B'] = {0x7F, 0x49, 0x49, 0x49, 0x36},
    ['C'] = {0x3E, 0x41, 0x41, 0x41, 0x22},
    ['D'] = {0x7F, 0x41, 0x41, 0x22, 0x1C},
    ['E'] = {0x7F, 0x49, 0x49, 0x49, 0x41},
    ['F'] = {0x7F, 0x09, 0x09, 0x09, 0x01},
    ['G'] = {0x3E, 0x41, 0x49, 0x49, 0x7A},
    ['T'] = {0x01, 0x01, 0x7F, 0x01, 0x01},

    ['a'] = {0x20, 0x54, 0x54, 0x54, 0x78},
    ['c'] = {0x38, 0x44, 0x44, 0x44, 0x20},
    ['d'] = {0x38, 0x44, 0x44, 0x48, 0x7F},
    ['g'] = {0x18, 0xA4, 0xA4, 0xA4, 0x7C},
    ['m'] = {0x7C, 0x04, 0x18, 0x04, 0x78},
    ['p'] = {0xF8, 0x24, 0x24, 0x24, 0x18},
    ['x'] = {0x44, 0x28, 0x10, 0x28, 0x44},
    ['y'] = {0x0C, 0x50, 0x50, 0x50, 0x3C},
    ['z'] = {0x44, 0x64, 0x54, 0x4C, 0x44},
    ['e'] = {0x38, 0x54, 0x54, 0x54, 0x18},
    ['s'] = {0x48, 0x54, 0x54, 0x54, 0x20},

    [':'] = {0x00, 0x36, 0x36, 0x00, 0x00},
    ['.'] = {0x00, 0x40, 0x60, 0x00, 0x00},
    ['-'] = {0x08, 0x08, 0x08, 0x08, 0x08},
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00}
};

void TFT_DC(uint8_t state) {
    if (state) {
        GPIOB->BSRR = (1U << 8);         // DC HIGH -> data
    } else {
        GPIOB->BSRR = (1U << (8 + 16));  // DC LOW -> command
    }
}

void TFT_Write_Command(uint8_t cmd) {
    SPI2_CS(LOW);
    TFT_DC(LOW);
    SPI2_Transmit(&cmd, 1);
    SPI2_CS(HIGH);
}

void TFT_Write_Data(uint8_t *data, int size) {
    SPI2_CS(LOW);
    TFT_DC(HIGH);
    SPI2_Transmit(data, size);
    SPI2_CS(HIGH);
}

void TFT_Reset(void) {
    GPIOB->BSRR = (1U << (9 + 16));   // RESET LOW
    HAL_Delay(10);
    GPIOB->BSRR = (1U << 9);          // RESET HIGH
    HAL_Delay(5);
}

void TFT_SetAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    uint8_t data[4];

    TFT_Write_Command(Column_Address_Set);
    data[0] = x1 >> 8;
    data[1] = x1 & 0xFF;
    data[2] = x2 >> 8;
    data[3] = x2 & 0xFF;
    TFT_Write_Data(data, 4);

    TFT_Write_Command(Page_Address_Set);
    data[0] = y1 >> 8;
    data[1] = y1 & 0xFF;
    data[2] = y2 >> 8;
    data[3] = y2 & 0xFF;
    TFT_Write_Data(data, 4);
}

void TFT_FillScreen(uint16_t color) {
    uint8_t data[2];
    data[0] = color >> 8;
    data[1] = color & 0xFF;

    TFT_SetAddress(0, 0, 239, 319);
    TFT_Write_Command(Memory_Write);

    for (uint32_t i = 0; i < 240UL * 320UL; i++) {
        TFT_Write_Data(data, 2);
    }
}

void TFT_Init(void) {
    uint8_t data;

    TFT_Reset();

    TFT_Write_Command(Software_Reset);
    HAL_Delay(5);

    TFT_Write_Command(Sleep_Out);
    HAL_Delay(120);

    data = Pixel_Format;
    TFT_Write_Command(Pixel_Format_Set);
    TFT_Write_Data(&data, 1);

    data = MADCTL_VALUE;
    TFT_Write_Command(Memory_Access_Control);
    TFT_Write_Data(&data, 1);

    TFT_Write_Command(Display_ON);
    HAL_Delay(5);
    TFT_FillScreen(0x0000);   // Black background

    // Draw static text labels ONCE during startup to save CPU in loop
    TFT_PrintString(10, 20,  "Ax:");
    TFT_PrintString(10, 40,  "Ay:");
    TFT_PrintString(10, 60,  "Az:");

    TFT_PrintString(10, 90,  "Gx:");
    TFT_PrintString(10, 110, "Gy:");
    TFT_PrintString(10, 130, "Gz:");

    TFT_PrintString(10, 160, "Temp:");
}

void TFT_DrawChar(uint16_t x, uint16_t y, char c) {
    const uint8_t *bitmap = font5x7[(uint8_t)c];
    uint8_t data[70];
    uint16_t index = 0;

    for (uint8_t row = 0; row < 7; row++) {
        for (uint8_t col = 0; col < 5; col++) {
            if (bitmap[col] & (1U << row)) {
                data[index++] = 0x07; // GREEN High byte
                data[index++] = 0xE0; // GREEN Low byte
            } else {
                data[index++] = 0x00; // BLACK Background
                data[index++] = 0x00;
            }
        }
    }

    TFT_SetAddress(x, y, x + 4, y + 6);
    TFT_Write_Command(Memory_Write);
    TFT_Write_Data(data, 70);
}

void TFT_PrintString(uint16_t x, uint16_t y, char *str) {
    while (*str) {
        TFT_DrawChar(x, y, *str);
        x += 6;
        str++;
    }
}

void TFT_PrintFloat(uint16_t x, uint16_t y, const char *name, float value) {
    char val_str[16];
    char display_buffer[32];

    // Convert float value using the shared converter
    float_to_str(value, val_str, sizeof(val_str));

    // Format with name prefix and extra space padding to clear old numbers
    snprintf(display_buffer, sizeof(display_buffer), "%s%s   ", name, val_str);

    // Render string to display
    TFT_PrintString(x, y, display_buffer);
}

void TFT_DisplayData(void) {
    // Only update numerical region (x = 55) at 10 Hz
    TFT_PrintFloat(55, 20,  "", Ax);
    TFT_PrintFloat(55, 40,  "", Ay);
    TFT_PrintFloat(55, 60,  "", Az);

    TFT_PrintFloat(55, 90,  "", Gx);
    TFT_PrintFloat(55, 110, "", Gy);
    TFT_PrintFloat(55, 130, "", Gz);

    TFT_PrintFloat(55, 160, "", Tempr);
}





