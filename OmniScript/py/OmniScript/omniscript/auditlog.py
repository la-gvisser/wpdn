"""AuditLog class.
"""
#Copyright (c) Savvius, Inc. 2017. All rights reserved.

import os
import sys
import xml.etree.ElementTree as ET

import omniscript

from omnierror import OmniError
from peektime import PeekTime


class AuditLogEntry(object):
    """An Entry from an Audit Log.
    """

    index = 0
    """The index of the log entry. The first index is 1."""

    client = None
    """The address of the client associated with the log entry."""

    user = None
    """The user account associated with the log entry."""

    timestamp = None
    """The date and time when the log entry was generated."""

    message = None
    """The message of the log entry."""

    result = 0
    """The result of the operation being logged."""

    #XML Tags
    _tag_index = "messageid"
    _tag_client = "client"
    _tag_user = "user"
    _tag_timestamp = "peektimestamp"
    _tag_message = "message"
    _tag_result = "result"

    def __init__(self, node):
        self.index = AuditLogEntry.index
        self.client = AuditLogEntry.client
        self.user = AuditLogEntry.user
        self.timestamp = AuditLogEntry.timestamp
        self.message = AuditLogEntry.message
        self.result = AuditLogEntry.result
        self._load(node)

    def __repr__(self):
        return 'AuditLogEntry: ' + str(self.message)

    def _load(self, node):
        if node is not None:
            self.index = int(node.findtext(AuditLogEntry._tag_index, '0'))
            self.client = node.findtext(AuditLogEntry._tag_client, '')
            self.user = node.findtext(AuditLogEntry._tag_user, '')
            _timestamp = node.findtext(AuditLogEntry._tag_timestamp, None)
            if _timestamp:
                self.timestamp = PeekTime(_timestamp)
            self.message = node.findtext(AuditLogEntry._tag_message, '')
            self.result = int(node.findtext(AuditLogEntry._tag_result, 0))

    def _key(self):
        return self.index


class AuditLog(object):
    """The AuditLog class.
    """
    
    count = 0
    """The total number of entries in the log."""

    engine = None
    """The OmniEngine object the Audit Log is from."""

    first = None
    """The timestamp of the first log entry."""
    
    last = None
    """The timestamp of the last log entry."""

    entries = None
    """The list of 
    :class:`AuditLogEntry <omniscript.auditlog.AuditLogEntry>`
    Entries.
    """

    #XML Tags
    _tag_first = "first_timestamp"
    _tag_last = "last_timestamp"
    _tag_total = "total"

    def __init__(self, engine, counts, entrys):
        self.engine = engine
        self.count = 0
        self.first = AuditLog.first
        self.last = AuditLog.last
        self.entries = []
        self._load_counts(counts)
        self._load_entrys(entrys)

    def __repr__(self):
        if self.engine:
            return 'AuditLog of ' + str(self.engine)
        else:
            return 'AuditLog'

    def _load_counts(self, counts):
        if counts is not None:
            _first = counts.findtext(AuditLog._tag_first, None)
            if _first:
                self.first = PeekTime(_first)
            _last = counts.findtext(AuditLog._tag_last, None)
            if _last:
                self.last = PeekTime(_last)
            _total = int(counts.findtext(AuditLog._tag_total, 0))
            self.count = max(self.count, _total)

    def _load_entrys(self, entrys):
        if entrys is not None:
            for _entry in entrys:
                self.entries.append(AuditLogEntry(_entry))

    def get(self, first, count):
        """Add entries to the AuditLog starting from first upto count entries.
        The object's entry list will be sorted and without duplicates.
        """
        if not self.engine:
            raise OmniError('AuditLog does not have an OmniEngine.')
        if isinstance(count, (int, long)) and count == 0:
            return
        if count > 0:
            _count = count
            _first = first
        else:
            _count = -count
            _first = (self.count - first) if (first < self.count) else 0
        _al = self.engine.get_audit_log(_first, _count)
        self.update(_al)

    def get_next(self, count=None):
        """Retrieve the next count entries of the AuditLog or the last
        count entries if the count is negative. If count is not specified,
        then the remaining entries are retrieved.
        """
        if not self.engine:
            raise OmniError('AuditLog does not have an OmniEngine.')
        if isinstance(count, (int, long)) and count == 0:
            return
        if count is None:
            _count = self.count - self.entries[-1].index
            _first = self.entries[-1].index
        elif count > 0:
            _count = count
            _first = self.entries[-1].index
        else:
            _count = -count
            _first = (self.count - _count) if (_count < self.count) else 0
        if _count == 0:
            return
        _al = self.engine.get_audit_log(_first, _count)
        self.update(_al)

    def update(self, al):
        """Update this AuditLog with the a nother AuditLog.
        """
        if al:
            if al.engine != self.engine:
                raise OmniError('AuditLog must be from the same OmniEngine.')
            self.count = al.count
            self.last = al.last
            _s = set(self.entries)
            _s.update(al.entries)
            self.entries = sorted(_s, key=AuditLogEntry._key)
