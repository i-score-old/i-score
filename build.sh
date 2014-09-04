#!/bin/sh

make distclean;
qmake -nocache -spec macx-clang i-score.pro;
make;
