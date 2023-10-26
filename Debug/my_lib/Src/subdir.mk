################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../my_lib/Src/my_conversions.c \
../my_lib/Src/my_nmea.c 

OBJS += \
./my_lib/Src/my_conversions.o \
./my_lib/Src/my_nmea.o 

C_DEPS += \
./my_lib/Src/my_conversions.d \
./my_lib/Src/my_nmea.d 


# Each subdirectory must supply rules for building sources it contributes
my_lib/Src/%.o my_lib/Src/%.su my_lib/Src/%.cyclo: ../my_lib/Src/%.c my_lib/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G071xx -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/mzeml/embedded/TSAT_P_P_001/my_lib/Inc" -I"C:/Users/mzeml/embedded/TSAT_P_P_001/my_drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-my_lib-2f-Src

clean-my_lib-2f-Src:
	-$(RM) ./my_lib/Src/my_conversions.cyclo ./my_lib/Src/my_conversions.d ./my_lib/Src/my_conversions.o ./my_lib/Src/my_conversions.su ./my_lib/Src/my_nmea.cyclo ./my_lib/Src/my_nmea.d ./my_lib/Src/my_nmea.o ./my_lib/Src/my_nmea.su

.PHONY: clean-my_lib-2f-Src

