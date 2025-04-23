"""NodeStatistic class.

The :class:`Capture <omniscript.capture.Capture>` function
:func:`get_node_stats() <omniscript.capture.Capture.get_node_stats>`
returns a list of NodeStatistic objects.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

import os
import sys
import logging
import time

import omniscript

from peektime import PeekTime
from readstream import ReadStream


_node_prop_dict = {
    'BroadcastBytes':'broadcast_bytes',
    'BroadcastMulticastBytes':'broadcast_multicast_bytes',
    'BroadcastMulticastPackets':'broadcast_multicast_packets',
    'BroadcastPackets':'broadcast_packets',
    'BytesReceived':'bytes_received',
    'BytesSent':'bytes_sent',
    'FirstTimeReceived':'first_time_received',
    'FirstTimeSent':'first_time_sent',
    'LastTimeReceived':'last_time_received',
    'LastTimeSent':'last_time_sent',
    'MaxPacketSizeReceived':'max_packet_size_received',
    'MaxPacketSizeSent':'max_packet_size_sent',
    'MinPacketSizeReceived':'min_packet_size_received',
    'MinPacketSizeSent':'min_packet_size_sent',
    'MulticastBytes':'multicast_bytes',
    'MulticastPackets':'multicast_packets',
    'Node':'node',
    'PacketsReceived':'packets_received',
    'PacketsSent':'packets_sent',
    'PercentageBytes':'percentage_bytes',
    'PercentagePackets':'percentage_packets',
    'TotalBytes':'total_bytes',
    'TotalPackets':'total_packets',
    'Utilization':'utilization',
    }

_node_column_attributes = [
    'bytes_sent',
    'bytes_received',
    'packets_sent',
    'packets_received',
    'broadcast_packets',
    'broadcast_bytes',
    'multicast_packets',
    'multicast_bytes',
    'min_packet_size_sent',
    'max_packet_size_sent',
    'min_packet_size_received',
    'max_packet_size_received',
    'first_time_sent',
    'last_time_sent',
    'first_time_received',
    'last_time_received'
    ]

_node_column_labels = [
    'Bytes Sent',
    'Bytes Received',
    'Packets Sent',
    'Packets Received',
    'Broadcast Packets',
    'Broadcast Bytes',
    'Multicast Packets',
    'Multicast Bytes',
    'Min. Size Sent',
    'Max. Size Sent',
    'Min. Size Received',
    'Max. Size Received',
    'First Time Sent',
    'Last Time Sent',
    'First Time Received',
    'Last Time Received'
    ]

def repr_float(value):
    return "%1.3f" % (value)

def repr_nonzero(value):
    return str(value) if value > 0 else ''

def repr_string(value):
    return str(value)

def repr_timedate(value):
    if value is None or value.value == 0:
        return ''
    tm = time.localtime(value.time())
    return "%d/%d/%d %d:%d:%d" % (tm.tm_mon, tm.tm_mday, tm.tm_year,
                                  tm.tm_hour, tm.tm_min, tm.tm_sec)

def repr_value(value):
    return str(value) if value is not None else ''

_repr_functions = [
    repr_string,
    repr_string,
    repr_string,
    repr_string,
    repr_value,
    repr_value,
    repr_value,
    repr_value,
    repr_value,
    repr_value,
    repr_value,
    repr_value,
    repr_timedate,
    repr_timedate,
    repr_timedate,
    repr_timedate
    ]


class NodeStatistic(object):
    """The NodeStatistic class has the attributes of a Node Statistic."""

    node = None
    """The address of the node."""

    packets_sent = 0
    """The number of packets sent by the node"""

    bytes_sent = 0
    """The number of bytes sent by the node"""

    min_packet_size_sent = None
    """The smallest packet sent by the node"""

    max_packet_size_sent = None
    """The largest packet sent by the node"""

    first_time_sent = None
    """The timestamp, as 
    :class:`PeekTime <omniscript.peektime.PeekTime>`,
    of the first packet sent by the node
    """

    last_time_sent = None
    """The timestamp, as 
    :class:`PeekTime <omniscript.peektime.PeekTime>`,
    of the last packet sent by the node"""

    broadcast_bytes = None
    """The number of bytes broadcasted by the node"""

    broadcast_packets = None
    """The number of packets broadcasted by the node"""

    multicast_bytes = None
    """The number of bytes multicasted by the node"""

    multicast_packets = None
    """The number of packets  multicasted by the node"""

    packets_received = 0
    """The number of packets received by the node"""

    bytes_received = 0
    """The number of bytes received by the node"""

    min_packet_size_received = None
    """The smallest packet received by the node"""

    max_packet_size_received = None
    """The largest packet received by the node"""

    first_time_received = None
    """The timestamp, as
    :class:`PeekTime <omniscript.peektime.PeekTime>`,
    of the first packet received by the node
    """

    last_time_received = None
    """The timestamp, as
    :class:`PeekTime <omniscript.peektime.PeekTime>`,
    of the last packet received by the node
    """

    #utilization = 0.0

    @classmethod
    def report_header(cls, columns, seperator=',', newline=True):
        """Returns a string that contains the two lines that begin a Node
        Statistics Report with the column names specified by columns.

        Args:
            columns (list): List of NODE COLUMN constants.
        """
        top_line = '# This is a statistics file created %s\n' % \
            time.strftime('%A, %B %d, %Y %H:%M:%S', time.localtime())
        header = 'Node'
        for c in columns:
            header += seperator + _node_column_labels[c]
        return top_line + header + '\n' if newline else ''

    def __init__(self, node=None, stream=None, dct=None):
        self.broadcast_bytes = NodeStatistic.broadcast_bytes
        self.broadcast_packets = NodeStatistic.broadcast_packets
        self.bytes_received = NodeStatistic.bytes_received
        self.bytes_sent = NodeStatistic.bytes_sent
        self.first_time_received = NodeStatistic.first_time_received
        self.first_time_sent = NodeStatistic.first_time_sent
        self.last_time_received = NodeStatistic.last_time_received
        self.last_time_sent = NodeStatistic.last_time_sent
        self.max_packet_size_received = NodeStatistic.max_packet_size_received
        self.max_packet_size_sent = NodeStatistic.max_packet_size_sent
        self.min_packet_size_received = NodeStatistic.min_packet_size_received
        self.min_packet_size_sent = NodeStatistic.min_packet_size_sent
        self.multicast_bytes = NodeStatistic.multicast_bytes
        self.multicast_packets = NodeStatistic.multicast_packets
        self.node = node
        self.packets_received = NodeStatistic.packets_received
        self.packets_sent = NodeStatistic.packets_sent
        #self.utilization = NodeStatistic.utilization

        if stream is not None:
            self._load(stream)
        elif dct is not None:
            for k, v in dct.iteritems():
                a = _node_prop_dict.get(k)
                if a is not None:
                    if hasattr(self, a):
                        if isinstance(getattr(self, a), (int, long)):
                            setattr(self, a, int(v) if v else 0)
                        elif isinstance(getattr(self, a), basestring):
                            setattr(self, a, v if v else '')
                        elif isinstance(getattr(self, a), float):
                            setattr(self, a, float(v) if v else 0.0)
                        elif getattr(self, a) is None:
                            if (a == 'first_time_received') or \
                               (a == 'first_time_sent') or \
                               (a == 'last_time_received') or \
                               (a == 'last_time_sent'):
                                setattr(self, a, PeekTime(v))
        if self.did_send() == 0:
            self.min_packet_size_sent = None
            self.max_packet_size_sent = None
            self.first_time_sent = None
            self.last_time_sent = None
            self.broadcast_bytes = None
            self.broadcast_packets = None
            self.multicast_bytes = None
            self.multicast_packets = None
        if self.did_receive() == 0:
            self.min_packet_size_received = None
            self.max_packet_size_received = None
            self.first_time_received = None
            self.last_time_received = None

    @property
    def address_type(self):
        """Returns the address type of this node"""
        if self.node is not None:
            return self.node.media_type
        return omniscript.ADDRESS_TYPE_UNDEFINED

    @property
    def name(self):
        """The name (address) of the node. (Read Only)"""
        return str(self.node)

    @property
    def broadcast_multicast_bytes(self):
        """Returns the sum of broadcast and multicast bytes sent by this node"""
        return (self.broadcast_bytes + self.multicast_bytes) if \
            self.did_send() else 0

    @property
    def broadcast_multicast_packets(self):
        """Returns the sum of broadcast and multicast packets sent by this node"""
        return (self.broadcast_packets + self.multicast_packets) if \
            self.did_send() else 0

    @property
    def total_bytes(self):
        """Returns the sum of bytes sent and received by this node"""
        return self.bytes_sent + self.bytes_received

    @property
    def total_packets(self):
        """Returns the sum of packets sent and received by this node"""
        return self.packets_sent + self.packets_received

    def __repr__(self):
        return 'NodeStatistic: %s' % self.node

    def _load(self, stream):
        if isinstance(stream, ReadStream):
            self.first_time_sent = PeekTime(stream.read_ulong())
            self.last_time_sent = PeekTime(stream.read_ulong())
            self.first_time_received = PeekTime(stream.read_ulong())
            self.last_time_received = PeekTime(stream.read_ulong())
            self.packets_sent = stream.read_ulong()
            self.bytes_sent = stream.read_ulong()
            self.packets_received = stream.read_ulong()
            self.bytes_received = stream.read_ulong()
            self.broadcast_packets = stream.read_ulong()
            self.broadcast_bytes = stream.read_ulong()
            self.multicast_packets = stream.read_ulong()
            self.multicast_bytes = stream.read_ulong()
            self.min_packet_size_sent = stream.read_ushort()
            self.max_packet_size_sent = stream.read_ushort()
            self.min_packet_size_received = stream.read_ushort()
            self.max_packet_size_received = stream.read_ushort()

    def did_receive(self):
        """Did the node receive any packets."""
        return (self.packets_received + self.bytes_received) > 0

    def did_send(self):
        """Did the node send any packets?"""
        return (self.packets_sent + self.bytes_sent) > 0

    def is_spec(self, media_class, media_type):
        return self.node.is_spec(media_class, media_type)

    def report(self, columns, seperator=',', newline=True):
        """ Returns a string in the form of a OmniPeek Report record.

        Arguments:
            columns (list): List of NODE COLUMN constants.

        Return:
            Returns a string with the Node's address followed by the values
            attributes in order specified by columns.
        """
        line = self.node.report()
        for c in columns:
            line += "%s%s" % (seperator,
                              _repr_functions[c](getattr(self, \
                                  _node_column_attributes[c])))
        return "%s%s" % (line, ('\n' if newline else ''))

    def percentage_bytes(self, total):
        return (self.total_bytes / total) if total > 0 else 0

    def percentage_packets(self, total):
        return (self.total_packets / total) if total > 0 else 0
