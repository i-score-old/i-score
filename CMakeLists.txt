cmake_minimum_required(VERSION 2.8.11)

project(i-score)

##################################
######### Settings ###############
##################################
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Network Xml Svg PrintSupport)

include_directories("${PROJECT_SOURCE_DIR}/headers")
include_directories("${PROJECT_SOURCE_DIR}/headers/GUI")
include_directories("${PROJECT_SOURCE_DIR}/headers/data")

add_definitions(-D__Types__)
add_definitions(-DTT_NO_DSP)

#TODO find_package
if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-unused-parameter -Wno-deprecated-register -Wno-unknown-pragmas -Wno-conversion -Wl,-rpath,/usr/local/lib/jamoma/lib  -Wl,-rpath,/usr/local/jamoma/lib -Wl,-rpath,/usr/local/jamoma/extensions")

	include_directories("/usr/local/lib/jamoma/include")
	include_directories("/usr/local/jamoma/include")
	include_directories("/usr/include/libxml2")
elseif(WIN32)
	add_definitions(-DQ_COMPILER_INITIALIZER_LISTS)
	add_definitions(-DNOMINMAX)
	include_directories("C:\\Program Files (x86)\\GnuWin32\\include")
	include_directories("C:\\Program Files (x86)\\libxml2-2.7.8.win32\\include")
	include_directories("C:\\Program Files (x86)\\JamomaCore 0.6-dev\\include")
endif()




##################################
######### Source files ###########
##################################
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

IF(APPLE)
	set(MACOSX_BUNDLE_INFO_PLIST "Info.plist")
	set(MACOSX_BUNDLE_ICON_FILE "resources/images/i-score.icns") 
	set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/resources/images/i-score.icns" 
								PROPERTIES 
								MACOSX_PACKAGE_LOCATION Resources)
							  
ENDIF(APPLE)

qt5_add_resources(i-score_RESOURCES_RCC "i-score.qrc")

#resources/translations = i-score_en.ts i-score_fr.ts

##################################
############ Target ##############
##################################
add_executable(i-score MACOSX_BUNDLE WIN32
			   ${PROJECT_SRCS} 
			   ${PROJECT_HDRS} 
			   ${i-score_RESOURCES_RCC}
			   "${CMAKE_CURRENT_SOURCE_DIR}/resources/images/i-score.icns")



##################################
############ Libraries ###########
##################################
#qt5_use_modules(i-score Core Svg Xml Network Widgets PrintSupport)

# Todo in a cleanlier fashion... What about creating a package file for Jamoma?
if(UNIX)
	set(JAMOMA_LIB_FOLDER "/usr/local/lib/jamoma/lib")
elseif(WIN32)
	set(JAMOMA_LIB_FOLDER_WIN32 "c:\\Program Files (x86)\\JamomaCore 0.6-dev\\lib")
	FILE(TO_CMAKE_PATH ${JAMOMA_LIB_FOLDER_WIN32} JAMOMA_LIB_FOLDER)
endif()

FIND_LIBRARY(JAMOMA_FOUNDATION_LIBRARY JamomaFoundation ${JAMOMA_LIB_FOLDER})
FIND_LIBRARY(JAMOMA_SCORE_LIBRARY JamomaScore ${JAMOMA_LIB_FOLDER})
FIND_LIBRARY(JAMOMA_MODULAR_LIBRARY JamomaModular ${JAMOMA_LIB_FOLDER})

target_link_libraries(i-score ${JAMOMA_FOUNDATION_LIBRARY} 
							  ${JAMOMA_SCORE_LIBRARY} 
							  ${JAMOMA_MODULAR_LIBRARY}
							  Qt5::Core 
							  Qt5::Gui
							  Qt5::Svg 
							  Qt5::Xml 
							  Qt5::Network 
							  Qt5::Widgets 
							  Qt5::PrintSupport)



#############################
######## Packaging ##########
#############################
## Bundlin' like mad
set(plugin_dest_dir bin)
set(qtconf_dest_dir bin)
set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/i-score")

if(APPLE)
  set(plugin_dest_dir i-score.app/Contents/MacOS)
  set(qtconf_dest_dir i-score.app/Contents/Resources)
  set(APPS "\${CMAKE_INSTALL_PREFIX}/i-score.app")
elseif(WIN32)
  set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/i-score.exe")
endif()

INSTALL(TARGETS i-score 
    BUNDLE DESTINATION . COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime)

# Beware : Fifth circle of the DLL Hell
if(WIN32)
	# Qt stuff
	if(CMAKE_BUILD_TYPE MATCHES Debug)
		get_target_property(QtWinPlugin Qt5::QWindowsIntegrationPlugin LOCATION_Debug)
		file(TO_CMAKE_PATH "C:\\Qt\\Qt5.3.1\\5.3\\msvc2013\\bin\\libEGLd.dll" LIBEGL_DLL)
	elseif(CMAKE_BUILD_TYPE MATCHES Release)
		get_target_property(QtWinPlugin Qt5::QWindowsIntegrationPlugin LOCATION)
		file(TO_CMAKE_PATH "C:\\Qt\\Qt5.3.1\\5.3\\msvc2013\\bin\\libEGL.dll" LIBEGL_DLL)
	endif()
	
	install(FILES "${QtWinPlugin}" DESTINATION "${plugin_dest_dir}/platforms" COMPONENT Runtime)
	install(FILES ${LIBEGL_DLL} DESTINATION bin COMPONENT Runtime)

	# Libraries
	file(TO_CMAKE_PATH "C:\\Qt\\Qt5.3.1\\5.3\\msvc2013\\bin" QT_FOLDER)
	file(TO_CMAKE_PATH "C:\\Program Files (x86)\\libxml2-2.7.8.win32\\bin" LIBXML_FOLDER)
	# Fuck this
	file(TO_CMAKE_PATH "C:\\Users\\Doom\\Documents\\GitHub\\Jamoma\\Core\\Foundation\\library\\libiconv\\bin" LIBICONV_FOLDER)
	file(TO_CMAKE_PATH "C:\\Program Files (x86)\\zlib" ZLIB_FOLDER)
	
	file(TO_CMAKE_PATH "C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\redist\\x86\\Microsoft.VC120.CRT" MSVC_REDIST_RELEASE)
	file(TO_CMAKE_PATH "C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\redist\\Debug_NonRedist\\x86\\Microsoft.VC120.DebugCRT" MSVC_REDIST_DEBUG)
	
	install(FILES "${MSVC_REDIST_RELEASE}/msvcr120.dll" 
				  "${MSVC_REDIST_RELEASE}/msvcp120.dll" 
				  "${MSVC_REDIST_DEBUG}/msvcr120d.dll" 
				  "${MSVC_REDIST_DEBUG}/msvcp120d.dll"
			DESTINATION bin
			COMPONENT Runtime)
	
	# Jamoma extensions
	file(TO_CMAKE_PATH "C:\\Program Files (x86)\\JamomaCore 0.6-dev\\extensions" JAMOMA_EXTENSIONS_FOLDER)
	set(JAMOMA_EXTENSIONS 
		"${JAMOMA_EXTENSIONS_FOLDER}/Scenario.ttdll"  
		"${JAMOMA_EXTENSIONS_FOLDER}/Automation.ttdll"  
		"${JAMOMA_EXTENSIONS_FOLDER}/Interval.ttdll"  
		"${JAMOMA_EXTENSIONS_FOLDER}/OSC.ttdll" 
		"${JAMOMA_EXTENSIONS_FOLDER}/Minuit.ttdll" 
		"${JAMOMA_EXTENSIONS_FOLDER}/MIDI.ttdll" 
		"${JAMOMA_EXTENSIONS_FOLDER}/AnalysisLib.ttdll" 
		"${JAMOMA_EXTENSIONS_FOLDER}/DataspaceLib.ttdll" 
		"${JAMOMA_EXTENSIONS_FOLDER}/FunctionLib.ttdll" 
		"${JAMOMA_EXTENSIONS_FOLDER}/System.ttdll" 
		"${JAMOMA_EXTENSIONS_FOLDER}/NetworkLib.ttdll")
		
	install(FILES ${JAMOMA_EXTENSIONS}
			DESTINATION bin
			COMPONENT Runtime)
	
	# Jamoma extensions also require their own DLLs.
	file(TO_CMAKE_PATH "C:\\Program Files (x86)\\Portmidi\\lib\\portmidi.dll" PORTMIDI_DLL)
	install(FILES ${PORTMIDI_DLL} DESTINATION bin COMPONENT Runtime)
	file(TO_CMAKE_PATH "C:\\Program Files (x86)\\pthread-win32\\dll\\x86\\pthreadVC2.dll" PTHREAD_DLL)
	install(FILES ${PTHREAD_DLL} DESTINATION bin COMPONENT Runtime)
	file(TO_CMAKE_PATH "C:\\Program Files (x86)\\Gecode\\bin" GECODE_DLL_FOLDER)
	set(GECODE_DLLS 
		"${GECODE_DLL_FOLDER}/GecodeDriver-4-3-0-d-x86.dll"   
		"${GECODE_DLL_FOLDER}/GecodeFlatZinc-4-3-0-d-x86.dll"   
		"${GECODE_DLL_FOLDER}/GecodeInt-4-3-0-d-x86.dll"   
		"${GECODE_DLL_FOLDER}/GecodeFloat-4-3-0-d-x86.dll"   
		"${GECODE_DLL_FOLDER}/GecodeGist-4-3-0-d-x86.dll"  
		"${GECODE_DLL_FOLDER}/GecodeKernel-4-3-0-d-x86.dll"  
		"${GECODE_DLL_FOLDER}/GecodeMinimodel-4-3-0-d-x86.dll"  
		"${GECODE_DLL_FOLDER}/GecodeSet-4-3-0-d-x86.dll"  
		"${GECODE_DLL_FOLDER}/GecodeSupport-4-3-0-d-x86.dll"  
		"${GECODE_DLL_FOLDER}/GecodeSearch-4-3-0-d-x86.dll")
	install(FILES ${GECODE_DLLS} DESTINATION bin COMPONENT Runtime)
	
	
	set(DIRS "${QT_FOLDER}" "${JAMOMA_LIB_FOLDER}" "${JAMOMA_EXTENSIONS_FOLDER}" "${LIBXML_FOLDER}" "${LIBICONV_FOLDER}" "${ZLIB_FOLDER}" )
	#~ set(DIRS ${QtCore_location} ${QtSvg_location} ${QtXml_location} ${QtNetwork_location} ${QtWidgets_location} ${QtPrintSupport_location})
	install(CODE "
				    file(GLOB_RECURSE QTPLUGINS
				      \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/platforms/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
				      set(PLUGINS ${QTPLUGINS} ${JAMOMA_EXTENSIONS})
					include(BundleUtilities) 
					fixup_bundle(\"${APPS}\"   \"${PLUGINS}\"   \"${DIRS}\")
				 "
			COMPONENT Runtime)
endif()
#INSTALL(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/i-score" DESTINATION "bin")

## CPack-specific stuff
set(CPACK_PACKAGE_NAME "i-score")
set(CPACK_PACKAGE_VENDOR "i-score")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An intermedia sequencer for the precise and flexible scripting of interactive scenarios.")
set(CPACK_PACKAGE_VERSION "0.2.2")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "2")

set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_SOURCE_PACKAGE_FILE_NAME "i-score-0.2")

if(UNIX)
	if(APPLE)
		# TODO
	else()
		# Debian... Others will come.
		set(CPACK_GENERATOR "DEB")
		set(CPACK_SET_DESTDIR true)
		set(CPACK_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
		
		set(CPACK_SOURCE_GENERATOR TGZ)
		set(CPACK_SOURCE_PACKAGE_FILE_NAME "i-score-0.2")
		
		set(CPACK_DEBIAN_PACKAGE_MAINTAINER "i-score")
		set(CPACK_DEBIAN_PACKAGE_DEPENDS "jamomacore, libxml2")
		set(CPACK_DEBIAN_PACKAGE_SECTION "sound")
	endif()
elseif(WIN32)
	set(CPACK_GENERATOR "NSIS")
endif()
INCLUDE(CPack)

