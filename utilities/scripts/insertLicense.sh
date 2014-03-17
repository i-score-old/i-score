#!/bin/sh

# Licence File
LICENCE=./LicenceCecill.txt

# For each code file in the directory 
for FILE in $(ls *.hpp *.cpp)
# Concatenating licence and file into file
do
    touch tmp ; cat $LICENCE > tmp ; cat $FILE >> tmp ; cat tmp > $FILE ; rm tmp;
    echo "Handling file "$FILE
done
