#!/bin/bash

if [ -d "_CPack_Pakages" ]; then
    rm -rf "_CPack_Pakages/"
fi
if [ -d CMakeFiles ]; then
    rm -rf CMakeFiles
fi
if [ -d CMakeFiles ]; then
    rm -rf CMakeFiles
fi
if [ -e "Plugin/*.zip" ]; then
    rm -f Plugin/*.zip
fi
if [ -e "Plugin/*.tar.gz" ]; then
    rm -f Plugin/*.tar.gz
fi
if [ -f CMakeCache.txt ]; then
    rm CMakeCache.txt
fi
if [ -f cmake_install.cmake ]; then
    rm cmake_install.cmake
fi
if [ -f MakeFile ]; then
    rm Makefile
fi
if [ -f CPackConfig.cmake ]; then
    rm CPackConfig.cmake
fi
if [ -f CPackSourceConfig.cmake ]; then
    rm CPackSourceConfig.cmake
fi
if [ -f install_manifest.txt ]; then
    rm install_manifest.txt
fi
if [ -e "*.deb" ]; then
    rm *.deb
fi
if [ -e "*.so" ]; then
    rm *.so
fi