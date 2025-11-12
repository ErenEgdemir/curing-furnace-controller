################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/RTC/ds1307_for_stm32_hal.c 

OBJS += \
./Drivers/RTC/ds1307_for_stm32_hal.o 

C_DEPS += \
./Drivers/RTC/ds1307_for_stm32_hal.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/RTC/%.o Drivers/RTC/%.su Drivers/RTC/%.cyclo: ../Drivers/RTC/%.c Drivers/RTC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I"/Users/erenegdemir/Desktop/Calismalar/Stm/oven_prototype/Drivers/RTC" -I"/Users/erenegdemir/Desktop/Calismalar/Stm/oven_prototype/Drivers/csrc" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-RTC

clean-Drivers-2f-RTC:
	-$(RM) ./Drivers/RTC/ds1307_for_stm32_hal.cyclo ./Drivers/RTC/ds1307_for_stm32_hal.d ./Drivers/RTC/ds1307_for_stm32_hal.o ./Drivers/RTC/ds1307_for_stm32_hal.su

.PHONY: clean-Drivers-2f-RTC

