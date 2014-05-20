#!/bin/bash
########################################
###### Intro & Parameter handling ######
########################################
ISCORE_CMAKE_DEBUG=""
ISCORE_QMAKE_DEBUG=""
ISCORE_CMAKE_TOOLCHAIN=""
ISCORE_QMAKE_TOOLCHAIN=""
ISCORE_DEPTH_GIT="--depth=1"
ISCORE_FOLDER="iscore0.2"

HELP_MESSAGE="Usage : $(basename "$0") [software] [options]
Builds Jamoma, Score, and i-score 0.2, 0.3 on Linux and OS X systems.

Software :
iscore
  Builds (not installs, yet) i-score.

iscore-recast
  Builds i-score 0.3 instead of 0.2. Overrides iscore.

jamoma
  Installs Jamoma on the system folders.

Options :
--clone
  Clones the git repositories.
--fetch-all
  Fetches the full git repositories instead of the tip of the feature/cmake branch. Useful for development.

--install-deps
  Installs dependencies using apt-get / yum on Linux and brew / port on OS X.

--debug
  Builds everything with debug informations.
--use-clang
  Builds everything with the Clang compiler. Only useful on Linux systems.
--android
  Cross-build for Android. Only i-score 0.3. Requires the NDK & a toolchain with compiled libs. See AndroidBuild.txt.
  To cross-build, please set ANDROID_NDK_ROOT to your NDK path and ANDROID_QT_BIN to the corresponding qmake executable folder.

--clean
  Removes the build folder.

--help
  Shows this message
"

if test $# -eq 0 ; then
	echo "$HELP_MESSAGE"
	exit 1
fi

while test $# -gt 0
do
	case "$1" in
	--help) echo "$HELP_MESSAGE"
		exit
		;;
	--debug) echo "Debug build"
		ISCORE_CMAKE_DEBUG="-DCMAKE_BUILD_TYPE=Debug"
		ISCORE_QMAKE_DEBUG="CONFIG+=debug"
		;;
	--use-clang) echo "Build uses Clang (only for Linux)"
		ISCORE_USE_CLANG=1
		;;
	--android) echo "Android cross-build (only Jamoma for now, i-score needs to be ported to Qt5.x)"
		OSTYPE="android"
		;;
	--install-deps) echo "Required dependencies will be installed"
		ISCORE_INSTALL_DEPS=1
		;;
	--clone) echo "Will clone the git repositories"
		ISCORE_CLONE_GIT=1
		;;
	--fetch-all) echo "Will fetch the entire repositories. Useful for development".
		ISCORE_FETCH_GIT=1
		ISCORE_DEPTH_GIT=""
		;;
	iscore-recast) echo "Will build i-score v0.3 instead of v0.2"
		ISCORE_INSTALL_ISCORE=1
		ISCORE_RECAST=1
		ISCORE_FOLDER="iscore0.3"
		;;
	iscore) echo "Will build iscore"
		ISCORE_INSTALL_ISCORE=1
		;;
	jamoma) echo "Will install Jamoma in the system folders"
		ISCORE_INSTALL_JAMOMA=1
		;;
	--clean) "Removal of the build folder"
		rm -rf build
		;;
	*) echo "Wrong option : $1"
		echo "$HELP_MESSAGE"
		exit 1
		;;
	esac
	shift
done

###### Check of the Linux distribution ######
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	if [ -f /etc/fedora-release ] ; then # yum
		ISCORE_FEDORA=1
	elif [[ `lsb_release -si` = 'Debian' || `lsb_release -si` = 'Ubuntu' ]]; then # apt
		ISCORE_DEBIAN=1
	fi
fi

###### Set compiler toolchains ######
if [[ $ISCORE_USE_CLANG ]]; then
	if [[ "$OSTYPE" == "android"* ]]; then
	ISCORE_CMAKE_TOOLCHAIN="-DCMAKE_TOOLCHAIN_FILE=../../JamomaCore/Shared/CMake/toolchains/android-clang.cmake"
	ISCORE_QMAKE_TOOLCHAIN="-spec android-clang"
	elif [[ "$OSTYPE" != "darwin"* ]]; then
	ISCORE_CMAKE_TOOLCHAIN="-DCMAKE_TOOLCHAIN_FILE=../../JamomaCore/Shared/CMake/toolchains/linux-clang.cmake"
	ISCORE_QMAKE_TOOLCHAIN="-spec unsupported/linux-clang"
	fi
else
	if [[ "$OSTYPE" == "android"* ]]; then
	ISCORE_CMAKE_TOOLCHAIN="-DCMAKE_TOOLCHAIN_FILE=../../JamomaCore/Shared/CMake/toolchains/android-gcc.cmake"
	ISCORE_QMAKE_TOOLCHAIN="-spec android-g++"
	fi
fi

###### Install dependencies ######
if [[ $ISCORE_INSTALL_DEPS ]]; then
	if [[ "$OSTYPE" == "linux-gnu"* ]]; then # Desktop & Embedded Linux
		if [[ $ISCORE_RECAST ]]; then # Install dependencies required for iscore 0.3
			if [[ $ISCORE_FEDORA ]]; then
				su -c 'yum install qt5-qtbase qt5-qtbase-devel qt5-qttools qt5-qtsvg qt5-qtsvg-devel cmake git gecode-devel libxml2-devel libsndfile-devel portaudio-devel portmidi portmidi-tools portmidi-devel libstdc++-devel'
			elif [[ $ISCORE_DEBIAN ]]; then
				sudo apt-get install qtchooser qt5-default qt5-qmake qtbase5-dev qtbase5-dev-tools libqt5svg5-dev qtdeclarative5-dev libqt5svg5-dev cmake git libgl1-mesa-dev libgecode-dev libxml2-dev libsndfile-dev portaudio19-dev libportmidi-dev clang-3.4 libstdc++-4.8-dev
			fi
		else								 # Install dependencies required for iscore 0.2
			if [[ $ISCORE_FEDORA ]]; then
				su -c 'yum install cmake git qt qt-devel gecode-devel libxml2-devel libsndfile-devel portaudio-devel portmidi portmidi-tools portmidi-devel libstdc++-devel'
			elif [[ $ISCORE_DEBIAN ]]; then
				sudo apt-get install qtchooser cmake git libqt4-dev qt4-qmake libgl1-mesa-dev libgecode-dev libxml2-dev libsndfile-dev portaudio19-dev libportmidi-dev git clang-3.4 cmake libstdc++-4.8-dev
			fi
		fi
	elif [[ "$OSTYPE" == "darwin"* ]]; then # Mac OS X
		if command $brew > /dev/null; then # Brew
			if [[ $ISCORE_RECAST ]]; then
				brew install cmake gecode portaudio portmidi libsndfile qt5
			else
				brew install cmake gecode portaudio portmidi libsndfile qt
			fi
		elif command $port > /dev/null; then # MacPorts
			if [[ $ISCORE_RECAST ]]; then
				port install cmake gecode portaudio portmidi libsndfile qt5
			else
				port install cmake gecode portaudio portmidi libsndfile qt
			fi
		else
			echo "Warning : --install-deps was specified but no suitable package manager was found.
				  Please install Homebrew or Macports."
			exit
		fi
	fi
fi

###### Configure qmake ######
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	if [[ -z $ISCORE_QMAKE ]]; then
		if [[ $ISCORE_FEDORA ]]; then
			qmake4=qmake-qt4
			qmake5=qmake-qt5
		elif command $qtchooser > /dev/null; then
			qmake4="qtchooser -run-tool=qmake -qt=qt4"
			qmake5="qtchooser -run-tool=qmake -qt=qt5"
		else
			echo "Warning : could not differenciate between Qt4 and Qt5"
			qmake4=qmake
			qmake5=qmake
		fi
	fi
fi

########################################
#####         Installation         #####
########################################

##### Cloning #####
if [[ $ISCORE_CLONE_GIT ]]; then
	# Jamoma
	export GIT_SSL_NO_VERIFY=1

	git clone -b feature/cmake https://github.com/jamoma/JamomaCore.git $ISCORE_DEPTH_GIT
	git clone -b feature/cmake https://github.com/OSSIA/Score.git JamomaCore/Score $ISCORE_DEPTH_GIT

	# i-score
	if [[ $ISCORE_RECAST ]]; then
		git clone -b master https://github.com/OSSIA/i-score.git $ISCORE_FOLDER $ISCORE_DEPTH_GIT
	else
		git clone -b dev https://github.com/i-score/i-score.git $ISCORE_FOLDER $ISCORE_DEPTH_GIT
	fi

	if [[ $ISCORE_FETCH_GIT ]]; then
		cd JamomaCore
		git fetch
		cd Score
		git fetch
		cd ../../$ISCORE_FOLDER
		git fetch
		cd ..
	fi
fi

# Create build folders
mkdir -p build/jamoma
cd build/jamoma

##### Build Jamoma #####
if [[ $ISCORE_INSTALL_JAMOMA ]]; then
	cmake ../../JamomaCore $ISCORE_CMAKE_DEBUG $ISCORE_CMAKE_TOOLCHAIN

	# Creation of Jamoma packages
	if [[ "$OSTYPE" == "linux-gnu"* ]]; then # Desktop & Embedded Linux
		if [[ $ISCORE_FEDORA ]]; then # RPM
			cpack -G RPM

			# Install
			su -c 'rpm -Uvh --force JamomaCore-0.6-dev-Linux.rpm'
			su -c 'ln -s /usr/local/lib/jamoma/lib/* -t /usr/lib'

		elif [[ $ISCORE_DEBIAN ]]; then # DEB
			make package

			# Install
			sudo dpkg -i JamomaCore-0.6-dev-Linux.deb
			sudo ln -s /usr/local/lib/jamoma/lib/* -t /usr/lib

		else
			echo "Warning : no suitable packaging method found. Please package Jamoma yourself or run make install."
		fi
	elif [[ "$OSTYPE" == "android" ]]; then # Android
		make
		sudo cp *.so /opt/android-toolchain/arm-linux-androideabi/lib/jamoma
	elif [[ "$OSTYPE" == "darwin"* ]]; then # Mac OS X
		make install
	else
		echo "Not supported yet."
	fi
fi


##### Build i-score #####
if [[ $ISCORE_INSTALL_ISCORE ]]; then
	if [[ "$OSTYPE" == "linux-gnu"* ]]; then # Desktop & Embedded Linux
		# Build i-score
		cd ..
		mkdir $ISCORE_FOLDER
		export JAMOMA_INCLUDE_PATH=`pwd`/../JamomaCore
		cd $ISCORE_FOLDER

		if [[ $ISCORE_RECAST ]]; then
			${qmake5} ../../$ISCORE_FOLDER $ISCORE_QMAKE_TOOLCHAIN $ISCORE_QMAKE_DEBUG
			make
			cp i-scoreRecast ../../i-score
		else
			${qmake4} ../../$ISCORE_FOLDER $ISCORE_QMAKE_TOOLCHAIN $ISCORE_QMAKE_DEBUG
			make
			cp i-score ../../i-score
		fi


	elif [[ "$OSTYPE" == "android" ]]; then # Android
		cd ..
		mkdir $ISCORE_FOLDER
		export JAMOMA_INCLUDE_PATH=`pwd`/../JamomaCore
		cd $ISCORE_FOLDER
		mkdir android_build_output

		echo "Using following NDK root : $ANDROID_NDK_ROOT."
		$ANDROID_QT_BIN/qmake -r $ISCORE_QMAKE_TOOLCHAIN $ISCORE_QMAKE_DEBUG ../../$ISCORE_FOLDER
		make
		make install INSTALL_ROOT=android_build_output
		$ANDROID_QT_BIN/androiddeployqt --output android_build_output --input android-libi-scoreRecast.so-deployment-settings.json

		cp android_build_output/bin/QtApp-debug.apk ../../i-score-debug.apk

	elif [[ "$OSTYPE" == "darwin"* ]]; then # Mac OS X
		cd ..
		mkdir $ISCORE_FOLDER
		export JAMOMA_INCLUDE_PATH=`pwd`/../JamomaCore
		cd $ISCORE_FOLDER
		qmake ../../$ISCORE_FOLDER $ISCORE_QMAKE_TOOLCHAIN $ISCORE_QMAKE_DEBUG
		make

		cd ../..
		if [[ $ISCORE_RECAST ]]; then
			ISCORE_EXECUTABLE_NAME=i-scoreRecast
		else
			ISCORE_EXECUTABLE_NAME=i-score
		fi

		rm -rf $ISCORE_EXECUTABLE_NAME.app
		cp -rf build/$ISCORE_FOLDER/$ISCORE_EXECUTABLE_NAME.app .
		mkdir -p $ISCORE_EXECUTABLE_NAME.app/Contents/Frameworks/
		cp -rf /usr/local/jamoma* $ISCORE_EXECUTABLE_NAME.app/Contents/Frameworks/

		# Jamoma rpath
		install_name_tool -add_rpath @executable_path/../Frameworks/jamoma/lib $ISCORE_EXECUTABLE_NAME.app/Contents/MacOS/$ISCORE_EXECUTABLE_NAME
		install_name_tool -add_rpath @executable_path/../Frameworks/jamoma/extensions $ISCORE_EXECUTABLE_NAME.app/Contents/MacOS/$ISCORE_EXECUTABLE_NAME
	else
		echo "Not supported yet."
	fi
fi
