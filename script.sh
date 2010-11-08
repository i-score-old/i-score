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
mkdir acousmoscribe.app/Contents/Frameworks/gecode.framework
mkdir acousmoscribe.app/Contents/Frameworks/libxml.framework

echo '...Transfert'
cp -R /Library/Frameworks/gecode.framework/ acousmoscribe.app/Contents/Frameworks/gecode.framework/
cp -R /Library/Frameworks/libxml.framework/ acousmoscribe.app/Contents/Frameworks/libxml.framework/

echo '...Configuration'
echo '......Identification Names'
install_name_tool -id @executable_path/../Frameworks/gecode.framework/Versions/17/gecode acousmoscribe.app/Contents/Frameworks/gecode.framework/Versions/17/gecode
install_name_tool -id @executable_path/../Frameworks/libxml.framework/Versions/2.6.30/libxml acousmoscribe.app/Contents/Frameworks/libxml.framework/Versions/2.6.30/libxml
echo '......Link application to libraries'
install_name_tool -change /Library/Frameworks/gecode.framework/Versions/17/gecode @executable_path/../Frameworks/gecode.framework/Versions/17/gecode acousmoscribe.app/Contents/MacOS/acousmoscribe
install_name_tool -change /Library/Frameworks/libxml.framework/Versions/2.6.30/libxml @executable_path/../Frameworks/libxml.framework/Versions/2.6.30/libxml acousmoscribe.app/Contents/MacOS/acousmoscribe

#echo 'Qt Deployment'
#./macdeployqt acousmoscribe2.app
