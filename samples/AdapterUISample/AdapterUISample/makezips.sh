#!/usr/bin/env bash
PLUGIN_DIR=./Plugin
OPTIONS_DIR=./Options
ZIP_FILE=Options

cd $PLUGIN_DIR
rm -f $ZIP_FILE.zip
cd $OPTIONS_DIR
zip -r ../$ZIP_FILE.zip *
cd ../..
