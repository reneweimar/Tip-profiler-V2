/**
 * This Library is written and optimized by Olivier Van den Eede(4ilo) in 2016
 * for Stm32 Uc and HAL-i2c lib's.
 *
 * To use this library with ssd1306 oled display you will need to customize the defines below.
 *
 * This library uses 2 extra files (fonts.c/h).
 * In this files are 3 different fonts you can use:
 *     - Font_7x10
 *     - Font_11x18
 *     - Font_16x26
 *
 */

#ifndef _SSD1306_H
#define _SSD1306_H

#include "stm32f1xx_hal.h"
#include "fonts.h"

// I2c address
#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR        0x78
#endif // SSD1306_I2C_ADDR

// SSD1306 width in pixels
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH 128 //128 for SSD1306,SSD3315, 130 for SH1106 (1.3" display)
#endif // SSD1306_WIDTH

// SSD1306 LCD height in pixels
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT          64
#endif // SSD1306_HEIGHT


//
//  Enumeration for screen colors
//
typedef enum {
    Black = 0x00,   // Black color, no pixel
    White = 0x01,   // Pixel is set. Color depends on LCD
} SSD1306_COLOR;

//
//  Struct to store transformations
//
typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Inverted;
    uint8_t Initialized;
} SSD1306_t;

//
//  Function definitions
//
uint8_t ssd1306_Init(I2C_HandleTypeDef *hi2c);
extern void ssd1306_UpdateScreen(I2C_HandleTypeDef *hi2c);
void ssd1306_UpdateScreenPage(I2C_HandleTypeDef *hi2c, uint8_t newPage);
extern void ssd1306_Fill(SSD1306_COLOR color);
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color);
extern char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color);
extern void ssd1306_SetCursor(uint8_t newX, uint8_t newY);
void ssd1306_InvertColors(void);
uint8_t ssd1306_GetInvertColors(void);
extern void ssd1306_DrawLogo (SSD1306_COLOR color);
extern void ssd1306_DrawBattery (SSD1306_COLOR color, uint8_t newPercentage, uint8_t newX, uint8_t newY);
extern void ssd1306_DrawRectangle(SSD1306_COLOR color, uint8_t newX, uint8_t newY, uint8_t width, uint8_t height, uint8_t Chamfer);
extern char ssd1306_WriteStringEightBitFont(char* str, FontDefEightBit Fontt, SSD1306_COLOR color);
extern uint8_t ssd1306_SetContrast(I2C_HandleTypeDef *hi2c,uint8_t NewContrast);


#endif  // _SSD1306_H
