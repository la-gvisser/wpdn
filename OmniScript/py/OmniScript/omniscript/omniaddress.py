﻿"""OmniAddress classes.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

import os
import ctypes
import re
import string
import struct

import omniscript

import xml.etree.ElementTree as ET


def _clean_split(value):
    clean = value.replace('\t', ' ')
    clean = clean.replace('\r', ' ')
    clean = clean.replace('\n', ' ')
    clean = clean.replace(',', ' ')
    return clean.split()


def _hight_bits_set(value):
    count = 0
    mask = 0x80000000
    while (value & mask) != 0:
        count += 1
        mask = mask >> 1


def _compress_hextets(hextets):
    """Compresses a list of hextets.

    Compresses a list of strings, replacing the longest continuous
    sequence of "0" in the list with "" and adding empty strings at
    the beginning or at the end of the string such that subsequently
    calling ":".join(hextets) will produce the compressed version of
    the IPv6 address.

    Args:
        hextets: A list of strings, the hextets to compress.

    Returns:
        A string.

    """
    best_doublecolon_start = -1
    best_doublecolon_len = 0
    doublecolon_start = -1
    doublecolon_len = 0
    for index, hextet in enumerate(hextets):
        if hextet == '0' or hextet == '0000':
            doublecolon_len += 1
            if doublecolon_start == -1:
                # Start of a sequence of zeros.
                doublecolon_start = index
            if doublecolon_len > best_doublecolon_len:
                # This is the longest sequence of zeros so far.
                best_doublecolon_len = doublecolon_len
                best_doublecolon_start = doublecolon_start
        else:
            doublecolon_len = 0
            doublecolon_start = -1
    if best_doublecolon_len > 1:
        best_doublecolon_end = (best_doublecolon_start +
                                best_doublecolon_len)
        # For zeros at the end of the address.
        if best_doublecolon_end == len(hextets):
            hextets += ['']
        hextets[best_doublecolon_start:best_doublecolon_end] = ['']
        # For zeros at the beginning of the address.
        if best_doublecolon_start == 0:
            hextets = [''] + hextets
    return ":".join(hextets)


def format_ethernet(mac):
    """Format an Ethernet Address.
    
    Args:
        mac (str, int, long): address to format as '0123456789AB' or
        as an integer value.

    Returns:
        String as '01:23:45:67:89:AB'
    """
    data = '%012X' % mac if isinstance(mac, (int, long)) else mac
    return ':'.join([data[i:i+2] for i in range(0, 12, 2)])


def format_ipv4(ip, mask=0xF0000000):
    """Format an IPv4 Address.
    
    Args:
        ip (str, int, long): address to format as '12345678' or
        as an integer value.

    Returns:
        String as '18.52.86.120' or '12.34.56.0/24'
    """
    _ip = '%08X' % ip if isinstance(ip, (int, long)) else ip
    text = '.'.join(("%d" % int(_ip[i:i+2], 16)) for i in range(0, 8, 2))
    if mask != 0xF0000000:
        if mask == 0xE0000000:
            text += '/24'  # 3 * 8 bits
        elif mask == 0xC0000000:
            text += '/16'  # 2 * 8 bits
        elif mask == 0x80000000:
            text += '/8'  # 1 * 8 bits
        elif mask == 0x00000000:
            text += '/0'  # 0 * 8 bits
    return text


def format_ipv6(ip, compress=True):
    """Format an IPv6 Address.
    
    Args:
        ip (str, list): address to format as string
            '0123456789ABCDEFFEDCBA9876543210'
            or list (0123, 4567, 89AB, CDEF, FEDC, BA98, 7654, 3210)
            or as an integer value.
        
        compress (bool): remove the fist ':0000' strings.
    
    Returns:
        String as '0123:4567:89AB:CDEF:FEDC:BA98:7654:3210'
    """
    if isinstance(ip, (int, long)):
        data = '%032X' % ip
        hextets = list(([data[i:i+4] for i in range(0, 32, 4)]))
    elif isinstance(ip, basestring):
        hextets = list(([ip[i:i+4] for i in range(0, 32, 4)]))
    elif isinstance(ip, tuple) or isinstance(ip, list):
        if len(ip) > 0 and isinstance(ip[0], (int, long)):
            hextets = list(('%04X' % i) for i in ip)
        else:
            hextets = ip
    if compress:
        return _compress_hextets(hextets)
    return ':'.join(hextets)


def parse_ethernet(address):
    """Parse an Ethernet Address.
    
    Args:
        address (str, int, long): address to parse as '12:34:56:78:9A:BC' or
        as an integer value. The string may contain trailing wildcards '*'.

    Returns:
        (Address as String '123456789ABC', Mask as Integer 0xFC000000)
    """
    mask = 0
    bit_mask = 0x80000000
    has_wild = False
    if isinstance(address, (int, long)):
        octets = list(([address[i:i+2] for i in range(0, 12, 2)]))
        mask = None
    else:
        octets = address.upper().split(':')
        for i in range(6):
            if octets[i] == '*' or has_wild:
                octets[i] = '0'
                has_wild = True
            else:
                mask |= bit_mask
            bit_mask = bit_mask >> 1
            
        addr = ''.join(o.zfill(2) for o in octets)
        if mask == 0xFC000000:
            mask = None
    return (addr, mask)


def parse_ipv4(address):
    """Parse an IPv4 Address.
    
    Args:
        address (str, int, long): address to parse as '18.52.86.120' or
        as an integer value.

    Returns:
        String as '12345678'
    """
    result = address
    if isinstance(address, basestring):
        hexes = list(('%02X' % int(x)) for x in address.split('.'))
        result = ''.join(x for x in hexes)
    elif isinstance(address, (int, long)):
        result = '%08X' % address
    return result


def parse_ipv6(address):
    """Parse an IPv6 Address.
    
    Args:
        address (str, int, long): address to parse as
        '0123:4567:89AB:CDEF:FEDC:BA98:7654:3210' or 
        as an integer value.

    Returns:
        String as '0123456789ABCDEFFEDCBA9876543210'
    """
    if isinstance(address, (int, long)):
        data = '%32X' % address
    else:
        hextets = address.upper().split(':')
        blanks = hextets.count('')
        if blanks == 2:
            index = next(i for i, v in enumerate(hextets) if v == '')
            hextets[index:index+2] = ['']
        data = ''
        for h in hextets:
            if len(h) == 0:
                data += ''.join('0000' for p in range(9 - len(hextets)))
            else:
                data += h.zfill(4)
    return data


class BaseAddress(object):
    """The BaseAddress class is the base class for all Address classes."""

    mask = None
    """The mask of the address or None."""

    series = False
    """Is the address a series?"""

    def __init__(self, series, mask):
        self.mask = int(mask) if mask is not None else None
        self.series = series

    def format(self, compressed=True):
        return str(self)


class UndefinedAddress(BaseAddress):
    def __init__(self):
        BaseAddress.__init__(self, False, None)

    @property
    def address_type(self):
        return omniscript.ADDRESS_TYPE_UNDEFINED

    @property
    def _data(self):
        return None


class EthernetAddress(BaseAddress):
    """An Ethernet address."""

    address = None
    """An ethernet address string.
    Format is '1234567890AB'
    """

    def __init__(self, address=None, series=False, mask=None, data=None):
        BaseAddress.__init__(self, series, mask)
        if address is not None:
            self.address, self.mask = parse_ethernet(address)
        elif data is not None:
            self.address = data
##        self.addresses = []
##        self.addresses = _clean_split(data)
##        for address in addresses:
##            if address.find(':') > 0:
##                self.addresses.append(address)
##            else:
##                addr = format_ethernet(address)
##                if mask is not None:
##                    cidr = int(mask)
##                    if cidr != 0:
##                        addr + r'/' + str(_high_bits_set(cidr))
##                self.addresses.append(addr)

    def __repr__(self):
        return format_ethernet(self.address)

    @property
    def _data(self):
        return self.address

    @property
    def address_type(self):
        return omniscript.ADDRESS_TYPE_ETHERNET


class IPv4Address(BaseAddress):
    """An IP version 4 address."""

    address = None
    """The IPv4 address strings.
    Format: 01020304, 12345600/24, 2468AC80/25
    """

    def __init__(self, address=None, series=False, mask=None, data=None):
        BaseAddress.__init__(self, series, mask)
        if address is not None:
            self.address = parse_ipv4(address)
        elif data is not None:
            self.address = data
##        self.addresses = []
##        self.addresses = _clean_split(data)
##        for address in addresses:
##            if address.find('.') > 0:
##                self.addresses.append(address)
##            else:
##                addr = format_ipv4(address)
##                if mask is not None:
##                    cidr = int(mask)
##                    if cidr != 0:
##                        addr + r'/' + str(cidr)
##                self.addresses.append(addr)

    def __repr__(self):
        return format_ipv4(self.address, self.mask)

    @property
    def _data(self):
        return self.address

    @property
    def address_type(self):
        return omniscript.ADDRESS_TYPE_IP


class IPv6Address(BaseAddress):
    """An IP version 6 address."""

    address = None
    """An IPv6 address string.
    Format: '123456789ABCDEF0FEDCBA9876543210'
    """

    compressed = True
    """When True the address will be rendered in an abbrieviated form.
    The address: 0123:0000:0000:0000:0000:0000:0054:3210
    When True: 123::3210
    When False: 0123:0000:0000:0000:0000:0000:0000:3210
    """

    def __init__(self, address=None, series=False, mask=None, data=None):
        BaseAddress.__init__(self, series, mask)
        self.compressed = IPv6Address.compressed
        if address is not None:
            self.address = parse_ipv6(address)
        elif data is not None:
            self.address = data
##        self.addresses = []
##        addresses = _clean_split(data)
##        for address in addresses:
##            if address.find(':') > 0:
##                self.addresses.append(address)
##            else:
##                addr = format_ipv6(address)
##                if mask is not None:
##                    cidr = int(mask)
##                    if cidr != 0:
##                        addr + r'/' + str(cidr)
##                self.addresses.append(addr)

    def __repr__(self):
        return format_ipv6(self.address, self.compressed)

    def __str__(self):
        return format_ipv6(self.address, self.compressed)

    @property
    def _data(self):
        return self.address

    @property
    def address_type(self):
        return omniscript.ADDRESS_TYPE_IPV6

    def format(self, compressed=True):
        return format_ipv6(self.address, compressed)


class OtherAddress(BaseAddress):
    """All Other address types."""

    address = None
    """The address string.
    Format is '1234', ASCII-Hex string.
    """

    def __init__(self, mask, data):
        BaseAddress.__init__(self, False, mask)
        self.address = data

    @property
    def _data(self):
        return self.address

    @property
    def address_type(self):
        return omniscript.ADDRESS_TYPE_OTHER


class OmniAddress(object):
    """The OmniAddress class holds various types of network addresses."""

    name = None

    address = None
    """One of the BaseAddress class of address."""

    def __init__(self, name, address):
        if isinstance(address, BaseAddress):
            self.name = name
            self.address = address
        elif isinstance(address, ET.Element):
            self._load(address)

    @property
    def mask(self):
        """Get the mask of the address."""
        return self.address.mask

    @mask.setter
    def mask(self, mask):
        """Set the mask of the address."""
        self.address.mask = mask

    @mask.deleter
    def mask(self):
        """Delete the mask of the address."""
        del self.address.mask

    @property
    def series(self):
        """Is the addess a series. (Read Only)"""
        return self.address.series

    @property
    def address_type(self):
        """The type of the address. (Read Only)"""
        return self.address.address_type

    def _load(self, node):
        self.name = node.tag
        series = (self.name[0].lower() == 's')
        address_type = int(node.get('type', '0'))
        mask = node.get('mask')
        if mask:
            mask = int(mask)
        data = node.get('data')
        if address_type == omniscript.ADDRESS_TYPE_ETHERNET:
            self.address = EthernetAddress(None, series, mask, data)
        elif address_type == omniscript.ADDRESS_TYPE_IP:
            self.address = IPv4Address(None, series, mask, data)
        elif address_type == omniscript.ADDRESS_TYPE_IPV6:
            self.address = IPv6Address(None, series, mask, data)
        elif address_type == omniscript.ADDRESS_TYPE_WIRELESS:
            self.address = EthernetAddress(None, series, mask, data)
        else:
            self.address = OtherAddress(mask, data)

    def _store(self, node):
        elem = ET.SubElement(node, self.name,
                             {'class':'2',
                              'type':str(self.address.address_type),
                              'data':self.address._data})
        if self.mask is not None:
            elem.set('mask', str(self.mask))

    def parse(self, address):
        if address.find('.'):
            offset = address.find('/')
            series = offset != -1
            if series:
                mask = int(address[offset+1:])
                data = address[:offset]
            else:
                mask = 0
                data = address
            self.address = IPv4Address(series, mask, data)
            

def parse_ip_address(address):
    """Parse IP Address.
    
    Args:
        address (str, int, long): address to parse as an integer or a string: 
        '18.52.86.120' or '1234::5678:90ab' or '15 1234::5678:90ab' 
        (IPv6 address return by an Expert Query).

    Returns:
        An OmniAddress object:
        :class:`IPv4Address <omniscript.omniaddress.IPv4Address>` or
        :class:`IPv6Address <omniscript.omniaddress.IPv6Address>` or
        None.
    """
    if isinstance(address, (int, long)):
        if address > 0xFFFFFFFFL:
            return IPv6Address(address)
        return IPv4Address(address)
    if isinstance(address, basestring):
        if '.' in address:          # 1.2.3.4
            return IPv4Address(address)

        # Expert IPv6 format - 15 1234::5678:90ab
        r = re.search(r'(\d+)\s(.+)', address)
        if r:
            return IPv6Address(r.group(2))

        if ':' in address:          # 1234::5678:90AB
            return IPv6Address(address)
        elif len(address) == 8:     # 12345678
            return IPv4Address(address)
        elif len(address) == 32:     # 1234567890ABCDEF01234567890ABCDEF0
            return IPv6Address(address)
    return None
