"""ExpertResult class.
"""
#Copyright (c) Savvius, Inc. 2013-2017. All rights reserved.

import os
import sys
import logging
import xml.etree.ElementTree as ET

import config
import omniscript

from peektime import PeekTime


def _to_bool(value):
    return "1" if value else "0"

_tag_delimiter = 'delimiter'
_tag_list = 'list'
_tag_null = 'null'
_tag_string = 'string'
_tag_type = 'type'
_tag_uint = 'unsigned int'

def _parse_result(value):
    if isinstance(value, ET.Element):
        _dist = False
        if _tag_list in value.attrib:
            _dist = int(value.attrib[_tag_list]) == 0
            if _tag_delimiter in value.attrib:
                _del = value.attrib[_tag_delimiter]
                return [] if value.text is None else [int(v) for v in value.text.split(_del)]
        if _tag_type in value.attrib:
            t = value.attrib[_tag_type]
            if t == _tag_uint:
                return int(value.text)
            elif t == _tag_string:
                return value.text
            elif t == _tag_null:
                return None
            else:
                return value.text
    return value


class ExpertResult(object):
    """Results of a Query to Engine's Expert.
    """

    table = ''
    """The table being queried.
    The ExpertQuery will have the same name.
    """

    columns = None
    """List of column names."""

    first_index = 0
    """The index of the first row returned.
    """

    time = None
    """Time as
    :class:`PeekTime <omniscript.peektime.PeekTime>`.
    """

    packet_time = None
    """Time of the first packet as
    :class:`PeekTime <omniscript.peektime.PeekTime>`.
    """

    rows = None
    """A list of row data, each item is a dictionary with the key column name
    and the value the value of the item.
    The item values may be: None, string, integer or list.
    Use pare_ip_address() to convert the value into the appropriate type:
    :class:`IPv4Address <omniscript.omniaddress.IPv4Address>` or
    :class:`IPv6Address <omniscript.omniaddress.IPv6Address>`.
    """

    #XML Tags
    _tag_root_name = "result-set"
    _tag_table = "table"
    _tag_columns = "column-list"
    _tag_column = "column"
    _tag_first_index = "first-row-index"
    _tag_row_count = "total-row-count"
    _tag_result = "result-code"
    _tag_rows = "row-list"
    _tag_row = "row"
    _tag_time = "time-wall-clock"
    _tag_packet_time = "time-packet-clock"
    _tag_value = "value"

    def __init__(self, node=None):
        self.table = ExpertResult.table
        self.first_index = ExpertResult.first_index
        self.columns = None
        self.rows = None
        if node is not None:
            self._load(node)

    def _load(self, obj):
        if obj is None: return
        _result = obj.find(ExpertResult._tag_result)
        if _result is not None:
            self.result = int(_result.text)
        _table = obj.find(ExpertResult._tag_table)
        if _table is not None:
            self.table = _table.text
        _time = obj.find(ExpertResult._tag_time)
        if _time is not None:
            self.time = PeekTime(_time.text)
        _packet_time = obj.find(ExpertResult._tag_packet_time)
        if _packet_time is not None:
            self.packet_time = PeekTime(_packet_time.text)
        _count = obj.find(ExpertResult._tag_row_count)
        _row_count = 0 if _count is None else int(_count.text)
        _columns = obj.find(ExpertResult._tag_columns)
        if _columns is not None:
            self.columns = [col.text for col in _columns.findall(ExpertResult._tag_column)]
        if self.columns:
            _rows = obj.find(ExpertResult._tag_rows)
            if _rows is not None:
                self.rows = [{k:_parse_result(v) for k, v in zip(self.columns, row.iter(ExpertResult._tag_value))} for row in _rows.findall('row')]


def _create_expert_result_list(results):
    return None if results is None else [ExpertResult(node=i) for i in results.findall('result-set')]
