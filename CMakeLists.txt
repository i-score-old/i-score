project (i-score) #should be on top, otherwise potential cmake options need 2 cmake runs to be effective

cmake_minimum_required(VERSION 2.6)

OPTION(BUILD_DOCUMENTATION "Build reference documentation using doxygen (use: make documentation)" ON)
OPTION(USE_DEBUG "Use Debug" ON)
IF ( USE_DEBUG )
	MESSAGE( "\nConfigured for Debug Build")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_DEBUG -DTI_DEBUG=1 -g")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_DEBUG -DTI_DEBUG=1 -g")
	SET(CMAKE_BUILD_TYPE "Debug")
	SET(CMAKE_VERBOSE_MAKEFILE ON)
ELSE ( USE_DEBUG )
	MESSAGE( "\nConfigured for Release Build")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTI_DEBUG=0 -O3")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTI_DEBUG=0 -O3")
	SET(CMAKE_BUILD_TYPE "Release")
	SET(CMAKE_VERBOSE_MAKEFILE OFF)
	IF(APPLE)
		SET(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/release")
	ENDIF()
ENDIF ( USE_DEBUG )

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -Wno-unknown-pragmas -Wno-conversion")

function(setExtensionSuffix)
	if(APPLE)
		SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES PREFIX "")
		SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SUFFIX ".ttdylib")
	elseif(UNIX)
		SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SUFFIX ".ttso")
	elseif(WIN32)
		SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SUFFIX ".ttdll")
	endif(APPLE)
endFunction(setExtensionSuffix)

function(addAppleFramework FRAMEWORK_NAME)
	IF(APPLE)
		INCLUDE_DIRECTORIES ( /Developer/Headers/FlatCarbon )
		FIND_LIBRARY(THE_LIBRARY ${FRAMEWORK_NAME})
		MARK_AS_ADVANCED (THE_LIBRARY)
		SET(EXTRA_LIBS ${EXTRA_LIBS} ${THE_LIBRARY})
	ENDIF (APPLE)
endFunction(addAppleFramework)

MACRO(SUBDIRLIST result curdir)
	FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
	SET(dirlist "")
	FOREACH(child ${children})
		IF(IS_DIRECTORY ${curdir}/${child})
			SET(dirlist ${dirlist} ${child})
		ENDIF()
	ENDFOREACH()
	SET(${result} ${dirlist})
ENDMACRO()

## Find JamomaCore paths and libraries, can become FindJamomaCore.cmake once paths are settled
# Hints (paths) allow to find Jamoma libraries that are not yet installed but located on the same root dir as this source
include(FindPackageHandleStandardArgs)
find_path(JAMOMA_FOUNDATION_INCLUDE_DIR TTBase.h HINTS ${CMAKE_SOURCE_DIR}/../../../Foundation/library/includes PATH_SUFFIXES jamoma)
find_library(JAMOMA_FOUNDATION_LIBRARY NAMES JamomaFoundation)
find_package_handle_standard_args(JAMOMA_FOUNDATION DEFAULT_MSG JAMOMA_FOUNDATION_LIBRARY JAMOMA_FOUNDATION_INCLUDE_DIR)
include_directories(${JAMOMA_FOUNDATION_INCLUDE_DIR})
find_path(JAMOMA_DSP_INCLUDE_DIR TTDSP.h HINTS ${CMAKE_SOURCE_DIR}/../../../DSP/library/includes PATH_SUFFIXES jamoma)
find_library(JAMOMA_DSP_LIBRARY NAMES JamomaDSP)
find_package_handle_standard_args(JAMOMA_DSP DEFAULT_MSG JAMOMA_DSP_LIBRARY JAMOMA_DSP_INCLUDE_DIR)
include_directories(${JAMOMA_DSP_INCLUDE_DIR})
find_path(JAMOMA_MODULAR_INCLUDE_DIR TTModular.h HINTS ${CMAKE_SOURCE_DIR}/../../../Modular/library/includes PATH_SUFFIXES jamoma)
find_path(JAMOMA_MODULAR_PROTOCOLLIB_INCLUDE_DIR Protocol.h HINTS ${CMAKE_SOURCE_DIR}/../../../Modular/library/ProtocolLib PATH_SUFFIXES jamoma)
find_path(JAMOMA_MODULAR_SCHEDULERLIB_INCLUDE_DIR Scheduler.h HINTS ${CMAKE_SOURCE_DIR}/../../../Modular/library/SchedulerLib PATH_SUFFIXES jamoma)
find_path(JAMOMA_MODULAR_PEEROBJECT_INCLUDE_DIR TTMapper.h HINTS ${CMAKE_SOURCE_DIR}/../../../Modular/library/PeerObject PATH_SUFFIXES jamoma)
find_library(JAMOMA_MODULAR_LIBRARY NAMES JamomaModular)
find_package_handle_standard_args(JAMOMA_MODULAR DEFAULT_MSG JAMOMA_MODULAR_LIBRARY JAMOMA_MODULAR_INCLUDE_DIR)
include_directories(${JAMOMA_MODULAR_INCLUDE_DIR})

# Find JamomaScore paths and libraries, can become FindJamomaScore.cmake once paths are settled
# Hints (paths) allow to find Jamoma libraries that are not yet installed but located on the same root dir as this source
find_path(JAMOMA_SCORE_INCLUDE_DIR TTScore.h HINTS ${CMAKE_SOURCE_DIR}/../../../Score/library/includes PATH_SUFFIXES jamoma)
find_library(JAMOMA_SCORE_LIBRARY NAMES JamomaScore)
find_package_handle_standard_args(JAMOMA_SCORE DEFAULT_MSG JAMOMA_SCORE_LIBRARY JAMOMA_SCORE_INCLUDE_DIR)
include_directories(${JAMOMA_SCORE_INCLUDE_DIR})

#LibXml2
FIND_PACKAGE ( LibXml2 REQUIRED )
INCLUDE_DIRECTORIES( ${LIBXML2_INCLUDE_DIR} )

# Find Gecode paths and libraries, can become FindGecode.cmake
#find_package(Gecode REQUIRED COMPONENTS kernel support int set driver flatzinc minimodel search)
#if(${GECODE_VERSION} MATCHES "4.?.?")# not the best test
	#    message(FATAL_ERROR "Gecode 4.x is not supported")
#endif()

#Qt4
find_package(Qt4 REQUIRED COMPONENTS QtCore QtGui QtXml QtSvg QtNetwork)
include(${QT_USE_FILE})

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/Shared/toolchains/modules/")

if(APPLE)
	if(NOT CMAKE_INSTALL_PREFIX)
		set(CMAKE_INSTALL_PREFIX "/usr/local/")
	endif()
	message("CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}")
	add_definitions(-DTT_PLATFORM_MAC)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
	# Independent shared libraries
	set(CMAKE_MACOSX_RPATH 1)

	# CPack
	SET(CPACK_GENERATOR "Bundle")
	SET(CPACK_BUNDLE_NAME "i-score")
	SET(CPACK_BUNDLE_ICON "${CMAKE_SOURCE_DIR}/resources/images/i-score.icns")
	#SET(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Shared/toolchains/jamoma-Info.plist")

elseif(UNIX)
	add_definitions(-DTT_PLATFORM_LINUX)
	SET(CPACK_GENERATOR "DEB")
	SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "The i-score team")
	SET(CPACK_SET_DESTDIR true)
	SET(CPACK_INSTALL_PREFIX "/usr/local/")

	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		include_directories("/usr/include/i386-linux-gnu/c++/4.8/")
	endif()
elseif(WIN32)
	add_definitions(-DTT_PLATFORM_WIN)
	SET(CPACK_GENERATOR "NSIS")
endif()


# i-score
include_directories("${CMAKE_SOURCE_DIR}/headers" "${CMAKE_SOURCE_DIR}/headers/data" "${CMAKE_SOURCE_DIR}/headers/GUI")
file(GLOB ISCORE_SRCS ${CMAKE_SOURCE_DIR}/src/data/*.cpp ${CMAKE_SOURCE_DIR}/src/GUI/*.cpp ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB ISCORE_HDRS ${CMAKE_SOURCE_DIR}/headers/data/*.hpp ${CMAKE_SOURCE_DIR}/headers/GUI/*.hpp)
file(GLOB ISCORE_UIS ${CMAKE_SOURCE_DIR}/src/data/*.ui ${CMAKE_SOURCE_DIR}/src/GUI/*.ui ${CMAKE_SOURCE_DIR}/src/*.ui)
file(GLOB ISCORE_QRCS ${CMAKE_SOURCE_DIR}/*.qrc)

# generate rules for building source files from the resources
QT4_ADD_RESOURCES(ISCORE_RCC_SRCS ${ISCORE_QRCS})

# Generates ui_*.h files
QT4_WRAP_UI(ISCORE_UIS_H ${ISCORE_UIS})

# Generates moc_*.cxx files
QT4_WRAP_CPP(ISCORE_MOC_SRCS ${ISCORE_HDRS} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories("${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_BINARY_DIR}/${CMAKE_SOURCE_DIR}/headers")

SET(ICON_NAME "i-score")
IF ( WIN32 OR MINGW)
        EXECUTE_PROCESS(COMMAND echo "IDI_ICON1	ICON	DISCARDABLE \"${CMAKE_SOURCE_DIR}/resources/images/${ICON_NAME}.ico\"" OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc)
        EXECUTE_PROCESS(COMMAND ${CMAKE_RC_COMPILER} -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc
        -o ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.o)
	IF(USE_DEBUG)
                SET(OS_SPECIFIC ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.o)
	ELSE()
                #SET(OS_SPECIFIC "WIN32" ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.o)
                SET(OS_SPECIFIC ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.o)
	ENDIF()
ELSE ()
	IF ( APPLE )
		SET(APP_TYPE MACOSX_BUNDLE)
                SET(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
		# set how it shows up in the Info.plist file
		SET(MACOSX_BUNDLE_ICON_FILE ${ICON_NAME}.icns)
		# set the bundle identifier (REQUIRED, or some strange GUI bugs may appear)
                SET(MACOSX_BUNDLE_GUI_IDENTIFIER "org.mediacycle.${PROJECT_NAME}")
                SET(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME}")
                SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME}")
                SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_NAME}")
                SET(MACOSX_BUNDLE_BUNDLE_VERSION "0.2")
                SET(MACOSX_BUNDLE_COPYRIGHT "Copyright i-score")
		# set where in the bundle to put the icns file
                SET_SOURCE_FILES_PROPERTIES(${CMAKE_SOURCE_DIR}/resources/images/${ICON_NAME}.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
		# include the icns file in the target
                SET(ISCORE_SRCS ${ISCORE_SRCS} ${CMAKE_SOURCE_DIR}/resources/images/${ICON_NAME}.icns)
		SET(OS_SPECIFIC ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST})
	ENDIF()
ENDIF()
add_executable(i-score ${OS_SPECIFIC} ${ISCORE_SRCS} ${ISCORE_HDRS} ${ISCORE_UIS_H} ${ISCORE_MOC_SRCS} ${ISCORE_RCC_SRCS})
TARGET_LINK_LIBRARIES(i-score ${LIBXML2_LIBRARIES} ${JAMOMA_SCORE_LIBRARY} ${JAMOMA_MODULAR_LIBRARY} ${JAMOMA_DSP_LIBRARY} ${JAMOMA_FOUNDATION_LIBRARY} ${QT_LIBRARIES})
INSTALL(TARGETS i-score DESTINATION ${CMAKE_INSTALL_PREFIX}/bin COMPONENT i-score)

### Creation of packages ###
SET(CPACK_PACKAGE_NAME "i-score")
SET(CPACK_PACKAGE_VENDOR "The i-score team")

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "i-score is an intermedia sequencer for the precise and flexible scripting of interactive scenarios.")
SET(CPACK_PACKAGE_VERSION "0.3a-dev")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "3")

SET(CPACK_SOURCE_GENERATOR TGZ)
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "i-score-0.3a-dev")
INCLUDE(CPack)


IF(BUILD_DOCUMENTATION)
	FIND_PACKAGE(UnixCommands REQUIRED)
	FIND_PACKAGE(Doxygen REQUIRED)
	FIND_PACKAGE(Gnuplot REQUIRED)
	FIND_PACKAGE(HTMLHelp REQUIRED)
	FIND_PACKAGE(Perl REQUIRED)
	FIND_PACKAGE(Wget REQUIRED)

	# Warning: doxygen doesn't confuse YES/NO and ON/OFF like CMake does	
	SET(GENERATE_LATEX NO)
	FIND_PACKAGE(LATEX)
	IF(PDFLATEX_COMPILER)
		SET(GENERATE_LATEX YES)
	ENDIF()

	IF(DOT)
		SET(HAVE_DOT YES)
	ELSE()
		SET(HAVE_DOT NO)
		MESSAGE(FATAL_ERROR "Graphviz dot missing.")
	ENDIF()

	# A graphical call graph is drawn for each function showing the functions that the function directly or indirectly calls.
	SET(CALL_GRAPH NO) 

	# A graphical caller graph is drawn for each function showing the functions that the function is directly or indirectly called by. 
	SET(CALLER_GRAPH NO)
	OPTION(DOCUMENTATION_GRAPH_FUNCTION_CALLS "Enable graphical call/callers graphs for each function that the functions calls/are called by." ON)
	IF(DOCUMENTATION_GRAPH_FUNCTION_CALLS)
		SET(CALL_GRAPH YES)
		SET(CALLER_GRAPH YES)
	ENDIF()

	OPTION(DOCUMENTATION_HTML_HELP "Build the HTML Help file (CHM)." OFF)
	OPTION(DOCUMENTATION_HTML_TARZ "Build a compressed tar archive of the HTML doc." OFF)

	OPTION(BUILD_REF_DOCS_SEARCHENGINE "Enable doxygen's search engine (requires that documentation to be installed on a php enabled web server)" ON)
	IF(BUILD_REF_DOCS_SEARCHENGINE)
		SET(SEARCHENGINE YES)
	ELSE()
		SET(SEARCHENGINE NO)
	ENDIF()

	OPTION(BUILD_REF_DOCS_TAGFILE "Generate a tag file named i-score.tag on the documentation web server" OFF)
	IF(BUILD_REF_DOCS_TAGFILE)
		SET(GENERATE_TAGFILE "${CMAKE_BINARY_DIR}/doc/i-scoreReferenceDocs/i-score.tag")
	ELSE()
		SET(GENERATE_TAGFILE "")
	ENDIF()

	# If html help generation was requested. DOCUMENTATION_HTML_HELP is defined by Documentation.cmake
	SET(GENERATE_HTMLHELP "NO")
	IF(DOCUMENTATION_HTML_HELP)
		# on windows Documentation.cmake finds the html help workshop fi it exists. On u*ix we might have it with wine but no way to point it out
		IF(NOT WIN32)
			SET(HTML_HELP_COMPILER "" CACHE FILEPATH "Enter location of the HTML help compiler to let doxygen compile html")
			MARK_AS_ADVANCED(HTML_HELP_COMPILER)
		ENDIF()
		# this var sets a proper value in .doxygen files when configuring them below
		SET(GENERATE_HTMLHELP "YES")
	endif()

	# This processes our resources/doxygen/Doxyfile and substitutes paths to generate
	# a final Doxyfile
	CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/resources/doxygen/Doxyfile
		${PROJECT_BINARY_DIR}/doc/i-score.doxyfile
	)
	# copy the logo to documentations target folder
        #CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/resources/images/i-score.png
		#   ${PROJECT_BINARY_DIR}/doc/i-scoreReferenceDocs/i-score.png COPYONLY
	#)
	#INSTALL(FILES ${PROJECT_BINARY_DIR}/doc/${PROJECT_NAME}ReferenceDocs-${i-score_VERSION}.chm DESTINATION doc OPTIONAL COMPONENT i-score-doc)
	INSTALL(DIRECTORY ${PROJECT_BINARY_DIR}/doc/i-scoreReferenceDocs DESTINATION doc COMPONENT i-score-doc)

	# This creates a new target to build documentation.
	# It runs ${DOXYGEN} which is the full path and executable to
	# Doxygen on your system.
	# It runs the final generated Doxyfile against it.
	# The DOT_PATH is substituted into the Doxyfile.
	ADD_CUSTOM_TARGET(documentation ${DOXYGEN}
		${PROJECT_BINARY_DIR}/doc/i-score.doxyfile
	)
ENDIF(BUILD_DOCUMENTATION)
