"""MediaInfo class.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

import os
import sys
import ctypes
import struct
import xml.etree.ElementTree as ET

import omniscript
import config
import omniaddress

from readstream import ReadStream

_media_spec_size = [
    0,  #  0 MEDIA_SPEC_NULL
    2,  #  1 MEDIA_SPEC_ETHERNET_PROTOCOL
    1,  #  2 MEDIA_SPEC_LSAP
    5,  #  3 MEDIA_SPEC_SNAP
    1,  #  4 MEDIA_SPEC_LAP
    1,  #  5 MEDIA_SPEC_DDP
    1,  #  6 MEDIA_SPEC_MAC_CONTROL
    64, #  7 MEDIA_SPEC_PROTOSPEC_HIERARCHY
    8,  #  8 MEDIA_SPEC_APPLICATION_ID
    4,  #  9 MEDIA_SPEC_PROTOSPEC
    6,  # 10 MEDIA_SPEC_ETHERNET_ADDRESS
    6,  # 11 MEDIA_SPEC_TOKENRING_ADDRESS
    6,  # 12 MEDIA_SPEC_LAP_ADDRESS
    6,  # 13 MEDIA_SPEC_WIRELESS_ADDRESS
    0,  # 14 UNUSED
    0,  # 15 UNUSED
    0,  # 16 UNUSED
    0,  # 17 UNUSED
    0,  # 18 UNUSED
    0,  # 19 UNUSED
    3,  # 20 MEDIA_SPEC_APPLETALK_ADDRESS
    4,  # 21 MEDIA_SPEC_IP_ADDRESS
    2,  # 22 MEDIA_SPEC_DECNET_ADDRESS
    0,  # 23 MEDIA_SPEC_OTHER_ADDRESS
    16, # 24 MEDIA_SPEC_IPV6_ADDRESS
    10, # 25 MEDIA_SPEC_IPX_ADDRESS
    0,  # 26 UNUSED
    0,  # 27 UNUSED
    0,  # 28 UNUSED
    0,  # 29 UNUSED
    0,  # 30 UNUSED
    0,  # 31 UNUSED
    1,  # 32 MEDIA_SPEC_ERROR
    1,  # 33 MEDIA_SPEC_AT_PORT
    2,  # 34 MEDIA_SPEC_IP_PORT
    2,  # 35 MEDIA_SPEC_NW_PORT
    4,  # 36 MEDIA_SPEC_TCP_PORTPAIR
    2,  # 37 MEDIA_SPEC_WAN_PPP_PROTOCOL
    2,  # 38 MEDIA_SPEC_WAN_FRAMERELAY_PROTOCOL
    0,  # 39 MEDIA_SPEC_WAN_X25_PROTOCOL
    0,  # 40 MEDIA_SPEC_WAN_X25E_PROTOCOL
    0,  # 41 MEDIA_SPEC_WAN_U200_PROTOCOL
    0,  # 42 MEDIA_SPEC_WAN_IPARS_PROTOCOL
    2,  # 43 MEDIA_SPEC_WAN_DLCI_ADDRESS
    0   # 44 MEDIA_SPEC_WAN_Q931_PROTOCOL
    ]


def repr_other(value):
    return str(value).encode(encoding='hex_codec')


def repr_ipv4(value):
    return "%d.%d.%d.%d" % struct.unpack("<BBBB", value[:4])


def repr_ipv6(value, compress=False):
    return omniaddress.format_ipv6(struct.unpack(">HHHHHHHH", value[:16]), compress)


def repr_ethernet(value):
    return "%02X:%02X:%02X:%02X:%02X:%02X" % \
        struct.unpack("<BBBBBB", value[:6])


def repr_portpair(value):
    return "%d-%d" % struct.unpack("<HH", value[:4])


def repr_protospec(value):
    id_protocol_short_names = omniscript.get_id_protocol_short_names()
    short_name = id_protocol_short_names.get(value)
    return short_name if short_name else str(value)


def repr_pspec_hier(value):
    ids = struct.unpack("<HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH", value[:64])
    name = ""
    for id in ids:
        if id > 0:
            name += str(id)
        else:
            break
    return name


def repr_app_id(value):
    name = "%c%c%c%c%c%c%c%c" % struct.unpack("<BBBBBBBB", value[:8])
    return name.rstrip('\0')


def report_ipv6(value):
    words = struct.unpack(">HHHHHHHH", value[:16])
    long = 0
    for wv in words:
        long = (long << 16) + wv
    hex_str = '%032x' % long
    hextets = ['%x' % int(hex_str[x:x + 4], 16) for x in range(0, 32, 4)]
    hextets = omniaddress._compress_hextets(hextets)
    return hextets


_repr_table = [
    repr_other,         #  0 MEDIA_SPEC_NULL
    repr_other,         #  1 MEDIA_SPEC_ETHERNET_PROTOCOL
    repr_other,         #  2 MEDIA_SPEC_LSAP
    repr_other,         #  3 MEDIA_SPEC_SNAP
    repr_other,         #  4 MEDIA_SPEC_LAP
    repr_other,         #  5 MEDIA_SPEC_DDP
    repr_other,         #  6 MEDIA_SPEC_MAC_CONTROL
    repr_pspec_hier,    #  7 MEDIA_SPEC_PROTOSPEC_HIERARCHY
    repr_app_id,        #  8 MEDIA_SPEC_APPLICATION_ID
    repr_protospec,     #  9 MEDIA_SPEC_PROTOSPEC
    repr_ethernet,      # 10 MEDIA_SPEC_ETHERNET_ADDRESS
    repr_ethernet,      # 11 MEDIA_SPEC_TOKENRING_ADDRESS
    repr_ethernet,      # 12 MEDIA_SPEC_LAP_ADDRESS
    repr_ethernet,      # 13 MEDIA_SPEC_WIRELESS_ADDRESS
    repr_other,         # 14 UNUSED
    repr_other,         # 15 UNUSED
    repr_other,         # 16 UNUSED
    repr_other,         # 17 UNUSED
    repr_other,         # 18 UNUSED
    repr_other,         # 19 UNUSED
    repr_other,         # 20 MEDIA_SPEC_APPLETALK_ADDRESS
    repr_ipv4,          # 21 MEDIA_SPEC_IP_ADDRESS
    repr_other,         # 22 MEDIA_SPEC_DECNET_ADDRESS
    repr_other,         # 23 MEDIA_SPEC_OTHER_ADDRESS
    repr_ipv6,          # 24 MEDIA_SPEC_IPV6_ADDRESS
    repr_other,         # 25 MEDIA_SPEC_IPX_ADDRESS
    repr_other,         # 26 UNUSED
    repr_other,         # 27 UNUSED
    repr_other,         # 28 UNUSED
    repr_other,         # 29 UNUSED
    repr_other,         # 30 UNUSED
    repr_other,         # 31 UNUSED
    repr_other,         # 32 MEDIA_SPEC_ERROR
    repr_other,         # 33 MEDIA_SPEC_AT_PORT
    repr_other,         # 34 MEDIA_SPEC_IP_PORT
    repr_other,         # 35 MEDIA_SPEC_NW_PORT
    repr_portpair,      # 36 MEDIA_SPEC_TCP_PORTPAIR
    repr_other,         # 37 MEDIA_SPEC_WAN_PPP_PROTOCOL
    repr_other,         # 38 MEDIA_SPEC_WAN_FRAMERELAY_PROTOCOL
    repr_other,         # 39 MEDIA_SPEC_WAN_X25_PROTOCOL
    repr_other,         # 40 MEDIA_SPEC_WAN_X25E_PROTOCOL
    repr_other,         # 41 MEDIA_SPEC_WAN_U200_PROTOCOL
    repr_other,         # 42 MEDIA_SPEC_WAN_IPARS_PROTOCOL
    repr_other,         # 43 MEDIA_SPEC_WAN_DLCI_ADDRESS
    repr_other          # 44 MEDIA_SPEC_WAN_Q931_PROTOCOL
    ]


class MediaSpec(object):
    """The MediaSpec class.
    """

    media_class = 0
    """The specification class, one of MEDIA_CLASS constants"""

    media_type = 0
    """The specification type, one of MEDIA_SPEC constants"""

    value = None
    """The value of the media specification."""

    def __init__(self, cls=None, typ=None, vl=None, data=None):
        self.media_class = cls if cls is not None else MediaSpec.media_class
        self.media_type = typ if typ is not None else MediaSpec.media_type
        self.value = vl
        if data is not None:
            self._load(data)

    def _load(self, data):
        if isinstance(data, basestring):
            stream = ReadStream(data)
            self.media_class = stream.read_byte()
            self.medai_type = stream.read_byte()
            stream.seek(2, os.SEEK_CUR)
            stream.read_uint() # mask
            len = _media_spec_size[self.media_type]
            if len == 4:
                self.value = int(stream.read_uint())
            elif len == 2:
                self.value = int(stream.read_ushort())
            elif len == 1:
                self.value = int(stream.read_byte())
            else:
                self.value = stream.read(len)

    def __repr__(self):
        return _repr_table[self.media_type](self.value) \
            if self.media_type <= config.MEDIA_SPEC_WAN_Q931_PROTOCOL \
            else ''

    def is_spec(self, media_class, media_type):
        """Returns True if this MediaSpec is of class and type specified."""
        return self.media_class == media_class and \
            self.media_type == media_type

    def report(self):
        return self.__repr__() \
            if self.media_type != config.MEDIA_SPEC_IPV6_ADDRESS \
            else report_ipv6(self.value)
