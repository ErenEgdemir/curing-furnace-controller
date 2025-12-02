/*
 * Interrupt_Handler.c
 *
 *  Created on: Dec 2, 2025
 *      Author: erenegdemir
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

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {

	if(hspi->Instance == SPI1)
	{
		SPI_flag = TRUE;
	}



}

