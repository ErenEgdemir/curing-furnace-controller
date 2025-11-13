################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/UserLibraries/ds1307_for_stm32_hal.c \
../Core/Src/UserLibraries/menu.c \
../Core/Src/UserLibraries/pi_control.c \
../Core/Src/UserLibraries/settings.c 

OBJS += \
./Core/Src/UserLibraries/ds1307_for_stm32_hal.o \
./Core/Src/UserLibraries/menu.o \
./Core/Src/UserLibraries/pi_control.o \
./Core/Src/UserLibraries/settings.o 

C_DEPS += \
./Core/Src/UserLibraries/ds1307_for_stm32_hal.d \
./Core/Src/UserLibraries/menu.d \
./Core/Src/UserLibraries/pi_control.d \
./Core/Src/UserLibraries/settings.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/UserLibraries/%.o Core/Src/UserLibraries/%.su Core/Src/UserLibraries/%.cyclo: ../Core/Src/UserLibraries/%.c Core/Src/UserLibraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I"/Users/erenegdemir/Desktop/Calismalar/Stm/oven_prototype/Core/Inc/UserLibraries" -I"/Users/erenegdemir/Desktop/Calismalar/Stm/oven_prototype/Core/Src/UserLibraries" -I../Core/Inc -I"/Users/erenegdemir/Desktop/Calismalar/Stm/oven_prototype/Drivers/csrc" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-UserLibraries

clean-Core-2f-Src-2f-UserLibraries:
	-$(RM) ./Core/Src/UserLibraries/ds1307_for_stm32_hal.cyclo ./Core/Src/UserLibraries/ds1307_for_stm32_hal.d ./Core/Src/UserLibraries/ds1307_for_stm32_hal.o ./Core/Src/UserLibraries/ds1307_for_stm32_hal.su ./Core/Src/UserLibraries/menu.cyclo ./Core/Src/UserLibraries/menu.d ./Core/Src/UserLibraries/menu.o ./Core/Src/UserLibraries/menu.su ./Core/Src/UserLibraries/pi_control.cyclo ./Core/Src/UserLibraries/pi_control.d ./Core/Src/UserLibraries/pi_control.o ./Core/Src/UserLibraries/pi_control.su ./Core/Src/UserLibraries/settings.cyclo ./Core/Src/UserLibraries/settings.d ./Core/Src/UserLibraries/settings.o ./Core/Src/UserLibraries/settings.su

.PHONY: clean-Core-2f-Src-2f-UserLibraries

