################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/riley/Github/BeagleBone/source/cprog/task_backpack/timer/timer_driver.c 

OBJS += \
./timer/timer_driver.o 

C_DEPS += \
./timer/timer_driver.d 


# Each subdirectory must supply rules for building sources it contributes
timer/timer_driver.o: /home/riley/Github/BeagleBone/source/cprog/task_backpack/timer/timer_driver.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I../../task -I../../timer -I../../user_leds -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


