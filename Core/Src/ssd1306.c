#include "ssd1306.h"

// Pinovi za softverski I2C
#define I2C_SCL_PIN       GPIO_PIN_9
#define I2C_SDA_PIN       GPIO_PIN_10
#define I2C_GPIO_PORT     GPIOA

// 16x16 font za brojeve
static const uint8_t Font16[][16] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // space (0)
    {0x00,0x00,0x3C,0x66,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0x66,0x3C,0x00,0x00}, // 0 (1)
    {0x00,0x00,0x18,0x38,0x78,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x7E,0x00,0x00}, // 1 (2)
    {0x00,0x00,0x7C,0xC6,0x06,0x0C,0x18,0x30,0x60,0xC0,0xC6,0xC6,0xC6,0xFE,0x00,0x00}, // 2 (3)
    {0x00,0x00,0x7C,0xC6,0x06,0x06,0x06,0x3C,0x06,0x06,0x06,0x06,0xC6,0x7C,0x00,0x00}, // 3 (4)
    {0x00,0x00,0x0C,0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x00,0x00}, // 4 (5)
    {0x00,0x00,0xFE,0xC0,0xC0,0xC0,0xC0,0xFC,0x06,0x06,0x06,0x06,0xC6,0x7C,0x00,0x00}, // 5 (6)
    {0x00,0x00,0x38,0x60,0xC0,0xC0,0xC0,0xFC,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00}, // 6 (7)
    {0x00,0x00,0xFE,0xC6,0x06,0x06,0x0C,0x18,0x30,0x30,0x30,0x30,0x30,0x30,0x00,0x00}, // 7 (8)
    {0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00}, // 8 (9)
    {0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0x7E,0x06,0x06,0x06,0x0C,0x78,0x70,0x00,0x00}, // 9 (10)
};

// 8x8 font za slovo 'g'
static const uint8_t Font8_g[8] = {
    0x00, 0x00, 0x3E, 0x41, 0x41, 0x3E, 0x02, 0x3C
};

// OLED framebuffer (1024 bajta)
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// Kratka pauza za softverski I2C
static void I2C_Soft_Delay(void)
{
    for (volatile int i = 0; i < 10; i++);
}

// Inicijalizacija I2C pinova
static void I2C_Soft_Init(void)
{
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
    I2C_Soft_Delay();
}

// Start uvjet na I2C sabirnici
static void I2C_Soft_Start(void)
{
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
    I2C_Soft_Delay();
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SDA_PIN, GPIO_PIN_RESET);
    I2C_Soft_Delay();
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_RESET);
    I2C_Soft_Delay();
}

// Stop uvjet na I2C sabirnici
static void I2C_Soft_Stop(void)
{
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SDA_PIN, GPIO_PIN_RESET);
    I2C_Soft_Delay();
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
    I2C_Soft_Delay();
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
    I2C_Soft_Delay();
}

// Slanje jednog bajta preko softverskog I2C
static uint8_t I2C_Soft_Write(uint8_t data)
{
    for (int i = 7; i >= 0; i--)
    {
        if (data & (1 << i))
            HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SDA_PIN, GPIO_PIN_RESET);
        I2C_Soft_Delay();
        HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
        I2C_Soft_Delay();
        HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_RESET);
        I2C_Soft_Delay();
    }

    // ACK bit
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
    I2C_Soft_Delay();
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
    I2C_Soft_Delay();
    HAL_GPIO_ReadPin(I2C_GPIO_PORT, I2C_SDA_PIN);
    HAL_GPIO_WritePin(I2C_GPIO_PORT, I2C_SCL_PIN, GPIO_PIN_RESET);
    I2C_Soft_Delay();

    return 0;
}

// Slanje komande SSD1306 kontroleru
static void SSD1306_WriteCommand(uint8_t cmd)
{
    I2C_Soft_Start();
    I2C_Soft_Write(SSD1306_I2C_ADDR);
    I2C_Soft_Write(0x00); // Command mode
    I2C_Soft_Write(cmd);
    I2C_Soft_Stop();
}

// Inicijalizacija OLED displaya
void SSD1306_Init(void)
{
    I2C_Soft_Init();
    HAL_Delay(100);

    // Sekvenca inicijalizacije SSD1306
    SSD1306_WriteCommand(SSD1306_CMD_DISPLAY_OFF);
    SSD1306_WriteCommand(0xD5);
    SSD1306_WriteCommand(0x80);
    SSD1306_WriteCommand(SSD1306_CMD_MULTIPLEX_RATIO);
    SSD1306_WriteCommand(0x3F);
    SSD1306_WriteCommand(SSD1306_CMD_DISPLAY_OFFSET);
    SSD1306_WriteCommand(0x00);
    SSD1306_WriteCommand(SSD1306_CMD_SET_START_LINE | 0x00);
    SSD1306_WriteCommand(SSD1306_CMD_CHARGE_PUMP);
    SSD1306_WriteCommand(0x14);
    SSD1306_WriteCommand(SSD1306_CMD_MEMORY_MODE);
    SSD1306_WriteCommand(0x00);
    SSD1306_WriteCommand(SSD1306_CMD_SEG_REMAP | 0x00);
    SSD1306_WriteCommand(SSD1306_CMD_COM_SCAN_DIR | 0x08);
    SSD1306_WriteCommand(SSD1306_CMD_SET_COM_PINS);
    SSD1306_WriteCommand(0x12);
    SSD1306_WriteCommand(SSD1306_CMD_SET_CONTRAST);
    SSD1306_WriteCommand(0x7F);
    SSD1306_WriteCommand(0xD9);
    SSD1306_WriteCommand(0x22);
    SSD1306_WriteCommand(0xDB);
    SSD1306_WriteCommand(0x20);
    SSD1306_WriteCommand(0xA4);
    SSD1306_WriteCommand(0xA6);

    SSD1306_Fill(0);
    SSD1306_UpdateScreen();

    SSD1306_WriteCommand(SSD1306_CMD_DISPLAY_ON);
}

// Ispuna cijelog buffera
void SSD1306_Fill(uint8_t color)
{
    memset(SSD1306_Buffer, color ? 0xFF : 0x00, sizeof(SSD1306_Buffer));
}

// Brisanje ekrana
void SSD1306_Clear(void)
{
    SSD1306_Fill(0);
}

// Crtanje velikog broja (16x16)
void SSD1306_DrawBigDigit(uint8_t x, uint8_t y, uint8_t digit)
{
    if (digit > 9) return;

    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (Font16[digit + 1][i] & (1 << j))
            {
                uint16_t index = (x + j) + ((y + i) / 8) * SSD1306_WIDTH;
                if ((x + j) < SSD1306_WIDTH && (y + i) < SSD1306_HEIGHT)
                {
                    SSD1306_Buffer[index] |= (1 << ((y + i) % 8));
                }
            }
        }
    }
}

// Crtanje malog slova 'g' (8x8)
void SSD1306_DrawSmallG(uint8_t x, uint8_t y)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (Font8_g[i] & (1 << j))
            {
                uint16_t index = (x + j) + ((y + i) / 8) * SSD1306_WIDTH;
                if ((x + j) < SSD1306_WIDTH && (y + i) < SSD1306_HEIGHT)
                {
                    SSD1306_Buffer[index] |= (1 << ((y + i) % 8));
                }
            }
        }
    }
}

// Slanje buffera na display
void SSD1306_UpdateScreen(void)
{
    SSD1306_WriteCommand(SSD1306_CMD_COLUMN_ADDR);
    SSD1306_WriteCommand(0);
    SSD1306_WriteCommand(127);

    SSD1306_WriteCommand(SSD1306_CMD_PAGE_ADDR);
    SSD1306_WriteCommand(0);
    SSD1306_WriteCommand(7);

    I2C_Soft_Start();
    I2C_Soft_Write(SSD1306_I2C_ADDR);
    I2C_Soft_Write(0x40); // Data mode

    for (uint16_t i = 0; i < sizeof(SSD1306_Buffer); i++)
    {
        I2C_Soft_Write(SSD1306_Buffer[i]);
    }
    I2C_Soft_Stop();
}
