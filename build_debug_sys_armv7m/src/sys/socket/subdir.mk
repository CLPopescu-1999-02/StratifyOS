################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/sys/socket/socket_api.c 

OBJS += \
./src/sys/socket/socket_api.o 

C_DEPS += \
./src/sys/socket/socket_api.d 


# Each subdirectory must supply rules for building sources it contributes
src/sys/socket/%.o: ../src/sys/socket/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-eabi-gcc -D__StratifyOS__ -D__armv7m -D__sys -D___debug -I"/Users/tgil/git/StratifyOS/include/posix" -I"/Users/tgil/git/StratifyOS/include" -I"/Users/tgil/git/StratifyOS/src" -I"/Applications/StratifyLabs-SDK/Tools/gcc/arm-none-eabi/include" -Os -Wall -c -ffunction-sections -fomit-frame-pointer -march=armv7-m -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


