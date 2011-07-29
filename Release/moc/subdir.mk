################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../moc/moc_AttributesEditor.cpp \
../moc/moc_BoxContextMenu.cpp \
../moc/moc_ChooseTemporalRelation.cpp \
../moc/moc_ControlBoxContextMenu.cpp \
../moc/moc_CurveWidget.cpp \
../moc/moc_CurvesWidget.cpp \
../moc/moc_Help.cpp \
../moc/moc_Interpolation.cpp \
../moc/moc_MainWindow.cpp \
../moc/moc_Maquette.cpp \
../moc/moc_MaquetteScene.cpp \
../moc/moc_MaquetteView.cpp \
../moc/moc_NetworkConfig.cpp \
../moc/moc_NetworkMessagesEditor.cpp \
../moc/moc_NetworkTree.cpp \
../moc/moc_ParentBoxContextMenu.cpp \
../moc/moc_PreviewArea.cpp \
../moc/moc_RelationEdit.cpp \
../moc/moc_SoundBoxContextMenu.cpp \
../moc/moc_TextEdit.cpp \
../moc/moc_TreeMap.cpp \
../moc/moc_TreeMapElement.cpp \
../moc/moc_ViewRelations.cpp 

OBJS += \
./moc/moc_AttributesEditor.o \
./moc/moc_BoxContextMenu.o \
./moc/moc_ChooseTemporalRelation.o \
./moc/moc_ControlBoxContextMenu.o \
./moc/moc_CurveWidget.o \
./moc/moc_CurvesWidget.o \
./moc/moc_Help.o \
./moc/moc_Interpolation.o \
./moc/moc_MainWindow.o \
./moc/moc_Maquette.o \
./moc/moc_MaquetteScene.o \
./moc/moc_MaquetteView.o \
./moc/moc_NetworkConfig.o \
./moc/moc_NetworkMessagesEditor.o \
./moc/moc_NetworkTree.o \
./moc/moc_ParentBoxContextMenu.o \
./moc/moc_PreviewArea.o \
./moc/moc_RelationEdit.o \
./moc/moc_SoundBoxContextMenu.o \
./moc/moc_TextEdit.o \
./moc/moc_TreeMap.o \
./moc/moc_TreeMapElement.o \
./moc/moc_ViewRelations.o 

CPP_DEPS += \
./moc/moc_AttributesEditor.d \
./moc/moc_BoxContextMenu.d \
./moc/moc_ChooseTemporalRelation.d \
./moc/moc_ControlBoxContextMenu.d \
./moc/moc_CurveWidget.d \
./moc/moc_CurvesWidget.d \
./moc/moc_Help.d \
./moc/moc_Interpolation.d \
./moc/moc_MainWindow.d \
./moc/moc_Maquette.d \
./moc/moc_MaquetteScene.d \
./moc/moc_MaquetteView.d \
./moc/moc_NetworkConfig.d \
./moc/moc_NetworkMessagesEditor.d \
./moc/moc_NetworkTree.d \
./moc/moc_ParentBoxContextMenu.d \
./moc/moc_PreviewArea.d \
./moc/moc_RelationEdit.d \
./moc/moc_SoundBoxContextMenu.d \
./moc/moc_TextEdit.d \
./moc/moc_TreeMap.d \
./moc/moc_TreeMapElement.d \
./moc/moc_ViewRelations.d 


# Each subdirectory must supply rules for building sources it contributes
moc/%.o: ../moc/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


