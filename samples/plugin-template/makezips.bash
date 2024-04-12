#!/bin/bash

PLUGIN=PluginTemplateOE

function need_7z {
  echo $0 "requires 7z. 'sudo apt-get install p7zip-full'"
  exit 1
}

if [ -d "Plugin/About" ]; then
  pushd Plugin/About
  7z a ../$PLUGIN-About.zip
  chmod 644 ../$PLUGIN-About.zip
  popd
fi

if [ -d "Plugin/CaptureTab" ]; then
  pushd Plugin/CaptureTab
  7z a ../$PLUGIN-CaptureTab.zip
  chmod 644 ../$PLUGIN-CaptureTab.zip
  popd
fi

# if [ -d "Plugin/EngineTab" ]; then
#   pushd Plugin/EngineTab
#   7z a ../$PLUGIN-EngineTab.zip
#   chmod 644 ../$PLUGIN-EngineTab.zip
#   popd
# fi

if [ -d "Plugin/Options" ]; then
  pushd Plugin/Options
  7z a ../$PLUGIN-Options.zip
  chmod 644 ../$PLUGIN-Options.zip
  popd
fi

if [ -d "Plugin/web" ]; then
   pushd Plugin/web
   tar -czf ../$PLUGIN-web.tar.gz *
   chmod 644 ../$PLUGIN-web.tar.gz
   popd
fi
