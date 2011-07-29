################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/data/Abstract.cpp \
../src/data/AbstractBox.cpp \
../src/data/AbstractComment.cpp \
../src/data/AbstractControlBox.cpp \
../src/data/AbstractCurve.cpp \
../src/data/AbstractParentBox.cpp \
../src/data/AbstractRelation.cpp \
../src/data/AbstractSoundBox.cpp \
../src/data/AbstractTriggerPoint.cpp \
../src/data/Maquette.cpp \
../src/data/Palette.cpp 

OBJS += \
./src/data/Abstract.o \
./src/data/AbstractBox.o \
./src/data/AbstractComment.o \
./src/data/AbstractControlBox.o \
./src/data/AbstractCurve.o \
./src/data/AbstractParentBox.o \
./src/data/AbstractRelation.o \
./src/data/AbstractSoundBox.o \
./src/data/AbstractTriggerPoint.o \
./src/data/Maquette.o \
./src/data/Palette.o 

CPP_DEPS += \
./src/data/Abstract.d \
./src/data/AbstractBox.d \
./src/data/AbstractComment.d \
./src/data/AbstractControlBox.d \
./src/data/AbstractCurve.d \
./src/data/AbstractParentBox.d \
./src/data/AbstractRelation.d \
./src/data/AbstractSoundBox.d \
./src/data/AbstractTriggerPoint.d \
./src/data/Maquette.d \
./src/data/Palette.d 


# Each subdirectory must supply rules for building sources it contributes
src/data/%.o: ../src/data/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


