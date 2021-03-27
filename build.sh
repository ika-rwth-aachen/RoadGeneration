#!/bin/bash

git submodule update --init
mkdir -p build
cd build
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../bin ..
cmake --build .
cd ..
