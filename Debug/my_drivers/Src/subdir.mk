################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../my_drivers/Src/astronode_application.c \
../my_drivers/Src/astronode_transport.c \
../my_drivers/Src/lis2dw12_reg.c \
../my_drivers/Src/my_astronode.c \
../my_drivers/Src/my_lis2dw12.c \
../my_drivers/Src/my_lx6_gnss.c \
../my_drivers/Src/my_rtc.c 

OBJS += \
./my_drivers/Src/astronode_application.o \
./my_drivers/Src/astronode_transport.o \
./my_drivers/Src/lis2dw12_reg.o \
./my_drivers/Src/my_astronode.o \
./my_drivers/Src/my_lis2dw12.o \
./my_drivers/Src/my_lx6_gnss.o \
./my_drivers/Src/my_rtc.o 

C_DEPS += \
./my_drivers/Src/astronode_application.d \
./my_drivers/Src/astronode_transport.d \
./my_drivers/Src/lis2dw12_reg.d \
./my_drivers/Src/my_astronode.d \
./my_drivers/Src/my_lis2dw12.d \
./my_drivers/Src/my_lx6_gnss.d \
./my_drivers/Src/my_rtc.d 


# Each subdirectory must supply rules for building sources it contributes
my_drivers/Src/%.o my_drivers/Src/%.su my_drivers/Src/%.cyclo: ../my_drivers/Src/%.c my_drivers/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G071xx -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/mzeml/embedded/TSAT_P_P_001/my_lib/Inc" -I"C:/Users/mzeml/embedded/TSAT_P_P_001/my_drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfloat-abi=soft -mthumb -o "$@"

clean: clean-my_drivers-2f-Src

clean-my_drivers-2f-Src:
	-$(RM) ./my_drivers/Src/astronode_application.cyclo ./my_drivers/Src/astronode_application.d ./my_drivers/Src/astronode_application.o ./my_drivers/Src/astronode_application.su ./my_drivers/Src/astronode_transport.cyclo ./my_drivers/Src/astronode_transport.d ./my_drivers/Src/astronode_transport.o ./my_drivers/Src/astronode_transport.su ./my_drivers/Src/lis2dw12_reg.cyclo ./my_drivers/Src/lis2dw12_reg.d ./my_drivers/Src/lis2dw12_reg.o ./my_drivers/Src/lis2dw12_reg.su ./my_drivers/Src/my_astronode.cyclo ./my_drivers/Src/my_astronode.d ./my_drivers/Src/my_astronode.o ./my_drivers/Src/my_astronode.su ./my_drivers/Src/my_lis2dw12.cyclo ./my_drivers/Src/my_lis2dw12.d ./my_drivers/Src/my_lis2dw12.o ./my_drivers/Src/my_lis2dw12.su ./my_drivers/Src/my_lx6_gnss.cyclo ./my_drivers/Src/my_lx6_gnss.d ./my_drivers/Src/my_lx6_gnss.o ./my_drivers/Src/my_lx6_gnss.su ./my_drivers/Src/my_rtc.cyclo ./my_drivers/Src/my_rtc.d ./my_drivers/Src/my_rtc.o ./my_drivers/Src/my_rtc.su

.PHONY: clean-my_drivers-2f-Src

