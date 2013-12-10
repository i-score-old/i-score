TEMPLATE = app
TARGET = i-score
CONFIG += x86_64
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7

QMAKE_CXXFLAGS += -O0 -fPIC -msse3

# This variable specifies the #include directories which should be searched when compiling the project.
INCLUDEPATH += headers/GUI headers/data /usr/local/include/IScore /usr/local/include/libxml2

# This variable contains a general set of flags that are passed to the linker.
QMAKE_LFLAGS += -L/usr/local/lib/

QT += network xml svg

# This variable specifies the directory where all intermediate objetcts and moc files should be placed.
OBJECTS_DIR = bin
MOC_DIR = moc

# This variable contains the name of the resource collection file (qrc) for the application.
RESOURCES += i-score.qrc

# qmake adds the values of this variable as compiler C preprocessor macros (-D option).
DEFINES += __Types__

ICON = images/acousmoscribe.icns

TRANSLATIONS = acousmoscribe_en.ts acousmoscribe_fr.ts

# Support for conditional structures is made available via these scopes
linux-g++ {
    # This variable specifies the C++ compiler that will be used when building projects containing C++ source code
    QMAKE_CXX = /usr/bin/g++

    LIBS += -lIscore -lDeviceManager -lxml2 -lgecodeint -lgecodesearch -lgecodedriver -lgecodeflatzinc -lgecodekernel -lgecodeminimodel -lgecodescheduling -lgecodeset -lgecodesupport -lgecodegraph
}

linux-g++-64 {
QMAKE_CXX = /usr/bin/g++

    LIBS += -lIscore -lDeviceManager -lxml2 -lgecodeint -lgecodesearch -lgecodedriver -lgecodeflatzinc -lgecodekernel -lgecodeminimodel -lgecodescheduling -lgecodeset -lgecodesupport -lgecodegraph
}

macx-g++ {
    QMAKE_CXX = /usr/bin/g++
    #QMAKE_CXXFLAGS += -std=c++11  #have to update g++ to v2.4

    QMAKE_LFLAGS += -L/System/Library/Frameworks/ -L/Library/Frameworks/
    QMAKE_CXXFLAGS_X86_64 = -mmacosx-version-min=$$QMAKE_MACOSX_DEPLOYMENT_TARGET

    LIBS += -lIscore -lDeviceManager -framework gecode -lxml2
}

macx-clang {

    QMAKE_CXX = /usr/bin/clang++

    QMAKE_CXXFLAGS += -std=c++11
    QMAKE_CXXFLAGS += -stdlib=libc++
    QMAKE_CXXFLAGS += -mmacosx-version-min=$$QMAKE_MACOSX_DEPLOYMENT_TARGET

    QMAKE_LFLAGS += -stdlib=libc++
    QMAKE_LFLAGS += -L/usr/local/lib/ -L/usr/local/jamoma/lib -L/System/Library/Frameworks/ -L/Library/Frameworks/

    INCLUDEPATH += .
    INCLUDEPATH += headers/GUI
    INCLUDEPATH += headers/data
    INCLUDEPATH += /Library/Frameworks/
    INCLUDEPATH += /usr/local/jamoma/includes
    INCLUDEPATH += /usr/local/include/libxml2

    LIBS += /usr/local/jamoma/lib/JamomaFoundation.dylib
    LIBS += /usr/local/jamoma/lib/JamomaDSP.dylib
    LIBS += /usr/local/jamoma/lib/JamomaScore.dylib
    LIBS += /usr/local/jamoma/lib/JamomaModular.dylib
    LIBS += -framework gecode
    LIBS += -lxml2
}

# Input
HEADERS += /usr/local/jamoma/includes/TTScoreAPI.h \
/usr/local/jamoma/includes/TTModular.h \
/usr/local/jamoma/includes/TTDSP.h \
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
