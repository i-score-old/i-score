#!/bin/bash -
# Parameter handling
ISCORE_CMAKE_DEBUG=""
ISCORE_QMAKE_DEBUG=""
ISCORE_CMAKE_TOOLCHAIN=""
ISCORE_QMAKE_TOOLCHAIN=""



while test $# -gt 0
do
	case "$1" in
		--debug) echo "Debug build"
			ISCORE_CMAKE_DEBUG="-DCMAKE_BUILD_TYPE=Debug"
			ISCORE_QMAKE_DEBUG="CONFIG+=debug"
			;;
		--use-clang) echo "Build uses Clang (only for Linux)"
			if [[ "$OSTYPE" != "darwin"* ]]; then
				ISCORE_CMAKE_TOOLCHAIN="-DCMAKE_TOOLCHAIN_FILE=../../JamomaCore/Shared/toolchains/linux-clang.cmake"
				ISCORE_QMAKE_TOOLCHAIN="unsupported/linux-clang"
			fi
			;;
		--android) echo "Android cross-build (only Jamoma for now, i-score needs to be ported in Qt5.2)"
			OSTYPE="android"
			ISCORE_CMAKE_TOOLCHAIN="-DCMAKE_TOOLCHAIN_FILE=../../JamomaCore/Shared/toolchains/android.cmake"
			ISCORE_QMAKE_TOOLCHAIN="android-clang"
			;;
		--install-deps echo "Required dependencies will be installed"
			INSTALL_DEPS=1
			;;
		--*) echo "Wrong option : $1"
			exit 1
			;;
		*) echo "Argument $1"
			;;
	esac
	shift
done

# Clone Jamoma and Score
export GIT_SSL_NO_VERIFY=1
git clone -b feature/cmake https://github.com/jamoma/JamomaCore.git --depth=1
git clone -b feature/cmake https://github.com/i-score/i-score.git --depth=1
git clone -b feature/cmake https://github.com/OSSIA/Score.git JamomaCore/Score --depth=1

# Build
mkdir -p build/jamoma
cd build/jamoma
cmake ../../JamomaCore $ISCORE_CMAKE_DEBUG $ISCORE_CMAKE_TOOLCHAIN
cmake ../../JamomaCore $ISCORE_CMAKE_DEBUG $ISCORE_CMAKE_TOOLCHAIN

if [[ "$OSTYPE" == "linux-gnu" ]]; then # Desktop & Embedded Linux

	if [[ -z "$INSTALL_DEPS" ]]; then
		sudo apt-get install libqt4-dev qt4-qmake libgl1-mesa-dev libgecode-dev libxml2-dev libsndfile-dev portaudio19-dev libportmidi-dev git clang-3.4 cmake libstdc++-4.8-dev
	fi

	make package

	# Install
	sudo dpkg -i Jamoma-0.6-dev-Linux.deb

	# Create Jamoma symlinks (to put in the .deb)
	sudo ln -s /usr/local/lib/jamoma/lib/* -t /usr/lib

	# Build i-score
	cd ..
	mkdir i-score
	export JAMOMA_INCLUDE_PATH=`pwd`/../JamomaCore
	cd i-score
	qtchooser -run-tool=qmake -qt=qt4 ../../i-score $ISCORE_QMAKE_TOOLCHAIN $ISCORE_QMAKE_DEBUG
	make
	cp i-score ../../iscore

elif [[ "$OSTYPE" == "android" ]]; then # Android
	make

	# To put in CMake
	for file in *.ttso; do
		mv "$file" "`basename $file .ttso`.so"
	done
	sudo cp *.so /opt/android-toolchain/arm-linux-androideabi/lib/jamoma
	ANDROID_LIBS=/opt/android-toolchain/android-toolchain/arm-linux-androideabi/lib/jamoma/*.so

	cd ..
	mkdir i-score
	export JAMOMA_INCLUDE_PATH=`pwd`/../JamomaCore
	cd i-score

	# qmake-android ../../i-score $ISCORE_QMAKE_TOOLCHAIN $ISCORE_QMAKE_DEBUG "ANDROID_EXTRA_LIBS=$ANDROID_LIBS"
	# make

elif [[ "$OSTYPE" == "darwin"* ]]; then # Mac OS X

	if [[ -z "$INSTALL_DEPS" ]]; then
		brew install cmake gecode portaudio portmidi libsndfile qt
	fi

	make install

	cd ..
	mkdir i-score
	export JAMOMA_INCLUDE_PATH=`pwd`/../JamomaCore
	cd i-score
	qmake ../../i-score $ISCORE_QMAKE_TOOLCHAIN $ISCORE_QMAKE_DEBUG
	make

	cd ../..
	cp -rf build/i-score/i-score.app .
	mkdir -p i-score.app/Contents/Frameworks/
	cp -rf /usr/local/jamoma* i-score.app/Contents/Frameworks/

	# Jamoma rpath
	install_name_tool -add_rpath @executable_path/../Frameworks/jamoma/lib i-score.app/Contents/MacOS/i-score
	install_name_tool -add_rpath @executable_path/../Frameworks/jamoma/extensions i-score.app/Contents/MacOS/i-score
else
	echo "Not supported yet."
fi
