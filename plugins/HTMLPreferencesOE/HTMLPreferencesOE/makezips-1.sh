#!/usr/bin/env bash
cd Plugin

# Remove the existing zip files.
rm -f HTMLPrefsOE-1-About.zip
rm -f HTMLPrefsOE-1-CaptureTab.zip
rm -f HTMLPrefsOE-1-EngineTab.zip
rm -f HTMLPrefsOE-1-Options.zip

# Create HTMLPrefsOE-1-About.zip
cd About/About-1
zip -r ../../HTMLPrefsOE-1-About.zip *
cd ../..

# Create HTMLPrefsOE-1-CaptureTab.zip
cd CaptureTab/CaptureTab-1
zip -r ../../HTMLPrefsOE-1-CaptureTab.zip *
cd ../..

# Create HTMLPrefsOE-1-EngineTab.zip
cd EngineTab/EngineTab-1
zip -r ../../HTMLPrefsOE-1-EngineTab.zip *
cd ../..

# Create HTMLPrefsOE-1-Options.zip
cd Options/Options-1
zip -r ../../HTMLPrefsOE-1-Options.zip *
cd ../..

# Return to the original folder.
cd ..

