#!/bin/bash

INSTALL_PATH="/home/luc/Documents/workspace/acousmoscribe/installation/"
INSTALL_U64_PATH="/home/luc/Documents/workspace/acousmoscribe/installation/acousmoscribe_ubuntu_64"
INSTALL_U32_PATH="/home/luc/Documents/workspace/acousmoscribe/installation/acousmoscribe_ubuntu_32"

LOCAL_BIN_PATH="/home/luc/Documents/workspace/acousmoscribe/acousmoscribe"
LOCAL_SHARE_DOCS_PATH=$INSTALL_PATH"ubuntuDocs/shareDocs/"
LOCAL_DEBIAN_PATH=$INSTALL_PATH"ubuntuDocs/DEBIAN/"

ROOT_USR_PATH="/usr/"
ROOT_LIB_PATH="/usr/bin/"
ROOT_LIB_PATH="/usr/local/lib/"
ROOT_INCLUDE_PATH="/usr/local/include/"
ROOT_SHARE_PATH="/usr/local/share/"

INSTALL_USR_PATH=$INSTALL_U64_PATH$ROOT_USR_PATH
INSTALL_LIB_PATH=$INSTALL_U64_PATH$ROOT_LIB_PATH
INSTALL_INCLUDE_PATH=$INSTALL_U64_PATH$ROOT_INCLUDE_PATH
INSTALL_BIN_PATH=$INSTALL_U64_PATH$ROOT_BIN_PATH
INSTALL_SHARE_PATH=$INSTALL_U64_PATH$ROOT_SHARE_PATH
INSTALL_DEBIAN_PATH=$INSTALL_U64_PATH"/DEBIAN/"

# UBUNTU 64

echo "CREATING INSTALLATION REPOSITORY"

sudo mkdir -p $INSTALL_USR_PATH
sudo mkdir -p $INSTALL_LIB_PATH
sudo mkdir -p $INSTALL_INCLUDE_PATH
sudo mkdir -p $INSTALL_BIN_PATH
sudo mkdir -p $INSTALL_SHARE_PATH
sudo mkdir -p $INSTALL_DEBIAN_PATH

echo "COPYING BINARY"

sudo cp -r $LOCAL_BIN_PATH $INSTALL_BIN_PATH

echo "COPYING INCLUDES"

for i in "DeviceManager/" "gecode/" "IScore/" "libxml2/" 
do
    sudo mkdir -p $INSTALL_INCLUDE_PATH$i
    sudo cp -r $ROOT_INCLUDE_PATH$i* $INSTALL_INCLUDE_PATH$i
done

echo "COPYING LIBRARIES"

sudo mkdir -p $INSTALL_LIB_PATH"/IScore"

for i in "IScore/" "libDeviceManager.a" "libIscore.a" "libgecode*.a" "libxml*.a" "libxml*.so*"
do
    if [ $i = "IScore/" ]
    then
	sudo cp -r $ROOT_LIB_PATH$i $INSTALL_LIB_PATH$i
    else
	sudo cp -r $ROOT_LIB_PATH$i $INSTALL_LIB_PATH
    fi
done

echo "COPYING SHARE DOCS"

sudo cp -r $LOCAL_SHARE_DOCS_PATH $INSTALL_SHARE_PATH

echo "COPYING DEBIAN FILES"

sudo cp -r $LOCAL_DEBIAN_PATH $INSTALL_DEBIAN_PATH

sudo chown root -R $INSTALL_USR_PATH
sudo chmod u+x $INSTALL_DEBIAN_PATH"post"*
sudo chmod u+x $INSTALL_DEBIAN_PATH"post"*