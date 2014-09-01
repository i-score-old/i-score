#!/bin/sh

make distclean;
qmake -nocache -spec unsupported/macx-clang i-score.pro;
make;
