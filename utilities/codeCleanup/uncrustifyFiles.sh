#! /bin/sh

files=$(find src headers -name "*.cpp" -or -name "*.hpp")


for item in $files ; do

  uncrustify -c ./kr_indent.cfg --no-backup $item

done

