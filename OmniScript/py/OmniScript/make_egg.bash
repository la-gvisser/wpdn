#!/bin/bash

main_src="../../../../eng/main/src/omni/bin"
priv_src="../../../../eng/main/src/omni/bin"
while [[ "$#" > 0 ]];
  do case $1 in
    "gary") priv_src="../../../../eng/gary/src/omni/bin";;
    *) echo "Unknown parameter passed: $1"; exit 1;;
  esac;
  shift;
done

copy_library() {
  if [ -e /usr/lib/$1 ]; then
    cp -f /usr/lib/$1 ./omniscript
  elif [ -e $main_sr/lib/$1 ]; then
    cp -f $main_sr/lib/$1 ./omniscript
  elif [ -e $priv_sr/lib/$1 ]; then
    cp -f $priv_sr/lib/$1 ./omniscript
  else
    echo "Can't find source for $1"
    exit 1
  fi
}

# create or clear the directories
if [ ! -e build ]; then
    mkdir build
else
	rm -fr ./build/*
fi
if [ ! -e bin ]; then
    mkdir bin
else
	rm -fr ./bin/*
fi
if [ ! -e ./omniscript/Decodes ]; then
    mkdir ./omniscript/Decodes
else
	rm -fr ./omniscript/Decodes/*
fi

rm -f ./omniscript/omniapi.he
rm -f ./omniscript/*.pyc
rm -f ./omniscript/*.so
rm -f ./omniscript/*.txt
rm -f ./omniscript/*.xml

# populate the build directory
cp ./RCapture/RCapture.py ./bin
cp ./Forensics/Forensics.py ./bin
cp ../../shared/*.txt ./omniscript/
cp ../../shared/*.xml ./omniscript/
cp ../../shared/decodes.bin ./omniscript/Decodes/

copy_library libheruntime.so
copy_library libwpz.so
copy_library peekutil.so
copy_library wpxml.so

cp $priv_src/libomniapi.so ./omniscript/
cp $priv_src/omniapi.he ./omniscript/

# build the egg.
python ./setup_ux.py bdist_egg

# install the new egg.
cd dist
sudo pip uninstall OmniScript < y
sudo python /usr/share/pyshared/ez_setup.py ./OmniScript-0.6.3-py2.7.egg
cd ..
