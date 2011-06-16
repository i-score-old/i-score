#!/bin/bash

GCC=g++
CFLAGS=-D X86_64 -c -fPIC

cd Minuit
$(GCC) $(CFLAGS) *.cpp
cd ../NetworkCommunication/ip
$(GCC) $(CFLAGS) *.cpp
cd ../osc
$(GCC) $(CFLAGS) *.cpp
cd ../..