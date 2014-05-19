#!/bin/sh
cp -rf ../../../build-i-score-Desktop-Debug/i-score.app .
mkdir -p i-score.app/Contents/Frameworks/
cp -rf /usr/local/jamoma* i-score.app/Contents/Frameworks/

# Jamoma rpath
install_name_tool -add_rpath @executable_path/../Frameworks/jamoma/lib i-score.app/Contents/MacOS/i-score
install_name_tool -add_rpath @executable_path/../Frameworks/jamoma/extensions i-score.app/Contents/MacOS/i-score

# Qt
#cp -R /usr/local/lib/QtSvg.framework /usr/local/lib/QtGui.framework /usr/local/lib/QtCore.framework /usr/local/lib/QtXml.framework i-score.app/Contents/Frameworks/

#install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore i-score.app/Contents/Frameworks/QtCore.framework/Versions/4/QtCore
#install_name_tool -id @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui i-score.app/Contents/Frameworks/QtGui.framework/Versions/4/QtGui
#install_name_tool -id @executable_path/../Frameworks/QtSvg.framework/Versions/4/QtSvg i-score.app/Contents/Frameworks/QtSvg.framework/Versions/4/QtSvg
#install_name_tool -id @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml i-score.app/Contents/Frameworks/QtXml.framework/Versions/4/QtXml

#install_name_tool -change /usr/local/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore i-score.app/Contents/MacOS/i-score
#install_name_tool -change /usr/local/lib/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui i-score.app/Contents/MacOS/i-score
#install_name_tool -change /usr/local/lib/QtSvg.framework/Versions/4/QtSvg @executable_path/../Frameworks/QtSvg.framework/Versions/4/QtSvg i-score.app/Contents/MacOS/i-score
#install_name_tool -change /usr/local/lib/QtXml.framework/Versions/4/QtXml @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml i-score.app/Contents/MacOS/i-score

# À faire aussi pour les librairies propres à Qt ?

# Gecode
