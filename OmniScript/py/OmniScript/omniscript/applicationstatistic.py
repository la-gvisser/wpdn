"""ApplicationStatistic class.

The :class:`Capture <omniscript.capture.Capture>` function
:func:`get_node_stats() <omniscript.capture.Capture.get_node_stats>`
returns a list of ApplicationStatistic objects.
"""
#Copyright (c) Savvius, Inc. 2015. All rights reserved.

import os
import sys
import logging
import time

from peektime import PeekTime


_application_column_attributes = [
    'bytes',
    'first_sent',
    'last_sent',
    'packets'
    ]

_application_column_labels = [
    'Bytes',
    'First Sent',
    'Last Received',
    'Packets'
    ]


class ApplicationStatistic(object):
    """The ApplicationStatistic class has the attributes of an Application Statistic."""

    node = None
    """The address of the node."""

    bytes = 0
    """The number of bytes processed by the application"""

    first_time = None
    """The timestamp, as
    :class:`PeekTime <omniscript.peektime.PeekTime>`,
    of the first packet sent by the application
    """

    last_time = None
    """The timestamp as
    :class:`PeekTime <omniscript.peektime.PeekTime>`,
    of the last packet sent by the application"""

    packets = 0
    """The number of packets processed by the application"""

    @classmethod
    def report_header(cls, columns, seperator=',', newline=True):
        """Returns a string that contains the two lines that begin a Application
        Statistics Report with the column names specified by columns.

        Args:
            columns (list): List of APPLICATION COLUMN constants.
        """
        top_line = '# This is a statistics file created %s\n' % \
            time.strftime('%A, %B %d, %Y %H:%M:%S', time.localtime())
        header = 'Application'
        for c in columns:
            header += seperator + _application_column_labels[c]
        return top_line + header + '\n' if newline else ''

    def __init__(self, node=None, stream=None):
        self.bytes = ApplicationStatistic.bytes
        self.first_time = ApplicationStatistic.first_time
        self.last_time_ = ApplicationStatistic.last_time
        self.node = node
        self.packets = ApplicationStatistic.packets

        if stream is not None:
            self._load(stream)

    @property
    def address_type(self):
        """Returns the address type of this node"""
        if self.node is not None:
            return self.node.media_type
        return ADDRESS_TYPE_UNDEFINED

    @property
    def name(self):
        """The name (address) of the node. (Read Only)"""
        return str(self.node)

    def __repr__(self):
        return 'ApplicationStatistic: %s' % self.node

    def _load(self, stream):
        self.first_time = PeekTime(stream.read_ulong())
        self.last_time = PeekTime(stream.read_ulong())
        self.packets = stream.read_ulong()
        self.bytes = stream.read_ulong()

    def is_spec(self, media_class, media_type):
        return self.node.is_spec(media_class, media_type)

    def report(self, columns, seperator=',', newline=True):
        """ Returns a string in the form of a OmniPeek Report record.

        Arguments:
            columns (list): List of APPLICATION COLUMN constants.

        Return:
            Returns a string with the Application's name followed by the values
            attributes in order specified by columns.
        """
        line = self.node.report()
        for c in columns:
            line += "%s%s" % (seperator,
                              _repr_functions[c](getattr(self, \
                                  _application_column_attributes[c])))
        return "%s%s" % (line, ('\n' if newline else ''))
