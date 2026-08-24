#ifndef __SSD1306_H
#define __SSD1306_H

#include "stm32f0xx_hal.h"
#include <stdint.h>
#include <string.h>

// SSD1306 OLED adresa i dimenzije
#define SSD1306_I2C_ADDR        0x78
#define SSD1306_WIDTH           128
#define SSD1306_HEIGHT          64

// SSD1306 komande
#define SSD1306_CMD_DISPLAY_OFF       0xAE
#define SSD1306_CMD_DISPLAY_ON        0xAF
#define SSD1306_CMD_SET_CONTRAST      0x81
#define SSD1306_CMD_MEMORY_MODE       0x20
#define SSD1306_CMD_COLUMN_ADDR       0x21
#define SSD1306_CMD_PAGE_ADDR         0x22
#define SSD1306_CMD_SET_START_LINE    0x40
#define SSD1306_CMD_SEG_REMAP         0xA0
#define SSD1306_CMD_COM_SCAN_DIR      0xC0
#define SSD1306_CMD_MULTIPLEX_RATIO   0xA8
#define SSD1306_CMD_DISPLAY_OFFSET    0xD3
#define SSD1306_CMD_CHARGE_PUMP       0x8D
#define SSD1306_CMD_SET_COM_PINS      0xDA

// Funkcije za OLED
void SSD1306_Init(void);
void SSD1306_UpdateScreen(void);
void SSD1306_Fill(uint8_t color);
void SSD1306_Clear(void);
void SSD1306_DrawBigDigit(uint8_t x, uint8_t y, uint8_t digit);
void SSD1306_DrawSmallG(uint8_t x, uint8_t y);

#endif /* __SSD1306_H */
