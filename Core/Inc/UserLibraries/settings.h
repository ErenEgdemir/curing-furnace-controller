/*
 * settings.h
 *
 *  Created on: Oct 28, 2025
 *      Author: erenegdemir
 */

#ifndef INC_SETTINGS_H_
#define INC_SETTINGS_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Sürüm/Magic ---- */
#define SETTINGS_MAGIC    (0xA5F1C0DEu)
#define SETTINGS_VERSION  (0x0001u)

/* ---- Hata Kodları ---- */
typedef enum {
    SETTINGS_OK                 =  0,
    SETTINGS_ERR_MAGIC_VERSION  = -1,
    SETTINGS_ERR_CRC            = -2,
    SETTINGS_ERR_FLASH_ERASE    = -3,
    SETTINGS_ERR_FLASH_PROGRAM  = -4
} settings_status_t;

/* ---- Uygulama ayar yapısı ---- */
typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint16_t version;
    uint16_t reserved;
    uint32_t gen;

    /* örnek alanlar */
    float    kp, ki, kd, slew, maxPwr;
    float    setpoint_default_C;
    float    calib_offset_C;
    uint8_t  fan_stage_map[3];
    uint32_t soak_time_s;

    /* 5×(512×32-bit) dizi (≈10 KB) */
    uint32_t arr0[512];
    uint32_t arr1[512];
    uint32_t arr2[512];
    uint32_t arr3[512];
    uint32_t arr4[512];

    uint32_t crc32;  /* (crc32 alanı hariç) */
} AppSettings;

/* ---- API ---- */
void        Settings_Init(void);
AppSettings* Settings_Get(void);
int         Settings_Save(void);
void        Settings_ResetToDefaults(int also_save);

/* Varsayılanları özelleştirmek istersen override edilebilen kanca */
void Settings_FillDefaults(AppSettings* s) __attribute__((weak));

#ifdef __cplusplus
}
#endif

#endif /* SETTINGS_H */
