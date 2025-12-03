/*
 * common_def.h
 *
 *  Created on: Dec 2, 2025
 *      Author: erenegdemir
 */

#ifndef INC_USERLIBRARIES_COMMON_DEF_H_
#define INC_USERLIBRARIES_COMMON_DEF_H_

#define TRUE		1
#define FALSE		0

/**
 * @brief  User interface menu/state flags for the curing oven controller.
 *
 * This enum defines every screen and editing state in the UI state machine.
 * All encoder handlers (tick + button) use these flags to decide screen
 * transitions, parameter ranges, and editing modes.
 */
typedef enum {

	/* ----------------------------------------------------------------------
	 *  MAIN SCREENS
	 * ---------------------------------------------------------------------- */
	HOME = 0,                  /**< Home screen */
	PROGRAMS,                 /**< Program selection menu */
	SETTINGS,                 /**< Settings main menu */
	COOKING,                  /**< Active cooking screen */

	/* ----------------------------------------------------------------------
	 *  COOKING EDIT MODES
	 * ---------------------------------------------------------------------- */
	COOKING_TEMPCHANGE,       /**< Editing target temperature during cooking */
	COOKING_PWRCHANGE,        /**< Editing maximum heater power */
	COOKING_DURATION,         /**< Editing duration of the active cooking stage */

	/* ----------------------------------------------------------------------
	 *  PI CONFIGURATION (SETTINGS → PI)
	 * ---------------------------------------------------------------------- */
	SETTINGS_PI,              /**< PI configuration menu */
	SETTINGS_PI_KP,           /**< Kp gain edit */
	SETTINGS_PI_KI,           /**< Ki gain edit */
	SETTINGS_PI_SLEW,         /**< Slew-rate limit edit */
	SETTINGS_PI_MAXPOWER,     /**< Maximum allowed power edit */

	/* ----------------------------------------------------------------------
	 *  TEMPERATURE OFFSET SETTINGS
	 * ---------------------------------------------------------------------- */
	SETTINGS_TEMPOFFSET,      /**< Multi-zone temperature offset menu */
	SETTINGS_TEMPOFFSET_S1,   /**< Temperature offset for sensor 1 */
	SETTINGS_TEMPOFFSET_S2,   /**< Temperature offset for sensor 2 */
	SETTINGS_TEMPOFFSET_S3,   /**< Temperature offset for sensor 3 */
	SETTINGS_TEMPOFFSET_S4,   /**< Temperature offset for sensor 4 */
	SETTINGS_TEMPOFFSET_S5,   /**< Temperature offset for sensor 5 */

	/* ----------------------------------------------------------------------
	 *  TIME & DATE SETTINGS
	 * ---------------------------------------------------------------------- */
	SETTINGS_TIMEDATE,        /**< Time/date adjustment screen */
	SETTINGS_TIMEDATE_HOUR,   /**< Hour edit */
	SETTINGS_TIMEDATE_MINUTE, /**< Minute edit */
	SETTINGS_TIMEDATE_DAY,    /**< Day edit */
	SETTINGS_TIMEDATE_MONTH,  /**< Month edit */
	SETTINGS_TIMEDATE_YEAR,   /**< Year edit */

	/* ----------------------------------------------------------------------
	 *  FASTSTART PROGRAM (PRESET SIMPLE PROFILE)
	 * ---------------------------------------------------------------------- */
	PROGRAMS_FASTSTART,            /**< FastStart program main menu */
	PROGRAMS_FASTSTART_TEMP,       /**< FastStart target temperature edit */
	PROGRAMS_FASTSTART_RISETIME,   /**< FastStart rise time edit */
	PROGRAMS_FASTSTART_FALLTIME,   /**< FastStart fall time edit */
	PROGRAMS_FASTSTART_DURATION,   /**< FastStart duration edit */

	/* ----------------------------------------------------------------------
	 *  ADVANCED 6-STAGE TEMPERATURE CONTROL PROGRAM
	 * ---------------------------------------------------------------------- */
	PROGRAMS_ADVTEMPCTRL,                 /**< Advanced temp control main menu */
	PROGRAMS_ADVTEMPCTRL_RISINGTIME,      /**< Stage rising-time selection menu */

	/* Rising-time adjustments for individual stages */
	PROGRAMS_ADVTEMPCTRL_RISINGTIME_S1,   /**< Stage 1 rising time edit */
	PROGRAMS_ADVTEMPCTRL_RISINGTIME_S2,   /**< Stage 2 rising time edit */
	PROGRAMS_ADVTEMPCTRL_RISINGTIME_S3,   /**< Stage 3 rising time edit */
	PROGRAMS_ADVTEMPCTRL_RISINGTIME_S4,   /**< Stage 4 rising time edit */
	PROGRAMS_ADVTEMPCTRL_RISINGTIME_S5,   /**< Stage 5 rising time edit */
	PROGRAMS_ADVTEMPCTRL_RISINGTIME_S6,   /**< Stage 6 rising time edit */

	PROGRAMS_ADVTEMPCTRL_GENERATE,        /**< Generates full 6-stage program summary */

	/* ----------------------------------------------------------------------
	 *  PER-STAGE TEMPERATURE + DURATION + ENABLE/RESET
	 * ---------------------------------------------------------------------- */

	/* ---- Stage 1 ---- */
	PROGRAMS_ADVTEMPCTRL_T1,              /**< Stage 1 config menu */
	PROGRAMS_ADVTEMPCTRL_T1_TEMP,         /**< Stage 1 temperature edit */
	PROGRAMS_ADVTEMPCTRL_T1_DUR,          /**< Stage 1 duration edit */
	PROGRAMS_ADVTEMPCTRL_T1_SETRESET,     /**< Stage 1 enable/reset */

	/* ---- Stage 2 ---- */
	PROGRAMS_ADVTEMPCTRL_T2,
	PROGRAMS_ADVTEMPCTRL_T2_TEMP,
	PROGRAMS_ADVTEMPCTRL_T2_DUR,
	PROGRAMS_ADVTEMPCTRL_T2_SETRESET,

	/* ---- Stage 3 ---- */
	PROGRAMS_ADVTEMPCTRL_T3,
	PROGRAMS_ADVTEMPCTRL_T3_TEMP,
	PROGRAMS_ADVTEMPCTRL_T3_DUR,
	PROGRAMS_ADVTEMPCTRL_T3_SETRESET,

	/* ---- Stage 4 ---- */
	PROGRAMS_ADVTEMPCTRL_T4,
	PROGRAMS_ADVTEMPCTRL_T4_TEMP,
	PROGRAMS_ADVTEMPCTRL_T4_DUR,
	PROGRAMS_ADVTEMPCTRL_T4_SETRESET,

	/* ---- Stage 5 ---- */
	PROGRAMS_ADVTEMPCTRL_T5,
	PROGRAMS_ADVTEMPCTRL_T5_TEMP,
	PROGRAMS_ADVTEMPCTRL_T5_DUR,
	PROGRAMS_ADVTEMPCTRL_T5_SETRESET,

	/* ---- Stage 6 ---- */
	PROGRAMS_ADVTEMPCTRL_T6,
	PROGRAMS_ADVTEMPCTRL_T6_TEMP,
	PROGRAMS_ADVTEMPCTRL_T6_DUR,
	PROGRAMS_ADVTEMPCTRL_T6_SETRESET

} MenuFlag;

/**
 * @brief  Cooking state machine flags for the curing oven controller.
 *
 * These values represent the active stage of the cooking process.
 * The cooking logic, display updates, remaining-time calculations and
 * encoder handlers use this flag to determine which stage is currently active.
 *
 * The stages consist of:
 *  - Six configurable heating stages (STAGE_1 .. STAGE_6)
 *  - A simplified FastStart mode
 *  - A pending state (before start)
 *  - An abort state (manual or safety-triggered cancellation)
 */
typedef enum
{
	PENDING = 0,  /**< Cooking has not started yet; waiting for user action */

	/* ----------------------------------------------------------------------
	 *  Multi-stage program (Advanced Temp Control)
	 * ---------------------------------------------------------------------- */
	STAGE_1,      /**< Stage 1 active */
	STAGE_2,      /**< Stage 2 active */
	STAGE_3,      /**< Stage 3 active */
	STAGE_4,      /**< Stage 4 active */
	STAGE_5,      /**< Stage 5 active */
	STAGE_6,      /**< Stage 6 active */

	/* ----------------------------------------------------------------------
	 *  FastStart program
	 * ---------------------------------------------------------------------- */
	FASTSTART,    /**< FastStart (single-profile) cooking mode active */

	/* ----------------------------------------------------------------------
	 *  Abort / Emergency stop
	 * ---------------------------------------------------------------------- */
	ABORT,        /**< Cooking aborted manually or due to a safety event */

} CookingFlag;


#endif /* INC_USERLIBRARIES_COMMON_DEF_H_ */
