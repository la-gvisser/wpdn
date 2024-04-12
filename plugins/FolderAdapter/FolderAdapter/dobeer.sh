#!/bin/bash

./clean.sh
./build.sh --clean --release
apt remove folderadapteroe -y
apt install ./FolderAdapterOE-1.12.0.0-22.04.deb
if compgen -G "/tmp/FolderAdapter*" > /dev/null; then
    echo "Delete all /tmp/FolderAdapter* files"
    rm /tmp/FolderAdapter*
fi
if [ -f "/usr/lib/omni/plugins/FolderAdapterOE.xml" ]; then
    # echo "Don't Delete /usr/lib/omni/plugins/FolderAdapterOE.xml"
    echo "Deleted /usr/lib/omni/plugins/FolderAdapterOE.xml"
    rm -f /usr/lib/omni/plugins/FolderAdapterOE.xml
fi
chown -R gary:gary *
chmod -R 775 *
