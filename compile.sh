#!/bin/sh

## USAGE : 
## ./compile.sh LINUX
##         OR
## ./compile.sh MACOS

CURRENT_PATH=`pwd`

OS=$1
DEVICE_MANAGER_PATH='/Users/luc/Documents/workspace/device-manager'
LIB_ISCORE_PATH='/Users/luc/Documents/workspace/libIscore'
ACOUSMOSCRIBE_PATH='/Users/luc/Documents/workspace/Acousmoscribe'
QMAKE='qmake'
MAKE='make'
MAKE_LINUX='make linux'
MAKE_MAC='make mac'
MAKE_CLEAN='make clean'

echo '-=[START]=-'

## Device-Manager Compilation

echo '-=[Device Manager : Library]=- Compiling ...'
cd $DEVICE_MANAGER_PATH
if [ "$OS" = "LINUX" ]
then
{
echo '-=[Device Manager : LINUX]=-'
echo '-=[Device Manager : Library]=- Cleaning ...'
$MAKE_CLEAN
echo '-=[Device Manager : Library]=- Compiling  ...'
$MAKE_LINUX
echo '-=[Device Manager : Library]=- Installing ...'
sudo cp libDeviceManager.a /usr/local/lib/
echo '-=[Device Manager : Library]=- Done.'
cd Plugins/Linux_MacOS/
echo '-=[Device Manager : Plugins]=- Cleaning ...'
$MAKE_CLEAN
echo '-=[Device Manager : Plugins]=- Compiling  ...'
chmod u+x compileForLinux.sh
./compileForLinux.sh
$MAKE_LINUX
echo '-=[Device Manager : Plugins]=- Installing ...'
sudo mkdir -p /usr/local/lib/IScore/
sudo cp Plugins/*.so /usr/local/lib/IScore/
}
elif [ "$OS" = "MACOS" ]
then
echo '-=[Device Manager : MACOS]=-'
echo '-=[Device Manager : Library]=- Cleaning ...'
$MAKE_CLEAN
echo '-=[Device Manager : Library]=- Compiling  ...'
$MAKE_MAC
echo '-=[Device Manager : Library]=- Installing ...'
sudo cp libDeviceManager.a /usr/local/lib/
echo '-=[Device Manager : Library]=- Done.'
cd Plugins/Linux_MacOS/
echo '-=[Device Manager : Plugins]=- Cleaning ...'
$MAKE_CLEAN
echo '-=[Device Manager : Plugins]=- Compiling  ...'
chmod u+x compileForMac.sh
./compileForMac.sh
echo '-=[Device Manager : Plugins]=- Compiling  ...'
$MAKE_MAC
echo '-=[Device Manager : Plugins]=- Installing ...'
sudo mkdir -p /usr/local/lib/IScore/
sudo cp Plugins/*.dylib /usr/local/lib/IScore/
fi

## LibIscore Compilation

cd $LIB_ISCORE_PATH
echo '-=[LibIScore]=- Cleaning ...'
$MAKE_CLEAN
if [ "$OS" = "LINUX" ]
then
echo '-=[LibIScore]=- Generating makefile ...'
$QMAKE -spec linux-g++ iscore.pro
elif [ "$OS" = "MACOS" ]
then
echo '-=[LibIScore]=- Generating makefile ...'
qmake -spec macx-g++ iscore.pro
else
echo '-=[LibIScore]=- ERROR : Unhandled OS'
fi
echo '-=[LibIScore]=- Compiling  ...'
$MAKE
echo '-=[LibIScore]=- Installing ...'
chmod u+x install.sh
./install.sh

## Acousmoscribe Compilation
cd $ACOUSMOSCRIBE_PATH
echo '-=[Acousmoscribe]=- Cleaning ...'
$MAKE_CLEAN
if [ "$OS" = "LINUX" ]
then
echo '-=[Acousmocribe]=- Generating makefile ...'
$QMAKE -spec linux-g++ acousmoscribe.pro
elif [ "$OS" = "MACOS" ]
then
echo '-=[Acousmoscribe]=- Generating makefile ...'
$QMAKE -spec macx-g++ acousmoscribe.pro
else
echo '-=[Acousmoscribe]=- ERROR : Unhandled OS'
fi
echo '-=[Acousmoscribe]=- Compiling  ...'
$MAKE

cd $CURRENT_PATH
echo '-=[END]=-'
