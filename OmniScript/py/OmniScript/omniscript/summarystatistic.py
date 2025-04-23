"""SummaryStatistic class.

The :class:`Capture <omniscript.capture.Capture>` function
:func:`get_summary_stats() <omniscript.capture.Capture.get_summary_stats>`
returns a list of SummaryStatistic objects.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

import os
import sys
import ctypes
import logging
import struct
import time

import omniscript

from omniid import OmniId
from peektime import PeekTime

_type_labels = [
    'None',
    'Date',
    'Time',
    'Duration',
    'Packets',
    'Bytes',
    'Packets/Bytes',
    'Integer',
    'Float'
    ]

def eval_zero(value):
    return 0

def eval_date(value):
    return PeekTime(struct.unpack_from("=Q", value)[0])

def eval_time(value):
    return PeekTime(struct.unpack_from("=Q", value)[0])

def eval_duration(value):
    return struct.unpack_from("=Q", value)[0]

def eval_packets(value):
    return struct.unpack_from("=Q", value)[0]

def eval_bytes(value):
    return struct.unpack_from("=Q", value)[0]

def eval_pair(value):
    return [struct.unpack_from("=Q", value)[0],
            struct.unpack_from("=Q", value, 8)[0]]

def eval_int(value):
    return struct.unpack_from("=Q", value)[0]

def eval_double(value):
    return struct.unpack_from("=d", value)[0]

eval_map = {
    0 : eval_zero,
    1 : eval_date,
    2 : eval_time,
    3 : eval_duration,
    4 : eval_packets,
    5 : eval_bytes,
    6 : eval_pair,
    7 : eval_int,
    8 : eval_double
    }


def _name_lookup(id, names):
    name = None
    if id is not None:
        if names is not None:
            name = names.get(id, None)
        if name is not None:
            return name
        id_stat_names = omniscript.get_id_stat_names()
        name = id_stat_names.get(id, None)
        if name is None:
            id_class_names = omniscript.get_id_class_names()
            name = id_class_names.get(id, None)
    if name is None:
        name = str(id)
    return name


class SummaryStatistic(object):
    """The SummaryStatistic class has the attributes of a Summary Statistic.
    """

    id = None
    """The GUID of the summary statistic."""

    name = ''
    """The name of the summary statistic."""

    flags = 0
    """The flags of the summary statistic."""

    parent = ''
    """The name of the parent summary statistic."""

    parent_id = None
    """The GUID of the parent summary statistic."""

    value = None
    """The value of the summary statistic."""

    value_type = 0
    """The type of summary statitic."""

    @classmethod
    def report_header(cls, seperator=',', newline=True):
        return '# This is a statistics file created %s\n' \
               'Group%sLabel%sPackets%sBytes%sValue%sSnapshot 1..n%s' % \
               (time.strftime('%A, %B %d, %Y %H:%M:%S', time.localtime()),
                seperator, seperator, seperator, seperator, seperator,
                '\n' if newline else '')

    def __init__(self, stream=None, dct=None, names=None):
        self.id = SummaryStatistic.id
        self.name = SummaryStatistic.name
        self.flags = SummaryStatistic.flags
        self.parent = SummaryStatistic.parent
        self.parent_id = SummaryStatistic.parent_id
        self.value = SummaryStatistic.value
        self.value_type = SummaryStatistic.value_type

        if stream is not None:
            self._load(stream, names)
        elif dct is not None:
            for k, v in dct.iteritems():
                if k == 'parent':
                    k = 'parent_id'
                if hasattr(self, k):
                    if isinstance(getattr(self, k), (int, long)):
                        setattr(self, k, int(v) if v else 0)
                    elif isinstance(getattr(self, k), basestring):
                        setattr(self, k, v if v else '')
                    elif getattr(self, k) is None:
                        setattr(self, k, omniscript.OmniId(v))
            self.name = _name_lookup(self.id, names)
            self.parent = _name_lookup(self.parent_id, names)
            if self.value_type == omniscript.SUMMARY_TYPE_NULL:
                self.value = 0
            elif self.value_type == omniscript.SUMMARY_TYPE_DATE:
                self.value = PeekTime(dct['date']) if \
                'date' in dct else PeekTime(0)
            elif self.value_type == omniscript.SUMMARY_TYPE_TIME:
                self.value = PeekTime(dct['time']) if \
                    'time' in dct else PeekTime(0)
            elif self.value_type == omniscript.SUMMARY_TYPE_DURATION:
                self.value = int(dct['duration']) if \
                    'duration' in dct else 0
            elif self.value_type == omniscript.SUMMARY_TYPE_PACKETS:
                self.value = int(dct['packets']) if \
                    'packets' in dct else 0
            elif self.value_type == omniscript.SUMMARY_TYPE_BYTES:
                self.value = int(dct['bytes']) if 'bytes' in dct else 0
            elif self.value_type == omniscript.SUMMARY_TYPE_PAIR:
                self.value = (int(dct['packets']), int(dct['bytes'])) if \
                    'packets' in dct and 'bytes' in dct else (0, 0)
            elif self.value_type == omniscript.SUMMARY_TYPE_INT:
                self.value = int(dct['int']) if 'int' in dct else 0
            elif self.value_type == omniscript.SUMMARY_TYPE_DOUBLE:
                self.value = float(dct['double']) if 'double' in dct else 0.0

    def __repr__(self):
        return 'SummaryStatistic: %s = %s' % (self.name, self.value)

    def _load(self, stream, names):
        self.id = stream.read_guid()
        self.parent_id = stream.read_guid()
        self.name = stream.read_unicode()
        self.value_type = stream.read_uint()
        self.flags = stream.read_uint()
        bytes = stream.read(16)
        self.parent = _name_lookup(self.parent_id, names)
        self.value = eval_map[self.value_type](bytes)
        if len(self.name) == 0:
            self.name = _name_lookup(self.id, names)
        else:
            name = omniscript.string_from_unicode(self.name)
            if name is not None:
                self.name = name

    def packets_bytes_value(self):
        """Returns a tuple with the 'Packets', 'Bytes' and 'Value' of the
        statistic. Any of the three values may be None.
        """
        packets = self.value if self.value_type == omniscript.SUMMARY_TYPE_PACKETS else ''
        bytes = self.value if self.value_type == omniscript.SUMMARY_TYPE_BYTES else ''
        value = ''
        if self.value_type == omniscript.SUMMARY_TYPE_DATE:
            if self.value.value != 0 and \
               self.value.value != 0xFFFFFFFFFFFFFFFFL:
                tm = time.localtime(self.value.time())
                value = "%d/%d/%d" % (tm.tm_mon, tm.tm_mday, tm.tm_year)
        elif self.value_type == omniscript.SUMMARY_TYPE_TIME:
            if self.value.value != 0 and \
               self.value.value != 0xFFFFFFFFFFFFFFFFL:
                tm = time.localtime(self.value.time())
                value = "%d:%d:%d" % (tm.tm_hour, tm.tm_min, tm.tm_sec)
        elif self.value_type == omniscript.SUMMARY_TYPE_DURATION:
            value = "%1.6f" % (self.value / 1000000000.0)
        elif self.value_type == omniscript.SUMMARY_TYPE_PAIR:
            packets = self.value[0]
            bytes = self.value[1]
            value = ''
        elif self.value_type == omniscript.SUMMARY_TYPE_INT:
            value = str(self.value)
        elif self.value_type == omniscript.SUMMARY_TYPE_DOUBLE:
            value = "%1.3f" % (self.value)
        return (packets, bytes, value)
    
    def report(self, seperator=',', newline=True):
        """ Returns a string in the form of a OmniPeek Report record:
        Group, Label, Packets, Bytes, Value
        """
        packets, bytes, value = self.packets_bytes_value()
        return "%s%s%s%s%s%s%s%s%s%s" % (self.parent, seperator,
                                       self.name, seperator,
                                       packets, seperator,
                                       bytes, seperator,
                                       value, ('\n' if newline else ''))

    def type_label(self):
        return _type_labels[self.value_type] \
            if self.value_type < len(_type_labels) else ''
