"""DecodedPacket class.
"""
#Copyright (c) Savvius, Inc. 2016. All rights reserved.

import os
import sys
import logging

import omniscript
import config

from readstream import ReadStream
from packet import Packet


class EthernetHeader(object):
    """The header of an Ethernet Type 2 paceket.
    """

    destination = None
    """The destination address as an array of bytes."""

    source = None
    """The source address as an array of bytes."""

    protocol_type = 0
    """The Protocol Type from the Ethernet header."""

    _length = 14
    """Number of bytes needed from the stream."""

    def __init__(self):
        self.ethernet_destination = None
        self.ethernet_source = None
        self.protocol_type = 0

    def load(self, stream):
        if not stream or stream.remaining() < EthernetHeader._length:
            return
        self.ethernet_destination = stream.read(6)
        self.ethernet_source = stream.read(6)
        self.protocol_type = stream.read_ushort()


class IPHeader(object):
    """The IP header of a paceket.
    """

    destination = 0
    """The destination address as an unsigned integer."""

    total_length = 0
    """The length of the packet data plus the IP Header."""

    protocol = 0
    """The protocol of the packet."""

    source = 0
    """The source address as an unsigned integer."""

    _length = 20
    """Number of bytes needed from the stream."""

    def __init__(self, stream):
        self.destination = 0
        self.protocol = 0
        self.source = 0
        self.total_length = 0
        self._load(stream)

    def _load(self, stream):
        if not stream or stream.remaining() < IPHeader._length:
            return
        version_hdr_len = stream.read_byte()
        diff_serv = stream.read_byte()
        self.total_length = stream.read_ushort()
        id = stream.read_ushort()
        fragmentation = stream.read_ushort()
        time_to_live = stream.read_byte()
        self.protocol = stream.read_byte()
        chk_sum = stream.read_ushort()
        self.source = stream.read_uint()
        self.destination = stream.read_uint()


class TCPHeader(object):
    """The TCP header of a paceket.
    """

    ack_number = 0
    """The ack number of the packet."""

    destination = 0
    """The destination address as an unsigned short integer."""

    length = 0
    """The length of the packet data plus the IP Header."""

    offset_flags = 0
    """The TCP offset of the packet."""

    sequence_number = 0
    """The sequence number of the packet."""

    source = 0
    """The source address as an unsigned short integer."""

    window = 0
    """The TCP window of the packet."""

    _length = 14
    """Number of bytes needed from the stream."""

    def __init__(self, stream):
        self.ack_number = 0
        self.destination = 0
        self.length = 0
        self.offset_flags = 0
        self.source = 0
        self._load(stream)

    def _load(self, stream):
        if not stream or stream.remaining() < TCPHeader._length:
            return
        self.source = stream.read_ushort()
        self.destination = stream.read_ushort()
        self.sequence_number = stream.read_ushort()
        self.ack_number = stream.read_ushort()
        self.offset_flags = stream.read_byte()
        self.window = stream.read_ushort()
        chk_sum = stream.read_ushort()


class UDPHeader(object):
    """The UDP header of a paceket.
    """

    destination = 0
    """The destination address as an unsigned short integer."""

    length = 0
    """The length of the packet data plus the IP Header."""

    source = 0
    """The source address as an unsigned short integer."""

    _length = 8
    """Number of bytes needed from the stream."""

    def __init__(self, stream):
        self.destination = 0
        self.length = 0
        self.source = 0
        self._load(stream)

    def _load(self, stream):
        if not stream or stream.remaining() < UDPHeader._length:
            return
        self.source = stream.read_ushort()
        self.destination = stream.read_ushort()
        self.length = stream.read_ushort()
        chk_sum = stream.read_ushort()


class DecodedPacket(object):
    """The DecodedPacket class decodes the Ethernet, IP, IPv6, TCP and
    UDP headers.
    Decodes a 
    :class:`Packet <omniscript.packet.Packet>`,
    or an array containing the packet data.
    """

    ethernet = None
    """The Ethernet Header of the packet."""

    ip = None
    """The IP Header of the packet."""

    tcp = None
    """The TCP Header of the packet."""

    udp = None
    """The UDP Header of the packet."""

    payload = None
    """The payload that starts after the TCP or UDP Header."""

    def __init__(self, data):
        self.ethernet = EthernetHeader()
        self.ip = None
        self.tcp = None
        self.udp = None
        if data is not None:
            if isinstance(data, Packet):
                self._load(data.data)
            elif isinstance(data, array):
                self._load(data)

    def _load(self, data):
        stream = ReadStream(data)
        if not stream:
            return
        self.ethernet.load(stream)
        if self.ethernet.protocol_type == 0x0008:
            self.ip = IPHeader(stream)
            if self.ip.protocol == 6:
                self.tcp = TCPHeader(stream)
            if self.ip.protocol == 23:
                self.udp = UDPHeader(stream)
            self.payload = stream.read(stream.remaining())
