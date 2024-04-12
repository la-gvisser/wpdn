#!/usr/bin/env bash
cd Plugin

# Remove the existing zip files.
rm -f HTMLPrefsOE-3-About.zip
rm -f HTMLPrefsOE-3-CaptureTab.zip
rm -f HTMLPrefsOE-3-EngineTab.zip
rm -f HTMLPrefsOE-3-Options.zip

# Create HTMLPrefsOE-3-About.zip
cd About/About-3
zip -r ../../HTMLPrefsOE-3-About.zip *
cd ../..

# Create HTMLPrefsOE-3-CaptureTab.zip
cd CaptureTab/CaptureTab-3
zip -r ../../HTMLPrefsOE-3-CaptureTab.zip *
cd ../..

# Create HTMLPrefsOE-3-EngineTab.zip
cd EngineTab/EngineTab-3
zip -r ../../HTMLPrefsOE-3-EngineTab.zip *
cd ../..

# Create HTMLPrefsOE-3-Options.zip
cd Options/Options-3
zip -r ../../HTMLPrefsOE-3-Options.zip *
cd ../..

# Return to the original folder.
cd ..

