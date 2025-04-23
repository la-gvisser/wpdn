#!/bin/bash

pushd /usr/lib/omni/plugins >/dev/null
if [ -f CefPrefsOE-2.so ]; then
    rm CefPrefsOE-2.so
fi
if [ -f CefPrefsOE-2.he ]; then
    rm CefPrefsOE-2.he
fi
if [ -f CefPrefsOE-2-About.zip ]; then
    rm CefPrefsOE-2-About.zip
fi
if [ -f CefPrefsOE-2-CaptureTab.zip ]; then
    rm CefPrefsOE-2-CaptureTab.zip
fi
if [ -f CefPrefsOE-2-EngineTab.zip ]; then
    rm CefPrefsOE-2-EngineTab.zip
fi
if [ -f CefPrefsOE-2-Options.zip ]; then
    rm CefPrefsOE-2-Options.zip
fi
if [ -d web/C0E0F020-2468-1357-C1C4-323232323232 ]; then
    rm -r web/C0E0F020-2468-1357-C1C4-323232323232
fi
popd >/dev/null