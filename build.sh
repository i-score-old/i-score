#!/bin/sh

make distclean;
qmake -spec unsupported/macx-clang i-score.pro;
make;
