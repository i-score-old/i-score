#!/bin/bash
ISCORE_BUNDLE=_CPack_Packages/Darwin/DragNDrop/i-score/ALL_IN_ONE/i-score.app
ISCORE_EXECUTABLE_NAME=i-score

if [[ -f $PWD/../../settings.sh ]]; then
	source $PWD/../../settings.sh
fi

if [[ -z "$ISCORE_QT_PATH" ]]; then
	MACDEPLOYQT=$(find /usr/local/Cellar/qt5/ -name macdeployqt)
else
	MACDEPLOYQT="$ISCORE_QT_PATH/bin/macdeployqt"
fi
$MACDEPLOYQT $ISCORE_BUNDLE

mkdir -p $ISCORE_BUNDLE/Contents/Frameworks/jamoma/lib
mkdir -p $ISCORE_BUNDLE/Contents/Frameworks/jamoma/extensions


### Deployment ###
# Jamoma libs
declare -a jamomalibs=("Foundation" "Modular" "DSP" "Score")
declare -a jamomaexts=("Scenario" "Automation" "Interval" "Loop" "OSC" "Minuit" "MIDI" "AnalysisLib" "DataspaceLib" "FunctionLib" "System" "NetworkLib")
for JAMOMA_LIB in "${jamomalibs[@]}"
do
	cp -rf /usr/local/jamoma/lib/libJamoma$JAMOMA_LIB.6.dylib $ISCORE_BUNDLE/Contents/Frameworks/jamoma/lib/
done
for JAMOMA_EXT in "${jamomaexts[@]}"
do
	cp -rf /usr/local/jamoma/extensions/$JAMOMA_EXT.ttdylib $ISCORE_BUNDLE/Contents/Frameworks/jamoma/extensions/
done
# Jamoma rpath
install_name_tool -add_rpath @executable_path/../Frameworks/jamoma/lib $ISCORE_BUNDLE/Contents/MacOS/i-score
install_name_tool -add_rpath @executable_path/../Frameworks/jamoma/extensions $ISCORE_BUNDLE/Contents/MacOS/i-score


# Portmidi
cp -rf /usr/local/lib/libportmidi.dylib $ISCORE_BUNDLE/Contents/Frameworks/
install_name_tool -change /usr/local/lib/libportmidi.dylib @executable_path/../Frameworks/libportmidi.dylib $ISCORE_BUNDLE/Contents/Frameworks/jamoma/extensions/MIDI.ttdylib

# Gecode linking
GECODE_DIR=$(dirname $(find /usr -name libgecodekernel.dylib ! -path "*Cellar*"|head -1))
declare -a gecodelibs=("kernel" "support" "int" "set" "driver" "flatzinc" "minimodel" "search" "float")
for GECODE_LIB in "${gecodelibs[@]}"
do
	install_name_tool -change $GECODE_DIR/libgecode$GECODE_LIB.39.dylib @executable_path/../Frameworks/libgecode$GECODE_LIB.39.dylib $ISCORE_BUNDLE/Contents/Frameworks/jamoma/extensions/Scenario.ttdylib
done
