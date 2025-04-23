"""ProtocolStatistic class.

The :class:`Capture <omniscript.capture.Capture>` function
:func:`get_protocol_stats() <omniscript.capture.Capture.get_protocol_stats>`
returns a list of ProtocolStatistic objects.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

import copy
import os
import sys
import ctypes
import struct
import logging
import time

import omniscript
import config

from peektime import PeekTime
from mediaspec import MediaSpec


_protocol_prop_dict = {
    'Bytes':'bytes',
    'FirstTime':'first_time',
    'LastTime':'last_time',
    'Packets':'packets',
    'PercentageBytes':'percentage_bytes',
    'PercentagePackets':'percentage_packets',
    'Protocol':'protocol',
    'Utilization':'utilization',
    }

class ProtocolStatistic(object):
    """The ProtocolStatistic class has the attributes of a Protocol Statistic.
    """

    bytes = 0
    """The number of bytes transfered with this protocol."""

    first_time = None
    """The time and date, as
   :class:`PeekTime <omniscript.peektime.PeekTime>`,
   when this protocol was first seen.
   """

    last_time = None
    """The time and date, as
    :class:`PeekTime <omniscript.peektime.PeekTime>`,
    when this protocol was last seen.
    """

    layers = None

    packets = 0
    """The number of packets transfered with this protocol."""

    percentage_bytes = 0.0
    """The percentage of total bytes transfered that where transfered with 
    this protocol.
    """

    percentage_packets = 0.0
    """The percentage of total packets transfered that where transfered with 
    this protocol.
    """
    
    protocol = ''
    """The name of the protocol."""
    
    utilization = 0.0
    """The percentage of total traffic transfered that where transfered with 
    this protocol.
    """

    @classmethod
    def report_header(cls, seperator=',', newline=True):
        return '# This is a statistics file created %s\n' \
               'Indent%sProtocol%sBytes%sPackets%s' % \
               (time.strftime('%A, %B %d, %Y %H:%M:%S', time.localtime()),
                seperator, seperator, seperator,
                '\n' if newline else '')

    def __init__(self, protocol=None, layers=None, stream=None, dct=None):
        self.bytes = ProtocolStatistic.bytes
        self.first_time = ProtocolStatistic.first_time
        self.last_time = ProtocolStatistic.last_time
        self.layers = layers
        self.packets = ProtocolStatistic.packets
        self.percentage_bytes = ProtocolStatistic.percentage_bytes
        self.percentage_packets = ProtocolStatistic.percentage_packets
        self.protocol = ProtocolStatistic.protocol
        self.utilization = ProtocolStatistic.utilization

        if isinstance(protocol, basestring):
            self.protocol = protocol

        if isinstance(protocol, (int, long)):
            id_protocol_short_names = omniscript.get_id_protocol_short_names()
            short_name = id_protocol_short_names.get(protocol)
            self.protocol = short_name if short_name else str(protocol)
        elif isinstance(layers, list):
            self.protocol = str(layers[-1])
        elif isinstance(layers, MediaSpec):
            if layers.is_spec(config.MEDIA_CLASS_PROTOCOL, config.MEDIA_SPEC_PROTOSPEC_HIERARCHY):
                id_protocol_short_names = omniscript.get_id_protocol_short_names()
                indexes = struct.unpack("<H", layers.value[:2])
                short_name = id_protocol_short_names.get(indexes[0])
                self.protocol = short_name if short_name else str(indexes[0])

        if stream is not None:
            self._load(stream)
        elif dct is not None:
            for k, v in dct.iteritems():
                a = _protocol_prop_dict.get(k)
                if a is not None:
                    if hasattr(self, a):
                        if isinstance(getattr(self, a), (int, long)):
                            setattr(self, a, int(v) if v else 0)
                        elif isinstance(getattr(self, a), basestring):
                            setattr(self, a, v if v else 0)
                        elif isinstance(getattr(self, a), float):
                            setattr(self, a, float(v) if v else 0.0)
                        elif getattr(self, a) is None:
                            setattr(self, a, PeekTime(v))

    @property
    def name(self):
        """The name of the protocol. (Read Only)"""
        return self.protocol

    def __repr__(self):
        return 'ProtocolStatistic: %s' % self.protocol

    def _load(self, stream):
        self.first_time = PeekTime(stream.read_ulong())
        self.last_time = PeekTime(stream.read_ulong())
        self.packets = stream.read_ulong()
        self.bytes = stream.read_ulong()

    def report(self, seperator=',', newline=True):
        """ Returns a string in the form of a OmniPeek Report record:
        Indent, Protocol, Bytes, Packets
        """
        return "%s%s%s%s%s%s%s%s" % ('1', seperator,
                                     self.name, seperator,
                                     self.bytes, seperator,
                                     self.packets, ('\n' if newline else ''))

def flatten(protocols, sort=False):
    flat = []
    for p in protocols:
        if p.packets + p.bytes > 0:
            try:
                res = next(f for f in flat if f.name == p.name)
                res.packets += p.packets
                res.bytes += p.bytes
            except StopIteration:
                flat.append(copy.copy(p))
    if sort:
        flat.sort(key=lambda f: f.name)
    return flat
