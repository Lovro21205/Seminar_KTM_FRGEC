#ifndef __HX711_H
#define __HX711_H

#include "stm32f0xx_hal.h"

// HX711 pinovi
#define HX711_DATA_PIN    GPIO_PIN_0
#define HX711_SCK_PIN     GPIO_PIN_1
#define HX711_GPIO_PORT   GPIOA

// Kalibracijski faktor (prilagodite prema vašem load cellu)
#define CALIBRATION_FACTOR  400.0f

// Funkcije za HX711
void HX711_Init(void);
int32_t HX711_Read(void);
int32_t HX711_ReadAverage(uint8_t times);
float HX711_GetWeight(void);
void HX711_Tare(void);

#endif /* __HX711_H */
