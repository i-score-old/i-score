################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/data/subdir.mk
-include src/GUI/subdir.mk
-include moc/subdir.mk
-include devManChanges/Plugins/Linux_MacOS/NetworkCommunication/osc/subdir.mk
-include Acousmosynth/ACOUSMOSYNTH2.app/Contents/Frameworks/MaxAPI.framework/Versions/Current/Headers/subdir.mk
-include Acousmosynth/ACOUSMOSYNTH2.app/Contents/Frameworks/MaxAPI.framework/Versions/A/Headers/subdir.mk
-include Acousmosynth/ACOUSMOSYNTH2.app/Contents/Frameworks/MaxAPI.framework/Headers/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: acousmoscribe

# Tool invocations
acousmoscribe: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MacOS X C++ Linker'
	g++  -o "acousmoscribe" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) acousmoscribe
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
