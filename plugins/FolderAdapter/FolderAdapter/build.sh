#!/bin/bash -e

#---------------------------------------------------------------------------------
function displayHelp() {
  echo ""
  echo -e "${BROWN}=========================================================${NO_COLOR}"
  echo -e "${BROWN}================= Plugin build script. }=================${NO_COLOR}"
  echo -e "${BROWN}=========================================================${NO_COLOR}"
  echo ""
  echo "Plugin build options (all optional, apply in any order):"
  echo ""
  echo -e "${YELLOW}  --release${NO_COLOR}      Build in release mode (default)."
  echo -e "${YELLOW}  --debug${NO_COLOR}        Build in debug mode."
  echo -e "${YELLOW}  --nobuild${NO_COLOR}      Don't build, used with --clean."
  echo -e "${YELLOW}  --clean${NO_COLOR}        Does a clean build."
  echo -e "${YELLOW}  --help${NO_COLOR}         Displays this help message."
  echo ""
  echo -e "Example usage:"
  echo -e ""
  echo -e "${YELLOW}  ./build.sh --release --clean${NO_COLOR}"
  echo ""
  exit
}


#---------------------------------------------------------------------------------
function initialize() {
  buildType=RELEASE
  cleanOpt=""
  for opt in "$@"
  do
    if [ "$opt" = "--release" ]; then
      buildType=RELEASE
    elif [ "$opt" = "--debug" ]; then
      buildType=DEBUG
    elif [ "$opt" = "--nobuild" ]; then
      buildType=NOBUILD
    elif [ "$opt" = "--clean" ]; then
      cleanOpt="clean"
    elif [ "$opt" = "--help" ]; then
      displayHelp
    else
      echo "Unrecognized option: $opt"
      echo ""
      displayHelp
    fi
  done;
}


#---------------------------------------------------------------------------------
# Main script
initialize $@

# Clean
if [ "$cleanOpt" = "clean" ]; then
  rm -rf CMakeFiles
  rm -rf _CPack_Packages
  rm -f Plugin/*.zip
  rm -f Plugin/*.tar.gz
  rm -f CMakeCache.txt
  rm -f cmake_install.cmake
  rm -f Makefile
  rm -f CPackConfig.cmake
  rm -f CPackSourceConfig.cmake
  rm -f install_manifest.txt
  rm -f *.deb
  rm -f *.so
fi

if [ "$buildType" = "NOBUILD" ]; then
  exit 0
fi

# Force rebuild of all .zip and .tar.gz files
rm -f Plugin/*.zip
rm -f Plugin/*.tar.gz
./makezips.bash

# OS Version
osversion=$(lsb_release -rs)

# Build Plugin and the Debian installer.
# The default is to build in Release mode, which is what folks will probably want.
echo "Build Plugin and the Debian installer for Ubuntu $osversion"
/usr/bin/cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=$buildType ./CMakeLists.txt
make -j8
echo "make Debian package"
if [[ "$USER" != "" ]]; then
  make package
  chmod 666 *.deb
else
  make package
  chmod 666 *.deb
fi
