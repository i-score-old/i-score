#!/bin/sh

make distclean;
qmake -nocache -spec macx-clang CONFIG+=debug i-score.pro;
make -j 4;
