"""Diagnostic Info classes.
"""
#Copyright (c) Savvius, Inc. 2013-2017. All rights reserved.

import os
import sys
import logging
import ctypes
import socket
import struct
import time
import xml.etree.ElementTree as ET

import omniscript
import config

_kinds = None

def parse_count(count):
    """Count is a string with format [0-9]+[KMGT]B
    Kilobytes (KB), Megabytes (MB), Gigabytes (GB) and Terabytes (TB).
    Otherwise int(count[:-2]) is returned.

    Returns:
        The count in bytes.

    Examples:
        count='123MB' returns the integer value 128,974,848.

    """
    base = int(count[:-2])
    unt = count[-2:].upper()
    if unt == 'KB':
        value = base * config.BYTES_IN_KILOBYTE
    elif unt == 'MB':
        value = base * config.BYTES_IN_MEGABYTE
    elif unt == 'GB':
        value = base * config.BYTES_IN_GIGABYTE
    elif unt == 'TB':
        value = base * config.BYTES_IN_TERABYTE
    else:
        value = base
    return value


class Diagnostic(object):
    """Diagnostics classes.
    Creates a Diagnostic specific class of object.
    """

    kind = None
    """The type (kind) of Diagnostic object."""

    content = None
    """The content, a list of strings."""

    @classmethod
    def construct(cls, kind, content):
        global _kinds
        if kind in _kinds.keys():
            cls = _kinds[kind]
            return cls(kind, content)
        return None

    def __init__(self, kind, content):
        self.kind = kind
        self.content = content


class adapter(Diagnostic):
    def __init__(self, kind, content):
        super(adapter, self).__init__(kind, content)

    def __repr__(self):
        return 'adapter: %d lines' % (len(self.content))


class admin(Diagnostic):
    def __init__(self, kind, content):
        super(admin, self).__init__(kind, content)

    def __repr__(self):
        return 'admin: %d lines' % (len(self.content))


class dbadmin(Diagnostic):
    dbs = None
    """Dictionary with key Database name and value boolean if was vacuumed.
    The value is True if the database was vacuumed. False if not.
    """

    def __init__(self, kind, content):
        super(dbadmin, self).__init__(kind, content)
        self.dbs ={}
        self._parse()

    def __repr__(self):
        return 'dbadmin: %d databases' % (len(self.dbs))

    def __str__(self):
        _str = 'dbadmin:'
        if len(self.dbs) == 0:
            _str += ' Empty'
        else:
            for k,v in self.dbs.iteritems():
                _str += '\n%s: %s' % (k, 'OK' if v else 'FAIL')
        return _str

    def _parse(self):
        for line in self.content:
            (label, status) = line.split('Vacuum...')
            label = label.strip()
            self.dbs[label] = 'OK' == status.strip().upper()


class dbhealth(Diagnostic):
    dbs = None
    """Dictionary with key Database name and value boolean integrity.
    The value is True if the database passed the validation test. False if not.
    """

    def __init__(self, kind, content):
        super(dbhealth, self).__init__(kind, content)
        self.dbs = {}
        self._parse()

    def __repr__(self):
        return 'dbhealth: %d databases' % (len(self.dbs))

    def __str__(self):
        _str = 'dbhealth:'
        if len(self.dbs) == 0:
            _str += ' Empty'
        else:
            for k,v in self.dbs.iteritems():
                _str += '\n%s: %s' % (k, 'PASS' if v else 'FAIL')
        return _str

    def _parse(self):
        for line in self.content:
            (label, integrity) = line.split('integrity:')
            label = label.strip()
            self.dbs[label] = 'PASS' == integrity.strip().upper()


class dmesg(Diagnostic):
    def __init__(self, kind, content):
        super(dmesg, self).__init__(kind, content)

    def __repr__(self):
        return 'dmesg: %d lines' % (len(self.content))


class dbsize(Diagnostic):
    dbs = None
    """Dictionary with key Database name and value integer byte count."""

    def __init__(self, kind, content):
        super(dbsize, self).__init__(kind, content)
        self.dbs = {}
        self._parse()

    def __repr__(self):
        return 'dbsize: %d databases' % (len(self.dbs))

    def __str__(self):
        _str = 'dbsize:'
        if len(self.dbs) == 0:
            _str += ' Empty'
        else:
            for k,v in self.dbs.iteritems():
                _str += '\n%s: %d' % (k, v)
        return _str

    def _parse(self):
        for line in self.content:
            (label, count) = line.split('size:')
            label = label.strip()
            bytes = parse_count(count)
            self.dbs[label] = bytes


class drives(Diagnostic):
    def __init__(self, kind, content):
        super(drives, self).__init__(kind, content)

    def __repr__(self):
        return 'drives: %d lines' % (len(self.content))


class log(Diagnostic):
    def __init__(self, kind, content):
        super(log, self).__init__(kind, content)

    def __repr__(self):
        return 'log: %d lines' % (len(self.content))


class mount(Diagnostic):
    def __init__(self, kind, content):
        super(mount, self).__init__(kind, content)

    def __repr__(self):
        return 'mount: %d lines' % (len(self.content))


class processes(Diagnostic):
    def __init__(self, kind, content):
        super(processes, self).__init__(kind, content)

    def __repr__(self):
        return 'processes: %d lines' % (len(self.content))


_kinds = {
    'adapters': adapter,
    'admin': admin,
    'dbadmin': dbadmin,
    'dbhealth': dbhealth,
    'dmesg': dmesg,
    'dbsize': dbsize,
    'drives': drives,
    'log': log,
    'mount': mount,
    'processes': processes
    }


