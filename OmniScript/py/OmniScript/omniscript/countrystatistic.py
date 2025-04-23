"""CountryStatistic class.

The :class:`Capture <omniscript.capture.Capture>` function
:func:`get_call_stats() <omniscript.capture.Capture.get_call_stats>`
returns a CountryStatistic objects.
"""
#Copyright (c) WildPackets, Inc. 2015. All rights reserved.

import os
import sys
import logging
import time

import omniscript

from peektime import PeekTime


class CountryStatistic(object):
    """The CountryStatistic class has the statistic of the country the
    packet originated or was received in.
    """

    bytes_from = 0
    """The number of bytes received from the Country."""

    bytes_to = 0
    """The number of bytes sent to the Country."""

    country_code = ""
    """The 2 letter ISO Country Code string, Unknown, Private or Multicast."""

    country_name = ""
    """The name of the country, Unknown, Private or Multicast."""

    first_time_from = None
    """The first time, as
    :class:`PeekTime <omniscript.peektime.PeekTime>`,
    a packet was received from the Country."""

    first_time_to = None
    """The first time, as
    :class:`PeekTime <omniscript.peektime.PeekTime>`,
    a packet was sent to the Country.
    """

    last_time_from = None
    """The last time, as
    :class:`PeekTime <omniscript.peektime.PeekTime>`,
    a packet was received from the Country."""

    last_time_to = None
    """The last time, as
    :class:`PeekTime <omniscript.peektime.PeekTime>`,
    a packet was sent to the Country."""

    packets_from = 0
    """The number of packets received from the Country."""

    packets_to = 0
    """The number of packets sent to the Country."""

    def __init__(self, stream):
        self.bytes_from = CountryStatistic.bytes_from
        self.bytes_to = CountryStatistic.bytes_to
        self.country_code = CountryStatistic.country_code
        self.country_name = CountryStatistic.country_name
        self.first_time_from = CountryStatistic.first_time_from
        self.first_time_to = CountryStatistic.first_time_to
        self.last_time_from = CountryStatistic.last_time_from
        self.last_time_to = CountryStatistic.last_time_to
        self.packets_from = CountryStatistic.packets_from
        self.packets_to = CountryStatistic.packets_to
        if stream is not None:
            self._load(stream)

    def _load(self, stream):
        code = stream.read_string(2)
        if ord(code[0]) == 0:
            if ord(code[1]) == 0:
                code = 'Unknown'
            elif ord(code[1]) == 1:
                code = 'Private'
            elif ord(code[1]) == 2:
                code = 'Multicast'
        self.country_code = code
        stream.seek(6, os.SEEK_CUR) # padding
        self.first_time_from = PeekTime(stream.read_ulong())
        self.last_time_from = PeekTime(stream.read_ulong())
        self.packets_from = stream.read_ulong()
        self.bytes_from = stream.read_ulong()
        self.first_time_to = PeekTime(stream.read_ulong())
        self.last_time_to = PeekTime(stream.read_ulong())
        self.packets_to = stream.read_ulong()
        self.bytes_to = stream.read_ulong()
        co_names = omniscript.get_country_code_names()
        if code in co_names:
            self.country_name = co_names[code]
        
    def get(self):
        return self
