#!/usr/bin/env bash
cd Plugin

# Remove the existing zip files.
rm -f HTMLPrefsOE-4-About.zip
rm -f HTMLPrefsOE-4-CaptureTab.zip
rm -f HTMLPrefsOE-4-EngineTab.zip
rm -f HTMLPrefsOE-4-Options.zip

# Create HTMLPrefsOE-4-About.zip
cd About/About-4
zip -r ../../HTMLPrefsOE-4-About.zip *
cd ../..

# Create HTMLPrefsOE-4-CaptureTab.zip
cd CaptureTab/CaptureTab-4
zip -r ../../HTMLPrefsOE-4-CaptureTab.zip *
cd ../..

# Create HTMLPrefsOE-4-EngineTab.zip
cd EngineTab/EngineTab-4
zip -r ../../HTMLPrefsOE-4-EngineTab.zip *
cd ../..

# Create HTMLPrefsOE-4-Options.zip
cd Options/Options-4
zip -r ../../HTMLPrefsOE-4-Options.zip *
cd ../..

# Return to the original folder.
cd ..

