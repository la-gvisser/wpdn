﻿"""StatsLimit class.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

import os
import sys
import logging
import uuid

import omniscript
import config

import xml.etree.ElementTree as ET

from omnierror import OmniError


ATTRIB_TRUE = '1'
ATTRIB_FALSE = '0'

STATS_LIMIT_MAGIC = 0x6D696C73
STATS_LIMIT_VERSION_1 = 0x0001
STATS_LIMIT_VERSION_2 = 0x0002

limit_types = {
    'none':config.LIMIT_TYPE_NONE,
    'packets':config.LIMIT_TYPE_PACKETS,
    'bytes':config.LIMIT_TYPE_BYTES,
    'buffer':config.LIMIT_TYPE_BUFFER
    }


def _is_attrib_enabled(prop, attrib):
    """Return if an attribute of a node is aa non-zero value."""
    return int(prop.attrib.get(attrib, '0')) != 0


def _to_attrib_boolean(value):
    return ATTRIB_TRUE if value else ATTRIB_FALSE


class StatsLimit(object):
    """The Stats Limit settings of an object."""

    enabled = False
    """Is this Stats Limit enabled."""

    limit = 100000
    """The maximum number of statitic entries to maintain. Default is 100,000"""

    reset_count = 0
    """The number of times this statistic has been reset."""

    severity = config.SEVERITY_SEVERE
    """The severity level of the Notification to post."""

    timeout_count = 0
    """The number of times this statistic has timed-out."""

    option_reset = False
    """Is the Reset the statistics when the limit has been reached
    option enabled.
    """

    option_notify = True
    """Is the Post a Notification when the limit has been reached
    option enabled.
    """

    _name = '_'
    """The XML Tag of this Stats Limit."""

    # XML Tags
    _tag_class_name = "PeekStatsLimitSettings"
    _tag_root_name = "PeekStatsLimitSettings"
    _tag_enabled = "Enabled"
    _tag_limit = "Limit"
    _tag_notify = "Notify"
    _tag_reset = "Reset"
    _tag_severity = "Severity"

    def __init__(self, name, stream=None, limit=100000):
        """Default to OmniPeek setttings"""
        self._name = name if name is not None else StatsLimit._name
        self.enabled = StatsLimit.enabled
        self.limit = limit
        self.reset_count = StatsLimit.reset_count
        self.severity = StatsLimit.severity
        self.timeout_count = StatsLimit.timeout_count
        self.option_notify = StatsLimit.option_notify
        self.options_reset = StatsLimit.option_reset
        if stream is not None:
            self._load(stream)

    def _load(self, stream):
        magic = stream.read_int()
        version = 0
        if magic == STATS_LIMIT_MAGIC:
            version = stream.read_short()
        else:
            raise OmniError("Unsupported version: Stats Limit")
        if version == STATS_LIMIT_VERSION_1:
            self.reset = stream.read_int()
        elif version == STATS_LIMIT_VERSION_2:
            self.reset = stream.read_int()
            self.timeouts = stream.read_long()
        else:
            raise OmniError("Unsupported version: Stats Limit")

    def parse(self, obj):
        """Parse the limit settings from an ETree.SubElement."""
        limit = obj.find(StatsLimit._tag_root_name)
        if limit is not None:
            self.enabled = _is_attrib_enabled(limit, StatsLimit._tag_enabled)
            self.limit = int(limit.attrib.get(StatsLimit._tag_limit, '0'))
            self.severity = int(limit.attrib.get(StatsLimit._tag_severity, '0'))
            self.option_reset = _is_attrib_enabled(limit, StatsLimit._tag_reset)
            self.option_notify = _is_attrib_enabled(limit, StatsLimit._tag_notify)

    def to_xml(self, format=0):
        """Render as XML.

        format 0 (default) is as an object (obj), otherwise with the same
        tag as it's type (_name).
        
        Returns:
            :class:`ETree.SubElement`
        """
        class_name_ids = omniscript.get_class_name_ids()
        if format == 1:
            elem = ET.Element(self._name,
                              {'clsid':str(class_name_ids[StatsLimit._tag_class_name])})
        else:
            elem = ET.Element('obj',
                              {'clsid':str(class_name_ids[StatsLimit._tag_class_name]),
                               'name':self._name})

        ET.SubElement(elem, StatsLimit._tag_root_name,
                      {StatsLimit._tag_enabled:_to_attrib_boolean(self.enabled),
                       StatsLimit._tag_limit:str(self.limit),
                       StatsLimit._tag_severity:str(self.severity),
                       StatsLimit._tag_notify:_to_attrib_boolean(self.option_notify),
                       StatsLimit._tag_reset:_to_attrib_boolean(self.option_reset)})
        return elem

    def __repr__(self):
        return 'StatsLimit: %s' % self._name
