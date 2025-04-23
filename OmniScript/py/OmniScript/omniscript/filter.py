"""Filter class.
"""
#Copyright (c) WildPackets, Inc. 2013-2017. All rights reserved.

import os
import xml.etree.ElementTree as ET

import omniscript

from filternode import *
from omniid import OmniId
from peektime import PeekTime


find_attribs = ['name', 'id']
_attrib_props = ['Name', 'ID']


class Filter(object):
    """The Filter class.
    """
    
    color = None
    """The color of the filter."""

    comment = None
    """The filter's comment."""

    created = None
    """When the filter was created as an
    :class:`PeekTime <omniscript.peektime.PeekTime>` object.
    """

    group = None
    """The group that the filter belongs to."""

    id = None
    """The id of the filter. Filter Ids are engine specific."""

    modified = None
    """The last time the filter was modified as an
    :class:`PeekTime <omniscript.peektime.PeekTime>` object.
    """
    name = ''
    """The name of the filter."""

    criteria = None
    """The criteria of the the filter. A hierarchy of
    objectes that are sub-classed from
    :class:`FilterNode <omniscript.filternode.FilterNode>`.
    """

    def __init__(self, name=None, criteria=None):
        self.id = OmniId(True) if criteria is None else Filter.id
        self.name = name
        self.color = Filter.color
        self.comment = Filter.comment
        self.group = Filter.group
        self.created = PeekTime() if criteria is None else Filter.created
        self.modified = PeekTime() if criteria is None else Filter.modified
        self.criteria = Filter.criteria
        if isinstance(criteria, basestring):
            xml = ET.parse(criteria)
            self._load(xml)
        elif isinstance(criteria, ET.Element):
            self._load(criteria)
        elif isinstance(criteria, FilterNode):
            self._load(criteria)

    def __str__(self):
        return 'Filter: %s' % self.name

    def _load(self, element):
        engine = True # Does element contain an Engine Filter?
        filter_obj = element.find('filter')
        if filter_obj is None:
            filter_obj = element.find('filterobj') #from Filter List.
        if filter_obj is not None:
            for attrib in filter_obj.items():
                if attrib[0] == 'id':
                    engine = OmniId.is_id(attrib[1])
                    self.id = OmniId(attrib[1]) if engine else OmniId(True)
                elif attrib[0] == 'color':
                    self.color = int(attrib[1])
                elif attrib[0] == 'comment':
                    self.comment = attrib[1]
                elif attrib[0] == 'created':
                    self.created = PeekTime(attrib[1])
                elif attrib[0] == 'group':
                    self.group = attrib[1]
                elif attrib[0] == 'modified':
                    self.modified = PeekTime(attrib[1])
                elif attrib[0] == 'name':
                    self.name = attrib[1]
            if self.id is None:
                self.id = OmniId(None)
            root_node = filter_obj.find('rootnode')
            if root_node is not None:
                self.criteria = parse_omni_filter(root_node) \
                    if engine else parse_console_filter(root_node)
 
    def _store(self, node):
        _att = {'id':str(self.id)}
        if self.name:
            _att['name'] = str(self.name)
        if self.created:
            _att['created'] = str(self.created)
        if self.modified:
            _att['modified'] = str(self.modified)
        filter_obj = ET.SubElement(node, 'filter', _att)
        if self.comment:
            filter_obj.set('comment', self.comment)
        if self.color is not None:
            filter_obj.set('color', str(self.color))
        if self.criteria is not None:
            store_omni_filter(filter_obj, 'rootnode', self.criteria)

    def to_string(self, pad):
        text = str(self) + "\n"
        if self.criteria:
            operation = "or: " if self.criteria.or_node != None else ""
            text += self.criteria.to_string((pad+1), operation)
        return text

    def to_xml(self, pretty=False):
        """Return the Filter encoded in XML as a string."""
        class_name_ids = omniscript.get_class_name_ids()
        filter_obj = ET.Element('filterobj',
                                {'clsid':str(class_name_ids['Filter'])})
        self._store(filter_obj)
        return ET.tostring(filter_obj).replace('\n', '')


def _create_filter_list(xml_list):
    lst = []
    filters = omniscript._parse_command_response(xml_list, 'filters')
    if filters is not None:
        for obj in filters.findall('filterobj'):
            lst.append(Filter(criteria=obj))
    lst.sort(key=lambda x: x.name)
    return lst


def find_filter(filters, value, attrib=find_attribs[0]):
    """Finds a filter in the list"""
    if (not filters) or (attrib not in find_attribs):
        return None

    if len(filters) == 0:
        return None

    if isinstance(filters[0], Filter):
        return next((i for i in filters if getattr(i, attrib) == value), None)


def read_filter_file(filename):
    """Read filters from a file. The file may be either an Engines filters.xml
    file or Peek's filters.flt file.

    Returns:
        A list of :class:`Filter <omniscript.filter.Filter>` objects.
    """
    lst = []
    xml = ET.parse(filename)
    filters = xml.getroot()
    if filters is not None:
        for obj in filters.findall('filterobj'):
            lst.append(Filter(criteria=obj))
    lst.sort(key=lambda x: x.name)
    return lst


def store_filter_list(engine, lst):
    class_name_ids = omniscript.get_class_name_ids()
    filters = ET.Element('filters')
    for fltr in lst:
        obj = ET.SubElement(filters, 'filterobj',
                            {'clsid':str(class_name_ids['Filter'])})
        fltr._store(obj)
    return filters
