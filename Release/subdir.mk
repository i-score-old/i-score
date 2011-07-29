################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../Abstract.o \
../AbstractBox.o \
../AbstractComment.o \
../AbstractControlBox.o \
../AbstractCurve.o \
../AbstractParentBox.o \
../AbstractRelation.o \
../AbstractSoundBox.o \
../AbstractTriggerPoint.o \
../AttributesEditor.o \
../BasicBox.o \
../BoxContextMenu.o \
../ChooseTemporalRelation.o \
../Comment.o \
../ControlBox.o \
../ControlBoxContextMenu.o \
../CurveWidget.o \
../CurvesWidget.o \
../Help.o \
../Interpolation.o \
../LogarithmicSlider.o \
../MainWindow.o \
../Maquette.o \
../MaquetteScene.o \
../MaquetteView.o \
../NetworkConfig.o \
../NetworkMessagesEditor.o \
../NetworkTree.o \
../Palette.o \
../ParentBox.o \
../ParentBoxContextMenu.o \
../PlayingThread.o \
../PreviewArea.o \
../Relation.o \
../RelationEdit.o \
../SoundBox.o \
../SoundBoxContextMenu.o \
../TextEdit.o \
../TreeMap.o \
../TreeMapElement.o \
../TriggerPoint.o \
../ViewRelations.o \
../main.o \
../moc_AttributesEditor.o \
../moc_BoxContextMenu.o \
../moc_ChooseTemporalRelation.o \
../moc_ControlBoxContextMenu.o \
../moc_CurveWidget.o \
../moc_CurvesWidget.o \
../moc_Help.o \
../moc_Interpolation.o \
../moc_MainWindow.o \
../moc_Maquette.o \
../moc_MaquetteScene.o \
../moc_MaquetteView.o \
../moc_NetworkConfig.o \
../moc_NetworkMessagesEditor.o \
../moc_NetworkTree.o \
../moc_ParentBoxContextMenu.o \
../moc_PreviewArea.o \
../moc_RelationEdit.o \
../moc_SoundBoxContextMenu.o \
../moc_TextEdit.o \
../moc_TreeMap.o \
../moc_TreeMapElement.o \
../moc_ViewRelations.o \
../qrc_acousmoscribe.o 

CPP_SRCS += \
../main.cpp \
../qrc_acousmoscribe.cpp 

OBJS += \
./main.o \
./qrc_acousmoscribe.o 

CPP_DEPS += \
./main.d \
./qrc_acousmoscribe.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


