#!/bin/bash

PLUGIN=FolderAdapterOE

function need_7z {
  echo $0 "requires 7z. 'sudo apt-get install p7zip-full'"
  exit 1
}

if [ -d "Plugin/CaptureTab" ]; then
  pushd Plugin/CaptureTab
  if [ -f ../$PLUGIN-CaptureTab.zip ]; then
    rm ../$PLUGIN-CaptureTab.zip
  fi
  7z a ../$PLUGIN-CaptureTab.zip
  chmod 644 ../$PLUGIN-CaptureTab.zip
  popd
fi

if [ -d "Plugin/Options" ]; then
  pushd Plugin/Options
  if [ -f ../$PLUGIN-Options.zip ]; then
    rm ../$PLUGIN-Options.zip
  fi
  7z a ../$PLUGIN-Options.zip
  chmod 644 ../$PLUGIN-Options.zip
  popd
fi

if [ -d "Plugin/web" ]; then
  pushd Plugin/web
  if [ -f ../$PLUGIN-web.tar.gz ]; then
    rm ../$PLUGIN-web.tar.gz
  fi
  tar -czf ../$PLUGIN-web.tar.gz *
  chmod 644 ../$PLUGIN-web.tar.gz
  popd
fi
