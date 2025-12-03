/**
 * @file    Interrupt_Handler.h
 * @brief   Centralized interrupt flag definitions and external handles.
 *
 * This header exposes the global flags, encoder instances, and timer handles
 * used by the interrupt service routines implemented in Interrupt_Handler.c.
 * These flags are set by lightweight ISRs and consumed by higher-level
 * application logic in the main execution loop.
 *
 * The module provides:
 *  - Global volatile flags for sensor sampling, encoder button presses, and SPI events.
 *  - External encoder structures updated by timer-based quadrature decoding.
 *  - External timer handles used for debounce timing (TIM9).
 *
 * @date    03.12.2025
 * @author  Eren Egdemir
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
