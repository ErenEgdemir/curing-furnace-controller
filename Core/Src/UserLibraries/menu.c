/*
 * menu.c
 *
 *  Created on: Oct 16, 2025
 *      Author: erenegdemir
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
#define FALSE 	0
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


void menu_GenerateCookingProcess(u8g2_t *display, uint32_t *menu_flag, uint32_t *menu_counter, stage_t *s1, stage_t *s2,
		stage_t *s3, stage_t *s4, stage_t *s5, stage_t *s6, stage_t *fast) {

	total_duration = s1->totalDuration + s2->totalDuration + s3->totalDuration
			+ s4->totalDuration + s5->totalDuration + s6->totalDuration;
	fast->totalDuration = fast->changePeriod + fast->duration + fast->fallTime;

	Loading_Coefficent = 100 / (float) total_duration;
	Loading_Coefficent_R = 100 / (float) fast->totalDuration;
	u8g2_ClearBuffer(display);
	u8g2_DrawFrame(display, 14, 40, 100, 8);

	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 10, 15, "Generating Cooking");
	u8g2_DrawStr(display, 43, 25, "Process");

	if (s1->state == TRUE) {
		s1->totalDuration = s1->changePeriod + s1->duration;
		s1->tempPerMin = (float)(s1->temperature - cs1_temp) / (float)s1->changePeriod;
		s1->remTime = s1->totalDuration;

		s1->tempForMin[0] = cs1_temp;

		for (int i = 1; i < s1->changePeriod; i++) {

			s1->tempForMin[i] = s1->tempForMin[i-1] + s1->tempPerMin;

			if (s1->tempForMin[i] == s1->temperature) {

				s1->tempForMin[i] = s1->temperature;

			}
			Loading = (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;

		for (int i = s1->changePeriod - 1; i < s1->totalDuration; i++) {

			s1->tempForMin[i] = s1->temperature;
			Loading = Loading_P + (i * Loading_Coefficent);
			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;
	}

	if (s2->state == TRUE) {
		s2->totalDuration = s2->changePeriod + s2->duration;
		s2->tempPerMin = (float)(s2->temperature - s1->temperature) / (float)s2->changePeriod;
		s2->remTime = s2->totalDuration;

		s2->tempForMin[0] = s1->temperature;

		for (int i = 1; i < s2->changePeriod; i++) {

			s2->tempForMin[i] = s2->tempForMin[i-1] + s2->tempPerMin;

			if (s2->tempForMin[i] == s2->temperature) {

				s2->tempForMin[i] = s2->temperature;
			}
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;

		for (int i = s2->changePeriod - 1; i < s2->totalDuration; i++) {

			s2->tempForMin[i] = s2->temperature;
			Loading = Loading_P + (i * Loading_Coefficent);;

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;
	}

	if (s3->state == TRUE) {
		s3->totalDuration = s3->changePeriod + s3->duration;
		s3->tempPerMin = (float)(s3->temperature - s2->temperature) / (float)s3->changePeriod;
		s3->remTime = s3->totalDuration;

		s3->tempForMin[0] = s2->temperature;

		for (int i = 1; i < s3->changePeriod; i++) {

			s3->tempForMin[i] = s3->tempForMin[i-1] + s3->tempPerMin;

			if (s3->tempForMin[i] == s3->temperature) {

				s3->tempForMin[i] = s3->temperature;
			}
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;

		for (int i = s3->changePeriod - 1; i < s3->totalDuration; i++) {

			s3->tempForMin[i] = s3->temperature;
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);

		}
		Loading_P = Loading;
	}

	if (s4->state == TRUE) {
		s4->totalDuration = s4->changePeriod + s4->duration;
		s4->tempPerMin = (float)(s4->temperature - s3->temperature) / (float)s4->changePeriod;
		s4->remTime = s4->totalDuration;

		s4->tempForMin[0] = s3->temperature;

		for (int i = 1; i < s4->changePeriod; i++) {

			s4->tempForMin[i] = s4->tempForMin[i-1] +  s4->tempPerMin;

			if (s4->tempForMin[i] == s4->temperature) {

				s4->tempForMin[i] = s4->temperature;
			}
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;

		for (int i = s4->changePeriod - 1; i < s4->totalDuration; i++) {

			s4->tempForMin[i] = s4->temperature;
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;
	}

	if (s5->state == TRUE) {
		s5->totalDuration = s5->changePeriod + s1->duration;
		s5->tempPerMin = (float)(s5->temperature - s4->temperature) / (float)s5->changePeriod;
		s5->remTime = s5->totalDuration;

		s5->tempForMin[0] = s4->temperature;

		for (int i = 1; i < s5->changePeriod; i++) {

			s5->tempForMin[i] = s5->tempForMin[i-1] + s5->tempPerMin;

			if (s5->tempForMin[i] == s5->temperature) {

				s5->tempForMin[i] = s5->temperature;
			}
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;

		for (int i = s5->changePeriod - 1; i < s5->totalDuration; i++) {

			s5->tempForMin[i] = s5->temperature;
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);
		}
		Loading_P = Loading;


	}

	if (s6->state == TRUE) {
		s6->totalDuration = s6->changePeriod + s6->duration;
		s6->tempPerMin = (float)(s6->temperature - s5->temperature) / (float)s6->changePeriod;
		s6->remTime = s6->totalDuration;

		s6->tempForMin[0] = s5->temperature;

		for (int i = 1; i < s6->changePeriod; i++) {

			s6->tempForMin[i] = s6->tempForMin[i-1] + s6->tempPerMin;

			if (s6->tempForMin[i] == s6->temperature) {
				s6->tempForMin[i] = s6->temperature;
			}
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);

		}
		for (int i = s6->changePeriod - 1; i < s6->totalDuration; i++) {

			s6->tempForMin[i] = s6->temperature;
			Loading = Loading_P + (i * Loading_Coefficent);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);

		}
	}

	if (fast->state == TRUE) {
		fast->totalDuration = fast->changePeriod + fast->duration + fast->fallTime;
		fast->tempPerMin = (float)(fast->temperature - cs1_temp) / (float)fast->changePeriod;
		fast->tempPerMinDown = (float)(cs1_temp - fast->temperature) / (float)fast->fallTime;
		fast->remTime = fast->totalDuration;

		fast->tempForMin[0] = cs1_temp;

		for (int i = 1; i < fast->changePeriod; i++) {

			fast->tempForMin[i] = fast->tempForMin[i-1] + fast->tempPerMin;

			if (fast->tempForMin[i] == fast->temperature) {
				fast->tempForMin[i] = fast->temperature;
			}
			Loading = Loading_P + (i * Loading_Coefficent_R);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);

		}
		for (int i = fast->changePeriod - 1; i < fast->changePeriod + fast->duration; i++) {

			fast->tempForMin[i] = fast->temperature;
			Loading = Loading_P + (i * Loading_Coefficent_R);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);

		}
		for (int i = fast->changePeriod + fast->duration - 1; i < fast->totalDuration; i++) {

			fast->tempForMin[i] = fast->tempForMin[i-1] + fast->tempPerMinDown;

			if (fast->tempForMin[i] == fast->temperature) {
				fast->tempForMin[i] = fast->temperature;
			}
			Loading = Loading_P + (i * Loading_Coefficent_R);

			if (Loading >= 100) {
						Loading = 100;
			}

			u8g2_DrawBox(display, 14, 40, Loading, 8);
			u8g2_SendBuffer(display);

		}


	}

	if(cooking_flag == 0){
		if(fast->state == TRUE){
			*menu_flag = 3;
			cooking_flag = FASTSTART;
		}else *menu_flag = 11;

		*menu_counter = 0;
		generate_flag = TRUE;

	}else{
		*menu_flag = 3;
		*menu_counter = 0;
	}





}

void menu_cooking_run(u8g2_t *display, stage_t *stage1, stage_t *stage2,
		stage_t *stage3, stage_t *stage4, stage_t *stage5, stage_t *stage6, stage_t *fastStart){

	uint32_t remaningTime;
	u8g2_ClearBuffer(display);
	menu_send_fan(display, 113, 2);
	menu_send_flame(display, 97, 0);
	u8g2_DrawFrame(display, 101, 17, 8, 40);
	uint8_t map = 40 - (heater_pwm * 40) / 100;
	u8g2_DrawBox(display, 101, 17 + map, 8, 40 - map);
	map = 0;
	map = 40 - (cooling_state * 40) / 100;
	u8g2_DrawFrame(display, 117, 17, 8, 40);
	u8g2_DrawBox(display, 117, 17 + map, 8, 40 - map);
	map = 0;
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	menu_send_int_3(display, 98, 63, heater_pwm);
	menu_send_int_1(display, 119, 63, cooling_state);
	u8g2_DrawStr(display, 77, 6, "Max");
	u8g2_DrawStr(display, 77, 12, "PWR");
	u8g2_DrawStr(display, 74, 18, "%");
	menu_send_int_3(display, 80, 18, setting.max_pwr);
	menu_send_date(display, 37, 63, date, month, year);
	menu_send_time(display, 0, 63, hour, minute);

	switch (cooking_flag){
	case STAGE_1:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 1);
		menu_send_int_3(display, 42, 9, stage1->temperature);
		remaningTime = stage1->remTime + stage2->totalDuration + stage3->totalDuration + stage4->totalDuration +
				stage5->totalDuration + stage6->totalDuration;
		break;
	case STAGE_2:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 2);
		menu_send_int_3(display, 42, 9, stage2->temperature);
		remaningTime = stage2->remTime + stage3->totalDuration + stage4->totalDuration +
				stage5->totalDuration + stage6->totalDuration;

		break;
	case STAGE_3:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 3);
		menu_send_int_3(display, 42, 9, stage3->temperature);
		remaningTime = stage3->remTime + stage4->totalDuration + stage5->totalDuration + stage6->totalDuration;
		break;
	case STAGE_4:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 4);
		menu_send_int_3(display, 42, 9, stage4->temperature);
		remaningTime = stage4->remTime + stage5->totalDuration + stage6->totalDuration;
		break;
	case STAGE_5:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 5);
		menu_send_int_3(display, 42, 9, stage5->temperature);
		remaningTime = stage5->remTime + stage6->totalDuration;
		break;
	case STAGE_6:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 6);
		menu_send_int_3(display, 42, 9, stage6->temperature);
		remaningTime = stage6->remTime;
		break;
	case FASTSTART:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Target");
		menu_send_int_3(display, 42, 9, fastStart->temperature);
		remaningTime = fastStart->remTime;
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
	menu_send_int_3(display, 15, 19 + 4, cs1_temp);
	u8g2_DrawStr(display, 31, 23, "C");
	u8g2_DrawStr(display, 3, 25 + 4, "T2:");
	menu_send_int_3(display, 15, 25 + 4, cs2_temp);
	u8g2_DrawStr(display, 31, 29, "C");
	u8g2_DrawStr(display, 3, 31 + 4, "T3:");
	menu_send_int_3(display, 15, 31 + 4, cs3_temp);
	u8g2_DrawStr(display, 31, 35, "C");
	u8g2_DrawStr(display, 3, 37 + 4, "T4:");
	menu_send_int_3(display, 15, 37 + 4, cs4_temp);
	u8g2_DrawStr(display, 31, 41, "C");
	u8g2_DrawStr(display, 3, 43 + 4, "T5:");
	menu_send_int_3(display, 15, 43 + 4, cs5_temp);
	u8g2_DrawStr(display, 31, 47, "C");

	u8g2_SendBuffer(display);



}

void menu_cooking_run_TempChange(u8g2_t *display, uint32_t menu_counter, stage_t *stage1, stage_t *stage2,
		stage_t *stage3, stage_t *stage4, stage_t *stage5, stage_t *stage6, stage_t *fast){

	uint32_t remaningTime;
	u8g2_ClearBuffer(display);
	menu_send_fan(display, 113, 2);
	menu_send_flame(display, 97, 0);
	u8g2_DrawFrame(display, 101, 17, 8, 40);
	uint8_t map = 40 - (heater_pwm * 40) / 100;
	u8g2_DrawBox(display, 101, 17 + map, 8, 40 - map);
	map = 0;
	map = 40 - (cooling_state * 40) / 100;
	u8g2_DrawFrame(display, 117, 17, 8, 40);
	u8g2_DrawBox(display, 117, 17 + map, 8, 40 - map);
	map = 0;
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	menu_send_int_3(display, 98, 63, heater_pwm);
	menu_send_int_1(display, 119, 63, cooling_state);
	u8g2_DrawStr(display, 77, 6, "Max");
	u8g2_DrawStr(display, 77, 12, "PWR");
	u8g2_DrawStr(display, 74, 18, "%");
	menu_send_int_3(display, 80, 18, setting.max_pwr);
	menu_send_date(display, 37, 63, date, month, year);
	menu_send_time(display, 0, 63, hour, minute);

	switch (cooking_flag){
	case STAGE_1:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 1);
		menu_send_int_3(display, 42, 9, menu_counter);
		remaningTime = stage1->remTime + stage2->totalDuration + stage3->totalDuration + stage4->totalDuration +
				stage5->totalDuration + stage6->totalDuration;
		break;
	case STAGE_2:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 2);
		menu_send_int_3(display, 42, 9, menu_counter);
		remaningTime = stage2->remTime + stage3->totalDuration + stage4->totalDuration +
				stage5->totalDuration + stage6->totalDuration;

		break;
	case STAGE_3:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 3);
		menu_send_int_3(display, 42, 9, menu_counter);
		remaningTime = stage3->remTime + stage4->totalDuration + stage5->totalDuration + stage6->totalDuration;
		break;
	case STAGE_4:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 4);
		menu_send_int_3(display, 42, 9, menu_counter);
		remaningTime = stage4->remTime + stage5->totalDuration + stage6->totalDuration;
		break;
	case STAGE_5:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 5);
		menu_send_int_3(display, 42, 9, menu_counter);
		remaningTime = stage5->remTime + stage6->totalDuration;
		break;
	case STAGE_6:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 6);
		menu_send_int_3(display, 42, 9, menu_counter);
		remaningTime = stage6->remTime;
		break;
	case FASTSTART:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Target");
		menu_send_int_3(display, 42, 9, menu_counter);
		remaningTime = fast->remTime;
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
	menu_send_int_3(display, 15, 19 + 4, cs1_temp);
	u8g2_DrawStr(display, 31, 23, "C");
	u8g2_DrawStr(display, 3, 25 + 4, "T2:");
	menu_send_int_3(display, 15, 25 + 4, cs2_temp);
	u8g2_DrawStr(display, 31, 29, "C");
	u8g2_DrawStr(display, 3, 31 + 4, "T3:");
	menu_send_int_3(display, 15, 31 + 4, cs3_temp);
	u8g2_DrawStr(display, 31, 35, "C");
	u8g2_DrawStr(display, 3, 37 + 4, "T4:");
	menu_send_int_3(display, 15, 37 + 4, cs4_temp);
	u8g2_DrawStr(display, 31, 41, "C");
	u8g2_DrawStr(display, 3, 43 + 4, "T5:");
	menu_send_int_3(display, 15, 43 + 4, cs5_temp);
	u8g2_DrawStr(display, 31, 47, "C");

	u8g2_SendBuffer(display);



}

void menu_cooking_run_Duration(u8g2_t *display, uint32_t menu_counter, stage_t *stage1, stage_t *stage2,
		stage_t *stage3, stage_t *stage4, stage_t *stage5, stage_t *stage6, stage_t *fast){

	uint32_t remaningTime;
	u8g2_ClearBuffer(display);
	menu_send_fan(display, 113, 2);
	menu_send_flame(display, 97, 0);
	u8g2_DrawFrame(display, 101, 17, 8, 40);
	uint8_t map = 40 - (heater_pwm * 40) / 100;
	u8g2_DrawBox(display, 101, 17 + map, 8, 40 - map);
	map = 0;
	map = 40 - (cooling_state * 40) / 100;
	u8g2_DrawFrame(display, 117, 17, 8, 40);
	u8g2_DrawBox(display, 117, 17 + map, 8, 40 - map);
	map = 0;
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	menu_send_int_3(display, 98, 63, heater_pwm);
	menu_send_int_1(display, 119, 63, cooling_state);
	u8g2_DrawStr(display, 77, 6, "Max");
	u8g2_DrawStr(display, 77, 12, "PWR");
	u8g2_DrawStr(display, 74, 18, "%");
	menu_send_int_3(display, 80, 18, setting.max_pwr);
	menu_send_date(display, 37, 63, date, month, year);
	menu_send_time(display, 0, 63, hour, minute);

	switch (cooking_flag){
	case STAGE_1:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 1);
		menu_send_int_3(display, 42, 9, stage1->temperature);
		remaningTime = menu_counter + stage2->totalDuration + stage3->totalDuration + stage4->totalDuration +
				stage5->totalDuration + stage6->totalDuration;
		break;
	case STAGE_2:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 2);
		menu_send_int_3(display, 42, 9, stage2->temperature);
		remaningTime = menu_counter + stage3->totalDuration + stage4->totalDuration +
				stage5->totalDuration + stage6->totalDuration;

		break;
	case STAGE_3:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 3);
		menu_send_int_3(display, 42, 9, stage3->temperature);
		remaningTime = menu_counter + stage4->totalDuration + stage5->totalDuration + stage6->totalDuration;
		break;
	case STAGE_4:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 4);
		menu_send_int_3(display, 42, 9, stage4->temperature);
		remaningTime = menu_counter + stage5->totalDuration + stage6->totalDuration;
		break;
	case STAGE_5:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 5);
		menu_send_int_3(display, 42, 9, stage5->temperature);
		remaningTime = menu_counter + stage6->totalDuration;
		break;
	case STAGE_6:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 6);
		menu_send_int_3(display, 42, 9, stage6->temperature);
		remaningTime = menu_counter;
		break;
	case FASTSTART:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Target");
		menu_send_int_3(display, 42, 9, fast->temperature);
		remaningTime = menu_counter;
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
	menu_send_int_3(display, 15, 19 + 4, cs1_temp);
	u8g2_DrawStr(display, 31, 23, "C");
	u8g2_DrawStr(display, 3, 25 + 4, "T2:");
	menu_send_int_3(display, 15, 25 + 4, cs2_temp);
	u8g2_DrawStr(display, 31, 29, "C");
	u8g2_DrawStr(display, 3, 31 + 4, "T3:");
	menu_send_int_3(display, 15, 31 + 4, cs3_temp);
	u8g2_DrawStr(display, 31, 35, "C");
	u8g2_DrawStr(display, 3, 37 + 4, "T4:");
	menu_send_int_3(display, 15, 37 + 4, cs4_temp);
	u8g2_DrawStr(display, 31, 41, "C");
	u8g2_DrawStr(display, 3, 43 + 4, "T5:");
	menu_send_int_3(display, 15, 43 + 4, cs5_temp);
	u8g2_DrawStr(display, 31, 47, "C");

	u8g2_SendBuffer(display);



}

void menu_cooking_PowerChange_run(u8g2_t *display, uint32_t menu_counter, stage_t *stage1, stage_t *stage2,
		stage_t *stage3, stage_t *stage4, stage_t *stage5, stage_t *stage6, stage_t *fastStart){

	uint32_t remaningTime;
	u8g2_ClearBuffer(display);
	menu_send_fan(display, 113, 2);
	menu_send_flame(display, 97, 0);
	u8g2_DrawFrame(display, 101, 17, 8, 40);
	uint8_t map = 40 - (heater_pwm * 40) / 100;
	u8g2_DrawBox(display, 101, 17 + map, 8, 40 - map);
	map = 0;
	map = 40 - (cooling_state * 40) / 100;
	u8g2_DrawFrame(display, 117, 17, 8, 40);
	u8g2_DrawBox(display, 117, 17 + map, 8, 40 - map);
	map = 0;
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	menu_send_int_3(display, 98, 63, heater_pwm);
	menu_send_int_1(display, 119, 63, cooling_state);
	u8g2_DrawStr(display, 77, 6, "Max");
	u8g2_DrawStr(display, 77, 12, "PWR");
	u8g2_DrawStr(display, 74, 18, "%");
	menu_send_int_3(display, 80, 18, menu_counter);
	menu_send_date(display, 37, 63, date, month, year);
	menu_send_time(display, 0, 63, hour, minute);

	switch (cooking_flag){
	case STAGE_1:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 1);
		menu_send_int_3(display, 42, 9, stage1->temperature);
		remaningTime = stage1->remTime + stage2->totalDuration + stage3->totalDuration + stage4->totalDuration +
				stage5->totalDuration + stage6->totalDuration;
		break;
	case STAGE_2:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 2);
		menu_send_int_3(display, 42, 9, stage2->temperature);
		remaningTime = stage2->remTime + stage3->totalDuration + stage4->totalDuration +
				stage5->totalDuration + stage6->totalDuration;

		break;
	case STAGE_3:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 3);
		menu_send_int_3(display, 42, 9, stage3->temperature);
		remaningTime = stage3->remTime + stage4->totalDuration + stage5->totalDuration + stage6->totalDuration;
		break;
	case STAGE_4:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 4);
		menu_send_int_3(display, 42, 9, stage4->temperature);
		remaningTime = stage4->remTime + stage5->totalDuration + stage6->totalDuration;
		break;
	case STAGE_5:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 5);
		menu_send_int_3(display, 42, 9, stage5->temperature);
		remaningTime = stage5->remTime + stage6->totalDuration;
		break;
	case STAGE_6:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Stage");
		menu_send_int_1(display, 32, 9, 6);
		menu_send_int_3(display, 42, 9, stage6->temperature);
		remaningTime = stage6->remTime;
		break;
	case FASTSTART:
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 2, 9, "Target");
		menu_send_int_3(display, 42, 9, fastStart->temperature);
		remaningTime = fastStart->remTime;
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
	menu_send_int_3(display, 15, 19 + 4, cs1_temp);
	u8g2_DrawStr(display, 31, 23, "C");
	u8g2_DrawStr(display, 3, 25 + 4, "T2:");
	menu_send_int_3(display, 15, 25 + 4, cs2_temp);
	u8g2_DrawStr(display, 31, 29, "C");
	u8g2_DrawStr(display, 3, 31 + 4, "T3:");
	menu_send_int_3(display, 15, 31 + 4, cs3_temp);
	u8g2_DrawStr(display, 31, 35, "C");
	u8g2_DrawStr(display, 3, 37 + 4, "T4:");
	menu_send_int_3(display, 15, 37 + 4, cs4_temp);
	u8g2_DrawStr(display, 31, 41, "C");
	u8g2_DrawStr(display, 3, 43 + 4, "T5:");
	menu_send_int_3(display, 15, 43 + 4, cs5_temp);
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

void menu_settings_run(u8g2_t *display, uint8_t menu_counter){

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


	switch (menu_counter) {

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



void menu_settings_Pi_run(u8g2_t *display, uint32_t menu_counter, setting_t *s){

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

	switch(menu_counter){
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
void menu_settings_PiKp_run(u8g2_t *display, uint8_t menu_counter, setting_t *s)
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
	menu_send_int_3(display, 68+3, 19, menu_counter);
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
void menu_settings_PiKi_run(u8g2_t *display, uint8_t menu_counter, setting_t *s)
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
	menu_send_int_3(display, 68+3, 29, menu_counter);
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
void menu_settings_PiSlew_run(u8g2_t *display, uint8_t menu_counter, setting_t *s)
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
	menu_send_int_3(display, 68+3, 39, menu_counter);
	u8g2_DrawStr(display, 85+3, 39, "%");
//	u8g2_SetDrawColor(display, 1);
	u8g2_DrawStr(display, 30+3, 49, "Max Pwr:");
	menu_send_int_3(display, 68+3, 49, (uint32_t)(ram.max_pwr * 100));
	u8g2_DrawStr(display, 85+3, 49, "%");
	u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 55, 62 - 1, "Exit");
	u8g2_SendBuffer(display);

}
void menu_settings_PiMaxPower_run(u8g2_t *display, uint8_t menu_counter, setting_t *s)
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
	menu_send_int_3(display, 68+3, 49, menu_counter);
	u8g2_DrawStr(display, 85+3, 49, "%");
//	u8g2_SetDrawColor(display, 1);
	u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 55, 62 - 1, "Exit");
	u8g2_SendBuffer(display);

}

void menu_settings_TimeDate_run(u8g2_t *display, uint8_t menu_counter)
{
	switch (menu_counter) {
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_6x12_tr);
		u8g2_DrawStr(display, 4, 20 - 13, "Time & Date Settings");
		u8g2_DrawLine(display, 0, 22 - 13, 128, 22 - 13);
		u8g2_SetFont(display, u8g2_font_8x13_tr);
		u8g2_DrawRFrame(display, 42, 13, 18, 15, 5);
		menu_send_time(display, 43, 25, hour, minute);
		menu_send_date(display, 25, 45, date, month, year);
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
		menu_send_time(display, 43, 25, hour, minute);
		menu_send_date(display, 25, 45, date, month, year);
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
		menu_send_time(display, 43, 25, hour, minute);
		u8g2_DrawRFrame(display, 24, 33, 18, 15, 5);
		menu_send_date(display, 25, 45, date, month, year);
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
		menu_send_time(display, 43, 25, hour, minute);
		u8g2_DrawRFrame(display, 48, 33, 18, 15, 5);
		menu_send_date(display, 25, 45, date, month, year);
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
		menu_send_time(display, 43, 25, hour, minute);
		u8g2_DrawRFrame(display, 72, 33, 34, 15, 5);
		menu_send_date(display, 25, 45, date, month, year);
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
		menu_send_time(display, 43, 25, hour, minute);
		menu_send_date(display, 25, 45, date, month, year);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawRBox(display, 55 - 5, 59 - 6, 23, 11, 5);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 55, 62 - 1, "Exit");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;

	}
}
void menu_settings_TimeDateHour_run(u8g2_t *display, uint8_t menu_counter)
{

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 4, 20 - 13, "Time & Date Settings");
	u8g2_DrawLine(display, 0, 22 - 13, 128, 22 - 13);
	u8g2_SetFont(display, u8g2_font_8x13_tr);
	u8g2_DrawRBox(display, 42, 13, 18, 15, 5);
	u8g2_SetDrawColor(display, 0);
	menu_send_int_2(display, 43, 25, menu_counter);
	u8g2_SetDrawColor(display, 1);
	u8g2_DrawStr(display, 59, 25, ":");
	menu_send_int_2(display, 67, 25, minute);
	menu_send_date(display, 25, 45, date, month, year);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 55, 62 - 1, "Exit");
	u8g2_SendBuffer(display);

}
void menu_settings_TimeDateMin_run(u8g2_t *display, uint8_t menu_counter){
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 4, 20 - 13, "Time & Date Settings");
	u8g2_DrawLine(display, 0, 22 - 13, 128, 22 - 13);
	u8g2_SetFont(display, u8g2_font_8x13_tr);
	menu_send_int_2(display, 43, 25, hour);
	u8g2_DrawStr(display, 59, 25, ":");
	u8g2_DrawRBox(display, 66, 13, 18, 15, 5);
	u8g2_SetDrawColor(display, 0);
	menu_send_int_2(display, 67, 25, menu_counter);
	u8g2_SetDrawColor(display, 1);
	menu_send_date(display, 25, 45, date, month, year);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 55, 62 - 1, "Exit");
	u8g2_SendBuffer(display);
}
void menu_settings_TimeDateDay_run(u8g2_t *display, uint8_t menu_counter){
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 4, 20 - 13, "Time & Date Settings");
	u8g2_DrawLine(display, 0, 22 - 13, 128, 22 - 13);
	u8g2_SetFont(display, u8g2_font_8x13_tr);
	menu_send_int_2(display, 43, 25, hour);
	u8g2_DrawStr(display, 59, 25, ":");
	menu_send_int_2(display, 67, 25, minute);
	u8g2_DrawRBox(display, 24, 33, 18, 15, 5);
	u8g2_SetDrawColor(display, 0);
	menu_send_int_2(display, 25, 45, menu_counter);
	u8g2_SetDrawColor(display, 1);
	u8g2_DrawStr(display, 41, 45, ".");
	menu_send_int_2(display, 49, 45, month);
	u8g2_DrawStr(display, 65, 45, ".");
	menu_send_int_4(display, 73, 45, year);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 55, 62 - 1, "Exit");
	u8g2_SendBuffer(display);
}
void menu_settings_TimeDateMonth_run(u8g2_t *display, uint8_t menu_counter){
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 4, 20 - 13, "Time & Date Settings");
	u8g2_DrawLine(display, 0, 22 - 13, 128, 22 - 13);
	u8g2_SetFont(display, u8g2_font_8x13_tr);
	menu_send_int_2(display, 43, 25, hour);
	u8g2_DrawStr(display, 59, 25, ":");
	menu_send_int_2(display, 67, 25, minute);
	menu_send_int_2(display, 25, 45, date);
	u8g2_DrawStr(display, 41, 45, ".");
	u8g2_DrawRBox(display, 48, 33, 18, 15, 5);
	u8g2_SetDrawColor(display, 0);
	menu_send_int_2(display, 49, 45, menu_counter);
	u8g2_SetDrawColor(display, 1);
	u8g2_DrawStr(display, 65, 45, ".");
	menu_send_int_4(display, 73, 45, year);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 55, 62 - 1, "Exit");
	u8g2_SendBuffer(display);
}
void menu_settings_TimeDateYear_run(u8g2_t *display, uint32_t menu_counter){
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 4, 20 - 13, "Time & Date Settings");
	u8g2_DrawLine(display, 0, 22 - 13, 128, 22 - 13);
	u8g2_SetFont(display, u8g2_font_8x13_tr);
	menu_send_int_2(display, 43, 25, hour);
	u8g2_DrawStr(display, 59, 25, ":");
	menu_send_int_2(display, 67, 25, minute);
	menu_send_int_2(display, 25, 45, date);
	u8g2_DrawStr(display, 41, 45, ".");
	menu_send_int_2(display, 49, 45, month);
	u8g2_DrawStr(display, 65, 45, ".");
	u8g2_DrawRBox(display, 72, 33, 34, 15, 5);
	u8g2_SetDrawColor(display, 0);
	menu_send_int_4(display, 73, 45, menu_counter);
	u8g2_SetDrawColor(display, 1);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawRFrame(display, 55 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 55, 62 - 1, "Exit");
	u8g2_SendBuffer(display);
}


void menu_programs_run(u8g2_t *display, uint8_t menu_counter) {

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 40, 20 - 13, "Programs");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);

	switch (menu_counter) {

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

void menu_prg_FastStart_run(u8g2_t *display, uint8_t menu_counter)
{
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 34, 20 - 13, "Fast Start");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 23 + 3, 19, "Trgt Temp");
	u8g2_DrawStr(display, 64 + 3, 19, ":");
	menu_send_int_3(display, 68 + 3, 19, fastStart.temperature);
	u8g2_DrawStr(display, 85 + 3, 19, "C");
	u8g2_DrawStr(display, 23 + 3, 29, "Rise Time");
	u8g2_DrawStr(display, 64 + 3, 29, ":");
	menu_send_int_3(display, 68 + 3, 29, fastStart.changePeriod);
	u8g2_DrawStr(display, 85 + 3, 29, "Min");
	u8g2_DrawStr(display, 23 + 3, 39, "Fall Time");
	u8g2_DrawStr(display, 64 + 3, 39, ":");
	menu_send_int_3(display, 68 + 3, 39, fastStart.fallTime);
	u8g2_DrawStr(display, 85 + 3, 39, "Min");
	u8g2_DrawStr(display, 23 + 3, 49, "Cook Time");
	u8g2_DrawStr(display, 64 + 3, 49, ":");
	menu_send_int_3(display, 68 + 3, 49, fastStart.duration);
	u8g2_DrawStr(display, 85 + 3, 49, "Min");
	u8g2_DrawRFrame(display, 80 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 80, 62 - 1, "Exit");
	u8g2_DrawRFrame(display, 32 - 5, 59 - 6, 30, 11, 5);
	u8g2_DrawStr(display, 31, 62 - 1, "Start");

	switch(menu_counter){
	case 0:
		u8g2_DrawBox(display, 23, 12, 79, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 23 + 3, 19, "Trgt Temp");
		u8g2_DrawStr(display, 64 + 3, 19, ":");
		menu_send_int_3(display, 68 + 3, 19, fastStart.temperature);
		u8g2_DrawStr(display, 85 + 3, 19, "C");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 1:
		u8g2_DrawBox(display, 23, 22, 79, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 23 + 3, 29, "Rise Time");
		u8g2_DrawStr(display, 64 + 3, 29, ":");
		menu_send_int_3(display, 68 + 3, 29, fastStart.changePeriod);
		u8g2_DrawStr(display, 85 + 3, 29, "Min");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 2:
		u8g2_DrawBox(display, 23, 32, 79, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 23 + 3, 39, "Fall Time");
		u8g2_DrawStr(display, 64 + 3, 39, ":");
		menu_send_int_3(display, 68 + 3, 39, fastStart.fallTime);
		u8g2_DrawStr(display, 85 + 3, 39, "Min");
		u8g2_SetDrawColor(display, 1);
		u8g2_SendBuffer(display);
		break;
	case 3:
		u8g2_DrawBox(display, 23, 42, 79, 10);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 23 + 3, 49, "Cook Time");
		u8g2_DrawStr(display, 64 + 3, 49, ":");
		menu_send_int_3(display, 68 + 3, 49, fastStart.duration);
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

void menu_prg_FastStart_TrgtTemp_run(u8g2_t *display, uint8_t menu_counter)
{
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 34, 20 - 13, "Fast Start");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 23, 12, 79, 10);
	u8g2_DrawStr(display, 23 + 3, 19, "Trgt Temp");
	u8g2_DrawStr(display, 64 + 3, 19, ":");
	menu_send_int_3(display, 68 + 3, 19, menu_counter);
	u8g2_DrawStr(display, 85 + 3, 19, "C");
	u8g2_DrawStr(display, 23 + 3, 29, "Rise Time");
	u8g2_DrawStr(display, 64 + 3, 29, ":");
	menu_send_int_3(display, 68 + 3, 29, fastStart.changePeriod);
	u8g2_DrawStr(display, 85 + 3, 29, "Min");
	u8g2_DrawStr(display, 23 + 3, 39, "Fall Time");
	u8g2_DrawStr(display, 64 + 3, 39, ":");
	menu_send_int_3(display, 68 + 3, 39, fastStart.fallTime);
	u8g2_DrawStr(display, 85 + 3, 39, "Min");
	u8g2_DrawStr(display, 23 + 3, 49, "Cook Time");
	u8g2_DrawStr(display, 64 + 3, 49, ":");
	menu_send_int_3(display, 68 + 3, 49, fastStart.duration);
	u8g2_DrawStr(display, 85 + 3, 49, "Min");
	u8g2_DrawRFrame(display, 80 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 80, 62 - 1, "Exit");
	u8g2_DrawRFrame(display, 32 - 5, 59 - 6, 30, 11, 5);
	u8g2_DrawStr(display, 31, 62 - 1, "Start");
	u8g2_SendBuffer(display);
}

void menu_prg_FastStart_RiseTime_run(u8g2_t *display, uint8_t menu_counter)
{
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 34, 20 - 13, "Fast Start");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 23 + 3, 19, "Trgt Temp");
	u8g2_DrawStr(display, 64 + 3, 19, ":");
	menu_send_int_3(display, 68 + 3, 19, fastStart.temperature);
	u8g2_DrawStr(display, 85 + 3, 19, "C");
	u8g2_DrawFrame(display, 23, 22, 79, 10);
	u8g2_DrawStr(display, 23 + 3, 29, "Rise Time");
	u8g2_DrawStr(display, 64 + 3, 29, ":");
	menu_send_int_3(display, 68 + 3, 29, menu_counter);
	u8g2_DrawStr(display, 85 + 3, 29, "Min");
	u8g2_DrawStr(display, 23 + 3, 39, "Fall Time");
	u8g2_DrawStr(display, 64 + 3, 39, ":");
	menu_send_int_3(display, 68 + 3, 39, fastStart.fallTime);
	u8g2_DrawStr(display, 85 + 3, 39, "Min");
	u8g2_DrawStr(display, 23 + 3, 49, "Cook Time");
	u8g2_DrawStr(display, 64 + 3, 49, ":");
	menu_send_int_3(display, 68 + 3, 49, fastStart.duration);
	u8g2_DrawStr(display, 85 + 3, 49, "Min");
	u8g2_DrawRFrame(display, 80 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 80, 62 - 1, "Exit");
	u8g2_DrawRFrame(display, 32 - 5, 59 - 6, 30, 11, 5);
	u8g2_DrawStr(display, 31, 62 - 1, "Start");
	u8g2_SendBuffer(display);
}

void menu_prg_FastStart_FallTime_run(u8g2_t *display, uint8_t menu_counter)
{
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 34, 20 - 13, "Fast Start");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 23 + 3, 19, "Trgt Temp");
	u8g2_DrawStr(display, 64 + 3, 19, ":");
	menu_send_int_3(display, 68 + 3, 19, fastStart.temperature);
	u8g2_DrawStr(display, 85 + 3, 19, "C");
	u8g2_DrawStr(display, 23 + 3, 29, "Rise Time");
	u8g2_DrawStr(display, 64 + 3, 29, ":");
	menu_send_int_3(display, 68 + 3, 29, fastStart.changePeriod);
	u8g2_DrawStr(display, 85 + 3, 29, "Min");
	u8g2_DrawFrame(display, 23, 32, 79, 10);
	u8g2_DrawStr(display, 23 + 3, 39, "Fall Time");
	u8g2_DrawStr(display, 64 + 3, 39, ":");
	menu_send_int_3(display, 68 + 3, 39, menu_counter);
	u8g2_DrawStr(display, 85 + 3, 39, "Min");
	u8g2_DrawStr(display, 23 + 3, 49, "Cook Time");
	u8g2_DrawStr(display, 64 + 3, 49, ":");
	menu_send_int_3(display, 68 + 3, 49, fastStart.duration);
	u8g2_DrawStr(display, 85 + 3, 49, "Min");
	u8g2_DrawRFrame(display, 80 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 80, 62 - 1, "Exit");
	u8g2_DrawRFrame(display, 32 - 5, 59 - 6, 30, 11, 5);
	u8g2_DrawStr(display, 31, 62 - 1, "Start");
	u8g2_SendBuffer(display);
}

void menu_prg_FastStart_Duration_run(u8g2_t *display, uint8_t menu_counter)
{
	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_6x12_tr);
	u8g2_DrawStr(display, 34, 20 - 13, "Fast Start");
	u8g2_DrawLine(display, 25, 22 - 13, 100, 22 - 13);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawStr(display, 23 + 3, 19, "Trgt Temp");
	u8g2_DrawStr(display, 64 + 3, 19, ":");
	menu_send_int_3(display, 68 + 3, 19, fastStart.temperature);
	u8g2_DrawStr(display, 85 + 3, 19, "C");
	u8g2_DrawStr(display, 23 + 3, 29, "Rise Time");
	u8g2_DrawStr(display, 64 + 3, 29, ":");
	menu_send_int_3(display, 68 + 3, 29, fastStart.changePeriod);
	u8g2_DrawStr(display, 85 + 3, 29, "Min");
	u8g2_DrawStr(display, 23 + 3, 39, "Fall Time");
	u8g2_DrawStr(display, 64 + 3, 39, ":");
	menu_send_int_3(display, 68 + 3, 39, fastStart.fallTime);
	u8g2_DrawStr(display, 85 + 3, 39, "Min");
	u8g2_DrawFrame(display, 23, 42, 79, 10);
	u8g2_DrawStr(display, 23 + 3, 49, "Cook Time");
	u8g2_DrawStr(display, 64 + 3, 49, ":");
	menu_send_int_3(display, 68 + 3, 49, menu_counter);
	u8g2_DrawStr(display, 85 + 3, 49, "Min");
	u8g2_DrawRFrame(display, 80 - 5, 59 - 6, 23, 11, 5);
	u8g2_DrawStr(display, 80, 62 - 1, "Exit");
	u8g2_DrawRFrame(display, 32 - 5, 59 - 6, 30, 11, 5);
	u8g2_DrawStr(display, 31, 62 - 1, "Start");
	u8g2_SendBuffer(display);
}


void menu_prg_AdvTempCtrl_run(u8g2_t *display, uint8_t menu_counter){
	switch(menu_counter)
	{
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawBox(display, 30, 46, 31, 9);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawBox(display, 60, 46, 31, 9);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawBox(display, 0, 54, 31, 9);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawBox(display, 30, 54, 31, 9);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_SetDrawColor(display, 1);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawBox(display, 60, 54, 31, 9);
		u8g2_SetDrawColor(display, 0);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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

void menu_prg_AdvTempCtrl_STG1_run(u8g2_t *display, uint8_t menu_counter)
{
	switch(menu_counter)
	{
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage1.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_SetDrawColor(display, 1);
		menu_send_int_3(display, 78 + 3, 30, stage1.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage1.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_DrawBox(display, 80, 22, 40, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 78 + 3, 30, stage1.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage1.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage1.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage1.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage1.duration);
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
void menu_prg_AdvTempCtrl_STG1_SetTemp_run(u8g2_t *display, uint16_t menu_counter) {

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, stage1.temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, stage4.temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
	menu_send_int_3(display, 22, 30, menu_counter);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	menu_send_int_3(display, 78 + 3, 30, stage1.duration);
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
void menu_prg_AdvTempCtrl_STG1_SetDur_run(u8g2_t *display, uint16_t menu_counter){

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, stage1.temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, stage4.temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
	menu_send_int_3(display, 22, 30, stage1.temperature);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	u8g2_DrawFrame(display, 79, 21, 42, 11);
	u8g2_SetDrawColor(display, 1);
	menu_send_int_3(display, 78 + 3, 30, menu_counter);
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
void menu_prg_AdvTempCtrl_STG1_SetReset_run(u8g2_t *display, uint8_t menu_counter){

	switch(menu_counter){
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage1.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage1.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage1.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage1.duration);
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

void menu_prg_AdvTempCtrl_STG2_run(u8g2_t *display, uint8_t menu_counter)
{
	switch(menu_counter)
	{
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage2.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_SetDrawColor(display, 1);
		menu_send_int_3(display, 78 + 3, 30, stage2.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage2.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_DrawBox(display, 80, 22, 40, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 78 + 3, 30, stage2.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage2.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage2.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage2.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage2.duration);
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
void menu_prg_AdvTempCtrl_STG2_SetTemp_run(u8g2_t *display, uint16_t menu_counter) {

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, stage1.temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, stage4.temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
	menu_send_int_3(display, 22, 30, menu_counter);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	menu_send_int_3(display, 78 + 3, 30, stage2.duration);
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
void menu_prg_AdvTempCtrl_STG2_SetDur_run(u8g2_t *display, uint16_t menu_counter){

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, stage1.temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, stage4.temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
	menu_send_int_3(display, 22, 30, stage2.temperature);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	u8g2_DrawFrame(display, 79, 21, 42, 11);
	u8g2_SetDrawColor(display, 1);
	menu_send_int_3(display, 78 + 3, 30, menu_counter);
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
void menu_prg_AdvTempCtrl_STG2_SetReset_run(u8g2_t *display, uint8_t menu_counter){

	switch(menu_counter){
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage2.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage2.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage2.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage2.duration);
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

void menu_prg_AdvTempCtrl_STG3_run(u8g2_t *display, uint8_t menu_counter)
{
	switch(menu_counter)
	{
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage3.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_SetDrawColor(display, 1);
		menu_send_int_3(display, 78 + 3, 30, stage3.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage3.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_DrawBox(display, 80, 22, 40, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 78 + 3, 30, stage3.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage3.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage3.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage3.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage3.duration);
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
void menu_prg_AdvTempCtrl_STG3_SetTemp_run(u8g2_t *display, uint16_t menu_counter) {

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, stage1.temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, stage4.temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
	menu_send_int_3(display, 22, 30, menu_counter);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	menu_send_int_3(display, 78 + 3, 30, stage3.duration);
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
void menu_prg_AdvTempCtrl_STG3_SetDur_run(u8g2_t *display, uint16_t menu_counter){

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, stage1.temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, stage4.temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
	menu_send_int_3(display, 22, 30, stage3.temperature);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	u8g2_DrawFrame(display, 79, 21, 42, 11);
	u8g2_SetDrawColor(display, 1);
	menu_send_int_3(display, 78 + 3, 30, menu_counter);
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
void menu_prg_AdvTempCtrl_STG3_SetReset_run(u8g2_t *display, uint8_t menu_counter){

	switch(menu_counter){
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage3.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage3.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage3.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage3.duration);
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

void menu_prg_AdvTempCtrl_STG4_run(u8g2_t *display, uint8_t menu_counter)
{
	switch(menu_counter)
	{
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage4.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_SetDrawColor(display, 1);
		menu_send_int_3(display, 78 + 3, 30, stage4.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage4.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_DrawBox(display, 80, 22, 40, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 78 + 3, 30, stage4.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage4.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage4.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage4.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage4.duration);
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
void menu_prg_AdvTempCtrl_STG4_SetTemp_run(u8g2_t *display, uint16_t menu_counter) {

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, stage1.temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, stage4.temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
	menu_send_int_3(display, 22, 30, menu_counter);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	menu_send_int_3(display, 78 + 3, 30, stage2.duration);
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
void menu_prg_AdvTempCtrl_STG4_SetDur_run(u8g2_t *display, uint16_t menu_counter){

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, stage1.temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, stage4.temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
	menu_send_int_3(display, 22, 30, stage4.temperature);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	u8g2_DrawFrame(display, 79, 21, 42, 11);
	u8g2_SetDrawColor(display, 1);
	menu_send_int_3(display, 78 + 3, 30, menu_counter);
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
void menu_prg_AdvTempCtrl_STG4_SetReset_run(u8g2_t *display, uint8_t menu_counter){

	switch(menu_counter){
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage4.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage4.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage4.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage4.duration);
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

void menu_prg_AdvTempCtrl_STG5_run(u8g2_t *display, uint8_t menu_counter)
{
	switch(menu_counter)
	{
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage5.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_SetDrawColor(display, 1);
		menu_send_int_3(display, 78 + 3, 30, stage5.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage5.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_DrawBox(display, 80, 22, 40, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 78 + 3, 30, stage5.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage5.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage5.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage5.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage5.duration);
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
void menu_prg_AdvTempCtrl_STG5_SetTemp_run(u8g2_t *display, uint16_t menu_counter) {

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, stage1.temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, stage4.temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
	menu_send_int_3(display, 22, 30, menu_counter);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	menu_send_int_3(display, 78 + 3, 30, stage5.duration);
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
void menu_prg_AdvTempCtrl_STG5_SetDur_run(u8g2_t *display, uint16_t menu_counter){

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, stage1.temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, stage4.temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
	menu_send_int_3(display, 22, 30, stage5.temperature);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	u8g2_DrawFrame(display, 79, 21, 42, 11);
	u8g2_SetDrawColor(display, 1);
	menu_send_int_3(display, 78 + 3, 30, menu_counter);
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
void menu_prg_AdvTempCtrl_STG5_SetReset_run(u8g2_t *display, uint8_t menu_counter){

	switch(menu_counter){
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage5.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage5.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage5.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage5.duration);
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

void menu_prg_AdvTempCtrl_STG6_run(u8g2_t *display, uint8_t menu_counter)
{
	switch(menu_counter)
	{
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage6.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_SetDrawColor(display, 1);
		menu_send_int_3(display, 78 + 3, 30, stage6.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage6.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		u8g2_DrawBox(display, 80, 22, 40, 9);
		u8g2_SetDrawColor(display, 0);
		menu_send_int_3(display, 78 + 3, 30, stage6.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage6.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage6.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage6.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage6.duration);
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
void menu_prg_AdvTempCtrl_STG6_SetTemp_run(u8g2_t *display, uint16_t menu_counter) {

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, stage1.temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, stage4.temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
	menu_send_int_3(display, 22, 30, menu_counter);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	menu_send_int_3(display, 78 + 3, 30, stage6.duration);
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
void menu_prg_AdvTempCtrl_STG6_SetDur_run(u8g2_t *display, uint16_t menu_counter){

	u8g2_ClearBuffer(display);
	u8g2_SetFont(display, u8g2_font_04b_03_tr);
	u8g2_DrawFrame(display, 0, 46, 31, 9);
	u8g2_DrawStr(display, 3, 53, "T1:");
	menu_send_int_3(display, 14, 53, stage1.temperature);
	u8g2_DrawFrame(display, 0, 54, 31, 9);
	u8g2_DrawStr(display, 3, 61, "T4:");
	menu_send_int_3(display, 14, 61, stage4.temperature);
	u8g2_DrawFrame(display, 30, 46, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 53, "T2:");
	menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
	u8g2_DrawFrame(display, 30, 54, 31, 9);
	u8g2_DrawStr(display, 29 + 4, 61, "T5:");
	menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
	u8g2_DrawFrame(display, 60, 46, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 53, "T3:");
	menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
	u8g2_DrawFrame(display, 60, 54, 31, 9);
	u8g2_DrawStr(display, 55 + 8, 61, "T6:");
	menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
	menu_send_int_3(display, 22, 30, stage6.temperature);
	u8g2_DrawStr(display, 45 - 3, 30, "C");
	u8g2_DrawFrame(display, 79, 21, 42, 11);
	u8g2_SetDrawColor(display, 1);
	menu_send_int_3(display, 78 + 3, 30, menu_counter);
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
void menu_prg_AdvTempCtrl_STG6_SetReset_run(u8g2_t *display, uint8_t menu_counter){

	switch(menu_counter){
	case 0:
		u8g2_ClearBuffer(display);
		u8g2_SetFont(display, u8g2_font_04b_03_tr);
		u8g2_DrawFrame(display, 0, 46, 31, 9);
		u8g2_DrawStr(display, 3, 53, "T1:");
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage6.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage6.duration);
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
		menu_send_int_3(display, 14, 53, stage1.temperature);
		u8g2_DrawFrame(display, 0, 54, 31, 9);
		u8g2_DrawStr(display, 3, 61, "T4:");
		menu_send_int_3(display, 14, 61, stage4.temperature);
		u8g2_DrawFrame(display, 30, 46, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 53, "T2:");
		menu_send_int_3(display, 39 + 5, 53, stage2.temperature);
		u8g2_DrawFrame(display, 30, 54, 31, 9);
		u8g2_DrawStr(display, 29 + 4, 61, "T5:");
		menu_send_int_3(display, 39 + 5, 61, stage5.temperature);
		u8g2_DrawFrame(display, 60, 46, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 53, "T3:");
		menu_send_int_3(display, 65 + 9, 53, stage3.temperature);
		u8g2_DrawFrame(display, 60, 54, 31, 9);
		u8g2_DrawStr(display, 55 + 8, 61, "T6:");
		menu_send_int_3(display, 65 + 9, 61, stage6.temperature);
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
		menu_send_int_3(display, 22, 30, stage6.temperature);
		u8g2_DrawStr(display, 45 - 3, 30, "C");
		menu_send_int_3(display, 78 + 3, 30, stage6.duration);
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

void menu_prg_AdvTempCtrl_RisingTimes_run(u8g2_t *display, uint32_t *menu_counter_p) {


	uint32_t menu_counter = *menu_counter_p;

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

	if (stage1.state == TRUE) {
		u8g2_DrawStr(display, 5, 16, "Stage 1:");
		menu_send_int_3(display, 42, 16, cs1_temp);
		u8g2_DrawStr(display, 58, 16, "C");
		u8g2_DrawTriangle(display, 63, 16, 63, 10, 69, 13);
		menu_send_int_3(display, 71, 16, stage1.temperature);
		u8g2_DrawStr(display, 86, 16, "C");
		u8g2_DrawStr(display, 90, 16, ":");
		menu_send_int_3(display, 92, 16, stage1.changePeriod);
		u8g2_DrawStr(display, 107, 16, "min");
	} else
		;
	if (stage2.state == TRUE) {
		u8g2_DrawStr(display, 5, 25, "Stage 2:");
		menu_send_int_3(display, 42, 25, stage1.temperature);
		u8g2_DrawStr(display, 58, 25, "C");
		u8g2_DrawTriangle(display, 63, 25, 63, 19, 69, 22);
		menu_send_int_3(display, 71, 25, stage2.temperature);
		u8g2_DrawStr(display, 86, 25, "C");
		u8g2_DrawStr(display, 90, 25, ":");
		menu_send_int_3(display, 92, 25, stage2.changePeriod);
		u8g2_DrawStr(display, 107, 25, "min");
	} else;
	if (stage3.state == TRUE) {
		u8g2_DrawStr(display, 5, 34, "Stage 3:");
		menu_send_int_3(display, 42, 34, stage2.temperature);
		u8g2_DrawStr(display, 58, 34, "C");
		u8g2_DrawTriangle(display, 63, 34, 63, 28, 69, 31);
		menu_send_int_3(display, 71, 34, stage3.temperature);
		u8g2_DrawStr(display, 86, 34, "C");
		u8g2_DrawStr(display, 90, 34, ":");
		menu_send_int_3(display, 92, 34, stage3.changePeriod);
		u8g2_DrawStr(display, 107, 34, "min");
	} else ;
	if (stage4.state == TRUE) {
		u8g2_DrawStr(display, 5, 43, "Stage 4:");
		menu_send_int_3(display, 42, 43, stage3.temperature);
		u8g2_DrawStr(display, 58, 43, "C");
		u8g2_DrawTriangle(display, 63, 43, 63, 37, 69, 40);
		menu_send_int_3(display, 71, 43, stage4.temperature);
		u8g2_DrawStr(display, 86, 43, "C");
		u8g2_DrawStr(display, 90, 43, ":");
		menu_send_int_3(display, 92, 43, stage4.changePeriod);
		u8g2_DrawStr(display, 107, 43, "min");
	} else
		;
	if (stage5.state == TRUE) {
		u8g2_DrawStr(display, 5, 52, "Stage 5:");
		menu_send_int_3(display, 42, 52, stage4.temperature);
		u8g2_DrawStr(display, 58, 52, "C");
		u8g2_DrawTriangle(display, 63, 52, 63, 46, 69, 49);
		menu_send_int_3(display, 71, 52, stage5.temperature);
		u8g2_DrawStr(display, 86, 52, "C");
		u8g2_DrawStr(display, 90, 52, ":");
		menu_send_int_3(display, 92, 52, stage5.changePeriod);
		u8g2_DrawStr(display, 107, 52, "min");
	} else
		;
	if (stage6.state == TRUE) {
		u8g2_DrawStr(display, 5, 61, "Stage 6:");
		menu_send_int_3(display, 42, 61, stage5.temperature);
		u8g2_DrawStr(display, 58, 61, "C");
		u8g2_DrawTriangle(display, 63, 61, 63, 55, 69, 58);
		menu_send_int_3(display, 71, 61, stage6.temperature);
		u8g2_DrawStr(display, 86, 61, "C");
		u8g2_DrawStr(display, 90, 61, ":");
		menu_send_int_3(display, 92, 61, stage6.changePeriod);
		u8g2_DrawStr(display, 107, 61, "min");
	} else
		;

	switch (menu_counter) {
	case 0:
		u8g2_DrawStr(display, 5, 16, "Stage 1:");
		u8g2_DrawFrame(display, 3, 9, 38, 10);
		break;
	case 1:

		if(stage2.state == TRUE)
		{
			u8g2_SetDrawColor(display, 0);
			u8g2_DrawFrame(display, 3, 9, 38, 10);
			u8g2_SetDrawColor(display, 1);
			u8g2_DrawStr(display, 5, 25, "Stage 2:");
			u8g2_DrawFrame(display, 3, 18, 38, 10);
		}else *menu_counter_p = 7;

		break;
	case 2:
		if(stage3.state == TRUE){
			u8g2_SetDrawColor(display, 0);
			u8g2_DrawFrame(display, 3, 18, 38, 10);
			u8g2_SetDrawColor(display, 1);
			u8g2_DrawStr(display, 5, 34, "Stage 3:");
			u8g2_DrawFrame(display, 3, 27, 38, 10);
		}else *menu_counter_p = 7;

		break;
	case 3:
		if(stage4.state == TRUE){
			u8g2_SetDrawColor(display, 0);
			u8g2_DrawFrame(display, 3, 27, 38, 10);
			u8g2_SetDrawColor(display, 1);
			u8g2_DrawStr(display, 5, 43, "Stage 4:");
			u8g2_DrawFrame(display, 3, 36, 38, 10);
		}else {
			*menu_counter_p = 7;
		}

		break;
	case 4:
		if(stage5.state == TRUE){
			u8g2_SetDrawColor(display, 0);
			u8g2_DrawFrame(display, 3, 27, 38, 10);
			u8g2_SetDrawColor(display, 1);
			u8g2_DrawStr(display, 5, 52, "Stage 5:");
			u8g2_DrawFrame(display, 3, 45, 38, 10);
		}else {
			*menu_counter_p = 7;
		}

		break;
	case 5:

		if(stage6.state == TRUE){
			u8g2_SetDrawColor(display, 0);
			u8g2_DrawFrame(display, 3, 27, 38, 10);
			u8g2_SetDrawColor(display, 1);
			u8g2_DrawStr(display, 5, 61, "Stage 6:");
			u8g2_DrawFrame(display, 3, 54, 38, 10);
		}else *menu_counter_p = 7;
		break;
	case 6:
		if(stage2.state == FALSE && stage3.state == FALSE && stage4.state == FALSE && stage5.state == FALSE && stage6.state == FALSE){
			*menu_counter_p = 0;
//			break;
		}else if(stage3.state == FALSE && stage4.state == FALSE && stage5.state == FALSE && stage6.state == FALSE){
			*menu_counter_p = 1;
//			break;
		}else if(stage4.state == FALSE && stage5.state == FALSE && stage6.state == FALSE){
			*menu_counter_p = 2;
//			break;
		}else if(stage5.state == FALSE && stage6.state == FALSE){
			*menu_counter_p = 3;
//			break;
		}else if(stage6.state == FALSE){
			*menu_counter_p = 4;
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

void menu_prg_AdvTempCtrl_RisingTimes_st1_run(u8g2_t *display, uint32_t menu_counter){
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

	if (stage1.state == TRUE) {
		u8g2_DrawStr(display, 5, 16, "Stage 1:");
		menu_send_int_3(display, 42, 16, cs1_temp);
		u8g2_DrawStr(display, 58, 16, "C");
		u8g2_DrawTriangle(display, 63, 16, 63, 10, 69, 13);
		menu_send_int_3(display, 71, 16, stage1.temperature);
		u8g2_DrawStr(display, 86, 16, "C");
		u8g2_DrawStr(display, 90, 16, ":");
		menu_send_int_3(display, 92, 16, menu_counter);
		u8g2_DrawStr(display, 107, 16, "min");
	} else
		;
	if (stage2.state == TRUE) {
		u8g2_DrawStr(display, 5, 25, "Stage 2:");
		menu_send_int_3(display, 42, 25, stage1.temperature);
		u8g2_DrawStr(display, 58, 25, "C");
		u8g2_DrawTriangle(display, 63, 25, 63, 19, 69, 22);
		menu_send_int_3(display, 71, 25, stage2.temperature);
		u8g2_DrawStr(display, 86, 25, "C");
		u8g2_DrawStr(display, 90, 25, ":");
		menu_send_int_3(display, 92, 25, stage2.changePeriod);
		u8g2_DrawStr(display, 107, 25, "min");
	} else;
	if (stage3.state == TRUE) {
		u8g2_DrawStr(display, 5, 34, "Stage 3:");
		menu_send_int_3(display, 42, 34, stage2.temperature);
		u8g2_DrawStr(display, 58, 34, "C");
		u8g2_DrawTriangle(display, 63, 34, 63, 28, 69, 31);
		menu_send_int_3(display, 71, 34, stage3.temperature);
		u8g2_DrawStr(display, 86, 34, "C");
		u8g2_DrawStr(display, 90, 34, ":");
		menu_send_int_3(display, 92, 34, stage3.changePeriod);
		u8g2_DrawStr(display, 107, 34, "min");
	} else ;
	if (stage4.state == TRUE) {
		u8g2_DrawStr(display, 5, 43, "Stage 4:");
		menu_send_int_3(display, 42, 43, stage3.temperature);
		u8g2_DrawStr(display, 58, 43, "C");
		u8g2_DrawTriangle(display, 63, 43, 63, 37, 69, 40);
		menu_send_int_3(display, 71, 43, stage4.temperature);
		u8g2_DrawStr(display, 86, 43, "C");
		u8g2_DrawStr(display, 90, 43, ":");
		menu_send_int_3(display, 92, 43, stage4.changePeriod);
		u8g2_DrawStr(display, 107, 43, "min");
	} else
		;
	if (stage5.state == TRUE) {
		u8g2_DrawStr(display, 5, 52, "Stage 5:");
		menu_send_int_3(display, 42, 52, stage4.temperature);
		u8g2_DrawStr(display, 58, 52, "C");
		u8g2_DrawTriangle(display, 63, 52, 63, 46, 69, 49);
		menu_send_int_3(display, 71, 52, stage5.temperature);
		u8g2_DrawStr(display, 86, 52, "C");
		u8g2_DrawStr(display, 90, 52, ":");
		menu_send_int_3(display, 92, 52, stage5.changePeriod);
		u8g2_DrawStr(display, 107, 52, "min");
	} else
		;
	if (stage6.state == TRUE) {
		u8g2_DrawStr(display, 5, 61, "Stage 6:");
		menu_send_int_3(display, 42, 61, stage5.temperature);
		u8g2_DrawStr(display, 58, 61, "C");
		u8g2_DrawTriangle(display, 63, 61, 63, 55, 69, 58);
		menu_send_int_3(display, 71, 61, stage6.temperature);
		u8g2_DrawStr(display, 86, 61, "C");
		u8g2_DrawStr(display, 90, 61, ":");
		menu_send_int_3(display, 92, 61, stage6.changePeriod);
		u8g2_DrawStr(display, 107, 61, "min");
	} else
		;

	u8g2_DrawBox(display, 3, 9, 38, 10);
	u8g2_SetDrawColor(display, 0);
	u8g2_DrawStr(display, 5, 16, "Stage 1:");
	u8g2_SetDrawColor(display, 1);
	u8g2_SendBuffer(display);
}
void menu_prg_AdvTempCtrl_RisingTimes_st2_run(u8g2_t *display, uint32_t menu_counter){
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

	if (stage1.state == TRUE) {
		u8g2_DrawStr(display, 5, 16, "Stage 1:");
		menu_send_int_3(display, 42, 16, cs1_temp);
		u8g2_DrawStr(display, 58, 16, "C");
		u8g2_DrawTriangle(display, 63, 16, 63, 10, 69, 13);
		menu_send_int_3(display, 71, 16, stage1.temperature);
		u8g2_DrawStr(display, 86, 16, "C");
		u8g2_DrawStr(display, 90, 16, ":");
		menu_send_int_3(display, 92, 16, stage1.changePeriod);
		u8g2_DrawStr(display, 107, 16, "min");
	} else
		;
	if (stage2.state == TRUE) {
		u8g2_DrawStr(display, 5, 25, "Stage 2:");
		menu_send_int_3(display, 42, 25, stage1.temperature);
		u8g2_DrawStr(display, 58, 25, "C");
		u8g2_DrawTriangle(display, 63, 25, 63, 19, 69, 22);
		menu_send_int_3(display, 71, 25, stage2.temperature);
		u8g2_DrawStr(display, 86, 25, "C");
		u8g2_DrawStr(display, 90, 25, ":");
		menu_send_int_3(display, 92, 25, menu_counter);
		u8g2_DrawStr(display, 107, 25, "min");
	} else;
	if (stage3.state == TRUE) {
		u8g2_DrawStr(display, 5, 34, "Stage 3:");
		menu_send_int_3(display, 42, 34, stage2.temperature);
		u8g2_DrawStr(display, 58, 34, "C");
		u8g2_DrawTriangle(display, 63, 34, 63, 28, 69, 31);
		menu_send_int_3(display, 71, 34, stage3.temperature);
		u8g2_DrawStr(display, 86, 34, "C");
		u8g2_DrawStr(display, 90, 34, ":");
		menu_send_int_3(display, 92, 34, stage3.changePeriod);
		u8g2_DrawStr(display, 107, 34, "min");
	} else ;
	if (stage4.state == TRUE) {
		u8g2_DrawStr(display, 5, 43, "Stage 4:");
		menu_send_int_3(display, 42, 43, stage3.temperature);
		u8g2_DrawStr(display, 58, 43, "C");
		u8g2_DrawTriangle(display, 63, 43, 63, 37, 69, 40);
		menu_send_int_3(display, 71, 43, stage4.temperature);
		u8g2_DrawStr(display, 86, 43, "C");
		u8g2_DrawStr(display, 90, 43, ":");
		menu_send_int_3(display, 92, 43, stage4.changePeriod);
		u8g2_DrawStr(display, 107, 43, "min");
	} else
		;
	if (stage5.state == TRUE) {
		u8g2_DrawStr(display, 5, 52, "Stage 5:");
		menu_send_int_3(display, 42, 52, stage4.temperature);
		u8g2_DrawStr(display, 58, 52, "C");
		u8g2_DrawTriangle(display, 63, 52, 63, 46, 69, 49);
		menu_send_int_3(display, 71, 52, stage5.temperature);
		u8g2_DrawStr(display, 86, 52, "C");
		u8g2_DrawStr(display, 90, 52, ":");
		menu_send_int_3(display, 92, 52, stage5.changePeriod);
		u8g2_DrawStr(display, 107, 52, "min");
	} else
		;
	if (stage6.state == TRUE) {
		u8g2_DrawStr(display, 5, 61, "Stage 6:");
		menu_send_int_3(display, 42, 61, stage5.temperature);
		u8g2_DrawStr(display, 58, 61, "C");
		u8g2_DrawTriangle(display, 63, 61, 63, 55, 69, 58);
		menu_send_int_3(display, 71, 61, stage6.temperature);
		u8g2_DrawStr(display, 86, 61, "C");
		u8g2_DrawStr(display, 90, 61, ":");
		menu_send_int_3(display, 92, 61, stage6.changePeriod);
		u8g2_DrawStr(display, 107, 61, "min");
	} else
		;

	u8g2_DrawBox(display, 3, 18, 38, 10);
	u8g2_SetDrawColor(display, 0);
	u8g2_DrawStr(display, 5, 25, "Stage 2:");
	u8g2_SetDrawColor(display, 1);
	u8g2_SendBuffer(display);
}
void menu_prg_AdvTempCtrl_RisingTimes_st3_run(u8g2_t *display, uint32_t menu_counter){
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

	if (stage1.state == TRUE) {
		u8g2_DrawStr(display, 5, 16, "Stage 1:");
		menu_send_int_3(display, 42, 16, cs1_temp);
		u8g2_DrawStr(display, 58, 16, "C");
		u8g2_DrawTriangle(display, 63, 16, 63, 10, 69, 13);
		menu_send_int_3(display, 71, 16, stage1.temperature);
		u8g2_DrawStr(display, 86, 16, "C");
		u8g2_DrawStr(display, 90, 16, ":");
		menu_send_int_3(display, 92, 16, stage1.changePeriod);
		u8g2_DrawStr(display, 107, 16, "min");
	} else
		;
	if (stage2.state == TRUE) {
		u8g2_DrawStr(display, 5, 25, "Stage 2:");
		menu_send_int_3(display, 42, 25, stage1.temperature);
		u8g2_DrawStr(display, 58, 25, "C");
		u8g2_DrawTriangle(display, 63, 25, 63, 19, 69, 22);
		menu_send_int_3(display, 71, 25, stage2.temperature);
		u8g2_DrawStr(display, 86, 25, "C");
		u8g2_DrawStr(display, 90, 25, ":");
		menu_send_int_3(display, 92, 25, stage2.changePeriod);
		u8g2_DrawStr(display, 107, 25, "min");
	} else;
	if (stage3.state == TRUE) {
		u8g2_DrawStr(display, 5, 34, "Stage 3:");
		menu_send_int_3(display, 42, 34, stage2.temperature);
		u8g2_DrawStr(display, 58, 34, "C");
		u8g2_DrawTriangle(display, 63, 34, 63, 28, 69, 31);
		menu_send_int_3(display, 71, 34, stage3.temperature);
		u8g2_DrawStr(display, 86, 34, "C");
		u8g2_DrawStr(display, 90, 34, ":");
		menu_send_int_3(display, 92, 34, menu_counter);
		u8g2_DrawStr(display, 107, 34, "min");
	} else ;
	if (stage4.state == TRUE) {
		u8g2_DrawStr(display, 5, 43, "Stage 4:");
		menu_send_int_3(display, 42, 43, stage3.temperature);
		u8g2_DrawStr(display, 58, 43, "C");
		u8g2_DrawTriangle(display, 63, 43, 63, 37, 69, 40);
		menu_send_int_3(display, 71, 43, stage4.temperature);
		u8g2_DrawStr(display, 86, 43, "C");
		u8g2_DrawStr(display, 90, 43, ":");
		menu_send_int_3(display, 92, 43, stage4.changePeriod);
		u8g2_DrawStr(display, 107, 43, "min");
	} else
		;
	if (stage5.state == TRUE) {
		u8g2_DrawStr(display, 5, 52, "Stage 5:");
		menu_send_int_3(display, 42, 52, stage4.temperature);
		u8g2_DrawStr(display, 58, 52, "C");
		u8g2_DrawTriangle(display, 63, 52, 63, 46, 69, 49);
		menu_send_int_3(display, 71, 52, stage5.temperature);
		u8g2_DrawStr(display, 86, 52, "C");
		u8g2_DrawStr(display, 90, 52, ":");
		menu_send_int_3(display, 92, 52, stage5.changePeriod);
		u8g2_DrawStr(display, 107, 52, "min");
	} else
		;
	if (stage6.state == TRUE) {
		u8g2_DrawStr(display, 5, 61, "Stage 6:");
		menu_send_int_3(display, 42, 61, stage5.temperature);
		u8g2_DrawStr(display, 58, 61, "C");
		u8g2_DrawTriangle(display, 63, 61, 63, 55, 69, 58);
		menu_send_int_3(display, 71, 61, stage6.temperature);
		u8g2_DrawStr(display, 86, 61, "C");
		u8g2_DrawStr(display, 90, 61, ":");
		menu_send_int_3(display, 92, 61, stage6.changePeriod);
		u8g2_DrawStr(display, 107, 61, "min");
	} else
		;

	u8g2_DrawBox(display, 3, 27, 38, 10);
	u8g2_SetDrawColor(display, 0);
	u8g2_DrawStr(display, 5, 34, "Stage 3:");
	u8g2_SetDrawColor(display, 1);
	u8g2_SendBuffer(display);

}
void menu_prg_AdvTempCtrl_RisingTimes_st4_run(u8g2_t *display, uint32_t menu_counter){
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

	if (stage1.state == TRUE) {
		u8g2_DrawStr(display, 5, 16, "Stage 1:");
		menu_send_int_3(display, 42, 16, cs1_temp);
		u8g2_DrawStr(display, 58, 16, "C");
		u8g2_DrawTriangle(display, 63, 16, 63, 10, 69, 13);
		menu_send_int_3(display, 71, 16, stage1.temperature);
		u8g2_DrawStr(display, 86, 16, "C");
		u8g2_DrawStr(display, 90, 16, ":");
		menu_send_int_3(display, 92, 16, stage1.changePeriod);
		u8g2_DrawStr(display, 107, 16, "min");
	} else
		;
	if (stage2.state == TRUE) {
		u8g2_DrawStr(display, 5, 25, "Stage 2:");
		menu_send_int_3(display, 42, 25, stage1.temperature);
		u8g2_DrawStr(display, 58, 25, "C");
		u8g2_DrawTriangle(display, 63, 25, 63, 19, 69, 22);
		menu_send_int_3(display, 71, 25, stage2.temperature);
		u8g2_DrawStr(display, 86, 25, "C");
		u8g2_DrawStr(display, 90, 25, ":");
		menu_send_int_3(display, 92, 25, stage2.changePeriod);
		u8g2_DrawStr(display, 107, 25, "min");
	} else;
	if (stage3.state == TRUE) {
		u8g2_DrawStr(display, 5, 34, "Stage 3:");
		menu_send_int_3(display, 42, 34, stage2.temperature);
		u8g2_DrawStr(display, 58, 34, "C");
		u8g2_DrawTriangle(display, 63, 34, 63, 28, 69, 31);
		menu_send_int_3(display, 71, 34, stage3.temperature);
		u8g2_DrawStr(display, 86, 34, "C");
		u8g2_DrawStr(display, 90, 34, ":");
		menu_send_int_3(display, 92, 34, stage3.changePeriod);
		u8g2_DrawStr(display, 107, 34, "min");
	} else ;
	if (stage4.state == TRUE) {
		u8g2_DrawStr(display, 5, 43, "Stage 4:");
		menu_send_int_3(display, 42, 43, stage3.temperature);
		u8g2_DrawStr(display, 58, 43, "C");
		u8g2_DrawTriangle(display, 63, 43, 63, 37, 69, 40);
		menu_send_int_3(display, 71, 43, stage4.temperature);
		u8g2_DrawStr(display, 86, 43, "C");
		u8g2_DrawStr(display, 90, 43, ":");
		menu_send_int_3(display, 92, 43, menu_counter);
		u8g2_DrawStr(display, 107, 43, "min");
	} else
		;
	if (stage5.state == TRUE) {
		u8g2_DrawStr(display, 5, 52, "Stage 5:");
		menu_send_int_3(display, 42, 52, stage4.temperature);
		u8g2_DrawStr(display, 58, 52, "C");
		u8g2_DrawTriangle(display, 63, 52, 63, 46, 69, 49);
		menu_send_int_3(display, 71, 52, stage5.temperature);
		u8g2_DrawStr(display, 86, 52, "C");
		u8g2_DrawStr(display, 90, 52, ":");
		menu_send_int_3(display, 92, 52, stage5.changePeriod);
		u8g2_DrawStr(display, 107, 52, "min");
	} else
		;
	if (stage6.state == TRUE) {
		u8g2_DrawStr(display, 5, 61, "Stage 6:");
		menu_send_int_3(display, 42, 61, stage5.temperature);
		u8g2_DrawStr(display, 58, 61, "C");
		u8g2_DrawTriangle(display, 63, 61, 63, 55, 69, 58);
		menu_send_int_3(display, 71, 61, stage6.temperature);
		u8g2_DrawStr(display, 86, 61, "C");
		u8g2_DrawStr(display, 90, 61, ":");
		menu_send_int_3(display, 92, 61, stage6.changePeriod);
		u8g2_DrawStr(display, 107, 61, "min");
	} else;

	u8g2_DrawBox(display, 3, 36, 38, 10);
	u8g2_SetDrawColor(display, 0);
	u8g2_DrawStr(display, 5, 43, "Stage 4:");
	u8g2_SetDrawColor(display, 1);

	u8g2_SendBuffer(display);

}
void menu_prg_AdvTempCtrl_RisingTimes_st5_run(u8g2_t *display, uint32_t menu_counter){
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

	if (stage1.state == TRUE) {
		u8g2_DrawStr(display, 5, 16, "Stage 1:");
		menu_send_int_3(display, 42, 16, cs1_temp);
		u8g2_DrawStr(display, 58, 16, "C");
		u8g2_DrawTriangle(display, 63, 16, 63, 10, 69, 13);
		menu_send_int_3(display, 71, 16, stage1.temperature);
		u8g2_DrawStr(display, 86, 16, "C");
		u8g2_DrawStr(display, 90, 16, ":");
		menu_send_int_3(display, 92, 16, stage1.changePeriod);
		u8g2_DrawStr(display, 107, 16, "min");
	} else
		;
	if (stage2.state == TRUE) {
		u8g2_DrawStr(display, 5, 25, "Stage 2:");
		menu_send_int_3(display, 42, 25, stage1.temperature);
		u8g2_DrawStr(display, 58, 25, "C");
		u8g2_DrawTriangle(display, 63, 25, 63, 19, 69, 22);
		menu_send_int_3(display, 71, 25, stage2.temperature);
		u8g2_DrawStr(display, 86, 25, "C");
		u8g2_DrawStr(display, 90, 25, ":");
		menu_send_int_3(display, 92, 25, stage2.changePeriod);
		u8g2_DrawStr(display, 107, 25, "min");
	} else;
	if (stage3.state == TRUE) {
		u8g2_DrawStr(display, 5, 34, "Stage 3:");
		menu_send_int_3(display, 42, 34, stage2.temperature);
		u8g2_DrawStr(display, 58, 34, "C");
		u8g2_DrawTriangle(display, 63, 34, 63, 28, 69, 31);
		menu_send_int_3(display, 71, 34, stage3.temperature);
		u8g2_DrawStr(display, 86, 34, "C");
		u8g2_DrawStr(display, 90, 34, ":");
		menu_send_int_3(display, 92, 34, stage3.changePeriod);
		u8g2_DrawStr(display, 107, 34, "min");
	} else ;
	if (stage4.state == TRUE) {
		u8g2_DrawStr(display, 5, 43, "Stage 4:");
		menu_send_int_3(display, 42, 43, stage3.temperature);
		u8g2_DrawStr(display, 58, 43, "C");
		u8g2_DrawTriangle(display, 63, 43, 63, 37, 69, 40);
		menu_send_int_3(display, 71, 43, stage4.temperature);
		u8g2_DrawStr(display, 86, 43, "C");
		u8g2_DrawStr(display, 90, 43, ":");
		menu_send_int_3(display, 92, 43, stage4.changePeriod);
		u8g2_DrawStr(display, 107, 43, "min");
	} else
		;
	if (stage5.state == TRUE) {
		u8g2_DrawStr(display, 5, 52, "Stage 5:");
		menu_send_int_3(display, 42, 52, stage4.temperature);
		u8g2_DrawStr(display, 58, 52, "C");
		u8g2_DrawTriangle(display, 63, 52, 63, 46, 69, 49);
		menu_send_int_3(display, 71, 52, stage5.temperature);
		u8g2_DrawStr(display, 86, 52, "C");
		u8g2_DrawStr(display, 90, 52, ":");
		menu_send_int_3(display, 92, 52, menu_counter);
		u8g2_DrawStr(display, 107, 52, "min");
	} else
		;
	if (stage6.state == TRUE) {
		u8g2_DrawStr(display, 5, 61, "Stage 6:");
		menu_send_int_3(display, 42, 61, stage5.temperature);
		u8g2_DrawStr(display, 58, 61, "C");
		u8g2_DrawTriangle(display, 63, 61, 63, 55, 69, 58);
		menu_send_int_3(display, 71, 61, stage6.temperature);
		u8g2_DrawStr(display, 86, 61, "C");
		u8g2_DrawStr(display, 90, 61, ":");
		menu_send_int_3(display, 92, 61, stage6.changePeriod);
		u8g2_DrawStr(display, 107, 61, "min");
	} else;

	u8g2_DrawBox(display, 3, 45, 38, 10);
	u8g2_SetDrawColor(display, 0);
	u8g2_DrawStr(display, 5, 52, "Stage 5:");
	u8g2_SetDrawColor(display, 1);

	u8g2_SendBuffer(display);

}
void menu_prg_AdvTempCtrl_RisingTimes_st6_run(u8g2_t *display, uint32_t menu_counter){
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

	if (stage1.state == TRUE) {
		u8g2_DrawStr(display, 5, 16, "Stage 1:");
		menu_send_int_3(display, 42, 16, cs1_temp);
		u8g2_DrawStr(display, 58, 16, "C");
		u8g2_DrawTriangle(display, 63, 16, 63, 10, 69, 13);
		menu_send_int_3(display, 71, 16, stage1.temperature);
		u8g2_DrawStr(display, 86, 16, "C");
		u8g2_DrawStr(display, 90, 16, ":");
		menu_send_int_3(display, 92, 16, stage1.changePeriod);
		u8g2_DrawStr(display, 107, 16, "min");
	} else
		;
	if (stage2.state == TRUE) {
		u8g2_DrawStr(display, 5, 25, "Stage 2:");
		menu_send_int_3(display, 42, 25, stage1.temperature);
		u8g2_DrawStr(display, 58, 25, "C");
		u8g2_DrawTriangle(display, 63, 25, 63, 19, 69, 22);
		menu_send_int_3(display, 71, 25, stage2.temperature);
		u8g2_DrawStr(display, 86, 25, "C");
		u8g2_DrawStr(display, 90, 25, ":");
		menu_send_int_3(display, 92, 25, stage2.changePeriod);
		u8g2_DrawStr(display, 107, 25, "min");
	} else;
	if (stage3.state == TRUE) {
		u8g2_DrawStr(display, 5, 34, "Stage 3:");
		menu_send_int_3(display, 42, 34, stage2.temperature);
		u8g2_DrawStr(display, 58, 34, "C");
		u8g2_DrawTriangle(display, 63, 34, 63, 28, 69, 31);
		menu_send_int_3(display, 71, 34, stage3.temperature);
		u8g2_DrawStr(display, 86, 34, "C");
		u8g2_DrawStr(display, 90, 34, ":");
		menu_send_int_3(display, 92, 34, stage3.changePeriod);
		u8g2_DrawStr(display, 107, 34, "min");
	} else ;
	if (stage4.state == TRUE) {
		u8g2_DrawStr(display, 5, 43, "Stage 4:");
		menu_send_int_3(display, 42, 43, stage3.temperature);
		u8g2_DrawStr(display, 58, 43, "C");
		u8g2_DrawTriangle(display, 63, 43, 63, 37, 69, 40);
		menu_send_int_3(display, 71, 43, stage4.temperature);
		u8g2_DrawStr(display, 86, 43, "C");
		u8g2_DrawStr(display, 90, 43, ":");
		menu_send_int_3(display, 92, 43, stage4.changePeriod);
		u8g2_DrawStr(display, 107, 43, "min");
	} else
		;
	if (stage5.state == TRUE) {
		u8g2_DrawStr(display, 5, 52, "Stage 5:");
		menu_send_int_3(display, 42, 52, stage4.temperature);
		u8g2_DrawStr(display, 58, 52, "C");
		u8g2_DrawTriangle(display, 63, 52, 63, 46, 69, 49);
		menu_send_int_3(display, 71, 52, stage5.temperature);
		u8g2_DrawStr(display, 86, 52, "C");
		u8g2_DrawStr(display, 90, 52, ":");
		menu_send_int_3(display, 92, 52, stage5.changePeriod);
		u8g2_DrawStr(display, 107, 52, "min");
	} else
		;
	if (stage6.state == TRUE) {
		u8g2_DrawStr(display, 5, 61, "Stage 6:");
		menu_send_int_3(display, 42, 61, stage5.temperature);
		u8g2_DrawStr(display, 58, 61, "C");
		u8g2_DrawTriangle(display, 63, 61, 63, 55, 69, 58);
		menu_send_int_3(display, 71, 61, stage6.temperature);
		u8g2_DrawStr(display, 86, 61, "C");
		u8g2_DrawStr(display, 90, 61, ":");
		menu_send_int_3(display, 92, 61, menu_counter);
		u8g2_DrawStr(display, 107, 61, "min");
	} else;

	u8g2_DrawBox(display, 3, 54, 38, 10);
	u8g2_SetDrawColor(display, 0);
	u8g2_DrawStr(display, 5, 61, "Stage 6:");
	u8g2_SetDrawColor(display, 1);

	u8g2_SendBuffer(display);
}

void menu_home_run(u8g2_t *display, uint8_t menu_counter){

	  switch(menu_counter){
	  case 0:
		  u8g2_ClearBuffer(display);
		  u8g2_DrawStr(display, 11, 7, "Composite Curing Furnace");
		  u8g2_DrawLine(display, 5, 10, 122, 10);
		  u8g2_DrawStr(display, 3, 19+4, "T1:");
		  menu_send_int_1(display, 18, 19+4, cs1_temp);
		  u8g2_DrawStr(display, 3, 25+4, "T2:");
		  menu_send_int_1(display, 18, 25+4, cs2_temp);
		  u8g2_DrawStr(display, 3, 31+4, "T3:");
		  menu_send_int_1(display, 18, 31+4, cs3_temp);
		  u8g2_DrawStr(display, 3, 37+4, "T4:");
		  menu_send_int_1(display, 18, 37+4, cs4_temp);
		  u8g2_DrawStr(display, 3, 43+4, "T5:");
		  menu_send_int_1(display, 18, 43+4, cs5_temp);
		  menu_send_logo(display, 85, 13);
		  menu_send_date(display, 40, 60, date, month, year);
		  menu_send_time(display, 3, 60, hour, minute);
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
		  menu_send_int_1(display, 18, 19+4, cs1_temp);
		  u8g2_DrawStr(display, 3, 25+4, "T2:");
		  menu_send_int_1(display, 18, 25+4, cs2_temp);
		  u8g2_DrawStr(display, 3, 31+4, "T3:");
		  menu_send_int_1(display, 18, 31+4, cs3_temp);
		  u8g2_DrawStr(display, 3, 37+4, "T4:");
		  menu_send_int_1(display, 18, 37+4, cs4_temp);
		  u8g2_DrawStr(display, 3, 43+4, "T5:");
		  menu_send_int_1(display, 18, 43+4, cs5_temp);
		  menu_send_logo(display, 85, 13);
		  menu_send_date(display, 40, 60, date, month, year);
		  menu_send_time(display, 3, 60, hour, minute);
		  u8g2_SetFont(display, u8g2_font_6x12_tr);
		  u8g2_DrawStr(display, 34, 25, "Programs");
		  u8g2_DrawStr(display, 34, 44, "Settings");
		  u8g2_SetFont(display, u8g2_font_04b_03_tr);
		  u8g2_DrawRFrame(display, 30, 34, 55, 15, 5);
		  u8g2_SendBuffer(display);
	  }





}
