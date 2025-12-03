/**
 * @file    Cooking_Handler.c
 * @brief   Main cooking control module for the composite curing oven.
 *
 * This module implements the heating/cooking control logic for the oven:
 *   - Initializes PI controllers and PWM outputs for three heater zones
 *   - Executes the cooking state machine (FASTSTART + 6-stage program)
 *   - Applies per-minute temperature profiles for each stage
 *   - Computes heater duty cycles using PI_Control_Update()
 *   - Writes duty cycles to TIM5 PWM channels
 *   - Continuously monitors sensor data for safety (via check_error())
 *
 * Control loop overview:
 *   - Cooking_Init():
 *       Starts TIM5 PWM channels (CH2, CH3, CH4) and initializes all
 *       PI_Oven instances (gain, max power, filter, slew rate).
 *
 *   - Cooking_Handler():
 *       Called periodically with fixed timestep PI_DT.
 *       Uses c->flag (CookingFlag) to:
 *         * Run FASTSTART or STAGE_1..STAGE_6 profiles
 *         * Track profile index (i) and sub-tick counter (k)
 *         * Update remaining time (remTime) and move to the next stage
 *         * Abort cooking on faults or when profile completes
 *
 *   - update_DutyCycle():
 *       Applies the computed duty cycles (r1_duty, r2_duty, r3_duty) to the
 *       hardware PWM channels of TIM5.
 *
 * Safety:
 *   - check_error() is called inside Cooking_Handler() to validate sensor
 *     drift between composite sensors (sens1..sens3) and furnace sensors
 *     (sens4..sens5), as well as absolute over-temperature and sensor faults.
 *   - On any violation, c->flag is set to ABORT and Cooking_Handler()
 *     eventually stops heating and triggers a system reset.
 *
 * Globals:
 *   - i, k:
 *       Profile/time indexing counters used by Cooking_Handler() to walk
 *       through tempForMin[] arrays and to manage the time resolution.
 *   - htim5:
 *       External TIM_HandleTypeDef for the PWM timer used to drive the heaters.
 *
 * Usage:
 *   1) Call Cooking_Init() once after system startup and settings load.
 *   2) Periodically call Cooking_Handler() at a fixed rate (matching PI_DT).
 *   3) Use Display_Handler() and encoder handlers to configure stages and
 *      start/stop the cooking process.
 *
 * @author
 *   Eren Eğdemir
 */


#include "Cooking_Handler.h"

#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_tim_ex.h"
#include "stm32f4xx_it.h"
#include <stdio.h>

uint32_t i;
uint32_t k;

void Cooking_Init(setting_t setting, PI_Oven *pi1, PI_Oven *pi2, PI_Oven *pi3)
{
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);

	PI_Control_Init(pi1, setting.kp, setting.ki, 0.0f, setting.max_pwr);
	PI_Control_SetFilter(pi1, 0.6f);        // 0.3–0.7 tipik
	PI_Control_SetSlew(pi1, setting.slew);

	PI_Control_Init(pi2, setting.kp, setting.ki, 0.0f, setting.max_pwr);
	PI_Control_SetFilter(pi2, 0.6f);        // 0.3–0.7 tipik
	PI_Control_SetSlew(pi2, setting.slew);

	PI_Control_Init(pi3, setting.kp, setting.ki, 0.0f, setting.max_pwr);
	PI_Control_SetFilter(pi3, 0.6f);        // 0.3–0.7 tipik
	PI_Control_SetSlew(pi3, setting.slew);

}

void update_DutyCycle(CookingCtx *c, TIM_HandleTypeDef *htim)
{

	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, c->r1_duty * 10);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, c->r2_duty * 10);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, c->r3_duty * 10);
}

void Cooking_Handler(CookingCtx *c, Sens t, PI_Oven *pi1, PI_Oven *pi2, PI_Oven *pi3)
{


	switch (c->flag){
	case PENDING:
		check_error(c, t);
		break;
	case FASTSTART:

			c->r1_duty = PI_Control_Update(pi1, c->faststart->tempForMin[i], t.sens1_temp, PI_DT);
			c->r2_duty = PI_Control_Update(pi2, c->faststart->tempForMin[i], t.sens2_temp, PI_DT);
			c->r3_duty = PI_Control_Update(pi3, c->faststart->tempForMin[i], t.sens3_temp, PI_DT);

			c->duty_mean = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

			check_error(c, t);
			update_DutyCycle(c, &htim5);

		if (++k >= 600) {
			i++;
		    k = 0;
		    c->faststart->remTime--;

		}
		if(i >= c->faststart->totalDuration){

			c->flag = ABORT;

		}
		break;
	case STAGE_1:



			c->r1_duty = PI_Control_Update(pi1, c->stage1->tempForMin[i], t.sens1_temp, PI_DT);
			c->r2_duty = PI_Control_Update(pi2, c->stage1->tempForMin[i], t.sens2_temp, PI_DT);
			c->r3_duty = PI_Control_Update(pi3, c->stage1->tempForMin[i], t.sens3_temp, PI_DT);

			c->duty_mean = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

			check_error(c, t);
			update_DutyCycle(c, &htim5);

		if (++k >= 600) {
			i++;
		    k = 0;
		    c->stage1->remTime--;

		}
		if(i >= c->stage1->totalDuration){

			if(c->stage2->state == TRUE){

				c->flag = STAGE_2;
				i = 0;
				k = 0;
			}else c->flag = ABORT;

		}
		break;
	case STAGE_2:


			c->r1_duty = PI_Control_Update(pi1, c->stage2->tempForMin[i], t.sens1_temp, PI_DT);
			c->r2_duty = PI_Control_Update(pi2, c->stage2->tempForMin[i], t.sens2_temp, PI_DT);
			c->r3_duty = PI_Control_Update(pi3, c->stage2->tempForMin[i], t.sens3_temp, PI_DT);

			c->duty_mean = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

			check_error(c, t);
			update_DutyCycle(c, &htim5);

		if (++k >= 600) {
			i++;
		    k = 0;
		    c->stage2->remTime--;

		}
		if(i >= c->stage2->totalDuration){
			if(c->stage3->state == TRUE){

				c->flag = STAGE_2;
				i = 0;
				k = 0;
			}else c->flag = ABORT;
		}
		break;
	case STAGE_3:


			c->r1_duty = PI_Control_Update(pi1, c->stage3->tempForMin[i], t.sens1_temp, PI_DT);
			c->r2_duty = PI_Control_Update(pi2, c->stage3->tempForMin[i], t.sens2_temp, PI_DT);
			c->r3_duty = PI_Control_Update(pi3, c->stage3->tempForMin[i], t.sens3_temp, PI_DT);

			c->duty_mean = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

			check_error(c, t);
			update_DutyCycle(c, &htim5);

		if (++k >= 600) {
			i++;
		    k = 0;
		    c->stage3->remTime--;
		}
		if(i >= c->stage3->totalDuration){
			if(c->stage4->state == TRUE){

				c->flag = STAGE_4;
				i = 0;
				k = 0;
			}else c->flag = ABORT;
		}
		break;
	case STAGE_4:

			c->r1_duty = PI_Control_Update(pi1, c->stage4->tempForMin[i], t.sens1_temp, PI_DT);
			c->r2_duty = PI_Control_Update(pi2, c->stage4->tempForMin[i], t.sens2_temp, PI_DT);
			c->r3_duty = PI_Control_Update(pi3, c->stage4->tempForMin[i], t.sens3_temp, PI_DT);

			c->duty_mean = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

			check_error(c, t);
			update_DutyCycle(c, &htim5);

		if (++k >= 600) {
			i++;
		    k = 0;
		    c->stage4->remTime--;

		}
		if(i >= c->stage4->totalDuration){
			if(c->stage5->state == TRUE){

				c->flag = STAGE_5;
				i = 0;
				k = 0;
			}else c->flag = ABORT;
		}
		break;
	case STAGE_5:

			c->r1_duty = PI_Control_Update(pi1, c->stage5->tempForMin[i], t.sens1_temp, PI_DT);
			c->r2_duty = PI_Control_Update(pi2, c->stage5->tempForMin[i], t.sens2_temp, PI_DT);
			c->r3_duty = PI_Control_Update(pi3, c->stage5->tempForMin[i], t.sens3_temp, PI_DT);

			c->duty_mean = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

			check_error(c, t);
			update_DutyCycle(c, &htim5);

		if (++k >= 600) {
			i++;
		    k = 0;
		    c->stage5->remTime--;

		}
		if(i >= c->stage5->totalDuration){
			if(c->stage6->state == TRUE){

				c->flag = STAGE_6;
				i = 0;
				k = 0;
			}else c->flag = ABORT;
		}
		break;
	case STAGE_6:

			c->r1_duty = PI_Control_Update(pi1, c->stage6->tempForMin[i], t.sens1_temp, PI_DT);
			c->r2_duty = PI_Control_Update(pi2, c->stage6->tempForMin[i], t.sens2_temp, PI_DT);
			c->r3_duty = PI_Control_Update(pi3, c->stage6->tempForMin[i], t.sens3_temp, PI_DT);

			c->duty_mean = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

			check_error(c, t);
			update_DutyCycle(c, &htim5);

		if (++k >= 600) {
			i++;
		    k = 0;
		    c->stage6->remTime--;

		}
		if(i >= c->stage6->totalDuration){
			i = 0;
			k = 0;
			c->flag = ABORT;
		}
		break;
	case ABORT:
		__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, 0);
		HAL_NVIC_SystemReset();
		break;
	default:
		break;
	}

}

