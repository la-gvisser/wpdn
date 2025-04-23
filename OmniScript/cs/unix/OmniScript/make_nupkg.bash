#!/bin/bash

version=2.7.1
while [[ "$#" > 0 ]];
  do case $1 in
    "-v") shift; version=$1;;
    *) echo "Unknown parameter passed: $1"; exit 1;;
  esac;
  shift;
done

if [ ! -d ~/packages ]; then mkdir ~/packages; fi
if [ -f ~/packages/OmniScript.$version.nupkg ]; then rm -f ~/packages/OmniScript.$version.nupkg; fi
if [ -f ~/.nuget/packages/omniscript ]; then rm -fr ~/.nuget/packages/omniscript; fi

if [ -d ./bin ]; then rm -fr ./bin; fi
if [ -d ./obj ]; then rm -fr ./obj; fi

dotnet restore
dotnet build -c Release
dotnet pack -c Release
dotnet publish -c Release

cp ~/wp/wpdn/OmniScript/cs/unix/OmniScript/bin/Release/OmniScript.$version.nupkg ~/packages/
