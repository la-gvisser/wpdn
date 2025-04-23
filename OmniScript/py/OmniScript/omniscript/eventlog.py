"""EventLog class.
"""
#Copyright (c) Savvius, Inc. 2017. All rights reserved.

import os
import sys
import xml.etree.ElementTree as ET

import omniscript

from omniid import OmniId
from omnierror import OmniError
from peektime import PeekTime


class EventLogEntry(object):
    """An Entry from an Audit Log.
    """

    capture_id = None
    """The GUID of the Capture this log entry. Or None or Null GUID"""

    index = 0
    """The index of the log entry. The first index is 1."""

    message = None
    """The message of the log entry."""

    severity = 0
    """The severity of the log entry: 0-Informationa, 1-Minor, 2-Major, 3-Severe."""

    source_id = None
    """The GUID of the source of the log entry."""

    source_key = 0
    """The GUID of the source of the log entry."""

    timestamp = None
    """The date and time when the log entry was generated."""

    #XML Tags
    _tag_capture_id = "contextid"
    _tag_index = "messageid"
    _tag_message = "shortmessage"
    _tag_severity = "severity"
    _tag_source_id = "sourceid"
    _tag_source_key = "sourcekey"
    _tag_timestamp = "peektimestamp"

    def __init__(self, node):
        self.capture_id = EventLogEntry.capture_id
        self.index = EventLogEntry.index
        self.message = EventLogEntry.message
        self.severity = EventLogEntry.severity
        self.source_id = EventLogEntry.source_id
        self.source_key = EventLogEntry.source_key
        self.timestamp = EventLogEntry.timestamp
        self._load(node)

    def __repr__(self):
        return 'AuditLogEntry: ' + str(self.message)

    def _load(self, node):
        if node is not None:
            _c_id = OmniId(node.findtext(EventLogEntry._tag_capture_id, None))
            if _c_id and _c_id.id != OmniId.null_id:
                self.capture_id = _c_id
            self.index = int(node.findtext(EventLogEntry._tag_index, '0'))
            self.message = node.findtext(EventLogEntry._tag_message, '')
            self.severity = int(node.findtext(EventLogEntry._tag_severity, '0'))
            _s_id = OmniId(node.findtext(EventLogEntry._tag_source_id, None))
            if _s_id and _s_id.id != OmniId.null_id:
                self.source_id = _s_id
            self.source_key = int(node.findtext(EventLogEntry._tag_source_key, '0'))
            _timestamp = node.findtext(EventLogEntry._tag_timestamp, None)
            if _timestamp:
                self.timestamp = PeekTime(_timestamp)

    def _key(self):
        return self.index


class EventLog(object):
    """The EventLog class.
    """
    
    count = 0
    """The total number of entries in the log."""

    engine = None
    """The OmniEngine object the Audit Log is from."""

    first = None
    """The timestamp of the first log entry."""
    
    informational = 0
    """The number of informational (0) events."""

    last = None
    """The timestamp of the last log entry."""

    major = 0
    """The number of major (2) events."""

    minor = 0
    """The number of minor (1) events."""

    severe = 0
    """The number of severe (3) events."""

    entries = None
    """The list of 
    :class:`EventLogEntry <omniscript.eventlog.EventLogEntry>`
    Entries.
    """

    #XML Tags
    _tag_count = "total"
    _tag_first = "first_timestamp"
    _tag_informational = "informational"
    _tag_last = "last_timestamp"
    _tag_major = "major"
    _tag_minor = "minor"
    _tag_severe = "severe"

    def __init__(self, engine, counts, entrys):
        self.count = EventLog.count
        self.engine = engine
        self.first = EventLog.first
        self.informational = EventLog.informational
        self.last = EventLog.last
        self.major = EventLog.major
        self.minor = EventLog.minor
        self.severe = EventLog.severe
        self.entries = []
        self._load_counts(counts)
        self._load_entrys(entrys)

    def __repr__(self):
        if self.engine:
            return 'EventLog of ' + str(self.engine)
        else:
            return 'EventLog'

    def _load_counts(self, counts):
        if counts is not None:
            _count = int(counts.findtext(EventLog._tag_count, 0))
            self.count = max(self.count, _count)
            _informational = int(counts.findtext(EventLog._tag_informational, None))
            self.informational = max(self.informational, _informational)
            _major = int(counts.findtext(EventLog._tag_major, None))
            self.major = max(self.major, _major)
            _minor = int(counts.findtext(EventLog._tag_minor, None))
            self.minor = max(self.minor, _minor)
            _severe = int(counts.findtext(EventLog._tag_severe, None))
            self.severe = max(self.severe, _severe)

            _first = counts.findtext(EventLog._tag_first, None)
            if _first:
                self.first = PeekTime(_first)
            _last = counts.findtext(EventLog._tag_last, None)
            if _last:
                self.last = PeekTime(_last)

    def _load_entrys(self, entrys):
        if entrys is not None:
            for _entry in entrys:
                self.entries.append(EventLogEntry(_entry))

    def get(self, first, count):
        """Add entries to the EventLog starting from first upto count entries.
        The object's entry list will be sorted and without duplicates.
        """
        if not self.engine:
            raise OmniError('EventLog does not have an OmniEngine.')
        if isinstance(count, (int, long)) and count == 0:
            return
        if count > 0:
            _count = count
            _first = first
        else:
            _count = -count
            _first = (self.count - first) if (first < self.count) else 0
        _el = self.engine.get_event_log(_first, _count)
        self.update(_el)

    def get_next(self, count=None):
        """Retrieve the next count entries of the EventLog or the last
        count entries if count is negative. If count is not specified, 
        then the remaining entries are retrieved.
        """
        if not self.engine:
            raise OmniError('EventLog does not have an OmniEngine.')
        if isinstance(count, (int, long)) and count == 0:
            return
        if count > 0:
            _count = count
            _first = self.entries[-1].index
        else:
            _count = -count
            _first = (self.count - _count) if (_count < self.count) else 0
        _el = self.engine.get_event_log(_first, _count)
        self.update(_el)

    def update(self, el):
        """Update this EventLog with the a nother EventLog.
        """
        if el:
            if el.engine != self.engine:
                raise OmniError('EventLog must be from the same OmniEngine.')
            self.count = el.count
            self.last = el.last
            _s = set(self.entries)
            _s.update(el.entries)
            self.entries = sorted(_s, key=EventLogEntry._key)

