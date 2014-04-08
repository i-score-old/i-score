#!/bin/sh
git clone -b feature/cmake http://github.com/Jamoma/JamomaCore.git --depth=1
git clone -b feature/cmake http://github.com/OSSIA/Score.git JamomaCore/Score --depth=1
git clone -b feature/cmake http://github.com/i-score/i-score.git --depth=1

mkdir -p build/jamoma
cd build/jamoma
cmake ../../JamomaCore
cmake ../../JamomaCore
make install

cd ..
mkdir i-score
export JAMOMA_INCLUDE_PATH=`pwd`/../JamomaCore
cd i-score
qmake ../../i-score
make

cd ../..
cp -rf build/i-score/i-score.app .
mkdir -p i-score.app/Contents/Frameworks/
cp -rf /usr/local/jamoma* i-score.app/Contents/Frameworks/

# Jamoma rpath
install_name_tool -add_rpath @executable_path/../Frameworks/jamoma/lib i-score.app/Contents/MacOS/i-score
install_name_tool -add_rpath @executable_path/../Frameworks/jamoma/extensions i-score.app/Contents/MacOS/i-score
