/**
 * @file    Sensor_Handler.h
 * @brief   Definitions for thermocouple sensor acquisition, SPI-based MAX31855
 *          interface control, and RTC-assisted timestamp reading.
 *
 * This header exposes the API used by the sensor subsystem of the composite
 * curing furnace controller. It provides:
 *
 *  - IRQ-level temperature decoding from MAX31855 thermocouple converters
 *    (TempSens_IRQ_Handler)
 *  - Time-sliced polling of multiple sensors and DS1307 RTC fields
 *    (Sens_Handler)
 *  - Initialization utilities for sensor chip-select lines
 *    (Temp_Sens_Init)
 *
 * **Functional Scope**
 *  - Defines temperature scaling constants (TEMP_CAL)
 *  - Declares handlers for DMA-driven SPI acquisition
 *  - Uses GPIO-controlled multiplexing to cycle through multiple thermocouple
 *    channels
 *  - Integrates with system-wide timing, SPI, and GPIO HAL infrastructure
 *
 * **Intended Usage**
 *  - Called from ISR + main loop cooperative scheduling
 *  - Designed for lightweight and deterministic temperature updates
 *  - Complements Sensor_Handler.c implementation
 *
 * @date    Dec 3, 2025
 * @author  Eren Egdemir
 */

#ifndef INC_USERLIBRARIES_SENSOR_HANDLER_H_
#define INC_USERLIBRARIES_SENSOR_HANDLER_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "main.h"            // pin tanımları için (yukarıda anlatılan)
#include "common_def.h"
#include "types.h"

#define TEMP_CAL	0.25
#define TRUE		1

/**
 * @brief  Handles raw MAX31855 thermocouple IRQ data and updates sensor readings.
 *
 * This function processes a recently received SPI frame stored in `t->buffer`,
 * extracts the 12-bit signed thermocouple temperature value, performs proper
 * sign-extension, scales it using `TEMP_CAL`, and assigns the result to the
 * correct sensor field based on the active chip-select (CS) line.
 *
 * Processing steps:
 *   1. Extract the 12-bit raw temperature field:
 *         - Bits [14:3] of buffer[0]
 *         - Stored in variable `t12`
 *   2. Perform sign-extension:
 *         - If 12-bit value has sign bit (0x800), extend into 16-bit signed
 *   3. Multiply by TEMP_CAL to convert raw code to °C
 *   4. Determine which sensor is active based on which MAX_CSx pin is LOW
 *   5. Store computed temperature into:
 *         • sens1_temp … sens5_temp
 *   6. Reset temp_f after assignment
 *
 * After processing, all MAX31855 CS lines are driven HIGH to finalize
 * the readout cycle.
 *
 * @param[in,out] t   Pointer to the sensor structure that holds:
 *                    - buffer[] : raw SPI frame
 *                    - temp_f   : decoded float temperature
 *                    - sensX_temp fields to be updated
 *
 * @note  This function is intended to be called from an IRQ-driven context
 *        immediately after SPI reception completes.
 *
 * @note  Only one sensor CS line should be LOW during invocation; the function
 *        assumes proper external multiplexing/timing.
 */
void TempSens_IRQ_Handler(Sens *t);

/**
 * @brief  Sequential sensor and RTC acquisition handler for time-sliced sampling.
 *
 * This function polls RTC fields (date, month, year, hour, minute) and
 * sequentially triggers MAX31855 thermocouple conversions via DMA-based SPI
 * transfers. It is designed to be called periodically with an incrementing
 * counter (`cntr`) that selects which sensor or RTC field is processed.
 *
 * Operation (per value of `cntr`):
 *   cntr = 0:
 *      - Read RTC date from DS1307
 *      - Activate MAX_CS1 and start DMA SPI read → buffer[0]
 *
 *   cntr = 1:
 *      - Read RTC month
 *      - Activate MAX_CS2 and start DMA read
 *
 *   cntr = 2:
 *      - Read RTC year
 *      - Activate MAX_CS3 and start DMA read
 *
 *   cntr = 3:
 *      - Read RTC hour
 *      - Activate MAX_CS4 and start DMA read
 *
 *   cntr = 4:
 *      - Read RTC minute
 *      - Activate MAX_CS5 and start DMA read
 *      - Reset cntr → 0 (start new cycle)
 *
 * The MAX_CSx lines ensure that only one MAX31855 device is active during
 * each SPI transaction. The actual temperature decoding is performed later by
 * TempSens_IRQ_Handler(), which is triggered by the SPI Rx Complete IRQ.
 *
 * @param[in,out] t      Pointer to the global sensor structure storing:
 *                       - current RTC fields
 *                       - raw MAX31855 buffer[] (used by IRQ handler)
 * @param[in,out] cntr   State index used to determine which sensor/RTC field
 *                       to process during this call.
 *                       Updated internally to advance to the next step.
 * @param[in]     hspi   SPI handle used for DMA reception from MAX31855 chips.
 *
 * @note  This function is non-blocking; all temperature reads happen
 *        asynchronously via DMA + IRQ.
 *
 * @note  All five MAX31855 sensors are sampled over 5 consecutive calls,
 *        interleaved with RTC fetches, creating a lightweight cooperative
 *        scheduler without delays inside the ISR or main loop.
 */
void Sens_Handler(Sens *t, uint32_t cntr, SPI_HandleTypeDef *hspi);

/**
 * @brief  Initializes all MAX31855 chip-select lines to the inactive state.
 *
 * This function ensures that every MAX31855 thermocouple interface begins
 * in a safe and idle condition by driving all CS pins HIGH.
 * In the MAX31855 protocol, a HIGH level means "device not selected".
 *
 * After initialization:
 *   - No MAX31855 device is active.
 *   - The SPI bus is free and stable.
 *   - The system is ready for controlled CS activation during temperature reads.
 *
 * Typical usage:
 *   - Call once during system startup (before any SPI transactions).
 *
 * @note  All GPIO pins must be configured as outputs by CubeMX prior to calling.
 *
 * @param  None
 * @return None
 */
void Temp_Sens_Init(void);

#endif /* INC_USERLIBRARIES_SENSOR_HANDLER_H_ */
