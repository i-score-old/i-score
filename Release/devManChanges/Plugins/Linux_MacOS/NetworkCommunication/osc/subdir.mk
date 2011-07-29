################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../devManChanges/Plugins/Linux_MacOS/NetworkCommunication/osc/OscPrintReceivedElements.cpp 

OBJS += \
./devManChanges/Plugins/Linux_MacOS/NetworkCommunication/osc/OscPrintReceivedElements.o 

CPP_DEPS += \
./devManChanges/Plugins/Linux_MacOS/NetworkCommunication/osc/OscPrintReceivedElements.d 


# Each subdirectory must supply rules for building sources it contributes
devManChanges/Plugins/Linux_MacOS/NetworkCommunication/osc/%.o: ../devManChanges/Plugins/Linux_MacOS/NetworkCommunication/osc/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


