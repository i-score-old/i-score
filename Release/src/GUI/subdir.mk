################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/GUI/AttributesEditor.cpp \
../src/GUI/BasicBox.cpp \
../src/GUI/BoxContextMenu.cpp \
../src/GUI/ChooseTemporalRelation.cpp \
../src/GUI/Comment.cpp \
../src/GUI/ControlBox.cpp \
../src/GUI/ControlBoxContextMenu.cpp \
../src/GUI/CurveWidget.cpp \
../src/GUI/CurvesWidget.cpp \
../src/GUI/Help.cpp \
../src/GUI/Interpolation.cpp \
../src/GUI/LogarithmicSlider.cpp \
../src/GUI/MainWindow.cpp \
../src/GUI/MaquetteScene.cpp \
../src/GUI/MaquetteView.cpp \
../src/GUI/NetworkConfig.cpp \
../src/GUI/NetworkMessagesEditor.cpp \
../src/GUI/NetworkTree.cpp \
../src/GUI/ParentBox.cpp \
../src/GUI/ParentBoxContextMenu.cpp \
../src/GUI/PlayingThread.cpp \
../src/GUI/PreviewArea.cpp \
../src/GUI/Relation.cpp \
../src/GUI/RelationEdit.cpp \
../src/GUI/SoundBox.cpp \
../src/GUI/SoundBoxContextMenu.cpp \
../src/GUI/TextEdit.cpp \
../src/GUI/TreeMap.cpp \
../src/GUI/TreeMapElement.cpp \
../src/GUI/TriggerPoint.cpp \
../src/GUI/ViewRelations.cpp 

OBJS += \
./src/GUI/AttributesEditor.o \
./src/GUI/BasicBox.o \
./src/GUI/BoxContextMenu.o \
./src/GUI/ChooseTemporalRelation.o \
./src/GUI/Comment.o \
./src/GUI/ControlBox.o \
./src/GUI/ControlBoxContextMenu.o \
./src/GUI/CurveWidget.o \
./src/GUI/CurvesWidget.o \
./src/GUI/Help.o \
./src/GUI/Interpolation.o \
./src/GUI/LogarithmicSlider.o \
./src/GUI/MainWindow.o \
./src/GUI/MaquetteScene.o \
./src/GUI/MaquetteView.o \
./src/GUI/NetworkConfig.o \
./src/GUI/NetworkMessagesEditor.o \
./src/GUI/NetworkTree.o \
./src/GUI/ParentBox.o \
./src/GUI/ParentBoxContextMenu.o \
./src/GUI/PlayingThread.o \
./src/GUI/PreviewArea.o \
./src/GUI/Relation.o \
./src/GUI/RelationEdit.o \
./src/GUI/SoundBox.o \
./src/GUI/SoundBoxContextMenu.o \
./src/GUI/TextEdit.o \
./src/GUI/TreeMap.o \
./src/GUI/TreeMapElement.o \
./src/GUI/TriggerPoint.o \
./src/GUI/ViewRelations.o 

CPP_DEPS += \
./src/GUI/AttributesEditor.d \
./src/GUI/BasicBox.d \
./src/GUI/BoxContextMenu.d \
./src/GUI/ChooseTemporalRelation.d \
./src/GUI/Comment.d \
./src/GUI/ControlBox.d \
./src/GUI/ControlBoxContextMenu.d \
./src/GUI/CurveWidget.d \
./src/GUI/CurvesWidget.d \
./src/GUI/Help.d \
./src/GUI/Interpolation.d \
./src/GUI/LogarithmicSlider.d \
./src/GUI/MainWindow.d \
./src/GUI/MaquetteScene.d \
./src/GUI/MaquetteView.d \
./src/GUI/NetworkConfig.d \
./src/GUI/NetworkMessagesEditor.d \
./src/GUI/NetworkTree.d \
./src/GUI/ParentBox.d \
./src/GUI/ParentBoxContextMenu.d \
./src/GUI/PlayingThread.d \
./src/GUI/PreviewArea.d \
./src/GUI/Relation.d \
./src/GUI/RelationEdit.d \
./src/GUI/SoundBox.d \
./src/GUI/SoundBoxContextMenu.d \
./src/GUI/TextEdit.d \
./src/GUI/TreeMap.d \
./src/GUI/TreeMapElement.d \
./src/GUI/TriggerPoint.d \
./src/GUI/ViewRelations.d 


# Each subdirectory must supply rules for building sources it contributes
src/GUI/%.o: ../src/GUI/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


