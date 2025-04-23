#!/bin/bash

pushd /usr/lib/omni/plugins >/dev/null
if [ -f CefPrefsOE-3.so ]; then
    rm CefPrefsOE-3.so
fi
if [ -f CefPrefsOE-3.he ]; then
    rm CefPrefsOE-3.he
fi
if [ -f CefPrefsOE-3-About.zip ]; then
    rm CefPrefsOE-3-About.zip
fi
if [ -f CefPrefsOE-3-CaptureTab.zip ]; then
    rm CefPrefsOE-3-CaptureTab.zip
fi
if [ -f CefPrefsOE-3-EngineTab.zip ]; then
    rm CefPrefsOE-3-EngineTab.zip
fi
if [ -f CefPrefsOE-3-Options.zip ]; then
    rm CefPrefsOE-3-Options.zip
fi
if [ -d web/C0E0F020-2468-1357-C1C4-333333333333 ]; then
    rm -r web/C0E0F020-2468-1357-C1C4-333333333333
fi
popd >/dev/null