/*
 * Cooking_Handler.c
 *
 *  Created on: Nov 12, 2025
 *      Author: erenegdemir
 */

#include "Cooking_Handler.h"

#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_tim_ex.h"
#include "stm32f4xx_it.h"

uint32_t i;
uint32_t k;

extern static PI_Oven    pi1;
extern static PI_Oven    pi2;
extern static PI_Oven    pi3;

extern TIM_HandleTypeDef htim5;


void update_DutyCycle(CookingCtx *c, TIM_HandleTypeDef *htim)
{

	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, c->r1_duty * 10);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, c->r2_duty * 10);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, c->r3_duty * 10);
}

void cooking_handler(CookingCtx *c, Sens t)
{


	switch (c->flag){
	case PENDING:
		break;
	case FASTSTART:

		if (c->faststart->tempPerMin >= 0){
			c->r1_duty = PI_Control_Update(&pi1, c->faststart->tempForMin[i], t.sens1_temp, PI_DT);
			c->r2_duty = PI_Control_Update(&pi2, c->faststart->tempForMin[i], t.sens2_temp, PI_DT);
			c->r3_duty = PI_Control_Update(&pi3, c->faststart->tempForMin[i], t.sens3_temp, PI_DT);

			c->duty_mean = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

			update_DutyCycle(c, &htim5);


		}else {

//			uint8_t coolingDiff = tempMean - c->faststart->tempForMin[i];
//
//
//			if(coolingDiff >= 10){
//				cooling_state = 1;
//
//			}else if(coolingDiff >=15){
//				cooling_state = 2;
//
//
//			}else if(coolingDiff >=30){
//				cooling_state = 3;
//
//			}

		}


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


		if (c->stage1->tempPerMin >= 0){
			c->r1_duty = PI_Control_Update(&pi1, c->stage1->tempForMin[i], t.sens1_temp, PI_DT);
			c->r2_duty = PI_Control_Update(&pi2, c->stage1->tempForMin[i], t.sens2_temp, PI_DT);
			c->r3_duty = PI_Control_Update(&pi3, c->stage1->tempForMin[i], t.sens3_temp, PI_DT);

			c->duty_mean = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

			update_DutyCycle(c, &htim5);
		}else {

//			uint8_t coolingDiff = tempMean - c->stage1->tempForMin[i];
//
//
//			if(coolingDiff >= 10){
//				cooling_state = 1;
//
//			}else if(coolingDiff >=15){
//				cooling_state = 2;
//
//
//			}else if(coolingDiff >=30){
//				cooling_state = 3;
//
//			}

		}


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

		if (c->stage2->tempPerMin >= 0){
			c->r1_duty = PI_Control_Update(&pi1, c->stage2->tempForMin[i], t.sens1_temp, PI_DT);
			c->r2_duty = PI_Control_Update(&pi2, c->stage2->tempForMin[i], t.sens2_temp, PI_DT);
			c->r3_duty = PI_Control_Update(&pi3, c->stage2->tempForMin[i], t.sens3_temp, PI_DT);

			c->duty_mean = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

			update_DutyCycle(c, &htim5);
		}else {

//			uint8_t coolingDiff = tempMean - c->stage2->tempForMin[i];
//
//
//			if(coolingDiff >= 10){
//				cooling_state = 1;
//
//			}else if(coolingDiff >=15){
//				cooling_state = 2;
//
//
//			}else if(coolingDiff >=30){
//				cooling_state = 3;
//
//			}

		}

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

		if (c->stage3->tempPerMin >= 0){
			c->r1_duty = PI_Control_Update(&pi1, c->stage3->tempForMin[i], t.sens1_temp, PI_DT);
			c->r2_duty = PI_Control_Update(&pi2, c->stage3->tempForMin[i], t.sens2_temp, PI_DT);
			c->r3_duty = PI_Control_Update(&pi3, c->stage3->tempForMin[i], t.sens3_temp, PI_DT);

			c->duty_mean = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

			update_DutyCycle(c, &htim5);
		}else {

//			uint8_t coolingDiff = tempMean - c->stage3->tempForMin[i];
//
//
//			if(coolingDiff >= 10){
//				cooling_state = 1;
//
//			}else if(coolingDiff >=15){
//				cooling_state = 2;
//
//
//			}else if(coolingDiff >=30){
//				cooling_state = 3;
//
//			}

		}

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

		if (c->stage4->tempPerMin >= 0){
			c->r1_duty = PI_Control_Update(&pi1, c->stage4->tempForMin[i], t.sens1_temp, PI_DT);
			c->r2_duty = PI_Control_Update(&pi2, c->stage4->tempForMin[i], t.sens2_temp, PI_DT);
			c->r3_duty = PI_Control_Update(&pi3, c->stage4->tempForMin[i], t.sens3_temp, PI_DT);

			c->duty_mean = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

			update_DutyCycle(c, &htim5);
		}else {

//			uint8_t coolingDiff = tempMean - c->stage1->tempForMin[i];
//
//
//			if(coolingDiff >= 10){
//				cooling_state = 1;
//
//			}else if(coolingDiff >=15){
//				cooling_state = 2;
//
//
//			}else if(coolingDiff >=30){
//				cooling_state = 3;
//
//			}

		}

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

		if (c->stage5->tempPerMin >= 0){
			c->r1_duty = PI_Control_Update(&pi1, c->stage5->tempForMin[i], t.sens1_temp, PI_DT);
			c->r2_duty = PI_Control_Update(&pi2, c->stage5->tempForMin[i], t.sens2_temp, PI_DT);
			c->r3_duty = PI_Control_Update(&pi3, c->stage5->tempForMin[i], t.sens3_temp, PI_DT);

			c->duty_mean = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

			update_DutyCycle(c, &htim5);
		}else {

//			uint8_t coolingDiff = tempMean - c->stage5->tempForMin[i];
//
//
//			if(coolingDiff >= 10){
//				cooling_state = 1;
//
//			}else if(coolingDiff >=15){
//				cooling_state = 2;
//
//
//			}else if(coolingDiff >=30){
//				cooling_state = 3;
//
//			}

		}

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

		if (c->stage6->tempPerMin >= 0){
			c->r1_duty = PI_Control_Update(&pi1, c->stage6->tempForMin[i], t.sens1_temp, PI_DT);
			c->r2_duty = PI_Control_Update(&pi2, c->stage6->tempForMin[i], t.sens2_temp, PI_DT);
			c->r3_duty = PI_Control_Update(&pi3, c->stage6->tempForMin[i], t.sens3_temp, PI_DT);

			c->duty_mean = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

			update_DutyCycle(c, &htim5);
		}else {

//			uint8_t coolingDiff = tempMean - c->stage6->tempForMin[i];
//
//
//			if(coolingDiff >= 10){
//				cooling_state = 1;
//
//			}else if(coolingDiff >=15){
//				cooling_state = 2;
//
//
//			}else if(coolingDiff >=30){
//				cooling_state = 3;
//
//			}

		}

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
		break;
	default:
		break;
	}

}

