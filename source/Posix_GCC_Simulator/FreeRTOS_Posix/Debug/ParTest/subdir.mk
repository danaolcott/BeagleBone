################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ParTest/ParTest.c 

OBJS += \
./ParTest/ParTest.o 

C_DEPS += \
./ParTest/ParTest.d 


# Each subdirectory must supply rules for building sources it contributes
ParTest/%.o: ../ParTest/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -D__GCC_POSIX__=1 -DDEBUG_BUILD=1 -DUSE_STDIO=1 -I../FreeRTOS_Kernel/portable/GCC/Posix -I.././Common_Demo/include -I.././ -I.././FreeRTOS_Kernel/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


