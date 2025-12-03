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
      Cooking_Handler.h
      Display_Handler.h
      Encoder_Handler.h
      Sensor_Handler.h
      Helper_Functions.h
      menu.h
      types.h
  Src/
    UserLibraries/
      Cooking_Handler.c
      Display_Handler.c
      Encoder_Handler.c
      Sensor_Handler.c
      Helper_Functions.c
      menu.c
Drivers/
  # STM32 HAL, CMSIS, BSP, etc.
Building the Firmware
Toolchain

STM32CubeIDE (recommended)

or any ARM-GCC based toolchain with HAL and startup files set up

Steps (STM32CubeIDE)

Import the project as an existing STM32CubeIDE project

Ensure the correct target MCU is selected (e.g. STM32F401xC)

Verify clock configuration and peripheral mappings (TIM, SPI, I2C, GPIO)

Build the project (Debug or Release)

Flash to target via ST-Link

Runtime Behavior (High-Level)
On startup:

All sensor chip-selects are set to inactive (Temp_Sens_Init()).

PI controllers and PWM outputs are initialized (Cooking_Init()).

The menu system starts at HOME state.

Main loop (simplified):

Read sensor flags (set by timer/SPI interrupts)

Update sensor frame (Sens_Handler() + TempSens_IRQ_Handler())

Run cooking state machine (Cooking_Handler())

Refresh display (Display_Handler())

Handle encoder ticks and button events to navigate menus and adjust settings

Documentation (Doxygen)
This project is documented using Doxygen.

To generate HTML documentation:


doxygen Doxyfile
Then open:


docs/html/index.html
If you want to use this README as the main page in Doxygen, set in Doxyfile:


USE_MDFILE_AS_MAINPAGE = README.md
Safety Notice
This firmware controls hardware capable of generating dangerous temperatures.
Before using it in a real furnace:

Verify all safety mechanisms (over-temperature, sensor drift checks, ABORT state).

Add independent hardware safety layers (thermal fuse, hardware limiters, E-stop).

Validate all control parameters on a safe test bench.

Use at your own risk.

