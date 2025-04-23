"""AnalysisModule class.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

import os
import sys
import xml.etree.ElementTree as ET

import omniscript

from omniid import OmniId


find_attribs = ['name', 'id']

_plugin_prop_dict = {
    'Identifier':'id',
    'Name':'name'
    }


class AnalysisModule(object):
    """The AnalysisModule class has the attributes of an analysis module 
    (plugin). The 
    :func:`get_analysis_module_list() 
    <omniscript.omniengine.OmniEngine.get_analysis_module_list>`
    function returns a list of AnalysisModule objects.
    """

    id = None
    """The analysis module's identifier.
    """

    name = ''
    """The name of the analysis module."""

    version = None
    """The version of the analysis module."""

    def __init__(self, element):
        self.id = OmniId(element.find('CLSID').text)
        self.name = element.find('Name').text
        self.version = element.find('Version').text

    def __repr__(self):
        return 'AnalysisModule: %s' % self.name


def _create_analysis_module_list(engine, xml_list):
    lst = []
    plugins = omniscript._parse_command_response(xml_list, 'Plugins')
    if plugins is not None:
        for plugin in plugins.findall('Plugin'):
            lst.append(AnalysisModule(plugin))
    lst.sort(key=lambda x: x.name)
    return lst


def find_analysis_module(analysis_modules, value, attrib=find_attribs[0]):
    """Finds an analysis module in the list"""
    if (not analysis_modules) or (attrib not in find_attribs):
        return None

    if len(analysis_modules) == 0:
        return None

    if isinstance(value, AnalysisModule):
        _value = value.id
        attrib = 'id'
    else:
        _value = value
    
    return next((i for i in analysis_modules if getattr(i, attrib) == _value), None)
