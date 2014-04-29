#!/bin/bash
# Parameter handling
ISCORE_CMAKE_DEBUG=""
ISCORE_QMAKE_DEBUG=""
ISCORE_CMAKE_TOOLCHAIN=""
ISCORE_QMAKE_TOOLCHAIN=""
ISCORE_INSTALL_DEPS=0
ISCORE_USE_CLANG=0
ISCORE_FETCH_GIT=0
ISCORE_DEPTH_GIT="--depth=1"
ISCORE_RECAST=0
ISCORE_FOLDER="iscore0.2"
ISCORE_ONLY_JAMOMA=0
ISCORE_INSTALL_JAMOMA=0

HELP_MESSAGE="Usage : $(basename "$0") [options]
Builds Jamoma, Score, and i-score 0.2, 0.3 on Linux and OS X systems.

Options :
	--help			Shows this message
	--debug			Builds everything with debug informations.
	--use-clang		Builds everything with the Clang compiler. Only useful on Linux systems.
	--install-deps		Installs dependencies using apt-get on Linux and Brew on OS X.
	--fetch-all		Fetches the full git repositories instead of the tip of the feature/cmake branch.
	--iscore-recast 	Builds i-score 0.3 instead of 0.2.
	--only-jamoma		Does not build i-score, only Jamoma & Score.
	--install-jamoma	Installs Jamoma on the system folders.
	--android		Cross-build for Android. Only i-score 0.3. Requires the NDK & a toolchain with compiled libs. See AndroidBuild.txt.
					To cross-build, please set ANDROID_NDK_ROOT to your NDK path and ANDROID_QT_BIN to the corresponding qmake executable folder.
	--clean			Removes the build folder.
"
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
		--fetch-all) echo "Will fetch the entire repositories. Useful for development".
			ISCORE_FETCH_GIT=1
			ISCORE_DEPTH_GIT=""
			;;
		--iscore-recast) echo "Will build i-score v0.3 instead of v0.2"
			ISCORE_RECAST=1
			ISCORE_FOLDER="iscore0.3"
			;;
		--only-jamoma) echo "Will only build JamomaCore and Score"
			ISCORE_ONLY_JAMOMA=1
			;;
		--install-jamoma) echo "Will install Jamoma in the system folders"
			ISCORE_INSTALL_JAMOMA=1
			;;
		--clean) "Removal of the build folder"
			rm -rf build
			;;
		--*) echo "Wrong option : $1"
			 echo "$HELP_MESSAGE"
			exit 1
			;;
		*) echo "Argument $1"
			;;
	esac
	shift
done


if [[ "$ISCORE_USE_CLANG" == 1 ]]; then
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

# Install dependencies
if [[ "$ISCORE_INSTALL_DEPS" == 1 ]]; then
	if [[ "$OSTYPE" == "linux-gnu"* ]]; then # Desktop & Embedded Linux
		if [[ "$ISCORE_RECAST" == 1 ]]; then
			sudo apt-get install qtchooser qt5-qmake qtbase5-dev qtbase5-dev-tools libqt5svg5-dev qtdeclarative5-dev libqt5svg5-dev cmake git libgl1-mesa-dev libgecode-dev libxml2-dev libsndfile-dev portaudio19-dev libportmidi-dev git clang-3.4 cmake libstdc++-4.8-dev
		else
			sudo apt-get install qtchooser cmake git libqt4-dev qt4-qmake libgl1-mesa-dev libgecode-dev libxml2-dev libsndfile-dev portaudio19-dev libportmidi-dev git clang-3.4 cmake libstdc++-4.8-dev
		fi
	elif [[ "$OSTYPE" == "darwin"* ]]; then # Mac OS X with brew
		if [[ "$ISCORE_RECAST" == 1 ]]; then
			brew install cmake gecode portaudio portmidi libsndfile qt5
		else
			brew install cmake gecode portaudio portmidi libsndfile qt
		fi
		# TODO Macports
	fi
fi

# Clone Jamoma and Score
export GIT_SSL_NO_VERIFY=1

git clone -b feature/cmake https://github.com/jamoma/JamomaCore.git $ISCORE_DEPTH_GIT
git clone -b feature/cmake https://github.com/OSSIA/Score.git JamomaCore/Score $ISCORE_DEPTH_GIT

# Clone i-score
if [[ "$ISCORE_RECAST" == 1 ]]; then
	git clone -b master https://github.com/OSSIA/i-score.git $ISCORE_FOLDER $ISCORE_DEPTH_GIT
else
	git clone -b feature/cmake https://github.com/i-score/i-score.git $ISCORE_FOLDER $ISCORE_DEPTH_GIT
fi

if [[ "$ISCORE_FETCH_GIT" == 1 ]]; then
	cd JamomaCore
	git fetch
	cd Score
	git fetch
	cd ../../$ISCORE_FOLDER
	git fetch
	cd ..
fi

# Build
mkdir -p build/jamoma
cd build/jamoma
cmake ../../JamomaCore $ISCORE_CMAKE_DEBUG $ISCORE_CMAKE_TOOLCHAIN

if [[ "$OSTYPE" == "linux-gnu"* ]]; then # Desktop & Embedded Linux
	if [[ "$ISCORE_INSTALL_JAMOMA" == 1 ]]; then
		# Create a Debian package
		make package

		# Install
		sudo dpkg -i JamomaCore-0.6-dev-Linux.deb

		# Create Jamoma symlinks
		# TODO put them in the .deb
		sudo ln -s /usr/local/lib/jamoma/lib/* -t /usr/lib
	else
		make
	fi

	if [[ "$ISCORE_ONLY_JAMOMA" == 0 ]]; then
		# Build i-score
		cd ..
		mkdir $ISCORE_FOLDER
		export JAMOMA_INCLUDE_PATH=`pwd`/../JamomaCore
		cd $ISCORE_FOLDER

		if [[ "$ISCORE_RECAST" == 1 ]]; then
			qtchooser -run-tool=qmake -qt=qt5 ../../$ISCORE_FOLDER $ISCORE_QMAKE_TOOLCHAIN $ISCORE_QMAKE_DEBUG
			make
			cp i-scoreRecast ../../i-score
		else
			qtchooser -run-tool=qmake -qt=qt4 ../../$ISCORE_FOLDER $ISCORE_QMAKE_TOOLCHAIN $ISCORE_QMAKE_DEBUG
			make
			cp i-score ../../i-score
		fi


	fi

elif [[ "$OSTYPE" == "android" ]]; then # Android
	make

	if [[ "$ISCORE_INSTALL_JAMOMA" == 1 ]]; then
		sudo cp *.so /opt/android-toolchain/arm-linux-androideabi/lib/jamoma
	fi

	if [[ "$ISCORE_ONLY_JAMOMA" == 0 ]]; then
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
	fi

elif [[ "$OSTYPE" == "darwin"* ]]; then # Mac OS X
	if [[ "$ISCORE_INSTALL_JAMOMA" == 1 ]]; then
		make install
	fi

	if [[ "$ISCORE_ONLY_JAMOMA" == 0 ]]; then
		cd ..
		mkdir $ISCORE_FOLDER
		export JAMOMA_INCLUDE_PATH=`pwd`/../JamomaCore
		cd $ISCORE_FOLDER
		qmake ../../$ISCORE_FOLDER $ISCORE_QMAKE_TOOLCHAIN $ISCORE_QMAKE_DEBUG
		make

		cd ../..
		if [[ "$ISCORE_RECAST" == 1 ]]; then
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
	fi
else
	echo "Not supported yet."
fi
