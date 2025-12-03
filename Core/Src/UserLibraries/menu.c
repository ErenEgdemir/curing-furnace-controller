/**
 * @file    menu.c
 * @brief   Rendering engine for all UI menu pages of the curing furnace controller.
 *
 * This module implements the complete on-device graphical user interface built
 * on top of the U8g2 library. Each function in this file corresponds to a
 * dedicated screen or sub-menu in the system’s hierarchical UI state machine.
 *
 * The menu system is structured around:
 *   - **MenuCtx** : holds the current menu state and navigation cursor
 *   - **CookingCtx** : exposes live cooking-process data for visualization
 *   - **Sens** : provides real-time sensor feedback (temperatures, time, etc.)
 *   - **setting_t** : exposes user-configurable parameters (PI gains, offsets…)
 *
 * Major UI categories implemented in this file:
 *
 *   • **Home & Program Selection Screens**
 *       - High-level navigation menus, program select pages,
 *         advanced temperature-control setup screens.
 *
 *   • **Cooking Screens**
 *       - Live cooking view, temperature adjustments, power tuning,
 *         duration editing, and process-generation previews.
 *
 *   • **FastStart & Advanced TempControl Editors**
 *       - Parameter editors for multi-stage temperature profiles
 *         (T1–T6 stage editors, rise-time editors, generator page, etc.)
 *
 *   • **Settings Screens**
 *       - System configuration interfaces (PI tuning, time/date settings,
 *         temp-offset editing).
 *
 * Each UI function in this file is strictly *render-only*:
 * they do not modify system state or logic, but instead draw the relevant
 * menu page based on the input context structures.
 *
 * Navigation behavior (state transitions, cursor updates, encoder inputs)
 * is performed externally by the menu/state machine handler and passed into
 * these rendering functions as pure data.
 *
 * @note This module requires:
 *       - U8g2 graphics library
 *       - Global font configuration
 *       - Properly initialized Display_Handler before use
 *
 * @date    Dec 3, 2025
 * @author  Eren Egdemir
 */

#include "menu.h"

#define u8g_logo_width 		40
#define u8g_logo_height 	46

#define u8g_fan_width		16
#define u8g_fan_height		16

#define u8g_flame_width		16
#define u8g_flame_height	16

#define u8g_setupLogo_width		125
#define u8g_setupLogo_height	40

#define TRUE 	1

uint8_t generate_flag = FALSE;

float Loading_Coefficent;
float Loading_Coefficent_R;
uint32_t total_duration;
uint32_t Loading;
uint32_t Loading_P;

static unsigned char u8g_logo_bits[] = {

		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x60,
		0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00,
		0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x18, 0x00, 0x00, 0x60, 0x80, 0xef,
		0x03, 0x00, 0xc0, 0x60, 0x00, 0x0c, 0x00, 0x80, 0x19, 0x00, 0x30, 0x00, 0x00, 0x07, 0x00, 0x60,
		0x00, 0x00, 0x06, 0x00, 0xc0, 0x00, 0x00, 0x0c, 0x00, 0x80, 0x01, 0x00, 0x18, 0xfe, 0x00, 0x01,
		0x00, 0xa0, 0x81, 0x01, 0x02, 0xc0, 0xc0, 0x00, 0x06, 0x02, 0x40, 0x80, 0x00, 0x04, 0x04, 0x40,
		0x00, 0x10, 0x08, 0x04, 0x20, 0x10, 0x7c, 0x08, 0x04, 0x20, 0x10, 0xc2, 0x10, 0x04, 0x20, 0x10,
		0x83, 0x10, 0x0c, 0x20, 0x10, 0x81, 0x10, 0x08, 0x20, 0x10, 0x82, 0x10, 0x04, 0x20, 0x10, 0xc2,
		0x10, 0x04, 0x60, 0x10, 0x7c, 0x08, 0x04, 0x40, 0x20, 0x00, 0x00, 0x04, 0x40, 0x40, 0x00, 0x02,
		0x06, 0x80, 0xc0, 0x00, 0x06, 0x02, 0x80, 0x00, 0x83, 0x09, 0x00, 0x00, 0x01, 0x7c, 0x30, 0x00,
		0x00, 0x03, 0x00, 0x60, 0x00, 0x00, 0x06, 0x00, 0xc0, 0x00, 0x00, 0x0c, 0x00, 0xa0, 0x01, 0x00,
		0x10, 0x00, 0x18, 0x03, 0x00, 0xe0, 0x00, 0x06, 0x06, 0x00, 0x00, 0xff, 0x01, 0x0c, 0x00, 0x00,
		0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00,
		0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00,
		0x03, 0x00, 0x00, 0x00, 0x00, 0x00
};


static unsigned char u8g_fan_bits[] = {
		0x00, 0x00, 0xc0, 0x07, 0xc0, 0x07, 0xc0, 0x03, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0x40, 0x02,
		0xe2, 0x1f, 0xfe, 0x7f, 0x7e, 0x7c, 0x3e, 0x78, 0x1c, 0x38, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00
};

static unsigned char u8g_flame_bits[] = {
		0x80, 0x00, 0x80, 0x01, 0xc0, 0x03, 0xe0, 0x03, 0xe0, 0x13, 0xf0, 0x13, 0xf8, 0x3f, 0xf8, 0x3f,
		0xfc, 0x3f, 0xfc, 0x3e, 0x7c, 0x3c, 0x1c, 0x38, 0x1c, 0x38, 0x1c, 0x18, 0x38, 0x18, 0x20, 0x00
};

static unsigned char u8g_setupLogo_bits[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86, 0x03, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xfe, 0x00, 0x22, 0x00, 0x00, 0x00, 0x38, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x10, 0x00, 0x22, 0x00, 0x00, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x10, 0x42, 0x22, 0xc6, 0xc3, 0x01, 0x40, 0x60, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x10, 0x42, 0x22, 0x21, 0x22, 0x02, 0x80, 0x1c, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x10, 0x42, 0xa2, 0x00, 0x22, 0x00, 0x04, 0x03, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x10, 0x42, 0xe2, 0xc1, 0xc3, 0x01, 0x02, 0x02, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x10, 0x42, 0x22, 0x21, 0x02, 0x02, 0x02, 0x00, 0x10, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x10, 0x62, 0x22, 0x23, 0x22, 0x02, 0x82, 0x98, 0x11, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x10, 0x3c, 0x22, 0xc6, 0xc1, 0x01, 0x82, 0x08, 0x21, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x08, 0x22, 0x08, 0x7c, 0x00, 0x00, 0x08, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x08, 0x22, 0x08, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x08, 0x21, 0x08, 0x02, 0xe1, 0x70, 0x88, 0x87, 0x07,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0xf0, 0x11, 0x04, 0x02, 0x11, 0x89, 0x48, 0x48, 0x08,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x00, 0x00, 0x04, 0x02, 0x09, 0x09, 0x48, 0x48, 0x08,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x04, 0x04, 0x02, 0xf9, 0x71, 0x48, 0x48, 0x08,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x06, 0x1c, 0x00, 0x02, 0x09, 0x80, 0x48, 0x48, 0x08,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0xfc, 0x33, 0x00, 0x82, 0x10, 0x89, 0x88, 0x4f, 0x08,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x60, 0x00, 0x7c, 0xe0, 0x70, 0x08, 0x48, 0x08,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x40, 0x08, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x80, 0x0f, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x30, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x1c, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x03, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void menu_send_logo(u8g2_t *display, uint8_t x, uint8_t y){

	u8g2_DrawXBMP(display, x, y, u8g_logo_width, u8g_logo_height, u8g_logo_bits);

}

void menu_send_fan(u8g2_t *display, uint8_t x, uint8_t y){

	u8g2_DrawXBMP(display, x, y, u8g_fan_width, u8g_fan_height, u8g_fan_bits);
}

void menu_send_flame(u8g2_t *display, uint8_t x, uint8_t y){
	u8g2_DrawXBMP(display, x, y, u8g_flame_width, u8g_flame_height, u8g_flame_bits);
}

void menu_send_setupLogo(u8g2_t *display, uint8_t x, uint8_t y){
	u8g2_DrawXBMP(display, x, y, u8g_setupLogo_width, u8g_setupLogo_height, u8g_setupLogo_bits);
}


void menu_GenerateCookingProcess(u8g2_t *display, MenuCtx *m, CookingCtx *c, Sens t) {

	total_duration = c->stage1->totalDuration + c->stage2->totalDuration + c->stage3->totalDuration
			+ c->stage4->totalDuration + c->stage5->totalDuration + c->stage6->totalDuration;
	c->faststart->totalDuration = c->faststart->changePeriod + c->faststart->duration + c->faststart->fallTime;

	Loading_Coefficent = 100 / (float) total_duration;
	Loading_Coefficent_R = 100 / (float) c->faststart->totalDuration;
	u8g2_ClearBuffer(display);
	u8g2_DrawFrame(display, 14, 40, 100, 8);

	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 10, 15, "Generating Cooking");
	u8g2_DrawStr(display, 43, 25, "Process");

	if (c->stage1->state == TRUE) {
		c->stage1->totalDuration = c->stage1->changePeriod + c->stage1->duration;
		c->stage1->tempPerMin = (float)(c->stage1->temperature - t.sens1_temp) / (float)c->stage1->changePeriod;
		c->stage1->remTime = c->stage1->totalDuration;

		c->stage1->tempForMin[0] = (t.sens4_temp + t.sens5_temp)/2;

		for (int i = 1; i < c->stage1->changePeriod; i++) {

			c->stage1->tempForMin[i] = c->stage1->tempForMin[i-1] + c->stage1->tempPerMin;

			if (c->stage1->tempForMin[i] == c->stage1->temperature) {

				c->stage1->tempForMin[i] = c->stage1->temperature;

			}
			Loading = (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;

		for (int i = c->stage1->changePeriod - 1; i < c->stage1->totalDuration; i++) {

			c->stage1->tempForMin[i] = c->stage1->temperature;
			Loading = Loading_P + (i * Loading_Coefficent);
			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;
	}

	if (c->stage2->state == TRUE) {
		c->stage2->totalDuration = c->stage2->changePeriod + c->stage2->duration;
		c->stage2->tempPerMin = (float)(c->stage2->temperature - c->stage1->temperature) / (float)c->stage2->changePeriod;
		c->stage2->remTime = c->stage2->totalDuration;

		c->stage2->tempForMin[0] = c->stage1->temperature;

		for (int i = 1; i < c->stage2->changePeriod; i++) {

			c->stage2->tempForMin[i] = c->stage2->tempForMin[i-1] + c->stage2->tempPerMin;

			if (c->stage2->tempForMin[i] == c->stage2->temperature) {

				c->stage2->tempForMin[i] = c->stage2->temperature;
			}
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;

		for (int i = c->stage2->changePeriod - 1; i < c->stage2->totalDuration; i++) {

			c->stage2->tempForMin[i] = c->stage2->temperature;
			Loading = Loading_P + (i * Loading_Coefficent);;

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;
	}

	if (c->stage3->state == TRUE) {
		c->stage3->totalDuration = c->stage3->changePeriod + c->stage3->duration;
		c->stage3->tempPerMin = (float)(c->stage3->temperature - c->stage2->temperature) / (float)c->stage3->changePeriod;
		c->stage3->remTime = c->stage3->totalDuration;

		c->stage3->tempForMin[0] = c->stage2->temperature;

		for (int i = 1; i < c->stage3->changePeriod; i++) {

			c->stage3->tempForMin[i] = c->stage3->tempForMin[i-1] + c->stage3->tempPerMin;

			if (c->stage3->tempForMin[i] == c->stage3->temperature) {

				c->stage3->tempForMin[i] = c->stage3->temperature;
			}
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;

		for (int i = c->stage3->changePeriod - 1; i < c->stage3->totalDuration; i++) {

			c->stage3->tempForMin[i] = c->stage3->temperature;
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);

		}
		Loading_P = Loading;
	}

	if (c->stage4->state == TRUE) {
		c->stage4->totalDuration = c->stage4->changePeriod + c->stage4->duration;
		c->stage4->tempPerMin = (float)(c->stage4->temperature - c->stage3->temperature) / (float)c->stage4->changePeriod;
		c->stage4->remTime = c->stage4->totalDuration;

		c->stage4->tempForMin[0] = c->stage3->temperature;

		for (int i = 1; i < c->stage4->changePeriod; i++) {

			c->stage4->tempForMin[i] = c->stage4->tempForMin[i-1] +  c->stage4->tempPerMin;

			if (c->stage4->tempForMin[i] == c->stage4->temperature) {

				c->stage4->tempForMin[i] = c->stage4->temperature;
			}
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;

		for (int i = c->stage4->changePeriod - 1; i < c->stage4->totalDuration; i++) {

			c->stage4->tempForMin[i] = c->stage4->temperature;
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;
	}

	if (c->stage5->state == TRUE) {
		c->stage5->totalDuration = c->stage5->changePeriod + c->stage1->duration;
		c->stage5->tempPerMin = (float)(c->stage5->temperature - c->stage4->temperature) / (float)c->stage5->changePeriod;
		c->stage5->remTime = c->stage5->totalDuration;

		c->stage5->tempForMin[0] = c->stage4->temperature;

		for (int i = 1; i < c->stage5->changePeriod; i++) {

			c->stage5->tempForMin[i] = c->stage5->tempForMin[i-1] + c->stage5->tempPerMin;

			if (c->stage5->tempForMin[i] == c->stage5->temperature) {

				c->stage5->tempForMin[i] = c->stage5->temperature;
			}
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;

		for (int i = c->stage5->changePeriod - 1; i < c->stage5->totalDuration; i++) {

			c->stage5->tempForMin[i] = c->stage5->temperature;
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;


	}

	if (c->stage6->state == TRUE) {
		c->stage6->totalDuration = c->stage6->changePeriod + c->stage6->duration;
		c->stage6->tempPerMin = (float)(c->stage6->temperature - c->stage5->temperature) / (float)c->stage6->changePeriod;
		c->stage6->remTime = c->stage6->totalDuration;

		c->stage6->tempForMin[0] = c->stage5->temperature;

		for (int i = 1; i < c->stage6->changePeriod; i++) {

			c->stage6->tempForMin[i] = c->stage6->tempForMin[i-1] + c->stage6->tempPerMin;

			if (c->stage6->tempForMin[i] == c->stage6->temperature) {
				c->stage6->tempForMin[i] = c->stage6->temperature;
			}
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);

		}
		for (int i = c->stage6->changePeriod - 1; i < c->stage6->totalDuration; i++) {

			c->stage6->tempForMin[i] = c->stage6->temperature;
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);

		}
	}

	if (c->faststart->state == TRUE) {
		c->faststart->totalDuration = c->faststart->changePeriod + c->faststart->duration + c->faststart->fallTime;
		c->faststart->tempPerMin = (float)(c->faststart->temperature - t.sens1_temp) / (float)c->faststart->changePeriod;
		c->faststart->tempPerMinDown = (float)(t.sens1_temp - c->faststart->temperature) / (float)c->faststart->fallTime;
		c->faststart->remTime = c->faststart->totalDuration;

		c->faststart->tempForMin[0] = (t.sens4_temp + t.sens5_temp)/2;

		for (int i = 1; i < c->faststart->changePeriod; i++) {

			c->faststart->tempForMin[i] = c->faststart->tempForMin[i-1] + c->faststart->tempPerMin;

			if (c->faststart->tempForMin[i] == c->faststart->temperature) {
				c->faststart->tempForMin[i] = c->faststart->temperature;
			}
			Loading = Loading_P + (i * Loading_Coefficent_R);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);

		}
		for (int i = c->faststart->changePeriod - 1; i < c->faststart->changePeriod + c->faststart->duration; i++) {

			c->faststart->tempForMin[i] = c->faststart->temperature;
			Loading = Loading_P + (i * Loading_Coefficent_R);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);

		}
		for (int i = c->faststart->changePeriod + c->faststart->duration - 1; i < c->faststart->totalDuration; i++) {

			c->faststart->tempForMin[i] = c->faststart->tempForMin[i-1] + c->faststart->tempPerMinDown;

			if (c->faststart->tempForMin[i] == c->faststart->temperature) {
				c->faststart->tempForMin[i] = c->faststart->temperature;
			}
			Loading = Loading_P + (i * Loading_Coefficent_R);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);

		}


	}

	if(c->flag == 0){
		if(c->faststart->state == TRUE){
			m->flag = 3;
			c->flag = FASTSTART;
		}else m->flag = 11;

		m->counter = 0;
		generate_flag = TRUE;

	}else{
		m->flag = 3;
		m->counter = 0;
	}





}

void menu_cooking_run(u8g2_t *display, CookingCtx *c, Sens t, setting_t setting){

	uint16_t heater_pwm = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

	uint32_t remaningTime;
	u8g2_ClearBuffer(display);
	menu_send_fan(display, 113, 2);
	menu_send_flame(display, 97, 0);
	u8g2_DrawFrame(display, 101, 17, 8, 40);
	uint8_t map = 40 - (heater_pwm * 40) / 100;
	u8g2_DrawBox(display, 101, 17 + map, 8, 40 - map);
	map = 0;
	map = 40 - (0 * 40) / 100; //cooling state
	u8g2_DrawFrame(display, 117, 17, 8, 40);
	u8g2_DrawBox(display, 117, 17 + map, 8, 40 - map);
	map = 0;
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	menu_send_int_3(display, 98, 63, heater_pwm);
	menu_send_int_1(display, 119, 63, 0); //cooling state
	u8g2_DrawStr(display, 77, 6, "Max");
	u8g2_DrawStr(display, 77, 12, "PWR");
	u8g2_DrawStr(display, 74, 18, "%");
	menu_send_int_3(display, 80, 18, setting.max_pwr);
	menu_send_date(display, 37, 63, t.date, t.month, t.year);
	menu_send_time(display, 0, 63, t.hour, t.minute);

	switch (c->flag){
	case STAGE_1:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 1);
		menu_send_int_3(display, 42, 9, c->stage1->temperature);
		remaningTime = c->stage1->remTime + c->stage2->totalDuration + c->stage3->totalDuration + c->stage4->totalDuration +
				c->stage5->totalDuration + c->stage6->totalDuration;
		break;
	case STAGE_2:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 2);
		menu_send_int_3(display, 42, 9, c->stage2->temperature);
		remaningTime = c->stage2->remTime + c->stage3->totalDuration + c->stage4->totalDuration +
				c->stage5->totalDuration + c->stage6->totalDuration;

		break;
	case STAGE_3:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 3);
		menu_send_int_3(display, 42, 9, c->stage3->temperature);
		remaningTime = c->stage3->remTime + c->stage4->totalDuration + c->stage5->totalDuration + c->stage6->totalDuration;
		break;
	case STAGE_4:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 4);
		menu_send_int_3(display, 42, 9, c->stage4->temperature);
		remaningTime = c->stage4->remTime + c->stage5->totalDuration + c->stage6->totalDuration;
		break;
	case STAGE_5:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 5);
		menu_send_int_3(display, 42, 9, c->stage5->temperature);
		remaningTime = c->stage5->remTime + c->stage6->totalDuration;
		break;
	case STAGE_6:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 6);
		menu_send_int_3(display, 42, 9, c->stage6->temperature);
		remaningTime = c->stage6->remTime;
		break;
	case FASTSTART:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Target");
		menu_send_int_3(display, 42, 9, c->faststart->temperature);
		remaningTime = c->faststart->remTime;
		break;
	default:
		break;

	}

	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 40, 30, "Duration");
	menu_send_int_3(display, 45, 42, remaningTime);
	remaningTime = 0;
	u8g2_DrawStr(display, 65, 42, "MIN");


	u8g2_DrawStr(display, 38, 9, ":");
	u8g2_DrawStr(display, 62, 9, "C");
	u8g2_DrawLine(display, 0, 12, 69, 12);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 3, 19 + 4, "T1");
	u8g2_DrawStr(display, 12, 19 + 4, ":");
	menu_send_int_3(display, 15, 19 + 4, t.sens1_temp);
	u8g2_DrawStr(display, 31, 23, "C");
	u8g2_DrawStr(display, 3, 25 + 4, "T2:");
	menu_send_int_3(display, 15, 25 + 4, t.sens2_temp);
	u8g2_DrawStr(display, 31, 29, "C");
	u8g2_DrawStr(display, 3, 31 + 4, "T3:");
	menu_send_int_3(display, 15, 31 + 4, t.sens3_temp);
	u8g2_DrawStr(display, 31, 35, "C");
	u8g2_DrawStr(display, 3, 37 + 4, "T4:");
	menu_send_int_3(display, 15, 37 + 4, t.sens4_temp);
	u8g2_DrawStr(display, 31, 41, "C");
	u8g2_DrawStr(display, 3, 43 + 4, "T5:");
	menu_send_int_3(display, 15, 43 + 4, t.sens5_temp);
	u8g2_DrawStr(display, 31, 47, "C");

	u8g2_SendBuffer(display);



}

void menu_cooking_run_TempChange(u8g2_t *display, MenuCtx m, CookingCtx *c, Sens t, setting_t setting){
	uint16_t heater_pwm = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

	uint32_t remaningTime;
	u8g2_ClearBuffer(display);
	menu_send_fan(display, 113, 2);
	menu_send_flame(display, 97, 0);
	u8g2_DrawFrame(display, 101, 17, 8, 40);
	uint8_t map = 40 - (heater_pwm * 40) / 100;
	u8g2_DrawBox(display, 101, 17 + map, 8, 40 - map);
	map = 0;
	map = 40 - (0 * 40) / 100; //cooling state
	u8g2_DrawFrame(display, 117, 17, 8, 40);
	u8g2_DrawBox(display, 117, 17 + map, 8, 40 - map);
	map = 0;
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	menu_send_int_3(display, 98, 63, heater_pwm);
	menu_send_int_1(display, 119, 63, 0); //cooling state
	u8g2_DrawStr(display, 77, 6, "Max");
	u8g2_DrawStr(display, 77, 12, "PWR");
	u8g2_DrawStr(display, 74, 18, "%");
	menu_send_int_3(display, 80, 18, setting.max_pwr);
	menu_send_date(display, 37, 63, t.date, t.month, t.year);
	menu_send_time(display, 0, 63, t.hour, t.minute);

	switch (c->flag){
	case STAGE_1:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 1);
		menu_send_int_3(display, 42, 9, m.counter);
		remaningTime = c->stage1->remTime + c->stage2->totalDuration + c->stage3->totalDuration + c->stage4->totalDuration +
				c->stage5->totalDuration + c->stage6->totalDuration;
		break;
	case STAGE_2:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 2);
		menu_send_int_3(display, 42, 9, m.counter);
		remaningTime = c->stage2->remTime + c->stage3->totalDuration + c->stage4->totalDuration +
				c->stage5->totalDuration + c->stage6->totalDuration;

		break;
	case STAGE_3:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 3);
		menu_send_int_3(display, 42, 9, m.counter);
		remaningTime = c->stage3->remTime + c->stage4->totalDuration + c->stage5->totalDuration + c->stage6->totalDuration;
		break;
	case STAGE_4:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 4);
		menu_send_int_3(display, 42, 9, m.counter);
		remaningTime = c->stage4->remTime + c->stage5->totalDuration + c->stage6->totalDuration;
		break;
	case STAGE_5:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 5);
		menu_send_int_3(display, 42, 9, m.counter);
		remaningTime = c->stage5->remTime + c->stage6->totalDuration;
		break;
	case STAGE_6:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 6);
		menu_send_int_3(display, 42, 9, m.counter);
		remaningTime = c->stage6->remTime;
		break;
	case FASTSTART:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Target");
		menu_send_int_3(display, 42, 9, m.counter);
		remaningTime = c->faststart->remTime;
		break;
	default:
		break;

	}

	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 40, 30, "Duration");
	menu_send_int_3(display, 45, 42, remaningTime);
	remaningTime = 0;
	u8g2_DrawStr(display, 65, 42, "MIN");


	u8g2_DrawStr(display, 38, 9, ":");
	u8g2_DrawStr(display, 62, 9, "C");
	u8g2_DrawLine(display, 0, 12, 69, 12);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 3, 19 + 4, "T1");
	u8g2_DrawStr(display, 12, 19 + 4, ":");
	menu_send_int_3(display, 15, 19 + 4, t.sens1_temp);
	u8g2_DrawStr(display, 31, 23, "C");
	u8g2_DrawStr(display, 3, 25 + 4, "T2:");
	menu_send_int_3(display, 15, 25 + 4, t.sens2_temp);
	u8g2_DrawStr(display, 31, 29, "C");
	u8g2_DrawStr(display, 3, 31 + 4, "T3:");
	menu_send_int_3(display, 15, 31 + 4, t.sens3_temp);
	u8g2_DrawStr(display, 31, 35, "C");
	u8g2_DrawStr(display, 3, 37 + 4, "T4:");
	menu_send_int_3(display, 15, 37 + 4, t.sens4_temp);
	u8g2_DrawStr(display, 31, 41, "C");
	u8g2_DrawStr(display, 3, 43 + 4, "T5:");
	menu_send_int_3(display, 15, 43 + 4, t.sens5_temp);
	u8g2_DrawStr(display, 31, 47, "C");

	u8g2_SendBuffer(display);



}

void menu_cooking_run_Duration(u8g2_t *display, MenuCtx m, CookingCtx *c, Sens t, setting_t setting){
	uint16_t heater_pwm = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

	uint32_t remaningTime;
	u8g2_ClearBuffer(display);
	menu_send_fan(display, 113, 2);
	menu_send_flame(display, 97, 0);
	u8g2_DrawFrame(display, 101, 17, 8, 40);
	uint8_t map = 40 - (heater_pwm * 40) / 100;
	u8g2_DrawBox(display, 101, 17 + map, 8, 40 - map);
	map = 0;
	map = 40 - (0 * 40) / 100; //cooling state
	u8g2_DrawFrame(display, 117, 17, 8, 40);
	u8g2_DrawBox(display, 117, 17 + map, 8, 40 - map);
	map = 0;
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	menu_send_int_3(display, 98, 63, heater_pwm);
	menu_send_int_1(display, 119, 63, 0); //cooling state
	u8g2_DrawStr(display, 77, 6, "Max");
	u8g2_DrawStr(display, 77, 12, "PWR");
	u8g2_DrawStr(display, 74, 18, "%");
	menu_send_int_3(display, 80, 18, setting.max_pwr);
	menu_send_date(display, 37, 63, t.date, t.month, t.year);
	menu_send_time(display, 0, 63, t.hour, t.minute);

	switch (c->flag){
	case STAGE_1:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 1);
		menu_send_int_3(display, 42, 9, c->stage1->temperature);
		remaningTime = m.counter + c->stage2->totalDuration + c->stage3->totalDuration + c->stage4->totalDuration +
				c->stage5->totalDuration + c->stage6->totalDuration;
		break;
	case STAGE_2:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 2);
		menu_send_int_3(display, 42, 9, c->stage2->temperature);
		remaningTime = m.counter + c->stage3->totalDuration + c->stage4->totalDuration +
				c->stage5->totalDuration + c->stage6->totalDuration;

		break;
	case STAGE_3:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 3);
		menu_send_int_3(display, 42, 9, c->stage3->temperature);
		remaningTime = m.counter + c->stage4->totalDuration + c->stage5->totalDuration + c->stage6->totalDuration;
		break;
	case STAGE_4:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 4);
		menu_send_int_3(display, 42, 9, c->stage4->temperature);
		remaningTime = m.counter + c->stage5->totalDuration + c->stage6->totalDuration;
		break;
	case STAGE_5:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 5);
		menu_send_int_3(display, 42, 9, c->stage5->temperature);
		remaningTime = m.counter + c->stage6->totalDuration;
		break;
	case STAGE_6:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 6);
		menu_send_int_3(display, 42, 9, c->stage6->temperature);
		remaningTime = m.counter;
		break;
	case FASTSTART:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Target");
		menu_send_int_3(display, 42, 9, c->faststart->temperature);
		remaningTime = m.counter;
		break;
	default:
		break;

	}

	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 40, 30, "Duration");
	menu_send_int_3(display, 45, 42, remaningTime);
	remaningTime = 0;
	u8g2_DrawStr(display, 65, 42, "MIN");


	u8g2_DrawStr(display, 38, 9, ":");
	u8g2_DrawStr(display, 62, 9, "C");
	u8g2_DrawLine(display, 0, 12, 69, 12);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 3, 19 + 4, "T1");
	u8g2_DrawStr(display, 12, 19 + 4, ":");
	menu_send_int_3(display, 15, 19 + 4, t.sens1_temp);
	u8g2_DrawStr(display, 31, 23, "C");
	u8g2_DrawStr(display, 3, 25 + 4, "T2:");
	menu_send_int_3(display, 15, 25 + 4, t.sens2_temp);
	u8g2_DrawStr(display, 31, 29, "C");
	u8g2_DrawStr(display, 3, 31 + 4, "T3:");
	menu_send_int_3(display, 15, 31 + 4, t.sens3_temp);
	u8g2_DrawStr(display, 31, 35, "C");
	u8g2_DrawStr(display, 3, 37 + 4, "T4:");
	menu_send_int_3(display, 15, 37 + 4, t.sens4_temp);
	u8g2_DrawStr(display, 31, 41, "C");
	u8g2_DrawStr(display, 3, 43 + 4, "T5:");
	menu_send_int_3(display, 15, 43 + 4, t.sens5_temp);
	u8g2_DrawStr(display, 31, 47, "C");

	u8g2_SendBuffer(display);



}

void menu_cooking_PowerChange_run(u8g2_t *display, MenuCtx m, CookingCtx *c, Sens t){

	uint16_t heater_pwm = (c->r1_duty + c->r2_duty + c->r3_duty) / 3;

	uint32_t remaningTime;

	u8g2_ClearBuffer(display);
	menu_send_fan(display, 113, 2);
	menu_send_flame(display, 97, 0);
	u8g2_DrawFrame(display, 101, 17, 8, 40);
	uint8_t map = 40 - (heater_pwm * 40) / 100;
	u8g2_DrawBox(display, 101, 17 + map, 8, 40 - map);
	map = 0;
	map = 40 - (0 * 40) / 100; //cooling state
	u8g2_DrawFrame(display, 117, 17, 8, 40);
	u8g2_DrawBox(display, 117, 17 + map, 8, 40 - map);
	map = 0;
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	menu_send_int_3(display, 98, 63, heater_pwm);
	menu_send_int_1(display, 119, 63, 0); //coolingstate
	u8g2_DrawStr(display, 77, 6, "Max");
	u8g2_DrawStr(display, 77, 12, "PWR");
	u8g2_DrawStr(display, 74, 18, "%");
	menu_send_int_3(display, 80, 18, m.counter);
	menu_send_date(display, 37, 63, t.date, t.month, t.year);
	menu_send_time(display, 0, 63, t.hour, t.minute);

	switch (c->flag){
	case STAGE_1:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 1);
		menu_send_int_3(display, 42, 9, c->stage1->temperature);
		remaningTime = c->stage1->remTime + c->stage2->totalDuration + c->stage3->totalDuration + c->stage4->totalDuration +
				c->stage5->totalDuration + c->stage6->totalDuration;
		break;
	case STAGE_2:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 2);
		menu_send_int_3(display, 42, 9, c->stage2->temperature);
		remaningTime = c->stage2->remTime + c->stage3->totalDuration + c->stage4->totalDuration +
				c->stage5->totalDuration + c->stage6->totalDuration;

		break;
	case STAGE_3:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 3);
		menu_send_int_3(display, 42, 9, c->stage3->temperature);
		remaningTime = c->stage3->remTime + c->stage4->totalDuration + c->stage5->totalDuration + c->stage6->totalDuration;
		break;
	case STAGE_4:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 4);
		menu_send_int_3(display, 42, 9, c->stage4->temperature);
		remaningTime = c->stage4->remTime + c->stage5->totalDuration + c->stage6->totalDuration;
		break;
	case STAGE_5:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 5);
		menu_send_int_3(display, 42, 9, c->stage5->temperature);
		remaningTime = c->stage5->remTime + c->stage6->totalDuration;
		break;
	case STAGE_6:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 6);
		menu_send_int_3(display, 42, 9, c->stage6->temperature);
		remaningTime = c->stage6->remTime;
		break;
	case FASTSTART:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Target");
		menu_send_int_3(display, 42, 9, c->faststart->temperature);
		remaningTime = c->faststart->remTime;
		break;
	default:
		break;

	}

	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 40, 30, "Duration");
	menu_send_int_3(display, 45, 42, remaningTime);
	remaningTime = 0;
	u8g2_DrawStr(display, 65, 42, "MIN");


	u8g2_DrawStr(display, 38, 9, ":");
	u8g2_DrawStr(display, 62, 9, "C");
	u8g2_DrawLine(display, 0, 12, 69, 12);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 3, 19 + 4, "T1");
	u8g2_DrawStr(display, 12, 19 + 4, ":");
	menu_send_int_3(display, 15, 19 + 4, t.sens1_temp);
	u8g2_DrawStr(display, 31, 23, "C");
	u8g2_DrawStr(display, 3, 25 + 4, "T2:");
	menu_send_int_3(display, 15, 25 + 4, t.sens2_temp);
	u8g2_DrawStr(display, 31, 29, "C");
	u8g2_DrawStr(display, 3, 31 + 4, "T3:");
	menu_send_int_3(display, 15, 31 + 4, t.sens3_temp);
	u8g2_DrawStr(display, 31, 35, "C");
	u8g2_DrawStr(display, 3, 37 + 4, "T4:");
	menu_send_int_3(display, 15, 37 + 4, t.sens4_temp);
	u8g2_DrawStr(display, 31, 41, "C");
	u8g2_DrawStr(display, 3, 43 + 4, "T5:");
	menu_send_int_3(display, 15, 43 + 4, t.sens5_temp);
	u8g2_DrawStr(display, 31, 47, "C");

	u8g2_SendBuffer(display);



}



void menu_send_int_1(u8g2_t *display, uint8_t x, uint8_t y, int val)
{

	char buf[32];

	snprintf(buf, sizeof(buf), "%d", val);

//	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, x, y, buf);


}
void menu_send_int_2(u8g2_t *display, uint8_t x, uint8_t y, int val)
{

	char buf[32];

	snprintf(buf, sizeof(buf), "%02d", val);


	u8g2_DrawStr(display, x, y, buf);


}
void menu_send_int_3(u8g2_t *display, uint8_t x, uint8_t y, int val)
{

	char buf[32];

	snprintf(buf, sizeof(buf), "%03d", val);


	u8g2_DrawStr(display, x, y, buf);


}
void menu_send_int_4(u8g2_t *display, uint8_t x, uint8_t y, int val)
{

	char buf[32];

	snprintf(buf, sizeof(buf), "%04d", (unsigned)val);


	u8g2_DrawStr(display, x, y, buf);


}

void menu_send_time(u8g2_t *display, uint8_t x, uint8_t y, int hh, int mm){

	char buf[32];

	snprintf(buf, sizeof(buf), "%02d:%02d", hh, mm);

//	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, x, y, buf);

}

void menu_send_date(u8g2_t *display, uint8_t x, uint8_t y, int dd, int mm, int yy){

	char buf[32];

	snprintf(buf, sizeof(buf), "%02d.%02d.%04d", dd, mm, yy);

//	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, x, y, buf);


}

void menu_settings_run(u8g2_t *display, MenuCtx m){

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 40, 20 - 13, "Settings");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 32, 32 - 13, "Pid Coefficents");
	u8g2_DrawStr(display, 36, 42 - 13, "Temp Offsets");
	u8g2_DrawStr(display, 36, 52 - 13, "Time Settings");
	u8g2_DrawStr(display, 48, 62 - 13, "Profiles");
	u8g2_DrawRFrame(display, 75 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 75, 62 - 1, "Exit");
	u8g2_DrawRFrame(display, 32 - 5, 59 - 6, 27, 11, 5);
	u8g2_DrawStr(display, 31, 62 - 1, "Save");


	switch (m.counter) {

	case 0:
		u8g2_DrawBox(display, 30, 25 - 13, 66, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 32, 32 - 13, "Pid Coefficents");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);

		break;
	case 1:
		u8g2_DrawBox(display, 36 - 2, 42 - 7 - 13, 59, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 36, 42 - 13, "Temp Offsets");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);

		break;
	case 2:
		u8g2_DrawBox(display, 36-2, 52 - 7 - 13, 59, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 36, 52 - 13, "Time Settings");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 3:
		u8g2_DrawBox(display, 46, 62 - 7 - 13, 35, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 48, 62 - 13, "Profiles");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 4:
		u8g2_DrawRBox(display, 32 - 5, 59 - 6, 27, 11, 5);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 31, 62 - 1, "Save");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 5:
		u8g2_DrawRBox(display, 75 - 5, 59 - 6, 23, 11, 5);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 75, 62 - 1, "Exit");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);

		break;
	default:
		break;

	}

}



void menu_settings_Pi_run(u8g2_t *display, MenuCtx m, setting_t *s){

	setting_t ram = *s;

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 40, 20 - 13, "Settings");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 30+3, 19, "Kp");
	u8g2_DrawStr(display, 64+3, 19, ":");
	menu_send_int_3(display, 68+3, 19, (uint32_t)(ram.kp * 100));
	u8g2_DrawStr(display, 85+3, 19, "%");
	u8g2_DrawStr(display, 30+3, 29, "Ki");
	u8g2_DrawStr(display, 64+3, 29, ":");
	menu_send_int_3(display, 68+3, 29,(uint32_t)(ram.ki * 100));
	u8g2_DrawStr(display, 85+3, 29, "%");
	u8g2_DrawStr(display, 30+3, 39, "Slew");
	u8g2_DrawStr(display, 64+3, 39, ":");
	menu_send_int_3(display, 68+3, 39, (uint32_t)(ram.slew * 100));
	u8g2_DrawStr(display, 85+3, 39, "%");
	u8g2_DrawStr(display, 30+3, 49, "Max Pwr:");
	menu_send_int_3(display, 68+3, 49, ram.max_pwr);
	u8g2_DrawStr(display, 85+3, 49, "%");
	u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 55, 62 - 1, "Exit");

	switch(m.counter){
	case 0:
		u8g2_DrawBox(display, 30, 12, 65, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 30+3, 19, "Kp");
		u8g2_DrawStr(display, 64+3, 19, ":");
		menu_send_int_3(display, 68+3, 19, (uint32_t)(ram.kp * 100));
		u8g2_DrawStr(display, 85+3, 19, "%");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 1:
		u8g2_DrawBox(display, 30, 22, 65, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 30+3, 29, "Ki");
		u8g2_DrawStr(display, 64+3, 29, ":");
		menu_send_int_3(display, 68+3, 29, (uint32_t)(ram.ki * 100));
		u8g2_DrawStr(display, 85+3, 29, "%");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 2:
		u8g2_DrawBox(display, 30, 32, 65, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 30+3, 39, "Slew");
		u8g2_DrawStr(display, 64+3, 39, ":");
		menu_send_int_3(display, 68+3, 39, (uint32_t)(ram.slew * 100));
		u8g2_DrawStr(display, 85+3, 39, "%");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 3:
		u8g2_DrawBox(display, 30, 42, 65, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 30+3, 49, "Max Pwr:");
		menu_send_int_3(display, 68+3, 49, ram.max_pwr);
		u8g2_DrawStr(display, 85+3, 49, "%");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 4:
		u8g2_DrawRBox(display, 55 - 5, 59 - 6, 23, 11, 5);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 55, 62 - 1, "Exit");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	default:
		break;
	}


}
void menu_settings_PiKp_run(u8g2_t *display, MenuCtx m, setting_t *s)
{
	setting_t ram = *s;

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 40, 20 - 13, "Settings");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 30, 12, 65, 10);
//	u8g2_SetDrawColor(display, 0);
	u8g2_DrawStr(display, 30+3, 19, "Kp");
	u8g2_DrawStr(display, 64+3, 19, ":");
	menu_send_int_3(display, 68+3, 19, m.counter);
	u8g2_DrawStr(display, 85+3, 19, "%");
//	u8g2_SetDrawColor(display, 1);
	u8g2_DrawStr(display, 30+3, 29, "Ki");
	u8g2_DrawStr(display, 64+3, 29, ":");
	menu_send_int_3(display, 68+3, 29, (uint32_t)(ram.ki * 100));
	u8g2_DrawStr(display, 85+3, 29, "%");
	u8g2_DrawStr(display, 30+3, 39, "Slew");
	u8g2_DrawStr(display, 64+3, 39, ":");
	menu_send_int_3(display, 68+3, 39, (uint32_t)(ram.slew * 100));
	u8g2_DrawStr(display, 85+3, 39, "%");
	u8g2_DrawStr(display, 30+3, 49, "Max Pwr:");
	menu_send_int_3(display, 68+3, 49, (uint32_t)(ram.max_pwr * 100));
	u8g2_DrawStr(display, 85+3, 49, "%");
	u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 55, 62 - 1, "Exit");
	u8g2_SendBuffer(display);

}
void menu_settings_PiKi_run(u8g2_t *display, MenuCtx m, setting_t *s)
{
	setting_t ram = *s;

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 40, 20 - 13, "Settings");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 30+3, 19, "Kp");
	u8g2_DrawStr(display, 64+3, 19, ":");
	menu_send_int_3(display, 68+3, 19, (uint32_t)(ram.kp * 100));
	u8g2_DrawStr(display, 85+3, 19, "%");
	u8g2_DrawFrame(display, 30, 22, 65, 10);
//	u8g2_SetDrawColor(display, 0);
	u8g2_DrawStr(display, 30+3, 29, "Ki");
	u8g2_DrawStr(display, 64+3, 29, ":");
	menu_send_int_3(display, 68+3, 29, m.counter);
	u8g2_DrawStr(display, 85+3, 29, "%");
//	u8g2_SetDrawColor(display, 1);
	u8g2_DrawStr(display, 30+3, 39, "Slew");
	u8g2_DrawStr(display, 64+3, 39, ":");
	menu_send_int_3(display, 68+3, 39, (uint32_t)(ram.slew * 100));
	u8g2_DrawStr(display, 85+3, 39, "%");
	u8g2_DrawStr(display, 30+3, 49, "Max Pwr:");
	menu_send_int_3(display, 68+3, 49, (uint32_t)(ram.max_pwr * 100));
	u8g2_DrawStr(display, 85+3, 49, "%");
	u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 55, 62 - 1, "Exit");
	u8g2_SendBuffer(display);

}
void menu_settings_PiSlew_run(u8g2_t *display, MenuCtx m, setting_t *s)
{
	setting_t ram = *s;

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 40, 20 - 13, "Settings");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
//	u8g2_DrawBox(display, 30, 12, 65, 10);
	u8g2_DrawStr(display, 30+3, 19, "Kp");
	u8g2_DrawStr(display, 64+3, 19, ":");
	menu_send_int_3(display, 68+3, 19, (uint32_t)(ram.kp * 100));
	u8g2_DrawStr(display, 85+3, 19, "%");
	u8g2_DrawStr(display, 30+3, 29, "Ki");
	u8g2_DrawStr(display, 64+3, 29, ":");
	menu_send_int_3(display, 68+3, 29, (uint32_t)(ram.ki * 100));
	u8g2_DrawStr(display, 85+3, 29, "%");
	u8g2_DrawFrame(display, 30, 32, 65, 10);
//	u8g2_SetDrawColor(display, 0);
	u8g2_DrawStr(display, 30+3, 39, "Slew");
	u8g2_DrawStr(display, 64+3, 39, ":");
	menu_send_int_3(display, 68+3, 39, m.counter);
	u8g2_DrawStr(display, 85+3, 39, "%");
//	u8g2_SetDrawColor(display, 1);
	u8g2_DrawStr(display, 30+3, 49, "Max Pwr:");
	menu_send_int_3(display, 68+3, 49, (uint32_t)(ram.max_pwr * 100));
	u8g2_DrawStr(display, 85+3, 49, "%");
	u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 55, 62 - 1, "Exit");
	u8g2_SendBuffer(display);

}
void menu_settings_PiMaxPower_run(u8g2_t *display, MenuCtx m, setting_t *s)
{
	setting_t ram = *s;

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 40, 20 - 13, "Settings");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
//	u8g2_DrawBox(display, 30, 12, 65, 10);
	u8g2_DrawStr(display, 30+3, 19, "Kp");
	u8g2_DrawStr(display, 64+3, 19, ":");
	menu_send_int_3(display, 68+3, 19, (uint32_t)(ram.kp * 100));
	u8g2_DrawStr(display, 85+3, 19, "%");
	u8g2_DrawStr(display, 30+3, 29, "Ki");
	u8g2_DrawStr(display, 64+3, 29, ":");
	menu_send_int_3(display, 68+3, 29, (uint32_t)(ram.ki * 100));
	u8g2_DrawStr(display, 85+3, 29, "%");
	u8g2_DrawStr(display, 30+3, 39, "Slew");
	u8g2_DrawStr(display, 64+3, 39, ":");
	menu_send_int_3(display, 68+3, 39, (uint32_t)(ram.slew * 100));
	u8g2_DrawStr(display, 85+3, 39, "%");
	u8g2_DrawFrame(display, 30, 42, 65, 10);
//	u8g2_SetDrawColor(display, 0);
	u8g2_DrawStr(display, 30+3, 49, "Max Pwr:");
	menu_send_int_3(display, 68+3, 49, m.counter);
	u8g2_DrawStr(display, 85+3, 49, "%");
//	u8g2_SetDrawColor(display, 1);
	u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 55, 62 - 1, "Exit");
	u8g2_SendBuffer(display);

}

void menu_settings_TimeDate_run(u8g2_t *display, MenuCtx m, Sens t)
{
	switch (m.counter) {
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 4, 20 - 13, "Time & Date Settings");
		u8g2_DrawLine(display, 0, 22 - 13, 128, 22 - 13);
		u8g2_SetFont(display, u8g2_font_8x13_tr);
		u8g2_DrawRFrame(display, 42, 13, 18, 15, 5);
		menu_send_time(display, 43, 25, t.hour, t.minute);
		menu_send_date(display, 25, 45, t.date, t.month, t.year);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
		u8g2_DrawStr(display, 55, 62 - 1, "Exit");
		u8g2_SendBuffer(display);
		break;
	case 1:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 4, 20 - 13, "Time & Date Settings");
		u8g2_DrawLine(display, 0, 22 - 13, 128, 22 - 13);
		u8g2_SetFont(display, u8g2_font_8x13_tr);
		u8g2_DrawRFrame(display, 66, 13, 18, 15, 5);
		menu_send_time(display, 43, 25, t.hour, t.minute);
		menu_send_date(display, 25, 45, t.date, t.month, t.year);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
		u8g2_DrawStr(display, 55, 62 - 1, "Exit");
		u8g2_SendBuffer(display);
		break;
	case 2:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 4, 20 - 13, "Time & Date Settings");
		u8g2_DrawLine(display, 0, 22 - 13, 128, 22 - 13);
		u8g2_SetFont(display, u8g2_font_8x13_tr);
		menu_send_time(display, 43, 25, t.hour, t.minute);
		u8g2_DrawRFrame(display, 24, 33, 18, 15, 5);
		menu_send_date(display, 25, 45, t.date, t.month, t.year);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
		u8g2_DrawStr(display, 55, 62 - 1, "Exit");
		u8g2_SendBuffer(display);
		break;
	case 3:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 4, 20 - 13, "Time & Date Settings");
		u8g2_DrawLine(display, 0, 22 - 13, 128, 22 - 13);
		u8g2_SetFont(display, u8g2_font_8x13_tr);
		menu_send_time(display, 43, 25, t.hour, t.minute);
		u8g2_DrawRFrame(display, 48, 33, 18, 15, 5);
		menu_send_date(display, 25, 45, t.date, t.month, t.year);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
		u8g2_DrawStr(display, 55, 62 - 1, "Exit");
		u8g2_SendBuffer(display);
		break;
	case 4:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 4, 20 - 13, "Time & Date Settings");
		u8g2_DrawLine(display, 0, 22 - 13, 128, 22 - 13);
		u8g2_SetFont(display, u8g2_font_8x13_tr);
		menu_send_time(display, 43, 25, t.hour, t.minute);
		u8g2_DrawRFrame(display, 72, 33, 34, 15, 5);
		menu_send_date(display, 25, 45, t.date, t.month, t.year);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
		u8g2_DrawStr(display, 55, 62 - 1, "Exit");
		u8g2_SendBuffer(display);
		break;
	case 5:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 4, 20 - 13, "Time & Date Settings");
		u8g2_DrawLine(display, 0, 22 - 13, 128, 22 - 13);
		u8g2_SetFont(display, u8g2_font_8x13_tr);
		menu_send_time(display, 43, 25, t.hour, t.minute);
		menu_send_date(display, 25, 45, t.date, t.month, t.year);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRBox(display, 55 - 5, 59 - 6, 23, 11, 5);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 55, 62 - 1, "Exit");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;

	}
}
void menu_settings_TimeDateHour_run(u8g2_t *display, MenuCtx m, Sens t)
{

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 4, 20 - 13, "Time & Date Settings");
	u8g2_DrawLine(display, 0, 22 - 13, 128, 22 - 13);
	u8g2_SetFont(display, u8g2_font_8x13_tr);
	u8g2_DrawRBox(display, 42, 13, 18, 15, 5);
	u8g2_SetDrawColor(display, 0);
	menu_send_int_2(display, 43, 25, m.counter);
	u8g2_SetDrawColor(display, 1);
	u8g2_DrawStr(display, 59, 25, ":");
	menu_send_int_2(display, 67, 25, t.minute);
	menu_send_date(display, 25, 45, t.date, t.month, t.year);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 55, 62 - 1, "Exit");
	u8g2_SendBuffer(display);

}
void menu_settings_TimeDateMin_run(u8g2_t *display, MenuCtx m, Sens t){
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 4, 20 - 13, "Time & Date Settings");
	u8g2_DrawLine(display, 0, 22 - 13, 128, 22 - 13);
	u8g2_SetFont(display, u8g2_font_8x13_tr);
	menu_send_int_2(display, 43, 25, t.hour);
	u8g2_DrawStr(display, 59, 25, ":");
	u8g2_DrawRBox(display, 66, 13, 18, 15, 5);
	u8g2_SetDrawColor(display, 0);
	menu_send_int_2(display, 67, 25, m.counter);
	u8g2_SetDrawColor(display, 1);
	menu_send_date(display, 25, 45, t.date, t.month, t.year);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 55, 62 - 1, "Exit");
	u8g2_SendBuffer(display);
}
void menu_settings_TimeDateDay_run(u8g2_t *display, MenuCtx m, Sens t){
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 4, 20 - 13, "Time & Date Settings");
	u8g2_DrawLine(display, 0, 22 - 13, 128, 22 - 13);
	u8g2_SetFont(display, u8g2_font_8x13_tr);
	menu_send_int_2(display, 43, 25, t.hour);
	u8g2_DrawStr(display, 59, 25, ":");
	menu_send_int_2(display, 67, 25, t.minute);
	u8g2_DrawRBox(display, 24, 33, 18, 15, 5);
	u8g2_SetDrawColor(display, 0);
	menu_send_int_2(display, 25, 45, m.counter);
	u8g2_SetDrawColor(display, 1);
	u8g2_DrawStr(display, 41, 45, ".");
	menu_send_int_2(display, 49, 45, t.month);
	u8g2_DrawStr(display, 65, 45, ".");
	menu_send_int_4(display, 73, 45, t.year);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 55, 62 - 1, "Exit");
	u8g2_SendBuffer(display);
}
void menu_settings_TimeDateMonth_run(u8g2_t *display, MenuCtx m, Sens t){
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 4, 20 - 13, "Time & Date Settings");
	u8g2_DrawLine(display, 0, 22 - 13, 128, 22 - 13);
	u8g2_SetFont(display, u8g2_font_8x13_tr);
	menu_send_int_2(display, 43, 25, t.hour);
	u8g2_DrawStr(display, 59, 25, ":");
	menu_send_int_2(display, 67, 25, t.minute);
	menu_send_int_2(display, 25, 45, t.date);
	u8g2_DrawStr(display, 41, 45, ".");
	u8g2_DrawRBox(display, 48, 33, 18, 15, 5);
	u8g2_SetDrawColor(display, 0);
	menu_send_int_2(display, 49, 45, m.counter);
	u8g2_SetDrawColor(display, 1);
	u8g2_DrawStr(display, 65, 45, ".");
	menu_send_int_4(display, 73, 45, t.year);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 55, 62 - 1, "Exit");
	u8g2_SendBuffer(display);
}
void menu_settings_TimeDateYear_run(u8g2_t *display, MenuCtx m, Sens t){
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 4, 20 - 13, "Time & Date Settings");
	u8g2_DrawLine(display, 0, 22 - 13, 128, 22 - 13);
	u8g2_SetFont(display, u8g2_font_8x13_tr);
	menu_send_int_2(display, 43, 25, t.hour);
	u8g2_DrawStr(display, 59, 25, ":");
	menu_send_int_2(display, 67, 25, t.minute);
	menu_send_int_2(display, 25, 45, t.date);
	u8g2_DrawStr(display, 41, 45, ".");
	menu_send_int_2(display, 49, 45, t.month);
	u8g2_DrawStr(display, 65, 45, ".");
	u8g2_DrawRBox(display, 72, 33, 34, 15, 5);
	u8g2_SetDrawColor(display, 0);
	menu_send_int_4(display, 73, 45, m.counter);
	u8g2_SetDrawColor(display, 1);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 55, 62 - 1, "Exit");
	u8g2_SendBuffer(display);
}


void menu_programs_run(u8g2_t *display, MenuCtx m) {

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 40, 20 - 13, "Programs");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);

	switch (m.counter) {

	case 0:
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawBox(display, 40, 25 - 13, 47, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 42, 32 - 13, "Fast Start");
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawStr(display, 15, 42 - 13, "Advanced Temp. Control");
		u8g2_DrawStr(display, 31, 52 - 13, "Auto Calibration");
		u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
		u8g2_DrawStr(display, 55, 62 - 1, "Exit");
		u8g2_SendBuffer(display);

		break;
	case 1:
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawStr(display, 42, 32 - 13, "Fast Start");
		u8g2_DrawBox(display, 15 - 2, 42 - 7 - 13, 101, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 15, 42 - 13, "Advanced Temp. Control");
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawStr(display, 31, 52 - 13, "Auto Calibration");
		u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
		u8g2_DrawStr(display, 55, 62 - 1, "Exit");
		u8g2_SendBuffer(display);

		break;
	case 2:
//		  u8g2_DrawRFrame(&display, 8, 0, 110, 11, 5);
//		  u8g2_SetFont(&display, u8g2_font_04b_03_tr);
//		  u8g2_DrawStr(&display, 11, 7, "Composite Curing Furnace");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawStr(display, 42, 32 - 13, "Fast Start");
		u8g2_DrawStr(display, 15, 42 - 13, "Advanced Temp. Control");
		u8g2_DrawBox(display, 29, 52 - 7 - 13, 69, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 31, 52 - 13, "Auto Calibration");
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
		u8g2_DrawStr(display, 55, 62 - 1, "Exit");
		u8g2_SendBuffer(display);
		break;
	case 3:
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawStr(display, 42, 32 - 13, "Fast Start");
		u8g2_DrawStr(display, 15, 42 - 13, "Advanced Temp. Control");
		u8g2_DrawStr(display, 31, 52 - 13, "Auto Calibration");
		u8g2_DrawRBox(display, 55 - 5, 59 - 6, 23, 11, 5);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 55, 62 - 1, "Exit");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);

		break;

	}
}

void menu_prg_FastStart_run(u8g2_t *display, MenuCtx m, CookingCtx c)
{
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 34, 20 - 13, "Fast Start");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 23 + 3, 19, "Trgt Temp");
	u8g2_DrawStr(display, 64 + 3, 19, ":");
	menu_send_int_3(display, 68 + 3, 19, c.faststart->temperature);
	u8g2_DrawStr(display, 85 + 3, 19, "C");
	u8g2_DrawStr(display, 23 + 3, 29, "Rise Time");
	u8g2_DrawStr(display, 64 + 3, 29, ":");
	menu_send_int_3(display, 68 + 3, 29, c.faststart->changePeriod);
	u8g2_DrawStr(display, 85 + 3, 29, "Min");
	u8g2_DrawStr(display, 23 + 3, 39, "Fall Time");
	u8g2_DrawStr(display, 64 + 3, 39, ":");
	menu_send_int_3(display, 68 + 3, 39, c.faststart->fallTime);
	u8g2_DrawStr(display, 85 + 3, 39, "Min");
	u8g2_DrawStr(display, 23 + 3, 49, "Cook Time");
	u8g2_DrawStr(display, 64 + 3, 49, ":");
	menu_send_int_3(display, 68 + 3, 49, c.faststart->duration);
	u8g2_DrawStr(display, 85 + 3, 49, "Min");
	u8g2_DrawRFrame(display, 80 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 80, 62 - 1, "Exit");
	u8g2_DrawRFrame(display, 32 - 5, 59 - 6, 30, 11, 5);
	u8g2_DrawStr(display, 31, 62 - 1, "Start");

	switch(m.counter){
	case 0:
		u8g2_DrawBox(display, 23, 12, 79, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 23 + 3, 19, "Trgt Temp");
		u8g2_DrawStr(display, 64 + 3, 19, ":");
		menu_send_int_3(display, 68 + 3, 19, c.faststart->temperature);
		u8g2_DrawStr(display, 85 + 3, 19, "C");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 1:
		u8g2_DrawBox(display, 23, 22, 79, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 23 + 3, 29, "Rise Time");
		u8g2_DrawStr(display, 64 + 3, 29, ":");
		menu_send_int_3(display, 68 + 3, 29, c.faststart->changePeriod);
		u8g2_DrawStr(display, 85 + 3, 29, "Min");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 2:
		u8g2_DrawBox(display, 23, 32, 79, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 23 + 3, 39, "Fall Time");
		u8g2_DrawStr(display, 64 + 3, 39, ":");
		menu_send_int_3(display, 68 + 3, 39, c.faststart->fallTime);
		u8g2_DrawStr(display, 85 + 3, 39, "Min");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 3:
		u8g2_DrawBox(display, 23, 42, 79, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 23 + 3, 49, "Cook Time");
		u8g2_DrawStr(display, 64 + 3, 49, ":");
		menu_send_int_3(display, 68 + 3, 49, c.faststart->duration);
		u8g2_DrawStr(display, 85 + 3, 49, "Min");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 4:
		u8g2_DrawRBox(display, 32 - 5, 59 - 6, 30, 11, 5);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 31, 62 - 1, "Start");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 5:
		u8g2_DrawRBox(display, 80 - 5, 59 - 6, 23, 11, 5);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 80, 62 - 1, "Exit");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	default:
		break;
	}

}

void menu_prg_FastStart_TrgtTemp_run(u8g2_t *display, MenuCtx m, CookingCtx c)
{
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 34, 20 - 13, "Fast Start");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 23, 12, 79, 10);
	u8g2_DrawStr(display, 23 + 3, 19, "Trgt Temp");
	u8g2_DrawStr(display, 64 + 3, 19, ":");
	menu_send_int_3(display, 68 + 3, 19, m.counter);
	u8g2_DrawStr(display, 85 + 3, 19, "C");
	u8g2_DrawStr(display, 23 + 3, 29, "Rise Time");
	u8g2_DrawStr(display, 64 + 3, 29, ":");
	menu_send_int_3(display, 68 + 3, 29, c.faststart->changePeriod);
	u8g2_DrawStr(display, 85 + 3, 29, "Min");
	u8g2_DrawStr(display, 23 + 3, 39, "Fall Time");
	u8g2_DrawStr(display, 64 + 3, 39, ":");
	menu_send_int_3(display, 68 + 3, 39, c.faststart->fallTime);
	u8g2_DrawStr(display, 85 + 3, 39, "Min");
	u8g2_DrawStr(display, 23 + 3, 49, "Cook Time");
	u8g2_DrawStr(display, 64 + 3, 49, ":");
	menu_send_int_3(display, 68 + 3, 49, c.faststart->duration);
	u8g2_DrawStr(display, 85 + 3, 49, "Min");
	u8g2_DrawRFrame(display, 80 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 80, 62 - 1, "Exit");
	u8g2_DrawRFrame(display, 32 - 5, 59 - 6, 30, 11, 5);
	u8g2_DrawStr(display, 31, 62 - 1, "Start");
	u8g2_SendBuffer(display);
}

void menu_prg_FastStart_RiseTime_run(u8g2_t *display, MenuCtx m, CookingCtx c)
{
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 34, 20 - 13, "Fast Start");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 23 + 3, 19, "Trgt Temp");
	u8g2_DrawStr(display, 64 + 3, 19, ":");
	menu_send_int_3(display, 68 + 3, 19, c.faststart->temperature);
	u8g2_DrawStr(display, 85 + 3, 19, "C");
	u8g2_DrawFrame(display, 23, 22, 79, 10);
	u8g2_DrawStr(display, 23 + 3, 29, "Rise Time");
	u8g2_DrawStr(display, 64 + 3, 29, ":");
	menu_send_int_3(display, 68 + 3, 29, m.counter);
	u8g2_DrawStr(display, 85 + 3, 29, "Min");
	u8g2_DrawStr(display, 23 + 3, 39, "Fall Time");
	u8g2_DrawStr(display, 64 + 3, 39, ":");
	menu_send_int_3(display, 68 + 3, 39, c.faststart->fallTime);
	u8g2_DrawStr(display, 85 + 3, 39, "Min");
	u8g2_DrawStr(display, 23 + 3, 49, "Cook Time");
	u8g2_DrawStr(display, 64 + 3, 49, ":");
	menu_send_int_3(display, 68 + 3, 49, c.faststart->duration);
	u8g2_DrawStr(display, 85 + 3, 49, "Min");
	u8g2_DrawRFrame(display, 80 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 80, 62 - 1, "Exit");
	u8g2_DrawRFrame(display, 32 - 5, 59 - 6, 30, 11, 5);
	u8g2_DrawStr(display, 31, 62 - 1, "Start");
	u8g2_SendBuffer(display);
}

void menu_prg_FastStart_FallTime_run(u8g2_t *display, MenuCtx m, CookingCtx c)
{
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 34, 20 - 13, "Fast Start");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 23 + 3, 19, "Trgt Temp");
	u8g2_DrawStr(display, 64 + 3, 19, ":");
	menu_send_int_3(display, 68 + 3, 19, c.faststart->temperature);
	u8g2_DrawStr(display, 85 + 3, 19, "C");
	u8g2_DrawStr(display, 23 + 3, 29, "Rise Time");
	u8g2_DrawStr(display, 64 + 3, 29, ":");
	menu_send_int_3(display, 68 + 3, 29, c.faststart->changePeriod);
	u8g2_DrawStr(display, 85 + 3, 29, "Min");
	u8g2_DrawFrame(display, 23, 32, 79, 10);
	u8g2_DrawStr(display, 23 + 3, 39, "Fall Time");
	u8g2_DrawStr(display, 64 + 3, 39, ":");
	menu_send_int_3(display, 68 + 3, 39, m.counter);
	u8g2_DrawStr(display, 85 + 3, 39, "Min");
	u8g2_DrawStr(display, 23 + 3, 49, "Cook Time");
	u8g2_DrawStr(display, 64 + 3, 49, ":");
	menu_send_int_3(display, 68 + 3, 49, c.faststart->duration);
	u8g2_DrawStr(display, 85 + 3, 49, "Min");
	u8g2_DrawRFrame(display, 80 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 80, 62 - 1, "Exit");
	u8g2_DrawRFrame(display, 32 - 5, 59 - 6, 30, 11, 5);
	u8g2_DrawStr(display, 31, 62 - 1, "Start");
	u8g2_SendBuffer(display);
}

void menu_prg_FastStart_Duration_run(u8g2_t *display, MenuCtx m, CookingCtx c)
{
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 34, 20 - 13, "Fast Start");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 23 + 3, 19, "Trgt Temp");
	u8g2_DrawStr(display, 64 + 3, 19, ":");
	menu_send_int_3(display, 68 + 3, 19, c.faststart->temperature);
	u8g2_DrawStr(display, 85 + 3, 19, "C");
	u8g2_DrawStr(display, 23 + 3, 29, "Rise Time");
	u8g2_DrawStr(display, 64 + 3, 29, ":");
	menu_send_int_3(display, 68 + 3, 29, c.faststart->changePeriod);
	u8g2_DrawStr(display, 85 + 3, 29, "Min");
	u8g2_DrawStr(display, 23 + 3, 39, "Fall Time");
	u8g2_DrawStr(display, 64 + 3, 39, ":");
	menu_send_int_3(display, 68 + 3, 39, c.faststart->fallTime);
	u8g2_DrawStr(display, 85 + 3, 39, "Min");
	u8g2_DrawFrame(display, 23, 42, 79, 10);
	u8g2_DrawStr(display, 23 + 3, 49, "Cook Time");
	u8g2_DrawStr(display, 64 + 3, 49, ":");
	menu_send_int_3(display, 68 + 3, 49, m.counter);
	u8g2_DrawStr(display, 85 + 3, 49, "Min");
	u8g2_DrawRFrame(display, 80 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 80, 62 - 1, "Exit");
	u8g2_DrawRFrame(display, 32 - 5, 59 - 6, 30, 11, 5);
	u8g2_DrawStr(display, 31, 62 - 1, "Start");
	u8g2_SendBuffer(display);
}


void menu_prg_AdvTempCtrl_run(u8g2_t *display, MenuCtx m, CookingCtx c){
	switch(m.counter)
	{
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");


		u8g2_DrawFrame(display, 0, 0, 105, 44);
		u8g2_DrawFrame(display, 1, 1, 103, 42);
		u8g2_DrawFrame(display, 2, 2, 101, 40);
		u8g2_DrawStr(display, 19, 21, "Generate curing");
		u8g2_DrawStr(display, 35, 29, "process");
		u8g2_DrawRFrame(display, 108, 0, 17, 33, 6);
		u8g2_DrawStr(display, 115, 7, "S");
		u8g2_DrawStr(display, 115, 13, "T");
		u8g2_DrawStr(display, 115, 19, "A");
		u8g2_DrawStr(display, 115, 25, "R");
		u8g2_DrawStr(display, 115, 31, "T");
		u8g2_DrawRFrame(display, 107, 35, 19, 11, 5);
		u8g2_DrawStr(display, 110, 43, "Exit");
		u8g2_SendBuffer(display);


		break;
	case 1:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 105, 44);
		u8g2_DrawStr(display, 19, 21, "Generate curing");
		u8g2_DrawStr(display, 35, 29, "process");
		u8g2_DrawRBox(display, 108, 0, 17, 33, 6);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 115, 7, "S");
		u8g2_DrawStr(display, 115, 13, "T");
		u8g2_DrawStr(display, 115, 19, "A");
		u8g2_DrawStr(display, 115, 25, "R");
		u8g2_DrawStr(display, 115, 31, "T");
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawRFrame(display, 107, 35, 19, 11, 5);
		u8g2_DrawStr(display, 110, 43, "Exit");
		u8g2_SendBuffer(display);
		break;
	case 2:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawBox(display, 0, 46, 31, 9);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 105, 44);
		u8g2_DrawStr(display, 19, 21, "Generate curing");
		u8g2_DrawStr(display, 35, 29, "process");
		u8g2_DrawRFrame(display, 108, 0, 17, 33, 6);
		u8g2_DrawStr(display, 115, 7, "S");
		u8g2_DrawStr(display, 115, 13, "T");
		u8g2_DrawStr(display, 115, 19, "A");
		u8g2_DrawStr(display, 115, 25, "R");
		u8g2_DrawStr(display, 115, 31, "T");
		u8g2_DrawRFrame(display, 107, 35, 19, 11, 5);
		u8g2_DrawStr(display, 110, 43, "Exit");
		u8g2_SendBuffer(display);
		break;
	case 3:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawBox(display, 30, 46, 31, 9);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 105, 44);
		u8g2_DrawStr(display, 19, 21, "Generate curing");
		u8g2_DrawStr(display, 35, 29, "process");
		u8g2_DrawRFrame(display, 108, 0, 17, 33, 6);
		u8g2_DrawStr(display, 115, 7, "S");
		u8g2_DrawStr(display, 115, 13, "T");
		u8g2_DrawStr(display, 115, 19, "A");
		u8g2_DrawStr(display, 115, 25, "R");
		u8g2_DrawStr(display, 115, 31, "T");
		u8g2_DrawRFrame(display, 107, 35, 19, 11, 5);
		u8g2_DrawStr(display, 110, 43, "Exit");
		u8g2_SendBuffer(display);
		break;
	case 4:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawBox(display, 60, 46, 31, 9);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 105, 44);
		u8g2_DrawStr(display, 19, 21, "Generate curing");
		u8g2_DrawStr(display, 35, 29, "process");
		u8g2_DrawRFrame(display, 108, 0, 17, 33, 6);
		u8g2_DrawStr(display, 115, 7, "S");
		u8g2_DrawStr(display, 115, 13, "T");
		u8g2_DrawStr(display, 115, 19, "A");
		u8g2_DrawStr(display, 115, 25, "R");
		u8g2_DrawStr(display, 115, 31, "T");
		u8g2_DrawRFrame(display, 107, 35, 19, 11, 5);
		u8g2_DrawStr(display, 110, 43, "Exit");
		u8g2_SendBuffer(display);
		break;
	case 5:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawBox(display, 0, 54, 31, 9);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 105, 44);
		u8g2_DrawStr(display, 19, 21, "Generate curing");
		u8g2_DrawStr(display, 35, 29, "process");
		u8g2_DrawRFrame(display, 108, 0, 17, 33, 6);
		u8g2_DrawStr(display, 115, 7, "S");
		u8g2_DrawStr(display, 115, 13, "T");
		u8g2_DrawStr(display, 115, 19, "A");
		u8g2_DrawStr(display, 115, 25, "R");
		u8g2_DrawStr(display, 115, 31, "T");
		u8g2_DrawRFrame(display, 107, 35, 19, 11, 5);
		u8g2_DrawStr(display, 110, 43, "Exit");
		u8g2_SendBuffer(display);
		break;
	case 6:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawBox(display, 30, 54, 31, 9);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 105, 44);
		u8g2_DrawStr(display, 19, 21, "Generate curing");
		u8g2_DrawStr(display, 35, 29, "process");
		u8g2_DrawRFrame(display, 108, 0, 17, 33, 6);
		u8g2_DrawStr(display, 115, 7, "S");
		u8g2_DrawStr(display, 115, 13, "T");
		u8g2_DrawStr(display, 115, 19, "A");
		u8g2_DrawStr(display, 115, 25, "R");
		u8g2_DrawStr(display, 115, 31, "T");
		u8g2_DrawRFrame(display, 107, 35, 19, 11, 5);
		u8g2_DrawStr(display, 110, 43, "Exit");
		u8g2_SendBuffer(display);
		break;
	case 7:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawBox(display, 60, 54, 31, 9);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 105, 44);
		u8g2_DrawStr(display, 19, 21, "Generate curing");
		u8g2_DrawStr(display, 35, 29, "process");
		u8g2_DrawRFrame(display, 108, 0, 17, 33, 6);
		u8g2_DrawStr(display, 115, 7, "S");
		u8g2_DrawStr(display, 115, 13, "T");
		u8g2_DrawStr(display, 115, 19, "A");
		u8g2_DrawStr(display, 115, 25, "R");
		u8g2_DrawStr(display, 115, 31, "T");
		u8g2_DrawRFrame(display, 107, 35, 19, 11, 5);
		u8g2_DrawStr(display, 110, 43, "Exit");
		u8g2_SendBuffer(display);
		break;
	case 8:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 105, 44);
		u8g2_DrawStr(display, 19, 21, "Generate curing");
		u8g2_DrawStr(display, 35, 29, "process");
		u8g2_DrawRFrame(display, 108, 0, 17, 33, 6);
		u8g2_DrawStr(display, 115, 7, "S");
		u8g2_DrawStr(display, 115, 13, "T");
		u8g2_DrawStr(display, 115, 19, "A");
		u8g2_DrawStr(display, 115, 25, "R");
		u8g2_DrawStr(display, 115, 31, "T");
		u8g2_DrawRBox(display, 107, 35, 19, 11, 5);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 110, 43, "Exit");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;

	default:
		break;

	}
}

void menu_prg_AdvTempCtrl_STG1_run(u8g2_t *display, MenuCtx m, CookingCtx c)
{
	switch(m.counter)
	{
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 1 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawBox(display, 20, 22, 30, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 22, 30, c.stage1->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_SetDrawColor(display, 1);
		menu_send_int_3(display, 78 + 3, 30, c.stage1->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	case 1:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 1 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage1->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_DrawBox(display, 80, 22, 40, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 78 + 3, 30, c.stage1->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetDrawColor(display, 1);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	case 2:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 1 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage1->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage1->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRBox(display, 36, 33, 18, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 39, 40, "SET");
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	case 3:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 1 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage1->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage1->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		u8g2_DrawRBox(display, 76, 33, 15, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		break;

	}

}
void menu_prg_AdvTempCtrl_STG1_SetTemp_run(u8g2_t *display, MenuCtx m, CookingCtx c) {

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, c.stage1->temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, c.stage4->temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
	u8g2_DrawStr(display, 93, 52, "Duration");
	u8g2_DrawLine(display, 93, 54, 126, 54);
	menu_send_int_3(display, 93, 62, 0);
	u8g2_DrawStr(display, 113, 62, "MIN");

	u8g2_DrawFrame(display, 0, 0, 128, 44);
	u8g2_DrawStr(display, 28 - 1, 7, "Stage 1 Settings");
	u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
	u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
	u8g2_DrawStr(display, 80 + 3, 18, "Duration");
	u8g2_DrawFrame(display, 19, 21, 32, 11);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	menu_send_int_3(display, 22, 30, m.counter);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	menu_send_int_3(display, 78 + 3, 30, c.stage1->duration);
	u8g2_DrawStr(display, 98 + 3, 30, "Min");
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
	u8g2_DrawStr(display, 79, 40, "OK");
	u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
	u8g2_DrawStr(display, 39, 40, "SET");
	//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
	//	  u8g2_DrawStr(&display, 35, 40, "RESET");
	u8g2_SendBuffer(display);
}
void menu_prg_AdvTempCtrl_STG1_SetDur_run(u8g2_t *display, MenuCtx m, CookingCtx c){

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, c.stage1->temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, c.stage4->temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
	u8g2_DrawStr(display, 93, 52, "Duration");
	u8g2_DrawLine(display, 93, 54, 126, 54);
	menu_send_int_3(display, 93, 62, 0);
	u8g2_DrawStr(display, 113, 62, "MIN");

	u8g2_DrawFrame(display, 0, 0, 128, 44);
	u8g2_DrawStr(display, 28 - 1, 7, "Stage 1 Settings");
	u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
	u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
	u8g2_DrawStr(display, 80 + 3, 18, "Duration");
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	menu_send_int_3(display, 22, 30, c.stage1->temperature);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	u8g2_DrawFrame(display, 79, 21, 42, 11);
	u8g2_SetDrawColor(display, 1);
	menu_send_int_3(display, 78 + 3, 30, m.counter);
	u8g2_DrawStr(display, 98 + 3, 30, "Min");
	u8g2_SetDrawColor(display, 1);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
	u8g2_DrawStr(display, 79, 40, "OK");
	u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
	u8g2_DrawStr(display, 39, 40, "SET");
	//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
	//	  u8g2_DrawStr(&display, 35, 40, "RESET");
	u8g2_SendBuffer(display);

}
void menu_prg_AdvTempCtrl_STG1_SetReset_run(u8g2_t *display, MenuCtx m, CookingCtx c){

	switch(m.counter){
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 1 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage1->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage1->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRBox(display, 32, 33, 26, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 35, 40, "RESET");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 1:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 1 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage1->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage1->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRBox(display, 76, 33, 15, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 32, 33, 26, 9, 3);
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawStr(display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	}

}

void menu_prg_AdvTempCtrl_STG2_run(u8g2_t *display, MenuCtx m, CookingCtx c)
{
	switch(m.counter)
	{
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 2 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawBox(display, 20, 22, 30, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 22, 30, c.stage2->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_SetDrawColor(display, 1);
		menu_send_int_3(display, 78 + 3, 30, c.stage2->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	case 1:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 2 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage2->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_DrawBox(display, 80, 22, 40, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 78 + 3, 30, c.stage2->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetDrawColor(display, 1);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	case 2:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 2 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage2->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage2->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRBox(display, 36, 33, 18, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 39, 40, "SET");
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	case 3:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 2 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage2->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage2->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		u8g2_DrawRBox(display, 76, 33, 15, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_SetDrawColor(display, 1);
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	default:
		break;


	}

}
void menu_prg_AdvTempCtrl_STG2_SetTemp_run(u8g2_t *display, MenuCtx m, CookingCtx c) {

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, c.stage1->temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, c.stage4->temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
	u8g2_DrawStr(display, 93, 52, "Duration");
	u8g2_DrawLine(display, 93, 54, 126, 54);
	menu_send_int_3(display, 93, 62, 0);
	u8g2_DrawStr(display, 113, 62, "MIN");

	u8g2_DrawFrame(display, 0, 0, 128, 44);
	u8g2_DrawStr(display, 28 - 1, 7, "Stage 2 Settings");
	u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
	u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
	u8g2_DrawStr(display, 80 + 3, 18, "Duration");
	u8g2_DrawFrame(display, 19, 21, 32, 11);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	menu_send_int_3(display, 22, 30, m.counter);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	menu_send_int_3(display, 78 + 3, 30, c.stage2->duration);
	u8g2_DrawStr(display, 98 + 3, 30, "Min");
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
	u8g2_DrawStr(display, 79, 40, "OK");
	u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
	u8g2_DrawStr(display, 39, 40, "SET");
	//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
	//	  u8g2_DrawStr(&display, 35, 40, "RESET");
	u8g2_SendBuffer(display);
}
void menu_prg_AdvTempCtrl_STG2_SetDur_run(u8g2_t *display, MenuCtx m, CookingCtx c){

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, c.stage1->temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, c.stage4->temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
	u8g2_DrawStr(display, 93, 52, "Duration");
	u8g2_DrawLine(display, 93, 54, 126, 54);
	menu_send_int_3(display, 93, 62, 0);
	u8g2_DrawStr(display, 113, 62, "MIN");

	u8g2_DrawFrame(display, 0, 0, 128, 44);
	u8g2_DrawStr(display, 28 - 1, 7, "Stage 2 Settings");
	u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
	u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
	u8g2_DrawStr(display, 80 + 3, 18, "Duration");
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	menu_send_int_3(display, 22, 30, c.stage2->temperature);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	u8g2_DrawFrame(display, 79, 21, 42, 11);
	u8g2_SetDrawColor(display, 1);
	menu_send_int_3(display, 78 + 3, 30, m.counter);
	u8g2_DrawStr(display, 98 + 3, 30, "Min");
	u8g2_SetDrawColor(display, 1);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
	u8g2_DrawStr(display, 79, 40, "OK");
	u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
	u8g2_DrawStr(display, 39, 40, "SET");
	//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
	//	  u8g2_DrawStr(&display, 35, 40, "RESET");
	u8g2_SendBuffer(display);

}
void menu_prg_AdvTempCtrl_STG2_SetReset_run(u8g2_t *display, MenuCtx m, CookingCtx c){

	switch(m.counter){
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 2 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage2->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage2->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRBox(display, 32, 33, 26, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 35, 40, "RESET");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 1:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 2 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage2->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage2->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRBox(display, 76, 33, 15, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 32, 33, 26, 9, 3);
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawStr(display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	}

}

void menu_prg_AdvTempCtrl_STG3_run(u8g2_t *display, MenuCtx m, CookingCtx c)
{
	switch(m.counter)
	{
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 3 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawBox(display, 20, 22, 30, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 22, 30, c.stage3->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_SetDrawColor(display, 1);
		menu_send_int_3(display, 78 + 3, 30, c.stage3->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	case 1:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 3 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage3->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_DrawBox(display, 80, 22, 40, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 78 + 3, 30, c.stage3->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetDrawColor(display, 1);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	case 2:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 3 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage3->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage3->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRBox(display, 36, 33, 18, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 39, 40, "SET");
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	case 3:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 3 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage3->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage3->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		u8g2_DrawRBox(display, 76, 33, 15, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_SetDrawColor(display, 1);
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	default:
		break;


	}

}
void menu_prg_AdvTempCtrl_STG3_SetTemp_run(u8g2_t *display, MenuCtx m, CookingCtx c) {

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, c.stage1->temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, c.stage4->temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
	u8g2_DrawStr(display, 93, 52, "Duration");
	u8g2_DrawLine(display, 93, 54, 126, 54);
	menu_send_int_3(display, 93, 62, 0);
	u8g2_DrawStr(display, 113, 62, "MIN");

	u8g2_DrawFrame(display, 0, 0, 128, 44);
	u8g2_DrawStr(display, 28 - 1, 7, "Stage 3 Settings");
	u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
	u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
	u8g2_DrawStr(display, 80 + 3, 18, "Duration");
	u8g2_DrawFrame(display, 19, 21, 32, 11);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	menu_send_int_3(display, 22, 30, m.counter);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	menu_send_int_3(display, 78 + 3, 30, c.stage3->duration);
	u8g2_DrawStr(display, 98 + 3, 30, "Min");
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
	u8g2_DrawStr(display, 79, 40, "OK");
	u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
	u8g2_DrawStr(display, 39, 40, "SET");
	//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
	//	  u8g2_DrawStr(&display, 35, 40, "RESET");
	u8g2_SendBuffer(display);
}
void menu_prg_AdvTempCtrl_STG3_SetDur_run(u8g2_t *display, MenuCtx m, CookingCtx c){

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, c.stage1->temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, c.stage4->temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
	u8g2_DrawStr(display, 93, 52, "Duration");
	u8g2_DrawLine(display, 93, 54, 126, 54);
	menu_send_int_3(display, 93, 62, 0);
	u8g2_DrawStr(display, 113, 62, "MIN");

	u8g2_DrawFrame(display, 0, 0, 128, 44);
	u8g2_DrawStr(display, 28 - 1, 7, "Stage 3 Settings");
	u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
	u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
	u8g2_DrawStr(display, 80 + 3, 18, "Duration");
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	menu_send_int_3(display, 22, 30, c.stage3->temperature);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	u8g2_DrawFrame(display, 79, 21, 42, 11);
	u8g2_SetDrawColor(display, 1);
	menu_send_int_3(display, 78 + 3, 30, m.counter);
	u8g2_DrawStr(display, 98 + 3, 30, "Min");
	u8g2_SetDrawColor(display, 1);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
	u8g2_DrawStr(display, 79, 40, "OK");
	u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
	u8g2_DrawStr(display, 39, 40, "SET");
	//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
	//	  u8g2_DrawStr(&display, 35, 40, "RESET");
	u8g2_SendBuffer(display);

}
void menu_prg_AdvTempCtrl_STG3_SetReset_run(u8g2_t *display, MenuCtx m, CookingCtx c){

	switch(m.counter){
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 3 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage3->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage3->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRBox(display, 32, 33, 26, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 35, 40, "RESET");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 1:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 3 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage3->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage3->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRBox(display, 76, 33, 15, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 32, 33, 26, 9, 3);
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawStr(display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	}

}

void menu_prg_AdvTempCtrl_STG4_run(u8g2_t *display, MenuCtx m, CookingCtx c)
{
	switch(m.counter)
	{
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 4 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawBox(display, 20, 22, 30, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 22, 30, c.stage4->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_SetDrawColor(display, 1);
		menu_send_int_3(display, 78 + 3, 30, c.stage4->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	case 1:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 4 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage4->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_DrawBox(display, 80, 22, 40, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 78 + 3, 30, c.stage4->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetDrawColor(display, 1);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	case 2:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 4 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage4->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage4->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRBox(display, 36, 33, 18, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 39, 40, "SET");
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	case 3:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 4 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage4->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage4->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		u8g2_DrawRBox(display, 76, 33, 15, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		break;
	default:
		break;


	}

}
void menu_prg_AdvTempCtrl_STG4_SetTemp_run(u8g2_t *display, MenuCtx m, CookingCtx c) {

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, c.stage1->temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, c.stage4->temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
	u8g2_DrawStr(display, 93, 52, "Duration");
	u8g2_DrawLine(display, 93, 54, 126, 54);
	menu_send_int_3(display, 93, 62, 0);
	u8g2_DrawStr(display, 113, 62, "MIN");

	u8g2_DrawFrame(display, 0, 0, 128, 44);
	u8g2_DrawStr(display, 28 - 1, 7, "Stage 4 Settings");
	u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
	u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
	u8g2_DrawStr(display, 80 + 3, 18, "Duration");
	u8g2_DrawFrame(display, 19, 21, 32, 11);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	menu_send_int_3(display, 22, 30, m.counter);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	menu_send_int_3(display, 78 + 3, 30, c.stage2->duration);
	u8g2_DrawStr(display, 98 + 3, 30, "Min");
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
	u8g2_DrawStr(display, 79, 40, "OK");
	u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
	u8g2_DrawStr(display, 39, 40, "SET");
	//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
	//	  u8g2_DrawStr(&display, 35, 40, "RESET");
	u8g2_SendBuffer(display);
}
void menu_prg_AdvTempCtrl_STG4_SetDur_run(u8g2_t *display, MenuCtx m, CookingCtx c){

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, c.stage1->temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, c.stage4->temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
	u8g2_DrawStr(display, 93, 52, "Duration");
	u8g2_DrawLine(display, 93, 54, 126, 54);
	menu_send_int_3(display, 93, 62, 0);
	u8g2_DrawStr(display, 113, 62, "MIN");

	u8g2_DrawFrame(display, 0, 0, 128, 44);
	u8g2_DrawStr(display, 28 - 1, 7, "Stage 4 Settings");
	u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
	u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
	u8g2_DrawStr(display, 80 + 3, 18, "Duration");
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	menu_send_int_3(display, 22, 30, c.stage4->temperature);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	u8g2_DrawFrame(display, 79, 21, 42, 11);
	u8g2_SetDrawColor(display, 1);
	menu_send_int_3(display, 78 + 3, 30, m.counter);
	u8g2_DrawStr(display, 98 + 3, 30, "Min");
	u8g2_SetDrawColor(display, 1);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
	u8g2_DrawStr(display, 79, 40, "OK");
	u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
	u8g2_DrawStr(display, 39, 40, "SET");
	//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
	//	  u8g2_DrawStr(&display, 35, 40, "RESET");
	u8g2_SendBuffer(display);

}
void menu_prg_AdvTempCtrl_STG4_SetReset_run(u8g2_t *display, MenuCtx m, CookingCtx c){

	switch(m.counter){
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 4 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage4->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage4->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRBox(display, 32, 33, 26, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 35, 40, "RESET");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 1:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 4 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage4->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage4->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRBox(display, 76, 33, 15, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 32, 33, 26, 9, 3);
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawStr(display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	}

}

void menu_prg_AdvTempCtrl_STG5_run(u8g2_t *display, MenuCtx m, CookingCtx c)
{
	switch(m.counter)
	{
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 1 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawBox(display, 20, 22, 30, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 22, 30, c.stage5->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_SetDrawColor(display, 1);
		menu_send_int_3(display, 78 + 3, 30, c.stage5->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	case 1:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 1 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage5->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_DrawBox(display, 80, 22, 40, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 78 + 3, 30, c.stage5->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetDrawColor(display, 1);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		u8g2_SendBuffer(display);
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		break;
	case 2:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 1 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage5->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage5->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRBox(display, 36, 33, 18, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 39, 40, "SET");
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_SendBuffer(display);
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		break;
	case 3:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 5 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage5->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage5->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		u8g2_DrawRBox(display, 76, 33, 15, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		break;
	default:
		break;


	}

}
void menu_prg_AdvTempCtrl_STG5_SetTemp_run(u8g2_t *display, MenuCtx m, CookingCtx c) {

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, c.stage1->temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, c.stage4->temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
	u8g2_DrawStr(display, 93, 52, "Duration");
	u8g2_DrawLine(display, 93, 54, 126, 54);
	menu_send_int_3(display, 93, 62, 0);
	u8g2_DrawStr(display, 113, 62, "MIN");

	u8g2_DrawFrame(display, 0, 0, 128, 44);
	u8g2_DrawStr(display, 28 - 1, 7, "Stage 5 Settings");
	u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
	u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
	u8g2_DrawStr(display, 80 + 3, 18, "Duration");
	u8g2_DrawFrame(display, 19, 21, 32, 11);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	menu_send_int_3(display, 22, 30, m.counter);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	menu_send_int_3(display, 78 + 3, 30, c.stage5->duration);
	u8g2_DrawStr(display, 98 + 3, 30, "Min");
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
	u8g2_DrawStr(display, 79, 40, "OK");
	u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
	u8g2_DrawStr(display, 39, 40, "SET");
	//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
	//	  u8g2_DrawStr(&display, 35, 40, "RESET");
	u8g2_SendBuffer(display);
}
void menu_prg_AdvTempCtrl_STG5_SetDur_run(u8g2_t *display, MenuCtx m, CookingCtx c){

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, c.stage1->temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, c.stage4->temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
	u8g2_DrawStr(display, 93, 52, "Duration");
	u8g2_DrawLine(display, 93, 54, 126, 54);
	menu_send_int_3(display, 93, 62, 0);
	u8g2_DrawStr(display, 113, 62, "MIN");

	u8g2_DrawFrame(display, 0, 0, 128, 44);
	u8g2_DrawStr(display, 28 - 1, 7, "Stage 5 Settings");
	u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
	u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
	u8g2_DrawStr(display, 80 + 3, 18, "Duration");
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	menu_send_int_3(display, 22, 30, c.stage5->temperature);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	u8g2_DrawFrame(display, 79, 21, 42, 11);
	u8g2_SetDrawColor(display, 1);
	menu_send_int_3(display, 78 + 3, 30, m.counter);
	u8g2_DrawStr(display, 98 + 3, 30, "Min");
	u8g2_SetDrawColor(display, 1);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
	u8g2_DrawStr(display, 79, 40, "OK");
	u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
	u8g2_DrawStr(display, 39, 40, "SET");
	//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
	//	  u8g2_DrawStr(&display, 35, 40, "RESET");
	u8g2_SendBuffer(display);

}
void menu_prg_AdvTempCtrl_STG5_SetReset_run(u8g2_t *display, MenuCtx m, CookingCtx c){

	switch(m.counter){
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 3 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage5->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage5->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRBox(display, 32, 33, 26, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 35, 40, "RESET");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 1:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 5 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage5->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage5->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRBox(display, 76, 33, 15, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 32, 33, 26, 9, 3);
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawStr(display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	}

}

void menu_prg_AdvTempCtrl_STG6_run(u8g2_t *display, MenuCtx m, CookingCtx c)
{
	switch(m.counter)
	{
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 1 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawBox(display, 20, 22, 30, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 22, 30, c.stage6->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_SetDrawColor(display, 1);
		menu_send_int_3(display, 78 + 3, 30, c.stage6->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		u8g2_SendBuffer(display);
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		break;
	case 1:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 1 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage6->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_DrawBox(display, 80, 22, 40, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 78 + 3, 30, c.stage6->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetDrawColor(display, 1);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		u8g2_SendBuffer(display);
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		break;
	case 2:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 1 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage6->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage6->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRBox(display, 36, 33, 18, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 39, 40, "SET");
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_SendBuffer(display);
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		break;
	case 3:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 6 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage6->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage6->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
		u8g2_DrawStr(display, 39, 40, "SET");
		u8g2_DrawRBox(display, 76, 33, 15, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
		//	  u8g2_DrawStr(&display, 35, 40, "RESET");
		break;
	default:
		break;


	}

}
void menu_prg_AdvTempCtrl_STG6_SetTemp_run(u8g2_t *display, MenuCtx m, CookingCtx c) {

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, c.stage1->temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, c.stage4->temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
	u8g2_DrawStr(display, 93, 52, "Duration");
	u8g2_DrawLine(display, 93, 54, 126, 54);
	menu_send_int_3(display, 93, 62, 0);
	u8g2_DrawStr(display, 113, 62, "MIN");

	u8g2_DrawFrame(display, 0, 0, 128, 44);
	u8g2_DrawStr(display, 28 - 1, 7, "Stage 6 Settings");
	u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
	u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
	u8g2_DrawStr(display, 80 + 3, 18, "Duration");
	u8g2_DrawFrame(display, 19, 21, 32, 11);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	menu_send_int_3(display, 22, 30, m.counter);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	menu_send_int_3(display, 78 + 3, 30, c.stage6->duration);
	u8g2_DrawStr(display, 98 + 3, 30, "Min");
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
	u8g2_DrawStr(display, 79, 40, "OK");
	u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
	u8g2_DrawStr(display, 39, 40, "SET");
	//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
	//	  u8g2_DrawStr(&display, 35, 40, "RESET");
	u8g2_SendBuffer(display);
}
void menu_prg_AdvTempCtrl_STG6_SetDur_run(u8g2_t *display, MenuCtx m, CookingCtx c){

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, c.stage1->temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, c.stage4->temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
	u8g2_DrawStr(display, 93, 52, "Duration");
	u8g2_DrawLine(display, 93, 54, 126, 54);
	menu_send_int_3(display, 93, 62, 0);
	u8g2_DrawStr(display, 113, 62, "MIN");

	u8g2_DrawFrame(display, 0, 0, 128, 44);
	u8g2_DrawStr(display, 28 - 1, 7, "Stage 6 Settings");
	u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
	u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
	u8g2_DrawStr(display, 80 + 3, 18, "Duration");
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	menu_send_int_3(display, 22, 30, c.stage6->temperature);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	u8g2_DrawFrame(display, 79, 21, 42, 11);
	u8g2_SetDrawColor(display, 1);
	menu_send_int_3(display, 78 + 3, 30, m.counter);
	u8g2_DrawStr(display, 98 + 3, 30, "Min");
	u8g2_SetDrawColor(display, 1);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
	u8g2_DrawStr(display, 79, 40, "OK");
	u8g2_DrawRFrame(display, 36, 33, 18, 9, 3);
	u8g2_DrawStr(display, 39, 40, "SET");
	//	  u8g2_DrawRFrame(&display, 32, 33, 26, 9, 3);
	//	  u8g2_DrawStr(&display, 35, 40, "RESET");
	u8g2_SendBuffer(display);

}
void menu_prg_AdvTempCtrl_STG6_SetReset_run(u8g2_t *display, MenuCtx m, CookingCtx c){

	switch(m.counter){
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 4 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage6->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage6->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRFrame(display, 76, 33, 15, 9, 3);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRBox(display, 32, 33, 26, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 35, 40, "RESET");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 1:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, c.stage1->temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, c.stage4->temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, c.stage2->temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, c.stage5->temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, c.stage3->temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 93, 52, "Duration");
		u8g2_DrawLine(display, 93, 54, 126, 54);
		menu_send_int_3(display, 93, 62, 0);
		u8g2_DrawStr(display, 113, 62, "MIN");

		u8g2_DrawFrame(display, 0, 0, 128, 44);
		u8g2_DrawStr(display, 28 - 1, 7, "Stage 4 Settings");
		u8g2_DrawFrame(display, 26 - 1, 0, 77, 11);
		u8g2_DrawStr(display, 13 - 3, 18, "Temperature");
		u8g2_DrawStr(display, 80 + 3, 18, "Duration");
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		menu_send_int_3(display, 22, 30, c.stage6->temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, c.stage6->duration);
		u8g2_DrawStr(display, 98 + 3, 30, "Min");
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRBox(display, 76, 33, 15, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 79, 40, "OK");
		u8g2_DrawRFrame(display, 32, 33, 26, 9, 3);
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawStr(display, 35, 40, "RESET");
		u8g2_SendBuffer(display);
		break;
	}

}

void menu_prg_AdvTempCtrl_RisingTimes_run(u8g2_t *display, MenuCtx *m, CookingCtx c, Sens t) {


	uint32_t menu_counter = m->counter;

//	if(generate_flag == FALSE){
//		u8g2_ClearBuffer(display);
//		u8g2_DrawFrame(display, 0, 0, 128, 64);
//		u8g2_SetFont(display, u8g2_font_04b_03_tr);
//		u8g2_DrawStr(display, 3, 7, "Temp Change Periods");
//		u8g2_DrawFrame(display, 120, 6, 8, 51);
//		u8g2_DrawStr(display, 122, 13, "G");
//		u8g2_DrawStr(display, 122, 19, "E");
//		u8g2_DrawStr(display, 122, 25, "N");
//		u8g2_DrawStr(display, 122, 31, "E");
//		u8g2_DrawStr(display, 122, 37, "R");
//		u8g2_DrawStr(display, 122, 43, "A");
//		u8g2_DrawStr(display, 122, 49, "T");
//		u8g2_DrawStr(display, 122, 55, "E");
//		u8g2_DrawRFrame(display, 95, 0, 23, 9, 3);
//		u8g2_DrawStr(display, 100, 7, "Exit");
//	}else{
//
//	}

		u8g2_ClearBuffer(display);
		u8g2_DrawFrame(display, 0, 0, 128, 64);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawStr(display, 3, 7, "Temp Change Periods");
		u8g2_DrawFrame(display, 120, 6, 8, 51);
		u8g2_DrawStr(display, 122, 13, "G");
		u8g2_DrawStr(display, 122, 19, "E");
		u8g2_DrawStr(display, 122, 25, "N");
		u8g2_DrawStr(display, 122, 31, "E");
		u8g2_DrawStr(display, 122, 37, "R");
		u8g2_DrawStr(display, 122, 43, "A");
		u8g2_DrawStr(display, 122, 49, "T");
		u8g2_DrawStr(display, 122, 55, "E");
		u8g2_DrawRFrame(display, 95, 0, 23, 9, 3);
		u8g2_DrawStr(display, 100, 7, "Exit");

	if (c.stage1->state == TRUE) {
		u8g2_DrawStr(display, 5, 16, "Stage 1:");
		menu_send_int_3(display, 42, 16, t.sens1_temp);
		u8g2_DrawStr(display, 58, 16, "C");
		u8g2_DrawTriangle(display, 63, 16, 63, 10, 69, 13);
		menu_send_int_3(display, 71, 16, c.stage1->temperature);
		u8g2_DrawStr(display, 86, 16, "C");
		u8g2_DrawStr(display, 90, 16, ":");
		menu_send_int_3(display, 92, 16, c.stage1->changePeriod);
		u8g2_DrawStr(display, 107, 16, "min");
	} else
		;
	if (c.stage2->state == TRUE) {
		u8g2_DrawStr(display, 5, 25, "Stage 2:");
		menu_send_int_3(display, 42, 25, c.stage1->temperature);
		u8g2_DrawStr(display, 58, 25, "C");
		u8g2_DrawTriangle(display, 63, 25, 63, 19, 69, 22);
		menu_send_int_3(display, 71, 25, c.stage2->temperature);
		u8g2_DrawStr(display, 86, 25, "C");
		u8g2_DrawStr(display, 90, 25, ":");
		menu_send_int_3(display, 92, 25, c.stage2->changePeriod);
		u8g2_DrawStr(display, 107, 25, "min");
	} else;
	if (c.stage3->state == TRUE) {
		u8g2_DrawStr(display, 5, 34, "Stage 3:");
		menu_send_int_3(display, 42, 34, c.stage2->temperature);
		u8g2_DrawStr(display, 58, 34, "C");
		u8g2_DrawTriangle(display, 63, 34, 63, 28, 69, 31);
		menu_send_int_3(display, 71, 34, c.stage3->temperature);
		u8g2_DrawStr(display, 86, 34, "C");
		u8g2_DrawStr(display, 90, 34, ":");
		menu_send_int_3(display, 92, 34, c.stage3->changePeriod);
		u8g2_DrawStr(display, 107, 34, "min");
	} else ;
	if (c.stage4->state == TRUE) {
		u8g2_DrawStr(display, 5, 43, "Stage 4:");
		menu_send_int_3(display, 42, 43, c.stage3->temperature);
		u8g2_DrawStr(display, 58, 43, "C");
		u8g2_DrawTriangle(display, 63, 43, 63, 37, 69, 40);
		menu_send_int_3(display, 71, 43, c.stage4->temperature);
		u8g2_DrawStr(display, 86, 43, "C");
		u8g2_DrawStr(display, 90, 43, ":");
		menu_send_int_3(display, 92, 43, c.stage4->changePeriod);
		u8g2_DrawStr(display, 107, 43, "min");
	} else
		;
	if (c.stage5->state == TRUE) {
		u8g2_DrawStr(display, 5, 52, "Stage 5:");
		menu_send_int_3(display, 42, 52, c.stage4->temperature);
		u8g2_DrawStr(display, 58, 52, "C");
		u8g2_DrawTriangle(display, 63, 52, 63, 46, 69, 49);
		menu_send_int_3(display, 71, 52, c.stage5->temperature);
		u8g2_DrawStr(display, 86, 52, "C");
		u8g2_DrawStr(display, 90, 52, ":");
		menu_send_int_3(display, 92, 52, c.stage5->changePeriod);
		u8g2_DrawStr(display, 107, 52, "min");
	} else
		;
	if (c.stage6->state == TRUE) {
		u8g2_DrawStr(display, 5, 61, "Stage 6:");
		menu_send_int_3(display, 42, 61, c.stage5->temperature);
		u8g2_DrawStr(display, 58, 61, "C");
		u8g2_DrawTriangle(display, 63, 61, 63, 55, 69, 58);
		menu_send_int_3(display, 71, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 86, 61, "C");
		u8g2_DrawStr(display, 90, 61, ":");
		menu_send_int_3(display, 92, 61, c.stage6->changePeriod);
		u8g2_DrawStr(display, 107, 61, "min");
	} else
		;

	switch (menu_counter) {
	case 0:
		u8g2_DrawStr(display, 5, 16, "Stage 1:");
		u8g2_DrawFrame(display, 3, 9, 38, 10);
		break;
	case 1:

		if(c.stage2->state == TRUE)
		{
			u8g2_SetDrawColor(display, 0);
			u8g2_DrawFrame(display, 3, 9, 38, 10);
			u8g2_SetDrawColor(display, 1);
			u8g2_DrawStr(display, 5, 25, "Stage 2:");
			u8g2_DrawFrame(display, 3, 18, 38, 10);
		}else m->counter = 7;

		break;
	case 2:
		if(c.stage3->state == TRUE){
			u8g2_SetDrawColor(display, 0);
			u8g2_DrawFrame(display, 3, 18, 38, 10);
			u8g2_SetDrawColor(display, 1);
			u8g2_DrawStr(display, 5, 34, "Stage 3:");
			u8g2_DrawFrame(display, 3, 27, 38, 10);
		}else m->counter = 7;

		break;
	case 3:
		if(c.stage4->state == TRUE){
			u8g2_SetDrawColor(display, 0);
			u8g2_DrawFrame(display, 3, 27, 38, 10);
			u8g2_SetDrawColor(display, 1);
			u8g2_DrawStr(display, 5, 43, "Stage 4:");
			u8g2_DrawFrame(display, 3, 36, 38, 10);
		}else {
			m->counter = 7;
		}

		break;
	case 4:
		if(c.stage5->state == TRUE){
			u8g2_SetDrawColor(display, 0);
			u8g2_DrawFrame(display, 3, 27, 38, 10);
			u8g2_SetDrawColor(display, 1);
			u8g2_DrawStr(display, 5, 52, "Stage 5:");
			u8g2_DrawFrame(display, 3, 45, 38, 10);
		}else {
			m->counter = 7;
		}

		break;
	case 5:

		if(c.stage6->state == TRUE){
			u8g2_SetDrawColor(display, 0);
			u8g2_DrawFrame(display, 3, 27, 38, 10);
			u8g2_SetDrawColor(display, 1);
			u8g2_DrawStr(display, 5, 61, "Stage 6:");
			u8g2_DrawFrame(display, 3, 54, 38, 10);
		}else m->counter = 7;
		break;
	case 6:
		if(c.stage2->state == FALSE && c.stage3->state == FALSE && c.stage4->state == FALSE && c.stage5->state == FALSE && c.stage6->state == FALSE){
			m->counter = 0;
//			break;
		}else if(c.stage3->state == FALSE && c.stage4->state == FALSE && c.stage5->state == FALSE && c.stage6->state == FALSE){
			m->counter = 1;
//			break;
		}else if(c.stage4->state == FALSE && c.stage5->state == FALSE && c.stage6->state == FALSE){
			m->counter = 2;
//			break;
		}else if(c.stage5->state == FALSE && c.stage6->state == FALSE){
			m->counter = 3;
//			break;
		}else if(c.stage6->state == FALSE){
			m->counter = 4;
//			break;
		}else;
		break;
	case 7:
		u8g2_DrawBox(display, 120, 6, 8, 51);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 122, 13, "G");
		u8g2_DrawStr(display, 122, 19, "E");
		u8g2_DrawStr(display, 122, 25, "N");
		u8g2_DrawStr(display, 122, 31, "E");
		u8g2_DrawStr(display, 122, 37, "R");
		u8g2_DrawStr(display, 122, 43, "A");
		u8g2_DrawStr(display, 122, 49, "T");
		u8g2_DrawStr(display, 122, 55, "E");
		u8g2_SetDrawColor(display, 1);
		break;
	case 8:
		u8g2_DrawRBox(display, 95, 0, 23, 9, 3);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 100, 7, "Exit");
		u8g2_SetDrawColor(display, 1);
		break;


	}

	u8g2_SendBuffer(display);
}

void menu_prg_AdvTempCtrl_RisingTimes_st1_run(u8g2_t *display, MenuCtx m, CookingCtx c){
	u8g2_ClearBuffer(display);
	u8g2_DrawFrame(display, 0, 0, 128, 64);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 3, 7, "Temp Change Periods");
	u8g2_DrawFrame(display, 120, 6, 8, 51);
	u8g2_DrawStr(display, 122, 13, "G");
	u8g2_DrawStr(display, 122, 19, "E");
	u8g2_DrawStr(display, 122, 25, "N");
	u8g2_DrawStr(display, 122, 31, "E");
	u8g2_DrawStr(display, 122, 37, "R");
	u8g2_DrawStr(display, 122, 43, "A");
	u8g2_DrawStr(display, 122, 49, "T");
	u8g2_DrawStr(display, 122, 55, "E");
	u8g2_DrawRFrame(display, 95, 0, 23, 9, 3);
	u8g2_DrawStr(display, 100, 7, "Exit");

	if (c.stage1->state == TRUE) {
		u8g2_DrawStr(display, 5, 16, "Stage 1:");
		menu_send_int_3(display, 42, 16, 60);
		u8g2_DrawStr(display, 58, 16, "C");
		u8g2_DrawTriangle(display, 63, 16, 63, 10, 69, 13);
		menu_send_int_3(display, 71, 16, c.stage1->temperature);
		u8g2_DrawStr(display, 86, 16, "C");
		u8g2_DrawStr(display, 90, 16, ":");
		menu_send_int_3(display, 92, 16, m.counter);
		u8g2_DrawStr(display, 107, 16, "min");
	} else
		;
	if (c.stage2->state == TRUE) {
		u8g2_DrawStr(display, 5, 25, "Stage 2:");
		menu_send_int_3(display, 42, 25, c.stage1->temperature);
		u8g2_DrawStr(display, 58, 25, "C");
		u8g2_DrawTriangle(display, 63, 25, 63, 19, 69, 22);
		menu_send_int_3(display, 71, 25, c.stage2->temperature);
		u8g2_DrawStr(display, 86, 25, "C");
		u8g2_DrawStr(display, 90, 25, ":");
		menu_send_int_3(display, 92, 25, c.stage2->changePeriod);
		u8g2_DrawStr(display, 107, 25, "min");
	} else;
	if (c.stage3->state == TRUE) {
		u8g2_DrawStr(display, 5, 34, "Stage 3:");
		menu_send_int_3(display, 42, 34, c.stage2->temperature);
		u8g2_DrawStr(display, 58, 34, "C");
		u8g2_DrawTriangle(display, 63, 34, 63, 28, 69, 31);
		menu_send_int_3(display, 71, 34, c.stage3->temperature);
		u8g2_DrawStr(display, 86, 34, "C");
		u8g2_DrawStr(display, 90, 34, ":");
		menu_send_int_3(display, 92, 34, c.stage3->changePeriod);
		u8g2_DrawStr(display, 107, 34, "min");
	} else ;
	if (c.stage4->state == TRUE) {
		u8g2_DrawStr(display, 5, 43, "Stage 4:");
		menu_send_int_3(display, 42, 43, c.stage3->temperature);
		u8g2_DrawStr(display, 58, 43, "C");
		u8g2_DrawTriangle(display, 63, 43, 63, 37, 69, 40);
		menu_send_int_3(display, 71, 43, c.stage4->temperature);
		u8g2_DrawStr(display, 86, 43, "C");
		u8g2_DrawStr(display, 90, 43, ":");
		menu_send_int_3(display, 92, 43, c.stage4->changePeriod);
		u8g2_DrawStr(display, 107, 43, "min");
	} else
		;
	if (c.stage5->state == TRUE) {
		u8g2_DrawStr(display, 5, 52, "Stage 5:");
		menu_send_int_3(display, 42, 52, c.stage4->temperature);
		u8g2_DrawStr(display, 58, 52, "C");
		u8g2_DrawTriangle(display, 63, 52, 63, 46, 69, 49);
		menu_send_int_3(display, 71, 52, c.stage5->temperature);
		u8g2_DrawStr(display, 86, 52, "C");
		u8g2_DrawStr(display, 90, 52, ":");
		menu_send_int_3(display, 92, 52, c.stage5->changePeriod);
		u8g2_DrawStr(display, 107, 52, "min");
	} else
		;
	if (c.stage6->state == TRUE) {
		u8g2_DrawStr(display, 5, 61, "Stage 6:");
		menu_send_int_3(display, 42, 61, c.stage5->temperature);
		u8g2_DrawStr(display, 58, 61, "C");
		u8g2_DrawTriangle(display, 63, 61, 63, 55, 69, 58);
		menu_send_int_3(display, 71, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 86, 61, "C");
		u8g2_DrawStr(display, 90, 61, ":");
		menu_send_int_3(display, 92, 61, c.stage6->changePeriod);
		u8g2_DrawStr(display, 107, 61, "min");
	} else
		;

	u8g2_DrawBox(display, 3, 9, 38, 10);
	u8g2_SetDrawColor(display, 0);
	u8g2_DrawStr(display, 5, 16, "Stage 1:");
	u8g2_SetDrawColor(display, 1);
	u8g2_SendBuffer(display);
}
void menu_prg_AdvTempCtrl_RisingTimes_st2_run(u8g2_t *display, MenuCtx m, CookingCtx c){
	u8g2_ClearBuffer(display);
	u8g2_DrawFrame(display, 0, 0, 128, 64);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 3, 7, "Temp Change Periods");
	u8g2_DrawFrame(display, 120, 6, 8, 51);
	u8g2_DrawStr(display, 122, 13, "G");
	u8g2_DrawStr(display, 122, 19, "E");
	u8g2_DrawStr(display, 122, 25, "N");
	u8g2_DrawStr(display, 122, 31, "E");
	u8g2_DrawStr(display, 122, 37, "R");
	u8g2_DrawStr(display, 122, 43, "A");
	u8g2_DrawStr(display, 122, 49, "T");
	u8g2_DrawStr(display, 122, 55, "E");
	u8g2_DrawRFrame(display, 95, 0, 23, 9, 3);
	u8g2_DrawStr(display, 100, 7, "Exit");

	if (c.stage1->state == TRUE) {
		u8g2_DrawStr(display, 5, 16, "Stage 1:");
		menu_send_int_3(display, 42, 16, 60);
		u8g2_DrawStr(display, 58, 16, "C");
		u8g2_DrawTriangle(display, 63, 16, 63, 10, 69, 13);
		menu_send_int_3(display, 71, 16, c.stage1->temperature);
		u8g2_DrawStr(display, 86, 16, "C");
		u8g2_DrawStr(display, 90, 16, ":");
		menu_send_int_3(display, 92, 16, c.stage1->changePeriod);
		u8g2_DrawStr(display, 107, 16, "min");
	} else
		;
	if (c.stage2->state == TRUE) {
		u8g2_DrawStr(display, 5, 25, "Stage 2:");
		menu_send_int_3(display, 42, 25, c.stage1->temperature);
		u8g2_DrawStr(display, 58, 25, "C");
		u8g2_DrawTriangle(display, 63, 25, 63, 19, 69, 22);
		menu_send_int_3(display, 71, 25, c.stage2->temperature);
		u8g2_DrawStr(display, 86, 25, "C");
		u8g2_DrawStr(display, 90, 25, ":");
		menu_send_int_3(display, 92, 25, m.counter);
		u8g2_DrawStr(display, 107, 25, "min");
	} else;
	if (c.stage3->state == TRUE) {
		u8g2_DrawStr(display, 5, 34, "Stage 3:");
		menu_send_int_3(display, 42, 34, c.stage2->temperature);
		u8g2_DrawStr(display, 58, 34, "C");
		u8g2_DrawTriangle(display, 63, 34, 63, 28, 69, 31);
		menu_send_int_3(display, 71, 34, c.stage3->temperature);
		u8g2_DrawStr(display, 86, 34, "C");
		u8g2_DrawStr(display, 90, 34, ":");
		menu_send_int_3(display, 92, 34, c.stage3->changePeriod);
		u8g2_DrawStr(display, 107, 34, "min");
	} else ;
	if (c.stage4->state == TRUE) {
		u8g2_DrawStr(display, 5, 43, "Stage 4:");
		menu_send_int_3(display, 42, 43, c.stage3->temperature);
		u8g2_DrawStr(display, 58, 43, "C");
		u8g2_DrawTriangle(display, 63, 43, 63, 37, 69, 40);
		menu_send_int_3(display, 71, 43, c.stage4->temperature);
		u8g2_DrawStr(display, 86, 43, "C");
		u8g2_DrawStr(display, 90, 43, ":");
		menu_send_int_3(display, 92, 43, c.stage4->changePeriod);
		u8g2_DrawStr(display, 107, 43, "min");
	} else
		;
	if (c.stage5->state == TRUE) {
		u8g2_DrawStr(display, 5, 52, "Stage 5:");
		menu_send_int_3(display, 42, 52, c.stage4->temperature);
		u8g2_DrawStr(display, 58, 52, "C");
		u8g2_DrawTriangle(display, 63, 52, 63, 46, 69, 49);
		menu_send_int_3(display, 71, 52, c.stage5->temperature);
		u8g2_DrawStr(display, 86, 52, "C");
		u8g2_DrawStr(display, 90, 52, ":");
		menu_send_int_3(display, 92, 52, c.stage5->changePeriod);
		u8g2_DrawStr(display, 107, 52, "min");
	} else
		;
	if (c.stage6->state == TRUE) {
		u8g2_DrawStr(display, 5, 61, "Stage 6:");
		menu_send_int_3(display, 42, 61, c.stage5->temperature);
		u8g2_DrawStr(display, 58, 61, "C");
		u8g2_DrawTriangle(display, 63, 61, 63, 55, 69, 58);
		menu_send_int_3(display, 71, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 86, 61, "C");
		u8g2_DrawStr(display, 90, 61, ":");
		menu_send_int_3(display, 92, 61, c.stage6->changePeriod);
		u8g2_DrawStr(display, 107, 61, "min");
	} else
		;

	u8g2_DrawBox(display, 3, 18, 38, 10);
	u8g2_SetDrawColor(display, 0);
	u8g2_DrawStr(display, 5, 25, "Stage 2:");
	u8g2_SetDrawColor(display, 1);
	u8g2_SendBuffer(display);
}
void menu_prg_AdvTempCtrl_RisingTimes_st3_run(u8g2_t *display, MenuCtx m, CookingCtx c){
	u8g2_ClearBuffer(display);
	u8g2_DrawFrame(display, 0, 0, 128, 64);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 3, 7, "Temp Change Periods");
	u8g2_DrawFrame(display, 120, 6, 8, 51);
	u8g2_DrawStr(display, 122, 13, "G");
	u8g2_DrawStr(display, 122, 19, "E");
	u8g2_DrawStr(display, 122, 25, "N");
	u8g2_DrawStr(display, 122, 31, "E");
	u8g2_DrawStr(display, 122, 37, "R");
	u8g2_DrawStr(display, 122, 43, "A");
	u8g2_DrawStr(display, 122, 49, "T");
	u8g2_DrawStr(display, 122, 55, "E");
	u8g2_DrawRFrame(display, 95, 0, 23, 9, 3);
	u8g2_DrawStr(display, 100, 7, "Exit");

	if (c.stage1->state == TRUE) {
		u8g2_DrawStr(display, 5, 16, "Stage 1:");
		menu_send_int_3(display, 42, 16, 60);
		u8g2_DrawStr(display, 58, 16, "C");
		u8g2_DrawTriangle(display, 63, 16, 63, 10, 69, 13);
		menu_send_int_3(display, 71, 16, c.stage1->temperature);
		u8g2_DrawStr(display, 86, 16, "C");
		u8g2_DrawStr(display, 90, 16, ":");
		menu_send_int_3(display, 92, 16, c.stage1->changePeriod);
		u8g2_DrawStr(display, 107, 16, "min");
	} else
		;
	if (c.stage2->state == TRUE) {
		u8g2_DrawStr(display, 5, 25, "Stage 2:");
		menu_send_int_3(display, 42, 25, c.stage1->temperature);
		u8g2_DrawStr(display, 58, 25, "C");
		u8g2_DrawTriangle(display, 63, 25, 63, 19, 69, 22);
		menu_send_int_3(display, 71, 25, c.stage2->temperature);
		u8g2_DrawStr(display, 86, 25, "C");
		u8g2_DrawStr(display, 90, 25, ":");
		menu_send_int_3(display, 92, 25, c.stage2->changePeriod);
		u8g2_DrawStr(display, 107, 25, "min");
	} else;
	if (c.stage3->state == TRUE) {
		u8g2_DrawStr(display, 5, 34, "Stage 3:");
		menu_send_int_3(display, 42, 34, c.stage2->temperature);
		u8g2_DrawStr(display, 58, 34, "C");
		u8g2_DrawTriangle(display, 63, 34, 63, 28, 69, 31);
		menu_send_int_3(display, 71, 34, c.stage3->temperature);
		u8g2_DrawStr(display, 86, 34, "C");
		u8g2_DrawStr(display, 90, 34, ":");
		menu_send_int_3(display, 92, 34, m.counter);
		u8g2_DrawStr(display, 107, 34, "min");
	} else ;
	if (c.stage4->state == TRUE) {
		u8g2_DrawStr(display, 5, 43, "Stage 4:");
		menu_send_int_3(display, 42, 43, c.stage3->temperature);
		u8g2_DrawStr(display, 58, 43, "C");
		u8g2_DrawTriangle(display, 63, 43, 63, 37, 69, 40);
		menu_send_int_3(display, 71, 43, c.stage4->temperature);
		u8g2_DrawStr(display, 86, 43, "C");
		u8g2_DrawStr(display, 90, 43, ":");
		menu_send_int_3(display, 92, 43, c.stage4->changePeriod);
		u8g2_DrawStr(display, 107, 43, "min");
	} else
		;
	if (c.stage5->state == TRUE) {
		u8g2_DrawStr(display, 5, 52, "Stage 5:");
		menu_send_int_3(display, 42, 52, c.stage4->temperature);
		u8g2_DrawStr(display, 58, 52, "C");
		u8g2_DrawTriangle(display, 63, 52, 63, 46, 69, 49);
		menu_send_int_3(display, 71, 52, c.stage5->temperature);
		u8g2_DrawStr(display, 86, 52, "C");
		u8g2_DrawStr(display, 90, 52, ":");
		menu_send_int_3(display, 92, 52, c.stage5->changePeriod);
		u8g2_DrawStr(display, 107, 52, "min");
	} else
		;
	if (c.stage6->state == TRUE) {
		u8g2_DrawStr(display, 5, 61, "Stage 6:");
		menu_send_int_3(display, 42, 61, c.stage5->temperature);
		u8g2_DrawStr(display, 58, 61, "C");
		u8g2_DrawTriangle(display, 63, 61, 63, 55, 69, 58);
		menu_send_int_3(display, 71, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 86, 61, "C");
		u8g2_DrawStr(display, 90, 61, ":");
		menu_send_int_3(display, 92, 61, c.stage6->changePeriod);
		u8g2_DrawStr(display, 107, 61, "min");
	} else
		;

	u8g2_DrawBox(display, 3, 27, 38, 10);
	u8g2_SetDrawColor(display, 0);
	u8g2_DrawStr(display, 5, 34, "Stage 3:");
	u8g2_SetDrawColor(display, 1);
	u8g2_SendBuffer(display);

}
void menu_prg_AdvTempCtrl_RisingTimes_st4_run(u8g2_t *display, MenuCtx m, CookingCtx c){
	u8g2_ClearBuffer(display);
	u8g2_DrawFrame(display, 0, 0, 128, 64);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 3, 7, "Temp Change Periods");
	u8g2_DrawFrame(display, 120, 6, 8, 51);
	u8g2_DrawStr(display, 122, 13, "G");
	u8g2_DrawStr(display, 122, 19, "E");
	u8g2_DrawStr(display, 122, 25, "N");
	u8g2_DrawStr(display, 122, 31, "E");
	u8g2_DrawStr(display, 122, 37, "R");
	u8g2_DrawStr(display, 122, 43, "A");
	u8g2_DrawStr(display, 122, 49, "T");
	u8g2_DrawStr(display, 122, 55, "E");
	u8g2_DrawRFrame(display, 95, 0, 23, 9, 3);
	u8g2_DrawStr(display, 100, 7, "Exit");

	if (c.stage1->state == TRUE) {
		u8g2_DrawStr(display, 5, 16, "Stage 1:");
		menu_send_int_3(display, 42, 16, 60);
		u8g2_DrawStr(display, 58, 16, "C");
		u8g2_DrawTriangle(display, 63, 16, 63, 10, 69, 13);
		menu_send_int_3(display, 71, 16, c.stage1->temperature);
		u8g2_DrawStr(display, 86, 16, "C");
		u8g2_DrawStr(display, 90, 16, ":");
		menu_send_int_3(display, 92, 16, c.stage1->changePeriod);
		u8g2_DrawStr(display, 107, 16, "min");
	} else
		;
	if (c.stage2->state == TRUE) {
		u8g2_DrawStr(display, 5, 25, "Stage 2:");
		menu_send_int_3(display, 42, 25, c.stage1->temperature);
		u8g2_DrawStr(display, 58, 25, "C");
		u8g2_DrawTriangle(display, 63, 25, 63, 19, 69, 22);
		menu_send_int_3(display, 71, 25, c.stage2->temperature);
		u8g2_DrawStr(display, 86, 25, "C");
		u8g2_DrawStr(display, 90, 25, ":");
		menu_send_int_3(display, 92, 25, c.stage2->changePeriod);
		u8g2_DrawStr(display, 107, 25, "min");
	} else;
	if (c.stage3->state == TRUE) {
		u8g2_DrawStr(display, 5, 34, "Stage 3:");
		menu_send_int_3(display, 42, 34, c.stage2->temperature);
		u8g2_DrawStr(display, 58, 34, "C");
		u8g2_DrawTriangle(display, 63, 34, 63, 28, 69, 31);
		menu_send_int_3(display, 71, 34, c.stage3->temperature);
		u8g2_DrawStr(display, 86, 34, "C");
		u8g2_DrawStr(display, 90, 34, ":");
		menu_send_int_3(display, 92, 34, c.stage3->changePeriod);
		u8g2_DrawStr(display, 107, 34, "min");
	} else ;
	if (c.stage4->state == TRUE) {
		u8g2_DrawStr(display, 5, 43, "Stage 4:");
		menu_send_int_3(display, 42, 43, c.stage3->temperature);
		u8g2_DrawStr(display, 58, 43, "C");
		u8g2_DrawTriangle(display, 63, 43, 63, 37, 69, 40);
		menu_send_int_3(display, 71, 43, c.stage4->temperature);
		u8g2_DrawStr(display, 86, 43, "C");
		u8g2_DrawStr(display, 90, 43, ":");
		menu_send_int_3(display, 92, 43, m.counter);
		u8g2_DrawStr(display, 107, 43, "min");
	} else
		;
	if (c.stage5->state == TRUE) {
		u8g2_DrawStr(display, 5, 52, "Stage 5:");
		menu_send_int_3(display, 42, 52, c.stage4->temperature);
		u8g2_DrawStr(display, 58, 52, "C");
		u8g2_DrawTriangle(display, 63, 52, 63, 46, 69, 49);
		menu_send_int_3(display, 71, 52, c.stage5->temperature);
		u8g2_DrawStr(display, 86, 52, "C");
		u8g2_DrawStr(display, 90, 52, ":");
		menu_send_int_3(display, 92, 52, c.stage5->changePeriod);
		u8g2_DrawStr(display, 107, 52, "min");
	} else
		;
	if (c.stage6->state == TRUE) {
		u8g2_DrawStr(display, 5, 61, "Stage 6:");
		menu_send_int_3(display, 42, 61, c.stage5->temperature);
		u8g2_DrawStr(display, 58, 61, "C");
		u8g2_DrawTriangle(display, 63, 61, 63, 55, 69, 58);
		menu_send_int_3(display, 71, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 86, 61, "C");
		u8g2_DrawStr(display, 90, 61, ":");
		menu_send_int_3(display, 92, 61, c.stage6->changePeriod);
		u8g2_DrawStr(display, 107, 61, "min");
	} else;

	u8g2_DrawBox(display, 3, 36, 38, 10);
	u8g2_SetDrawColor(display, 0);
	u8g2_DrawStr(display, 5, 43, "Stage 4:");
	u8g2_SetDrawColor(display, 1);

	u8g2_SendBuffer(display);

}
void menu_prg_AdvTempCtrl_RisingTimes_st5_run(u8g2_t *display, MenuCtx m, CookingCtx c){
	u8g2_ClearBuffer(display);
	u8g2_DrawFrame(display, 0, 0, 128, 64);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 3, 7, "Temp Change Periods");
	u8g2_DrawFrame(display, 120, 6, 8, 51);
	u8g2_DrawStr(display, 122, 13, "G");
	u8g2_DrawStr(display, 122, 19, "E");
	u8g2_DrawStr(display, 122, 25, "N");
	u8g2_DrawStr(display, 122, 31, "E");
	u8g2_DrawStr(display, 122, 37, "R");
	u8g2_DrawStr(display, 122, 43, "A");
	u8g2_DrawStr(display, 122, 49, "T");
	u8g2_DrawStr(display, 122, 55, "E");
	u8g2_DrawRFrame(display, 95, 0, 23, 9, 3);
	u8g2_DrawStr(display, 100, 7, "Exit");

	if (c.stage1->state == TRUE) {
		u8g2_DrawStr(display, 5, 16, "Stage 1:");
		menu_send_int_3(display, 42, 16, 60);
		u8g2_DrawStr(display, 58, 16, "C");
		u8g2_DrawTriangle(display, 63, 16, 63, 10, 69, 13);
		menu_send_int_3(display, 71, 16, c.stage1->temperature);
		u8g2_DrawStr(display, 86, 16, "C");
		u8g2_DrawStr(display, 90, 16, ":");
		menu_send_int_3(display, 92, 16, c.stage1->changePeriod);
		u8g2_DrawStr(display, 107, 16, "min");
	} else
		;
	if (c.stage2->state == TRUE) {
		u8g2_DrawStr(display, 5, 25, "Stage 2:");
		menu_send_int_3(display, 42, 25, c.stage1->temperature);
		u8g2_DrawStr(display, 58, 25, "C");
		u8g2_DrawTriangle(display, 63, 25, 63, 19, 69, 22);
		menu_send_int_3(display, 71, 25, c.stage2->temperature);
		u8g2_DrawStr(display, 86, 25, "C");
		u8g2_DrawStr(display, 90, 25, ":");
		menu_send_int_3(display, 92, 25, c.stage2->changePeriod);
		u8g2_DrawStr(display, 107, 25, "min");
	} else;
	if (c.stage3->state == TRUE) {
		u8g2_DrawStr(display, 5, 34, "Stage 3:");
		menu_send_int_3(display, 42, 34, c.stage2->temperature);
		u8g2_DrawStr(display, 58, 34, "C");
		u8g2_DrawTriangle(display, 63, 34, 63, 28, 69, 31);
		menu_send_int_3(display, 71, 34, c.stage3->temperature);
		u8g2_DrawStr(display, 86, 34, "C");
		u8g2_DrawStr(display, 90, 34, ":");
		menu_send_int_3(display, 92, 34, c.stage3->changePeriod);
		u8g2_DrawStr(display, 107, 34, "min");
	} else ;
	if (c.stage4->state == TRUE) {
		u8g2_DrawStr(display, 5, 43, "Stage 4:");
		menu_send_int_3(display, 42, 43, c.stage3->temperature);
		u8g2_DrawStr(display, 58, 43, "C");
		u8g2_DrawTriangle(display, 63, 43, 63, 37, 69, 40);
		menu_send_int_3(display, 71, 43, c.stage4->temperature);
		u8g2_DrawStr(display, 86, 43, "C");
		u8g2_DrawStr(display, 90, 43, ":");
		menu_send_int_3(display, 92, 43, c.stage4->changePeriod);
		u8g2_DrawStr(display, 107, 43, "min");
	} else
		;
	if (c.stage5->state == TRUE) {
		u8g2_DrawStr(display, 5, 52, "Stage 5:");
		menu_send_int_3(display, 42, 52, c.stage4->temperature);
		u8g2_DrawStr(display, 58, 52, "C");
		u8g2_DrawTriangle(display, 63, 52, 63, 46, 69, 49);
		menu_send_int_3(display, 71, 52, c.stage5->temperature);
		u8g2_DrawStr(display, 86, 52, "C");
		u8g2_DrawStr(display, 90, 52, ":");
		menu_send_int_3(display, 92, 52, m.counter);
		u8g2_DrawStr(display, 107, 52, "min");
	} else
		;
	if (c.stage6->state == TRUE) {
		u8g2_DrawStr(display, 5, 61, "Stage 6:");
		menu_send_int_3(display, 42, 61, c.stage5->temperature);
		u8g2_DrawStr(display, 58, 61, "C");
		u8g2_DrawTriangle(display, 63, 61, 63, 55, 69, 58);
		menu_send_int_3(display, 71, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 86, 61, "C");
		u8g2_DrawStr(display, 90, 61, ":");
		menu_send_int_3(display, 92, 61, c.stage6->changePeriod);
		u8g2_DrawStr(display, 107, 61, "min");
	} else;

	u8g2_DrawBox(display, 3, 45, 38, 10);
	u8g2_SetDrawColor(display, 0);
	u8g2_DrawStr(display, 5, 52, "Stage 5:");
	u8g2_SetDrawColor(display, 1);

	u8g2_SendBuffer(display);

}
void menu_prg_AdvTempCtrl_RisingTimes_st6_run(u8g2_t *display, MenuCtx m, CookingCtx c){
	u8g2_ClearBuffer(display);
	u8g2_DrawFrame(display, 0, 0, 128, 64);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 3, 7, "Temp Change Periods");
	u8g2_DrawFrame(display, 120, 6, 8, 51);
	u8g2_DrawStr(display, 122, 13, "G");
	u8g2_DrawStr(display, 122, 19, "E");
	u8g2_DrawStr(display, 122, 25, "N");
	u8g2_DrawStr(display, 122, 31, "E");
	u8g2_DrawStr(display, 122, 37, "R");
	u8g2_DrawStr(display, 122, 43, "A");
	u8g2_DrawStr(display, 122, 49, "T");
	u8g2_DrawStr(display, 122, 55, "E");
	u8g2_DrawRFrame(display, 95, 0, 23, 9, 3);
	u8g2_DrawStr(display, 100, 7, "Exit");

	if (c.stage1->state == TRUE) {
		u8g2_DrawStr(display, 5, 16, "Stage 1:");
		menu_send_int_3(display, 42, 16, 60);
		u8g2_DrawStr(display, 58, 16, "C");
		u8g2_DrawTriangle(display, 63, 16, 63, 10, 69, 13);
		menu_send_int_3(display, 71, 16, c.stage1->temperature);
		u8g2_DrawStr(display, 86, 16, "C");
		u8g2_DrawStr(display, 90, 16, ":");
		menu_send_int_3(display, 92, 16, c.stage1->changePeriod);
		u8g2_DrawStr(display, 107, 16, "min");
	} else
		;
	if (c.stage2->state == TRUE) {
		u8g2_DrawStr(display, 5, 25, "Stage 2:");
		menu_send_int_3(display, 42, 25, c.stage1->temperature);
		u8g2_DrawStr(display, 58, 25, "C");
		u8g2_DrawTriangle(display, 63, 25, 63, 19, 69, 22);
		menu_send_int_3(display, 71, 25, c.stage2->temperature);
		u8g2_DrawStr(display, 86, 25, "C");
		u8g2_DrawStr(display, 90, 25, ":");
		menu_send_int_3(display, 92, 25, c.stage2->changePeriod);
		u8g2_DrawStr(display, 107, 25, "min");
	} else;
	if (c.stage3->state == TRUE) {
		u8g2_DrawStr(display, 5, 34, "Stage 3:");
		menu_send_int_3(display, 42, 34, c.stage2->temperature);
		u8g2_DrawStr(display, 58, 34, "C");
		u8g2_DrawTriangle(display, 63, 34, 63, 28, 69, 31);
		menu_send_int_3(display, 71, 34, c.stage3->temperature);
		u8g2_DrawStr(display, 86, 34, "C");
		u8g2_DrawStr(display, 90, 34, ":");
		menu_send_int_3(display, 92, 34, c.stage3->changePeriod);
		u8g2_DrawStr(display, 107, 34, "min");
	} else ;
	if (c.stage4->state == TRUE) {
		u8g2_DrawStr(display, 5, 43, "Stage 4:");
		menu_send_int_3(display, 42, 43, c.stage3->temperature);
		u8g2_DrawStr(display, 58, 43, "C");
		u8g2_DrawTriangle(display, 63, 43, 63, 37, 69, 40);
		menu_send_int_3(display, 71, 43, c.stage4->temperature);
		u8g2_DrawStr(display, 86, 43, "C");
		u8g2_DrawStr(display, 90, 43, ":");
		menu_send_int_3(display, 92, 43, c.stage4->changePeriod);
		u8g2_DrawStr(display, 107, 43, "min");
	} else
		;
	if (c.stage5->state == TRUE) {
		u8g2_DrawStr(display, 5, 52, "Stage 5:");
		menu_send_int_3(display, 42, 52, c.stage4->temperature);
		u8g2_DrawStr(display, 58, 52, "C");
		u8g2_DrawTriangle(display, 63, 52, 63, 46, 69, 49);
		menu_send_int_3(display, 71, 52, c.stage5->temperature);
		u8g2_DrawStr(display, 86, 52, "C");
		u8g2_DrawStr(display, 90, 52, ":");
		menu_send_int_3(display, 92, 52, c.stage5->changePeriod);
		u8g2_DrawStr(display, 107, 52, "min");
	} else
		;
	if (c.stage6->state == TRUE) {
		u8g2_DrawStr(display, 5, 61, "Stage 6:");
		menu_send_int_3(display, 42, 61, c.stage5->temperature);
		u8g2_DrawStr(display, 58, 61, "C");
		u8g2_DrawTriangle(display, 63, 61, 63, 55, 69, 58);
		menu_send_int_3(display, 71, 61, c.stage6->temperature);
		u8g2_DrawStr(display, 86, 61, "C");
		u8g2_DrawStr(display, 90, 61, ":");
		menu_send_int_3(display, 92, 61, m.counter);
		u8g2_DrawStr(display, 107, 61, "min");
	} else;

	u8g2_DrawBox(display, 3, 54, 38, 10);
	u8g2_SetDrawColor(display, 0);
	u8g2_DrawStr(display, 5, 61, "Stage 6:");
	u8g2_SetDrawColor(display, 1);

	u8g2_SendBuffer(display);
}

void menu_home_run(u8g2_t *display, MenuCtx m, Sens t){

	  switch(m.counter){
	  case 0:
		  u8g2_ClearBuffer(display);
		  u8g2_DrawStr(display, 11, 7, "Composite Curing Furnace");
		  u8g2_DrawLine(display, 5, 10, 122, 10);
		  u8g2_DrawStr(display, 3, 19+4, "T1:");
		  menu_send_int_1(display, 18, 19+4, t.sens1_temp);
		  u8g2_DrawStr(display, 3, 25+4, "T2:");
		  menu_send_int_1(display, 18, 25+4, t.sens2_temp);
		  u8g2_DrawStr(display, 3, 31+4, "T3:");
		  menu_send_int_1(display, 18, 31+4, t.sens3_temp);
		  u8g2_DrawStr(display, 3, 37+4, "T4:");
		  menu_send_int_1(display, 18, 37+4, t.sens4_temp);
		  u8g2_DrawStr(display, 3, 43+4, "T5:");
		  menu_send_int_1(display, 18, 43+4, t.sens5_temp);
		  menu_send_logo(display, 85, 13);
		  menu_send_date(display, 40, 60, t.date, t.month, t.year);
		  menu_send_time(display, 3, 60, t.hour, t.minute);
		  u8g2_SetFont(display, u8g2_font_6x12_tr);
		  u8g2_DrawStr(display, 34, 25, "Programs");
		  u8g2_DrawRFrame(display, 30, 15, 55, 15, 5);
		  u8g2_DrawStr(display, 34, 44, "Settings");
		  u8g2_SetFont(display, u8g2_font_04b_03_tr);
		  u8g2_SendBuffer(display);
		  break;
	  case 1:
		  u8g2_ClearBuffer(display);
		  u8g2_SetFont(display, u8g2_font_04b_03_tr);
		  u8g2_DrawStr(display, 11, 7, "Composite Curing Furnace");
		  u8g2_DrawLine(display, 5, 10, 122, 10);
		  u8g2_DrawStr(display, 3, 19+4, "T1:");
		  menu_send_int_1(display, 18, 19+4, t.sens1_temp);
		  u8g2_DrawStr(display, 3, 25+4, "T2:");
		  menu_send_int_1(display, 18, 25+4, t.sens2_temp);
		  u8g2_DrawStr(display, 3, 31+4, "T3:");
		  menu_send_int_1(display, 18, 31+4, t.sens3_temp);
		  u8g2_DrawStr(display, 3, 37+4, "T4:");
		  menu_send_int_1(display, 18, 37+4, t.sens4_temp);
		  u8g2_DrawStr(display, 3, 43+4, "T5:");
		  menu_send_int_1(display, 18, 43+4, t.sens5_temp);
		  menu_send_logo(display, 85, 13);
		  menu_send_date(display, 40, 60, t.date, t.month, t.year);
		  menu_send_time(display, 3, 60, t.hour, t.minute);
		  u8g2_SetFont(display, u8g2_font_6x12_tr);
		  u8g2_DrawStr(display, 34, 25, "Programs");
		  u8g2_DrawStr(display, 34, 44, "Settings");
		  u8g2_SetFont(display, u8g2_font_04b_03_tr);
		  u8g2_DrawRFrame(display, 30, 34, 55, 15, 5);
		  u8g2_SendBuffer(display);
	  }





}
