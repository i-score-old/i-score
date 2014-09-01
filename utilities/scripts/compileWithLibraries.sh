#!/bin/bash

## USAGE : 
## ./compile.sh LINUX
##         OR
## ./compile.sh LINUX64
##         OR
## ./compile.sh MACOS
##         OR
## ./compile.sh MACOS64

CURRENT_PATH=`pwd`

OS=$1

if [ "$OS" = "LINUX" ] || [ "$OS" = "LINUX64" ]
then
    {
    SCORE_PATH='/Users/WALL-E/Documents/Jamoma/Core/Score'
    I-SCORE_PATH='/Users/WALL-E/Documents/Iscore/i-score'
    }
elif [ "$OS" = "MACOS" ] || [ "$OS" = "MACOS64" ]
then
    {
    SCORE_PATH='/Users/jaimechao/Workspace/Score'
    I-SCORE_PATH='/Users/jaimechao/Workspace/i-score'
    }
fi

QMAKE_LINUX='qmake -spec linux-g++'
QMAKE_LINUX_64='qmake -spec linux-g++-64'
QMAKE_MACOS='qmake -spec unsupported/macx-clang'
# QMAKE_MACOS='qmake -spec unsupported/macx-clang'
MAKE='make'
MAKE_LINUX='make linux'
MAKE_LINUX64='make linux64'
MAKE_MAC='make mac'
MAKE_MAC64='make mac64'
MAKE_CLEAN='make clean'

echo '-=[START]=-'

## Score Compilation
#cd $SCORE_PATH
#echo '-=[Score]=- Building ...'
#buildWithSupport.rb dev clean

## i-score Compilation
cd $ISCORE_PATH
echo '-=[i-score]=- Cleaning ...'
$MAKE_CLEAN
if [ "$OS" = "LINUX" ]
then
    {
	echo '-=[i-score]=- Generating makefile ...'
	$QMAKE_LINUX i-score.pro
    }
elif [ "$OS" = "LINUX64" ]
then
    {
	echo '-=[i-score]=- Generating makefile ...'
	$QMAKE_LINUX_64 i-score.pro
    }
elif [ "$OS" = "MACOS" ] || [ "$OS" = "MACOS64" ]
then
    {
	echo '-=[i-score]=- Generating makefile ...'
	$QMAKE_MACOS i-score.pro
    }
else
    {
	echo '-=[i-score]=- ERROR : Unhandled OS'
    }
fi
echo '-=[i-score]=- Compiling  ...'
$MAKE

cd $CURRENT_PATH
echo '-=[END]=-'
