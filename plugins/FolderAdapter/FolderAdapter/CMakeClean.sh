#!/bin/bash

osversion=$(lsb_release -rs | sed 's/\./_/g')
make clean
rm -rf CMakeFiles
rm CMakeCache.txt
rm cmake_install.cmake
rm Makefile
rm -rf _CPack_Packages
rm CPackConfig.cmake
rm CPackSourceConfig.cmake
rm install_manifest.txt
rm -rf ./bin$osversion/_CPack_Packages
