#!/bin/bash

pushd /usr/lib/omni/plugins >/dev/null
if [ -f CefPrefsOE-1.so ]; then
    rm CefPrefsOE-1.so
fi
if [ -f CefPrefsOE-1.he ]; then
    rm CefPrefsOE-1.he
fi
if [ -f CefPrefsOE-1-About.zip ]; then
    rm CefPrefsOE-1-About.zip
fi
if [ -f CefPrefsOE-1-CaptureTab.zip ]; then
    rm CefPrefsOE-1-CaptureTab.zip
fi
if [ -f CefPrefsOE-1-EngineTab.zip ]; then
    rm CefPrefsOE-1-EngineTab.zip
fi
if [ -f CefPrefsOE-1-Options.zip ]; then
    rm CefPrefsOE-1-Options.zip
fi
if [ -d web/C0E0F020-2468-1357-C1C4-313131313131 ]; then
    rm -r web/C0E0F020-2468-1357-C1C4-313131313131
fi
popd >/dev/null