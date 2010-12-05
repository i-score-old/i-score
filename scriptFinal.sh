#!/bin/sh

echo 'Resources'
echo '...Ajout'
rm -rf acousmoscribe.app/Contents/Resources/images acousmoscribe.app/Contents/Resources/documentation
rm -rf acousmoscribe.app/Contents/Resources/images acousmoscribe.app/Contents/Resources/images
mkdir acousmoscribe.app/Contents/Resources/images
mkdir acousmoscribe.app/Contents/Resources/documentation

echo '...Transfert'
cp -R images/ acousmoscribe.app/Contents/Resources/images/
cp -R documentation/ acousmoscribe.app/Contents/Resources/documentation/

echo 'Frameworks'
echo '...Ajout'
mkdir acousmoscribe.app/Contents/Frameworks/
rm -rf acousmoscribe.app/Contents/Frameworks/gecode.framework
rm -rf acousmoscribe.app/Contents/Frameworks/libxml.framework
rm -rf acousmoscribe.app/Contents/Frameworks/QtCore.framework
rm -rf acousmoscribe.app/Contents/Frameworks/QtGui.framework
rm -rf acousmoscribe.app/Contents/Frameworks/QtNetwork.framework
rm -rf acousmoscribe.app/Contents/Frameworks/QtSvg.framework
rm -rf acousmoscribe.app/Contents/Frameworks/QtXml.framework
mkdir acousmoscribe.app/Contents/Frameworks/gecode.framework
mkdir acousmoscribe.app/Contents/Frameworks/libxml.framework
mkdir acousmoscribe.app/Contents/Frameworks/QtCore.framework
mkdir acousmoscribe.app/Contents/Frameworks/QtGui.framework
mkdir acousmoscribe.app/Contents/Frameworks/QtNetwork.framework
mkdir acousmoscribe.app/Contents/Frameworks/QtSvg.framework
mkdir acousmoscribe.app/Contents/Frameworks/QtXml.framework

rm -rf acousmoscribe.app/Contents/plugins
mkdir acousmoscribe.app/Contents/plugins


echo '...Transfert'
cp -R /Library/Frameworks/gecode.framework/ acousmoscribe.app/Contents/Frameworks/gecode.framework/
cp -R /Library/Frameworks/libxml.framework/ acousmoscribe.app/Contents/Frameworks/libxml.framework/
cp -R /Library/Frameworks/QtCore.framework/ acousmoscribe.app/Contents/Frameworks/QtCore.framework/
cp -R /Library/Frameworks/QtGui.framework/ acousmoscribe.app/Contents/Frameworks/QtGui.framework/
cp -R /Library/Frameworks/QtNetwork.framework/ acousmoscribe.app/Contents/Frameworks/QtNetwork.framework/
cp -R /Library/Frameworks/QtSvg.framework/ acousmoscribe.app/Contents/Frameworks/QtSvg.framework/
cp -R /Library/Frameworks/QtXml.framework/ acousmoscribe.app/Contents/Frameworks/QtXml.framework/

echo '...Configuration'
echo '......Identification Names'
install_name_tool -id @executable_path/../Frameworks/gecode.framework/Versions/17/gecode acousmoscribe.app/Contents/Frameworks/gecode.framework/Versions/17/gecode
install_name_tool -id @executable_path/../Frameworks/libxml.framework/Versions/2.6.30/libxml acousmoscribe.app/Contents/Frameworks/libxml.framework/Versions/2.6.30/libxml
install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore acousmoscribe.app/Contents/Frameworks/QtCore.framework/Versions/4/QtCore
install_name_tool -id @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui acousmoscribe.app/Contents/Frameworks/QtGui.framework/Versions/4/QtGui
install_name_tool -id @executable_path/../Frameworks/QtNetwork.framework/Versions/4/QtNetwork acousmoscribe.app/Contents/Frameworks/QtNetwork.framework/Versions/4/QtNetwork
install_name_tool -id @executable_path/../Frameworks/QtSvg.framework/Versions/4/QtSvg acousmoscribe.app/Contents/Frameworks/QtSvg.framework/Versions/4/QtSvg
install_name_tool -id @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml acousmoscribe.app/Contents/Frameworks/QtXml.framework/Versions/4/QtXml
echo '......Link application to libraries'
install_name_tool -change /Library/Frameworks/gecode.framework/Versions/17/gecode @executable_path/../Frameworks/gecode.framework/Versions/17/gecode acousmoscribe.app/Contents/MacOS/acousmoscribe
install_name_tool -change /Library/Frameworks/libxml.framework/Versions/2.6.30/libxml @executable_path/../Frameworks/libxml.framework/Versions/2.6.30/libxml acousmoscribe.app/Contents/MacOS/acousmoscribe
install_name_tool -change /Library/Frameworks/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore acousmoscribe.app/Contents/MacOS/acousmoscribe
install_name_tool -change /Library/Frameworks/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui acousmoscribe.app/Contents/MacOS/acousmoscribe
install_name_tool -change /Library/Frameworks/QtNetwork.framework/Versions/4/QtNetwork @executable_path/../Frameworks/QtNetwork.framework/Versions/4/QtNetwork acousmoscribe.app/Contents/MacOS/acousmoscribe
install_name_tool -change /Library/Frameworks/QtSvg.framework/Versions/4/QtSvg @executable_path/../Frameworks/QtSvg.framework/Versions/4/QtSvg acousmoscribe.app/Contents/MacOS/acousmoscribe
install_name_tool -change /Library/Frameworks/QtXml.framework/Versions/4/QtXml @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml acousmoscribe.app/Contents/MacOS/acousmoscribe

#echo 'Qt Deployment'
#./macdeployqt acousmoscribe2.app
