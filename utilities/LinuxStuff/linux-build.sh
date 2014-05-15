#!/bin/sh
# First install the required stuff (cf. INSTALL.txt)

# Clone Jamoma and Score
export GIT_SSL_NO_VERIFY=1
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
qtchooser -run-tool=qmake -qt=qt4 ../../i-score -spec unsupported/linux-clang
make
