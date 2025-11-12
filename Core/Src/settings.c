/*
 * settings.c
 *
 *  Created on: Oct 28, 2025
 *      Author: erenegdemir
 */

#include "settings.h"
#include <string.h>
#include <stddef.h>

/* ------------------------------- Yardımcılar ------------------------------- */

static uint32_t crc32_calc(const void *data, size_t len)
{
    uint32_t crc = 0xFFFFFFFFu;
    const uint8_t *p = (const uint8_t*)data;
    while (len--) {
        crc ^= *p++;
        for (int i = 0; i < 8; i++) {
            crc = (crc >> 1) ^ (0xEDB88320u & (-(int32_t)(crc & 1)));
        }
    }
    return ~crc;
}

static inline uint32_t FlashSizeKB(void)
{
    return (uint32_t)(*(uint16_t*)FLASHSIZE_BASE);
}

/* Adres→Sektör: 256 KB cihazda 0..5 mevcuttur; 512 KB’da 6 ve 7 de olabilir.
   Sembol mevcut değilse (#ifdef) o dala hiç girmeyeceğiz. */
static uint32_t GetSector(uint32_t Address)
{
    if      (Address < 0x08004000u) return FLASH_SECTOR_0; // 16KB
    else if (Address < 0x08008000u) return FLASH_SECTOR_1; // 16KB
    else if (Address < 0x0800C000u) return FLASH_SECTOR_2; // 16KB
    else if (Address < 0x08010000u) return FLASH_SECTOR_3; // 16KB
    else if (Address < 0x08020000u) return FLASH_SECTOR_4; // 64KB
    else if (Address < 0x08040000u) return FLASH_SECTOR_5; // 128KB
    #ifdef FLASH_SECTOR_6
    else if (Address < 0x08060000u) return FLASH_SECTOR_6; // 128KB
    #endif
    #ifdef FLASH_SECTOR_7
    else                             return FLASH_SECTOR_7; // 128KB
    #else
    else                             return FLASH_SECTOR_5; // 256KB cihazda emniyetli dönüş
    #endif
}

/* Voltaj aralığı: 2.7–3.6V için RANGE_3 tipiktir */
#define FLASH_VR FLASH_VOLTAGE_RANGE_3

static int Flash_EraseRange(uint32_t start_addr, uint32_t end_addr_exclusive)
{
    FLASH_EraseInitTypeDef ei = {0};
    uint32_t sector_error = 0;

    uint32_t start_sector = GetSector(start_addr);
    uint32_t end_sector   = GetSector(end_addr_exclusive);

    ei.TypeErase    = FLASH_TYPEERASE_SECTORS;
    ei.VoltageRange = FLASH_VR;
    ei.Sector       = start_sector;
    ei.NbSectors    = (end_sector - start_sector) + 1;

    HAL_FLASH_Unlock();
    HAL_StatusTypeDef st = HAL_FLASHEx_Erase(&ei, &sector_error);
    HAL_FLASH_Lock();

    return (st == HAL_OK) ? SETTINGS_OK : SETTINGS_ERR_FLASH_ERASE;
}

static int Flash_ProgramWords(uint32_t dst_addr, const uint32_t* words, uint32_t word_count)
{
    HAL_FLASH_Unlock();
    for (uint32_t i = 0; i < word_count; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, dst_addr, words[i]) != HAL_OK) {
            HAL_FLASH_Lock();
            return SETTINGS_ERR_FLASH_PROGRAM;
        }
        dst_addr += 4u;
    }
    HAL_FLASH_Lock();
    return SETTINGS_OK;
}

static void Flash_ReadWords(uint32_t src_addr, uint32_t* out_words, uint32_t word_count)
{
    for (uint32_t i = 0; i < word_count; i++) {
        out_words[i] = *(__IO uint32_t*)(src_addr);
        src_addr += 4u;
    }
}

/* 256 KB cihaz: SECTOR_5 (0x08020000)
   512 KB cihaz: SECTOR_6 (0x08040000) ve —varsa— SECTOR_7 (0x08060000) */
#define SETTINGS_BASE_256K     (0x08020000u) /* S5 */
#define SETTINGS_BASE_512K_A   (0x08040000u) /* S6 */
#define SETTINGS_BASE_512K_B   (0x08060000u) /* S7 */

static void PickBases(uint32_t* baseA, uint32_t* baseB)
{
    if (FlashSizeKB() >= 512u) {
        *baseA = SETTINGS_BASE_512K_A;
        /* Eğer HAL bu cihazda S7 tanımlamadıysa, B’yi kapat */
        #ifdef FLASH_SECTOR_7
        *baseB = SETTINGS_BASE_512K_B;
        #else
        *baseB = 0u;
        #endif
    } else {
        *baseA = SETTINGS_BASE_256K;
        *baseB = 0u;
    }
}

static int ReadOne(uint32_t base, AppSettings* out)
{
    if (base == 0u) return SETTINGS_ERR_MAGIC_VERSION;
    Flash_ReadWords(base, (uint32_t*)out, (uint32_t)((sizeof(AppSettings) + 3u)/4u));

    if (out->magic != SETTINGS_MAGIC || out->version != SETTINGS_VERSION)
        return SETTINGS_ERR_MAGIC_VERSION;

    uint32_t calc = crc32_calc(out, sizeof(AppSettings) - sizeof(uint32_t));
    if (calc != out->crc32) return SETTINGS_ERR_CRC;

    return SETTINGS_OK;
}

static int WriteOne(uint32_t base, const AppSettings* in)
{
    AppSettings tmp = *in;
    tmp.magic   = SETTINGS_MAGIC;
    tmp.version = SETTINGS_VERSION;
    tmp.crc32   = 0u;
    tmp.crc32   = crc32_calc(&tmp, sizeof(AppSettings) - sizeof(uint32_t));

    const uint32_t start = base;
    const uint32_t end   = base + (uint32_t)((sizeof(AppSettings) + 3u) & ~3u);

    int er = Flash_EraseRange(start, end);
    if (er != SETTINGS_OK) return er;

    int pr = Flash_ProgramWords(base, (const uint32_t*)&tmp, (uint32_t)((sizeof(AppSettings) + 3u)/4u));
    return pr;
}

/* ------------------------------- Varsayılanlar ------------------------------ */

static AppSettings g_settings;
static uint32_t    g_baseA = 0, g_baseB = 0;
static int         g_usingB = 0;

void Settings_FillDefaults(AppSettings* s)
{
    memset(s, 0, sizeof(*s));
    s->magic = SETTINGS_MAGIC;
    s->version = SETTINGS_VERSION;
    s->gen = 1;

    s->kp = 2.0f; s->ki = 0.5f; s->kd = 0.0f;
    s->setpoint_default_C = 120.0f;
    s->calib_offset_C = 0.0f;
    s->fan_stage_map[0] = 40;
    s->fan_stage_map[1] = 80;
    s->fan_stage_map[2] = 140;
    s->soak_time_s = 1800;

    for (int i=0;i<512;i++){
        s->arr0[i]=0; s->arr1[i]=0; s->arr2[i]=0; s->arr3[i]=0; s->arr4[i]=0;
    }
}

/* ---------------------------------- API ------------------------------------ */

void Settings_Init(void)
{
    PickBases(&g_baseA, &g_baseB);

    AppSettings a = {0}, b = {0};
    int ra = ReadOne(g_baseA, &a);
    int rb = (g_baseB ? ReadOne(g_baseB, &b) : SETTINGS_ERR_MAGIC_VERSION);

    if (ra == SETTINGS_OK && rb == SETTINGS_OK) {
        if (b.gen > a.gen) { g_settings = b; g_usingB = 1; }
        else               { g_settings = a; g_usingB = 0; }
    } else if (ra == SETTINGS_OK) {
        g_settings = a; g_usingB = 0;
    } else if (rb == SETTINGS_OK) {
        g_settings = b; g_usingB = 1;
    } else {
        Settings_FillDefaults(&g_settings);
        (void)Settings_Save();
    }
}

AppSettings* Settings_Get(void)
{
    return &g_settings;
}

int Settings_Save(void)
{
    g_settings.gen = (g_settings.gen == 0u) ? 1u : (g_settings.gen + 1u);

    if (g_baseB != 0u) {
        uint32_t target = g_usingB ? g_baseA : g_baseB;
        int rc = WriteOne(target, &g_settings);
        if (rc == SETTINGS_OK) g_usingB = !g_usingB;
        return rc;
    } else {
        return WriteOne(g_baseA, &g_settings);
    }
}

void Settings_ResetToDefaults(int also_save)
{
    Settings_FillDefaults(&g_settings);
    g_settings.gen += 1u;
    if (also_save) (void)Settings_Save();
}
