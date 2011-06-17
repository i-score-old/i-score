#!/bin/bash

cd Minuit
g++ -Dx86_64 -c -fPIC *.cpp
cd ../NetworkCommunication/ip
g++ -Dx86_64 -c -fPIC *.cpp
cd ../osc
g++ -Dx86_64 -c -fPIC *.cpp
cd ../..