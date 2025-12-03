/**
 * @file    Interrupt_Handler.c
 * @brief   Centralized interrupt service routines for timers, GPIO events, and SPI.
 *
 * This module provides the unified interrupt callback implementations required
 * by the application, including:
 *
 *  - Timer period elapsed callbacks for:
 *        * Sensor sampling trigger (TIM1)
 *        * Encoder button debounce timing (TIM9)
 *
 *  - External interrupt callback for encoder push buttons (EXTI lines)
 *
 *  - Timer input-capture callbacks for rotary encoder quadrature decoding
 *        (TIM2, TIM3, TIM4)
 *
 *  - SPI receive-complete callback for SPI1
 *
 * The callbacks do not contain heavy processing; instead, they set lightweight
 * flags that are later handled in the main application loop. This ensures
 * deterministic ISR behavior and minimal interrupt load.
 *
 * @date    03.12.2025
 * @author  Eren Egdemir
 */
#include "Interrupt_Handler.h"
#include "main.h"
#include "stm32f4xx_hal_tim.h"

volatile uint8_t sensorRead_flag = 0;
volatile uint8_t IRQ_enc1_btn_flag = 0;
volatile uint8_t IRQ_enc2_btn_flag = 0;
volatile uint8_t IRQ_enc3_btn_flag = 0;
volatile uint8_t buttonState = 0;
volatile uint8_t SPI_flag = 0;

/**
 * @brief   Period elapsed interrupt callback for sensor acquisition and
 *          encoder button debouncing.
 *
 * This ISR is triggered on every update event of the active hardware timers.
 * It handles two independent timing domains:
 *
 *  - **TIM1**
 *      Generates the high-frequency sampling trigger for sensor acquisition.
 *      When TIM1 overflows, `sensorRead_flag` is set to TRUE, notifying the
 *      main loop that new sensor data should be read.
 *
 *  - **TIM9**
 *      Used as a debounce timer for the three encoder push buttons.
 *      After a button press starts TIM9, the ISR checks which button line is
 *      held LOW and sets the corresponding IRQ flag:
 *
 *          - IRQ_enc1_btn_flag
 *          - IRQ_enc2_btn_flag
 *          - IRQ_enc3_btn_flag
 *
 *      Once a valid button press is detected:
 *          - `buttonState` is set to TRUE
 *          - TIM9 is stopped to finalize debouncing
 *
 * This callback does not perform heavy operations; instead, it sets flags that
 * are processed later in the main application loop to ensure minimal ISR load.
 *
 * @param[in] htim   Pointer to the timer handle that triggered the interrupt.
 *
 * @date    03.12.2025
 * @author  Eren Egdemir
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	if (htim->Instance == TIM1) {

		sensorRead_flag = TRUE;

	}

	if (htim->Instance == TIM9) {

		if (HAL_GPIO_ReadPin(ENC_1_BUTTON_GPIO_Port, ENC_1_BUTTON_Pin)
				== GPIO_PIN_RESET) {

			IRQ_enc1_btn_flag = TRUE;
			buttonState = TRUE;
			HAL_TIM_Base_Stop_IT(&htim9);

		} else if (HAL_GPIO_ReadPin(ENC_2_BUTTON_GPIO_Port, ENC_2_BUTTON_Pin)
				== GPIO_PIN_RESET) {

			IRQ_enc2_btn_flag = TRUE;
			buttonState = TRUE;
			HAL_TIM_Base_Stop_IT(&htim9);

		} else if (HAL_GPIO_ReadPin(ENC_3_BUTTON_GPIO_Port, ENC_3_BUTTON_Pin)
				== GPIO_PIN_RESET) {

			IRQ_enc3_btn_flag = TRUE;
			buttonState = TRUE;
			HAL_TIM_Base_Stop_IT(&htim9);

		}
	}
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	if(GPIO_Pin == ENC_1_BUTTON_Pin && buttonState == TRUE){
		HAL_TIM_Base_Start_IT(&htim9);
		buttonState = FALSE;
	}
	else{
		__NOP();
	}

	if(GPIO_Pin == ENC_2_BUTTON_Pin && buttonState == TRUE){
		HAL_TIM_Base_Start_IT(&htim9);
		buttonState = FALSE;
	}
	else{
		__NOP();
	}

	if(GPIO_Pin == ENC_3_BUTTON_Pin && buttonState == TRUE){
		HAL_TIM_Base_Start_IT(&htim9);
		buttonState = FALSE;
	}
	else{
		__NOP();
	}

}

/**
 * @brief   Period elapsed interrupt callback for sensor acquisition and
 *          encoder button debouncing.
 *
 * This ISR is triggered on every update event of the active hardware timers.
 * It handles two independent timing domains:
 *
 *  - **TIM1**
 *      Generates the high-frequency sampling trigger for sensor acquisition.
 *      When TIM1 overflows, `sensorRead_flag` is set to TRUE, notifying the
 *      main loop that new sensor data should be read.
 *
 *  - **TIM9**
 *      Used as a debounce timer for the three encoder push buttons.
 *      After a button press starts TIM9, the ISR checks which button line is
 *      held LOW and sets the corresponding IRQ flag:
 *
 *          - IRQ_enc1_btn_flag
 *          - IRQ_enc2_btn_flag
 *          - IRQ_enc3_btn_flag
 *
 *      Once a valid button press is detected:
 *          - `buttonState` is set to TRUE
 *          - TIM9 is stopped to finalize debouncing
 *
 * This callback does not perform heavy operations; instead, it sets flags that
 * are processed later in the main application loop to ensure minimal ISR load.
 *
 * @param[in] htim   Pointer to the timer handle that triggered the interrupt.
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {

	if (htim->Instance == TIM2) {

		encoder1.counter  = __HAL_TIM_GET_COUNTER(htim);

		encoder1.position = encoder1.counter / 4;

	}else if (htim->Instance == TIM3) {

		encoder2.counter  = __HAL_TIM_GET_COUNTER(htim);

		encoder2.position = encoder2.counter / 4;

	}else if (htim->Instance == TIM4) {

		encoder3.counter  = __HAL_TIM_GET_COUNTER(htim);

		encoder3.position = encoder3.counter / 4;

	}
}

/**
 * @brief  SPI receive-complete interrupt callback.
 *
 * This ISR is triggered when SPI1 finishes an interrupt- or DMA-based
 * receive operation. Once the transfer completes, the function sets
 * `SPI_flag` to TRUE so the main application loop can process the newly
 * received data.
 *
 * @param[in] hspi   Pointer to the SPI handle that generated the callback.
 *
 * @note Only SPI1 is handled in this implementation.
 */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {

	if(hspi->Instance == SPI1)
	{
		SPI_flag = TRUE;
	}



}

