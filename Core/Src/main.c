/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "u8g2.h"
#include "pi_control.h"
#include "ds1307_for_stm32_hal.h"
#include "menu.h"
#include <math.h>

#include "settings.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER INTERFACE STATE MACHINE DEFINES */

#define PI_DT				0.1f

#define PENDING				0
#define STAGE_1				1
//#define STAGE_1_SETTMP		10
//#define STAGE_1_SETPWR		11
#define STAGE_2				2
//#define STAGE_2_SETTMP		20
//#define STAGE_2_SETPWR		21
#define STAGE_3 			3
//#define STAGE_3_SETTMP		30
//#define STAGE_3_SETPWR		31
#define STAGE_4				4
//#define STAGE_4_SETTMP		40
//#define STAGE_4_SETPWR		51
#define STAGE_5 			5
//#define STAGE_5_SETTMP		50
//#define STAGE_6_SETPWR		51
#define STAGE_6				6
//#define STAGE_6_SETTMP		60
//#define STAGE_6_SETPWR		61
#define FASTSTART			7
#define ABORT				8

typedef enum {
	HOME = 0,
	PROGRAMS,
	SETTINGS,
	COOKING,
	COOKING_TEMPCHANGE,
	COOKING_PWRCHANGE,
	COOKING_DURATION,
	SETTINGS_PI,
	SETTINGS_PI_KP,
	SETTINGS_PI_KI,
	SETTINGS_PI_SLEW,
	SETTINGS_PI_MAXPOWER,
	SETTINGS_TEMPOFFSET,
	SETTINGS_TEMPOFFSET_S1,
	SETTINGS_TEMPOFFSET_S2,
	SETTINGS_TEMPOFFSET_S3,
	SETTINGS_TEMPOFFSET_S4,
	SETTINGS_TEMPOFFSET_S5,
	SETTINGS_TIMEDATE,
	SETTINGS_TIMEDATE_HOUR,
	SETTINGS_TIMEDATE_MINUTE,
	SETTINGS_TIMEDATE_DAY,
	SETTINGS_TIMEDATE_MONTH,
	SETTINGS_TIMEDATE_YEAR,
	PROGRAMS_FASTSTART,
	PROGRAMS_FASTSTART_TEMP,
	PROGRAMS_FASTSTART_RISETIME,
	PROGRAMS_FASTSTART_FALLTIME,
	PROGRAMS_FASTSTART_DURATION,
	PROGRAMS_ADVTEMPCTRL,
	PROGRAMS_ADVTEMPCTRL_RISINGTIME,
	PROGRAMS_ADVTEMPCTRL_RISINGTIME_S1,
	PROGRAMS_ADVTEMPCTRL_RISINGTIME_S2,
	PROGRAMS_ADVTEMPCTRL_RISINGTIME_S3,
	PROGRAMS_ADVTEMPCTRL_RISINGTIME_S4,
	PROGRAMS_ADVTEMPCTRL_RISINGTIME_S5,
	PROGRAMS_ADVTEMPCTRL_RISINGTIME_S6,
	PROGRAMS_ADVTEMPCTRL_GENERATE,
	PROGRAMS_ADVTEMPCTRL_T1,
	PROGRAMS_ADVTEMPCTRL_T1_TEMP,
	PROGRAMS_ADVTEMPCTRL_T1_DUR,
	PROGRAMS_ADVTEMPCTRL_T1_SETRESET,
	PROGRAMS_ADVTEMPCTRL_T2,
	PROGRAMS_ADVTEMPCTRL_T2_TEMP,
	PROGRAMS_ADVTEMPCTRL_T2_DUR,
	PROGRAMS_ADVTEMPCTRL_T2_SETRESET,
	PROGRAMS_ADVTEMPCTRL_T3,
	PROGRAMS_ADVTEMPCTRL_T3_TEMP,
	PROGRAMS_ADVTEMPCTRL_T3_DUR,
	PROGRAMS_ADVTEMPCTRL_T3_SETRESET,
	PROGRAMS_ADVTEMPCTRL_T4,
	PROGRAMS_ADVTEMPCTRL_T4_TEMP,
	PROGRAMS_ADVTEMPCTRL_T4_DUR,
	PROGRAMS_ADVTEMPCTRL_T4_SETRESET,
	PROGRAMS_ADVTEMPCTRL_T5,
	PROGRAMS_ADVTEMPCTRL_T5_TEMP,
	PROGRAMS_ADVTEMPCTRL_T5_DUR,
	PROGRAMS_ADVTEMPCTRL_T5_SETRESET,
	PROGRAMS_ADVTEMPCTRL_T6,
	PROGRAMS_ADVTEMPCTRL_T6_TEMP,
	PROGRAMS_ADVTEMPCTRL_T6_DUR,
	PROGRAMS_ADVTEMPCTRL_T6_SETRESET
}MenuFlag;

MenuFlag menu_flag;


static PI_Oven    pi1;
static PI_Oven    pi2;
static PI_Oven    pi3;



#define CS_EN HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define CS_DIS HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)

#define CLOCK_HIGH HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET)
#define CLOCK_LOW HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET)

#define TEMP_CAL 0.25
#define TRUE 								1
#define FALSE	 							0



/* PI CALIBRATION ALGORITHM STATE MACHINE DEFINES*/

//const float SAMPLE_PERIOD_S       = 1.0f;
//const float STEP_PERCENT          = +10.0f;    // soğutmada -10.0f
//const float STEP_TIME_S           = 10.0f;
//const float STABIL_SLOPE_C_PER_S  = 0.05f;
//const int   STABIL_HOLD_S         = 60;
//const int   TIMEOUT_S             = 1800;
//const float MIN_DELTA_TEMP_C      = 3.0f;
//const float MAX_SAFE_C            = 220.0f;




/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim9;
DMA_HandleTypeDef hdma_tim5_ch3_up;

/* USER CODE BEGIN PV */

/* PI CALIBRATION VERIABLES BEGIN*/
//enum State { TUNE_PREPARE, TUNE_STEP, TUNE_COLLECT, TUNE_EVALUATE, TUNE_DONE, TUNE_ABORT };
//State state = TUNE_PREPARE;
//
//float log_time_s[2000], log_output_percent[2000], log_temp_c[2000];
//int   log_count = 0;
//
static uint16_t buffer[1];
//
//float now_time_s = 0;
//float now_temp_c = 0, prev_temp_c = 0;
//float now_output_percent = 0;       // -100..+100
//float start_output_percent = 0;     // genelde 0
//float step_percent = STEP_PERCENT;  // ısıtma: +10, soğutma: -10
//float step_time_s = STEP_TIME_S;
//int   stable_counter_s = 0;
//
//uint32_t timer_250ms_counter;
//uint8_t timer_1sec_flag;
/* PI CALIBRATION VERIABLES END*/
float tempMean;
uint8_t cs1_temp = 0;
float cs1_temp_float;
uint8_t cs2_temp = 0;
float cs2_temp_float;
uint8_t cs3_temp = 0;
float cs3_temp_float;
uint8_t cs4_temp = 0;
float cs4_temp_float;
uint8_t cs5_temp = 0;
float cs5_temp_float;

uint8_t sensorRead_flag = FALSE;
static uint32_t i = 1;
static uint32_t k = 0;

AppSettings* cfg;


uint8_t date;
uint8_t month;
uint32_t year;
uint8_t dow;
uint8_t hour;
uint8_t minute;
uint8_t second;
uint8_t zone_hr;
uint8_t zone_min;

uint8_t buttonState = TRUE;
static volatile uint8_t buttonCounter = 0;
typedef struct
{
	volatile uint32_t counter;
	volatile uint32_t position;
	volatile uint32_t old_position;
	volatile uint32_t temp;
	volatile uint32_t duration;
	volatile uint32_t max_power;
	volatile uint32_t menu;


} encoder;



setting_t setting;

encoder encoder1;
encoder encoder2;
encoder encoder3;
encoder encoder4;



float temp_float;

stage_t stage1 = {0, 0, 0};
stage_t stage2 = {0, 0, 0};
stage_t stage3 = {0, 0, 0};
stage_t stage4 = {0, 0, 0};
stage_t stage5 = {0, 0, 0};
stage_t stage6 = {0, 0, 0};
stage_t fastStart;



uint32_t menu_counter = 0;

uint32_t menu_set_val = 0;
uint32_t menu_stage_counter = 0;
uint32_t menu_generate_flag = FALSE;
//uint32_t menu_set_switch = 0;

uint8_t button_handler_flag;

uint8_t cooking_flag = 0;
uint16_t heater_pwm = 0;
uint16_t heater_pwm_1 = 0;
uint16_t heater_pwm_2 = 0;
uint16_t heater_pwm_3 = 0;
uint8_t TIM_counter = 0;
uint8_t powerLimit = 100;
uint8_t cooling_state = 0;
uint8_t cooling_delay = 0;;
uint8_t cooling_flag = 0;





/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM9_Init(void);
/* USER CODE BEGIN PFP */
void set_value(volatile encoder *enc, uint32_t *var,
		uint32_t min_value, uint32_t max_value);

void autotune_tick_1s(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
// THIS IRQ HANDLER WILL BE SIMPLIFIED. THIS IS FOR FAST PROTOTYPING.

	if (htim->Instance == TIM1) {

		sensorRead_flag = TRUE;

		if(++cooling_delay >= 50 ){
			cooling_flag = TRUE;
		}


	}




	if (htim->Instance == TIM9) {

		if (HAL_GPIO_ReadPin(ENC_1_BUTTON_GPIO_Port, ENC_1_BUTTON_Pin)
				== GPIO_PIN_RESET) {

			switch (menu_flag) {
			case HOME:

				switch (menu_counter) {
				case 0:
					menu_flag = PROGRAMS;
					menu_counter = 0;
					break;
				case 1:
					menu_flag = SETTINGS;
					menu_counter = 0;
					break;
				}
				break;
			case PROGRAMS:
				switch (menu_counter) {
				case 0:
					menu_flag = PROGRAMS_FASTSTART;
					menu_counter = 0;
					break;
				case 1:
					menu_flag = PROGRAMS_ADVTEMPCTRL;
					menu_counter = 0;
					break;
				case 2:
					break;
				case 3:
					menu_flag = HOME;
					menu_counter = 0;
					break;
				}
				break;
			case PROGRAMS_FASTSTART:
				switch(menu_counter){
				case 0:
					menu_flag = PROGRAMS_FASTSTART_TEMP;
					menu_counter = 100;
					break;
				case 1:
					menu_flag = PROGRAMS_FASTSTART_RISETIME;
					menu_counter = 10;
					break;
				case 2:
					menu_flag = PROGRAMS_FASTSTART_FALLTIME;
					menu_counter = 10;
					break;
				case 3:
					menu_flag = PROGRAMS_FASTSTART_DURATION;
					menu_counter = 30;
					break;
				case 4:
					menu_flag = PROGRAMS_ADVTEMPCTRL_GENERATE;
					stage1.state = FALSE;
					stage2.state = FALSE;
					stage3.state = FALSE;
					stage4.state = FALSE;
					stage5.state = FALSE;
					stage6.state = FALSE;
					fastStart.state = TRUE;
					menu_counter = 0;
					break;
				case 5:
					menu_flag = PROGRAMS;
					menu_counter = 0;
					break;
				default:
					break;
				}
				break;
			case PROGRAMS_FASTSTART_TEMP:
				fastStart.temperature = menu_counter;
				menu_flag = PROGRAMS_FASTSTART;
				menu_counter = 0;
				break;
			case PROGRAMS_FASTSTART_RISETIME:
				fastStart.changePeriod = menu_counter;
				menu_flag = PROGRAMS_FASTSTART;
				menu_counter = 1;
				break;
			case PROGRAMS_FASTSTART_FALLTIME:
				fastStart.fallTime = menu_counter;
				menu_flag = PROGRAMS_FASTSTART;
				menu_counter = 2;
				break;
			case PROGRAMS_FASTSTART_DURATION:
				fastStart.duration = menu_counter;
				menu_flag = PROGRAMS_FASTSTART;
				menu_counter = 3;
				break;
			case PROGRAMS_ADVTEMPCTRL:
				fastStart.state = FALSE;
				switch (menu_counter) {
				case 0:
					menu_flag = PROGRAMS_ADVTEMPCTRL_RISINGTIME;
					menu_counter = 0;
					break;
				case 1:
					if (generate_flag == TRUE) {
						menu_flag = COOKING;
						cooking_flag = STAGE_1;
						menu_counter = 0;
					}
					break;
				case 2:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T1;
					menu_counter = 0;
					stage1.state = FALSE;
					stage1.changePeriod = 0;
					stage1.duration = 0;
					stage1.temperature = 0;
					stage1.totalDuration = 0;
					break;
				case 3:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T2;
					menu_counter = 0;
					stage2.state = FALSE;
					stage2.changePeriod = 0;
					stage2.duration = 0;
					stage2.temperature = 0;
					stage2.totalDuration = 0;
					break;
				case 4:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T3;
					menu_counter = 0;
					stage3.state = FALSE;
					stage3.changePeriod = 0;
					stage3.duration = 0;
					stage3.temperature = 0;
					stage3.totalDuration = 0;
					break;
				case 5:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T4;
					menu_counter = 0;
					stage4.state = FALSE;
					stage4.changePeriod = 0;
					stage4.duration = 0;
					stage4.temperature = 0;
					stage4.totalDuration = 0;
					break;
				case 6:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T5;
					menu_counter = 0;
					stage5.state = FALSE;
					stage5.changePeriod = 0;
					stage5.duration = 0;
					stage5.temperature = 0;
					stage5.totalDuration = 0;
					break;
				case 7:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T6;
					menu_counter = 0;
					stage6.state = FALSE;
					stage6.changePeriod = 0;
					stage6.duration = 0;
					stage6.temperature = 0;
					stage6.totalDuration = 0;
					break;
				case 8:
					menu_flag = HOME;
					menu_counter = 0;
					break;
				default:
					break;
				}
				break;
			case PROGRAMS_ADVTEMPCTRL_RISINGTIME:
				switch (menu_counter) {
				case 0:
					menu_flag = PROGRAMS_ADVTEMPCTRL_RISINGTIME_S1;
					menu_counter = 0;
					break;
				case 1:
					menu_flag = PROGRAMS_ADVTEMPCTRL_RISINGTIME_S2;
					menu_counter = 0;
					break;
				case 2:
					menu_flag = PROGRAMS_ADVTEMPCTRL_RISINGTIME_S3;
					menu_counter = 0;
					break;
				case 3:
					menu_flag = PROGRAMS_ADVTEMPCTRL_RISINGTIME_S4;
					menu_counter = 0;
					break;
				case 4:
					menu_flag = PROGRAMS_ADVTEMPCTRL_RISINGTIME_S5;
					menu_counter = 0;
					break;
				case 5:
					menu_flag = PROGRAMS_ADVTEMPCTRL_RISINGTIME_S6;
					menu_counter = 0;
					break;
				case 7:
					menu_flag = PROGRAMS_ADVTEMPCTRL_GENERATE;
					menu_counter = 0;
					break;
				case 8:
					menu_flag = PROGRAMS_ADVTEMPCTRL;
					menu_counter = 0;
					break;
				}
				break;
			case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S1:
				menu_flag = PROGRAMS_ADVTEMPCTRL_RISINGTIME;
				stage1.changePeriod = menu_counter;
				stage1.totalDuration = stage1.changePeriod + stage1.duration;
				menu_counter = 0;
				break;
			case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S2:
				menu_flag = PROGRAMS_ADVTEMPCTRL_RISINGTIME;
				stage2.changePeriod = menu_counter;
				stage2.totalDuration = stage2.changePeriod + stage2.duration;
				menu_counter = 1;
				break;
			case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S3:
				menu_flag = PROGRAMS_ADVTEMPCTRL_RISINGTIME;
				stage3.changePeriod = menu_counter;
				stage3.totalDuration = stage3.changePeriod + stage3.duration;
				menu_counter = 2;
				break;
			case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S4:
				menu_flag = PROGRAMS_ADVTEMPCTRL_RISINGTIME;
				stage4.changePeriod = menu_counter;
				stage4.totalDuration = stage4.changePeriod + stage4.duration;
				menu_counter = 3;
				break;
			case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S5:
				menu_flag = PROGRAMS_ADVTEMPCTRL_RISINGTIME;
				stage5.changePeriod = menu_counter;
				stage5.totalDuration = stage5.changePeriod + stage5.duration;
				menu_counter = 4;
				break;
			case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S6:
				menu_flag = PROGRAMS_ADVTEMPCTRL_RISINGTIME;
				stage6.changePeriod = menu_counter;
				stage6.totalDuration = stage6.changePeriod + stage6.duration;
				menu_counter = 5;
				break;
			case PROGRAMS_ADVTEMPCTRL_T1:
				switch (menu_counter) {
				case 0:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T1_TEMP;
					menu_counter = cs1_temp;
					break;
				case 1:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T1_DUR;
					menu_counter = 0;
					break;
				case 2:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T1_SETRESET;
					stage1.state = TRUE;
					menu_counter = 0;
					break;
				case 3:
					menu_flag = PROGRAMS_ADVTEMPCTRL;
					menu_counter = 2;
					break;
				default:
					break;
				}
				break;
			case PROGRAMS_ADVTEMPCTRL_T1_TEMP:
				menu_flag = PROGRAMS_ADVTEMPCTRL_T1;
				stage1.temperature = menu_counter;
				menu_counter = 0;
				break;
			case PROGRAMS_ADVTEMPCTRL_T1_DUR:
				menu_flag = PROGRAMS_ADVTEMPCTRL_T1;
				stage1.duration = menu_counter;
				menu_counter = 1;
				break;
			case PROGRAMS_ADVTEMPCTRL_T1_SETRESET:
				switch (menu_counter) {
				case 0:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T1;
					stage1.changePeriod = 0;
					stage1.duration = 0;
					stage1.state = 0;
					stage1.temperature = 0;
					stage1.totalDuration = 0;
					menu_counter = 0;
					break;
				case 1:
					menu_flag = PROGRAMS_ADVTEMPCTRL;
					menu_counter = 2;
					break;
				}
				break;
			case PROGRAMS_ADVTEMPCTRL_T2:
				switch (menu_counter) {
				case 0:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T2_TEMP;
					menu_counter = cs1_temp;
					break;
				case 1:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T2_DUR;
					menu_counter = 0;
					break;
				case 2:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T2_SETRESET;
					stage2.state = TRUE;
					menu_counter = 0;
					break;
				case 3:
					menu_flag = PROGRAMS_ADVTEMPCTRL;
					menu_counter = 3;
					break;
				default:
					break;
				}
				break;
			case PROGRAMS_ADVTEMPCTRL_T2_TEMP:
				menu_flag = PROGRAMS_ADVTEMPCTRL_T2;
				stage2.temperature = menu_counter;
				menu_counter = 0;
				break;
			case PROGRAMS_ADVTEMPCTRL_T2_DUR:
				menu_flag = PROGRAMS_ADVTEMPCTRL_T2;
				stage2.duration = menu_counter;
				menu_counter = 1;
				break;
			case PROGRAMS_ADVTEMPCTRL_T2_SETRESET:
				switch (menu_counter) {
				case 0:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T2;
					stage2.changePeriod = 0;
					stage2.duration = 0;
					stage2.state = 0;
					stage2.temperature = 0;
					stage2.totalDuration = 0;
					menu_counter = 0;
					break;
				case 1:
					menu_flag = PROGRAMS_ADVTEMPCTRL;
					menu_counter = 2;
					break;
				}
				break;
			case PROGRAMS_ADVTEMPCTRL_T3:
				switch (menu_counter) {
				case 0:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T3_TEMP;
					menu_counter = cs1_temp;
					break;
				case 1:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T3_DUR;
					menu_counter = 0;
					break;
				case 2:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T3_SETRESET;
					stage3.state = TRUE;
					menu_counter = 0;
					break;
				case 3:
					menu_flag = PROGRAMS_ADVTEMPCTRL;
					menu_counter = 2;
					break;
				default:
					break;
				}
				break;
			case PROGRAMS_ADVTEMPCTRL_T3_TEMP:
				menu_flag = PROGRAMS_ADVTEMPCTRL_T3;
				stage3.temperature = menu_counter;
				menu_counter = 0;
				break;
			case PROGRAMS_ADVTEMPCTRL_T3_DUR:
				menu_flag = PROGRAMS_ADVTEMPCTRL_T3;
				stage3.duration = menu_counter;
				menu_counter = 1;
				break;
			case PROGRAMS_ADVTEMPCTRL_T3_SETRESET:
				switch (menu_counter) {
				case 0:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T3;
					stage3.changePeriod = 0;
					stage3.duration = 0;
					stage3.state = 0;
					stage3.temperature = 0;
					stage3.totalDuration = 0;
					menu_counter = 0;
					break;
				case 1:
					menu_flag = PROGRAMS_ADVTEMPCTRL;
					menu_counter = 2;
					break;
				}
				break;
			case PROGRAMS_ADVTEMPCTRL_T4:
				switch (menu_counter) {
				case 0:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T4_TEMP;
					menu_counter = cs1_temp;
					break;
				case 1:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T4_DUR;
					menu_counter = 0;
					break;
				case 2:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T4_SETRESET;
					stage4.state = TRUE;
					menu_counter = 0;
					break;
				case 3:
					menu_flag = PROGRAMS_ADVTEMPCTRL;
					menu_counter = 2;
					break;
				default:
					break;
				}
				break;
			case PROGRAMS_ADVTEMPCTRL_T4_TEMP:
				menu_flag = PROGRAMS_ADVTEMPCTRL_T4;
				stage4.temperature = menu_counter;
				menu_counter = 0;
				break;
			case PROGRAMS_ADVTEMPCTRL_T4_DUR:
				menu_flag = PROGRAMS_ADVTEMPCTRL_T4;
				stage4.duration = menu_counter;
				menu_counter = 1;
				break;
			case PROGRAMS_ADVTEMPCTRL_T4_SETRESET:
				switch (menu_counter) {
				case 0:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T4;
					stage4.changePeriod = 0;
					stage4.duration = 0;
					stage4.state = 0;
					stage4.temperature = 0;
					stage4.totalDuration = 0;
					menu_counter = 0;
					break;
				case 1:
					menu_flag = PROGRAMS_ADVTEMPCTRL;
					menu_counter = 2;
					break;
				}
				break;
			case PROGRAMS_ADVTEMPCTRL_T5:
				switch (menu_counter) {
				case 0:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T5_TEMP;
					menu_counter = cs1_temp;
					break;
				case 1:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T5_DUR;
					menu_counter = 0;
					break;
				case 2:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T5_SETRESET;
					stage5.state = TRUE;
					menu_counter = 0;
					break;
				case 3:
					menu_flag = PROGRAMS_ADVTEMPCTRL;
					menu_counter = 2;
					break;
				default:
					break;
				}
				break;
			case PROGRAMS_ADVTEMPCTRL_T5_TEMP:
				menu_flag = PROGRAMS_ADVTEMPCTRL_T5;
				stage5.temperature = menu_counter;
				menu_counter = 0;
				break;
			case PROGRAMS_ADVTEMPCTRL_T5_DUR:
				menu_flag = PROGRAMS_ADVTEMPCTRL_T5;
				stage5.duration = menu_counter;
				menu_counter = 1;
				break;
			case PROGRAMS_ADVTEMPCTRL_T5_SETRESET:
				switch (menu_counter) {
				case 0:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T5;
					stage5.changePeriod = 0;
					stage5.duration = 0;
					stage5.state = 0;
					stage5.temperature = 0;
					stage5.totalDuration = 0;
					menu_counter = 0;
					break;
				case 1:
					menu_flag = PROGRAMS_ADVTEMPCTRL;
					menu_counter = 2;
					break;
				}
				break;
			case PROGRAMS_ADVTEMPCTRL_T6:
				switch (menu_counter) {
				case 0:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T6_TEMP;
					menu_counter = cs1_temp;
					break;
				case 1:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T6_DUR;
					menu_counter = 0;
					break;
				case 2:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T6_SETRESET;
					stage6.state = TRUE;
					menu_counter = 0;
					break;
				case 3:
					menu_flag = PROGRAMS_ADVTEMPCTRL;
					menu_counter = 2;
					break;
				default:
					break;
				}
				break;
			case PROGRAMS_ADVTEMPCTRL_T6_TEMP:
				menu_flag = PROGRAMS_ADVTEMPCTRL_T6;
				stage6.temperature = menu_counter;
				menu_counter = 0;
				break;
			case PROGRAMS_ADVTEMPCTRL_T6_DUR:
				menu_flag = PROGRAMS_ADVTEMPCTRL_T6;
				stage6.duration = menu_counter;
				menu_counter = 1;
				break;
			case PROGRAMS_ADVTEMPCTRL_T6_SETRESET:
				switch (menu_counter) {
				case 0:
					menu_flag = PROGRAMS_ADVTEMPCTRL_T6;
					stage6.changePeriod = 0;
					stage6.duration = 0;
					stage6.state = 0;
					stage6.temperature = 0;
					stage6.totalDuration = 0;
					menu_counter = 0;
					break;
				case 1:
					menu_flag = PROGRAMS_ADVTEMPCTRL;
					menu_counter = 2;
					break;
				}
				break;
			case SETTINGS:
				switch (menu_counter) {
				case 0:
					menu_flag = SETTINGS_PI;
					break;
				case 1:
					break;
				case 2:
					menu_flag = SETTINGS_TIMEDATE;
					menu_counter = 0;
					break;
				case 3:

					break;
				case 4:
					menu_flag = HOME;
					cfg->kp = setting.kp;
					cfg->ki = setting.ki;
					cfg->slew = setting.slew;
					cfg->maxPwr = setting.max_pwr;

					Settings_Save();
					HAL_NVIC_SystemReset();
					break;
				case 5:
					menu_flag = HOME;
					menu_counter = 0;
					break;
				}
				break;
			case SETTINGS_PI:
				switch(menu_counter){
				case 0:
					menu_flag = SETTINGS_PI_KP;
					menu_counter = setting.kp;
					break;
				case 1:
					menu_flag = SETTINGS_PI_KI;
					menu_counter = setting.ki;
					break;
				case 2:
					menu_flag = SETTINGS_PI_SLEW;
					menu_counter = setting.slew;
					break;
				case 3:
					menu_flag = SETTINGS_PI_MAXPOWER;
					menu_counter = setting.max_pwr;
					break;
				case 4:
					menu_flag = SETTINGS;
					menu_counter = 0;
				default:
					break;
				}
				break;
			case SETTINGS_PI_KP:
				menu_flag = SETTINGS_PI;
				setting.kp = (float)menu_counter / 100;
				menu_counter = 0;
				break;
			case SETTINGS_PI_KI:
				menu_flag = SETTINGS_PI;
				setting.ki = (float)menu_counter / 100;
				menu_counter = 1;
				break;
			case SETTINGS_PI_SLEW:
				menu_flag = SETTINGS_PI;
				setting.slew = (float)menu_counter / 100;
				menu_counter = 2;
				break;
			case SETTINGS_PI_MAXPOWER:
				menu_flag = SETTINGS_PI;
				setting.max_pwr = menu_counter;
				menu_counter = 3;
				break;
			case SETTINGS_TIMEDATE:
				switch (menu_counter) {
				case 0:
					menu_flag = SETTINGS_TIMEDATE_HOUR;
					menu_counter = hour;

					break;
				case 1:
					menu_flag = SETTINGS_TIMEDATE_MINUTE;
					menu_counter = minute;
					break;
				case 2:
					menu_flag = SETTINGS_TIMEDATE_DAY;
					menu_counter = date;
					break;
				case 3:
					menu_flag = SETTINGS_TIMEDATE_MONTH;
					menu_counter = month;
					break;
				case 4:
					menu_flag = SETTINGS_TIMEDATE_YEAR;
					menu_counter = year;
					break;
				case 5:
					menu_flag = HOME;
					menu_counter = 0;
					break;
				}

				break;
			case SETTINGS_TIMEDATE_HOUR:
				menu_flag = SETTINGS_TIMEDATE;
				DS1307_SetHour(menu_counter);
				menu_counter = 0;
				break;
			case SETTINGS_TIMEDATE_MINUTE:
				menu_flag = SETTINGS_TIMEDATE;
				DS1307_SetMinute(menu_counter);
				menu_counter = 1;
				break;
			case SETTINGS_TIMEDATE_DAY:
				menu_flag = SETTINGS_TIMEDATE;
				DS1307_SetDate(menu_counter);
				menu_counter = 2;
				break;
			case SETTINGS_TIMEDATE_MONTH:
				menu_flag = SETTINGS_TIMEDATE;
				DS1307_SetMonth(menu_counter);
				menu_counter = 3;
				break;
			case SETTINGS_TIMEDATE_YEAR:
				menu_flag = SETTINGS_TIMEDATE;
				DS1307_SetYear(menu_counter);
				menu_counter = 4;
				break;
			case COOKING_DURATION:

				menu_flag = PROGRAMS_ADVTEMPCTRL_GENERATE;

				switch (cooking_flag) {
				case STAGE_1:
					stage1.duration = menu_counter;
					break;
				case STAGE_2:
					stage2.duration = menu_counter;
					break;
				case STAGE_3:
					stage3.duration = menu_counter;
					break;
				case STAGE_4:
					stage4.duration = menu_counter;
					break;
				case STAGE_5:
					stage5.duration = menu_counter;
					break;
				case STAGE_6:
					stage6.duration = menu_counter;
					break;
				case FASTSTART:
					fastStart.duration = menu_counter;
					break;
				}
				menu_counter = 0;
				break;
			case COOKING:

				switch (cooking_flag) {
				case STAGE_1:
					menu_counter = stage1.remTime;
					menu_flag = COOKING_DURATION;
					break;
				case STAGE_2:
					menu_counter = stage2.remTime;
					menu_flag = COOKING_DURATION;
					break;
				case STAGE_3:
					menu_counter = stage3.remTime;
					menu_flag = COOKING_DURATION;
					break;
				case STAGE_4:
					menu_counter = stage4.remTime;
					menu_flag = COOKING_DURATION;
					break;
				case STAGE_5:
					menu_counter = stage5.remTime;
					menu_flag = COOKING_DURATION;
					break;
				case STAGE_6:
					menu_counter = stage6.remTime;
					menu_flag = COOKING_DURATION;
					break;
				case FASTSTART:
					menu_counter = fastStart.remTime;
					menu_flag = COOKING_DURATION;
					break;
				default:
					break;
				}

				break;
			default:
				break;


			}


			buttonState = TRUE;
			HAL_TIM_Base_Stop_IT(&htim9);
		}

	}else if (HAL_GPIO_ReadPin(ENC_2_BUTTON_GPIO_Port, ENC_2_BUTTON_Pin)
			== GPIO_PIN_RESET) {

		switch (menu_flag) {

		case COOKING_TEMPCHANGE:

			menu_flag = PROGRAMS_ADVTEMPCTRL_GENERATE;

			switch (cooking_flag) {
			case STAGE_1:
				stage1.temperature = menu_counter;
				break;
			case STAGE_2:
				stage2.temperature = menu_counter;
				break;
			case STAGE_3:
				stage3.temperature = menu_counter;
				break;
			case STAGE_4:
				stage4.temperature = menu_counter;
				break;
			case STAGE_5:
				stage5.temperature = menu_counter;
				break;
			case STAGE_6:
				stage6.temperature = menu_counter;
				break;
			case FASTSTART:
				fastStart.temperature = menu_counter;
				break;
			}
			menu_counter = 0;
			break;
		case COOKING:

			switch (cooking_flag) {
			case STAGE_1:
				menu_counter = stage1.temperature;
				menu_flag = COOKING_TEMPCHANGE;
				break;
			case STAGE_2:
				menu_counter = stage2.temperature;
				menu_flag = COOKING_TEMPCHANGE;
				break;
			case STAGE_3:
				menu_counter = stage3.temperature;
				menu_flag = COOKING_TEMPCHANGE;
				break;
			case STAGE_4:
				menu_counter = stage4.temperature;
				menu_flag = COOKING_TEMPCHANGE;
				break;
			case STAGE_5:
				menu_counter = stage5.temperature;
				menu_flag = COOKING_TEMPCHANGE;
				break;
			case STAGE_6:
				menu_counter = stage6.temperature;
				menu_flag = COOKING_TEMPCHANGE;
				break;
			case FASTSTART:
				menu_counter = fastStart.temperature;
				menu_flag = COOKING_TEMPCHANGE;
				break;
			default:
				break;
			}

			break;
		}

		buttonState = TRUE;
		HAL_TIM_Base_Stop_IT(&htim9);

	}else if (HAL_GPIO_ReadPin(ENC_3_BUTTON_GPIO_Port, ENC_3_BUTTON_Pin)
			== GPIO_PIN_RESET) {

		switch (menu_flag) {

		case COOKING_PWRCHANGE:
			setting.max_pwr = menu_counter;
			menu_flag = COOKING;
			PI_Control_Init(&pi1, setting.kp, setting.ki, 0.0f, setting.max_pwr);
			PI_Control_Init(&pi2, setting.kp, setting.ki, 0.0f, setting.max_pwr);
			PI_Control_Init(&pi3, setting.kp, setting.ki, 0.0f, setting.max_pwr);
			menu_counter = 0;
			break;
		case COOKING:
			menu_counter = setting.max_pwr;
			menu_flag = COOKING_PWRCHANGE;
			break;

		}

		buttonState = TRUE;
		HAL_TIM_Base_Stop_IT(&htim9);

	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	// THIS IRQ HANDLER WILL BE SIMPLIFIED. THIS IS FOR FAST PROTOTYPING.


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
	// THIS IRQ HANDLER WILL BE SIMPLIFIED. THIS IS FOR FAST PROTOTYPING.

	if (htim->Instance == TIM2) {

		encoder1.counter  = __HAL_TIM_GET_COUNTER(htim);

		encoder1.position = encoder1.counter / 4;

		switch(menu_flag){
		case HOME:
			set_value(&encoder1, &menu_counter, 0, 1);
			break;
		case PROGRAMS:
			set_value(&encoder1, &menu_counter, 0, 3);
			break;
		case PROGRAMS_FASTSTART:
			set_value(&encoder1, &menu_counter, 0, 5);
			break;
		case PROGRAMS_FASTSTART_TEMP:
			set_value(&encoder1, &menu_counter, 0, 300);
			break;
		case PROGRAMS_FASTSTART_RISETIME:
			set_value(&encoder1, &menu_counter, 0, 512);
			break;
		case PROGRAMS_FASTSTART_FALLTIME:
			set_value(&encoder1, &menu_counter, 0, 512);
			break;
		case PROGRAMS_FASTSTART_DURATION:
			set_value(&encoder1, &menu_counter, 0, 512);
			break;
		case PROGRAMS_ADVTEMPCTRL:
			set_value(&encoder1, &menu_counter, 0, 8);
			break;
		case PROGRAMS_ADVTEMPCTRL_RISINGTIME:
			set_value(&encoder1, &menu_counter, 0, 8);
			break;
		case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S1:
			set_value(&encoder1, &menu_counter, 0, 999);
			break;
		case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S2:
			set_value(&encoder1, &menu_counter, 0, 999);
			break;
		case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S3:
			set_value(&encoder1, &menu_counter, 0, 999);
			break;
		case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S4:
			set_value(&encoder1, &menu_counter, 0, 999);
			break;
		case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S5:
			set_value(&encoder1, &menu_counter, 0, 999);
			break;
		case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S6:
			set_value(&encoder1, &menu_counter, 0, 999);
			break;
		case PROGRAMS_ADVTEMPCTRL_T1:
			set_value(&encoder1, &menu_counter, 0, 3);
			break;
		case PROGRAMS_ADVTEMPCTRL_T1_TEMP:
			set_value(&encoder1, &menu_counter, 0, 300);
			break;
		case PROGRAMS_ADVTEMPCTRL_T1_DUR:
			set_value(&encoder1, &menu_counter, 0, 999);
			break;
		case PROGRAMS_ADVTEMPCTRL_T1_SETRESET:
			set_value(&encoder1, &menu_counter, 0, 1);
			break;
		case PROGRAMS_ADVTEMPCTRL_T2:
			set_value(&encoder1, &menu_counter, 0, 3);
			break;
		case PROGRAMS_ADVTEMPCTRL_T2_TEMP:
			set_value(&encoder1, &menu_counter, 0, 300);
			break;
		case PROGRAMS_ADVTEMPCTRL_T2_DUR:
			set_value(&encoder1, &menu_counter, 0, 999);
			break;
		case PROGRAMS_ADVTEMPCTRL_T2_SETRESET:
			set_value(&encoder1, &menu_counter, 0, 1);
			break;
		case PROGRAMS_ADVTEMPCTRL_T3:
			set_value(&encoder1, &menu_counter, 0, 3);
			break;
		case PROGRAMS_ADVTEMPCTRL_T3_TEMP:
			set_value(&encoder1, &menu_counter, 0, 300);
			break;
		case PROGRAMS_ADVTEMPCTRL_T3_DUR:
			set_value(&encoder1, &menu_counter, 0, 999);
			break;
		case PROGRAMS_ADVTEMPCTRL_T3_SETRESET:
			set_value(&encoder1, &menu_counter, 0, 1);
			break;
		case PROGRAMS_ADVTEMPCTRL_T4:
			set_value(&encoder1, &menu_counter, 0, 3);
			break;
		case PROGRAMS_ADVTEMPCTRL_T4_TEMP:
			set_value(&encoder1, &menu_counter, 0, 300);
			break;
		case PROGRAMS_ADVTEMPCTRL_T4_DUR:
			set_value(&encoder1, &menu_counter, 0, 999);
			break;
		case PROGRAMS_ADVTEMPCTRL_T4_SETRESET:
			set_value(&encoder1, &menu_counter, 0, 1);
			break;
		case PROGRAMS_ADVTEMPCTRL_T5:
			set_value(&encoder1, &menu_counter, 0, 3);
			break;
		case PROGRAMS_ADVTEMPCTRL_T5_TEMP:
			set_value(&encoder1, &menu_counter, 0, 300);
			break;
		case PROGRAMS_ADVTEMPCTRL_T5_DUR:
			set_value(&encoder1, &menu_counter, 0, 999);
			break;
		case PROGRAMS_ADVTEMPCTRL_T5_SETRESET:
			set_value(&encoder1, &menu_counter, 0, 1);
			break;
		case PROGRAMS_ADVTEMPCTRL_T6:
			set_value(&encoder1, &menu_counter, 0, 3);
			break;
		case PROGRAMS_ADVTEMPCTRL_T6_TEMP:
			set_value(&encoder1, &menu_counter, 0, 300);
			break;
		case PROGRAMS_ADVTEMPCTRL_T6_DUR:
			set_value(&encoder1, &menu_counter, 0, 999);
			break;
		case PROGRAMS_ADVTEMPCTRL_T6_SETRESET:
			set_value(&encoder1, &menu_counter, 0, 1);
			break;
		case SETTINGS:
			set_value(&encoder1, &menu_counter, 0, 5);
			break;
		case SETTINGS_PI:
			set_value(&encoder1, &menu_counter, 0, 4);
			break;
		case SETTINGS_PI_KP:
			set_value(&encoder1, &menu_counter, 0, 1000);
			break;
		case SETTINGS_PI_KI:
			set_value(&encoder1, &menu_counter, 0, 1000);
			break;
		case SETTINGS_PI_SLEW:
			set_value(&encoder1, &menu_counter, 0, 100);
			break;
		case SETTINGS_PI_MAXPOWER:
			set_value(&encoder1, &menu_counter, 0, 100);
			break;
		case SETTINGS_TIMEDATE:
			set_value(&encoder1, &menu_counter, 0, 5);
			break;
		case SETTINGS_TIMEDATE_HOUR:
			set_value(&encoder1, &menu_counter, 0, 23);
			break;
		case SETTINGS_TIMEDATE_MINUTE:
			 set_value(&encoder1, &menu_counter, 0, 59);
			 break;
		case SETTINGS_TIMEDATE_DAY:
			 set_value(&encoder1, &menu_counter, 0, 31);
			 break;
		case SETTINGS_TIMEDATE_MONTH:
			 set_value(&encoder1, &menu_counter, 0, 12);
			 break;
		case SETTINGS_TIMEDATE_YEAR:
			 set_value(&encoder1, &menu_counter, 2020, 2100);
			 break;
		case COOKING_DURATION:
			set_value(&encoder1, &menu_counter, 0, 1000);
			break;




		}




	}else if (htim->Instance == TIM3) {

		encoder2.counter  = __HAL_TIM_GET_COUNTER(htim);

		encoder2.position = encoder2.counter / 4;

		switch(cooking_flag){
		case STAGE_1:
			set_value(&encoder2, &menu_counter, 0, 300);
			break;
		case STAGE_2:
			set_value(&encoder2, &menu_counter, 0, 300);
			break;
		case STAGE_3:
			set_value(&encoder2, &menu_counter, 0, 300);
			break;
		case STAGE_4:
			set_value(&encoder2, &menu_counter, 0, 300);
			break;
		case STAGE_5:
			set_value(&encoder2, &menu_counter, 0, 300);
			break;
		case STAGE_6:
			set_value(&encoder2, &menu_counter, 0, 300);
			break;
		case FASTSTART:
			set_value(&encoder2, &menu_counter, 0, 300);
			break;
		default:
			break;

		}




	}else if (htim->Instance == TIM4) {

		encoder3.counter  = __HAL_TIM_GET_COUNTER(htim);

		encoder3.position = encoder3.counter / 4;

		set_value(&encoder3, &menu_counter, 0, 100);


	}


}




void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {

	// THIS IRQ HANDLER WILL BE SIMPLIFIED. THIS IS FOR FAST PROTOTYPING.

	if (hspi->Instance == SPI1) {

		uint16_t t12;

		t12 = ((buffer[0] & 0x7FF8u) >> 3);

		if (t12 & 0x800u) {

			temp_float = (int16_t) (t12 | 0xF000u) * TEMP_CAL; // 12 bitten 16 bite işaret uzat

		} else {

			temp_float = (int16_t) t12 * TEMP_CAL;
		}

		if (HAL_GPIO_ReadPin(MAX_CS1_GPIO_Port, MAX_CS1_Pin)
				== GPIO_PIN_RESET) {

			cs1_temp = temp_float;
			cs1_temp_float = temp_float;

			temp_float = 0;

		} else if (HAL_GPIO_ReadPin(MAX_CS2_GPIO_Port, MAX_CS2_Pin)
				== GPIO_PIN_RESET) {


			cs2_temp = temp_float;
			cs2_temp_float = temp_float;


			temp_float = 0;
		}else if (HAL_GPIO_ReadPin(MAX_CS3_GPIO_Port, MAX_CS3_Pin)
				== GPIO_PIN_RESET) {


			cs3_temp = temp_float;
			cs3_temp_float = temp_float;
			temp_float = 0;

		}else if (HAL_GPIO_ReadPin(MAX_CS4_GPIO_Port, MAX_CS4_Pin)
				== GPIO_PIN_RESET) {


			cs4_temp = temp_float;
			cs4_temp_float = temp_float;
			temp_float = 0;


		}else if (HAL_GPIO_ReadPin(MAX_CS5_GPIO_Port, MAX_CS5_Pin)
				== GPIO_PIN_RESET) {


			cs5_temp = temp_float;
			cs5_temp_float = temp_float;
			temp_float = 0;

		}

		HAL_GPIO_WritePin(MAX_CS1_GPIO_Port, MAX_CS1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MAX_CS2_GPIO_Port, MAX_CS2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MAX_CS3_GPIO_Port, MAX_CS3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MAX_CS4_GPIO_Port, MAX_CS4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MAX_CS5_GPIO_Port, MAX_CS5_Pin, GPIO_PIN_SET);
	}

}

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
		void *arg_ptr)
{
	switch (msg) {
	case U8X8_MSG_DELAY_MILLI:
		HAL_Delay(arg_int);
		break;
	case U8X8_MSG_GPIO_CS:
		HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, arg_int);
		break;
	case U8X8_MSG_GPIO_DC:
//		HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, arg_int);
		break;
	case U8X8_MSG_GPIO_RESET:
		HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, arg_int);
		break;
	}
	return 1;
}

uint8_t u8x8_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
		void *arg_ptr) {

	switch (msg) {
	case U8X8_MSG_BYTE_SET_DC:
//		HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, arg_int);
		break;
	case U8X8_MSG_BYTE_SEND:
		HAL_SPI_Transmit(&hspi2, (uint8_t *)arg_ptr, arg_int, 1000);
		break;
	case U8X8_MSG_BYTE_START_TRANSFER:
		HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
		break;
	case U8X8_MSG_BYTE_END_TRANSFER:
		HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
		break;
	}
	return 1;
}

u8g2_t display;


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	HAL_GPIO_WritePin(MAX_CS1_GPIO_Port, MAX_CS1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MAX_CS2_GPIO_Port, MAX_CS2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MAX_CS3_GPIO_Port, MAX_CS3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MAX_CS4_GPIO_Port, MAX_CS4_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MAX_CS5_GPIO_Port, MAX_CS5_Pin, GPIO_PIN_SET);

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_SPI2_Init();
  MX_TIM5_Init();
  MX_TIM9_Init();
  /* USER CODE BEGIN 2 */

	DS1307_Init(&hi2c1);
//  DS1307_SetTimeZone(+3, 00);
//  DS1307_SetHour(23);
//  DS1307_SetMinute(40);
//  DS1307_SetSecond(30);
//  DS1307_SetDate(29);
//  DS1307_SetMonth(2);
//  DS1307_SetYear(2024);
//  DS1307_SetDayOfWeek(4);
	         // %/s  (0 = kapalı)

	u8g2_Setup_st7920_s_128x64_f(&display, U8G2_R2, u8x8_spi,
			u8x8_gpio_and_delay);
	u8g2_InitDisplay(&display);
	u8g2_SetPowerSave(&display, 0);

	u8g2_ClearDisplay(&display);

	u8g2_SendBuffer(&display);

	u8g2_ClearBuffer(&display);
	u8g2_SetFont(&display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(&display, 13, 5, "Composite Curing Furnace");
	menu_send_setupLogo(&display, 2, 14);
	u8g2_DrawStr(&display, 9, 62, "Developed by Eren Egdemir");
	u8g2_DrawStr(&display, 45, 12, "Controller");
	u8g2_SendBuffer(&display);

	Settings_Init();
	cfg = Settings_Get();


	setting.kp = cfg->kp;
	setting.ki = cfg->ki;
	setting.slew = cfg->slew;
	setting.max_pwr = cfg->maxPwr;

	HAL_TIM_Encoder_Start_IT(&htim2, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start_IT(&htim3, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start_IT(&htim4, TIM_CHANNEL_ALL);
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);

	PI_Control_Init(&pi1, setting.kp, setting.ki, 0.0f, setting.max_pwr);
	PI_Control_SetFilter(&pi1, 0.6f);        // 0.3–0.7 tipik
	PI_Control_SetSlew(&pi1, setting.slew);

	PI_Control_Init(&pi2, setting.kp, setting.ki, 0.0f, setting.max_pwr);
	PI_Control_SetFilter(&pi2, 0.6f);        // 0.3–0.7 tipik
	PI_Control_SetSlew(&pi2, setting.slew);

	PI_Control_Init(&pi3, setting.kp, setting.ki, 0.0f, setting.max_pwr);
	PI_Control_SetFilter(&pi3, 0.6f);        // 0.3–0.7 tipik
	PI_Control_SetSlew(&pi3, setting.slew);
	HAL_Delay(1500);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//	  u8g2_ClearDisplay(&display);
//	  menu_programs_AdvTempCtrl_run(&display, menu_counter);




		if (sensorRead_flag == TRUE) {

			sensorRead_flag = FALSE;


			switch (TIM_counter) {
			case 0:
//				date = DS1307_GetDate();
				HAL_GPIO_WritePin(MAX_CS1_GPIO_Port, MAX_CS1_Pin,
						GPIO_PIN_RESET);
				HAL_SPI_Receive_DMA(&hspi1, (uint8_t*) buffer, 1);
//				HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
				TIM_counter++;
				break;
			case 1:
//				month = DS1307_GetMonth();
				HAL_GPIO_WritePin(MAX_CS2_GPIO_Port, MAX_CS2_Pin,
						GPIO_PIN_RESET);
				HAL_SPI_Receive_DMA(&hspi1, (uint8_t*) buffer, 1);
				TIM_counter++;
				break;
			case 2:
//				year = DS1307_GetYear();
				HAL_GPIO_WritePin(MAX_CS3_GPIO_Port, MAX_CS3_Pin,
						GPIO_PIN_RESET);
				HAL_SPI_Receive_DMA(&hspi1, (uint8_t*) buffer, 1);
				TIM_counter++;
				break;
			case 3:
//				hour = DS1307_GetHour();
				HAL_GPIO_WritePin(MAX_CS4_GPIO_Port, MAX_CS4_Pin,
						GPIO_PIN_RESET);
				HAL_SPI_Receive_DMA(&hspi1, (uint8_t*) buffer, 1);
				TIM_counter++;
				break;
			case 4:
//				minute = DS1307_GetMinute();
				HAL_GPIO_WritePin(MAX_CS5_GPIO_Port, MAX_CS5_Pin,
						GPIO_PIN_RESET);
				HAL_SPI_Receive_DMA(&hspi1, (uint8_t*) buffer, 1);
				TIM_counter = 0;
				break;
			default:
				break;
			}


			switch (cooking_flag){
			case PENDING:
				break;
			case FASTSTART:

				if (fastStart.tempPerMin >= 0){
					heater_pwm_1 = PI_Control_Update(&pi1, fastStart.tempForMin[i], cs1_temp_float, PI_DT);
					heater_pwm_2 = PI_Control_Update(&pi2, fastStart.tempForMin[i], cs2_temp_float, PI_DT);
					heater_pwm_3 = PI_Control_Update(&pi3, fastStart.tempForMin[i], cs3_temp_float, PI_DT);

					heater_pwm = (heater_pwm_1 + heater_pwm_2 + heater_pwm_3) / 3;

					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, heater_pwm_1 * 10);
					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, heater_pwm_2 * 10);
					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, heater_pwm_3 * 10);

				}else {

					uint8_t coolingDiff = tempMean - fastStart.tempForMin[i];


					if(coolingDiff >= 10){
						cooling_state = 1;

					}else if(coolingDiff >=15){
						cooling_state = 2;


					}else if(coolingDiff >=30){
						cooling_state = 3;

					}

				}


				if (++k >= 600) {
					i++;
				    k = 0;
				    fastStart.remTime--;

				}
				if(i >= fastStart.totalDuration){

					cooking_flag = ABORT;

				}
				break;
			case STAGE_1:


				if (stage1.tempPerMin >= 0){
					heater_pwm_1 = PI_Control_Update(&pi1, fastStart.tempForMin[i], cs1_temp_float, PI_DT);
					heater_pwm_2 = PI_Control_Update(&pi2, fastStart.tempForMin[i], cs2_temp_float, PI_DT);
					heater_pwm_3 = PI_Control_Update(&pi3, fastStart.tempForMin[i], cs3_temp_float, PI_DT);

					heater_pwm = (heater_pwm_1 + heater_pwm_2 + heater_pwm_3) / 3;

					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, heater_pwm_1 * 10);
					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, heater_pwm_2 * 10);
					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, heater_pwm_3 * 10);
				}else {

					uint8_t coolingDiff = tempMean - stage1.tempForMin[i];


					if(coolingDiff >= 10){
						cooling_state = 1;

					}else if(coolingDiff >=15){
						cooling_state = 2;


					}else if(coolingDiff >=30){
						cooling_state = 3;

					}

				}


				if (++k >= 600) {
					i++;
				    k = 0;
				    stage1.remTime--;

				}
				if(i >= stage1.totalDuration){

					if(stage2.state == TRUE){

						cooking_flag = STAGE_2;
						i = 0;
						k = 0;
					}else cooking_flag = ABORT;

				}
				break;
			case STAGE_2:

				if (stage2.tempPerMin >= 0){
					heater_pwm_1 = PI_Control_Update(&pi1, fastStart.tempForMin[i], cs1_temp_float, PI_DT);
					heater_pwm_2 = PI_Control_Update(&pi2, fastStart.tempForMin[i], cs2_temp_float, PI_DT);
					heater_pwm_3 = PI_Control_Update(&pi3, fastStart.tempForMin[i], cs3_temp_float, PI_DT);

					heater_pwm = (heater_pwm_1 + heater_pwm_2 + heater_pwm_3) / 3;

					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, heater_pwm_1 * 10);
					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, heater_pwm_2 * 10);
					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, heater_pwm_3 * 10);
				}else {

					uint8_t coolingDiff = tempMean - stage2.tempForMin[i];


					if(coolingDiff >= 10){
						cooling_state = 1;

					}else if(coolingDiff >=15){
						cooling_state = 2;


					}else if(coolingDiff >=30){
						cooling_state = 3;

					}

				}

				if (++k >= 600) {
					i++;
				    k = 0;
				    stage2.remTime--;

				}
				if(i >= stage2.totalDuration){
					if(stage3.state == TRUE){

						cooking_flag = STAGE_2;
						i = 0;
						k = 0;
					}else cooking_flag = ABORT;
				}
				break;
			case STAGE_3:

				if (stage3.tempPerMin >= 0){
					heater_pwm_1 = PI_Control_Update(&pi1, fastStart.tempForMin[i], cs1_temp_float, PI_DT);
					heater_pwm_2 = PI_Control_Update(&pi2, fastStart.tempForMin[i], cs2_temp_float, PI_DT);
					heater_pwm_3 = PI_Control_Update(&pi3, fastStart.tempForMin[i], cs3_temp_float, PI_DT);

					heater_pwm = (heater_pwm_1 + heater_pwm_2 + heater_pwm_3) / 3;

					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, heater_pwm_1 * 10);
					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, heater_pwm_2 * 10);
					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, heater_pwm_3 * 10);
				}else {

					uint8_t coolingDiff = tempMean - stage3.tempForMin[i];


					if(coolingDiff >= 10){
						cooling_state = 1;

					}else if(coolingDiff >=15){
						cooling_state = 2;


					}else if(coolingDiff >=30){
						cooling_state = 3;

					}

				}

				if (++k >= 600) {
					i++;
				    k = 0;
				    stage3.remTime--;
				}
				if(i >= stage3.totalDuration){
					if(stage4.state == TRUE){

						cooking_flag = STAGE_4;
						i = 0;
						k = 0;
					}else cooking_flag = ABORT;
				}
				break;
			case STAGE_4:

				if (stage4.tempPerMin >= 0){
					heater_pwm_1 = PI_Control_Update(&pi1, fastStart.tempForMin[i], cs1_temp_float, PI_DT);
					heater_pwm_2 = PI_Control_Update(&pi2, fastStart.tempForMin[i], cs2_temp_float, PI_DT);
					heater_pwm_3 = PI_Control_Update(&pi3, fastStart.tempForMin[i], cs3_temp_float, PI_DT);

					heater_pwm = (heater_pwm_1 + heater_pwm_2 + heater_pwm_3) / 3;

					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, heater_pwm_1 * 10);
					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, heater_pwm_2 * 10);
					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, heater_pwm_3 * 10);
				}else {

					uint8_t coolingDiff = tempMean - stage4.tempForMin[i];


					if(coolingDiff >= 10){
						cooling_state = 1;

					}else if(coolingDiff >=15){
						cooling_state = 2;


					}else if(coolingDiff >=30){
						cooling_state = 3;

					}

				}

				if (++k >= 600) {
					i++;
				    k = 0;
				    stage4.remTime--;

				}
				if(i >= stage4.totalDuration){
					if(stage5.state == TRUE){

						cooking_flag = STAGE_5;
						i = 0;
						k = 0;
					}else cooking_flag = ABORT;
				}
				break;
			case STAGE_5:

				if (stage5.tempPerMin >= 0){
					heater_pwm_1 = PI_Control_Update(&pi1, fastStart.tempForMin[i], cs1_temp_float, PI_DT);
					heater_pwm_2 = PI_Control_Update(&pi2, fastStart.tempForMin[i], cs2_temp_float, PI_DT);
					heater_pwm_3 = PI_Control_Update(&pi3, fastStart.tempForMin[i], cs3_temp_float, PI_DT);

					heater_pwm = (heater_pwm_1 + heater_pwm_2 + heater_pwm_3) / 3;

					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, heater_pwm_1 * 10);
					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, heater_pwm_2 * 10);
					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, heater_pwm_3 * 10);
				}else {

					uint8_t coolingDiff = tempMean - stage5.tempForMin[i];


					if(coolingDiff >= 10){
						cooling_state = 1;

					}else if(coolingDiff >=15){
						cooling_state = 2;


					}else if(coolingDiff >=30){
						cooling_state = 3;

					}

				}

				if (++k >= 600) {
					i++;
				    k = 0;
				    stage5.remTime--;

				}
				if(i >= stage5.totalDuration){
					if(stage6.state == TRUE){

						cooking_flag = STAGE_6;
						i = 0;
						k = 0;
					}else cooking_flag = ABORT;
				}
				break;
			case STAGE_6:

				if (stage6.tempPerMin >= 0){
					heater_pwm_1 = PI_Control_Update(&pi1, fastStart.tempForMin[i], cs1_temp_float, PI_DT);
					heater_pwm_2 = PI_Control_Update(&pi2, fastStart.tempForMin[i], cs2_temp_float, PI_DT);
					heater_pwm_3 = PI_Control_Update(&pi3, fastStart.tempForMin[i], cs3_temp_float, PI_DT);

					heater_pwm = (heater_pwm_1 + heater_pwm_2 + heater_pwm_3) / 3;

					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, heater_pwm_1 * 10);
					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, heater_pwm_2 * 10);
					__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, heater_pwm_3 * 10);
				}else {

					uint8_t coolingDiff = tempMean - stage6.tempForMin[i];


					if(coolingDiff >= 10){
						cooling_state = 1;

					}else if(coolingDiff >=15){
						cooling_state = 2;


					}else if(coolingDiff >=30){
						cooling_state = 3;

					}

				}

				if (++k >= 600) {
					i++;
				    k = 0;
				    stage6.remTime--;

				}
				if(i >= stage6.totalDuration){
					i = 0;
					k = 0;
					cooking_flag = ABORT;
				}
				break;
			case ABORT:
				__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, 0);
				break;
			default:
				break;
			}

		}

		if(cooling_flag == TRUE){
			cooling_flag = FALSE;

			switch(cooling_state){
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			default:
				break;
			}
		}

//		menu_settings_TempOffset_run(&display, menu_counter, &setting);

//		menu_settings_Pi_run(&display, menu_counter, &setting);


//		menu_prg_FastStart_run(&display, menu_counter);

	  switch(menu_flag){
	  case HOME:
		  menu_home_run(&display, menu_counter);
		  break;
	  case PROGRAMS:
	  	  menu_programs_run(&display, menu_counter);
	  	  break;
	  case PROGRAMS_ADVTEMPCTRL:
		  menu_prg_AdvTempCtrl_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_GENERATE:
		  menu_GenerateCookingProcess(&display, &menu_flag, &menu_counter, &stage1, &stage2,
				  &stage3, &stage4, &stage5, &stage6, &fastStart);
		  break;
	  case COOKING:
		  menu_cooking_run(&display, &stage1, &stage2, &stage3, &stage4, &stage5, &stage6, &fastStart);
		  break;
	  case COOKING_TEMPCHANGE:
		  menu_cooking_run_TempChange(&display, menu_counter, &stage1, &stage2, &stage3, &stage4,
				 &stage5, &stage6, &fastStart);
		  break;
	  case COOKING_PWRCHANGE:
		  menu_cooking_PowerChange_run(&display, menu_counter, &stage1, &stage2,
		  		&stage3, &stage4, &stage5, &stage6, &fastStart);
		  break;
	  case COOKING_DURATION:
		  menu_cooking_run_Duration(&display, menu_counter, &stage1, &stage2, &stage3, &stage4, &stage5, &stage6, &fastStart);
		  break;
	  case PROGRAMS_FASTSTART:
		  menu_prg_FastStart_run(&display, menu_counter);
		  break;
	  case PROGRAMS_FASTSTART_TEMP:
		  menu_prg_FastStart_TrgtTemp_run(&display, menu_counter);
		  break;
	  case PROGRAMS_FASTSTART_RISETIME:
		  menu_prg_FastStart_RiseTime_run(&display, menu_counter);
		  break;
	  case PROGRAMS_FASTSTART_FALLTIME:
		  menu_prg_FastStart_FallTime_run(&display, menu_counter);
		  break;
	  case PROGRAMS_FASTSTART_DURATION:
		  menu_prg_FastStart_Duration_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_RISINGTIME:
		  menu_prg_AdvTempCtrl_RisingTimes_run(&display, &menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S1:
		  menu_prg_AdvTempCtrl_RisingTimes_st1_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S2:
		  menu_prg_AdvTempCtrl_RisingTimes_st2_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S3:
		  menu_prg_AdvTempCtrl_RisingTimes_st3_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S4:
		  menu_prg_AdvTempCtrl_RisingTimes_st4_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S5:
		  menu_prg_AdvTempCtrl_RisingTimes_st5_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S6:
		  menu_prg_AdvTempCtrl_RisingTimes_st6_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T1:
		  menu_prg_AdvTempCtrl_STG1_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T1_TEMP:
		  menu_prg_AdvTempCtrl_STG1_SetTemp_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T1_DUR:
		  menu_prg_AdvTempCtrl_STG1_SetDur_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T1_SETRESET:
		  menu_prg_AdvTempCtrl_STG1_SetReset_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T2:
		  menu_prg_AdvTempCtrl_STG2_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T2_TEMP:
		  menu_prg_AdvTempCtrl_STG2_SetTemp_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T2_DUR:
		  menu_prg_AdvTempCtrl_STG2_SetDur_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T2_SETRESET:
		  menu_prg_AdvTempCtrl_STG2_SetReset_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T3:
		  menu_prg_AdvTempCtrl_STG3_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T3_TEMP:
		  menu_prg_AdvTempCtrl_STG3_SetTemp_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T3_DUR:
		  menu_prg_AdvTempCtrl_STG3_SetDur_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T3_SETRESET:
		  menu_prg_AdvTempCtrl_STG3_SetReset_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T4:
		  menu_prg_AdvTempCtrl_STG4_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T4_TEMP:
		  menu_prg_AdvTempCtrl_STG4_SetTemp_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T4_DUR:
		  menu_prg_AdvTempCtrl_STG4_SetDur_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T4_SETRESET:
		  menu_prg_AdvTempCtrl_STG4_SetReset_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T5:
		  menu_prg_AdvTempCtrl_STG5_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T5_TEMP:
		  menu_prg_AdvTempCtrl_STG5_SetTemp_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T5_DUR:
		  menu_prg_AdvTempCtrl_STG5_SetDur_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T5_SETRESET:
		  menu_prg_AdvTempCtrl_STG5_SetReset_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T6:
		  menu_prg_AdvTempCtrl_STG4_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T6_TEMP:
		  menu_prg_AdvTempCtrl_STG6_SetTemp_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T6_DUR:
		  menu_prg_AdvTempCtrl_STG6_SetDur_run(&display, menu_counter);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T6_SETRESET:
		  menu_prg_AdvTempCtrl_STG6_SetReset_run(&display, menu_counter);
		  break;
	  case SETTINGS:
		  menu_settings_run(&display, menu_counter);
		  break;
	  case SETTINGS_PI:
		  menu_settings_Pi_run(&display, menu_counter, &setting);
		  break;
	  case SETTINGS_PI_KP:
		  menu_settings_PiKp_run(&display, menu_counter, &setting);
		  break;
	  case SETTINGS_PI_KI:
		  menu_settings_PiKi_run(&display, menu_counter, &setting);
		  break;
	  case SETTINGS_PI_SLEW:
		  menu_settings_PiSlew_run(&display, menu_counter, &setting);
		  break;
	  case SETTINGS_PI_MAXPOWER:
		  menu_settings_PiMaxPower_run(&display, menu_counter, &setting);
		  break;
	  case SETTINGS_TIMEDATE:
		  menu_settings_TimeDate_run(&display, menu_counter);
		  break;
	  case SETTINGS_TIMEDATE_HOUR:
		  menu_settings_TimeDateHour_run(&display, menu_counter);
		  break;
	  case SETTINGS_TIMEDATE_MINUTE:
		  menu_settings_TimeDateMin_run(&display, menu_counter);
		  break;
	  case SETTINGS_TIMEDATE_DAY:
		  menu_settings_TimeDateDay_run(&display, menu_counter);
		  break;
	  case SETTINGS_TIMEDATE_MONTH:
		  menu_settings_TimeDateMonth_run(&display, menu_counter);
		  break;
	  case SETTINGS_TIMEDATE_YEAR:
		  menu_settings_TimeDateYear_run(&display, menu_counter);
		  break;
	  default:
		  break;

	  }


	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 1000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 40999;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 99;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xFFFFFFFF;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 10;
  sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 10;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 4;
  sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 4;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 41999;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 999;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */
  HAL_TIM_MspPostInit(&htim5);

}

/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM9_Init(void)
{

  /* USER CODE BEGIN TIM9_Init 0 */

  /* USER CODE END TIM9_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  /* USER CODE BEGIN TIM9_Init 1 */

  /* USER CODE END TIM9_Init 1 */
  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 41999;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 99;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV2;
  htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM9_Init 2 */

  /* USER CODE END TIM9_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, MAX_CS5_Pin|MAX_CS4_Pin|BUZZER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RELAY_3_Pin|MAX_CS2_Pin|RESET_Pin|MAX_CS1_Pin
                          |SD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin|MAX_CS3_Pin|RELAY_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : MAX_CS5_Pin MAX_CS4_Pin BUZZER_Pin */
  GPIO_InitStruct.Pin = MAX_CS5_Pin|MAX_CS4_Pin|BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : RELAY_3_Pin MAX_CS2_Pin MAX_CS1_Pin SD_CS_Pin */
  GPIO_InitStruct.Pin = RELAY_3_Pin|MAX_CS2_Pin|MAX_CS1_Pin|SD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_CS_Pin MAX_CS3_Pin RELAY_2_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin|MAX_CS3_Pin|RELAY_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC_1_BUTTON_Pin ENC_2_BUTTON_Pin ENC_3_BUTTON_Pin */
  GPIO_InitStruct.Pin = ENC_1_BUTTON_Pin|ENC_2_BUTTON_Pin|ENC_3_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : RESET_Pin */
  GPIO_InitStruct.Pin = RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(RESET_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


void set_value(volatile encoder *enc, uint32_t *var,
		uint32_t min_value, uint32_t max_value) {

	if (enc->position > enc->old_position) {

		if (*var < max_value) {
			(*var)++;
		}

	} else if (enc->position < enc->old_position) {
		if (*var > min_value) {
			(*var)--;
		}

	}

	enc->old_position = enc->position;
}

//void autotune_tick_1s(void)
//{
//    now_time_s += 1.0f;
//    now_temp_c = cs1_temp;
//
//    if (now_temp_c > MAX_SAFE_C) { now_output_percent = 0; state = TUNE_ABORT; }
//
//    switch (state) {
//      case TUNE_PREPARE:
//        now_output_percent = start_output_percent; // 0%
//        // kaydet
//        log_time_s[log_count] = now_time_s;
//        log_output_percent[log_count] = now_output_percent;
//        log_temp_c[log_count] = now_temp_c;
//        log_count++;
//
//        if (now_time_s >= step_time_s) state = TUNE_STEP;
//        break;
//
//      case TUNE_STEP:
//        now_output_percent = start_output_percent + step_percent; // tek adım
//        // hemen toplaya geç, bu saniyeyi de kaydet
//        log_time_s[log_count] = now_time_s;
//        log_output_percent[log_count] = now_output_percent;
//        log_temp_c[log_count] = now_temp_c;
//        log_count++;
//        state = TUNE_COLLECT;
//        break;
//
//      case TUNE_COLLECT: {
//        // kaydet
//        log_time_s[log_count] = now_time_s;
//        log_output_percent[log_count] = now_output_percent;
//        log_temp_c[log_count] = now_temp_c;
//        log_count++;
//
//        // stabil ölçütü: |dT| < eşik
//        float slope = fabsf(now_temp_c - prev_temp_c); // ~°C/s
//        prev_temp_c = now_temp_c;
//
//        if (slope < STABIL_SLOPE_C_PER_S) stable_counter_s++;
//        else                               stable_counter_s = 0;
//
//        // bitirme koşulları
//        if (stable_counter_s >= STABIL_HOLD_S) state = TUNE_EVALUATE;
//        if ((int)now_time_s >= TIMEOUT_S)      state = TUNE_EVALUATE;
//        break;
//      }
//
//      case TUNE_EVALUATE: {
//        // 1) başlangıç ve son ortalamaları
//        float temp_start_c = average_first_n(log_temp_c, log_count, 10);
//        float temp_final_c = average_last_n (log_temp_c, log_count, 60);
//
//        // 2) delta'lar
//        float first_output = log_output_percent[0];
//        float last_output  = log_output_percent[log_count-1];
//        float delta_output_percent = last_output - first_output;     // ≈ +10
//        float delta_temp_c = temp_final_c - temp_start_c;            // °C
//
//        if (fabsf(delta_temp_c) < MIN_DELTA_TEMP_C) {
//          // bir kez adımı büyüt ve başa sar
//          if (fabsf(step_percent) < 20.0f) {
//            step_percent += (step_percent > 0 ? +5.0f : -5.0f);
//            // reset:
//            log_count = 0; now_time_s = 0; stable_counter_s = 0; prev_temp_c = now_temp_c;
//            state = TUNE_PREPARE; now_output_percent = start_output_percent;
//            break;
//          } else { state = TUNE_ABORT; break; }
//        }
//
//        // 3) KPROC
//        float kproc = delta_temp_c / delta_output_percent; // °C / %
//
//        // 4) TAU (t63)
//        float target_temp_c = temp_start_c + 0.63f * delta_temp_c;
//        float time_t63_s = find_time_crossing(target_temp_c, log_time_s, log_temp_c, log_count);
//        float tau_s = time_t63_s - step_time_s;
//        if (!(tau_s > 0)) tau_s = 60.0f;
//
//        // 5) PI (derating ile)
//        float Kc = 1.0f / (2.0f * kproc);
//        float Ti = tau_s;
//        Kc *= 0.85f;
//        Ti *= 1.20f;
//
//        // 6) kaydet
//        if (step_percent > 0) { save_heat_Kc(Kc); save_heat_Ti(Ti); }
//        else                  { save_cool_Kc(Kc); save_cool_Ti(Ti); }
//
//        now_output_percent = 0;
//        state = TUNE_DONE;
//        break;
//      }
//
//      case TUNE_DONE:
//      case TUNE_ABORT:
//      default:
//        now_output_percent = 0;
//        break;
//    }
//}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
