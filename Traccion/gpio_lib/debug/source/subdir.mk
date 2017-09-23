################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
F:/Workspace/SDK46/boards/frdmkl46z/driver_examples/gpio/led_output/gpio_led_output.c 

OBJS += \
./source/gpio_led_output.o 

C_DEPS += \
./source/gpio_led_output.d 


# Each subdirectory must supply rules for building sources it contributes
source/gpio_led_output.o: F:/Workspace/SDK46/boards/frdmkl46z/driver_examples/gpio/led_output/gpio_led_output.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -ffreestanding -fno-builtin -Wall  -g -DDEBUG -DCPU_MKL46Z256VLL4 -DFRDM_KL46Z -DFREEDOM -I../../../../../../../CMSIS/Include -I../../../../../../../devices -I../.. -I../../../../.. -I../../../../../../../devices/MKL46Z4/drivers -I../../../../../../../devices/MKL46Z4/utilities -I../../../../../../../devices/MKL46Z4 -std=gnu99 -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


