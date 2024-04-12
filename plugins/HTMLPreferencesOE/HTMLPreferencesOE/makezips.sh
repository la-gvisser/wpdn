#!/usr/bin/env bash
cd Plugin

# Remove the existing zip files.
rm -f About.zip
rm -f CaptureTab.zip
rm -f EngineTab.zip
rm -f Options.zip

# Create About.zip
cd About/About
zip -r ../../About.zip *
cd ../..

# Create CaptureTab.zip
cd CaptureTab/CaptureTab
zip -r ../../CaptureTab.zip *
cd ../..

# Create EngineTab.zip
cd EngineTab/EngineTab
zip -r ../../EngineTab.zip *
cd ../..

# Create Options.zip
cd Options/Options
zip -r ../../Options.zip *
cd ../..

# Return to the original folder.
cd ..

