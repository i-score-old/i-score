#!/bin/bash

cd Minuit
g++ -c -fPIC *.cpp
cd ../NetworkCommunication/ip
g++ -c -fPIC *.cpp
cd ../osc
g++ -c -fPIC *.cpp
cd ../..
