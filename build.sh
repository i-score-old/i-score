#!/bin/sh

make distclean;
qmake -spec unsupported/macx-clang;
make;