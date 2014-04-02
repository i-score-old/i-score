#!/bin/sh
# Install the required stuff
sudo apt-get install libqt4-dev qt4-qmake libgl1-mesa-dev libgecode-dev libxml2-dev libsndfile-dev portaudio19-dev libportmidi-dev ruby git clang-3.4 cmake libstdc++-4.8-dev

# Clone Jamoma and Score
git clone -b feature/cmake git://github.com/jamoma/JamomaCore.git --depth=1
git clone -b dev https://github.com/i-score/i-score.git --depth=1
git clone -b feature/cmake git://github.com/OSSIA/Score.git JamomaCore/Score --depth=1

# Build
mkdir -p build/jamoma
cd build/jamoma
cmake ../../JamomaCore
cmake ../../JamomaCore
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
qmake ../../i-score -spec unsupported/linux-clang
make
