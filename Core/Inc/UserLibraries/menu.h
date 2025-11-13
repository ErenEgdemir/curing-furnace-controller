/*
 * menu.h
 *
 *  Created on: Oct 16, 2025
 *      Author: erenegdemir
 */

#ifndef INC_MENU_H_
#define INC_MENU_H_

#include "u8g2.h"
#include "main.h"
#include <stdint.h>
#include <stdio.h>

#define TRUE 				1
#define FALSE 				0
#define STAGE_1 			1
#define STAGE_2				2
#define STAGE_3 			3
#define STAGE_4				4
#define STAGE_5 			5
#define STAGE_6				6
#define FASTSTART			7

extern uint8_t generate_flag;

extern uint8_t cs1_temp;
extern uint8_t cs2_temp;
extern uint8_t cs3_temp;
extern uint8_t cs4_temp;
extern uint8_t cs5_temp;

extern uint8_t date;
extern uint8_t month;
extern uint32_t year;

extern uint8_t hour;
extern uint8_t minute;

extern float Loading_Coefficent;
extern uint32_t total_duration;
extern uint32_t Loading;
extern uint32_t Loading_P;




typedef struct {
	uint16_t temperature;
	uint16_t duration;
	uint8_t durationPX;
	uint8_t state;
	uint16_t changePeriod;
	uint8_t changePeriodPX;
	uint8_t fallTime;
	float realTimeTemp;
	float tempForMin[512];
	float tempPerMin;
	float tempPerMinDown;
	uint32_t remTime;
	uint16_t totalDuration;
}stage_t;

typedef struct
{
	float kp;
	float ki;
	float slew;
	float max_pwr;

	int8_t t1_offset;
	int8_t t2_offset;
	int8_t t3_offset;
	int8_t t4_offset;
	int8_t t5_offset;

//	uint16_t p1_stg1[512];
//	uint16_t p1_stg2[512];
//	uint16_t p1_stg3[512];
//	uint16_t p1_stg4[512];
//	uint16_t p1_stg5[512];
//	uint16_t p1_stg6[512];
////
//	uint16_t p2_stg1[512];
//	uint16_t p2_stg2[512];
//	uint16_t p2_stg3[512];
//	uint16_t p2_stg4[512];
//	uint16_t p2_stg5[512];
//	uint16_t p2_stg6[512];
//
//	uint16_t p3_stg1[512];
//	uint16_t p3_stg2[512];
//	uint16_t p3_stg3[512];
//	uint16_t p3_stg4[512];
//	uint16_t p3_stg5[512];
//	uint16_t p3_stg6[512];
}setting_t;

extern stage_t stage1;
extern stage_t stage2;
extern stage_t stage3;
extern stage_t stage4;
extern stage_t stage5;
extern stage_t stage6;

extern stage_t fastStart;

extern uint8_t cooking_flag;
extern uint16_t heater_pwm;

extern uint8_t powerLimit;
extern uint8_t cooling_state;

extern setting_t setting;


void menu_send_time(u8g2_t *display, uint8_t x, uint8_t y, int hh, int mm);
void menu_send_date(u8g2_t *display, uint8_t x, uint8_t y, int dd, int mm, int yy);
void menu_send_logo(u8g2_t *display, uint8_t x, uint8_t y);
void menu_send_int_1(u8g2_t *display, uint8_t x, uint8_t y, int val);
void menu_send_int_2(u8g2_t *display, uint8_t x, uint8_t y, int val);
void menu_send_int_3(u8g2_t *display, uint8_t x, uint8_t y, int val);
void menu_send_int_4(u8g2_t *display, uint8_t x, uint8_t y, int val);
void menu_send_fan(u8g2_t *display, uint8_t x, uint8_t y);
void menu_send_flame(u8g2_t *display, uint8_t x, uint8_t y);
void menu_send_setupLogo(u8g2_t *display, uint8_t x, uint8_t y);


void menu_home_run(u8g2_t *display, uint8_t menu_counter);


void menu_programs_run(u8g2_t *display, uint8_t menu_counter);

void menu_prg_AdvTempCtrl_run(u8g2_t *display, uint8_t menu_counter);

void menu_prg_AdvTempCtrl_RisingTimes_run(u8g2_t *display, uint32_t *menu_counter_p);
void menu_prg_AdvTempCtrl_RisingTimes_st1_run(u8g2_t *display, uint32_t menu_counter);
void menu_prg_AdvTempCtrl_RisingTimes_st2_run(u8g2_t *display, uint32_t menu_counter);
void menu_prg_AdvTempCtrl_RisingTimes_st3_run(u8g2_t *display, uint32_t menu_counter);
void menu_prg_AdvTempCtrl_RisingTimes_st4_run(u8g2_t *display, uint32_t menu_counter);
void menu_prg_AdvTempCtrl_RisingTimes_st5_run(u8g2_t *display, uint32_t menu_counter);
void menu_prg_AdvTempCtrl_RisingTimes_st6_run(u8g2_t *display, uint32_t menu_counter);

void menu_GenerateCookingProcess(u8g2_t *display,uint32_t *menu_flag, uint32_t *menu_counter, stage_t *s1, stage_t *s2,
		stage_t *s3, stage_t *s4,stage_t *s5, stage_t *s6, stage_t *fast);

void menu_cooking_run(u8g2_t *display, stage_t *stage1, stage_t *stage2, stage_t *stage3,
		stage_t *stage4, stage_t *stage5, stage_t *stage6, stage_t *fastStart);
void menu_cooking_run_TempChange(u8g2_t *display, uint32_t menu_counter, stage_t *stage1, stage_t *stage2,
		stage_t *stage3, stage_t *stage4, stage_t *stage5, stage_t *stage6, stage_t *fast);
void menu_cooking_PowerChange_run(u8g2_t *display, uint32_t menu_counter, stage_t *stage1, stage_t *stage2,
		stage_t *stage3, stage_t *stage4, stage_t *stage5, stage_t *stage6, stage_t *fastStart);
void menu_cooking_run_Duration(u8g2_t *display, uint32_t menu_counter, stage_t *stage1, stage_t *stage2,
		stage_t *stage3, stage_t *stage4, stage_t *stage5, stage_t *stage6, stage_t *fast);


void menu_prg_FastStart_run(u8g2_t *display, uint8_t menu_counter);
void menu_prg_FastStart_TrgtTemp_run(u8g2_t *display, uint8_t menu_counter);
void menu_prg_FastStart_RiseTime_run(u8g2_t *display, uint8_t menu_counter);
void menu_prg_FastStart_FallTime_run(u8g2_t *display, uint8_t menu_counter);
void menu_prg_FastStart_Duration_run(u8g2_t *display, uint8_t menu_counter);

void menu_prg_AdvTempCtrl_STG1_run(u8g2_t *display, uint8_t menu_counter);
void menu_prg_AdvTempCtrl_STG2_run(u8g2_t *display, uint8_t menu_counter);
void menu_prg_AdvTempCtrl_STG3_run(u8g2_t *display, uint8_t menu_counter);
void menu_prg_AdvTempCtrl_STG4_run(u8g2_t *display, uint8_t menu_counter);
void menu_prg_AdvTempCtrl_STG5_run(u8g2_t *display, uint8_t menu_counter);
void menu_prg_AdvTempCtrl_STG6_run(u8g2_t *display, uint8_t menu_counter);

void menu_prg_AdvTempCtrl_STG1_SetTemp_run(u8g2_t *display, uint16_t menu_counter);
void menu_prg_AdvTempCtrl_STG2_SetTemp_run(u8g2_t *display, uint16_t menu_counter);
void menu_prg_AdvTempCtrl_STG3_SetTemp_run(u8g2_t *display, uint16_t menu_counter);
void menu_prg_AdvTempCtrl_STG4_SetTemp_run(u8g2_t *display, uint16_t menu_counter);
void menu_prg_AdvTempCtrl_STG5_SetTemp_run(u8g2_t *display, uint16_t menu_counter);
void menu_prg_AdvTempCtrl_STG6_SetTemp_run(u8g2_t *display, uint16_t menu_counter);

void menu_prg_AdvTempCtrl_STG1_SetDur_run(u8g2_t *display, uint16_t menu_counter);
void menu_prg_AdvTempCtrl_STG2_SetDur_run(u8g2_t *display, uint16_t menu_counter);
void menu_prg_AdvTempCtrl_STG3_SetDur_run(u8g2_t *display, uint16_t menu_counter);
void menu_prg_AdvTempCtrl_STG4_SetDur_run(u8g2_t *display, uint16_t menu_counter);
void menu_prg_AdvTempCtrl_STG5_SetDur_run(u8g2_t *display, uint16_t menu_counter);
void menu_prg_AdvTempCtrl_STG6_SetDur_run(u8g2_t *display, uint16_t menu_counter);

void menu_prg_AdvTempCtrl_STG1_SetReset_run(u8g2_t *display, uint8_t menu_counter);
void menu_prg_AdvTempCtrl_STG2_SetReset_run(u8g2_t *display, uint8_t menu_counter);
void menu_prg_AdvTempCtrl_STG3_SetReset_run(u8g2_t *display, uint8_t menu_counter);
void menu_prg_AdvTempCtrl_STG4_SetReset_run(u8g2_t *display, uint8_t menu_counter);
void menu_prg_AdvTempCtrl_STG5_SetReset_run(u8g2_t *display, uint8_t menu_counter);
void menu_prg_AdvTempCtrl_STG6_SetReset_run(u8g2_t *display, uint8_t menu_counter);


void menu_settings_run(u8g2_t *display, uint8_t menu_counter);

void menu_settings_TempOffset_run(u8g2_t *display, uint32_t menu_counter, setting_t *s);


void menu_settings_Pi_run(u8g2_t *display, uint32_t menu_counter, setting_t *s);
void menu_settings_PiKp_run(u8g2_t *display, uint8_t menu_counter, setting_t *s);
void menu_settings_PiKi_run(u8g2_t *display, uint8_t menu_counter, setting_t *s);
void menu_settings_PiSlew_run(u8g2_t *display, uint8_t menu_counter, setting_t *s);
void menu_settings_PiMaxPower_run(u8g2_t *display, uint8_t menu_counter, setting_t *s);

void menu_settings_TimeDate_run(u8g2_t *display, uint8_t menu_counter);
void menu_settings_TimeDateHour_run(u8g2_t *display, uint8_t menu_counter);
void menu_settings_TimeDateMin_run(u8g2_t *display, uint8_t menu_counter);
void menu_settings_TimeDateDay_run(u8g2_t *display, uint8_t menu_counter);
void menu_settings_TimeDateMonth_run(u8g2_t *display, uint8_t menu_counter);
void menu_settings_TimeDateYear_run(u8g2_t *display, uint32_t menu_counter);





#endif /* INC_MENU_H_ */
