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
  Builds and installs Jamoma on the system folders.

Options :
--clone
  Clones the git repositories.
--fetch-all
  Fetches the full git repositories instead of the tip of the feature/cmake branch. Useful for development.

--install-deps
  Installs dependencies using apt-get / yum on Linux and brew / port on OS X.

--jamoma-path=/some/path/to/Jamoma/Core folder
  Uses an existing Jamoma installation. Note : it has to be on a branch with CMake (currently feature/cmake).
--classic (transitional)
  Uses the ruby.rb script to build Jamoma.
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
	--classic) echo "Will build using the Ruby script"
		ISCORE_CLASSIC_BUILD=1
		;;
	--jamoma-path=*)
		ISCORE_JAMOMA_PATH=$(cd "${1#*=}"; pwd)
		echo "Will use the Jamoma installation located in ${ISCORE_JAMOMA_PATH}"
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

########## CLASSIC BUILD ###########
if [[ $ISCORE_CLASSIC_BUILD ]]; then
	if [[ $ISCORE_INSTALL_DEPS ]]; then
		## Qt ##
		echo "Installing Qt..."
		curl -O http://download.qt-project.org/official_releases/qt/4.8/4.8.6/qt-opensource-mac-4.8.6.dmg
		hdiutil mount qt-opensource-mac-4.8.6.dmg
		sudo installer -pkg "/Volumes/Qt 4.8.6/Qt.mpkg" -target /

		## Gecode ##
		echo "Installing Gecode..."
		curl -O http://www.gecode.org/download/Gecode-3.7.3.dmg
		hdiutil mount Gecode-3.7.3.dmg
		sudo installer -pkg "/Volumes/Gecode/Install Gecode 3.7.3.pkg" -target /
		hdiutil unmount /Volumes/Gecode
	
		## libXml
		echo "Installing libXml..."
		curl -O http://www.explain.com.au/download/combo-2007-10-07.dmg.gz
		gunzip combo-2007-10-07.dmg.gz
		hdiutil mount combo-2007-10-07.dmg
		sudo cp /Volumes/gnome-combo-2007-10-07-rw/libxml.framework /Library/Frameworks
		hdiutil unmount /Volumes/gnome-combo-2007-10-07-rw
	fi

	if [[ $ISCORE_JAMOMA_PATH ]]; then
		export ISCORE_SCORE_PATH=$ISCORE_JAMOMA_PATH/Score
	else
		export ISCORE_SCORE_PATH=`pwd`/Score
	fi

	if [[ $ISCORE_CLONE_GIT ]]; then
		if [[ $ISCORE_JAMOMA_PATH ]]; then
			if [[ -e $ISCORE_JAMOMA_PATH/Score ]]; then
				echo "Will build using the existing Jamoma & Score installations"
			else
				echo "Will clone Score in the Jamoma/Core folder"
				git clone https://github.com/OSSIA/Score $ISCORE_JAMOMA_PATH/Score $ISCORE_DEPTH_GIT
			fi
		else
			git clone https://github.com/OSSIA/Score $ISCORE_DEPTH_GIT
		fi
		(cd $ISCORE_SCORE_PATH; git checkout dev)

		git clone https://github.com/i-score/i-score iscore0.2
		(cd iscore0.2; git checkout dev)
	fi 

	if [[ $ISCORE_INSTALL_JAMOMA ]]; then
		sudo mkdir -p /usr/local/jamoma -m 777
		sudo chmod 777 /usr/local/lib
		ruby $ISCORE_SCORE_PATH/build.rb dev clean
	fi

	if [[ $ISCORE_INSTALL_ISCORE ]]; then
		cd iscore0.2
		./build.sh
	fi
	exit 0
fi
#####################################

###### Check of the Linux distribution ######
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	if [ -f /etc/fedora-release ] ; then # yum
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
			brew link gecode
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

	if [[ $ISCORE_JAMOMA_PATH ]]; then
		if [[ -e $ISCORE_JAMOMA_PATH/CMakeLists.txt ]]; then
			if [[ -e $ISCORE_JAMOMA_PATH/Score ]]; then
				if [[ -e $ISCORE_JAMOMA_PATH/Score/CMakeLists.txt ]]; then
					echo "Building using the existing installation"
				else
					echo "Please switch OSSIA/Score to the feature/cmake branch"
					exit 1
				fi
			else
				git clone -b feature/cmake https://github.com/OSSIA/Score.git $ISCORE_JAMOMA_PATH/Score $ISCORE_DEPTH_GIT
			fi
		else
			echo "Please switch Jamoma/JamomaCore to the feature/cmake branch"
			exit 1
		fi
	else
		git clone -b feature/cmake https://github.com/jamoma/JamomaCore.git $ISCORE_DEPTH_GIT
		git clone -b feature/cmake https://github.com/OSSIA/Score.git JamomaCore/Score $ISCORE_DEPTH_GIT
		export ISCORE_JAMOMA_PATH=`pwd`/JamomaCore
	fi
	export JAMOMA_INCLUDE_PATH=$ISCORE_JAMOMA_PATH

	# i-score
	if [[ $ISCORE_RECAST ]]; then
		git clone -b master https://github.com/OSSIA/i-score.git $ISCORE_FOLDER $ISCORE_DEPTH_GIT
	elif [[ $ISCORE_INSTALL_ISCORE ]]; then
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
	cmake $ISCORE_JAMOMA_PATH $ISCORE_CMAKE_DEBUG $ISCORE_CMAKE_TOOLCHAIN

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


##### Build i-score #####
if [[ $ISCORE_INSTALL_ISCORE ]]; then
	if [[ "$OSTYPE" == "linux-gnu"* ]]; then # Desktop & Embedded Linux
		# Build i-score
		cd ..
		mkdir $ISCORE_FOLDER

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
		echo "System not supported yet."
	fi
fi
