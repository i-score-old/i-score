TEMPLATE = app
TARGET = i-score
QT += core network xml svg printsupport

DEFINES += __Types__ QT_DISABLE_DEPRECATED_BEFORE=0x000000
INCLUDEPATH += headers/GUI headers/data /usr/local/include/libxml2  /usr/include/libxml2

QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-deprecated-register -O3 -fPIC -msse3 -std=c++11
QMAKE_LFLAGS += -L/usr/local/lib/jamoma/lib -L/usr/local/lib/ -Wl,-rpath,/usr/local/jamoma/lib -Wl,-rpath,/usr/local/jamoma/extensions

RESOURCES += i-score.qrc

ICON = resources/images/i-score.icns
resources/translations = i-score_en.ts i-score_fr.ts

macx {
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
CONFIG += x86_64
QMAKE_CXXFLAGS += -mmacosx-version-min=$$QMAKE_MACOSX_DEPLOYMENT_TARGET -stdlib=libc++
QMAKE_LFLAGS   += -stdlib=libc++ -lc++
}

unix {
message("Unix build")
INCLUDEPATH +=	/usr/local/jamoma/includes \
		$$(JAMOMA_INCLUDE_PATH)/Score/library/tests/ \
		$$(JAMOMA_INCLUDE_PATH)/Modular/library/PeerObject \
		$$(JAMOMA_INCLUDE_PATH)/Modular/library/ProtocolLib \
		$$(JAMOMA_INCLUDE_PATH)/Modular/library/SchedulerLib \
		$$(JAMOMA_INCLUDE_PATH)/DSP/library/includes \
		$$(JAMOMA_INCLUDE_PATH)/Modular/library/includes \
		$$(JAMOMA_INCLUDE_PATH)/Score/library/includes \
		$$(JAMOMA_INCLUDE_PATH)/Foundation/library/includes

linux-clang:contains(QMAKE_HOST.arch, x86):{
	INCLUDEPATH += /usr/include/i386-linux-gnu/c++/4.8/
}

LIBS += -L/usr/local/jamoma/lib -lJamomaFoundation -lJamomaDSP -lJamomaScore -lJamomaModular
LIBS += -lxml2 -lgecodeint -lgecodesearch -lgecodedriver -lgecodeflatzinc -lgecodekernel -lgecodeminimodel -lgecodeset -lgecodesupport

}

# Input
HEADERS += \
headers/data/Abstract.hpp \
headers/data/AbstractBox.hpp \
headers/data/AbstractComment.hpp \
headers/data/AbstractCurve.hpp \
headers/data/AbstractRelation.hpp \
headers/data/AbstractParentBox.hpp \
headers/data/AbstractTriggerPoint.hpp \
headers/data/Engine.h \
headers/data/Maquette.hpp \
headers/data/NetworkMessages.hpp \
headers/GUI/AttributesEditor.hpp \
headers/GUI/BasicBox.hpp \
headers/GUI/BoxContextMenu.hpp \
headers/GUI/ChooseTemporalRelation.hpp \
headers/GUI/Comment.hpp \
headers/GUI/CurveWidget.hpp \
headers/GUI/CurvesWidget.hpp \
headers/GUI/Help.hpp \
headers/GUI/Interpolation.hpp \
headers/GUI/LogarithmicSlider.hpp \
headers/GUI/MainWindow.hpp \
headers/GUI/MaquetteScene.hpp \
headers/GUI/MaquetteView.hpp \
headers/GUI/NetworkConfig.hpp \
headers/GUI/NetworkMessagesEditor.hpp \
headers/GUI/NetworkTree.hpp \
headers/GUI/ParentBox.hpp \
headers/GUI/ParentBoxContextMenu.hpp \
headers/GUI/PlayingThread.hpp \
headers/GUI/Relation.hpp \
headers/GUI/RelationEdit.hpp \
headers/GUI/TextEdit.hpp \
headers/GUI/TriggerPoint.hpp \
headers/GUI/TreeMap.hpp \
headers/GUI/TreeMapElement.hpp \
headers/GUI/ViewRelations.hpp \
headers/GUI/BoxWidget.hpp \
headers/GUI/BoxCurveEdit.hpp \
headers/GUI/TimeBarWidget.hpp \
headers/GUI/DeviceEdit.hpp \
headers/GUI/HeaderPanelWidget.hpp

SOURCES += src/main.cpp \
src/data/Abstract.cpp \
src/data/AbstractBox.cpp \
src/data/AbstractComment.cpp \
src/data/AbstractCurve.cpp \
src/data/AbstractParentBox.cpp \
src/data/AbstractRelation.cpp \
src/data/AbstractTriggerPoint.cpp \
src/data/Engine.cpp \
src/data/Maquette.cpp \
src/data/NetworkMessages.cpp \
src/GUI/AttributesEditor.cpp \
src/GUI/BasicBox.cpp \
src/GUI/BoxContextMenu.cpp \
src/GUI/ChooseTemporalRelation.cpp \
src/GUI/Comment.cpp \
src/GUI/CurveWidget.cpp \
src/GUI/CurvesWidget.cpp \
src/GUI/Help.cpp \
src/GUI/Interpolation.cpp \
src/GUI/LogarithmicSlider.cpp \
src/GUI/MainWindow.cpp \
src/GUI/MaquetteScene.cpp \
src/GUI/MaquetteView.cpp \
src/GUI/NetworkConfig.cpp \
src/GUI/NetworkMessagesEditor.cpp \
src/GUI/NetworkTree.cpp \
src/GUI/ParentBox.cpp \
src/GUI/ParentBoxContextMenu.cpp \
src/GUI/PlayingThread.cpp \
src/GUI/Relation.cpp \
src/GUI/RelationEdit.cpp \
src/GUI/TextEdit.cpp \
src/GUI/TriggerPoint.cpp \
src/GUI/TreeMap.cpp \
src/GUI/TreeMapElement.cpp \
src/GUI/ViewRelations.cpp \
src/GUI/BoxWidget.cpp \
src/GUI/BoxCurveEdit.cpp \
src/GUI/TimeBarWidget.cpp \
src/GUI/DeviceEdit.cpp \
src/GUI/HeaderPanelWidget.cpp
