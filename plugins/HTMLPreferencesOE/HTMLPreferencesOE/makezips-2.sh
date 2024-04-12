#!/usr/bin/env bash
cd Plugin

# Remove the existing zip files.
rm -f HTMLPrefsOE-2-About.zip
rm -f HTMLPrefsOE-2-CaptureTab.zip
rm -f HTMLPrefsOE-2-EngineTab.zip
rm -f HTMLPrefsOE-2-Options.zip

# Create HTMLPrefsOE-2-About.zip
cd About/About-2
zip -r ../../HTMLPrefsOE-2-About.zip *
cd ../..

# Create HTMLPrefsOE-2-CaptureTab.zip
cd CaptureTab/CaptureTab-2
zip -r ../../HTMLPrefsOE-2-CaptureTab.zip *
cd ../..

# Create HTMLPrefsOE-2-EngineTab.zip
cd EngineTab/EngineTab-2
zip -r ../../HTMLPrefsOE-2-EngineTab.zip *
cd ../..

# Create HTMLPrefsOE-2-Options.zip
cd Options/Options-2
zip -r ../../HTMLPrefsOE-2-Options.zip *
cd ../..

# Return to the original folder.
cd ..

