#!/bin/bash

# download dependencies
sudo apt install gnuplot gnuplot-x11
sudo apt install xorg-dev
sudo apt install mesa-common-dev libgl1-mesa-dev

# build EMOC
mkdir build
cd build
cmake -DBUILD_PYTHON_DLL=OFF ..
make -j4
