/*
 * Display_Handler.c
 *
 *  Created on: Dec 2, 2025
 *      Author: erenegdemir
 */

#include "Display_Handler.h"   // kendi prototipin varsa

void Display_Handler(u8g2_t *d, MenuCtx *m, CookingCtx *c, Sens t, setting_t *s)
{
	  switch(m->flag){
	  case HOME:
		  menu_home_run(d, *m, t);
		  break;
	  case PROGRAMS:
	  	  menu_programs_run(d, *m);
	  	  break;
	  case PROGRAMS_ADVTEMPCTRL:
		  menu_prg_AdvTempCtrl_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_GENERATE:
		  menu_GenerateCookingProcess(d, m, c, t);
		  break;
	  case COOKING:
		  menu_cooking_run(d, c, t, *s);
		  break;
	  case COOKING_TEMPCHANGE:
		  menu_cooking_run_TempChange(d, *m, c, t, *s);
		  break;
	  case COOKING_PWRCHANGE:
		  menu_cooking_PowerChange_run(d, *m, c, t);
		  break;
	  case COOKING_DURATION:
		  menu_cooking_run_Duration(d, *m, c, t, *s);
		  break;
	  case PROGRAMS_FASTSTART:
		  menu_prg_FastStart_run(d, *m, *c);
		  break;
	  case PROGRAMS_FASTSTART_TEMP:
		  menu_prg_FastStart_TrgtTemp_run(d, *m, *c);
		  break;
	  case PROGRAMS_FASTSTART_RISETIME:
		  menu_prg_FastStart_RiseTime_run(d, *m, *c);
		  break;
	  case PROGRAMS_FASTSTART_FALLTIME:
		  menu_prg_FastStart_FallTime_run(d, *m, *c);
		  break;
	  case PROGRAMS_FASTSTART_DURATION:
		  menu_prg_FastStart_Duration_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_RISINGTIME:
		  menu_prg_AdvTempCtrl_RisingTimes_run(d, m, *c, t);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S1:
		  menu_prg_AdvTempCtrl_RisingTimes_st1_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S2:
		  menu_prg_AdvTempCtrl_RisingTimes_st2_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S3:
		  menu_prg_AdvTempCtrl_RisingTimes_st3_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S4:
		  menu_prg_AdvTempCtrl_RisingTimes_st4_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S5:
		  menu_prg_AdvTempCtrl_RisingTimes_st5_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_RISINGTIME_S6:
		  menu_prg_AdvTempCtrl_RisingTimes_st6_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T1:
		  menu_prg_AdvTempCtrl_STG1_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T1_TEMP:
		  menu_prg_AdvTempCtrl_STG1_SetTemp_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T1_DUR:
		  menu_prg_AdvTempCtrl_STG1_SetDur_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T1_SETRESET:
		  menu_prg_AdvTempCtrl_STG1_SetReset_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T2:
		  menu_prg_AdvTempCtrl_STG2_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T2_TEMP:
		  menu_prg_AdvTempCtrl_STG2_SetTemp_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T2_DUR:
		  menu_prg_AdvTempCtrl_STG2_SetDur_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T2_SETRESET:
		  menu_prg_AdvTempCtrl_STG2_SetReset_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T3:
		  menu_prg_AdvTempCtrl_STG3_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T3_TEMP:
		  menu_prg_AdvTempCtrl_STG3_SetTemp_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T3_DUR:
		  menu_prg_AdvTempCtrl_STG3_SetDur_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T3_SETRESET:
		  menu_prg_AdvTempCtrl_STG3_SetReset_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T4:
		  menu_prg_AdvTempCtrl_STG4_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T4_TEMP:
		  menu_prg_AdvTempCtrl_STG4_SetTemp_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T4_DUR:
		  menu_prg_AdvTempCtrl_STG4_SetDur_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T4_SETRESET:
		  menu_prg_AdvTempCtrl_STG4_SetReset_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T5:
		  menu_prg_AdvTempCtrl_STG5_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T5_TEMP:
		  menu_prg_AdvTempCtrl_STG5_SetTemp_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T5_DUR:
		  menu_prg_AdvTempCtrl_STG5_SetDur_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T5_SETRESET:
		  menu_prg_AdvTempCtrl_STG5_SetReset_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T6:
		  menu_prg_AdvTempCtrl_STG4_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T6_TEMP:
		  menu_prg_AdvTempCtrl_STG6_SetTemp_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T6_DUR:
		  menu_prg_AdvTempCtrl_STG6_SetDur_run(d, *m, *c);
		  break;
	  case PROGRAMS_ADVTEMPCTRL_T6_SETRESET:
		  menu_prg_AdvTempCtrl_STG6_SetReset_run(d, *m, *c);
		  break;
	  case SETTINGS:
		  menu_settings_run(d, *m);
		  break;
	  case SETTINGS_PI:
		  menu_settings_Pi_run(d, *m, s);
		  break;
	  case SETTINGS_PI_KP:
		  menu_settings_PiKp_run(d, *m, s);
		  break;
	  case SETTINGS_PI_KI:
		  menu_settings_PiKi_run(d, *m, s);
		  break;
	  case SETTINGS_PI_SLEW:
		  menu_settings_PiSlew_run(d, *m, s);
		  break;
	  case SETTINGS_PI_MAXPOWER:
		  menu_settings_PiMaxPower_run(d, *m, s);
		  break;
	  case SETTINGS_TIMEDATE:
		  menu_settings_TimeDate_run(d, *m, t);
		  break;
	  case SETTINGS_TIMEDATE_HOUR:
		  menu_settings_TimeDateHour_run(d, *m, t);
		  break;
	  case SETTINGS_TIMEDATE_MINUTE:
		  menu_settings_TimeDateMin_run(d, *m, t);
		  break;
	  case SETTINGS_TIMEDATE_DAY:
		  menu_settings_TimeDateDay_run(d, *m, t);
		  break;
	  case SETTINGS_TIMEDATE_MONTH:
		  menu_settings_TimeDateMonth_run(d, *m, t);
		  break;
	  case SETTINGS_TIMEDATE_YEAR:
		  menu_settings_TimeDateYear_run(d, *m, t);
		  break;
	  default:
		  break;

	  }
}
