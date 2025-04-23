"""ForensicTemplate class.
"""
#Copyright (c) WildPackets, Inc. 2013-2017. All rights reserved.

import os
import sys
import logging
import uuid

import omniscript
import config

import xml.etree.ElementTree as ET

from analysismodule import AnalysisModule
from capturetemplate import VoIPSettings
from filter import Filter
from forensicfile import ForensicFile
from omniid import OmniId
from peektime import PeekTime
from statslimit import StatsLimit


# XML Tags
_tag_clsid = "clsid"
_tag_compass = "Compass"
_tag_enabled = "enabled"
_tag_alt_enabled = "Enabled"
_tag_id = "id"
_tag_name = "name"
_tag_object = "obj"
_tag_props = "properties"
_tag_prop = "prop"
_tag_type = "type"
_tag_prop_bag = "SimplePropBag"

stats_labels = {
    '_option_break_out':'BreakOutStats',
    'option_expert':'Expert',
    'option_graphs':'Graphs',
    'option_indexing':'Index',
    'option_log':'Log',
    'option_packets':'Packets',
    'option_plugins':'Plugins',
    'option_statistics':'Statistics',
    'option_voice':'Voice',
    '_option_web':'Web'
    }


break_out_stats = {
    'ApplicationStats':'option_application',
    'BreakOutStats':'_option_break_out',
    'CountryStats':'option_country',
    'ErrorStats':'option_error',
    'HistoryStats':'option_history',
    'NetworkStats':'option_network',
    'NodeStats':'option_node',
    'ConversationStats':'option_node_protocol_detail',
    'ProtocolStats':'option_protocol',
    'SizeStats':'option_size',
    'SummaryStats':'option_summary',
    'TopTalkerStats':'option_top_talkers',
    'WirelessChannelStats':'option_wireless_channel',
    'WirelessNodeStats':'option_wireless_node'
    }

break_out_stats_labels = {
    'option_application':'ApplicationStats',
    'option_country':'CountryStats',
    'option_error':'ErrorStats',
    'option_history':'HistoryStats',
    'option_network':'NetworkStats',
    'option_node':'NodeStats',
    'option_node_protocol_detail':'ConversationStats',
    'option_protocol':'ProtocolStats',
    'option_size':'SizeStats',
    'option_summary':'SummaryStats',
    'option_top_talkers':'TopTalkerStats',
    'option_wireless_channel':'WirelessChannelStats',
    'option_wireless_node':'WirelessNodeStats'
    }

limit_types = {
    'none':config.LIMIT_TYPE_NONE,
    'packets':config.LIMIT_TYPE_PACKETS,
    'bytes':config.LIMIT_TYPE_BYTES,
    'buffer':config.LIMIT_TYPE_BUFFER
    }


def _find_properties(template):
    """Find the main Property Bag of the Capture Template."""
    # or next(e for e in template.iter() if e.tag == 'properties')
    props = template.find(_tag_props)
    if props is None:
        #Restart Capture Template File
        props = template.find('CaptureTemplate/properties')
    return props


def _find_property(props, key):
    if props is None:
        return None
    return next((p for p in props if _tag_name in \
        p.attrib and p.attrib[_tag_name] == key), None)


def _from_prop_boolean(value):
    return int(value) != 0


def _set_clsid(obj, clsid_name):
    if not _tag_clsid in obj.attrib:
        class_name_ids = omniscript.get_class_name_ids()
        obj.attrib[_tag_clsid] = str(class_name_ids[clsid_name])


def _set_property_value(parent, value_type, value):
    if not isinstance(value, basestring):
        value = str(value)
    ET.SubElement(parent, _tag_prop,
                  {_tag_type:str(value_type)}).text = value


def _to_prop_boolean(value):
    if isinstance(value, basestring):
        if int(value):
            return PROP_BAG_TRUE
        return PROP_BAG_FALSE
    if value:
        return PROP_BAG_TRUE
    return PROP_BAG_FALSE


class ForensicTemplate(object):
    """Forensic Template class.
    Use a Forensic Template object to create a
    :class:`ForensicSearch <omniscript.forensicsearch.ForensicSearch>`
    object with the function
    :func:`create_forensic_search() 
    <omniscript.omniengine.OmniEngine.create_forensic_search>`.
    """

    adapter_name = ''
    """The name of the search's adapter."""

    capture_name = None
    """The name of the Capture to search.
    Specifiy either a capture_name or file[], not both.
    If neither is specified than all packet files are searched.
    """

    end_time = None
    """The optional ending timestamp, as
    :class:`PeekTime <omniscript.peektime.PeekTime>`
    of the search time span.
    """

    filename = None
    """An optional file name of the Forensic Search Tempalte file."""

    files = []
    """The options list of packet files to search.
    If this list is empty and capture_name is empty, then all packet files
    are search. Do not specifiy capture_name if files is not empty.
    """

    filter = None
    """The 
    :class:`Filter <omniscript.filter.Filter>`
    object of the search.
    Use 
    :func:`find_filter()
    <omniscript.omniengine.OmniEngine.create_forensic_search>`
    to get one of the engine's filters.
    """
    
    filter_mode = config.MODE_ACCEPT_MATCHING
    """The filter mode:
           0 for MODE_ACCEPT_ALL,
           1 for MODE_ACCEPT_MATCHING (default),
           2 for MODE_REJECT_ALL,
           3 for MODE_REJECT_MATCHING
    """

    graph_interval = 0
    """The time interval for graphing when option[graphs] is enabled."""

    limit = config.LIMIT_TYPE_NONE
    """Limit the search by:
           0 for 'None' (default),
           1 for 'Packets', 
           2 for 'Bytes', 
           3 for 'Buffer'.
    """

    limit_size = 0
    """The number of Packets, Bytes or Buffer size in bytes to limit the
    search to."""

    media_type = config.MEDIA_TYPE_802_3
    """The Media Type of the search."""

    media_sub_type = config.MEDIA_SUB_TYPE_NATIVE
    """The Media Sub Type of the search."""

    name = ''
    """The name of the search."""

    node_limits = None
    """The Node Limits a
    :class:`StatsLimit <omniscript.statslimit.StatsLimit>`
    object. Set the option_node attribute to True to enable Node Stats.
    """

    node_protocol_detail_limits = None
    """The Node/Protocol Detail Limits a
    :class:`StatsLimit <omniscript.statslimit.StatsLimit>`
    object. Set the option_node_protocol_detail attribute to True to
    enable Node/Protocol Details Stats. Either Node Stats or Protocol 
    Stats must also be enabled or Node/Protocol Detail Stats will be
    disabled.
    """

    plugins = None
    """The list of Plugin (Analysis Module) Ids.
    A Plugin Id may be a string, 
    :class:`OmniId <omniscript.omniid.OmniId>`
    or an
    :class:`AnalysisModule <omniscript.analysismodule.AnalysisModule>`
    object.
    Call the 
    :func:`get_analysis_module_list() 
    <omniscript.omniengine.OmniEngine.get_analysis_module_list>`
    method to get the engine's list of plugins.
    """

    plugins_config = None
    """The list of Plugins (Analysis Modules) configuration."""

    protocol_limits = None
    """The Protocol Limits a
    :class:`StatsLimit <omniscript.statslimit.StatsLimit>`
    object. Set the option_protocol attribute to enable Protocol Stats.
    """

    session_id = None
    """The session id of the search. Do Not Modify."""

    start_time = None
    """The optional begining timestamp, as
    :class:`PeekTime <omniscript.peektime.PeekTime>`,
    of the search time span.
    """

    voip = None
    """The VoIP settings, as
    :class:`VoIPSettings <omniscript.capturetemplate.VoIPSettings>`,
    """

    option_all_plugins = False
    """Are all plugins enabled?"""

    option_application = False
    """Is the Application Statistics option enabled?"""

    option_compass = False
    """Is Compass enabled?"""

    option_country = False
    """Is the Country Statistics option enabled?"""

    option_error = False
    """Is the Error Statistics option enabled?"""

    option_expert = False
    """Is the Expert Event Statistics option enabled?"""

    option_graphs = False
    """Is the Graphs enabled?"""

    option_history = False
    """Is the Traffic History Statistics option enabled?"""

    option_indexing = True
    """Is the Indexing option enabled?"""

    option_log = False
    """Is the Events Log option enabled?"""

    option_network = False
    """Is the Network Statistics option enabled?"""

    option_node = False
    """Is the Node Statistics option enabled?"""

    option_node_protocol_detail = False
    """Is the Node/Protocol Detail Statistics option enabled?
    Either Node Stats or Protocol Stats must be enabled or Node/Protocol
    Stats will be disabled when the Forensic Search is created.
    """

    option_packets = False
    """Is the Packet option enabled?"""

    option_plugins = False
    """Is the Analysis Modules (Plugins) option enabled?"""

    option_protocol = False
    """Is the Protocol Statistics option enabled?"""

    option_size = False
    """Is the Size Statistics option enabled?"""

    option_statistics = False
    """Is the Statistics option enabled?
    The Statistics option is only enabled by reading in an existing template.
    """

    option_summary = False
    """Is the Summary Statistics option enabled?"""

    option_top_talkers = False
    """Is the Top Talkers option enabled?"""

    option_voice = False
    """Is the Voice and Video Statistics option enabled?"""

    option_wireless_channel = False
    """Is the Wireless Channel Statistics option enabled?"""

    option_wireless_node = False
    """Is the Wireless Node Statistics option enabled?"""

    # Private attributes
    _option_break_out = True
    """Are new options enabled?"""

    _option_web = False
    """ Not implemented on the engine.
    Is the Web Statistics option enabled?
    """

    #XML Tags
    _tag_root_name = "ForensicTemplate"
    _tag_adapter_name = "AdapterName"
    _tag_break_out = "BreakOutStats"
    _tag_capture_name = "Capture"
    _tag_config_plugins = "PluginsConfig"
    _tag_country = "Country"
    _tag_end_time = "EndTime"
    _tag_expert = "Expert"
    _tag_filter = "Filter"
    _tag_filter_mode = "FilterMode"
    _tag_graphs = "Graphs"
    _tag_graph_interval = "GraphsSampleInterval"
    _tag_indexing = "Index"
    _tag_limit = "LimitType"
    _tag_limit_count = "LimitNum"
    _tag_log = "Log"
    _tag_media_type = "MediaType"
    _tag_media_sub_type = "MediaSubType"
    _tag_name = "Name"
    _tag_packets = "Packets"
    _tag_plugin_alt = "plugin"
    _tag_plugins = "Plugins"
    _tag_plugins_alt = "plugins"
    _tag_plugins_clsid = "PropertyList"
    _tag_plugins_list = "PluginsList"
    _tag_plugins_config = "PluginsConfig"
    _tag_prop = "prop"
    _tag_properties = "properties"
    _tag_property_list = "PropertyList"
    _tag_session_id = "CaptureSessionID"
    _tag_single_file = "SingleFile"
    _tag_size = "Size"
    _tag_start_time = "StartTime"
    _tag_stats = "Statistics"
    _tag_summary = "SummaryStats"
    _tag_voice = "Voice"
    _tag_voip = "VoIPConfig"
    _tag_web = "Web"

    def __init__(self, name=None, filename=None, node=None):
        self.adapter_name = ForensicTemplate.adapter_name
        self.capture_name = ForensicTemplate.capture_name
        self.end_time = ForensicTemplate.end_time
        self.filename = filename
        self.files = []
        self.filter = None
        self.filter_mode = ForensicTemplate.filter_mode
        self.graph_interval = ForensicTemplate.graph_interval
        self.limit = ForensicTemplate.limit
        self.limit_size = ForensicTemplate.limit_size
        self.media_type = ForensicTemplate.media_type
        self.media_sub_type = ForensicTemplate.media_sub_type
        self.name = name if name is not None else ForensicTemplate.name
        self.node_limits = StatsLimit('NodeStatsLimitSettings')
        self.node_protocol_detail_limits = \
            StatsLimit('ConversationStatsLimitSettings', limit=200000)
        self.plugins = []
        self.plugins_config = []
        self.protocol_limits = StatsLimit('ProtocolStatsLimitSettings')
        self.session_id = ForensicTemplate.session_id
        self.start_time = ForensicTemplate.start_time
        self.voip = VoIPSettings()
        self.option_all_plugins = ForensicTemplate.option_all_plugins
        self.option_application = ForensicTemplate.option_application
        self.option_compass = ForensicTemplate.option_compass
        self.option_country = ForensicTemplate.option_country
        self.option_error = ForensicTemplate.option_error
        self.option_expert = ForensicTemplate.option_expert
        self.option_graphs = ForensicTemplate.option_graphs
        self.option_history = ForensicTemplate.option_history
        self.option_indexing = ForensicTemplate.option_indexing
        self.option_log = ForensicTemplate.option_log
        self.option_network = ForensicTemplate.option_network
        self.option_node = ForensicTemplate.option_node
        self.option_node_protocol_detail = ForensicTemplate.option_node_protocol_detail
        self.option_packets = ForensicTemplate.option_packets
        self.option_plugins = ForensicTemplate.option_plugins
        self.option_protocol = ForensicTemplate.option_protocol
        self.option_size = ForensicTemplate.option_size
        self.option_statistics = ForensicTemplate.option_statistics
        self.option_summary = ForensicTemplate.option_summary
        self.option_top_talkers = ForensicTemplate.option_top_talkers
        self.option_voice = ForensicTemplate.option_voice
        self.option_wireless_channel = ForensicTemplate.option_wireless_channel
        self.option_wireless_node = ForensicTemplate.option_wireless_node
        self._option_break_out = ForensicTemplate._option_break_out
        self._option_web = False
        if self.filename:
            tree = ET.parse(self.filename)
            root = tree.getroot()
            if root.tag == ForensicTemplate._tag_root_name:
                self._load(root)
        elif node:
            self._load(node)

    def __repr__(self):
        return 'ForensicTemplate: %s' % self.name

    def _load(self, props):
        if props is None:
            return
        for prop in props:
            name = prop.tag
            if name == ForensicTemplate._tag_adapter_name:
                self.adapter_name = prop.text
            elif name == ForensicTemplate._tag_capture_name:
                self.capture_name = prop.text
            elif name == ForensicTemplate._tag_country:
                self.option_country = _from_prop_boolean(prop.text)
            elif name == ForensicTemplate._tag_end_time:
                self.end_time = PeekTime(prop.text)
            elif name == ForensicTemplate._tag_expert:
                self.option_expert = _from_prop_boolean(prop.text)
            elif name == ForensicTemplate._tag_filter:
                self.filter = Filter(criteria=prop)
            elif name == ForensicTemplate._tag_filter_mode:
                self.filter_mode = int(prop.text)
            elif name == ForensicTemplate._tag_graphs:
                self.option_graphs = _from_prop_boolean(prop.text)
            elif name == ForensicTemplate._tag_graph_interval:
                self.graph_interval = int(prop.text)
            elif name == ForensicTemplate._tag_indexing:
                self.option_indexing = _from_prop_boolean(prop.text)
            elif name == ForensicTemplate._tag_limit:
                self.limit = int(prop.text)
            elif name == ForensicTemplate._tag_limit_count:
                self.limit = int(prop.text)
            elif name == ForensicTemplate._tag_log:
                self.option_log = _from_prop_boolean(prop.text)
            elif name == ForensicTemplate._tag_media_type:
                self.media_type = int(prop.text)
            elif name == ForensicTemplate._tag_media_sub_type:
                self.media_sub_type = int(prop.text)
            elif name == ForensicTemplate._tag_name:
                self.name = prop.text
            elif name == ForensicTemplate._tag_packets:
                self.option_packets = _from_prop_boolean(prop.text)
            elif name == ForensicTemplate._tag_plugins:
                self.option_plugins = _from_prop_boolean(prop.text)
            elif name == ForensicTemplate._tag_plugins_list:
                self._load_plugins(prop)
            elif name == ForensicTemplate._tag_session_id:
                self.session_id = OmniId(prop.text) \
                    if prop.text[0] == '{' else None
            elif name == ForensicTemplate._tag_single_file:
                self.single_file = prop.text
            elif name == ForensicTemplate._tag_start_time:
                self.start_time = PeekTime(prop.text)
            elif name == ForensicTemplate._tag_stats:
                self.option_statistics = _from_prop_boolean(prop.text)
            elif name == ForensicTemplate._tag_summary:
                self.option_summary = _from_prop_boolean(prop.text)
            elif name == ForensicTemplate._tag_voice:
                self.option_voice = _from_prop_boolean(prop.text)
            elif name == ForensicTemplate._tag_voip:
                self.voip = VoIPSettings()
                self.voip._load_raw(prop)
            elif name == ForensicTemplate._tag_web:
                # self._option_web = _from_prop_boolean(prop.text)
                self._option_web = False
            elif name == ForensicTemplate._tag_break_out:
                self._option_break_out = _from_prop_boolean(prop.text)
        # Set the break out statistics.
        if self._option_break_out:
            for prop in props:
                name = prop.tag
                if break_out_stats.has_key(name):
                    setattr(self, break_out_stats[name],
                            _from_prop_boolean(prop.text))
            self.node_limits.enabled = self.option_node
            self.protocol_limits.enabled = self.option_protocol
            self.node_protocol_detail_limits.enabled = self.option_node_protocol_detail
        self._option_break_out = True

    def _load_plugins(self, pluginlist):
        self.plugins = []
        props = pluginlist.find(ForensicTemplate._tag_properties)
        if props is not None:
            for prop in props.findall(ForensicTemplate._tag_prop):
                self.plugins.append(OmniId(prop.text))
        compass_id = omniscript.get_class_name_ids()[_tag_compass]
        self.option_compass = compass_id in self.plugins

    def add_file(self, name):
        """Add a file to the search."""
        if isinstance(name, ForensicFile):
            self.files.append(name.path)
        else:
            self.files.append(name)

    def add_plugin(self, plugin):
        """Add a plugin id to the search.
        A plugin id may be a string, 
        :class:`OmniId <omniscript.omniid.OmniId>`
        or an
        :class:`AnalysisModule <omniscript.analysismodule.AnalysisModule>`
        object.
        """
        if isinstance(plugin, basestring):
            if OmniId.is_id(plugin):
                self.plugins.append(OmniId(plugin))
            else:
                self.plugins.append(plugin)
        elif isinstance(plugin, OmniId):
            self.plugins.append(plugin)
        elif isinstance(plugin, AnalysisModule):
            self.plugins.append(plugin.id)
        self.option_plugins = True

    def is_node_limits_enabled(self):
        """Is the Node Statistics option enabled?"""
        if self.node_limits is None:
            return False
        return self.node_limits.enabled

    def is_node_protocol_detail_limits_enabled(self):
        """Is the Node Protocol Statistics option enabled?"""
        if self.node_protocol_detail_limits is None:
            return False
        return self.node_protocol_detail_limits.enabled

    def is_protocol_limits_enabled(self):
        """Is the Protocol Statistics option enabled?"""
        if self.protocol_limits is None:
            return False
        return self.protocol_limits.enabled

    def set_all(self, enable=True):
        """Enable or disable all options."""
        self.set_all_analysis_options(enable)
        self.set_all_output_options(enable)

    def set_all_analysis_options(self, enable=True):
        """Enable or disable all the analysis options."""
        self.node_limits.enabled = enable
        self.node_protocol_detail_limits.enabled = enable
        self.protocol_limits.enabled = enable
        self.option_all_plugins = enable
        self.option_application = enable
        self.option_compass = enable
        self.option_country = enable
        self.option_error = enable
        self.option_expert = enable
        self.option_history = enable
        self.option_network = enable
        self.option_node = enable
        self.option_node_protocol_detail = enable
        self.option_plugins = enable
        self.option_protocol = enable
        self.option_size = enable
        self.option_summary = enable
        self.option_top_talkers = enable
        self.option_voice = enable
        self.option_wireless_channel = enable
        self.option_wireless_node = enable
        self._option_web = False

    def set_all_output_options(self, enable=True):
        """Enable or disable all the output options."""
        self.option_graphs = enable
        self.option_indexing = enable
        self.option_log = enable
        self.option_packets = enable

    def set_limit(self, limit, limit_size):
        """Set a limit on the search.
        
        Args:
            limit (str): either 'none', 'packets', 'bytes' or 'buffer'.
            limit_size (int): packets are in 1k units, bytes and buffer
            in 1MB units.
        """
        self.limit = limit_types[limit]
        if self.limit == config.LIMIT_TYPE_BYTES or self.limit == config.LIMIT_TYPE_BUFFER:
            self.limit_size = limit_size * 1024 * 1024
        elif self.limit == config.LIMIT_TYPE_PACKETS:
            self.limit_size = limit_size * 1024
        else:
            self.limit_size = 0

    def to_xml(self, engine=None):
        """Returns the Forensic Search encoded in XML as a string."""
        if not self._option_break_out:
            for a, p in break_out_stats_labels.iteritems():
                if getattr(self, a):
                    self._option_break_out = True
                    self.option_statistics = False
                    break

        # The Compass hack.
        compass_id = omniscript.get_class_name_ids()[_tag_compass]
        if self.option_compass:
            self.option_plugins = True
            if compass_id not in self.plugins:
                self.plugins.append(compass_id)

        # Sync the stats limits with the options.
        self.option_node_protocol_detail = self.option_node_protocol_detail and \
                                          (self.option_node or self.option_protocol)
        self.node_limits.enabled = self.option_node
        self.node_protocol_detail_limits.enabled = self.option_node_protocol_detail
        self.protocol_limits.enabled = self.option_protocol

        search = ET.Element(ForensicTemplate._tag_root_name)
        if self.files and len(self.files) > 0:
            for f in self.files:
                ET.SubElement(search, ForensicTemplate._tag_single_file).text = f
        ET.SubElement(search, ForensicTemplate._tag_name).text = self.name
        ET.SubElement(search, ForensicTemplate._tag_session_id).text = str(self.session_id if self.session_id else -1)
        ET.SubElement(search, ForensicTemplate._tag_media_type).text = str(self.media_type)
        ET.SubElement(search, ForensicTemplate._tag_media_sub_type).text = str(self.media_sub_type)
        if self.adapter_name:
            ET.SubElement(search, ForensicTemplate._tag_adapter_name).text = self.adapter_name
        else:
            ET.SubElement(search, ForensicTemplate._tag_adapter_name)
        if self.start_time and self.end_time:
            ET.SubElement(search, ForensicTemplate._tag_start_time).text = str(self.start_time)
            ET.SubElement(search, ForensicTemplate._tag_end_time).text = str(self.end_time)
        ET.SubElement(search, ForensicTemplate._tag_filter_mode).text = str(self.filter_mode)
        if self.filter is not None:
            _filter = ET.SubElement(search, ForensicTemplate._tag_filter)
            self.filter._store(_filter)
        
        # options
        for a, p in stats_labels.iteritems():
            ET.SubElement(search, p).text = str(int(getattr(self, a)))

        # break_out indicates new options format.
        if self._option_break_out:
            for a, p in break_out_stats_labels.iteritems():
                ET.SubElement(search, p).text = str(int(getattr(self, a)))
            search.append(self.node_limits.to_xml(1))
            search.append(self.protocol_limits.to_xml(1))
            search.append(self.node_protocol_detail_limits.to_xml(1))

        if self.capture_name and len(self.capture_name) > 0:
            ET.SubElement(search, ForensicTemplate._tag_capture_name).text = self.capture_name
        ET.SubElement(search, ForensicTemplate._tag_limit).text = str(self.limit)
        ET.SubElement(search, ForensicTemplate._tag_limit_count).text = str(self.limit_size)
        ET.SubElement(search, ForensicTemplate._tag_graph_interval).text = str(self.graph_interval)

        if self.option_voice and self.voip:
            self.voip._store_raw(search)

        plugins_id = omniscript.get_class_name_ids()[ForensicTemplate._tag_plugins_clsid]
        plugins = ET.SubElement(search, ForensicTemplate._tag_plugins_list,
                               {"clsid": str(plugins_id)})
        props = ET.SubElement(plugins, ForensicTemplate._tag_properties)
        if self.option_plugins and (self.option_all_plugins or self.plugins):
            module_list = engine.get_analysis_module_list() if engine else []
            _set_clsid(plugins, ForensicTemplate._tag_property_list)
            if self.option_all_plugins:
                for module in module_list:
                    if module.id == compass_id:
                        if self.option_compass:
                            _set_property_value(props, 8, module.id)
                    else:
                        _set_property_value(props, 8, module.id)
            else:
                for plugin in self.plugins:
                    id = None
                    if isinstance(plugin, basestring):
                        if OmniId.is_id(plugin):
                            id = OmniId(plugin)
                        else:
                            module = next((i for i in module_list if getattr(i, 'name') == plugin), None)
                            if module:
                                id = module.id
                    elif isinstance(plugin, OmniId):
                        id = plugin
                    elif isinstance(plugin, AnalysisModule):
                        id = plugin.id
                    if id:
                        if id == compass_id:
                            if self.option_compass:
                                _set_property_value(props, 8, id)
                        else:
                            _set_property_value(props, 8, id)

        return ET.tostring(search).replace('\n', '')
