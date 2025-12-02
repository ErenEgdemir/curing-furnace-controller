/*
 * Interrupt_Handler.h
 *
 *  Created on: Dec 2, 2025
 *      Author: erenegdemir
 */

#ifndef INC_USERLIBRARIES_INTERRUPT_HANDLER_H_
#define INC_USERLIBRARIES_INTERRUPT_HANDLER_H_


#include "stm32f4xx_hal.h"
#include "types.h"
#include "common_def.h"


extern volatile uint8_t sensorRead_flag;
extern volatile uint8_t IRQ_enc1_btn_flag;
extern volatile uint8_t IRQ_enc2_btn_flag;
extern volatile uint8_t IRQ_enc3_btn_flag;
extern volatile uint8_t buttonState;
extern volatile uint8_t SPI_flag;

extern encoder encoder1;
extern encoder encoder2;
extern encoder encoder3;

extern TIM_HandleTypeDef htim9;

#endif /* INC_USERLIBRARIES_INTERRUPT_HANDLER_H_ */
