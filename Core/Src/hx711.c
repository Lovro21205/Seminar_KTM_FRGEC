#include "hx711.h"

static int32_t zero_offset = 0; // Offset za taru

// Inicijalizacija HX711
void HX711_Init(void)
{
    HAL_GPIO_WritePin(HX711_GPIO_PORT, HX711_SCK_PIN, GPIO_PIN_RESET);
    HAL_Delay(100);
    HX711_Tare(); // Postavi nulu na početku
}

// Čitanje jednog mjerenja (24 bita)
int32_t HX711_Read(void)
{
    int32_t data = 0;

    // Čekaj da podaci budu spremni (DOUT = 0)
    uint32_t timeout = 0;
    while (HAL_GPIO_ReadPin(HX711_GPIO_PORT, HX711_DATA_PIN) == GPIO_PIN_SET)
    {
        timeout++;
        if (timeout > 1000000) return 0; // Timeout
    }

    // Čitaj 24 bita
    for (int i = 0; i < 24; i++)
    {
        HAL_GPIO_WritePin(HX711_GPIO_PORT, HX711_SCK_PIN, GPIO_PIN_SET);
        for (volatile int j = 0; j < 5; j++);
        data = data << 1;
        if (HAL_GPIO_ReadPin(HX711_GPIO_PORT, HX711_DATA_PIN) == GPIO_PIN_SET)
            data |= 1;
        HAL_GPIO_WritePin(HX711_GPIO_PORT, HX711_SCK_PIN, GPIO_PIN_RESET);
        for (volatile int j = 0; j < 5; j++);
    }

    // Dodatni puls za gain 128
    HAL_GPIO_WritePin(HX711_GPIO_PORT, HX711_SCK_PIN, GPIO_PIN_SET);
    for (volatile int j = 0; j < 5; j++);
    HAL_GPIO_WritePin(HX711_GPIO_PORT, HX711_SCK_PIN, GPIO_PIN_RESET);

    // Pretvori u signed 32-bit
    if (data & 0x800000) data |= 0xFF000000;
    return data;
}

// Čitanje s usrednjavanjem (stabilnije)
int32_t HX711_ReadAverage(uint8_t times)
{
    int32_t sum = 0;
    for (int i = 0; i < times; i++)
    {
        sum += HX711_Read();
        HAL_Delay(5);
    }
    return sum / times;
}

// Dobivanje težine u gramima
float HX711_GetWeight(void)
{
    int32_t raw = HX711_ReadAverage(10); // 10 uzoraka
    int32_t net_value = raw - zero_offset;

    float weight = (float)net_value / CALIBRATION_FACTOR;

    // Filtriraj male negativne vrijednosti
    if (weight < 0 && weight > -2.0f) weight = 0.0f;

    // Zaokruži na cijele grame
    weight = (float)((int)(weight + 0.5f));

    // Ograniči na maksimum 1000g
    if (weight > 1000.0f) weight = 1000.0f;

    return weight;
}

// Tara (postavi nulu)
void HX711_Tare(void)
{
    zero_offset = HX711_ReadAverage(20); // 20 uzoraka za točnu nulu
}
