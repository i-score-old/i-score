#!/bin/sh

echo 'Resources'
echo '...Ajout'
rm -rf acousmoscribe2.app/Contents/Resources/images acousmoscribe2.app/Contents/Resources/documentation
rm -rf acousmoscribe2.app/Contents/Resources/images acousmoscribe2.app/Contents/Resources/images
mkdir acousmoscribe2.app/Contents/Resources/images
mkdir acousmoscribe2.app/Contents/Resources/documentation

echo '...Transfert'
cp -R images/ acousmoscribe2.app/Contents/Resources/images/
cp -R documentation/ acousmoscribe2.app/Contents/Resources/documentation/

echo 'Frameworks'
echo '...Ajout'
mkdir acousmoscribe2.app/Contents/Frameworks/
rm -rf acousmoscribe2.app/Contents/Frameworks/gecode.framework
rm -rf acousmoscribe2.app/Contents/Frameworks/libxml.framework
mkdir acousmoscribe2.app/Contents/Frameworks/gecode.framework
mkdir acousmoscribe2.app/Contents/Frameworks/libxml.framework

echo '...Transfert'
cp -R /Library/Frameworks/gecode.framework/ acousmoscribe2.app/Contents/Frameworks/gecode.framework/
cp -R /Library/Frameworks/libxml.framework/ acousmoscribe2.app/Contents/Frameworks/libxml.framework/

echo '...Configuration'
echo '......Identification Names'
install_name_tool -id @executable_path/../Frameworks/gecode.framework/Versions/17/gecode acousmoscribe2.app/Contents/Frameworks/gecode.framework/Versions/17/gecode
install_name_tool -id @executable_path/../Frameworks/libxml.framework/Versions/2.6.30/libxml acousmoscribe2.app/Contents/Frameworks/libxml.framework/Versions/2.6.30/libxml
echo '......Link application to libraries'
install_name_tool -change /Library/Frameworks/gecode.framework/Versions/17/gecode @executable_path/../Frameworks/gecode.framework/Versions/17/gecode acousmoscribe2.app/Contents/MacOs/acousmoscribe2
install_name_tool -change /Library/Frameworks/libxml.framework/Versions/2.6.30/libxml @executable_path/../Frameworks/libxml.framework/Versions/2.6.30/libxml acousmoscribe2.app/Contents/MacOs/acousmoscribe2

echo 'Qt Deployment'
./macdeployqt acousmoscribe2.app
