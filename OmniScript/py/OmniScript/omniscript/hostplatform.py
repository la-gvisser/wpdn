"""HostPlatform class.
"""
#Copyright (c) WildPackets, Inc. 2015. All rights reserved.

import os

import omniscript

CAPTURE_MAGIC = 0XABCD
VIGIL_MAGIC = 0X5653
INSIGHT_MAGIC = 0X4953

class HostPlatform(object):
    """Information about the engine's operating system.
    """

    kind = 0
    """The kind of engine. One of the Engine Kind constants."""

    major = 0
    """The major part of the version. The 1 in 1.2."""

    minor = 0
    """The minor part of the version. The 2 in 1.2."""

    os = 0
    """The kind of operating system. One of the
    Operating System constants
    """
    
    auth_strings = ["Default", "ThirdPartyAuth"]
    """List of auth strings. One of the 
    Operating System constants.
    """

    _magic = 0
    """The original magic value."""

    _kind_map = {
        CAPTURE_MAGIC : omniscript.ENGINE_KIND_CAPTURE,
        VIGIL_MAGIC : omniscript.ENGINE_KIND_VIGIL,
        INSIGHT_MAGIC : omniscript.ENGINE_KIND_INSIGHT
        }

    def __init__(self, magic, version, os):
        self._magic = magic
        self.kind = HostPlatform._kind_map[magic]
        # 0x0501 is 1.5
        self.major = version & 0x0FF
        self.minor = (version & 0x0FF00) >> 8
        if os == 3:
            self.os = omniscript.OPERATING_SYSTEM_WINDOWS
        elif os == 100:
            self.os = omniscript.OPERATING_SYSTEM_LINUX
        elif os == 70:
            self.os = omniscript.OPERATING_SYSTEM_MAC
        else:
            self.os = omniscript.OPERATING_SYSTEM_UNKNOWN

    def version(self):
        return '%d.%d' % (self.major, self.minor)

    def get_protocol(self):
        return (self._magic << 16) | (self.minor) << 8 | self.major

    @staticmethod
    def get_auth(platform, auth):
        if not auth:
            if platform:
                if platform.os == omniscript.OPERATING_SYSTEM_WINDOWS:
                    return HostPlatform.auth_strings[0]
                if platform.os == omniscript.OPERATING_SYSTEM_LINUX:
                     return HostPlatform.auth_strings[1]
            return HostPlatform.auth_strings[0]

        lowauth = auth.lower().replace(' ', '-')
        if lowauth == "default":
            return HostPlatform.auth_strings[0]
        if lowauth == "thirdparty" or lowauth == "third-party":
            return HostPlatform.auth_strings[1]
        return HostPlatform.auth_strings[0]
