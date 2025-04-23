#!/bin/bash

pushd /usr/lib/omni/plugins >/dev/null
if [ -f CefPrefsOE-4.so ]; then
    rm CefPrefsOE-4.so
fi
if [ -f CefPrefsOE-4.he ]; then
    rm CefPrefsOE-4.he
fi
if [ -f CefPrefsOE-4-About.zip ]; then
    rm CefPrefsOE-4-About.zip
fi
if [ -f CefPrefsOE-4-CaptureTab.zip ]; then
    rm CefPrefsOE-4-CaptureTab.zip
fi
if [ -f CefPrefsOE-4-EngineTab.zip ]; then
    rm CefPrefsOE-4-EngineTab.zip
fi
if [ -f CefPrefsOE-4-Options.zip ]; then
    rm CefPrefsOE-4-Options.zip
fi
if [ -d web/C0E0F020-2468-1357-C1C4-343434343434 ]; then
    rm -r web/C0E0F020-2468-1357-C1C4-343434343434
fi
popd >/dev/null