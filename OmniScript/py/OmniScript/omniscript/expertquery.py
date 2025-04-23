"""ExpertQuery class.
"""
#Copyright (c) Savvius, Inc. 2013-2017. All rights reserved.

import os
import sys
import logging
import xml.etree.ElementTree as ET

import config
import omniscript


def _to_bool(value):
    return "1" if value else "0"


class ExpertViewSettings(object):
    #XML Tags
    _tag_root_name = "view-settings"
    _tag_color = "color-by"
    _tag_direction_left = "la"
    _tag_direction_right = "ra"
    _tag_direction_both = "ba"
    _tag_time_precision = "timestamp-precision"
    _tag_units = "throughput-units"
    _tag_option_address_names = "show-address-names"
    _tag_option_port_names = "show-port-names"
    _tag_option_grid = "suppress-grid"
    _tag_option_utc = "utc"

    def __init__(self):
        self.color = 'independent'
        self.direction_labels = ['<--', '-->', '<->']
        self.units = 'bits/sec'
        self.time_precision = 'microseconds'
        self.option_address_names = True
        self.option_port_names = True
        self.option_grid = True
        self.option_utc = False

    def _store(self, obj):
        node = ET.SubElement(obj, ExpertViewSettings._tag_root_name)
        ET.SubElement(node, ExpertViewSettings._tag_time_precision).text = self.time_precision
        ET.SubElement(node, ExpertViewSettings._tag_units).text = self.units
        ET.SubElement(node, ExpertViewSettings._tag_color).text = self.color
        ET.SubElement(node, ExpertViewSettings._tag_option_address_names).text = _to_bool(self.option_address_names)
        ET.SubElement(node, ExpertViewSettings._tag_option_port_names).text = _to_bool(self.option_port_names)
        ET.SubElement(node, ExpertViewSettings._tag_option_grid).text = _to_bool(not self.option_grid)
        ET.SubElement(node, ExpertViewSettings._tag_option_utc).text = _to_bool(self.option_utc)
        ET.SubElement(node, ExpertViewSettings._tag_direction_left).text = self.direction_labels[config.DIRECTION_TO_LEFT]
        ET.SubElement(node, ExpertViewSettings._tag_direction_right).text = self.direction_labels[config.DIRECTION_TO_RIGHT]
        ET.SubElement(node, ExpertViewSettings._tag_direction_both).text = self.direction_labels[config.DIRECTION_BOTH]


class ExpertQuery(object):
    """Query to Engine's Expert.
    Pass in one or more ExpertQuery objects into
    :func:`query_expert() <omniscript.capture.Capture.query_expert>`.
    It will return a list of
    :class:`ExpertResult <omniscript.expertresult.ExpertResult>` objects.
    See :ref:`expert-tables-section` section for a list of table and
    column names.
    """

    table = ''
    """The table being queried.
    The resulting list of
    :class:`ExpertResult <omniscript.expertresult.ExpertResult>`
    will have the same 'table' name.
    """

    columns = None
    """The list of column names in the resulting ExpertResult."""

    order = None
    """The order the columns in the resulting ExpertResult
    is to be ordered.
    """

    where = None
    """A list of strings of the Severity levels to include:
    'informational', 'minor', 'major' or 'severe'.
    See the :ref:`query-expert-tutorial` tutorial.
    """

    first_index = 0
    """The index of the first of n records to return."""

    row_count = 100
    """The maximum number of rows to return."""

    view_settings = None
    """The View Settings of the Query."""

    options_scroll_back = False
    """Is scroll back enabled."""
    
    #XML Tags
    _tag_root_name = "query"
    _tag_table = "table"
    _tag_columns = "column-list"
    _tag_column = "column"
    _tag_criteria = "criteria"
    _tag_order = "order-by"
    _tag_row_count = "limit-row-count"
    _tag_tree_expand = "tree-expand"
    _tag_first_index = "first-row-index"
    _tag_scroll_back = "scroll-back"
    _tag_severity = "severity"
    _tag_view_settings = "view-settings"
    _tag_where = "where"

    def __init__(self, table=None, node=None):
        self.table = ExpertQuery.table if table is None else table
        self.columns = []
        self.order = None
        self.where = None
        self.first_index = ExpertQuery.first_index
        self.row_count = ExpertQuery.row_count
        self.view_settings = ExpertViewSettings()
        self.options_scroll_back = ExpertQuery.options_scroll_back
        if node is not None:
            self._load(node)

    def _load(self, obj):
        if obj is None: return
        _table = obj.find(ExpertQuery._tag_table)
        if _table is not None:
            self.table = _table.text
        _columns = obj.find(ExpertQuery._tag_columns)
        if _columns is not None:
            self.columns = [col.text for col in _columns.findall(ExpertQuery._tag_column)]
        _orders = obj.find(ExpertQuery._tag_order)
        if _orders is not None:
            self.order = [col.text for col in _orders.findall(ExpertQuery._tag_column)]
        _where = obj.find(ExpertQuery._tag_where)
        if _where is not None:
            pass
        _first = obj.find(ExpertQuery._tag_first_index)
        if _first is not None:
            self.first_index = int(_first.text)

    def _store(self, obj):
        if obj is None: return
        query = ET.SubElement(obj, ExpertQuery._tag_root_name)
        ET.SubElement(query, ExpertQuery._tag_table).text = self.table
        _columns = ET.SubElement(query, ExpertQuery._tag_columns)
        for c in self.columns:
            ET.SubElement(_columns, ExpertQuery._tag_column).text = c
        if self.order:
            _orders = ET.SubElement(query, ExpertQuery._tag_order, {'ascending':'1'})
            for c in self.order:
                ET.SubElement(_orders, ExpertQuery._tag_column).text = c
        if self.where:
            _where = ET.SubElement(query, ExpertQuery._tag_where,
                                   {ExpertQuery._tag_criteria:ExpertQuery._tag_severity})
            for w in self.where:
                ET.SubElement(_where, ExpertQuery._tag_severity).text = w
        ET.SubElement(query, ExpertQuery._tag_row_count).text = str(self.row_count)
        ET.SubElement(query, ExpertQuery._tag_first_index).text = str(self.first_index)
        ET.SubElement(query, ExpertQuery._tag_scroll_back).text = _to_bool(self.options_scroll_back)
        ET.SubElement(query, ExpertQuery._tag_tree_expand, {'default-state':'collapse-all'})
        self.view_settings._store(query)

    def to_xml(self):
        root = ET.Element('root')
        self._store(root)
        return ET.tostring(root.getroot()).replace('\n', '')


def _create_expert_query_list(querys):
    return None if querys is None else [ExpertQuery(node=i) for i in querys.findall('query')]
