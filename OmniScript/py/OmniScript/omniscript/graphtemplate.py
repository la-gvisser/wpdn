"""GraphTemplate class.
"""
#Copyright (c) WildPackets, Inc. 2013-2015. All rights reserved.

import os
import sys
import logging
import xml.etree.ElementTree as ET

import omniscript

from omniid import OmniId


find_attribs = ['name', 'id']


class GraphTemplate(object):
    """The GraphTemplate class has the attributes of an Graph Template.
    The 
    :func:`get_graph_template_list() 
    <omniscript.omniengine.OmniEngine.get_graph_template_list>`
    function returns a list of GraphTemplate objects.
    """

    id = OmniId()
    """The graph template's identifier."""

    name = ''
    """The name of the graph template."""

    description = ''
    """The description of the graph template."""

    graph_id = OmniId()
    """The graph identifier of the graph template."""

    start = 0
    """The start value"""

    sample_interval = 0
    """The sample interval"""

    sample_count = 0
    """The sample count"""

    def __init__(self, criteria):
        self.id = OmniId(True) if criteria is None else GraphTemplate.id
        self.name = GraphTemplate.name
        self.description = GraphTemplate.description
        self.graph_id = OmniId(True) if criteria is None else GraphTemplate.graph_id
        self.start = GraphTemplate.start
        self.sample_interval = GraphTemplate.sample_interval
        self.sample_count = GraphTemplate.sample_count
        if isinstance(criteria, basestring):
            xml = ET.parse(criteria)
            self._load(xml)
        elif isinstance(criteria, ET.Element):
            self._load(criteria)

    def __repr__(self):
        return 'GraphTemplate: %s' % self.name

    def _load(self, element):
        graph = element.find('simplegraph')
        if graph is not None:
          details = graph.find('details')
          if details is not None:
            for attrib in details.items():
                if attrib[0] == 'templateid':
                    self.id = OmniId(attrib[1])
                elif attrib[0] == 'title':
                    self.name = attrib[1]
                elif attrib[0] == 'desc':
                    self.description = attrib[1]
                elif attrib[0] == 'id':
                    self.graph_id =  OmniId(attrib[1])
                elif attrib[0] == 'start':
                    self.start = int(attrib[1])
                elif attrib[0] == 'sample-int':
                    self.sample_interval = int(attrib[1])
                elif attrib[0] == 'sample-count':
                    self.sample_count = int(attrib[1])
            if self.id is None:
                self.id = OmniId(None)


def _create_graph_template_list(xml_list):
    lst = []
    graphs = omniscript._parse_command_response(xml_list, 'graph-collection')
    if graphs is not None:
        for template in graphs.findall('graph-template'):
            lst.append(GraphTemplate(criteria=template))
    lst.sort(key=lambda x: x.name)
    return lst


def find_graph_template(graphs, value, attrib=find_attribs[0]):
    """Finds a graph_template in the list"""
    if (not graphs) or (attrib not in find_attribs):
        return []

    if len(graphs) == 0:
        return []

    if isinstance(value, GraphTemplate):
        _value = value.id
        attrib = 'id'
    else:
        _value = value

    return next((i for i in graphs if getattr(i, attrib) == _value), [])
