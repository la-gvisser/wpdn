"""FilterNode class.
"""
#Copyright (c) WildPackets, Inc. 2013-2017. All rights reserved.

import os
import xml.etree.ElementTree as ET

import omniscript
import config

from analysismodule import AnalysisModule
from omniaddress import *
from omniport import OmniPort
from omniid import OmniId
from peektime import PeekTime

_filter_id_to_class = None

def _build_class_id():
    global _filter_id_to_class
    class_name_ids = omniscript.get_class_name_ids()
    _filter_id_to_class = dict((class_name_ids[name_], class_) \
                        for (name_, class_) in _filter_name_class)
    #id_class_names = omniscript.get_id_class_names()
    #for item in _filter_name_class:
    #    id_list = list(k for k,v in id_class_names.iteritems() if v == item[0])
    #    for id in id_list:
    #        _filter_id_to_class[id] = item[1]


def get_id_to_class():
    global _filter_id_to_class
    if _filter_id_to_class is None:
        _build_class_id()
    return _filter_id_to_class


def parse_omni_filter(element):
    id_to_class = get_id_to_class()
    clsid = OmniId(element.get('clsid'))
    filter_node = element.find('filternode')
    root = id_to_class[clsid](filter_node)
    and_node = filter_node.find('andnode')
    if and_node is not None:
        root.and_node = parse_omni_filter(and_node)
    or_node = filter_node.find('ornode')
    if or_node is not None:
        root.or_node = parse_omni_filter(or_node)
    return root


def parse_console_filter(element):
    global _console_filter_id_class
    clsid = OmniId(element.get('clsid'))
    filter_node = element.find('filternode')
    root = _console_filter_id_class[clsid](filter_node)
    and_node = element.find('andnode')
    if and_node is not None:
        root.and_node = parse_console_filter(and_node)
    or_node = element.find('ornode')
    if or_node is not None:
        root.or_node = parse_console_filter(or_node)
    return root


def store_omni_filter(element, operator, criteria):
    class_name_ids = omniscript.get_class_name_ids()

    rootnode = ET.SubElement(element, operator,
                  {'clsid':str(class_name_ids[criteria._class_name])})
    filter_node = ET.SubElement(rootnode, 'filternode')
    criteria._store(filter_node)
    if criteria.and_node is not None:
        store_omni_filter(filter_node, 'andnode', criteria.and_node)
    if criteria.or_node is not None:
        store_omni_filter(filter_node, 'ornode', criteria.or_node)


LOGICAL_OP_AND = 0
LOGICAL_OP_OR = 1
LOGICAL_OP_NOT = 2

DATA_TYPE_UNDEFINED = 0
DATA_TYPE_NULL = 1
DATA_TYPE_ADDRESS = 2
DATA_TYPE_ANALYSIS_MODULE = 3
DATA_TYPE_CHANNEL = 4
DATA_TYPE_ERROR = 5
DATA_TYPE_LENGTH = 6
DATA_TYPE_LOGICAL_OPERATOR = 7
DATA_TYPE_PATTERN = 8
DATA_TYPE_PORT = 9
DATA_TYPE_PROTOCOL = 10
DATA_TYPE_TCP_DUMP = 11
DATA_TYPE_VALUE = 12
DATA_TYPE_VLAN_MPLS = 13
DATA_TYPE_WAN_DIRECTION = 14
DATA_TYPE_WIRELESS = 15

ERROR_FLAG_CRC = 0x002
ERROR_FLAG_FRAME = 0x004
ERROR_FLAG_OVERSIZE = 0x010
ERROR_FLAG_RUNT = 0x020

directions = ['---', '<--', '-->', '<->']


def _direction_string(first, second):
    return directions[[0, 1][first] + [0, 2][second]]


def _invert_string(inverted):
    return [' ', ' ! '][inverted]


def _make_mask(byte_count, bit_length=32):
    mask = 0
    for i in range(byte_count):
        mask = (mask << 1) | 1
    return mask << (bit_length - byte_count)


def _bytes_to_hex_string(bytes):
    """Return a string of hex characters from a list of integers (bytes).
    
    Args:
        bytes ([int]): list of integers:  [18, 52, 86, 120]

    Returns:
        String: '12345678' 
    """
    return ''.join(hex(n)[2:] for n in bytes)


def _hex_string_to_bytes(value):
    """Return a list of single byte intergers from each pair of hex characters.

    Args:
        value (string): string of hex characters: '12345678'
    
    Returns:
        List of integers: [18, 52, 86, 120]
    """
    return [int(value[i:i+2], 16) for i in range(0, len(value), 2)]


def _hex_string_to_shorts(value):
    """Return a list of short intergers from each four hex characters.
    Used to parse IPv6 hex strings into list of 16-bit values.

    Args:
        value (string): string of hex characters: '12345678'
    
    Returns:
        List of integers: [4660, 22136]
    """
    return [int(value[i:i+4], 16) for i in range(0, len(value), 4)]


def _hex_string_to_string(value):
    """Return a string from each pair of hex characters.

    Args:
        value (string): string of hex characters: '48656C6C6F'
    
    Returns:
        String: 'Hello'
    """
    return ''.join(chr(int(value[i:i+2], 16)) for i in range(0, len(value), 2)).strip('\0')


def _string_to_hex_string(value, pad=0):
    """Return a string of hex characters from a string.

    Args:
        value (string): string of hex characters: 'Hello'
        pad: min length of the resulting string.: 8
    
    Returns:
        String: '48656C6C6F000000'
    """
    result = ''.join('%X' % ord(i) for i in value)
    if len(value) < pad:
        result += ''.join('00' for i in range(8 - len(value)))
    return result


class DataIdType(object):
    """Data Id Type"""

    id = None
    """The GUID of the type as a
    :class:`OmniId <omniscript.omniid.OmniId>` object.
    """

    data_type = DATA_TYPE_UNDEFINED
    """The Named Type. One of the DATA TYPE constants."""

    def __init(self):
        self.id = DataIdType.id
        self.data_type = DataIdType.data_type


class NamedDataType(object):
    """The Named Data Type class."""

    name = ''
    """The name of the type."""

    data_type = DATA_TYPE_UNDEFINED
    """The type of data/node. One of the DATA TYPE constants."""

    def __init__(self):
        self.name = NamedType.name
        self.data_type = NamedType.data_type


class WirelessChannel(object):
    """A wireless channel."""

    channel = 0
    """The channel number."""

    frequency = 0
    """The channel's frequency."""

    band = config.WIRELESS_BAND_ALL
    """The channel's band."""

    def __init__(self):
        self.channel = WirelessChannel.channel
        self.frequency = WirelessChannel.frequency
        self.band = WirelessChannel.band


class FilterNode(object):
    """The FilterNode class is the criteria of
    :class:`Filter <omniscript.filter.Filter>` object.
    The other filter node types are subclasses of this class.
    """

    pad_depth = 4
    """The number of bytes of padding per indentation level."""

    comment = None
    """User supplied comment for this node."""
    
    inverted = False
    """Is the logic of this node inverted?
    Default is False.
    """
    
    nodes = None
    """List of node derived from the
    :class:`FilterNode <omniscript.filternode.FilterNode>`
    class.
    """

    and_node = None
    """The AND node."""

    or_node = None
    """The OR node."""

    def __init__(self):
        # pad_depth is a class property.
        self.comment = FilterNode.comment
        self.inverted = FilterNode.inverted
        self.and_node = FilterNode.and_node
        self.or_node = FilterNode.or_node

    def _load(self, element):
        self.inverted = int(element.get('inverted', '0')) != 0
        self.comment = element.get('comment')

    def _store(self, element):
        element.set('inverted', ['0', '1'][self.inverted])
        if self.comment is not None:
            element.set('comment', self.comment);
        
    def to_string(self, pad, operation=""):
        text = ''
        if self.and_node:
            text += "\n" + self.and_node.to_string((pad+1), "and: ")
        if self.or_node:
            text += "\n" + self.or_node.to_string(pad, "or: ")
        return text


class ApplicationNode(FilterNode):
    """The ApplicationNode class, subclass of the
    :class:`FilterNode <omniscript.filternode.FilterNode>` class.
    """

    names = []
    """A list of appliation names"""

    _class_name = 'ApplicationFilterNode'
    _display_name = 'Application'

    def __init__(self, element=None):
        super(ApplicationNode, self).__init__()
        self.names = []
        if element is not None:
            self._load(element)


    def __str__(self):
        return '%s: %s' % (ApplicationNode._class_name, 
                           self.names)

    def _load(self, element):
        super(ApplicationNode, self)._load(element)
        for name in element.findall('application'):
            data = name.get('data')
            if data is not None:
                self.names.append(_hex_string_to_string(data))

    def _store(self, element):
        super(ApplicationNode, self)._store(element)
        for name in self.names :
            ET.SubElement(element, 'application',
                          {'class':'1',
                           'type':'8',
                           'data':_string_to_hex_string(name, 8)
                           })

    def to_string(self, pad, operation=""):
        criteria  = '%s%s%s: %s%s' % (
            ''.ljust(pad * FilterNode.pad_depth),
            operation,
            ApplicationNode._display_name,
            _invert_string(self.inverted),
            self.names)
        return criteria + FilterNode.to_string(self, pad)


class AddressNode(FilterNode):
    """The AddressNode class, subclass of the
    :class:`FilterNode <omniscript.filternode.FilterNode>` class.
    """

    address_1 = None
    """The first address as an
    :class:`OmniAddress <omniscript.omniaddress.OmniAddress>` object."""

    address_2 = None
    """The second address as an
    :class:`OmniAddress <omniscript.omniaddress.OmniAddress>` object."""
    
    accept_1_to_2 = False
    """Accept traffic from the first to the second address."""
    
    accept_2_to_1 = False
    """Accept traffic from the second to the first address."""

    _class_name = 'AddressFilterNode'
    _display_name = 'Address'

    def __init__(self, element=None):
        super(AddressNode, self).__init__()
        self.address_1 = AddressNode.address_1
        self.address_2 = AddressNode.address_2
        self.accept_1_to_2 = AddressNode.accept_1_to_2
        self.accept_2_to_1 = AddressNode.accept_2_to_1
        if element is not None:
            self._load(element)

    def __str__(self):
        return '%s: %s %s %s' % (AddressNode._class_name, 
                                 self.address_1,
                                 _direction_string(self.accept_1_to_2,
                                                   self.accept_2_to_1),
                                 self.address_2)

    def _load(self, element):
        super(AddressNode, self)._load(element)
        addr1 = element.find('addr1')
        if addr1 is not None:
            self.address_1 = OmniAddress('addr1', addr1).address
        addr2 = element.find('addr2')
        if addr2 is not None:
            self.address_2 = OmniAddress('addr2', addr2).address
        accept1to2 = element.find('accept1to2')
        if accept1to2 is not None:
            self.accept_1_to_2 = int(accept1to2.get('data', '0')) != 0
        accept2to1 = element.find('accept2to1')
        if accept2to1 is not None:
            self.accept_2_to_1 = int(accept2to1.get('data', '0')) != 0

    def _store(self, element):
        super(AddressNode, self)._store(element)
        if isinstance(self.address_1, BaseAddress):
            OmniAddress('addr1', self.address_1)._store(element)
        else:
            self.address_1._store(element)
        if self.address_2 is None:
            data = '0' * len(self.address_1.address)
            addr2 = OmniAddress('addr2',
                                (type(self.address_1))(mask=0, data=data))
            addr2._store(element)
        else:
            if isinstance(self.address_2, BaseAddress):
                addr2 = OmniAddress('addr2', self.address_2)
                if addr2.mask is None:
                    addr2.mask = _make_mask(len(self.address_1.address))
                addr2._store(element)
            else:
                self.address_2._store(element)
        ET.SubElement(element, 'accept1to2',
            {'data':['0', '1'][self.accept_1_to_2]})
        ET.SubElement(element, 'accept2to1',
            {'data':['0', '1'][self.accept_2_to_1]})

    def to_string(self, pad, operation=""):
        criteria  = '%s%s%s: %s%s %s %s' % (
            ''.ljust(pad * FilterNode.pad_depth),
            operation,
            AddressNode._display_name,
            _invert_string(self.inverted),
            str(self.address_1),
            _direction_string(self.accept_1_to_2, self.accept_2_to_1),
            str(self.address_2))
        return criteria + FilterNode.to_string(self, pad)


class BpfNode(FilterNode):
    """The BpfNode class, subclass of the
    :class:`FilterNode <omniscript.filternode.FilterNode>` class.
    """

    filter = ''
    """The filter's criteria in BPF syntax"""

    _class_name = 'BpfFilterNode'
    _display_name = 'BPF'

    def __init__(self, element=None):
        super(BpfNode, self).__init__()
        self.filter = BpfNode.filter
        if element is not None:
            self._load(element)

    def __str__(self):
        return '%s: %s' % (BpfNode._class_name,
                           self.filter)

    def _load(self, element):
        super(BpfNode, self)._load(element)
        data = element.find('bpfdata')
        if data is not None:
            self.filter = data.get('data', '')

    def _store(self, element):
        super(BpfNode, self)._store(element)
        ET.SubElement(element, 'bpfdata', {'data':self.filter})

    def to_string(self, pad, operation=""):
        criteria  = '%s%s%s: %s%s' % (
            ''.ljust(pad * FilterNode.pad_depth),
            operation,
            BpfNode._display_name,
            _invert_string(self.inverted),
            self.filter)
        return criteria + FilterNode.to_string(self, pad)


class ChannelNode(FilterNode):
    """The ChannelNode class, subclass of the
    :class:`FilterNode <omniscript.filternode.FilterNode>` class.
    """

    channel = 0
    """The channel."""

    _class_name = 'ChannelFilterNode'
    _display_name = 'Channel'

    def __init__(self, element=None):
        super(ChannelNode, self).__init__()
        self.channel = ChannelNode.channel
        if element is not None:
            self._load(element)

    def __str__(self):
        return '%s: %d' % (ChannelNode._class_name, 
                           self.channel)

    def _load(self, element):
        super(ChannelNode, self)._load(element)
        channel = element.find('channel')
        if channel is not None:
            self.channel = int(channel.get('data', '0'))

    def _store(self, element):
        super(ChannelNode, self)._store(element)
        ET.SubElement(element, 'channel', {'data':str(self.channel)})

    def to_string(self, pad, operation=""):
        criteria  = '%s%s%s: %s%d' % (
            ''.ljust(pad * FilterNode.pad_depth),
            operation,
            ChannelNode._display_name,
            _invert_string(self.inverted),
            self.channel)
        return criteria + FilterNode.to_string(self, pad)


class CountryNode(FilterNode):
    """The CountryNode class, subclass of the
    :class:`FilterNode <omniscript.filternode.FilterNode>` class.
    """

    Country_1 = None
    """The first country as a String."""

    Country_2 = None
    """The second Country as a string."""
    
    accept_1_to_2 = False
    """Accept traffic from the first to the second country."""
    
    accept_2_to_1 = False
    """Accept traffic from the second to the first coutry."""

    _class_name = 'CountryFilterNode'
    _display_name = 'Country'

    def __init__(self, element=None):
        super(CountryNode, self).__init__()
        self.country_1 = ''
        self.country_2 = ''
        self.accept_1_to_2 = CountryNode.accept_1_to_2
        self.accept_2_to_1 = CountryNode.accept_2_to_1
        if element is not None:
            self._load(element)

    def __str__(self):
        return '%s: %s %s %s' % (CountryNode._class_name, 
                                 self.country_1,
                                 _direction_string(self.accept_1_to_2,
                                                   self.accept_2_to_1),
                                 self.country_2)

    def _load(self, element):
        super(CountryNode, self)._load(element)
        country1 = element.find('country1')
        self.country_1 = country1.get('data', '')
        country2 = element.find('country2')
        self.country_2 = country2.get('data', '')
        accept1to2 = element.find('accept1to2')
        if accept1to2 is not None:
            self.accept_1_to_2 = int(accept1to2.get('data', '0')) != 0
        accept2to1 = element.find('accept2to1')
        if accept2to1 is not None:
            self.accept_2_to_1 = int(accept2to1.get('data', '0')) != 0

    def _store(self, element):
        super(CountryNode, self)._store(element)
        ET.SubElement(element, 'country1', {'data':self.country_1})
        ET.SubElement(element, 'country2', {'data':self.country_2})
        ET.SubElement(element, 'accept1to2',
            {'data':['0', '1'][self.accept_1_to_2]})
        ET.SubElement(element, 'accept2to1',
            {'data':['0', '1'][self.accept_2_to_1]})

    def to_string(self, pad, operation=""):
        criteria  = '%s%s%s: %s%s %s %s' % (
            ''.ljust(pad * FilterNode.pad_depth),
            operation,
            CountryNode._display_name,
            _invert_string(self.inverted),
            self.country_1,
            _direction_string(self.accept_1_to_2, self.accept_2_to_1),
            self.country_2)
        return criteria + FilterNode.to_string(self, pad)


class ErrorNode(FilterNode):
    """The ErrorNode class, subclass of the
    :class:`FilterNode <omniscript.filternode.FilterNode>` class.
    """

    crc_errors = False
    """Cyclical Redundency Check failures?"""

    frame_errors = False
    """Framing Error failures?"""

    oversize_errors = False
    """Oversized Packets?"""

    runt_errors = False
    """Undersized Packets?"""

    _class_name = 'ErrorFilterNode'
    _display_name = 'Error'

    def __init__(self, element=None):
        super(ErrorNode, self).__init__()
        self.crc_errors = ErrorNode.crc_errors
        self.frame_errors = ErrorNode.frame_errors
        self.oversize_errors = ErrorNode.oversize_errors
        self.runt_errors = ErrorNode.runt_errors
        if element is not None:
            self._load(element)

    def __str__(self):
        return '%s: c:%d, f:%d, o:%d, r:%s' % (ErrorNode._class_name, 
                                               self.crc_errors,
                                               self.frame_errors,
                                               self.oversize_errors,
                                               self.runt_errors)

    def _load(self, element):
        super(ErrorNode, self)._load(element)
        errorflags = element.find('errorflags')
        if errorflags is not None:
            flags = int(errorflags.get('data', '0'))
            self.crc_errors = (flags & ERROR_FLAG_CRC) != 0
            self.frame_errors = (flags & ERROR_FLAG_FRAME) != 0
            self.oversize_errors = (flags & ERROR_FLAG_OVERSIZE) != 0
            self.runt_errors = (flags & ERROR_FLAG_RUNT) != 0

    def _store(self, element):
        super(ErrorNode, self)._store(element)
        errorflags = 0
        errorflags |= [0, ERROR_FLAG_CRC][self.crc_errors]
        errorflags |= [0, ERROR_FLAG_FRAME][self.frame_errors]
        errorflags |= [0, ERROR_FLAG_OVERSIZE][self.oversize_errors]
        errorflags |= [0, ERROR_FLAG_RUNT][self.runt_errors]
        ET.SubElement(element, 'errorflags', {'data':str(errorflags)})

    def to_string(self, pad, operation=""):
        criteria  = '%s%s%s: %scrc:%s frame:%s oversize:%s runt:%s' % (
            ''.ljust(pad * FilterNode.pad_depth),
            operation,
            ErrorNode._display_name,
            _invert_string(self.inverted),
            ['F', 'T'][self.crc_errors],
            ['F', 'T'][self.frame_errors],
            ['F', 'T'][self.oversize_errors],
            ['F', 'T'][self.runt_errors])
        return criteria + FilterNode.to_string(self, pad)


class LengthNode(FilterNode):
    """The LengthNode class, subclass of the
    :class:`FilterNode <omniscript.filternode.FilterNode>` class.
    """

    minimum = 64
    """The minimum length.
    Default is 64 btyes.
    """

    maximum = 1518
    """The maximum lenth.
    Default is 1518 bytes.
    """

    _class_name = 'LengthFilterNode'
    _display_name = 'Length'

    def __init__(self, element=None):
        super(LengthNode, self).__init__()
        self.minimum = LengthNode.minimum
        self.maximum = LengthNode.maximum
        if element is not None:
            self._load(element)

    def __str__(self):
        return '%s: %d to %d' % (LengthNode._class_name,
                                 self.minimum,
                                 self.maximum)

    def _load(self, element):
        super(LengthNode, self)._load(element)
        min = element.find('min')
        if min is not None:
            self.minimum = int(min.get('data', '0'))
        max = element.find('max')
        if max is not None:
            self.maximum = int(max.get('data', '0'))

    def _store(self, element):
        super(LengthNode, self)._store(element)
        ET.SubElement(element, 'min', {'data':str(self.minimum)})
        ET.SubElement(element, 'max', {'data':str(self.maximum)})

    def to_string(self, pad, operation=""):
        criteria  = '%s%s%s: %smin:%d max:%d' % (
            ''.ljust(pad * FilterNode.pad_depth),
            operation,
            LengthNode._display_name,
            _invert_string(self.inverted),
            self.minimum,
            self.maximum)
        return criteria + FilterNode.to_string(self, pad)


class LogicalNode(FilterNode):
    """The LogicalNode class, subclass of the
    :class:`FilterNode <omniscript.filternode.FilterNode>` class.
    """

    operator = 0
    """The logical operator:
        0 - And
        1 - Or
        2 - Not
    """

    left = None
    """The left side opperand.
    """

    right = None
    """The right side opperand.
    """

    _class_name = 'LogicalFilterNode'
    _display_name = 'Logical'
    _op_names = ['and', 'or', 'not']

    def __init__(self, element=None):
        super(LogicalNode, self).__init__()
        self.operator = LogicalNode.operator
        self.left = LogicalNode.left
        self.right = LogicalNode.right
        if element is not None:
            self._load(element)

    def __str__(self):
        return '%s: %s' % (LogicalNode._class_name,
                           LogicalNode._op_names[self.operator])

    def _load(self, element):
        super(LogicalNode, self)._load(element)
        logical = element.find('logical')
        if logical is not None:
            op = logical.find('op')
            if op is not None:
                self.operator = int(op.get('data', '0'))
            left = logical.find('left')
            if left is not None:
                self.left = parse_omni_filter(left)
            right = logical.find('right')
            if right is not None:
                self.right = parse_omni_filter(right)

    def _store(self, element):
        super(LogicalNode, self)._store(element)
        logical = ET.SubElement(element, 'logical')
        ET.SubElement(logical, 'op').text = str(self.operator)
        if self.left is not None:
            store_omni_filter(logical, 'left', self.left)
        if self.right is not None:
            store_omni_filter(logical, 'right', self.right)

    def to_string(self, pad, operation=""):
        _padding = ''.ljust(pad * FilterNode.pad_depth)
        _padding_plus = ''.ljust((pad+1)* FilterNode.pad_depth)
        criteria  = '%s%s%s: %s %s\n%sleft:%s\n%sright:%s' % (
            _padding,
            operation,
            LogicalNode._display_name,
            _invert_string(self.inverted),
            LogicalNode._op_names[self.operator],
            _padding_plus,
            self.left.to_string(pad+1),
            _padding_plus,
            self.right.to_string(pad+1))
        return criteria + FilterNode.to_string(self, pad)


class PatternNode(FilterNode):
    """The PatternNode class, subclass of the
    :class:`FilterNode <omniscript.filternode.FilterNode>` class.
    """

    pattern_type = config.PATTERN_TYPE_ASCII
    """The Pattern type. One of the PATTERN TYPE constants."""

    pattern = ''
    """The pattern to match as ASCII hex.
    1=31, 2=32, 3=33, ...
    a=61, b=62, c=63, ...
    Use set_pattern() to convert strings to ASCII hex.
    """

    start_offset = 0
    """Beginging offset within the packet."""

    end_offset = 1517
    """Ending offset within the packet."""

    case_sensitive = False
    """Is the match case sensitive?"""

    _class_name = 'PatternFilterNode'
    _display_name = 'Pattern'

    def __init__(self, element=None):
        super(PatternNode, self).__init__()
        self.pattern_type = PatternNode.pattern_type
        self.pattern = PatternNode.pattern
        self.start_offset = PatternNode.start_offset
        self.end_offset = PatternNode.end_offset
        self.case_sensitive = PatternNode.case_sensitive
        if element is not None:
            self._load(element)

    def __str__(self):
        return '%s: %s' % (PatternNode._class_name, 
                           self.pattern)

    def _load(self, element):
        super(PatternNode, self)._load(element)
        pattern_type = element.find('patterntype')
        if pattern_type is not None:
            self.pattern_type = int(pattern_type.get('data', '0'))
        pattern = element.find('patterndata')
        if pattern is not None:
            self.pattern = pattern.get('data', '00')
        start = element.find('startoffset')
        if start is not None:
            self.start_offset = int(start.get('data', '0'))
        end = element.find('endoffset')
        if end is not None:
            self.end_offset = int(end.get('data', '0'))
        case_sensitive = element.find('casesensitive')
        if case_sensitive is not None:
            self.case_sensitive = int(case_sensitive.get('data', '0')) != 0

    def _store(self, element):
        super(PatternNode, self)._store(element)
        ET.SubElement(element, 'patterntype', {'data':str(self.pattern_type)})
        ET.SubElement(element, 'patterndata', {'data':self.pattern})
        ET.SubElement(element, 'startoffset', {'data':str(self.start_offset)})
        ET.SubElement(element, 'endoffset', {'data':str(self.end_offset)})
        ET.SubElement(element, 'casesensitive',
                      {'data':['0', '1'][self.case_sensitive]})

    def set_pattern(self, value):
        """Sets the pattern attribte to the ASCII hex of value."""
        if isinstance(value, basestring):
            self.pattern = ''.join(("%02X" % ord(x)) for x in value)
        else:
            self.pattern = str(value)

    def to_string(self, pad, operation=""):
        criteria  = '%s%s%s: %s%s begin:%d end:%d' % (
            ''.ljust(pad * FilterNode.pad_depth),
            operation,
            PatternNode._display_name,
            _invert_string(self.inverted),
            self.pattern,
            self.start_offset,
            self.end_offset)
        return criteria + FilterNode.to_string(self, pad)


class PluginNode(FilterNode):
    """The PluginNode class, subclass of the
    :class:`FilterNode <omniscript.filternode.FilterNode>` class.
    """

    ids = []
    """A list of plugin ids as a list of 
    :class:`OmniId <omniscript.omniid.OmniId>` objects.
    Use the add_analysis_module function to add items to the list.
    """

    _class_name = 'PluginFilterNode'
    _display_name = 'Plugin'

    def __init__(self, element=None):
        super(PluginNode, self).__init__()
        self.ids = []
        if element is not None:
            self._load(element)


    def __str__(self):
        return '%s: %s' % (PluginNode._class_name, 
                           self.ids)

    def _load(self, element):
        super(PluginNode, self)._load(element)
        for id in element.findall('pluginid'):
            clsid = id.get('clsid')
            if clsid is not None:
                self.ids.append(OmniId(clsid))

    def _store(self, element):
        super(PluginNode, self)._store(element)
        for clsid in self.ids :
            ET.SubElement(element, 'pluginid', {'clsid':str(clsid)})

    def add_analysis_module(self, clsid):
        if isinstance(clsid, OmniId):
            self.ids.append(clsid)
        elif isinstance(clsid, basestring):
            self.ids.append(OmniId(clsid))
        elif isinstance(clsid, AnalysisModule):
            self.ids.append(clsid.id)

    def to_string(self, pad, operation=""):
        criteria  = '%s%s%s: %s%s' % (
            ''.ljust(pad * FilterNode.pad_depth),
            operation,
            PluginNode._display_name,
            _invert_string(self.inverted),
            self.ids)
        return criteria + FilterNode.to_string(self, pad)


class PortNode(FilterNode):
    """The PortNode class, subclass of the
    :class:`FilterNode <omniscript.filternode.FilterNode>` class.
    """

    port_1 = None
    """The first port as an
    :class:`OmniPort <omniscript.omniport.OmniPort>` object."""

    port_2 = None
    """The second port as an
    :class:`OmniPort <omniscript.omniport.OmniPort>` object."""
    
    accept_1_to_2 = False
    """Accept traffic from the first to the second address."""
    
    accept_2_to_1 = False
    """Accept traffic from the second to the first address."""

    _class_name = 'PortFilterNode'
    _display_name = 'Port'

    def __init__(self, element=None):
        super(PortNode, self).__init__()
        self.port_1 = PortNode.port_1
        self.port_2 = PortNode.port_2
        self.accept_1_to_2 = PortNode.accept_1_to_2
        self.accept_2_to_1 = PortNode.accept_2_to_1
        if element is not None:
            self._load(element)

    def __str__(self):
        return '%s: %s %s %s' % (PortNode._class_name, 
                                 self.port_1,
                                 _direction_string(self.accept_1_to_2,
                                                   self.accept_2_to_1),
                                 self.port_2)
    def _load(self, element):
        super(PortNode, self)._load(element)
        sport1 = element.find('Sport1')
        if sport1 is not None:
            self.port_1 = OmniPort(sport1)
        sport2 = element.find('Sport2')
        if sport2 is not None:
            self.port_2 = OmniPort(sport2)
        accept1to2 = element.find('accept1to2')
        if accept1to2 is not None:
            self.accept_1_to_2 = int(accept1to2.get('data', '0')) != 0
        accept2to1 = element.find('accept2to1')
        if accept2to1 is not None:
            self.accept_2_to_1 = int(accept2to1.get('data', '0')) != 0
        
    def _store(self, element):
        super(PortNode, self)._store(element)
        if self.port_1 is not None:
            if not isinstance(self.port_1, OmniPort):
                OmniPort(self.port_1)._store(element, 'Sport1')
            else:
                self.port_1._store(element, 'Sport1')
        else:
            OmniPort()._store(element, 'Sport1')
        if self.port_2 is not None:
            if not isinstance(self.port_2, OmniPort):
                OmniPort(self.port_2)._store(element, 'Sport2')
            self.port_2._store(element, 'Sport2')
        else:
            OmniPort()._store(element, 'Sport2')
        ET.SubElement(element, 'accept1to2', {'data':['0', '1'][self.accept_1_to_2]})
        ET.SubElement(element, 'accept2to1', {'data':['0', '1'][self.accept_2_to_1]})

    def to_string(self, pad, operation=""):
        criteria = \
            ''.ljust(pad * FilterNode.pad_depth) + \
            operation + \
            PortNode._display_name + \
            _invert_string(self.inverted) + \
            str(self.port_1) + \
            _direction_string(self.accept_1_to_2, self.accept_2_to_1) + \
            str(self.port_2)
        return criteria + FilterNode.to_string(self, pad)


class ProtocolNode(FilterNode):
    """The ProtocolNode class, subclass of the
    :class:`FilterNode <omniscript.filternode.FilterNode>` class.
    """

    protocol = 0
    """The protocol to match."""

    slice_to_header = False
    """Slice the packet to the end of the matching protocol's header.
    Default is False.
    """

    _class_name = 'ProtocolFilterNode'
    _display_name = 'Protocol'

    def __init__(self, element=None):
        super(ProtocolNode, self).__init__()
        self.protocol = ProtocolNode.protocol
        self.slice_to_header = ProtocolNode.slice_to_header
        if element is not None:
            self._load(element)

    def __str__(self):
        return '%s: %d' % (ProtocolNode._class_name,
                           self.protocol)

    def _load(self, element):
        super(ProtocolNode, self)._load(element)
        elem = element.find('protocol')
        data = elem.get('data')
        if data is not None:
            octets = list(data[x:x+2] for x in range(0, 8, 2))
            octets.reverse()
            self.protocol = int(''.join(octets), 16)
        self.slice_to_header = int(elem.get('slicetoheader', '0')) != 0

    def _store(self, element):
        super(ProtocolNode, self)._store(element)
        protocol = "%08X" % self.protocol
        octets = list(protocol[x:x+2] for x in range(0, 8, 2))
        octets.reverse()
        ET.SubElement(element, 'protocol',
                      {'data':''.join(octets),
                       'class':'1',
                       'type':'9',
                       'slicetoheader':['0', '1'][self.slice_to_header]})

    def set_protocol(self, name):
        name_ids = omniscript.get_protocol_short_name_ids()
        self.protocol = name_ids.get(name, 0)

    def to_string(self, pad, operation=""):
        criteria  = '%s%s%s: %s%d' % (
            ''.ljust(pad * FilterNode.pad_depth),
            operation,
            ProtocolNode._display_name,
            _invert_string(self.inverted),
            self.protocol)
        return criteria + FilterNode.to_string(self, pad)


class TimeRangeNode(FilterNode):
    """The TimeRangeNode class, subclass of the
    :class:`FilterNode <omniscript.filternode.FilterNode>` class.
    """

    start = None
    """The start time as
    :class:`PeekTime <omniscript.peektime.PeekTime>`.
    """

    end = None
    """The end time as
    :class:`PeekTime <omniscript.peektime.PeekTime>`.
    """

    _class_name = 'TimeRangeFilterNode'
    _display_name = 'TimeRange'

    def __init__(self, element=None):
        super(TimeRangeNode, self).__init__()
        self.start = TimeRangeNode.start
        self.end = TimeRangeNode.end
        if element is not None:
            self._load(element)

    def __str__(self):
        return '%s: %s to %s' % (TimeRangeNode._class_name,
                           str(self.start),
                           str(self.end))

    def _load(self, element):
        super(TimeRangeNode, self)._load(element)
        start = element.find('start')
        if start is not None:
            self.start = PeekTime(start.get('data'))
        end = element.find('end')
        if end is not None:
            self.end = PeekTime(end.get('data'))

    def _store(self, element):
        super(TimeRangeNode, self)._store(element)
        ET.SubElement(element, 'start', {'data':str(self.start.value)})
        ET.SubElement(element, 'end', {'data':str(self.end.value)})

    def to_string(self, pad, operation=""):
        criteria  = '%s%s%s: %sstart: %s end: %s' % (
            ''.ljust(pad * FilterNode.pad_depth),
            operation,
            TimeRangeNode._display_name,
            _invert_string(self.inverted),
            str(self.start),
            str(self.end))
        return criteria + FilterNode.to_string(self, pad)


class ValueNode(FilterNode):
    """The ValueNode class, subclass of the
    :class:`FilterNode <omniscript.filternode.FilterNode>` class.
    """

    value = 0
    """The value to match.
    Default is 0.
    """

    mask = 0xFFFFFFFF
    """The mask to apply to the packet data before the match.
    Default is 0xFFFFFFFF.
    """

    offset = 0
    """The offset within the packet to begin searching.
    Default is 0.
    """

    length = 4
    """The amount of packet data to search.
    Default is 4 bytes.
    """

    operator = config.VALUE_OPERATOR_EQUAL
    """The match operator. One of the VALUE OPERATOR  constants.
    Default is VALUE_OPERATOR_EQUAL.
    """

    flags = config.VALUE_FLAG_NETWORK_BYTE_ORDER
    """Flags to contrain the match. A set of VALUE FLAG  constants.
    Default is VALUE_FLAG_NETWORK_BYTE_ORDER.
    """

    _class_name = 'ValueFilterNode'
    _display_name = 'Value'

    def __init__(self, element=None):
        super(ValueNode, self).__init__()
        self.value = ValueNode.value
        self.mask = ValueNode.mask
        self.offset = ValueNode.offset
        self.length = ValueNode.length
        self.operator = ValueNode.operator
        self.flags = ValueNode.flags
        if element is not None:
            self._load(element)

    def __str__(self):
        return '%s: %d off:%d len:%d' % (ValueNode._class_name, 
                                         self.value,
                                         self.offset,
                                         self.length)

    def _load(self, element):
        super(ValueNode, self)._load(element)
        value = element.find('value')
        if value is not None:
            self.value = int(value.get('data', '0'))
        mask = element.find('mask')
        if mask is not None:
            self.mask = int(mask.get('data', '-1'))
        offset = element.find('offset')
        if offset is not None:
            self.offset = int(offset.get('data', '0'))
        op = element.find('op')
        if op is not None:
            self.op = int(op.get('data', '0'))
        flags = element.find('flags')
        if flags is not None:
            self.flags = int(flags.get('data', '0'))

    def _store(self, element):
        super(ValueNode, self)._store(element)
        ET.SubElement(element, 'value', {'type':'4', 'data':str(self.value)})
        ET.SubElement(element, 'mask', {'type':'4', 'data':str(self.mask)})
        ET.SubElement(element, 'offset', {'data':str(self.offset)})
        ET.SubElement(element, 'op', {'data':str(self.operator)})
        ET.SubElement(element, 'flags', {'data':str(self.flags)})

    def to_string(self, pad, operation=""):
        criteria  = '%s%s%s: %s%s' % (
            ''.ljust(pad * FilterNode.pad_depth),
            operation,
            ValueNode._display_name,
            _invert_string(self.inverted),
            self.value)
        return criteria + FilterNode.to_string(self, pad)


class VlanMplsNode(FilterNode):
    """The VlanMplsNode class, subclass of the
    :class:`FilterNode <omniscript.filternode.FilterNode>` class.
    """

    ids = None
    """A list of VLAN Ids and Id ranges."""
    
    labels = None
    """A list of MPLS Labels and Label ranges."""
    
    _class_name = 'VlanFilterNode'
    _display_name = 'Vlan-Mpls'

    def __init__(self, element=None):
        super(VlanMplsNode, self).__init__()
        self.ids = []
        self.labels = []
        if element is not None:
            self._load(element)

    def __str__(self):
        return '%s: %s %s' % (VlanMplsNode._class_name, 
                              self.ids,
                              self.labels)

    def _load(self, element):
        super(VlanMplsNode, self)._load(element)
        ids_elem = element.find('ids')
        if ids_elem is not None:
            ids = ids_elem.get('data', '')
            if len(ids) > 0:
                for _id in ids.split(' '):
                    self.add_id(_id)
        labels_elem = element.find('labels')
        if labels_elem is not None:
            labels = labels_elem.get('data', '')
            if len(labels) > 0:
                for _label in labels.split(' '):
                    self.add_label(_label)

    def _store(self, element):
        super(VlanMplsNode, self)._store(element)
        ids = ''
        for _id in self.ids:
            if isinstance(_id, (int, long)):
                ids += " %d" % (_id)
            elif isinstance(_id, tuple):
                ids += " %d-%d" % _id
        labels = ''
        for _label in self.labels:
            if isinstance(_label, (int, long)):
                labels += " %d" % (_label)
            elif isinstance(_label, tuple):
                labels += " %d-%d" % _label
        ET.SubElement(element, 'ids', {'data':ids.strip()})
        ET.SubElement(element, 'labels', {'data':labels.strip()})

    def add_id(self, _id):
        """Add a VLAN Id or Id range."""
        if isinstance(_id, (int, long)):
            self.ids.append(_id)
        elif isinstance(_id, basestring):
            items = _id.split('-')
            if len(items) == 1:
                self.ids.append(int(items[0]))
            elif len(items) > 1:
                self.ids.append((int(items[0]), int(items[1])))

    def add_ids(self, *_ids):
        """Add multiple VLAN Ids and/or Id ranges.
        Either as a string: '10 100-110 256' or a list: (10, '100-110', 256).
        If the list contains a range, the range must be quoted.
        """
        if isinstance(_ids, list) or isinstance(_ids, tuple):
            for _id in _ids:
                if isinstance(_id, (int, long)) or isinstance(_id, basestring):
                    self.add_id(_id)
                else:
                    self.add_ids(_id)
        elif isinstance(_ids, basestring):
            for _id in _ids.split(' '):
                self.add_id(_id)

    def add_label(self, _label):
        """Add an MPLS Label."""
        if isinstance(_label, (int, long)):
            self.labels.append(_label)
        elif isinstance(_label, basestring):
            items = _label.split('-')
            if len(items) == 1:
                self.labels.append(int(items[0]))
            elif len(items) > 1:
                self.labels.append((int(items[0]), int(items[1])))

    def add_labels(self, *_labels):
        """Add multiple MPLS Labels and/or Label ranges.
        Either as a string: '10 100-110 256' or a list: (10, '100-110', 256).
        If the list contains a range, the range must be quoted.
        """
        if isinstance(_labels, list) or isinstance(_labels, tuple):
            for _label in _labels:
                if isinstance(_label, (int, long)) or isinstance(_label, basestring):
                    self.add_label(_label)
                else:
                    self.add_labels(_label)
        elif isinstance(_labels, basestring):
            for _label in _labels:
                self.add_label(_label)

    def to_string(self, pad, operation=""):
        criteria  = '%s%s%s: %sIds:%s Labels:%s' % (
            ''.ljust(pad * FilterNode.pad_depth),
            operation,
            VlanMplsNode._display_name,
            _invert_string(self.inverted),
            self.ids,
            self.labels)
        return criteria + FilterNode.to_string(self, pad)


class WANDirectionNode(FilterNode):
    """The WANDirectionNode class, subclass of the
    :class:`FilterNode <omniscript.filternode.FilterNode>` class.
    """

    direction = config.WAN_DIRECTION_UNDEFINED
    """Direction: to the DCE or to the DTE."""
    
    _class_name = 'DirectionFilterNode'
    _display_name = 'WAN Direction'

    def __init__(self, element=None):
        super(WANDirectionNode, self).__init__()
        self.direction = WANDirectionNode.direction
        if element is not None:
            self._load(element)

    def __str__(self):
        return '%s: to %s' % (WANDirectionNode._class_name, 
                              ['', 'DCE', 'DTE'][self.direction])

    def _load(self, element):
        super(WANDirectionNode, self)._load(element)
        direction = element.find('direction')
        if direction is not None:
            self.direction = int(direction.get('data', '0'))

    def _store(self, element):
        super(WANDirectionNode, self)._store(element)
        if self.direction != config.WAN_DIRECTION_UNDEFINED:
            ET.SubElement(element, 'direction', {'data':str(self.direction)})

    def to_string(self, pad, operation=""):
        criteria  = '%s%s%s: %s%s' % (
            ''.ljust(pad * FilterNode.pad_depth),
            operation,
            WANDirectionNode._display_name,
            _invert_string(self.inverted),
            ['Unknown', 'to DCE', 'to DTE'][self.direction])
        return criteria + FilterNode.to_string(self, pad)


class WirelessNode(FilterNode):
    """The WirelessNode class, subclass of the
    :class:`FilterNode <omniscript.filternode.FilterNode>` class.
    """

    channel_number = None
    """The Channel Number to fileter on."""

    channel_frequency = None
    """The Channel Frequency to fileter on."""

    channel_band = config.WIRELESS_BAND_ALL
    """The Channel Band to fileter on. One of the WIRELESS BAND constants.
    Default is WIRELESS_BAND_ALL.
    """

    data_rate = None
    """The Data Rate, in megabits per second, to filter as a floating point
    value.
    """

    flags = None
    """The flags. A set of the WIRELESS FLAG constants."""
    
    signal_minimum = None
    """The Minumum Signal to filter on."""

    signal_maximum = None
    """The Maximum Signal to filter on."""

    noise_minimum = None
    """The Minumum Noise Level to filter on."""

    noise_maximum = None
    """The Maximum Noise Level to filter on."""

    encryption = None
    """Encryption information."""

    decryption = None
    """Decryption information."""

    bssid = None
    """The BSSID to filter on."""

    mask_bssid = 0
    """Mask to constrain the BSSID match."""
    
    _class_name = 'WirelessFilterNode'
    _display_name = 'Wireless'

    def __init__(self, element=None):
        super(WirelessNode, self).__init__()
        self.channel_number = WirelessNode.channel_number
        self.channel_frequency = WirelessNode.channel_frequency
        self.channel_band = WirelessNode.channel_band
        self.data_rate = WirelessNode.data_rate
        self.flags = WirelessNode.flags
        self.signal_minimum = WirelessNode.signal_minimum
        self.signal_maximum = WirelessNode.signal_maximum
        self.noise_minimum = WirelessNode.noise_minimum
        self.noise_maximum = WirelessNode.noise_maximum
        self.encryption = WirelessNode.encryption
        self.decryption = WirelessNode.decryption
        self.bssid = WirelessNode.bssid
        self.mask_bssid = WirelessNode.mask_bssid
        if element is not None:
            self._load(element)

    def __str__(self):
        return '%s: ch:%d Hz:%d' % (WirelessNode._class_name,
                                    self.channel_number,
                                    self.channel_frequency)

    def _load(self, element):
        super(WirelessNode, self)._load(element)
        channel = element.find('channel')
        if channel is not None:
            self.channel_number = int(channel.get('channelnumber', '0'))
            self.channel_frequency = int(channel.get('channelfrequency', '0'))
            self.channel_band = int(channel.get('channelband', '0'))
        datarate = element.find('datarate')
        if datarate is not None:
            data_rate = int(datarate.get('data', '0'))
            self.data_rate = data_rate / 2.0
        signal = element.find('signal')
        if signal is not None:
            self.signal_minimum = int(signal.get('min', '0'))
            self.signal_maximum = int(signal.get('max', '0'))
        noise = element.find('noise')
        if noise is not None:
            self.noise_minimum = int(noise.get('min', '0'))
            self.noise_maximum = int(noise.get('max', '0'))
        encryption = element.find('encryption')
        if encryption is not None:
            self.encryption = int(encryption.get('data', '0')) != 0
        decryption = element.find('decryptionerror')
        if decryption is not None:
            self.decryption = int(decryption.get('data', '0')) != 0
        bssid = element.find('bssid')
        if bssid is not None:
            self.bssid = OmniAddress(bssid)
        flags = element.find('flagsn')
        if flags is not None:
            self.flags = int(flags.get('data', '0'))

    def _store(self, element):
        if self.channel_number is not None:
            ET.SubElement(element, 'channel',
                          {'channelnumber':str(self.channel_number),
                           'channelfrequency':str(self.channel_frequency),
                           'channelband':str(self.channel_band)})
        if self.data_rate is not None:
            data_rate = int(self.data_rate * 2)
            ET.SubElement(element, 'datarate', {'data':str(data_rate)})
        if self.signal_minimum is not None:
            ET.SubElement(element, 'signal', {'min':str(self.signal_minimum),
                                              'max':str(self.signal_maximum)})
        if self.noise_minimum is not None:
            ET.SubElement(element, 'noise', {'min':str(self.noise_minimum),
                                             'max':str(self.noise_maximum)})
        if self.encryption is not None:
            ET.SubElement(element, 'encryption',
                          {'data':['0', '1'][self.encryption]})
        if self.decryption is not None:
            ET.SubElement(element, 'decryptionerror',
                          {'data':['0', '1'][self.decryption]})
        if self.bssid is not None:
            self.bssid._store(element)
        if self.flags is not None:
            ET.SubElement(element, 'flagsn', {'data':str(self.flags)})

    def to_string(self, pad, operation=""):
        criteria  = '%s%s%s: %s%s' % (
            ''.ljust(pad * FilterNode.pad_depth),
            operation,
            WirelessNode._display_name,
            _invert_string(self.inverted),
            int(self.channel_number) if self.channel_number else '')
        return criteria + FilterNode.to_string(self, pad)


_filter_name_class = [
    ('', None),
    ('AddressFilterNode', AddressNode),
    ('ApplicationFilterNode', ApplicationNode),
    ('BpfFilterNode', BpfNode),
    ('ChannelFilterNode', ChannelNode),
    ('CountryFilterNode', CountryNode),
    ('ErrorFilterNode', ErrorNode),
    ('LengthFilterNode', LengthNode),
    ('LogicalFilterNode', LogicalNode),
    ('PatternFilterNode', PatternNode),
    ('PluginFilterNode', PluginNode),
    ('PortFilterNode', PortNode),
    ('ProtocolFilterNode', ProtocolNode),
    ('TimeRangeFilterNode', TimeRangeNode),
    ('ValueFilterNode', ValueNode),
    ('VlanFilterNode', VlanMplsNode),
    ('DirectionFilterNode', WANDirectionNode),
    ('WirelessFilterNode', WirelessNode)
    ]

_console_filter_id_class = {
    OmniId('{B4298A64-5A40-4F5F-ABCD-B14BA0F8D9EB}'): None,
    OmniId('{2D2D9B91-08BF-44CF-B240-F0BBADBF21B5}'): AddressNode,
    OmniId('{8C7C9172-82B2-43DC-AAF1-41ED80CE828F}'): ApplicationNode,
    OmniId('{11FC8E5E-B21E-446B-8024-39E41E6865E1}'): BpfNode,
    OmniId('{6E8DAF74-AF0D-4CD3-865D-D559A5798C5B}'): ChannelNode,
    OmniId('{EF8A52E6-233F-4337-AAED-B021CBD8E9E4}'): CountryNode,
    OmniId('{D0BDFB3F-F72F-4AEF-8E17-B16D4D3543FF}'): ErrorNode,
    OmniId('{CF190294-C869-4D67-93F2-9A53FDFAE77D}'): LengthNode,
    OmniId('{D8B5CE02-90CA-48AC-8188-468AC293B9C6}'): None,
    OmniId('{47D49D7C-8219-40D5-9E5D-8ADEAACC644D}'): PatternNode,
    OmniId('{D0329C21-8B27-489F-84D7-C7B783634A6A}'): PluginNode,
    OmniId('{297D404D-3610-4A18-95A2-22768B554BED}'): PortNode,
    OmniId('{F4342DAD-4A56-4ABA-9436-6E3C30DAB1C8}'): ProtocolNode,
    OmniId('{85F2216E-6E65-4AE9-B14B-CC8DF48CAE6A}'): TimeRangeNode,
    OmniId('{838F0E57-0D9F-4095-9C12-F1040C84E428}'): ValueNode,
    OmniId('{1999CC65-01DA-4256-81B4-C303BDE88BDA}'): VlanMplsNode,
    OmniId('{90BAE500-B97B-42B0-9886-0947F34001EF}'): WANDirectionNode,
    OmniId('{899E11AD-1849-40BA-9454-9F03798B3A6C}'): WirelessNode
    }
