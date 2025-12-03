# Composite Curing Furnace Controller

An embedded control firmware for a multi-zone **composite curing furnace** based on an STM32F4 microcontroller.  
The system provides programmable multi-stage temperature profiles, PI-based heater control, safety monitoring, and a rotary-encoder-driven HMI on a 128x64 graphical LCD.

> This repository contains the firmware only. Power electronics, heater hardware and safety circuitry are assumed to be designed according to industrial and local safety standards.

---

## Features

- **Multi-zone temperature control**
  - 3 independent heater zones driven by PWM (TIM5 CH2/CH3/CH4)
  - Per-zone PI controllers (`PI_Oven`) with configurable `kp`, `ki`, slew rate and max power

- **Programmable curing profiles**
  - Up to **6 stages** + **FastStart** mode
  - Per-stage:
    - Target temperature
    - Duration
    - Rising time (ramp)
  - Generated minute-by-minute temperature curves (`tempForMin[]`)

- **Safety & fault handling**
  - Sensor-to-average drift checks (composite vs. furnace sensors)
  - Over-temperature protection
  - Sensor fault detection (open / 0°C readings)
  - Automatic transition to **ABORT** state on any violation
  - Heater outputs forced to safe state before optional system reset

- **User interface**
  - 128x64 graphical LCD driven by **U8g2**
  - Rotary encoders with push buttons for:
    - Menu navigation
    - Temperature / duration / power adjustments
  - Hierarchical menu system:
    - Home, Programs, Advanced Temp Control, Fast Start, Settings, Time/Date

- **Documentation-ready code**
  - Doxygen-style comments for all public APIs
  - Ready to generate HTML documentation

---

## System Architecture

The firmware is divided into several logical subsystems:

- **Cooking control**
  - `Cooking_Handler.c/.h`
  - State machine implementing:
    - `PENDING`, `FASTSTART`, `STAGE_1` … `STAGE_6`, `ABORT`
  - Uses PI controllers to compute heater duty cycles per zone
  - Integrates safety checks and remaining time tracking

- **Sensor subsystem**
  - `Sensor_Handler.c/.h`
  - Reads multiple SPI thermocouple interfaces (e.g. MAX31855-like devices)
  - Uses chip-select multiplexing (`MAX_CS1..MAX_CS5`)
  - Decodes raw thermocouple data into engineering units (°C)
  - Polls DS1307 RTC for date/time fields

- **Menu & display layer**
  - `menu.c/.h`
  - `Display_Handler.c/.h`
  - Implements all screen rendering for:
    - Home screen
    - Program selection (FastStart / Advanced Temp Control)
    - Stage configuration (T1..T6)
    - Cooking status pages
    - Settings, PI tuning, Time/Date configuration
  - Uses U8g2 + custom helper functions (`Helper_Functions.c`)

- **Encoder input handling**
  - `Encoder_Handler.c/.h`
  - Quadrature encoder reading using TIM encoder mode + GPIO
  - Debounce and button-press handling via timer interrupts
  - High-level helpers:
    - `tick_handler_encoder1/2/3()`
    - `button_handler_encoder1/2/3()`
    - `display_goto()`, `display_set()`

- **Shared types and configuration**
  - `types.h` : all core data structures
    - `setting_t`, `Sens`, `stage_t`, `CookingCtx`, `MenuCtx`, `encoder`
  - `common_def.h` : common macros / definitions
  - `pi_control.h` : PI controller abstraction

---

## Hardware Overview

> Update this section according to your actual hardware setup.

- **MCU**: STM32F4 series (e.g. STM32F401xC)
- **Display**: 128x64 ST7920-based graphical LCD driven via SPI using U8g2
- **Temperature sensors**:
  - 5 thermocouple channels via SPI front-ends (MAX_CS1..MAX_CS5)
  - Composite temperature sensors (sens1..sens3)
  - Furnace / air sensors (sens4..sens5)
- **Real-time clock**: DS1307 over I2C
- **User input**:
  - 3 rotary encoders with push buttons
- **Power stage**:
  - External power electronics driving heater zones using PWM signals from TIM5 CH2/CH3/CH4

---

## Firmware Structure

```text
Core/
  Inc/
    UserLibraries/
      Cooking_Handler.h      # Cooking state machine + PI control API
      Display_Handler.h      # High-level display routing for all menu pages
      Encoder_Handler.h      # Encoder tick + button handling
      Sensor_Handler.h       # Sensor acquisition and conversion
      Helper_Functions.h     # HAL glue + U8g2 hardware callbacks
      menu.h                 # Menu page APIs (HOME, PROGRAMS, SETTINGS, etc.)
      types.h                # Shared types (MenuCtx, CookingCtx, Sens, enums, etc.)
  Src/
    UserLibraries/
      Cooking_Handler.c
      Display_Handler.c
      Encoder_Handler.c
      Sensor_Handler.c
      Helper_Functions.c
      menu.c

Drivers/
  # STM32 HAL, CMSIS, startup files, BSP, etc.
```

---

## Building the Firmware

### Toolchain
- **STM32CubeIDE** (recommended)
- or **ARM-GCC** with HAL + startup files configured manually

### STM32CubeIDE Steps
1. Clone the repository
2. Open **STM32CubeIDE → Import → Existing Projects into Workspace**
3. Select the project root folder
4. Ensure the correct MCU is selected (**STM32F401xC**)
5. Verify peripheral configuration:
   - Clock tree
   - TIM5 PWM (channels 2, 3, 4)
   - TIM1 and TIM9 for periodic interrupts and debouncing
   - SPI1 for temperature sensors (MAX_CS1..MAX_CS5)
   - SPI2 for the LCD (U8g2 backend)
   - I²C (DS1307 RTC)
   - GPIO and interrupt mappings for encoders and buttons
6. Build the project (Debug / Release)
7. Flash onto the board using **ST-Link**

---

## Runtime Behavior (High-Level Overview)

### On Startup
- All MAX31855 (or compatible) chip-select lines are set HIGH via `Temp_Sens_Init()`.
- PI controllers and PWM outputs are initialized using `Cooking_Init()` and current settings.
- A splash screen is shown.
- The UI transitions to the **HOME** menu.

### Main Loop
A simplified main loop typically performs:

1. Check interrupt-driven flags:
   - `sensorRead_flag` (TIM1 period elapsed)
   - Encoder button flags (`IRQ_enc*_btn_flag`, TIM9 debounce)
   - `SPI_flag` (SPI thermocouple frame complete)
2. Run `Sens_Handler()` to acquire/update sensor values and RTC data.
3. Run `Cooking_Handler()` to advance the cooking state machine.
4. Call `Display_Handler()` to render the correct menu/page.
5. Process encoder ticks via `tick_handler_encoder*()` and their button handlers.

Real-time behavior (sampling rate, PI update period, UI refresh rate) is determined by timer configuration and `PI_DT`.

---

## Doxygen Documentation

The codebase is annotated with Doxygen-style comments for all public APIs and key internal helpers.

### Generate HTML Documentation

```bash
doxygen Doxyfile
```

Then open:

```text
docs/html/index.html
```

### Use README as Doxygen Main Page

In `Doxyfile`, set:

```text
USE_MDFILE_AS_MAINPAGE = README.md
```

---

## Testing & Validation

Before using the firmware in a production or high-value composite curing scenario, it is strongly recommended to:

- Validate all temperature readings against a calibrated reference.
- Test PI settings with dummy loads or low-power heaters.
- Verify stage transitions (STAGE_1..6, FASTSTART) against expected timing.
- Simulate sensor faults (open thermocouple, unplugged sensors) and confirm that the system enters **ABORT** safely.

---

## Safety Notice

This firmware controls a heating system capable of reaching dangerous temperatures.

Before connecting to a real furnace:

- Ensure a **hardware safety layer** exists:
  - Thermal fuse
  - Independent mechanical thermostat or over-temperature cut-off
  - Emergency stop (E-stop) button
- Verify proper grounding and insulation of all high-voltage and high-current paths.
- Validate that the **ABORT** state reliably disables heater outputs and, if desired, resets the system.

> Use this firmware at your own risk. The author assumes no liability for damages resulting from improper use.

---
## Author

**Eren Egdemir**

Electrical Engineering Student at Yildiz Technical  University

