/**
 * @file    Helper_Functions.h
 * @brief   Hardware-abstraction helper layer for SPI-driven ST7920 LCD using U8g2.
 *
 * This module provides low-level hardware interface callbacks and initialization
 * utilities required to operate an ST7920 128×64 graphic LCD over SPI through the
 * U8g2 graphics library. It contains:
 *
 *   - U8x8 SPI byte-level callback implementation (`u8x8_spi`)
 *   - U8x8 GPIO and delay handler (`u8x8_gpio_and_delay`)
 *   - High-level display initialization routine (`Display_Init`)
 *
 * The functions in this module are designed to integrate U8g2 with STM32 HAL,
 * providing the glue-code required for:
 *   - Chip Select (CS) handling
 *   - Reset line control
 *   - Millisecond delays
 *   - SPI transmit operations
 *
 * The module abstracts all hardware control details from the higher-level
 * display/rendering code and ensures consistent behavior across the firmware.
 *
 * @note This library assumes:
 *       - hspi2 is configured for SPI mode compatible with ST7920
 *       - GPIO pins LCD_CS_Pin, RESET_Pin are defined in main.h
 *       - U8g2 configured in ST7920 SPI mode (u8g2_Setup_st7920_s_128x64_f)
 *
 * @version 1.0
 * @date    2025-12-03
 * @author  Eren Eğdemir
 */

#ifndef INC_USERLIBRARIES_HELPER_FUNCTIONS_H_
#define INC_USERLIBRARIES_HELPER_FUNCTIONS_H_

#include "stm32f4xx_hal.h"
#include "u8g2.h"
#include "stm32f4xx_hal_gpio.h"
#include "main.h"
#include "menu.h"

extern SPI_HandleTypeDef hspi2;

/**
 * @brief  Low-level SPI byte callback for the U8x8 graphics library.
 *
 * This function acts as the hardware interface layer between the U8x8
 * display driver and the STM32 HAL SPI implementation. It handles all
 * byte-level SPI operations requested by the U8x8 library, including
 * sending data bytes and toggling the chip-select signal.
 *
 * Supported messages:
 *
 *  - U8X8_MSG_BYTE_SET_DC
 *        (Not used for SPI displays without a D/C pin at the byte layer)
 *
 *  - U8X8_MSG_BYTE_SEND
 *        Transmits a block of bytes over SPI using HAL_SPI_Transmit().
 *
 *  - U8X8_MSG_BYTE_START_TRANSFER
 *        Asserts the chip-select line before an SPI transfer.
 *
 *  - U8X8_MSG_BYTE_END_TRANSFER
 *        De-asserts the chip-select line after the transfer completes.
 *
 * @param[in] u8x8      Pointer to the U8x8 device structure.
 * @param[in] msg       Message identifier from U8x8 (U8X8_MSG_…).
 * @param[in] arg_int   Integer argument providing message-specific value
 *                      (e.g., number of bytes to transmit).
 * @param[in] arg_ptr   Pointer to message-specific data (e.g., bytes to send).
 *
 * @return 1 on success (required by U8x8 library).
 *
 * @note This backend is designed for STM32 HAL and assumes that:
 *       - SPI2 is configured and initialized.
 *       - LCD_CS_Pin and LCD_CS_GPIO_Port are valid chip-select signals.
 *       - Timing requirements are satisfied by the HAL implementation.
 */
uint8_t u8x8_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
		void *arg_ptr);

/**
 * @brief  GPIO and delay callback for the U8x8 graphics library.
 *
 * This function implements the GPIO and timing backend required by the U8x8
 * display driver. It provides millisecond delays, chip-select control, reset
 * pin control, and optional D/C pin handling for SPI-driven displays.
 *
 * Supported messages:
 *
 *  - U8X8_MSG_DELAY_MILLI
 *        Performs a blocking delay for @p arg_int milliseconds using HAL_Delay().
 *
 *  - U8X8_MSG_GPIO_CS
 *        Sets the display chip-select pin (LCD_CS_Pin) high or low depending
 *        on @p arg_int (0 = low, 1 = high).
 *
 *  - U8X8_MSG_GPIO_DC
 *        (Optional, currently disabled)
 *        Controls the D/C pin if required by the display. Uncomment HAL call
 *        if your display uses a data/command pin.
 *
 *  - U8X8_MSG_GPIO_RESET
 *        Sets the display reset pin (RESET_Pin) according to @p arg_int.
 *
 * @param[in] u8x8      Pointer to the U8x8 structure (unused but required).
 * @param[in] msg       Message ID from U8x8 indicating the requested operation.
 * @param[in] arg_int   Integer argument (milliseconds for delay, GPIO value, etc.).
 * @param[in] arg_ptr   Optional pointer for some messages (unused here).
 *
 * @return Always returns 1 to indicate success, as required by U8x8.
 *
 * @note This backend assumes:
 *       - HAL GPIO pins for CS and RESET are already configured as outputs.
 *       - HAL_Delay() is available and SysTick is running.
 *       - The D/C pin is optional; uncomment if your LCD requires it.
 */
uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
		void *arg_ptr);

/**
 * @brief  Initializes the ST7920-based display and renders the startup logo.
 *
 * This function configures and initializes the U8g2 graphics library for an
 * ST7920 (128×64) display using the SPI backend (`u8x8_spi`) and GPIO/delay
 * backend (`u8x8_gpio_and_delay`). After setup, it displays a startup splash
 * screen containing project information and a custom logo.
 *
 * Initialization steps:
 *  1. Configure U8g2 for ST7920 128×64 display in full-buffer mode.
 *  2. Initialize the display hardware.
 *  3. Disable power-save mode.
 *  4. Clear display RAM and internal buffer.
 *  5. Render startup text and logo:
 *        - Title: “Composite Curing Furnace”
 *        - Subtitle: “Controller”
 *        - Developer credit
 *        - Custom logo via menu_send_setupLogo()
 *  6. Send the rendered buffer to the display.
 *  7. Wait 1.5 seconds before continuing.
 *
 * @param[in,out] d   Pointer to a U8g2 display structure. The structure is
 *                    initialized and modified by this function.
 *
 * @note  This function should be called once at system startup before any menu
 *        or GUI rendering.
 *
 * @note  The rotation setting U8G2_R2 is used (180° rotation). Adjust if your
 *        LCD orientation differs.
 *
 * @note  Requires HAL SPI and GPIO peripherals to be initialized prior to use.
 */
void Display_Init(u8g2_t *d);

#endif /* INC_USERLIBRARIES_HELPER_FUNCTIONS_H_ */
