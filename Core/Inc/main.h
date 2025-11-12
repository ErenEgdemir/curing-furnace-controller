/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MAX_CS5_Pin GPIO_PIN_13
#define MAX_CS5_GPIO_Port GPIOC
#define MAX_CS4_Pin GPIO_PIN_14
#define MAX_CS4_GPIO_Port GPIOC
#define BUZZER_Pin GPIO_PIN_15
#define BUZZER_GPIO_Port GPIOC
#define RELAY_3_Pin GPIO_PIN_0
#define RELAY_3_GPIO_Port GPIOA
#define MAX_CS2_Pin GPIO_PIN_4
#define MAX_CS2_GPIO_Port GPIOA
#define ENC_2_1_Pin GPIO_PIN_6
#define ENC_2_1_GPIO_Port GPIOA
#define ENC_2_2_Pin GPIO_PIN_7
#define ENC_2_2_GPIO_Port GPIOA
#define LCD_CS_Pin GPIO_PIN_0
#define LCD_CS_GPIO_Port GPIOB
#define MAX_CS3_Pin GPIO_PIN_1
#define MAX_CS3_GPIO_Port GPIOB
#define RELAY_2_Pin GPIO_PIN_2
#define RELAY_2_GPIO_Port GPIOB
#define ENC_1_BUTTON_Pin GPIO_PIN_12
#define ENC_1_BUTTON_GPIO_Port GPIOB
#define ENC_1_BUTTON_EXTI_IRQn EXTI15_10_IRQn
#define ENC_2_BUTTON_Pin GPIO_PIN_13
#define ENC_2_BUTTON_GPIO_Port GPIOB
#define ENC_2_BUTTON_EXTI_IRQn EXTI15_10_IRQn
#define ENC_3_BUTTON_Pin GPIO_PIN_14
#define ENC_3_BUTTON_GPIO_Port GPIOB
#define ENC_3_BUTTON_EXTI_IRQn EXTI15_10_IRQn
#define RESET_Pin GPIO_PIN_8
#define RESET_GPIO_Port GPIOA
#define MAX_CS1_Pin GPIO_PIN_11
#define MAX_CS1_GPIO_Port GPIOA
#define SD_CS_Pin GPIO_PIN_12
#define SD_CS_GPIO_Port GPIOA
#define ENC_1_1_Pin GPIO_PIN_15
#define ENC_1_1_GPIO_Port GPIOA
#define ENC_1_2_Pin GPIO_PIN_3
#define ENC_1_2_GPIO_Port GPIOB
#define ENC_3_1_Pin GPIO_PIN_6
#define ENC_3_1_GPIO_Port GPIOB
#define ENC_3_2_Pin GPIO_PIN_7
#define ENC_3_2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
