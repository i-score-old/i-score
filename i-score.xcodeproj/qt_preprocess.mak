#############################################################################
# Makefile for building: i-score.app/Contents/MacOS/i-score
# Generated by qmake (2.01a) (Qt 4.8.1) on: Thu Dec 6 15:57:20 2012
# Project:  i-score.pro
# Template: app
# Command: /usr/bin/qmake -o i-score.xcodeproj/project.pbxproj i-score.pro
#############################################################################

MOC       = /Developer/Tools/Qt/moc
UIC       = /Developer/Tools/Qt/uic
LEX       = flex
LEXFLAGS  = 
YACC      = yacc
YACCFLAGS = -d
DEFINES       = -D__Types__ -DQT_SVG_LIB -DQT_XML_LIB -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_SHARED
INCPATH       = -I/usr/local/Qt4.8/mkspecs/macx-xcode -I. -I/Library/Frameworks/QtCore.framework/Versions/4/Headers -I/usr/include/QtCore -I/Library/Frameworks/QtNetwork.framework/Versions/4/Headers -I/usr/include/QtNetwork -I/Library/Frameworks/QtGui.framework/Versions/4/Headers -I/usr/include/QtGui -I/Library/Frameworks/QtXml.framework/Versions/4/Headers -I/usr/include/QtXml -I/Library/Frameworks/QtSvg.framework/Versions/4/Headers -I/usr/include/QtSvg -I/usr/include -Imoc -I/usr/local/include -I/System/Library/Frameworks/CarbonCore.framework/Headers -F/Library/Frameworks
DEL_FILE  = rm -f
MOVE      = mv -f

IMAGES = 
PARSERS =
preprocess: $(PARSERS) compilers
clean preprocess_clean: parser_clean compiler_clean

parser_clean:
check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compilers: moc/moc_Maquette.cpp moc/moc_AttributesEditor.cpp moc/moc_BoxContextMenu.cpp\
	 moc/moc_ChooseTemporalRelation.cpp moc/moc_ControlBoxContextMenu.cpp moc/moc_CurveWidget.cpp\
	 moc/moc_CurvesWidget.cpp moc/moc_Help.cpp moc/moc_Interpolation.cpp\
	 moc/moc_MainWindow.cpp moc/moc_MaquetteScene.cpp moc/moc_MaquetteView.cpp\
	 moc/moc_NetworkConfig.cpp moc/moc_NetworkMessagesEditor.cpp moc/moc_NetworkTree.cpp\
	 moc/moc_ParentBoxContextMenu.cpp moc/moc_PreviewArea.cpp moc/moc_RelationEdit.cpp\
	 moc/moc_SoundBoxContextMenu.cpp moc/moc_TextEdit.cpp moc/moc_TreeMap.cpp\
	 moc/moc_TreeMapElement.cpp moc/moc_ViewRelations.cpp moc/moc_NetworkMessages.cpp\
	 moc/moc_BoxWidget.cpp moc/moc_BoxCurveEdit.cpp moc/moc_MaquetteWidget.cpp\
	 moc/moc_TimeBarWidget.cpp ./qrc_i-score.cpp
compiler_objective_c_make_all:
compiler_objective_c_clean:
compiler_moc_header_make_all: moc/moc_Maquette.cpp moc/moc_AttributesEditor.cpp moc/moc_BoxContextMenu.cpp moc/moc_ChooseTemporalRelation.cpp moc/moc_ControlBoxContextMenu.cpp moc/moc_CurveWidget.cpp moc/moc_CurvesWidget.cpp moc/moc_Help.cpp moc/moc_Interpolation.cpp moc/moc_MainWindow.cpp moc/moc_MaquetteScene.cpp moc/moc_MaquetteView.cpp moc/moc_NetworkConfig.cpp moc/moc_NetworkMessagesEditor.cpp moc/moc_NetworkTree.cpp moc/moc_ParentBoxContextMenu.cpp moc/moc_PreviewArea.cpp moc/moc_RelationEdit.cpp moc/moc_SoundBoxContextMenu.cpp moc/moc_TextEdit.cpp moc/moc_TreeMap.cpp moc/moc_TreeMapElement.cpp moc/moc_ViewRelations.cpp moc/moc_NetworkMessages.cpp moc/moc_BoxWidget.cpp moc/moc_BoxCurveEdit.cpp moc/moc_MaquetteWidget.cpp moc/moc_TimeBarWidget.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc/moc_Maquette.cpp moc/moc_AttributesEditor.cpp moc/moc_BoxContextMenu.cpp moc/moc_ChooseTemporalRelation.cpp moc/moc_ControlBoxContextMenu.cpp moc/moc_CurveWidget.cpp moc/moc_CurvesWidget.cpp moc/moc_Help.cpp moc/moc_Interpolation.cpp moc/moc_MainWindow.cpp moc/moc_MaquetteScene.cpp moc/moc_MaquetteView.cpp moc/moc_NetworkConfig.cpp moc/moc_NetworkMessagesEditor.cpp moc/moc_NetworkTree.cpp moc/moc_ParentBoxContextMenu.cpp moc/moc_PreviewArea.cpp moc/moc_RelationEdit.cpp moc/moc_SoundBoxContextMenu.cpp moc/moc_TextEdit.cpp moc/moc_TreeMap.cpp moc/moc_TreeMapElement.cpp moc/moc_ViewRelations.cpp moc/moc_NetworkMessages.cpp moc/moc_BoxWidget.cpp moc/moc_BoxCurveEdit.cpp moc/moc_MaquetteWidget.cpp moc/moc_TimeBarWidget.cpp
moc/moc_Maquette.cpp: headers/data/Palette.hpp \
		headers/data/EnumPalette.hpp \
		headers/data/NetworkMessages.hpp \
		headers/GUI/BasicBox.hpp \
		headers/data/AbstractBox.hpp \
		headers/data/Abstract.hpp \
		headers/GUI/CurvesWidget.hpp \
		headers/GUI/BoxWidget.hpp \
		headers/data/Maquette.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/data/Maquette.hpp -o moc/moc_Maquette.cpp

moc/moc_AttributesEditor.cpp: headers/data/EnumPalette.hpp \
		headers/GUI/AttributesEditor.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/AttributesEditor.hpp -o moc/moc_AttributesEditor.cpp

moc/moc_BoxContextMenu.cpp: headers/GUI/BoxContextMenu.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/BoxContextMenu.hpp -o moc/moc_BoxContextMenu.cpp

moc/moc_ChooseTemporalRelation.cpp: headers/GUI/ChooseTemporalRelation.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/ChooseTemporalRelation.hpp -o moc/moc_ChooseTemporalRelation.cpp

moc/moc_ControlBoxContextMenu.cpp: headers/GUI/BoxContextMenu.hpp \
		headers/GUI/ControlBoxContextMenu.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/ControlBoxContextMenu.hpp -o moc/moc_ControlBoxContextMenu.cpp

moc/moc_CurveWidget.cpp: headers/GUI/CurveWidget.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/CurveWidget.hpp -o moc/moc_CurveWidget.cpp

moc/moc_CurvesWidget.cpp: headers/GUI/CurvesWidget.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/CurvesWidget.hpp -o moc/moc_CurvesWidget.cpp

moc/moc_Help.cpp: headers/GUI/Help.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/Help.hpp -o moc/moc_Help.cpp

moc/moc_Interpolation.cpp: headers/GUI/Interpolation.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/Interpolation.hpp -o moc/moc_Interpolation.cpp

moc/moc_MainWindow.cpp: headers/data/Palette.hpp \
		headers/data/EnumPalette.hpp \
		headers/GUI/MaquetteScene.hpp \
		headers/data/Maquette.hpp \
		headers/data/NetworkMessages.hpp \
		headers/GUI/BasicBox.hpp \
		headers/data/AbstractBox.hpp \
		headers/data/Abstract.hpp \
		headers/GUI/CurvesWidget.hpp \
		headers/GUI/BoxWidget.hpp \
		headers/data/AbstractRelation.hpp \
		headers/data/AbstractSoundBox.hpp \
		headers/data/AbstractControlBox.hpp \
		headers/data/AbstractParentBox.hpp \
		headers/GUI/TimeBarWidget.hpp \
		headers/GUI/MaquetteWidget.hpp \
		headers/GUI/MaquetteView.hpp \
		headers/GUI/LogarithmicSlider.hpp \
		headers/GUI/MainWindow.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/MainWindow.hpp -o moc/moc_MainWindow.cpp

moc/moc_MaquetteScene.cpp: headers/data/Maquette.hpp \
		headers/data/Palette.hpp \
		headers/data/EnumPalette.hpp \
		headers/data/NetworkMessages.hpp \
		headers/GUI/BasicBox.hpp \
		headers/data/AbstractBox.hpp \
		headers/data/Abstract.hpp \
		headers/GUI/CurvesWidget.hpp \
		headers/GUI/BoxWidget.hpp \
		headers/data/AbstractRelation.hpp \
		headers/data/AbstractSoundBox.hpp \
		headers/data/AbstractControlBox.hpp \
		headers/data/AbstractParentBox.hpp \
		headers/GUI/TimeBarWidget.hpp \
		headers/GUI/MaquetteScene.hpp \
		headers/GUI/MaquetteScene.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/MaquetteScene.hpp -o moc/moc_MaquetteScene.cpp

moc/moc_MaquetteView.cpp: headers/GUI/MaquetteView.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/MaquetteView.hpp -o moc/moc_MaquetteView.cpp

moc/moc_NetworkConfig.cpp: headers/data/Maquette.hpp \
		headers/data/Palette.hpp \
		headers/data/EnumPalette.hpp \
		headers/data/NetworkMessages.hpp \
		headers/GUI/BasicBox.hpp \
		headers/data/AbstractBox.hpp \
		headers/data/Abstract.hpp \
		headers/GUI/CurvesWidget.hpp \
		headers/GUI/BoxWidget.hpp \
		headers/GUI/NetworkConfig.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/NetworkConfig.hpp -o moc/moc_NetworkConfig.cpp

moc/moc_NetworkMessagesEditor.cpp: headers/GUI/NetworkMessagesEditor.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/NetworkMessagesEditor.hpp -o moc/moc_NetworkMessagesEditor.cpp

moc/moc_NetworkTree.cpp: headers/data/NetworkMessages.hpp \
		headers/data/AbstractBox.hpp \
		headers/data/Abstract.hpp \
		headers/GUI/NetworkTree.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/NetworkTree.hpp -o moc/moc_NetworkTree.cpp

moc/moc_ParentBoxContextMenu.cpp: headers/GUI/BoxContextMenu.hpp \
		headers/GUI/ParentBoxContextMenu.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/ParentBoxContextMenu.hpp -o moc/moc_ParentBoxContextMenu.cpp

moc/moc_PreviewArea.cpp: headers/data/EnumPalette.hpp \
		headers/data/Palette.hpp \
		headers/GUI/PreviewArea.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/PreviewArea.hpp -o moc/moc_PreviewArea.cpp

moc/moc_RelationEdit.cpp: headers/data/Maquette.hpp \
		headers/data/Palette.hpp \
		headers/data/EnumPalette.hpp \
		headers/data/NetworkMessages.hpp \
		headers/GUI/BasicBox.hpp \
		headers/data/AbstractBox.hpp \
		headers/data/Abstract.hpp \
		headers/GUI/CurvesWidget.hpp \
		headers/GUI/BoxWidget.hpp \
		headers/GUI/RelationEdit.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/RelationEdit.hpp -o moc/moc_RelationEdit.cpp

moc/moc_SoundBoxContextMenu.cpp: headers/GUI/BoxContextMenu.hpp \
		headers/GUI/SoundBoxContextMenu.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/SoundBoxContextMenu.hpp -o moc/moc_SoundBoxContextMenu.cpp

moc/moc_TextEdit.cpp: headers/GUI/TextEdit.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/TextEdit.hpp -o moc/moc_TextEdit.cpp

moc/moc_TreeMap.cpp: headers/GUI/TreeMapElement.hpp \
		headers/GUI/TreeMap.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/TreeMap.hpp -o moc/moc_TreeMap.cpp

moc/moc_TreeMapElement.cpp: headers/GUI/TreeMapElement.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/TreeMapElement.hpp -o moc/moc_TreeMapElement.cpp

moc/moc_ViewRelations.cpp: headers/GUI/ViewRelations.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/ViewRelations.hpp -o moc/moc_ViewRelations.cpp

moc/moc_NetworkMessages.cpp: headers/data/NetworkMessages.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/data/NetworkMessages.hpp -o moc/moc_NetworkMessages.cpp

moc/moc_BoxWidget.cpp: headers/GUI/BoxWidget.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/BoxWidget.hpp -o moc/moc_BoxWidget.cpp

moc/moc_BoxCurveEdit.cpp: headers/GUI/MaquetteScene.hpp \
		headers/data/Maquette.hpp \
		headers/data/Palette.hpp \
		headers/data/EnumPalette.hpp \
		headers/data/NetworkMessages.hpp \
		headers/GUI/BasicBox.hpp \
		headers/data/AbstractBox.hpp \
		headers/data/Abstract.hpp \
		headers/GUI/CurvesWidget.hpp \
		headers/GUI/BoxWidget.hpp \
		headers/data/AbstractRelation.hpp \
		headers/data/AbstractSoundBox.hpp \
		headers/data/AbstractControlBox.hpp \
		headers/data/AbstractParentBox.hpp \
		headers/GUI/TimeBarWidget.hpp \
		headers/GUI/BoxCurveEdit.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/BoxCurveEdit.hpp -o moc/moc_BoxCurveEdit.cpp

moc/moc_MaquetteWidget.cpp: headers/GUI/MaquetteView.hpp \
		headers/GUI/LogarithmicSlider.hpp \
		headers/GUI/TimeBarWidget.hpp \
		headers/GUI/MaquetteScene.hpp \
		headers/data/Maquette.hpp \
		headers/data/Palette.hpp \
		headers/data/EnumPalette.hpp \
		headers/data/NetworkMessages.hpp \
		headers/GUI/BasicBox.hpp \
		headers/data/AbstractBox.hpp \
		headers/data/Abstract.hpp \
		headers/GUI/CurvesWidget.hpp \
		headers/GUI/BoxWidget.hpp \
		headers/data/AbstractRelation.hpp \
		headers/data/AbstractSoundBox.hpp \
		headers/data/AbstractControlBox.hpp \
		headers/data/AbstractParentBox.hpp \
		headers/GUI/MaquetteWidget.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/MaquetteWidget.hpp -o moc/moc_MaquetteWidget.cpp

moc/moc_TimeBarWidget.cpp: headers/GUI/MaquetteScene.hpp \
		headers/data/Maquette.hpp \
		headers/data/Palette.hpp \
		headers/data/EnumPalette.hpp \
		headers/data/NetworkMessages.hpp \
		headers/GUI/BasicBox.hpp \
		headers/data/AbstractBox.hpp \
		headers/data/Abstract.hpp \
		headers/GUI/CurvesWidget.hpp \
		headers/GUI/BoxWidget.hpp \
		headers/data/AbstractRelation.hpp \
		headers/data/AbstractSoundBox.hpp \
		headers/data/AbstractControlBox.hpp \
		headers/data/AbstractParentBox.hpp \
		headers/GUI/TimeBarWidget.hpp \
		headers/GUI/TimeBarWidget.hpp
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ headers/GUI/TimeBarWidget.hpp -o moc/moc_TimeBarWidget.cpp

compiler_rcc_make_all: qrc_i-score.cpp
compiler_rcc_clean:
	-$(DEL_FILE) qrc_i-score.cpp
qrc_i-score.cpp: i-score.qrc
	/Developer/Tools/Qt/rcc -name i-score i-score.qrc -o qrc_i-score.cpp

compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_rez_source_make_all:
compiler_rez_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_rcc_clean 

