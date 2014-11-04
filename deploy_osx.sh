#!/bin/bash -x

ISCORE_EXECUTABLE_NAME=i-score
/usr/local/Cellar/qt5/5.3.2/bin/macdeployqt $ISCORE_EXECUTABLE_NAME.app

mkdir -p $ISCORE_EXECUTABLE_NAME.app/Contents/Frameworks/jamoma/lib
mkdir -p $ISCORE_EXECUTABLE_NAME.app/Contents/Frameworks/jamoma/extensions


### Deployment ###
# Jamoma libs
declare -a jamomalibs=("Foundation" "Modular" "DSP" "Score")
declare -a jamomaexts=("Scenario" "Automation" "Interval" "OSC" "Minuit" "MIDI" "AnalysisLib" "DataspaceLib" "FunctionLib" "System" "NetworkLib")
for JAMOMA_LIB in "${jamomalibs[@]}"
do
	cp -rf /usr/local/jamoma/lib/libJamoma$JAMOMA_LIB.dylib $ISCORE_EXECUTABLE_NAME.app/Contents/Frameworks/jamoma/lib/
done
for JAMOMA_EXT in "${jamomaexts[@]}"
do
	cp -rf /usr/local/jamoma/extensions/$JAMOMA_EXT.ttdylib $ISCORE_EXECUTABLE_NAME.app/Contents/Frameworks/jamoma/extensions/
done
# Jamoma rpath
install_name_tool -add_rpath @executable_path/../Frameworks/jamoma/lib $ISCORE_EXECUTABLE_NAME.app/Contents/MacOS/i-score
install_name_tool -add_rpath @executable_path/../Frameworks/jamoma/extensions $ISCORE_EXECUTABLE_NAME.app/Contents/MacOS/i-score


# Portmidi
cp -rf /usr/local/lib/libportmidi.dylib $ISCORE_EXECUTABLE_NAME.app/Contents/Frameworks/
install_name_tool -change /usr/local/lib/libportmidi.dylib @executable_path/../Frameworks/libportmidi.dylib $ISCORE_EXECUTABLE_NAME.app/Contents/Frameworks/jamoma/extensions/MIDI.ttdylib

# Gecode linking
declare -a gecodelibs=("kernel" "support" "int" "set" "driver" "flatzinc" "minimodel" "search" "float")
for GECODE_LIB in "${gecodelibs[@]}"
do
	install_name_tool -change /usr/local/lib/libgecode$GECODE_LIB.36.dylib @executable_path/../Frameworks/libgecode$GECODE_LIB.36.dylib $ISCORE_EXECUTABLE_NAME.app/Contents/Frameworks/jamoma/extensions/Scenario.ttdylib
done
