cmake_minimum_required(VERSION 2.8.11)

project(i-score)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-unused-parameter -Wno-deprecated-register -Wno-unknown-pragmas -Wno-conversion -Wl,-rpath,/usr/local/lib/jamoma/lib  -Wl,-rpath,/usr/local/jamoma/lib -Wl,-rpath,/usr/local/jamoma/extensions")


find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Network)
find_package(Qt5Xml)
find_package(Qt5Svg)
find_package(Qt5PrintSupport)

include_directories("${PROJECT_SOURCE_DIR}/headers")
include_directories("${PROJECT_SOURCE_DIR}/headers/GUI")
include_directories("${PROJECT_SOURCE_DIR}/headers/data")
include_directories("/usr/local/lib/jamoma/include")
include_directories("/usr/local/jamoma/include")
include_directories("/usr/include/libxml2")

add_definitions(-D__Types__)
add_definitions(-DTT_NO_DSP)
add_definitions(-DQ_COMPILER_INITIALIZER_LISTS)


#RESOURCES += i-score.qrc

#ICON = resources/images/i-score.icns
#resources/translations = i-score_en.ts i-score_fr.ts

set(PROJECT_HDRS
${CMAKE_CURRENT_SOURCE_DIR}/headers/data/Abstract.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/data/AbstractBox.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/data/AbstractComment.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/data/AbstractCurve.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/data/AbstractRelation.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/data/AbstractParentBox.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/data/AbstractTriggerPoint.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/data/Engine.h
${CMAKE_CURRENT_SOURCE_DIR}/headers/data/Maquette.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/data/NetworkMessages.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/AttributesEditor.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/BasicBox.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/Comment.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/CurveWidget.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/Help.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/LogarithmicSlider.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/MainWindow.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/MaquetteScene.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/MaquetteView.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/NetworkTree.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/ParentBox.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/PlayingThread.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/Relation.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/TriggerPoint.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/BoxWidget.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/TimeBarWidget.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/DeviceEdit.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/HeaderPanelWidget.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/ConditionalRelation.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/TriggerPointEdit.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/IScoreApplication.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/CurvesComboBox.hpp
${CMAKE_CURRENT_SOURCE_DIR}/headers/DelayedDelete.h
${CMAKE_CURRENT_SOURCE_DIR}/headers/GlobalEventFilter.h
${CMAKE_CURRENT_SOURCE_DIR}/headers/GUI/NetworkUpdater.h)

set(PROJECT_SRCS
${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/data/AbstractBox.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/data/AbstractComment.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/data/AbstractCurve.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/data/AbstractParentBox.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/data/AbstractRelation.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/data/AbstractTriggerPoint.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/data/Engine.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/data/Maquette.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/data/NetworkMessages.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/AttributesEditor.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/BasicBox.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/Comment.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/CurveWidget.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/Help.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/LogarithmicSlider.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/MainWindow.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/MaquetteScene.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/MaquetteView.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/NetworkTree.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/ParentBox.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/PlayingThread.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/Relation.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/TriggerPoint.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/BoxWidget.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/TimeBarWidget.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/DeviceEdit.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/HeaderPanelWidget.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/ConditionalRelation.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/TriggerPointEdit.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/IScoreApplication.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/CurvesComboBox.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/GUI/NetworkUpdater.cpp)


qt5_add_resources(i-score_RESOURCES_RCC "i-score.qrc")

add_executable(i-score ${PROJECT_SRCS} ${PROJECT_HDRS} ${i-score_RESOURCES_RCC})

qt5_use_modules(i-score Svg Xml Network Widgets PrintSupport)


target_link_libraries(i-score Qt5::Widgets Qt5::Core Qt5::Network Qt5::Xml Qt5::Svg Qt5::PrintSupport)

# To do in a cleanlier fashion... What about creating a package file for Jamoma?
set(JAMOMA_LIB_FOLDER "/usr/local/lib/jamoma/lib")
FIND_LIBRARY(JAMOMA_FOUNDATION_LIBRARY JamomaFoundation ${JAMOMA_LIB_FOLDER})
FIND_LIBRARY(JAMOMA_SCORE_LIBRARY JamomaScore ${JAMOMA_LIB_FOLDER})
FIND_LIBRARY(JAMOMA_MODULAR_LIBRARY JamomaModular ${JAMOMA_LIB_FOLDER})

target_link_libraries(i-score ${JAMOMA_FOUNDATION_LIBRARY} ${JAMOMA_SCORE_LIBRARY} ${JAMOMA_MODULAR_LIBRARY})

#############################
######## Installing #########
#############################
INSTALL(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/i-score" DESTINATION "bin")

#############################
######## Packaging ##########
#############################
SET(CPACK_PACKAGE_NAME "i-score")
SET(CPACK_PACKAGE_VENDOR "i-score")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "i-score")

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An intermedia sequencer for the precise and flexible scripting of interactive scenarios.")
SET(CPACK_PACKAGE_VERSION "0.2.2")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "2")

SET(CPACK_GENERATOR "DEB")
SET(CPACK_SET_DESTDIR true)
SET(CPACK_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

SET(CPACK_SOURCE_GENERATOR TGZ)
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "i-score-0.2")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "jamomacore, libxml2")
SET(CPACK_DEBIAN_PACKAGE_SECTION "sound")

INCLUDE(CPack)

