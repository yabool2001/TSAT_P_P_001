################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Astrocast/Src/astronode_application.c \
../Drivers/Astrocast/Src/astronode_transport.c \
../Drivers/Astrocast/Src/my_astronode.c 

OBJS += \
./Drivers/Astrocast/Src/astronode_application.o \
./Drivers/Astrocast/Src/astronode_transport.o \
./Drivers/Astrocast/Src/my_astronode.o 

C_DEPS += \
./Drivers/Astrocast/Src/astronode_application.d \
./Drivers/Astrocast/Src/astronode_transport.d \
./Drivers/Astrocast/Src/my_astronode.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Astrocast/Src/%.o Drivers/Astrocast/Src/%.su Drivers/Astrocast/Src/%.cyclo: ../Drivers/Astrocast/Src/%.c Drivers/Astrocast/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G071xx -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/mzeml/embedded/TSAT_P_P_001/Drivers/Astrocast/Inc" -I"C:/Users/mzeml/embedded/TSAT_P_P_001/Drivers/Astrocast/Src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-Astrocast-2f-Src

clean-Drivers-2f-Astrocast-2f-Src:
	-$(RM) ./Drivers/Astrocast/Src/astronode_application.cyclo ./Drivers/Astrocast/Src/astronode_application.d ./Drivers/Astrocast/Src/astronode_application.o ./Drivers/Astrocast/Src/astronode_application.su ./Drivers/Astrocast/Src/astronode_transport.cyclo ./Drivers/Astrocast/Src/astronode_transport.d ./Drivers/Astrocast/Src/astronode_transport.o ./Drivers/Astrocast/Src/astronode_transport.su ./Drivers/Astrocast/Src/my_astronode.cyclo ./Drivers/Astrocast/Src/my_astronode.d ./Drivers/Astrocast/Src/my_astronode.o ./Drivers/Astrocast/Src/my_astronode.su

.PHONY: clean-Drivers-2f-Astrocast-2f-Src

