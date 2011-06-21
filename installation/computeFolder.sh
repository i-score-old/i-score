#!/bin/bash

## USAGE : 
## ./computeFolder.sh UBUNTU32
##         OR
## ./computeFolder.sh UBUNTU64
##         OR
## ./computeFolder.sh MACOS32
##         OR
## ./computeFolder.sh MACOS64

OS=$1

echo "SETTING UP PATH VARIABLES"

ROOT_USR_PATH="/usr/"
ROOT_BIN_PATH="/usr/bin/"
ROOT_LIB_PATH="/usr/local/lib/"
ROOT_INCLUDE_PATH="/usr/local/include/"
ROOT_SHARE_PATH="/usr/local/share/"
ROOT_FRAMEWORKS_PATH="/Library/Frameworks/"

if [ "$OS" = "UBUNTU32" ] || [ "$OS" = "UBUNTU64" ]
then
    ACOUSMOSCRIBE_PATH="/home/luc/Documents/workspace/acousmoscribe/"
    INSTALL_PATH=$ACOUSMOSCRIBE_PATH"installation/"
    if [ "$OS" = "UBUNTU32" ]
    then
	INSTALL_UBUNTU_PATH=$INSTALL_PATH"acousmoscribe_ubuntu_32"
    elif [ "$OS" = "UBUNTU64" ]
    then
	INSTALL_UBUNTU_PATH=$INSTALL_PATH"acousmoscribe_ubuntu_64"
    fi
    sudo rm -rf $INSTALL_UBUNTU_PATH
    sudo mkdir -p $INSTALL_UBUNTU_PATH
    LOCAL_SHARE_DOCS_PATH=$INSTALL_PATH"ubuntuDocs/shareDocs/"
    LOCAL_DEBIAN_PATH=$INSTALL_PATH"ubuntuDocs/DEBIAN/"
    LOCAL_BIN_PATH=$ACOUSMOSCRIBE_PATH"acousmoscribe"
    INSTALL_USR_PATH=$INSTALL_UBUNTU_PATH$ROOT_USR_PATH
    INSTALL_LIB_PATH=$INSTALL_UBUNTU_PATH$ROOT_LIB_PATH
    INSTALL_INCLUDE_PATH=$INSTALL_UBUNTU_PATH$ROOT_INCLUDE_PATH
    INSTALL_BIN_PATH=$INSTALL_UBUNTU_PATH$ROOT_BIN_PATH
    INSTALL_SHARE_PATH=$INSTALL_UBUNTU_PATH$ROOT_SHARE_PATH
    INSTALL_DEBIAN_PATH=$INSTALL_UBUNTU_PATH"/DEBIAN/"
elif [ "$OS" = "MACOS32" ] || [ "$OS" = "MACOS64" ]
then
    ACOUSMOSCRIBE_PATH="/Users/luc/Documents/workspace/acousmoscribe/"
    INSTALL_PATH=$ACOUSMOSCRIBE_PATH"installation/"
    if [ "$OS" = "MACOS32" ]
    then
	INSTALL_MACOS_PATH=$INSTALL_PATH"acousmoscribe_MacOS_10_4"
    elif [ "$OS" = "MACOS64" ]
    then
	INSTALL_MACOS_PATH=$INSTALL_PATH"acousmoscribe_MacOS_10_6"
    fi
    sudo rm -rf $INSTALL_MACOS_PATH
    sudo mkdir -p $INSTALL_MACOS_PATH
    LOCAL_BIN_PATH=$ACOUSMOSCRIBE_PATH"acousmoscribe.app/"
    ROOT_FRAMEWORKS_PATH="/Library/Frameworks/"
    INSTALL_USR_PATH=$INSTALL_MACOS_PATH$ROOT_USR_PATH
    INSTALL_LIB_PATH=$INSTALL_MACOS_PATH$ROOT_LIB_PATH
    INSTALL_INCLUDE_PATH=$INSTALL_MACOS_PATH$ROOT_INCLUDE_PATH
    INSTALL_BIN_PATH=$INSTALL_MACOS_PATH"/Applications/Acousmoscribe.app/"
    INSTALL_FRAMEWORKS_PATH=$INSTALL_MACOS_PATH$ROOT_FRAMEWORKS_PATH
else
    echo "Unhandled OS"
fi

echo "BUILDING INSTALLATION REPOSITORY"

sudo mkdir -p $INSTALL_USR_PATH
sudo mkdir -p $INSTALL_LIB_PATH
sudo mkdir -p $INSTALL_INCLUDE_PATH
sudo mkdir -p $INSTALL_BIN_PATH

if [ "$OS" = "UBUNTU32" ] || [ "$OS" = "UBUNTU64" ]
then 
{
    sudo mkdir -p $INSTALL_SHARE_PATH
    sudo mkdir -p $INSTALL_DEBIAN_PATH
}
elif [ "$OS" = "MACOS32" ] || [ "$OS" = "MACOS64" ]
then
{
    echo ""
}
else
{
    echo "Unhandled OS"
}
fi

echo "COPYING BINARY ..."

if [ "$OS" = "UBUNTU32" ] || [ "$OS" = "UBUNTU64" ]
then 
{
    sudo rm -f $INSTALL_BIN_PATH
    sudo cp $LOCAL_BIN_PATH $INSTALL_BIN_PATH
}
elif [ "$OS" = "MACOS32" ] || [ "$OS" = "MACOS64" ]
then
{
    sudo rm -rf $INSTALL_BIN_PATH*
    sudo cp -r $LOCAL_BIN_PATH* $INSTALL_BIN_PATH
}
else
{
    echo "Unhandled OS"
}
fi

echo "COPYING INCLUDES..."

if [ "$OS" = "UBUNTU32" ] || [ "$OS" = "UBUNTU64" ]
then
    for i in "DeviceManager/" "gecode/" "IScore/" "libxml2/" 
    do
	echo "... "$i
	sudo rm -rf $INSTALL_INCLUDE_PATH$i
	sudo cp -r $ROOT_INCLUDE_PATH$i $INSTALL_INCLUDE_PATH
    done
elif [ "$OS" = "MACOS32" ] || [ "$OS" = "MACOS64" ]
then
    for i in "DeviceManager/" "IScore/" "libxml2/" 
    do
	echo "... "$i
	sudo rm -rf $INSTALL_INCLUDE_PATH$i
	sudo mkdir $INSTALL_INCLUDE_PATH$i
	sudo cp -r $ROOT_INCLUDE_PATH$i $INSTALL_INCLUDE_PATH$i
    done
else
    echo "Unhandled OS"
fi

echo "COPYING LIBRARIES ..."

if [ "$OS" = "UBUNTU32" ] || [ "$OS" = "UBUNTU64" ]
then
    sudo rm -rf $INSTALL_LIB_PATH"/IScore"
    
    for i in "IScore/" "libDeviceManager.a" "libIscore.a" "libgecode*.a" "libxml*.a" "libxml*.so*"
    do
	echo "... "$i
	sudo cp -r $ROOT_LIB_PATH$i $INSTALL_LIB_PATH
    done
elif [ "$OS" = "MACOS32" ] || [ "$OS" = "MACOS64" ]
then
    sudo rm -rf $INSTALL_LIB_PATH"/IScore"
    
    for i in "IScore/" "libDeviceManager.a" "libIscore.a" "libxml2.a" "libgecode*" "libxml*"
    do
	echo "... "$i
	if [ $i = "IScore/" ]
	then
	    sudo mkdir $INSTALL_LIB_PATH$i
	    sudo cp -r $ROOT_LIB_PATH$i $INSTALL_LIB_PATH$i
	else
	    sudo cp $ROOT_LIB_PATH$i $INSTALL_LIB_PATH
	fi
    done

    echo "COPYING FRAMEWORK ..."
    sudo rm -rf $INSTALL_FRAMEWORKS_PATH
    sudo mkdir -p $INSTALL_FRAMEWORKS_PATH
    sudo mkdir $INSTALL_FRAMEWORKS_PATH"gecode.framework"
    sudo cp -r $ROOT_FRAMEWORKS_PATH"gecode.framework/"* $INSTALL_FRAMEWORKS_PATH"gecode.framework"
    
else
    echo "Unhandled OS"
fi

if [ "$OS" = "UBUNTU32" ] || [ "$OS" = "UBUNTU64" ]
then
    echo "COPYING SHARE DOCS"
    
    sudo rm -rf $INSTALL_SHARE_PATH
    sudo cp -r $LOCAL_SHARE_DOCS_PATH $INSTALL_SHARE_PATH
    
    echo "COPYING DEBIAN FILES"
    
    sudo rm -rf $INSTALL_DEBIAN_PATH
    sudo cp -r $LOCAL_DEBIAN_PATH $INSTALL_DEBIAN_PATH
    
    sudo chown root -R $INSTALL_USR_PATH
    sudo chmod a+x $INSTALL_DEBIAN_PATH"post"*
    sudo chmod a+x $INSTALL_DEBIAN_PATH"post"*
    
    if [ "$OS" = "UBUNTU32" ]
    then
	sudo mv $INSTALL_DEBIAN_PATH"control_32" $INSTALL_DEBIAN_PATH"control"
	sudo rm $INSTALL_DEBIAN_PATH"control_64"
    elif [ "$OS" = "UBUNTU64" ]
    then
	sudo mv $INSTALL_DEBIAN_PATH"control_64" $INSTALL_DEBIAN_PATH"control"
	sudo rm $INSTALL_DEBIAN_PATH"control_32"
    fi
elif [ "$OS" = "MACOS32" ] || [ "$OS" = "MACOS64" ]
then
    echo "SETTING UP RIGHTS"
    sudo chown -R root $INSTALL_USR_PATH
    sudo chown -R root $INSTALL_FRAMEWORKS_PATH
else
    echo "Unhandled OS"
fi