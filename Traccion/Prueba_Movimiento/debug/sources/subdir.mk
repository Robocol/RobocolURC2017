################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../sources/board.c \
../sources/clock_config.c \
../sources/gpio_robocol.c \
../sources/i2c_robocol.c \
../sources/movimiento.c \
../sources/pin_mux.c \
../sources/pwm_robocol.c 

OBJS += \
./sources/board.o \
./sources/clock_config.o \
./sources/gpio_robocol.o \
./sources/i2c_robocol.o \
./sources/movimiento.o \
./sources/pin_mux.o \
./sources/pwm_robocol.o 

C_DEPS += \
./sources/board.d \
./sources/clock_config.d \
./sources/gpio_robocol.d \
./sources/i2c_robocol.d \
./sources/movimiento.d \
./sources/pin_mux.d \
./sources/pwm_robocol.d 


# Each subdirectory must supply rules for building sources it contributes
sources/%.o: ../sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -ffreestanding -fno-builtin -Wall  -g -DDEBUG -DCPU_MKL46Z256VLL4 -DFRDM_KL46Z -DFREEDOM -I../CMSIS/Include -I../devices -I../drivers -I../sources -I../utilities -std=gnu99 -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


