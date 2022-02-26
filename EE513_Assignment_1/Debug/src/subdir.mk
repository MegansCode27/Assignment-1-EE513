################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Byte_Manipulation.cpp \
../src/RTC.cpp \
../src/Temperature.cpp 

CPP_DEPS += \
./src/Byte_Manipulation.d \
./src/RTC.d \
./src/Temperature.d 

OBJS += \
./src/Byte_Manipulation.o \
./src/RTC.o \
./src/Temperature.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/Byte_Manipulation.d ./src/Byte_Manipulation.o ./src/RTC.d ./src/RTC.o ./src/Temperature.d ./src/Temperature.o

.PHONY: clean-src

