"""PeekTime class.
"""
#Copyright (c) WildPackets, Inc. 2013-2017. All rights reserved.

import os
import sys
import logging

import time

# Convert from Ansi time (seconds) to Peek Time (nanoseconds).
ANSI_TIME_MULTIPLIER = 1000000000L
# The adjustment in seconds (Ansi Time).
# Seconds between 1/1/1601 and 1/1/1970.
ANSI_TIME_ADJUSTMENT = 11644473600L

class PeekTime(object):
    """Peek Time is the number of nanoseconds since midnight January 1, 1601.
    
    PeekTime(), with no arguments is set to the current date and time.
    PeekTime(int or long), uses the integer as the value in nanoseconds.
    PeekTime(string), uses int(string) as the value in nanoseconds.
    PeekTime(PeekTime), copies the value of the other PeekTime.
    """

    value = 0
    """The number of nanoseconds since January 1, 1601."""

    _time_format = '%Y-%m-%dT%H:%M:%S'

    def __init__(self, value=None):
        if value is None:
            self.value = PeekTime.system_time_to_peek_time(time.time())
        elif isinstance(value, long):
            self.value = value if value >= 0 else 0L
        elif isinstance(value, basestring):
            i = long(value)
            self.value = i if i >= 0 else 0L
        elif isinstance(value, float):
            i = PeekTime.system_time_to_peek_time(value)
            self.value = i if i >= 0 else 0L
        else:
            self.value = 0L
            # TODO: Throw a Type Error.

    @classmethod
    def system_time_to_peek_time(cls, value):
        """A Class method that converts a system time to a Peek Time value."""
        return long((value + ANSI_TIME_ADJUSTMENT) * ANSI_TIME_MULTIPLIER)

    @classmethod
    def _decode_other(cls, other):
        """A Class method that converts various types to an integer value."""
        if isinstance(other, PeekTime):
            return other.value
        else:
            return long(other)

    def __repr__(self):
        return 'PeekTime: %d' % self.value

    def __str__(self):
        return str(self.value)

    def __cmp__(self, other):
        return (self.value - PeekTime._decode_other(other))

    # Rich Comparisons - otherwise __cmp__ is called.
    def __lt__(self, other):
        return (self.value < PeekTime._decode_other(other))

    def __le__(self, other):
        return (self.value <= PeekTime._decode_other(other))

    def __eq__(self, other):
        return (self.value == PeekTime._decode_other(other))

    def __ne__(self, other):
        return (self.value != PeekTime._decode_other(other))

    def __gt__(self, other):
        return (self.value > PeekTime._decode_other(other))

    def __ge__(self, other):
        return (self.value >= PeekTime._decode_other(other))

    def __hash__(self):
        return self.value

    def __add__(self, other):
        return PeekTime(self.value + PeekTime._decode_other(other))

    def __sub__(self, other):
        return PeekTime(self.value - PeekTime._decode_other(other))

    def __mul__(self, other):
        return PeekTime(self.value * PeekTime._decode_other(other))

    def from_system_time(self, value):
        """Set the PeekTime from Python System Time, which is the number
        of seconds since January 1, 1970.
        """
        self.value = PeekTime.system_time_to_peek_time(value)

    def time(self):
        """Return the PeekTime as Python System Time, which is the number
        of seconds since January 1, 1970.
        """
        systime = (self.value / ANSI_TIME_MULTIPLIER)
        if systime > ANSI_TIME_ADJUSTMENT:
            systime -= ANSI_TIME_ADJUSTMENT
        return systime

    def ctime(self):
        """Return the PeekTime as Python :class:`ctime <time.ctime>`."""
        return time.ctime(self.time())
