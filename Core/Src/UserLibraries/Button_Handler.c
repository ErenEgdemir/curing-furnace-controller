/*
 * Button_Handler.c
 *
 *  Created on: Nov 12, 2025
 *      Author: erenegdemir
 */

#include "Button_Handler.h"

/**
 * @brief Sets the menu flag and resets the menu counter.
 * @param[in,out] m  Menu context
 * @param[in]     f  Targeted menu flag.
 */
static inline void display_goto(MenuCtx *m, MenuFlag f)
{
	m->flag = f;
	m->counter = 0;
}

/**
 * @brief  Sets the menu flag and the menu counter.
 * @param[in,out] m  Menu context
 * @param[in]     f  Targeted menu flag.
 * @param[in]	  c  New counter value.
 */
static inline void display_set(MenuCtx *m, MenuFlag f, uint32_t c){
	m->flag = f;
	m->counter = c;
}

/**
 * @brief  Handles menu switching operations of encoder 1 button.
 * @param[in,out] m  Menu context
 */
void button_handler_encoder1(MenuCtx *m, CookingCtx *c, Sens t)
{


	switch (m->flag) {
	case HOME:

		switch (m->counter) {
		case 0:
			display_goto(m, PROGRAMS);
			break;
		case 1:
			display_goto(m, SETTINGS);
			break;
		}
		break;
	case PROGRAMS:
		switch (m->counter) {
		case 0:
			display_goto(m, PROGRAMS_FASTSTART);
			break;
		case 1:
			display_goto(m, PROGRAMS_ADVTEMPCTRL);
			break;
		case 2:
			break;
		case 3:
			display_goto(m, HOME);
			break;
		}
		break;
	case PROGRAMS_FASTSTART:
		switch(m->counter){
		case 0:
			display_set(m, PROGRAMS_FASTSTART_TEMP, 100);
			break;
		case 1:
			display_set(m, PROGRAMS_FASTSTART_RISETIME, 10);
			break;
		case 2:
			display_set(m, PROGRAMS_FASTSTART_FALLTIME, 10);
			break;
		case 3:
			display_set(m, PROGRAMS_FASTSTART_DURATION, 30);
			break;
		case 4:
			display_goto(m, PROGRAMS_ADVTEMPCTRL_GENERATE);
			c->stage1->state = FALSE;
			c->stage2->state = FALSE;
			c->stage3->state = FALSE;
			c->stage4->state = FALSE;
			c->stage5->state = FALSE;
			c->stage6->state = FALSE;
			c->faststart->state = TRUE;
			break;
		case 5:
			display_goto(m, PROGRAMS);
			break;
		default:
			break;
		}
		break;
	case PROGRAMS_FASTSTART_TEMP:
		c->faststart->temperature = m->counter;
		display_goto(m, PROGRAMS_FASTSTART);
		break;
	case PROGRAMS_FASTSTART_RISETIME:
		c->faststart->changePeriod = m->counter;
		display_set(m, PROGRAMS_FASTSTART, 1);
		break;
	case PROGRAMS_FASTSTART_FALLTIME:
		c->faststart->fallTime = m->counter;
		display_set(m, PROGRAMS_FASTSTART, 2);
		break;
	case PROGRAMS_FASTSTART_DURATION:
		c->faststart->duration = m->counter;
		display_set(m, PROGRAMS_FASTSTART, 3);
		break;
	case PROGRAMS_ADVTEMPCTRL:
		c->faststart->state = FALSE;
		switch (m->counter) {
		case 0:
			m->flag = PROGRAMS_ADVTEMPCTRL_RISINGTIME;
			m->counter = 0;
			break;
		case 1:
			if (c->generated == TRUE) {
				c->flag = STAGE_1;
				display_goto(m, COOKING);
			}
			break;
		case 2:
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T1);
			c->stage1->state = FALSE;
			c->stage1->changePeriod = 0;
			c->stage1->duration = 0;
			c->stage1->temperature = 0;
			c->stage1->totalDuration = 0;
			break;
		case 3:
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T2);
			c->stage2->state = FALSE;
			c->stage2->changePeriod = 0;
			c->stage2->duration = 0;
			c->stage2->temperature = 0;
			c->stage2->totalDuration = 0;
			break;
		case 4:
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T3);
			c->stage3->state = FALSE;
			c->stage3->changePeriod = 0;
			c->stage3->duration = 0;
			c->stage3->temperature = 0;
			c->stage3->totalDuration = 0;
			break;
		case 5:
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T4);
			c->stage4->state = FALSE;
			c->stage4->changePeriod = 0;
			c->stage4->duration = 0;
			c->stage4->temperature = 0;
			c->stage4->totalDuration = 0;
			break;
		case 6:
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T5);
			c->stage5->state = FALSE;
			c->stage5->changePeriod = 0;
			c->stage5->duration = 0;
			c->stage5->temperature = 0;
			c->stage5->totalDuration = 0;
			break;
		case 7:
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T6);
			c->stage6->state = FALSE;
			c->stage6->changePeriod = 0;
			c->stage6->duration = 0;
			c->stage6->temperature = 0;
			c->stage6->totalDuration = 0;
			break;
		case 8:
			display_goto(m, HOME);
			break;
		default:
			break;
		}
		break;
	case PROGRAMS_ADVTEMPCTRL_RISINGTIME:
		switch (m->counter) {
		case 0:
			display_goto(m, PROGRAMS_ADVTEMPCTRL_RISINGTIME_S1);
			break;
		case 1:
			display_goto(m, PROGRAMS_ADVTEMPCTRL_RISINGTIME_S2);
			break;
		case 2:
			display_goto(m, PROGRAMS_ADVTEMPCTRL_RISINGTIME_S3);
			break;
		case 3:
			display_goto(m, PROGRAMS_ADVTEMPCTRL_RISINGTIME_S4);
			break;
		case 4:
			display_goto(m, PROGRAMS_ADVTEMPCTRL_RISINGTIME_S5);
			break;
		case 5:
			display_goto(m, PROGRAMS_ADVTEMPCTRL_RISINGTIME_S6);
			break;
		case 7:
			display_goto(m, PROGRAMS_ADVTEMPCTRL_GENERATE);
			break;
		case 8:
			display_goto(m, PROGRAMS_ADVTEMPCTRL);
			break;
		}
		break;
	case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S1:
		c->stage1->changePeriod = m->counter;
		c->stage1->totalDuration = c->stage1->changePeriod + c->stage1->duration;
		display_goto(m, PROGRAMS_ADVTEMPCTRL_RISINGTIME);
		break;
	case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S2:
		c->stage2->changePeriod = m->counter;
		c->stage2->totalDuration = c->stage2->changePeriod + c->stage2->duration;
		display_set(m, PROGRAMS_ADVTEMPCTRL_RISINGTIME, 1);
		break;
	case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S3:
		c->stage3->changePeriod = m->counter;
		c->stage3->totalDuration = c->stage3->changePeriod + c->stage3->duration;
		display_set(m, PROGRAMS_ADVTEMPCTRL_RISINGTIME, 2);
		break;
	case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S4:
		c->stage4->changePeriod = m->counter;
		c->stage4->totalDuration = c->stage4->changePeriod + c->stage4->duration;
		display_set(m, PROGRAMS_ADVTEMPCTRL_RISINGTIME, 3);
		break;
	case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S5:
		c->stage5->changePeriod = m->counter;
		c->stage5->totalDuration = c->stage5->changePeriod + c->stage5->duration;
		display_set(m, PROGRAMS_ADVTEMPCTRL_RISINGTIME, 4);
		break;
	case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S6:
		c->stage6->changePeriod = m->counter;
		c->stage6->totalDuration = c->stage6->changePeriod + c->stage6->duration;
		display_set(m, PROGRAMS_ADVTEMPCTRL_RISINGTIME, 5);
		break;
	case PROGRAMS_ADVTEMPCTRL_T1:
		switch (m->counter) {
		case 0:
			display_set(m, PROGRAMS_ADVTEMPCTRL_T1_TEMP, 90);
			break;
		case 1:
			display_set(m, PROGRAMS_ADVTEMPCTRL_T1_DUR, 60);
			break;
		case 2:
			c->stage1->state = TRUE;
			display_set(m, PROGRAMS_ADVTEMPCTRL_T1_SETRESET, 0);
			break;
		case 3:
			display_set(m, PROGRAMS_ADVTEMPCTRL, 2);
			break;
		default:
			break;
		}
		break;
	case PROGRAMS_ADVTEMPCTRL_T1_TEMP:
		c->stage1->temperature = m->counter;
		display_goto(m, PROGRAMS_ADVTEMPCTRL_T1);
		break;
	case PROGRAMS_ADVTEMPCTRL_T1_DUR:
		c->stage1->duration = m->counter;
		display_set(m, PROGRAMS_ADVTEMPCTRL_T1, 1);
		break;
	case PROGRAMS_ADVTEMPCTRL_T1_SETRESET:
		switch (m->counter) {
		case 0:
			c->stage1->changePeriod = 0;
			c->stage1->duration = 0;
			c->stage1->state = 0;
			c->stage1->temperature = 0;
			c->stage1->totalDuration = 0;
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T1);
			break;
		case 1:
			display_set(m, PROGRAMS_ADVTEMPCTRL, 2);
			break;
		default:
			break;
		}
		break;
	case PROGRAMS_ADVTEMPCTRL_T2:
		switch (m->counter) {
		case 0:
			display_set(m, PROGRAMS_ADVTEMPCTRL_T2_TEMP, 90);
			break;
		case 1:
			display_set(m, PROGRAMS_ADVTEMPCTRL_T2_DUR, 60);
			break;
		case 2:
			c->stage2->state = TRUE;
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T2_SETRESET);
			break;
		case 3:
			display_set(m, PROGRAMS_ADVTEMPCTRL, 3);
			break;
		default:
			break;
		}
		break;
	case PROGRAMS_ADVTEMPCTRL_T2_TEMP:
		c->stage2->temperature = m->counter;
		display_goto(m, PROGRAMS_ADVTEMPCTRL_T2);
		break;
	case PROGRAMS_ADVTEMPCTRL_T2_DUR:
		c->stage2->duration = m->counter;
		display_set(m, PROGRAMS_ADVTEMPCTRL_T2, 1);
		break;
	case PROGRAMS_ADVTEMPCTRL_T2_SETRESET:
		switch (m->counter) {
		case 0:
			c->stage2->changePeriod = 0;
			c->stage2->duration = 0;
			c->stage2->state = 0;
			c->stage2->temperature = 0;
			c->stage2->totalDuration = 0;
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T2);
			break;
		case 1:
			display_set(m, PROGRAMS_ADVTEMPCTRL, 3);
			break;
		}
		break;
	case PROGRAMS_ADVTEMPCTRL_T3:
		switch (m->counter) {
		case 0:
			display_set(m, PROGRAMS_ADVTEMPCTRL_T3_TEMP, 90);
			break;
		case 1:
			display_set(m, PROGRAMS_ADVTEMPCTRL_T3_DUR, 60);
			break;
		case 2:
			c->stage3->state = TRUE;
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T3_SETRESET);
			break;
		case 3:
			display_set(m, PROGRAMS_ADVTEMPCTRL, 2);
			break;
		default:
			break;
		}
		break;
	case PROGRAMS_ADVTEMPCTRL_T3_TEMP:
		c->stage3->temperature = m->counter;
		display_goto(m, PROGRAMS_ADVTEMPCTRL_T3);
		break;
	case PROGRAMS_ADVTEMPCTRL_T3_DUR:
		c->stage3->duration = m->counter;
		display_set(m, PROGRAMS_ADVTEMPCTRL_T3, 1);
		break;
	case PROGRAMS_ADVTEMPCTRL_T3_SETRESET:
		switch (m->counter) {
		case 0:
			c->stage3->changePeriod = 0;
			c->stage3->duration = 0;
			c->stage3->state = 0;
			c->stage3->temperature = 0;
			c->stage3->totalDuration = 0;
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T3);
			break;
		case 1:
			display_set(m, PROGRAMS_ADVTEMPCTRL, 4);
			break;
		}
		break;
	case PROGRAMS_ADVTEMPCTRL_T4:
		switch (m->counter) {
		case 0:
			display_set(m, PROGRAMS_ADVTEMPCTRL_T4_TEMP, 90);
			break;
		case 1:
			display_set(m, PROGRAMS_ADVTEMPCTRL_T4_DUR, 60);
			break;
		case 2:
			c->stage4->state = TRUE;
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T4_SETRESET);
			break;
		case 3:
			display_set(m, PROGRAMS_ADVTEMPCTRL, 2);
			break;
		default:
			break;
		}
		break;
	case PROGRAMS_ADVTEMPCTRL_T4_TEMP:
		c->stage4->temperature = m->counter;
		display_goto(m, PROGRAMS_ADVTEMPCTRL_T4);
		break;
	case PROGRAMS_ADVTEMPCTRL_T4_DUR:
		c->stage4->duration = m->counter;
		display_set(m, PROGRAMS_ADVTEMPCTRL_T4, 1);
		break;
	case PROGRAMS_ADVTEMPCTRL_T4_SETRESET:
		switch (m->counter) {
		case 0:
			c->stage4->changePeriod = 0;
			c->stage4->duration = 0;
			c->stage4->state = 0;
			c->stage4->temperature = 0;
			c->stage4->totalDuration = 0;
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T4);
			break;
		case 1:
			display_set(m, PROGRAMS_ADVTEMPCTRL, 2);
			break;
		default:
			break;
		}
		break;
	case PROGRAMS_ADVTEMPCTRL_T5:
		switch (m->counter) {
		case 0:
			display_set(m, PROGRAMS_ADVTEMPCTRL_T5_TEMP, 90);
			break;
		case 1:
			display_set(m, PROGRAMS_ADVTEMPCTRL_T5_DUR, 60);
			break;
		case 2:
			c->stage5->state = TRUE;
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T5_SETRESET);
			break;
		case 3:
			display_set(m, PROGRAMS_ADVTEMPCTRL, 2);
			break;
		default:
			break;
		}
		break;
	case PROGRAMS_ADVTEMPCTRL_T5_TEMP:
		c->stage5->temperature = m->counter;
		display_goto(m, PROGRAMS_ADVTEMPCTRL_T5);
		break;
	case PROGRAMS_ADVTEMPCTRL_T5_DUR:
		c->stage5->duration = m->counter;
		display_set(m, PROGRAMS_ADVTEMPCTRL_T5, 1);
		break;
	case PROGRAMS_ADVTEMPCTRL_T5_SETRESET:
		switch (m->counter) {
		case 0:
			c->stage5->changePeriod = 0;
			c->stage5->duration = 0;
			c->stage5->state = 0;
			c->stage5->temperature = 0;
			c->stage5->totalDuration = 0;
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T5);
			break;
		case 1:
			display_set(m, PROGRAMS_ADVTEMPCTRL, 2);
			break;
		}
		break;
	case PROGRAMS_ADVTEMPCTRL_T6:
		switch (m->counter) {
		case 0:
			display_set(m, PROGRAMS_ADVTEMPCTRL_T6_TEMP, 90);
			break;
		case 1:
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T6_DUR);
			break;
		case 2:
			c->stage6->state = TRUE;
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T6_SETRESET);
			break;
		case 3:
			display_set(m, PROGRAMS_ADVTEMPCTRL, 2);
			break;
		default:
			break;
		}
		break;
	case PROGRAMS_ADVTEMPCTRL_T6_TEMP:
		c->stage6->temperature = m->counter;
		display_goto(m, PROGRAMS_ADVTEMPCTRL_T6);
		break;
	case PROGRAMS_ADVTEMPCTRL_T6_DUR:
		c->stage6->duration = m->counter;
		display_set(m, PROGRAMS_ADVTEMPCTRL_T6, 1);
		break;
	case PROGRAMS_ADVTEMPCTRL_T6_SETRESET:
		switch (m->counter) {
		case 0:
			c->stage6->changePeriod = 0;
			c->stage6->duration = 0;
			c->stage6->state = 0;
			c->stage6->temperature = 0;
			c->stage6->totalDuration = 0;
			display_goto(m, PROGRAMS_ADVTEMPCTRL_T6);
			break;
		case 1:
			m->flag = PROGRAMS_ADVTEMPCTRL;
			m->counter = 2;
			display_set(m, PROGRAMS_ADVTEMPCTRL, 7);
			break;
		default:
			break;
		}
		break;
	case SETTINGS:
		switch (m->counter) {
		case 0:
			display_goto(m, SETTINGS_PI);
			break;
		case 1:
			break;
		case 2:
			display_goto(m, SETTINGS_TIMEDATE);
			break;
		case 3:

			break;
		case 4:
			m->flag = HOME;
			cfg->kp = setting.kp;
			cfg->ki = setting.ki;
			cfg->slew = setting.slew;
			cfg->maxPwr = setting.max_pwr;

			Settings_Save();
			HAL_NVIC_SystemReset();
			break;
		case 5:
			display_goto(m, HOME);
			break;
		}
		break;
	case SETTINGS_PI:
		switch(m->counter){
		case 0:
			display_set(m, SETTINGS_PI_KP, setting.kp);
			break;
		case 1:
			display_set(m, SETTINGS_PI_KI, setting.ki);
			break;
		case 2:
			display_set(m, SETTINGS_PI_SLEW, setting.slew);
			break;
		case 3:
			display_set(m, SETTINGS_PI_MAXPOWER, setting.max_pwr);
			break;
		case 4:
			display_goto(m, SETTINGS);
		default:
			break;
		}
		break;
	case SETTINGS_PI_KP:
		setting.kp = (float)m->counter / 100;
		display_goto(m, SETTINGS_PI);
		break;
	case SETTINGS_PI_KI:
		setting.ki = (float)m->counter / 100;
		display_set(m, SETTINGS_PI, 1);
		break;
	case SETTINGS_PI_SLEW:
		setting.slew = (float)m->counter / 100;
		display_set(m, SETTINGS_PI, 2);
		break;
	case SETTINGS_PI_MAXPOWER:
		setting.max_pwr = m->counter;
		display_set(m, SETTINGS_PI, 3);
		break;
	case SETTINGS_TIMEDATE:
		switch (m->counter) {
		case 0:
			display_set(m, SETTINGS_TIMEDATE_HOUR, t.hour);
			break;
		case 1:
			display_set(m, SETTINGS_TIMEDATE_MINUTE, t.minute);
			break;
		case 2:
			display_set(m, SETTINGS_TIMEDATE_DAY, t.date);
			break;
		case 3:
			display_set(m, SETTINGS_TIMEDATE_MONTH, t.month);
			break;
		case 4:
			display_set(m, SETTINGS_TIMEDATE_YEAR, t.year);
			break;
		case 5:
			display_goto(m, HOME);
			break;
		default:
			break;
		}

		break;
	case SETTINGS_TIMEDATE_HOUR:
		DS1307_SetHour(m->counter);
		display_goto(m, SETTINGS_TIMEDATE);
		break;
	case SETTINGS_TIMEDATE_MINUTE:
		DS1307_SetMinute(m->counter);
		display_set(m, SETTINGS_TIMEDATE, 1);
		break;
	case SETTINGS_TIMEDATE_DAY:
		DS1307_SetDate(m->counter);
		display_set(m, SETTINGS_TIMEDATE, 2);
		break;
	case SETTINGS_TIMEDATE_MONTH:
		DS1307_SetMonth(m->counter);
		display_set(m, SETTINGS_TIMEDATE, 3);
		break;
	case SETTINGS_TIMEDATE_YEAR:
		DS1307_SetYear(m->counter);
		display_set(m, SETTINGS_TIMEDATE, 4);
		break;
	case COOKING_DURATION:
		switch (c->flag) {
		case STAGE_1:
			c->stage1->duration = m->counter;
			break;
		case STAGE_2:
			c->stage2->duration = m->counter;
			break;
		case STAGE_3:
			c->stage3->duration = m->counter;
			break;
		case STAGE_4:
			c->stage4->duration = m->counter;
			break;
		case STAGE_5:
			c->stage5->duration = m->counter;
			break;
		case STAGE_6:
			c->stage6->duration = m->counter;
			break;
		case FASTSTART:
			c->faststart->duration = m->counter;
			break;
		default:
			break;
		}
		display_goto(m, PROGRAMS_ADVTEMPCTRL_GENERATE);
		break;
	case COOKING:

		switch (c->flag) {
		case STAGE_1:
			display_set(m, COOKING_DURATION, c->stage1->remTime);
			break;
		case STAGE_2:
			display_set(m, COOKING_DURATION, c->stage2->remTime);
			break;
		case STAGE_3:
			display_set(m, COOKING_DURATION, c->stage3->remTime);
			break;
		case STAGE_4:
			display_set(m, COOKING_DURATION, c->stage4->remTime);
			break;
		case STAGE_5:
			display_set(m, COOKING_DURATION, c->stage5->remTime);
			break;
		case STAGE_6:
			display_set(m, COOKING_DURATION, c->stage6->remTime);
			break;
		case FASTSTART:
			display_set(m, COOKING_DURATION, c->faststart->remTime);
			break;
		default:
			break;
		}

		break;
	default:
		break;

	}

}

