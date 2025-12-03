#ifndef FAKE_HAL_H
#define FAKE_HAL_H

#include <stdint.h>

typedef enum {
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET
} GPIO_PinState;

// Donanım portlarını önemsemiyoruz, dummy struct yeter
typedef struct {
    int dummy;
} GPIO_TypeDef;

void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint32_t HAL_GetTick(void);

#endif