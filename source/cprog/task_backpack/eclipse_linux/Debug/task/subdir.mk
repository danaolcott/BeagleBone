################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/riley/Github/BeagleBone/source/cprog/task_backpack/task/task.c 

OBJS += \
./task/task.o 

C_DEPS += \
./task/task.d 


# Each subdirectory must supply rules for building sources it contributes
task/task.o: /home/riley/Github/BeagleBone/source/cprog/task_backpack/task/task.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I../../task -I../../timer -I../../user_leds -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


