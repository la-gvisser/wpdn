"""CaptureTemplate class.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

import os
import sys
import logging
import xml.etree.ElementTree as ET

import omniscript
import config

from adapter import Adapter
from alarm import Alarm
from fileadapter import FileAdapter
from filter import Filter
from graphtemplate import GraphTemplate
from omniid import OmniId
from omnierror import OmniError
from peektime import PeekTime
from statslimit import StatsLimit

from alarm import find_alarm
from filter import find_filter
from graphtemplate import find_graph_template

LOAD_FROM_NONE = 0
LOAD_FROM_FILE = 1
LOAD_FROM_NODE = 2

PROP_BAG_TRUE = '1'
PROP_BAG_FALSE = '0'

SECONDS_IN_A_MINUTE = 60
MINUTES_IN_A_HOUR = 60
SECONDS_IN_A_HOUR = SECONDS_IN_A_MINUTE * MINUTES_IN_A_HOUR
HOURS_IN_A_DAY = 24
SECONDS_IN_A_DAY = SECONDS_IN_A_HOUR * HOURS_IN_A_DAY

BYTES_IN_A_KILOBYTE = 1024
BYTES_IN_A_MEGABYTE = 1024 * 1024
BYTES_IN_A_GIGABYTE = 1024 * 1024 * 1024

GRAPHS_INTERVAL_SECONDS = 1
GRAPHS_INTERVAL_MINUTES = 2
GRAPHS_INTERVAL_HOURS = 3
GRAPHS_INTERVAL_DAYS = 4

find_attributes = ['name', 'id']

interval_multiplier = [0, 1, SECONDS_IN_A_MINUTE, SECONDS_IN_A_HOUR, SECONDS_IN_A_DAY]
interval_labels = ['None', 'seconds', 'minutes', 'hours', 'days']

# XML Tags
_tag_clsid = "clsid"
_tag_enabled = "enabled"
_tag_alt_enabled = "Enabled"
_tag_id = "id"
_tag_name = "name"
_tag_object = "obj"
_tag_props = "properties"
_tag_prop = "prop"
_tag_type = "type"
_tag_prop_bag = "SimplePropBag"
_tag_compass = "Compass"

def _to_interval_units(seconds):
    if (seconds % SECONDS_IN_A_DAY) == 0:
        return ((seconds / SECONDS_IN_A_DAY), GRAPHS_INTERVAL_DAYS)
    if (seconds % SECONDS_IN_A_HOUR) == 0:
        return ((seconds / SECONDS_IN_A_HOUR), GRAPHS_INTERVAL_HOURS)
    if (seconds % SECONDS_IN_A_MINUTE) == 0:
        return ((seconds / SECONDS_IN_A_MINUTE), GRAPHS_INTERVAL_MINUTES)
    return (seconds, GRAPHS_INTERVAL_SECONDS)


def _from_prop_boolean(value):
    return int(value) != 0


def _to_prop_boolean(value):
    if isinstance(value, basestring):
        if int(value):
            return PROP_BAG_TRUE
        return PROP_BAG_FALSE
    if value:
        return PROP_BAG_TRUE
    return PROP_BAG_FALSE


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


def _is_attribute_enabled(prop, attrib):
    """Return if an attribute of a node is aa non-zero value."""
    return int(prop.attrib.get(attrib, '0')) != 0


def _is_prop_enabled(prop):
    """Return if a prop node has an enabled attribute set to a non-zero
    value.
    """
    return int(prop.attrib.get('enabled', '0')) != 0


def _set_property(parent, key, value_type, value):
    if not isinstance(value, basestring):
        value = str(value)
    prop = _find_property(parent, key)
    if prop is not None:
        prop.text = value
        return
    if key:
        ET.SubElement(parent, _tag_prop,
                      {_tag_name:key, _tag_type:str(value_type)}).text = value
    else:
        ET.SubElement(parent, _tag_prop,
                      {_tag_type:str(value_type)}).text = value


def _set_label_clsid(obj, label, clsid_name):
    if not _tag_clsid in obj.attrib:
        class_name_ids = omniscript.get_class_name_ids()
        obj.attrib[_tag_clsid] = str(class_name_ids[clsid_name])
    if not _tag_name in obj.attrib:
        obj.attrib[_tag_name] = label


class CaptureLimit(object):
    """The Capture Limit of a Trigger Event Object.
    """

    bytes = 0
    """The number of bytes needed to trigger the Trigger."""

    enabled = False
    """Is the Capture Limit enabled?"""

    #XML Tags
    _tag_root_name = "triggerevent"
    _tag_bytes = "bytescaptured"

    def __init__(self):
        self.bytes = CaptureLimit.bytes
        self.enabled = CaptureLimit.enabled

    def _load(self, obj):
        trigger = obj.find(CaptureLimit._tag_root_name)
        if trigger is not None:
            self.enabled = _is_prop_enabled(trigger)
            self.bytes = trigger.attrib.get(CaptureLimit._tag_bytes, '0')

    def _store(self, obj):
        ET.SubElement(obj, CaptureLimit._tag_root_name,
                      {_tag_enabled : _to_prop_boolean(self.enabled),
                      CaptureLimit._tag_bytes : str(self.bytes)})


class DateLimit(object):
    """The DateLimit of a Trigger Event Object."""

    enabled = False
    """Is the Date Limit enabled?"""

    time = None
    """The timestamp of the Date Limit, as
    :class:`PeekTime <omniscript.peektime.PeekTime>`,
    When option_use_elapsed is true create a PeekTime with the time
    in nanoseconds. PeekTime(2 * 1000000000) is 2 seconds.
    """

    option_use_date = False
    """Use the date of the time?"""

    option_use_elapsed = False
    """Use the elapsed time?"""

    #XML Tags
    _tag_root_name = "triggerevent"
    _tag_time = "time"
    _tag_use_date = "usedate"
    _tag_use_elapsed = "useelapsedtime"

    def __init__(self):
        self.enabled = DateLimit.enabled
        self.time = PeekTime(0)
        self.option_use_date = DateLimit.option_use_date
        self.option_use_elapsed = DateLimit.option_use_elapsed

    def _load(self, obj):
        trigger = obj.find(DateLimit._tag_root_name)
        if trigger is not None:
            self.enabled = _is_prop_enabled(trigger)
            self.time = PeekTime(trigger.attrib.get(DateLimit._tag_time, '0'))
            self.option_use_date = _is_attribute_enabled(
                trigger, DateLimit._tag_use_date)
            self.option_use_elapsed = _is_attribute_enabled(
                trigger, DateLimit._tag_use_elapsed)

    def _store(self, obj):
        if not isinstance(self.time, PeekTime):
            self.time = PeekTime(self.time)
        ET.SubElement(obj, DateLimit._tag_root_name,
                      {_tag_enabled : _to_prop_boolean(self.enabled),
                       DateLimit._tag_use_date : _to_prop_boolean(self.option_use_date),
                       DateLimit._tag_use_elapsed : \
                           _to_prop_boolean(self.option_use_elapsed),
                       DateLimit._tag_time : str(self.time.value)})


class FilterLimit(object):
    """The Filter Limit of a Trigger Event Object.
    """

    enabled = False
    """Is the Date Limit enabled?"""

    filters = []
    """A FilterPack of the filters."""

    mode = 0
    """The filtering mode of the Filters."""

    _filter_ids = []

    #XML Tags
    _tag_clsid_name = "FilterPack"
    _tag_root_name = "triggerevent"
    _tag_filter_obj = "packetfilterobj"
    _tag_filter_pack = "filterpack"
    _tag_item = "item"
    _tag_mode = "mode"

    def __init__(self):
        self.enabled = FilterLimit.enabled
        self.filters = []
        self.mode = FilterLimit.mode
        self._filter_ids = []

    def _load(self, obj, engine):
        trigger = obj.find(FilterLimit._tag_root_name)
        if trigger is not None:
            self.enabled = _is_prop_enabled(trigger)
            self.mode = int(trigger.attrib.get(FilterLimit._tag_mode, '0'))
            filter_obj = trigger.find(FilterLimit._tag_filter_obj)
            if filter_obj is not None:
                filter_pack = filter_obj.find(FilterLimit._tag_filter_pack)
                if filter_pack is not None:
                    self._filter_ids = []
                    self.filters = []
                    items = filter_pack.findall(FilterLimit._tag_item)
                    for item in items:
                        id = OmniId(item.attrib[_tag_id])
                        self._filter_ids.append(id)
                        if engine is not None:
                            filter_list = engine.get_filter_list()
                            fltr = find_filter(filter_list, id, _tag_id)
                            if fltr:
                                self.filters.append(fltr.name)

    def _store(self, obj, engine):
        class_name_ids = omniscript.get_class_name_ids()
        _filter_pack_class_id = class_name_ids[FilterLimit._tag_clsid_name]
        if not _tag_clsid in obj.attrib:
            obj.attrib[_tag_clsid] = str(
                class_name_ids[FilterLimit._tag_clsid_name])

        trigger = ET.SubElement(obj, FilterLimit._tag_root_name,
                                {_tag_enabled : _to_prop_boolean(self.enabled),
                                 FilterLimit._tag_mode : str(self.mode)})
        filter_obj = ET.SubElement(trigger, FilterLimit._tag_filter_obj,
                                   {_tag_clsid : str(_filter_pack_class_id)})
        filter_pack = ET.SubElement(filter_obj, FilterLimit._tag_filter_pack)
        if (len(self.filters) > 0) and engine is not None:
            filter_list = engine.get_filter_list()
            for name in self.filters:
                fltr = find_filter(filter_list, name, _tag_name)
                if fltr:
                    ET.SubElement(filter_pack, FilterLimit._tag_item,
                                  {_tag_id : str(fltr.id)})
        else:
            for id in self._filter_ids:
                ET.SubElement(filter_pack, FilterLimit._tag_item,
                              {_tag_id : str(id)}) 


class AdapterSettings(object):
    """The Adapter Settings section of a
    :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`
    """

    device_name = None
    """The device name of the adapter: Broadcom, Intel..."""

    limit = 0
    """The number of times to replay a file adapter: 0 for infininte."""

    mode = 0
    """The timestamping mode of a file adapter."""

    name = None
    """The name (or description) of the adapter."""

    speed = 1.0
    """The replay speed multiplier of a file adapter."""

    adapter_type = config.ADAPTER_TYPE_NIC
    """The type of adapter. One of the ADAPTER TYPE constants."""

    #XML Tags
    _tag_label = "AdapterSettings"
    _tag_clsid_name = _tag_prop_bag
    _tag_root_name = _tag_props
    _tag_as_name = "Name"
    _tag_as_type = "Type"
    _tag_enumerator = "Enumerator"
    _tag_limit = "ReplayLimit"
    _tag_mode = "ReplayTimeStampMode"
    _tag_speed = "ReplaySpeed"

    def __init__(self):
        self.device_name = AdapterSettings.device_name
        self.limit = AdapterSettings.limit
        self.mode = AdapterSettings.mode
        self.name = AdapterSettings.name
        self.speed = AdapterSettings.speed
        self.adapter_type = AdapterSettings.adapter_type

    def __repr__(self):
        return 'AdapterSettings: %s' % self.name

    @property
    def description(self):
        """Return the name as the description of the adapter."""
        return self.name

    def _load(self, obj):
        """Load the Adapter Settings from an ETree SubElement."""
        props = obj.find(AdapterSettings._tag_root_name)
        prop_type = _find_property(props, AdapterSettings._tag_as_type)
        if prop_type is not None:
            self.adapter_type = int(prop_type.text)
            if self.adapter_type == config.ADAPTER_TYPE_FILE:
                _find_property(props, AdapterSettings._tag_limit)
                _find_property(props, AdapterSettings._tag_speed)
                _find_property(props, AdapterSettings._tag_mode)

        prop = _find_property(props, AdapterSettings._tag_as_name)
        if prop is not None:
            self.name = prop.text

    def _store(self, obj, engine):
        """Store the Adapter Settings into the ETree SubElement"""
        _set_label_clsid(obj, AdapterSettings._tag_label,
                        AdapterSettings._tag_clsid_name)
        props = obj.find(AdapterSettings._tag_root_name)
        if props is None:
            props = ET.SubElement(obj, _tag_props)

        name = None
        prop_name = _find_property(props, AdapterSettings._tag_as_name)
        if prop_name is not None:
            name = prop_name.text
        if self.name is not None and len(self.name) > 0:
            name = self.name

        if engine is not None:
            if self.adapter_type == config.ADAPTER_TYPE_FILE:
                enum = _find_property(props, AdapterSettings._tag_enumerator)
                if enum is not None:
                    props.remove(enum)
                _set_property(props, AdapterSettings._tag_as_name, 8, name)
                _set_property(props, AdapterSettings._tag_limit, 19, self.limit)
                _set_property(props, AdapterSettings._tag_mode, 3, self.mode)
                _set_property(props, AdapterSettings._tag_speed, 8, self.speed)
                _set_property(props, AdapterSettings._tag_as_type, 3, self.adapter_type)
            else:
                fa_prop_names = [AdapterSettings._tag_limit,
                                 AdapterSettings._tag_mode,
                                 AdapterSettings._tag_speed]
                for pn in fa_prop_names:
                    p = _find_property(props, pn)
                    if p is not None:
                        props.remove(p)
                adapter = None
                if name is not None:
                    adapter = engine.find_adapter(name)
                elif self.device_name is not None:
                    adapter = engine.find_adapter(self.device_name, 'device_name')

                if adapter is None:
                    raise OmniError('Adapter not found')

                _set_property(props, AdapterSettings._tag_enumerator, 8, adapter.id)
                _set_property(props, AdapterSettings._tag_as_name, 8, adapter.name)
                _set_property(props, AdapterSettings._tag_as_type, 3, adapter.adapter_type)
        elif self.name is not None:
            _set_property(props, AdapterSettings._tag_as_name, 8, self.name)


class AlarmSettings(object):
    """The Alarm Settings section of a
    :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`
    """

    alarms = []
    """A list of Alarms.
    :class:`Alarm <omniscript.alarm.Alarm>` or
    :class:`OmniId <omniscript.omniid.OmniId>`
    """

    #XML Tags
    _tag_label = "AlarmConfig"
    _tag_clsid_name = "AlarmConfig"
    _tag_config = "Alarmconfig"
    _tag_alarm = "Alarm"
    _tag_alarms = "Alarms"

    def __init__(self):
        self.alarms = []

    def _load(self, obj, engine=None):
        """Load the Alarm from an ETree.SubElement."""
        _config = obj.find(AlarmSettings._tag_config)
        if _config is not None:
            alarm_list = None
            if engine is not None:
                alarm_list = engine.get_alarm_list()
            alarms = _config.find(AlarmSettings._tag_alarms)
            for alarm in alarms.findall(AlarmSettings._tag_alarm):
                id = OmniId(alarm.attrib[_tag_id])
                _alarm = None
                if alarm_list is not None:
                    _alarm = find_alarm(alarm_list, id)
                if _alarm is not None:
                    self.alarms.append(_alarm)
                else:
                    self.alarms.append(id)

    def _store(self, obj):
        """Store the Alarms into the ETree.SubElement."""
        if not self.alarms:
            return
        if 'null' in obj.attrib:
            del obj.attrib['null']
        _set_label_clsid(obj, AlarmSettings._tag_label,
                         AlarmSettings._tag_clsid_name)
        alarm_config = obj.find(AlarmSettings._tag_config)
        if alarm_config is not None:
            obj.remove(alarm_config)
            alarm_config = None
        alarm_config = ET.SubElement(obj, AlarmSettings._tag_config)
        alarms = ET.SubElement(alarm_config, AlarmSettings._tag_alarms)
        for alarm in self.alarms:
            if isinstance(alarm, Alarm):
                id_string = str(alarm.id)
            elif isinstance(alarm, OmniId):
                id_string = str(alarm)
            else:
                id_string = str(alarm)
            ET.SubElement(alarms, AlarmSettings._tag_alarm,
                          {_tag_id:id_string})


class AnalysisModules(object):
    """The Analysis Module section of a
    :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`
    All the Analysis Modules installed on the engine and listed in 
    modules by name and in ids listed by
    :class:`OmniId <omniscript.omniid.OmniId>` will be enabled in the capture.
    """

    modules = []
    """A list of Analysis Module Names."""

    ids = []
    """A list of :class:`OmniId <omniscript.omniid.OmniId>` of enabled
    Analysis Modules.
    """

    option_enable_all = False
    """When True all of the engine's Analysis Modules will be enabled.
    """

    #XML Tags
    _tag_label = "PluginsList"
    _tag_clsid_name = "PropertyList"
    _tag_root_name = _tag_props

    def __init__(self):
        self.modules = []
        self.ids = []
        self.option_enable_all = AnalysisModules.option_enable_all

    def _load(self, obj, engine):
        """Load the list of Analysis Modules from an ETree.SubElement."""
        self.modules = []

        if 'null' in obj.attrib:
            null_enabled = obj.attrib['null']
            if null_enabled:
                return

        """ Build Analysis Module list """
        id_class_names = omniscript.get_id_class_names()
        module_list = engine.get_analysis_module_list() if engine else []

        props = obj.find(AnalysisModules._tag_root_name)
        if props is not None:
            for prop in props.findall(_tag_prop):
                if OmniId.is_id(prop.text):
                    id = OmniId(prop.text)
                    module_name = None
                    if module_list:
                        module = next((i for i in module_list if getattr(i, 'id') == id), None)
                        if module is not None:
                            module_name = module.name
                    if not module_name:
                        if id in id_class_names:
                            module_name = id_class_names[id]
                    if module_name:
                        if module_name not in self.modules:
                            self.modules.append(module_name)
                        else:
                            self.ids.append(id)
                else:
                    if prop.text not in self.modules:
                        self.modules.append(prop.text)

    def _store(self, obj, engine, compass=False):
        """Store the Analysis Module list into the ETree.SubElement."""
        _compass = compass
        module_list = engine.get_analysis_module_list() if engine else []
        if not next((i for i in module_list if getattr(i, 'name') == _tag_compass), None):
            _compass = False

        if 'null' in obj.attrib:
            del obj.attrib['null']
        _set_label_clsid(obj, AnalysisModules._tag_label,
                         AnalysisModules._tag_clsid_name)
        props = obj.find(AnalysisModules._tag_root_name)
        if props is not None:
            obj.remove(props)
            props = None
        props = ET.SubElement(obj, AnalysisModules._tag_root_name)
        _ids = self.ids
        _modules = self.modules
        # Compass as an Analysis Option hack.
        if _compass and _tag_compass not in _modules:
            _modules.append(_tag_compass)
        if not _compass and _tag_compass in _modules:
            _modules.remove(_tag_compass)
        if engine is not None:
            if self.option_enable_all:
                for module in module_list:
                    _set_property(props, None, 8, str(module.id))
            else:
                for name in self.modules:
                    module = next((i for i in module_list if getattr(i, 'name') == name), None)
                    if module is not None:
                        if module.id not in _ids:
                            _ids.append(module.id)
                for id in _ids:
                    _set_property(props, None, 8, str(id))
        else:
            for name in self.modules:
                _set_property(props, None, 8, name)

    def set_all(self, enable=True):
        """Enable or disable enabeling all of the Analysis Modules on the Engine."""
        self.option_enable_all = enable


class AnalysisSettings(object):
    """The Analysis Settings section of a
    :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`
    """

    node_limit = None
    """The Node Statistics settings."""

    node_protocol_detail_limit = None
    """The Node and Protocol Detail Statistics settings."""

    protocol_limit = None
    """The Protocol Statistics settings."""

    option_alarms = False
    """Is the Alarms option enabled?"""

    option_analysis_modules = False
    """Is the Analysis Module option enabled?"""

    option_application = False
    """Is the Application Statistics option enabled?"""

    option_compass = False
    """Is Compass enabled?"""

    option_country = False
    """Is the Country Statistics option enabled?"""

    option_error = False
    """Is the Error Statistics option enabled?"""

    option_expert = False
    """Is the Expert Analysis option enabled?"""

    option_network = False
    """Is the Network Statistics option enabled?"""

    option_size = False
    """Is the Size Statistics option enabled?"""

    option_summary = False
    """Is the Summary Statistics option enabled?"""

    option_top_talker = False
    """Is the Traffic History Statistics option enabled?
    This option may be unsupported.
    """

    option_traffic = False
    """Is the Traffic History Statistics option enabled?"""

    option_voice_video = False
    """Is the Voice and Video Analysis option enabled?"""

    #XML Tags
    _tag_label = "PerfConfig"
    _tag_clsid_name = _tag_prop_bag
    _tag_root_name = _tag_props
    _tag_alarms = "Alarms"
    _tag_analysis_modules = "Analysis Modules"
    _tag_application_stats = "Application Statistics"
    _tag_country_stats = "Country Statistics"
    _tag_detail_limit = "Node/Protocol Detail Statistics"
    _tag_error_stats = "Error Statistics"
    _tag_expert = "Expert Analysis"
    _tag_network_stats = "Network Statistics"
    _tag_node_limit = "Node Statistics"
    _tag_protocol_limit = "Protocol Statistics"
    _tag_size_stats = "Size Statistics"
    _tag_summary_stats = "Summary Statistics"
    _tag_top_talker_stats = "Top Talker Statistics"
    _tag_traffic_stats = "Traffic History Statistics"
    _tag_voice_video = "Voice and Video Analysis"

    def __init__(self):
        self.node_limit = StatsLimit(AnalysisSettings._tag_node_limit)
        self.node_protocol_detail_limit = \
            StatsLimit(AnalysisSettings._tag_detail_limit, limit=500000)
        self.protocol_limit = StatsLimit(AnalysisSettings._tag_protocol_limit)
        self.option_alarms = AnalysisSettings.option_alarms
        self.option_analysis_modules = AnalysisSettings.option_analysis_modules
        self.option_application = AnalysisSettings.option_application
        self.option_compass = AnalysisSettings.option_compass
        self.option_country = AnalysisSettings.option_country
        self.option_error = AnalysisSettings.option_error
        self.option_expert = AnalysisSettings.option_expert
        self.option_network = AnalysisSettings.option_network
        self.option_size = AnalysisSettings.option_size
        self.option_summary = AnalysisSettings.option_summary
        self.option_top_talker = AnalysisSettings.option_top_talker
        self.option_traffic = AnalysisSettings.option_traffic
        self.option_voice_video = AnalysisSettings.option_voice_video

    def _load(self, obj, engine=None):
        """Load the Analysis Settings from an ETree.SubElement."""
        props = obj.find(AnalysisSettings._tag_root_name)
        if props is not None:
            for obj in props.findall(_tag_object):
                objName = obj.attrib[_tag_name]
                if objName == AnalysisSettings._tag_node_limit:
                    self.node_limit.parse(obj)
                elif objName == AnalysisSettings._tag_detail_limit:
                    self.node_protocol_detail_limit.parse(obj)
                elif objName == AnalysisSettings._tag_protocol_limit:
                    self.protocol_limit.parse(obj)
            for prop in props.findall(_tag_prop):
                propName = prop.attrib[_tag_name]
                if propName == AnalysisSettings._tag_alarms:
                    self.option_alarms = _from_prop_boolean(prop.text)
                elif propName == AnalysisSettings._tag_analysis_modules:
                    self.option_analysis_modules = _from_prop_boolean(prop.text)
                elif propName == AnalysisSettings._tag_application_stats:
                    self.option_application = _from_prop_boolean(prop.text)
                elif propName == _tag_compass:
                    self.option_compass = _from_prop_boolean(prop.text)
                elif propName == AnalysisSettings._tag_country_stats:
                    self.option_country = _from_prop_boolean(prop.text)
                elif propName == AnalysisSettings._tag_error_stats:
                    self.option_error = _from_prop_boolean(prop.text)
                elif propName == AnalysisSettings._tag_expert:
                    self.option_expert = _from_prop_boolean(prop.text)
                elif propName == AnalysisSettings._tag_network_stats:
                    self.option_network = _from_prop_boolean(prop.text)
                elif propName == AnalysisSettings._tag_size_stats:
                    self.option_size = _from_prop_boolean(prop.text)
                elif propName == AnalysisSettings._tag_summary_stats:
                    self.option_summary = _from_prop_boolean(prop.text)
                elif propName == AnalysisSettings._tag_top_talker_stats:
                    self.option_top_talker = _from_prop_boolean(prop.text)
                elif propName == AnalysisSettings._tag_traffic_stats:
                    self.option_traffic = _from_prop_boolean(prop.text)
                elif propName == AnalysisSettings._tag_voice_video:
                    self.option_voice_video = _from_prop_boolean(prop.text)
                elif propName == AnalysisSettings._tag_node_limit:
                    self.node_limit = StatsLimit(AnalysisSettings._tag_node_limit)
                    self.node_limit.enabled = _from_prop_boolean(prop.text)
                elif propName == AnalysisSettings._tag_detail_limit:
                    self.node_protocol_detail_limit = StatsLimit(AnalysisSettings._tag_detail_limit)
                    self.node_protocol_detail_limit.enabled = _from_prop_boolean(prop.text)
                elif propName == AnalysisSettings._tag_protocol_limit:
                    self.protocol_limit = StatsLimit(AnalysisSettings._tag_protocol_limit)
                    self.protocol_limit.enabled = _from_prop_boolean(prop.text)

    def _store(self, obj):
        """Store the Analysis Settings into the ETree.SubElement."""
        _set_label_clsid(obj, AnalysisSettings._tag_label,
                         AnalysisSettings._tag_clsid_name)
        props = obj.find(AnalysisSettings._tag_root_name)
        if props is not None:
            obj.remove(props)
            props = None

        props = ET.SubElement(obj, AnalysisSettings._tag_root_name)
        if props is None: raise OmniError("Unable to create: " + AnalysisSettings._tag_label)

        _set_property(props, AnalysisSettings._tag_alarms, 22, 
                      _to_prop_boolean(self.option_alarms))
        _set_property(props, AnalysisSettings._tag_analysis_modules, 22, 
                      _to_prop_boolean(self.option_analysis_modules))
        _set_property(props, AnalysisSettings._tag_application_stats, 22, 
                      _to_prop_boolean(self.option_application))
        _set_property(props, _tag_compass, 22, 
                      _to_prop_boolean(self.option_compass))
        _set_property(props, AnalysisSettings._tag_country_stats, 22, 
                      _to_prop_boolean(self.option_country))
        _set_property(props, AnalysisSettings._tag_error_stats, 22, 
                      _to_prop_boolean(self.option_error))
        _set_property(props, AnalysisSettings._tag_expert, 22, 
                      _to_prop_boolean(self.option_expert))
        _set_property(props, AnalysisSettings._tag_network_stats, 22, 
                      _to_prop_boolean(self.option_network))
        
        # If a limit is None or disabled, then set it as a bool property.
        if self.node_limit is None or not self.node_limit.enabled:
            _set_property(props, AnalysisSettings._tag_node_limit, 22, 
                          _to_prop_boolean(False))
        else:
            props.append(self.node_limit.to_xml())
        if self.node_protocol_detail_limit is None or not self.node_protocol_detail_limit.enabled:
            _set_property(props, AnalysisSettings._tag_detail_limit, 22, 
                          _to_prop_boolean(False))
        else:
            props.append(self.node_protocol_detail_limit.to_xml())
        if self.protocol_limit is None or not self.protocol_limit.enabled:
            _set_property(props, AnalysisSettings._tag_protocol_limit, 22, 
                          _to_prop_boolean(False))
        else:
            props.append(self.protocol_limit.to_xml())

        _set_property(props, AnalysisSettings._tag_size_stats, 22, 
                      _to_prop_boolean(self.option_size))
        _set_property(props, AnalysisSettings._tag_summary_stats, 22, 
                      _to_prop_boolean(self.option_summary))
        _set_property(props, AnalysisSettings._tag_top_talker_stats, 11, 
                      _to_prop_boolean(self.option_top_talker))
        _set_property(props, AnalysisSettings._tag_traffic_stats, 22, 
                      _to_prop_boolean(self.option_traffic))
        _set_property(props, AnalysisSettings._tag_voice_video, 22, 
                      _to_prop_boolean(self.option_voice_video))

    def set_all(self, enable=True):
        """Enable or disable all of the Analysis Options."""
        self.node_limit.enabled = enable
        self.node_protocol_detail_limit.enabled = enable
        self.protocol_limit.enabled = enable
        self.option_alarms = enable
        self.option_analysis_modules = enable
        self.option_application = enable
        self.option_compass = enable
        self.option_country = enable
        self.option_error = enable
        self.option_expert = enable
        self.option_network = enable
        self.option_size = enable
        self.option_summary = enable
        self.option_top_talker = enable
        self.option_traffic = enable
        self.option_voice_video = enable


class FilterSettings(object):
    """The Filter Settings section of a
    :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`
    """

    filters = []
    """A list of Filter Names."""

    ids = []
    """A list of :class:`OmniId <omniscript.omniid.OmniId>` of the enabled
    filters.
    """

    mode = config.FILTER_MODE_NONE
    """The filtering mode. One of the FILTER MODE constants."""

    #XML Tags
    _tag_label = "FilterConfig"
    _tag_clsid_name = "FilterConfig"
    _tag_root_name = _tag_props
    _tag_config = "filterconfig"
    _tag_filter = "filter"
    _tag_filters = "filters"
    _tag_mode = "mode"

    def __init__(self):
        self.filters = []
        self.ids = []
        self.mode = FilterSettings.mode

    def _load(self, obj):
        """Load the Filter Settings from an ETree.SubElement."""
        self.filters = []
        self._filter_ids = []

        if 'null' in obj.attrib:
            null_enabled = obj.attrib['null']
            if null_enabled:
                return

        config = obj.find(FilterSettings._tag_config)
        if config:
            mode = config.attrib.get(FilterSettings._tag_mode)
            self.mode = int(mode) if mode is not None else FilterSettings.mode

        """ Build filter_list """
        pass

    def _store(self, obj, engine):
        """Store the Filter Settings into the ETree.SubElement."""

        if 'null' in obj.attrib:
            del obj.attrib['null']
        _set_label_clsid(obj, FilterSettings._tag_label,
                         FilterSettings._tag_clsid_name)
        filter_config = obj.find(FilterSettings._tag_config)
        if filter_config is not None:
            obj.remove(filter_config)
            filter_config = None

        if not self.filters:
            self.mode = config.FILTER_MODE_NONE
        else:
           if self.mode == config.FILTER_MODE_NONE:
               self.mode = config.FILTER_MODE_ACCEPT_MATCHING_ANY

        filter_config = ET.SubElement(obj, FilterSettings._tag_config,
                          {FilterSettings._tag_mode : str(self.mode)})
        filters = ET.SubElement(filter_config, FilterSettings._tag_filters)
        if engine is not None:
            filter_list = engine.get_filter_list()
            for name in self.filters:
                fltr = find_filter(filter_list, name, _tag_name)
                if fltr:
                    ET.SubElement(filters, FilterSettings._tag_filter,
                                  {_tag_id:str(fltr.id)})
        else:
            for name in self.filters:
                 ET.SubElement(filters, FilterSettings._tag_filter,
                               {_tag_name:name})


class GeneralSettings(object):
    """The General Settings section of a
    :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`
    """

    buffer_size = 100 * BYTES_IN_A_MEGABYTE
    """The size of the capture buffer in bytes. Default is 100 megabytes"""

    comment = None
    """The optional comment string. Default is None."""

    directory = None
    """The optional directory to save Capture to Disk files in. Default is
    the OmniEngine's Data Folder.
    """

    file_pattern = 'Capture 1-'
    """The Capture to Disk file pattern. Default is 'Capture 1-'."""

    file_size = 256 * BYTES_IN_A_MEGABYTE
    """The size in gigabytes of the all the capture's packet files.
    Default is 256 Megabytes.
    """

    group_id = None
    """The Id of the capture's group."""

    group_name = ''
    """The name of the capture's group."""

    id = None
    """The Global Unique Identier of the capture."""

    keep_last_files_count = 10
    """The number of files to keep if 
    :func:`option_keep_last_files
    <omniscript.capturetemplate.generalsettings.option_keep_last_files>`
    is enabled.
    Default is 10.
    """

    max_file_age = SECONDS_IN_A_DAY
    """The maximum number of seconds a capture file may be open until a new file
    is created if
    :func:`option_file_age
    <omniscript.capturetemplate.generalsettings.option_file_age>`
    is enabled. Default is 1 day.
    """

    max_total_file_size = 10 * BYTES_IN_A_GIGABYTE
    """The maximum amount of disk space if
    :func:`option_total_file_size
    <omniscript.capturetemplate.generalsettings.option_total_file_size>`
    is enabled.
    Default is 10 gigabytes.
    """

    name = 'Capture 1'
    """The name of the capture. Default is 'Capture 1'."""

    owner = None
    """The name of the account that owns the capture."""

    reserved_size = 16384
    """Amount of diskspace in megabytes (confirm) to reserve on a Timeline for
    captures. Default is 16 Gigabytes.
    """

    slice_length = 128
    """The number of bytes to slice a packet to when option_slicing is
    enabled. Default is 128 bytes.
    """

    tap_timestamps = 0
    """The type of tap timestamps. One of the TAP TIMESTAMPS constants."""

    option_capture_to_disk = False
    """Is Capture to disk is enabled? Default is False."""

    option_continuous_capture = True
    """Is continuous capture enabled? Default is True."""

    option_deduplicate = False
    """Is discarding duplicate packets enabled? Default is False."""

    option_file_age = False
    """Is the option to start a new capture files when file has been open for
    :func:`max_file_age
    <omniscript.capturetemplate.generalsettings.max_file_age>`
    seconds is enabled? Default is False.
    """

    option_keep_last_files = False
    """Is the options to keep only the last
    :func:`keep_last_files_count
    <omniscript.capturetemplate.generalsettings.keep_last_files_count>`
    files option enabled? Default is False.
    """

    option_multistream = False
    """Is multistream enabled? Default is False."""

    option_priority_ctd = False
    """Is priority to Capture to Disk disk enabled. Default is False.
    If enabled priority will be given to Capture to Disk over packet processing.
    """

    option_save_as_template = False
    """Save these setting as a Capture Template. Default is False."""

    option_slicing = False
    """Is packet slicing enabled? Default is False."""

    option_spotlight_capture = False
    """Is Capture the Spotlight Capture? Default is False."""

    option_start_capture = False
    """Is the option to start the capture when its created enabled? Default is False."""

    option_timeline_app_stats = False
    """Is Application Statistics enabled? Default is False."""

    option_timeline_stats = False
    """Is Timeline Statistics enabled? Default is False."""

    option_timeline_top_stats = False
    """Is Top Statistics enabled? Default is False."""

    option_timeline_voip_stats = False
    """Is VoIP Statistics enabled? Default is False."""

    option_total_file_size = False
    """Is the option to restrict the maximum amount of diskspace to a total of
    :func:`max_total_file_size
    <omniscript.capturetemplate.generalsettings.max_total_file_size>`
    bytes is enabled? Default is False.
    """

    #XML Tags
    _tag_label = "GeneralSettings"
    _tag_clsid_name = _tag_prop_bag
    _tag_root_name = _tag_props
    _tag_application_stats = "EnableAppStats"
    _tag_buffer_size = "BufferSize"
    _tag_capture_id = "CaptureID"
    _tag_capture_to_disk = "CaptureToDisk"
    _tag_comment = "Comment"
    _tag_continuous_capture = "ContinuousCapture"
    _tag_deduplicate = "Deduplicate"
    _tag_directory = "CtdDir"
    _tag_file_age = "CtdUseMaxFileAge"
    _tag_file_pattern = "CtdFilePattern"
    _tag_file_size = "CtdFileSize"
    _tag_group_id = "GroupID"
    _tag_group_name = "GroupName"
    _tag_keep_last_files = "CtdKeepLastFiles"
    _tag_keep_last_files_count = "CtdKeepLastFilesCount"
    _tag_max_file_age = "CtdMaxFileAge"
    _tag_max_total_file_size = "CtdMaxTotalFileSize"
    _tag_multistream = "MultiStream"
    _tag_gs_name = "Name"
    _tag_owner = "Owner"
    _tag_priority_ctd = "CtdPriority"
    _tag_reserved_size = "CtdReservedSize"
    _tag_save_as_template = "SaveAsTemplate"
    _tag_slice_length = "SliceLength"
    _tag_slicing = "Slicing"
    _tag_spotlight_capture = "SpotlightCapture"
    _tag_start_capture = "StartCapture"
    _tag_tap_timestamps = "TapTimestamps"
    _tag_timeline_stats = "EnableTimelineStats"
    _tag_timeline_top_stats = "EnableTopStats"
    _tag_total_file_size = "CtdUseMaxTotalFileSize"
    _tag_voip_stats = "EnableVoIPStats"

    def __init__(self):
        self.buffer_size = GeneralSettings.buffer_size
        self.comment = GeneralSettings.comment
        self.directory = GeneralSettings.directory
        self.file_pattern = GeneralSettings.file_pattern
        self.file_size = GeneralSettings.file_size
        self.group_id = GeneralSettings.group_id
        self.group_name = None
        self.id = GeneralSettings.id
        self.keep_last_files_count = GeneralSettings.keep_last_files_count
        self.max_file_age = GeneralSettings.max_file_age
        self.max_total_file_size = GeneralSettings.max_total_file_size
        self.name = GeneralSettings.name
        self.owner = GeneralSettings.owner
        self.reserved_size = GeneralSettings.reserved_size
        self.slice_length = GeneralSettings.slice_length
        self.tap_timestamps = GeneralSettings.tap_timestamps
        self.option_capture_to_disk = GeneralSettings.option_capture_to_disk
        self.option_continuous_capture = \
            GeneralSettings.option_continuous_capture
        self.option_deduplicate = GeneralSettings.option_deduplicate
        self.option_file_age = GeneralSettings.option_file_age
        self.option_keep_last_files = GeneralSettings.option_keep_last_files
        self.option_multistream = GeneralSettings.option_multistream
        self.option_priority_ctd = GeneralSettings.option_priority_ctd
        self.option_save_as_template = GeneralSettings.option_save_as_template
        self.option_slicing = GeneralSettings.option_slicing
        self.option_start_capture = GeneralSettings.option_start_capture
        self.option_timeline_app_stats = GeneralSettings.option_timeline_app_stats
        self.option_timeline_stats = GeneralSettings.option_timeline_stats
        self.option_timeline_top_stats = \
            GeneralSettings.option_timeline_top_stats
        self.option_timeline_voip_stats = GeneralSettings.option_timeline_voip_stats
        self.option_total_file_size = GeneralSettings.option_total_file_size
        self.option_spotlight_capture = GeneralSettings.option_spotlight_capture

    def _load(self, obj):
        """Load the General Settings from an ETree.SubElement."""
        props = obj.find(GeneralSettings._tag_root_name)
        if props is not None:
            for prop in props.findall(_tag_prop):
                propName = prop.attrib[_tag_name]
                if propName == GeneralSettings._tag_buffer_size:
                    self.buffer_size = int(prop.text)
                elif propName == GeneralSettings._tag_capture_id:
                    self.id = OmniId(prop.text)
                elif propName == GeneralSettings._tag_capture_to_disk:
                    self.option_capture_to_disk = _from_prop_boolean(prop.text)
                elif propName == GeneralSettings._tag_comment:
                    self.comment = prop.text
                elif propName == GeneralSettings._tag_continuous_capture:
                    self.option_continuous_capture = \
                        _from_prop_boolean(prop.text)
                elif propName == GeneralSettings._tag_deduplicate:
                    self.option_deduplicate = _from_prop_boolean(prop.text)
                elif propName == GeneralSettings._tag_directory:
                    self.directory = prop.text
                elif propName == GeneralSettings._tag_file_age:
                    self.option_file_age = _from_prop_boolean(prop.text)
                elif propName == GeneralSettings._tag_file_pattern:
                    self.file_pattern = prop.text
                elif propName == GeneralSettings._tag_file_size:
                    self.file_size = int(prop.text)
                elif propName == GeneralSettings._tag_group_id:
                    self.group_id = OmniId(prop.text)
                elif propName == GeneralSettings._tag_group_name:
                    self.group_name = prop.text
                elif propName == GeneralSettings._tag_keep_last_files:
                    self.option_keep_last_files = _from_prop_boolean(prop.text)
                elif propName == GeneralSettings._tag_keep_last_files_count:
                    self.keep_last_files_count = int(prop.text)
                elif propName == GeneralSettings._tag_max_file_age:
                    self.max_file_age = int(prop.text)
                elif propName == GeneralSettings._tag_max_total_file_size:
                    self.max_total_file_size = int(prop.text)
                elif propName == GeneralSettings._tag_multistream:
                    self.option_multistream = _from_prop_boolean(prop.text)
                elif propName == GeneralSettings._tag_gs_name:
                    self.name = prop.text
                elif propName == GeneralSettings._tag_owner:
                    self.owner = prop.text
                elif propName == GeneralSettings._tag_priority_ctd:
                    self.option_priority_ctd = _from_prop_boolean(prop.text)
                elif propName == GeneralSettings._tag_reserved_size:
                    self.reserved_size = int(prop.text)
                elif propName == GeneralSettings._tag_save_as_template:
                    self.option_save_as_template = _from_prop_boolean(prop.text)
                elif propName == GeneralSettings._tag_slice_length:
                    self.slice_length = int(prop.text)
                elif propName == GeneralSettings._tag_spotlight_capture:
                    self.option_spotlight_capture = _from_prop_boolean(prop.text)
                elif propName == GeneralSettings._tag_slicing:
                    self.option_slicing = _from_prop_boolean(prop.text)
                elif propName == GeneralSettings._tag_start_capture:
                    self.option_start_capture = _from_prop_boolean(prop.text)
                elif propName == GeneralSettings._tag_tap_timestamps:
                    self.tap_timestamps = int(prop.text)
                elif propName == GeneralSettings._tag_timeline_stats:
                    self.option_timeline_stats = _from_prop_boolean(prop.text)
                elif propName == GeneralSettings._tag_application_stats:
                    self.option_timeline_app_stats = _from_prop_boolean(prop.text)
                elif propName == GeneralSettings._tag_timeline_top_stats:
                    self.option_timeline_top_stats = _from_prop_boolean(prop.text)
                elif propName == GeneralSettings._tag_total_file_size:
                    self.option_total_file_size = _from_prop_boolean(prop.text)
                elif propName == GeneralSettings._tag_voip_stats:
                    self.option_timeline_voip_stats = _from_prop_boolean(prop.text)

    def _store(self, obj):
        """Store the General Settings into the ETree.SubElement."""
        _set_label_clsid(obj, GeneralSettings._tag_label,
                         GeneralSettings._tag_clsid_name)
        props = obj.find(GeneralSettings._tag_root_name)
        if props is None:
            props = ET.SubElement(obj, _tag_props)
        _set_property(props, GeneralSettings._tag_buffer_size, 21, str(self.buffer_size))
        _set_property(props, GeneralSettings._tag_capture_to_disk, 11, 
                      _to_prop_boolean(self.option_capture_to_disk))
        _comment = self.comment if self.comment is not None else ''
        _set_property(props, GeneralSettings._tag_comment, 8, _comment)
        _set_property(props, GeneralSettings._tag_continuous_capture, 11, 
                      _to_prop_boolean(self.option_continuous_capture))
        _set_property(props, GeneralSettings._tag_deduplicate, 11,
                      _to_prop_boolean(self.option_deduplicate))
        _directory = self.directory if self.directory is not None else ''
        _set_property(props, GeneralSettings._tag_directory, 8, _directory)
        _set_property(props, GeneralSettings._tag_file_age, 11,
                      _to_prop_boolean(self.option_file_age))
        _set_property(props, GeneralSettings._tag_file_pattern, 8, self.file_pattern)
        _set_property(props, GeneralSettings._tag_file_size, 21, str(self.file_size))
        if self.group_id is not None:
            _set_property(props, GeneralSettings._tag_group_id, 8, str(self.group_id))
        if self.group_name is not None:
            _set_property(props, GeneralSettings._tag_group_name, 8, self.group_name)
        if self.id is None:
            _id = _find_property(obj, GeneralSettings._tag_capture_id)
            if _id is not None:
                del obj.attrib[GeneralSettings._tag_capture_id]
        else:
            _set_property(props, _tag_id, 8, str(self.id))
        _set_property(props, GeneralSettings._tag_keep_last_files, 11, 
                      _to_prop_boolean(self.option_keep_last_files))
        _set_property(props, GeneralSettings._tag_keep_last_files_count, 19,
                      self.keep_last_files_count)
        _set_property(props, GeneralSettings._tag_max_file_age, 21, str(self.max_file_age))
        _set_property(props, GeneralSettings._tag_max_total_file_size, 21,
                      str(self.max_total_file_size))
        _set_property(props, GeneralSettings._tag_multistream, 11,
                      _to_prop_boolean(self.option_multistream))					  
        _set_property(props, GeneralSettings._tag_gs_name, 8, self.name)
        if self.owner is None or len(self.owner) > 0:
            _id = _find_property(obj, GeneralSettings._tag_owner)
            if _id is not None:
                del obj.attrib[GeneralSettings._tag_owner]
        else:
            _set_property(props, GeneralSettings._tag_owner, 8, self.owner)
        _set_property(props, GeneralSettings._tag_priority_ctd, 11,
                      _to_prop_boolean(self.option_priority_ctd))
#        _set_property(props, GeneralSettings._tag_reserved_size, 21, str(self.reserved_size))
        _set_property(props, GeneralSettings._tag_save_as_template, 11,
                      _to_prop_boolean(self.option_save_as_template))
        _set_property(props, GeneralSettings._tag_slice_length, 19, self.slice_length)
        _set_property(props, GeneralSettings._tag_slicing, 11,
                      _to_prop_boolean(self.option_slicing))
        if self.option_spotlight_capture:
            _set_property(props, GeneralSettings._tag_spotlight_capture, 11,
                          _to_prop_boolean(self.option_spotlight_capture))
        _set_property(props, GeneralSettings._tag_start_capture, 11,
                      _to_prop_boolean(self.option_start_capture))
        _set_property(props, GeneralSettings._tag_tap_timestamps, 22, str(self.tap_timestamps))
        _set_property(props, GeneralSettings._tag_application_stats, 11, 
                      _to_prop_boolean(self.option_timeline_app_stats))
        _set_property(props, GeneralSettings._tag_timeline_stats, 11,
                      _to_prop_boolean(self.option_timeline_stats))
        _set_property(props, GeneralSettings._tag_timeline_top_stats, 11,
                      _to_prop_boolean(self.option_timeline_top_stats))
        _set_property(props, GeneralSettings._tag_total_file_size, 11,
                      _to_prop_boolean(self.option_total_file_size))
        _set_property(props, GeneralSettings._tag_voip_stats, 11,
                      _to_prop_boolean(self.option_timeline_voip_stats))

    def set_timeline(self, enable=True):
        self.option_timeline_stats = enable
        self.option_timeline_app_stats = enable
        self.option_timeline_top_stats = enable
        self.option_timeline_voip_stats = enable

    def set_multistream(self, enable=False):
        self.option_multistream = enable


class GraphsSettings(object):
    """The Graphs Settings section of a
    :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`
    """

    graphs = []
    """A list of 
    :class:`GraphTemplate <omniscript.graphtemplate.GraphTemplate>`,
    or OmniId or OmniId as a string.
    
    The function :func:`get_graph_template_list()
    <omniscript.omniengine.OmniEngine.get_graph_template_list>`
    returns a list of GraphTemplate objects.
    """

    enabled = False
    """Are graphs enabled."""

    interval = 15
    """The interval in seconds."""

    file_count = 2

    file_buffer_size = 50

    hours_to_keep = 1
    """Keep most recent in hours."""

    option_preserve_files = False

    #XML Tags
    _tag_label = "GraphSettings"
    _tag_clsid_name = "GraphSettings"
    _tag_graphdata = "graphdata"
    _tag_interval = "Interval"
    _tag_file_count = "FileCnt"
    _tag_file_buffer_size = "FileBufferSize"
    _tag_preserve_files = "PreserveFiles"
    _tag_hours_to_keep = "memory"
    _tag_templates = "templates"
    _tag_template = "template"
    _tag_units = "IntervalUnit"

    def __init__(self):
        self.graphs = []
        self.enabled = GraphsSettings.enabled
        self.interval = GraphsSettings.interval
        self.file_count = GraphsSettings.file_count
        self.file_buffer_size = GraphsSettings.file_buffer_size
        self.option_preserve_files = GraphsSettings.option_preserve_files
        self.hours_to_keep = GraphsSettings.hours_to_keep

    def _load(self, obj, engine=None):
        """Load the Graph Settings from an ETree.SubElement."""
        graph_data = obj.find(GraphsSettings._tag_graphdata)
        self.enabled = _is_attribute_enabled(graph_data, _tag_alt_enabled)
        self.file_count = int(graph_data.attrib.get(
            GraphsSettings._tag_file_count, str(GraphsSettings.file_count)))
        self.file_buffer_size = int(graph_data.attrib.get(
            GraphsSettings._tag_file_buffer_size, str(GraphsSettings.file_buffer_size)))
        self.hours_to_keep = int(graph_data.attrib.get(
            GraphsSettings._tag_hours_to_keep, str(GraphsSettings.hours_to_keep)))
        self.option_preserve_files = _is_attribute_enabled(
            graph_data, GraphsSettings._tag_preserve_files)
        _interval = int(graph_data.attrib.get(
            GraphsSettings._tag_interval, str(GraphsSettings.interval)))
        _units = int(graph_data.attrib.get(GraphsSettings._tag_units, '1'))

        graph_template_list = []
        if engine is not None:
            graph_template_list = engine.get_graph_template_list()
        if graph_data is not None:
            templates = graph_data.find(GraphsSettings._tag_templates)
            for template in templates.findall(GraphsSettings._tag_template):
                id = OmniId(template.attrib[_tag_id])
                template = find_graph_template(graph_template_list, id, 'id')
                if template:
                    self.graphs.append(template)
                # else:
                #     name = id

        if (_units < GRAPHS_INTERVAL_SECONDS) or (_units > GRAPHS_INTERVAL_DAYS):
            _units = GRAPHS_INTERVAL_SECONDS
        self.interval = _interval * interval_multiplier[_units]

    def _store(self, obj):
        """Store the Graphs Data into the ETree.SubElement."""
        _set_label_clsid(obj, GraphsSettings._tag_label,
                         GraphsSettings._tag_clsid_name)
        graph_data = obj.find(GraphsSettings._tag_graphdata)
        if graph_data is not None:
            obj.remove(graph_data)
            graph_data = None

        _interval, _units = _to_interval_units(self.interval)
           
        graph_data = ET.SubElement(obj, GraphsSettings._tag_graphdata,
                                   {_tag_alt_enabled : _to_prop_boolean(self.enabled),
                                    GraphsSettings._tag_interval : str(_interval),
                                    GraphsSettings._tag_units : str(_units),
                                    GraphsSettings._tag_file_count : str(self.file_count),
                                    GraphsSettings._tag_file_buffer_size : \
                                        str(self.file_buffer_size),
                                    GraphsSettings._tag_preserve_files : \
                                        _to_prop_boolean(self.option_preserve_files),
                                    GraphsSettings._tag_hours_to_keep : str(self.hours_to_keep) })
        templates = ET.SubElement(graph_data , GraphsSettings._tag_templates)
        for graph in self.graphs:
            if isinstance(graph, GraphTemplate):
                id_string = str(graph.id)
            elif isinstance(graph, OmniId):
                id_string = str(graph)
            else:
                id_string = str(graph)
            ET.SubElement(templates, GraphsSettings._tag_template,
                          {_tag_id:id_string})


class IndexingSettings(object):
    """The Indexing Settings section of a
    :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`
    """

    option_application = False
    """Is Application indexing enabled?"""

    option_country = False
    """Is Country indexing enabled?"""

    option_ethernet = False
    """Is Ethernet indexing enabled?"""

    option_ipv4 = False
    """Is IPv4 indexing enabled?"""

    option_ipv6 = False
    """Is IPv6 indexing enabled?"""

    option_mpls = False
    """Is MPLS indexing enabled?"""

    option_port = False
    """Is Port indexing enabled?"""

    option_protospec = False
    """Is Protospec indexing enabled?"""

    option_vlan = False
    """Is VLAN indexing enabled?"""

    #XML Tags
    _tag_label = "IndexingSettings"
    _tag_clsid_name = "PropertyList"
    _tag_root_name = _tag_props

    #Names
    _name_application = "Indexing Application"
    _name_country = "Indexing Country"
    _name_ethernet = "Indexing Ethernet"
    _name_ipv4 = "Indexing IPv4"
    _name_ipv6 = "Indexing IPv6"
    _name_mpls = "Indexing MPLS"
    _name_port = "Indexing Port"
    _name_protospec = "Indexing Protospec"
    _name_vlan = "Indexing VLAN"

    #Options list
    _options = [
        'option_ipv4',
        'option_ipv6',
        'option_ethernet',
        'option_port',
        'option_protospec',
        'option_application',
        'option_vlan',
        'option_mpls',
        'option_country'
        ]

    def __init__(self):
        self.option_application = IndexingSettings.option_application
        self.option_country = IndexingSettings.option_country
        self.option_ethernet = IndexingSettings.option_ethernet
        self.option_ipv4 = IndexingSettings.option_ipv4
        self.option_ipv6 = IndexingSettings.option_ipv6
        self.option_mpls = IndexingSettings.option_mpls
        self.option_port = IndexingSettings.option_port
        self.option_protospec = IndexingSettings.option_protospec
        self.option_vlan = IndexingSettings.option_vlan

    def _load(self, obj):
        """Load the Indexing Settings from an ETree.SubElement."""
        if 'null' in obj.attrib:
            null_enabled = obj.attrib['null']
            if null_enabled:
                return
        self.set_all(False)
        props = obj.find(IndexingSettings._tag_root_name)
        if props is not None:
            for prop in props.findall(_tag_prop):
                _index = int(prop.text)
                if _index < len(IndexingSettings._options) and \
                        hasattr(self, IndexingSettings._options[_index]):
                    setattr(self, IndexingSettings._options[_index], True)

    def _store(self, obj):
        """Store the Indexing Data into the ETree.SubElement."""
        if 'null' in obj.attrib:
            del obj.attrib['null']
        _set_label_clsid(obj, IndexingSettings._tag_label,
                         IndexingSettings._tag_clsid_name)
        props = obj.find(IndexingSettings._tag_root_name)
        if props is not None:
            obj.remove(props)
            props = None
        props = ET.SubElement(obj, IndexingSettings._tag_root_name)
        if self.option_ipv4:
            _set_property(props, None, 17, "0")
        if self.option_ipv6:
            _set_property(props, None, 17, "1")
        if self.option_ethernet:
            _set_property(props, None, 17, "2")
        if self.option_port:
            _set_property(props, None, 17, "3")
        if self.option_protospec:
            _set_property(props, None, 17, "4")
        if self.option_application:
            _set_property(props, None, 17, "5")
        if self.option_vlan:
            _set_property(props, None, 17, "6")
        if self.option_mpls:
            _set_property(props, None, 17, "7")
        if self.option_country:
            _set_property(props, None, 17, "8")

    def set_all(self, enable=True):
        """Enable (default) or disable all the indexing options."""
        self.option_application = enable
        self.option_country = enable
        self.option_ethernet = enable
        self.option_ipv4 = enable
        self.option_ipv6 = enable
        self.option_mpls = enable
        self.option_port = enable
        self.option_protospec = enable
        self.option_vlan = enable


class StatisticsOutputSettings(object):
    """The Statistics Output Settings section of a
    :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`
    """

    enabled = False
    """Are the Statistics Output Settings enabled?"""

    files_to_keep = 60
    """The number of files to keep."""

    interval = 60
    """Output interval in seconds"""

    new_set_interval = 60
    """New Set interval in seconds"""

    report_path = ''
    """The path where reports will be written."""

    report_type = 4
    """The report format type."""

    user_path = ''
    """The user path."""

    option_notify = False
    """Is the notification option enabled?"""

    option_align_new_set = False
    """Are new sets to be aligned?"""

    option_align_output = False
    """Is the output to be aligned?"""

    option_new_set = False
    """Is the new set option enabled?"""

    option_scheduled = False
    """Is the scheduled option enabled?"""

    option_keep_files = False
    """Is the keep files option enabled?"""

    option_reset_output = False
    """Is the reset output option enabled?"""

    #XML Tags
    _tag_label = "StatsOutput"
    _tag_clsid_name = "StatsOutput"
    _tag_root_name = "statsoutput"
    _tag_align_new_set = "alignNewSet"
    _tag_align_output = "alignOutput"
    _tag_files_to_keep = "keep"
    _tag_keep_files = "keepEnabled"
    _tag_new_set = "newSetEnabled"
    _tag_new_set_interval = "newSetInterval"
    _tag_new_set_units = "newSetIntervalUnit"
    _tag_notify = "notify"
    _tag_interval = "outputInterval"
    _tag_report_path = "reportPath"
    _tag_report_type = "outputType"
    _tag_reset_output = "ResetOutput"
    _tag_scheduled = "scheduled"
    _tag_units = "outputIntervalUnit"
    _tag_user_path = "userPath"


    def __init__(self):
        self.enabled = StatisticsOutputSettings.enabled
        self.files_to_keep = StatisticsOutputSettings.files_to_keep
        self.interval = StatisticsOutputSettings.interval
        self.new_set_interval = StatisticsOutputSettings.new_set_interval
        self.report_path = StatisticsOutputSettings.report_path
        self.report_type = StatisticsOutputSettings.report_type
        self.user_path = StatisticsOutputSettings.user_path
        self.option_notify = StatisticsOutputSettings.option_notify
        self.option_align_new_set = StatisticsOutputSettings.option_align_new_set
        self.option_align_output = StatisticsOutputSettings.option_align_output
        self.option_new_set = StatisticsOutputSettings.option_new_set
        self.option_scheduled = StatisticsOutputSettings.option_scheduled
        self.option_keep_files = StatisticsOutputSettings.option_keep_files
        self.option_reset_output = StatisticsOutputSettings.option_reset_output

    def _load(self, obj):
        """Load the Statistics Output Settings from an ETree.SubElement."""
        settings = obj.find(StatisticsOutputSettings._tag_root_name)
        if settings is not None:
            self.enabled = _is_attribute_enabled(settings, StatisticsOutputSettings.enabled)
            self.files_to_keep = int(settings.attrib.get(
                StatisticsOutputSettings._tag_files_to_keep,
                StatisticsOutputSettings.files_to_keep))
            self.report_path = settings.attrib.get(
                StatisticsOutputSettings._tag_report_path,
                StatisticsOutputSettings.report_path)
            self.report_type = settings.attrib.get(
                StatisticsOutputSettings._tag_report_type, 
                StatisticsOutputSettings.report_type)
            self.user_path = settings.attrib.get(
                StatisticsOutputSettings._tag_user_path,
                StatisticsOutputSettings.user_path)
            self.option_notify = _is_attribute_enabled(
                settings, StatisticsOutputSettings._tag_notify)
            self.option_align_new_set = _is_attribute_enabled(
                settings, StatisticsOutputSettings._tag_align_new_set)
            self.option_align_output = _is_attribute_enabled(
                settings, StatisticsOutputSettings._tag_align_output)
            self.option_new_set = _is_attribute_enabled(
                settings, StatisticsOutputSettings._tag_new_set)
            self.option_scheduled = _is_attribute_enabled(
                settings, StatisticsOutputSettings._tag_scheduled)
            self.option_keep_files = _is_attribute_enabled(
                settings, StatisticsOutputSettings._tag_keep_files)
            self.option_reset_output = _is_attribute_enabled(
                settings, StatisticsOutputSettings._tag_reset_output)

            _interval = int(settings.attrib.get(StatisticsOutputSettings._tag_interval, 1))
            _units = int(settings.attrib.get(StatisticsOutputSettings._tag_units, 3))
            if (_units < GRAPHS_INTERVAL_SECONDS) or (_units > GRAPHS_INTERVAL_DAYS):
                _units = GRAPHS_INTERVAL_SECONDS
            self.interval = _interval * interval_multiplier[_units]

            _new_set_interval = int(settings.attrib.get(StatisticsOutputSettings._tag_new_set_interval,1 ))
            _new_set_units = int(settings.attrib.get(StatisticsOutputSettings._tag_new_set_units, 4))
            if (_new_set_units < GRAPHS_INTERVAL_SECONDS) or (_new_set_units > GRAPHS_INTERVAL_DAYS):
                _new_set_units = GRAPHS_INTERVAL_SECONDS
            self.new_set_interval = _new_set_interval * interval_multiplier[_new_set_units]

    def _store(self, obj):
        """Store the Statistics Output Settings into the ETree.SubElement."""
        _set_label_clsid(obj, StatisticsOutputSettings._tag_label,
                         StatisticsOutputSettings._tag_clsid_name)
        settings = obj.find(StatisticsOutputSettings._tag_root_name)
        if settings is not None:
            obj.remove(settings)
            settings = None
        settings = ET.SubElement(obj, StatisticsOutputSettings._tag_root_name)
        if settings is None: raise OmniError("Unable to create: " + StatisticsOutputSettings._tag_label)

        settings.attrib[_tag_enabled] = _to_prop_boolean(self.enabled)
        settings.attrib[StatisticsOutputSettings._tag_files_to_keep] = str(self.files_to_keep)
        settings.attrib[StatisticsOutputSettings._tag_report_path] = self.report_path
        settings.attrib[StatisticsOutputSettings._tag_report_type] = str(self.report_type)
        settings.attrib[StatisticsOutputSettings._tag_user_path] = self.user_path
        settings.attrib[StatisticsOutputSettings._tag_notify] = _to_prop_boolean(self.option_notify)
        settings.attrib[StatisticsOutputSettings._tag_align_new_set] = _to_prop_boolean(self.option_align_new_set)
        settings.attrib[StatisticsOutputSettings._tag_align_output] = _to_prop_boolean(self.option_align_output)
        settings.attrib[StatisticsOutputSettings._tag_new_set] = _to_prop_boolean(self.option_new_set)
        settings.attrib[StatisticsOutputSettings._tag_scheduled] = _to_prop_boolean(self.option_scheduled)
        settings.attrib[StatisticsOutputSettings._tag_keep_files] = _to_prop_boolean(self.option_keep_files)
        settings.attrib[StatisticsOutputSettings._tag_reset_output] = _to_prop_boolean(self.option_reset_output)

        _interval, _units = _to_interval_units(self.interval)
        settings.attrib[StatisticsOutputSettings._tag_interval] = str(_interval)
        settings.attrib[StatisticsOutputSettings._tag_units] = str(_units)

        _new_set_interval, _new_set_units = _to_interval_units(self.new_set_interval)
        settings.attrib[StatisticsOutputSettings._tag_new_set_interval] = str(_new_set_interval)
        settings.attrib[StatisticsOutputSettings._tag_new_set_units] = str(_new_set_units)


class StatisticsOutputPreferencesSettings(object):
    """The Statistics Output Preferences Settings section of a
    :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`
    """

    enabled = False
    """Are the Statistics Output Preferences Settings enabled?"""

    report_type = 3
    """The Report type. One of the REPORT TYPE constances."""

    #XML Tags
    _tag_label = "StatsOutputPrefs"
    _tag_clsid_name = "StatsOutputPrefs"
    _tag_root_name = "StatsOutputPrefs"
    _tag_report_type = "ReportType"

    def __init__(self):
        self.enabled = StatisticsOutputPreferencesSettings.enabled
        self.report_type = StatisticsOutputPreferencesSettings.report_type

    def _load(self, obj):
        """Load the Statistics Output Preferences Settings from an ETree.SubElement."""
        settings = obj.find(StatisticsOutputPreferencesSettings._tag_root_name)
        if settings is not None:
            self.enabled = _is_attribute_enabled(settings, StatisticsOutputPreferencesSettings.enabled)
            self.report_type = int(settings.attrib.get(
                StatisticsOutputPreferencesSettings._tag_report_type,
                StatisticsOutputPreferencesSettings.report_type))

    def _store(self, obj):
        """Store the Statistics Output Preferences Settings into the ETree.SubElement."""
        _set_label_clsid(obj, StatisticsOutputPreferencesSettings._tag_label,
                         StatisticsOutputPreferencesSettings._tag_clsid_name)
        settings = obj.find(StatisticsOutputPreferencesSettings._tag_root_name)
        if settings is not None:
            obj.remove(settings)
            settings = None
        settings = ET.SubElement(obj, StatisticsOutputPreferencesSettings._tag_root_name)
        if settings is None: raise OmniError("Unable to create: " + StatisticsOutputPreferencesSettings._tag_label)

        settings.attrib[_tag_enabled] = _to_prop_boolean(self.enabled)
        settings.attrib[StatisticsOutputPreferencesSettings._tag_report_type] = str(self.report_type)


class TriggerSettings(object):
    """The Trigger Settings section of a
    :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`
    """

    captured = None
    """The CaptureLimit of the Trigger. Initialized to a
    :class:`CaptureLimit <omniscript.capturetemplate.CaptureLimit>`
    object.
    """

    enabled = False
    """Is the trigger enabled."""

    filter = None
    """The Filter Limit of the Trigger. Initialized to a
    :class:`FilterLimit <omniscript.capturetemplate.FilterLimit>`
    object.
    """

    severity = config.SEVERITY_INFORMATIONAL
    """The Severity of the the notification. One of the SEVERITY constants"""

    time = None
    """The Date Limit of the Trigger. Initialized to a
    :class:`DateLimit <omniscript.capturetemplate.DateLimit>`
    object.
    """

    option_notify = True
    """Is notification enabled for when the Trigger triggers."""

    option_toggle_capture = True
    """Is Toggling the Capture when the Trigger triggers enabled?"""

    _label = None

    #XML Tags
    _tag_label = "Trigger"
    _tag_clsid_name = "Trigger"
    _tag_root_name = "trigger"
    _tag_events = "triggerevents"
    _tag_event_obj = "triggereventobj"
    _tag_notify = "notify"
    _tag_severity = "severity"
    _tag_toggle = "togglecapture"

    #Trigger Event Names
    _class_bytes_captured = "BytesCapturedTriggerEvent"
    _class_time = "TimeTriggerEvent"
    _class_flter = "FilterTriggerEvent"

    def __init__(self, label):
        self.captured = CaptureLimit()
        self.enabled = TriggerSettings.enabled
        self.filter = FilterLimit()
        self.severity = TriggerSettings.severity
        self.time = DateLimit()
        self.option_notify = TriggerSettings.option_notify
        self.option_toggle_capture = TriggerSettings.option_toggle_capture
        self._label = label

    def _load(self, obj, engine):
        """Load the Trigger Settings from an ETree SubElement."""
        self._name = obj.attrib[_tag_name]
        trigger = obj.find(TriggerSettings._tag_root_name)
        self.enabled = _is_prop_enabled(trigger)
        self.option_notify = _is_attribute_enabled(
            trigger, TriggerSettings._tag_notify)
        self.severity = int(trigger.attrib.get(
            TriggerSettings._tag_severity, str(TriggerSettings.severity)))
        self.option_toggle_capture = _is_attribute_enabled(
            trigger, TriggerSettings._tag_toggle)
        events = trigger.find(TriggerSettings._tag_events)
        for ev_obj in events.findall(TriggerSettings._tag_event_obj):
            class_name_ids = omniscript.get_class_name_ids()
            id = OmniId(ev_obj.attrib.get(_tag_clsid))
            if id == class_name_ids[TriggerSettings._class_bytes_captured]:
                self.captured = CaptureLimit()
                self.captured._load(ev_obj)
            elif id == class_name_ids[TriggerSettings._class_time]:
                self.time = DateLimit()
                self.time._load(ev_obj)
            elif id == class_name_ids[TriggerSettings._class_flter]:
                self.filter = FilterLimit()
                self.filter._load(ev_obj, engine)

    def _store(self, obj, engine):
        """Store the Trigger Settings into the ETree SubElement"""
        _set_label_clsid(obj, self._label, TriggerSettings._tag_clsid_name)
        class_name_ids = omniscript.get_class_name_ids()
        trigger = obj.find(TriggerSettings._tag_root_name)
        if trigger is not None:
            obj.remove(trigger)
        trigger = ET.SubElement(obj, TriggerSettings._tag_root_name)
        trigger.attrib[_tag_enabled] = _to_prop_boolean(self.enabled)
        trigger.attrib[TriggerSettings._tag_notify] = \
            _to_prop_boolean(self.option_notify)
        trigger.attrib[TriggerSettings._tag_severity] = str(self.severity)
        trigger.attrib[TriggerSettings._tag_toggle] = \
            _to_prop_boolean(self.option_toggle_capture)
        events = ET.SubElement(trigger, TriggerSettings._tag_events)
        if self.captured is not None:
            ev = ET.SubElement(events, TriggerSettings._tag_event_obj,
                               {_tag_clsid : \
                                   str(class_name_ids[TriggerSettings._class_bytes_captured])})
            self.captured._store(ev)
        if self.time is not None:
            ev = ET.SubElement(events, TriggerSettings._tag_event_obj,
                               {_tag_clsid : \
                                   str(class_name_ids[TriggerSettings._class_time])})
            self.time._store(ev)
        if self.filter is not None:
            ev = ET.SubElement(events, TriggerSettings._tag_event_obj,
                               {_tag_clsid : \
                                   str(class_name_ids[TriggerSettings._class_flter])})
            self.filter._store(ev, engine)


class VoIPSettings(object):
    """VoIP Settings."""

    max_calls = 2000
    """The maximum number of calls to track."""

    severity = 3
    """The serverity level of the notifications."""

    option_notify = True
    """Send notifications."""

    option_stop_analysis = True
    """Stop tracking VoIP calls if a notification is sent."""

    #XML Tags
    _tag_label = "VoIPConfig"
    _tag_clsid_name = _tag_prop_bag
    _tag_root_name = _tag_props
    _tag_max_calls = "MaxCalls"
    _tag_notify = "Notify"
    _tag_severity = "Severity"
    _tag_stop_analysis = "StopAnalysis"

    def __init__(self):
        self.max_calls = VoIPSettings.max_calls
        self.severity = VoIPSettings.severity
        self.option_notify = VoIPSettings.option_notify
        self.option_stop_analysis = VoIPSettings.option_stop_analysis

    def _load(self, obj):
        """Load the VoIP Settings from an ETree SubElement."""
        props = obj.find(VoIPSettings._tag_root_name)
        if props is not None:
            for prop in props.findall(_tag_prop):
                propName = prop.attrib[_tag_name]
                if propName == VoIPSettings._tag_max_calls:
                    self.max_calls = int(prop.text)
                elif propName == VoIPSettings._tag_severity:
                    self.severity = int(prop.text)
                elif propName == VoIPSettings._tag_notify:
                    self.option_notify = _from_prop_boolean(prop.text)
                elif propName == VoIPSettings._tag_stop_analysis:
                    self.option_stop_analysis = _from_prop_boolean(prop.text)

    def _load_raw(self, obj):
        """Load the VoIP Settings from an ETree SubElement."""
        for child in obj:
            if child.tag == VoIPSettings._tag_max_calls:
                self.max_calls = int(child.text)
            elif child.tag == VoIPSettings._tag_severity:
                self.severity = int(child.text)
            elif child.tag == VoIPSettings._tag_notify:
                self.option_notify = _from_prop_boolean(child.text)
            elif child.tag == VoIPSettings._tag_stop_analysis:
                self.option_stop_analysis = _from_prop_boolean(child.text)

    def _store(self, obj):
        """Store the VoPI Settings into the ETree.SubElement."""
        _set_label_clsid(obj, VoIPSettings._tag_label,
                         VoIPSettings._tag_clsid_name)
        props = obj.find(VoIPSettings._tag_root_name)
        if props is None:
            props = ET.SubElement(obj, _tag_props)
        _set_property(props, VoIPSettings._tag_max_calls, 19, str(self.max_calls))
        _set_property(props, VoIPSettings._tag_notify, 22, _to_prop_boolean(self.option_notify))
        _set_property(props, VoIPSettings._tag_severity, 19, str(self.severity))
        _set_property(props, VoIPSettings._tag_stop_analysis, 22, _to_prop_boolean(self.option_stop_analysis))

    def _store_raw(self, obj):
        """Store the VoPI Settings into the ETree.SubElement."""
        _voip = ET.SubElement(obj, VoIPSettings._tag_label)
        ET.SubElement(_voip, VoIPSettings._tag_max_calls).text = str(self.max_calls)
        ET.SubElement(_voip, VoIPSettings._tag_notify).text = _to_prop_boolean(self.option_notify)
        ET.SubElement(_voip, VoIPSettings._tag_severity).text = str(self.severity)
        ET.SubElement(_voip, VoIPSettings._tag_stop_analysis).text = _to_prop_boolean(self.option_stop_analysis)


class CaptureTemplate(object):
    """Basic Capture Template class, defaults come from the Capture Template 
    file. Load the Adapter, General and Filter settings from an existing
    capture template file. Modify the various settings.
    Then create the capture on an OmniEngine: engine.create_capture(template)
    """

    adapter = None
    """The Adapter Settings of the capture. Initialized to a
    :class:`AdapterSettings <omniscript.capturetemplate.AdapterSettings>`
    object.
    """

    alarms = None
    """The Alarm Settings of the capture."""

    analysis = None
    """The Analysis Settings of the capture. Initialized to a
    :class:`AnalysisSettings <omniscript.capturetemplate.AnalysisSettings>`
    """

    filename = None
    """An optional file name of a Capture Template file."""

    filter = None
    """The list of enabled filters of the capture. Initialized to a
    :class:`FilterSettings <omniscript.capturetemplate.FilterSettings>`
    object.
    """

    general = None
    """The General Settings of the capture. Initialized to a
    :class:`GeneralSettings <omniscript.capturetemplate.GeneralSettings>`
    object.
    """

    graphs = None
    """The Graphs Settings of the capture. Initialized to a
    :class:`GraphsSettings <omniscript.capturetemplate.GraphsSettings>`
    object.
    """

    indexing = None
    """The Indexing Settings of the capture. Initialized to a
    :class:`IndexingSettings <omniscript.capturetemplate.IndexingSettings>`
    object.
    """

    plugins = None
    """The Analysis Modules of the capture. Initialized to a
    :class:`AnalysisModules <omniscript.capturetemplate.AnalysisModules>`
    object.
    """

    plugins_config = None
    """The list of Plugins (Analysis Modules) configuration."""

    repeat_trigger = None
    """The capture's Repeat Trigger option. Initialzed to None so that
    value is not set. Set this attribute to '1' to enable, or '0' to disable
    the Repeat Trigger option.
    """

    start_trigger = None
    """The Start Trigger of the capture. Initialized to a
    :class:`TriggerSettings <omniscript.capturetemplate.TriggerSettings>`
    object.
    """

    statistics_output = None
    """The Statistics Output settings of the capture. Initialized to a
    :class:`StatisticsOutputSettings <omniscript.capturetemplate.StatisticsOutputSettings>`
    object.
    """

    statistics_output_preferences = None
    """The Statistics Output settings of the capture. Initialized to a
    :class:`StatisticsOutputSettings <omniscript.capturetemplate.StatisticsOutputSettings>`
    object.
    """

    stop_trigger = None
    """The Stop Trigger of the capture. Initialized to a
    :class:`TriggerSettings <omniscript.capturetemplate.TriggerSettings>`
    object.
    """

    voip = None
    """The VoIP configuration
    :class:`VoIP Settings <omniscript.capturetemplate.VoIPSettings>`
    object.
    """

    _default_filename = False
    """Is the default capture template being used?"""

    _load_from = LOAD_FROM_NONE

    #XML Tags
    _tag_adapter = "AdapterSettings"
    _tag_alarms = "AlarmConfig"
    _tag_analysis = "PerfConfig"
    _tag_filter = "FilterConfig"
    _tag_general = "GeneralSettings"
    _tag_graphs = "GraphSettings"
    _tag_indexing = "IndexingSettings"
    _tag_plugins = "PluginsList"
    _tag_plugins_config = "PluginsConfig"
    _tag_repeat = "RepeatTrigger"
    _tag_root_name = _tag_props
    _tag_start = "StartTrigger"
    _tag_stats = "StatsOutput"
    _tag_stats_prefs = "StatsOutputPrefs"
    _tag_stop = "StopTrigger"
    _tag_voip = "VoIPConfig"

    def __init__(self, filename=None, node=None, engine=None):
        self._load_from = CaptureTemplate._load_from
        self.filename = filename
        if self.filename is not None and len(self.filename) == 0:
            self.filename = None
        self.node = node
        self.adapter = AdapterSettings()
        self.alarms = AlarmSettings()
        self.analysis = AnalysisSettings()
        self.filter = FilterSettings()
        self.general = GeneralSettings()
        self.graphs = GraphsSettings()
        self.indexing = IndexingSettings()
        self.plugins = AnalysisModules()
        self.plugins_config = None
        self.repeat_trigger = None
        self.start_trigger = TriggerSettings(CaptureTemplate._tag_start)
        self.statistics_output = StatisticsOutputSettings()
        self.stop_trigger = TriggerSettings(CaptureTemplate._tag_stop)
        self.voip = VoIPSettings()
        if not self.filename:
            _dirname = os.path.dirname(omniscript.__file__)
            self.filename = os.path.join(_dirname, "_capture_template.xml")
            self._default_filename = True
        if self.filename is not None:
            tree = ET.parse(self.filename)
            props = _find_properties(tree)
            self._load(props, engine)
            self._load_from = LOAD_FROM_FILE
        if node is not None:
            self._load(node, engine)
            self._load_from = LOAD_FROM_NODE

    def __repr__(self):
        return 'CaptureTemplate: %s' % self.general.name

    def _load(self, props, engine):
        if props is not None:
            for obj in props.findall(_tag_object):
                objName = obj.attrib[_tag_name]
                if objName == CaptureTemplate._tag_adapter:
                    self.adapter._load(obj)
                elif objName == CaptureTemplate._tag_alarms:
                    self.alarms._load(obj)
                elif objName == CaptureTemplate._tag_analysis:
                    self.analysis._load(obj)
                elif objName == CaptureTemplate._tag_filter:
                    self.filter._load(obj)
                elif objName == CaptureTemplate._tag_general:
                    self.general._load(obj)
                elif objName == CaptureTemplate._tag_graphs:
                    self.graphs._load(obj)
                elif objName == CaptureTemplate._tag_indexing:
                    self.indexing._load(obj)
                elif objName == CaptureTemplate._tag_plugins:
                    self.plugins._load(obj, engine)
                elif objName == CaptureTemplate._tag_plugins_config:
                    self.plugins_config = None
                elif objName == CaptureTemplate._tag_start:
                    if self.start_trigger is None:
                        self.start_trigger = TriggerSettings(CaptureTemplate._tag_start)
                    self.start_trigger._load(obj, engine)
                elif objName == CaptureTemplate._tag_stats:
                    self.statistics_output._load(obj)
                elif objName == CaptureTemplate._tag_stop:
                    if self.stop_trigger is None:
                        self.stop_trigger = TriggerSettings(CaptureTemplate._tag_stop)
                    self.stop_trigger._load(obj, engine)
                elif objName == CaptureTemplate._tag_voip:
                    self.voip._load(obj)
            for prop in props.findall(_tag_prop):
                propName = prop.attrib[_tag_name]
                if propName == CaptureTemplate._tag_repeat:
                    self.repeat_trigger = int(prop.text)


    def add_filter(self, omnifilter):
        """Add a filter to the capture template."""
        name = None
        if isinstance(omnifilter, basestring):
            name = omnifilter
        if isinstance(omnifilter, Filter):
            name = omnifilter.name
        if name is not None:
            if name not in self.filter.filters:
                self.filter.filters.append(name)

    def add_filters(self, *filters):
        """Add filters to the capture template."""
        for item in filters:
            if isinstance(item, (list, tuple)):
                for f in item:
                    self.add_filters(f)
            else:
                self.add_filter(item)

    def save(self, filename, engine=None):
        """Save the capture template to file."""
        f = open(filename, "w")
        f.write(self.to_xml(engine, True))   #True: pretty print.
        f.close

    def set_adapter(self, value):
        """Set the adapter; only it's name/description is stored."""
        if isinstance(value, basestring):
            self.adapter.adapter_type = config.ADAPTER_TYPE_UNKNOWN
            self.adapter.name = value
        elif isinstance(value, Adapter):
            self.adapter.adapter_type = value.adapter_type
            self.adapter.name = value.name
        elif isinstance(value, FileAdapter):
            self.adapter.adapter_type = config.ADAPTER_TYPE_FILE
            self.adapter.name = value.filename
            self.adapter.limit = value.limit
            self.adapter.mode = value.mode
            self.adapter.speed = value.speed

    def set_filters(self, filter_list):
        """Set the filters of the capture."""
        self.filter.filters = []
        self.add_filters(filter_list)

    def set_multistream(self, enable=False):
        """Set the capture multistream."""
        self.general.set_multistream(enable)

    def set_all(self, enable=True):
        """Set all analysis options, default is True.
        When True all Analysis Options are enabled, all plugins are enabled
        and all Timeline Stats are enabled.
        When False the above are all disabled.
        """
        self.analysis.set_all(enable)
        self.plugins.set_all(enable)
        self.general.set_timeline(enable)

    def set_repeat_trigger(self, count):
        """Set the Repeat Trigger option.

        Args:
            count(int): the number of time the trigger will repeat.
        """
        self.repeat_trigger = int(count)

    def to_xml(self, engine=None, new=False, pretty=False, modify=False):
        """Return the Capture Template encoded in XML as a string."""
        if self._load_from == LOAD_FROM_FILE or self._load_from == LOAD_FROM_NONE:
            if self._default_filename and engine:
                _dirname = os.path.dirname(omniscript.__file__)
                hp = engine.get_host_platform()
                if hp and hp.os == omniscript.OPERATING_SYSTEM_LINUX:
                    _filename = os.path.join(_dirname, "_capture_template_linux.xml")
                else:
                    _filename = os.path.join(_dirname, "_capture_template_windows.xml")
            else:
                _filename = self.filename
            with open(_filename, "r") as xmlfile:
                xml = xmlfile.read().replace('\n', '').replace('\t', '')

        if new:
            self.general.id = OmniId(True)

        if self._load_from == LOAD_FROM_NODE and self.node is not None:
            props = self.node
        else:
            template = ET.fromstring(xml)
            props = _find_properties(template)

        for obj in props.findall(_tag_object):
            objName = obj.attrib[_tag_name]
            if objName == CaptureTemplate._tag_adapter:
                self.adapter._store(obj, engine)
            #elif objName == CaptureTemplate._tag_alarms:
            #    self.alarms._store(obj)
            elif objName == CaptureTemplate._tag_analysis:
                self.analysis._store(obj)
            elif objName == CaptureTemplate._tag_filter:
                self.filter._store(obj, engine)
            elif objName == CaptureTemplate._tag_general:
                self.general._store(obj)
            elif objName == CaptureTemplate._tag_graphs:
                self.graphs._store(obj)
            elif objName == CaptureTemplate._tag_indexing:
                self.indexing._store(obj)
            elif objName == CaptureTemplate._tag_plugins:
                self.plugins._store(obj, engine, self.analysis.option_compass)
            elif objName == CaptureTemplate._tag_start and \
                    self.start_trigger is not None:
                self.start_trigger._store(obj, engine)
            elif objName == CaptureTemplate._tag_stop and \
                    self.stop_trigger is not None:
                self.stop_trigger._store(obj, engine)
        if self.repeat_trigger is not None:
            _set_property(props, CaptureTemplate._tag_repeat, 22, 
                    int(self.repeat_trigger))

        #template = ET.Element("capturetemplate", {"version":"1.0"})
        #props = ET.SubElement(template, "properties")
        #self.adapter._store(ET.SubElement(props, "obj"), engine)
        #self.alarms._store(ET.SubElement(props, "obj"))
        #self.analysis._store(ET.SubElement(props, "obj"))
        #self.filter._store(ET.SubElement(props, "obj"), engine)
        #self.general._store(ET.SubElement(props, "obj"))
        #self.graphs._store(ET.SubElement(props, "obj"))
        #self.start_trigger._store(ET.SubElement(props, "obj"), engine)
        #self.statistics_output._store(ET.SubElement(props, "obj"))
        #self.stop_trigger._store(ET.SubElement(props, "obj"), engine)
        #self.voip._store(ET.SubElement(props, "obj"))
        #
        #_set_property(props, CaptureTemplate._tag_repeat, "22",
        #              self.repeat_trigger)

        if modify:
            mod = ET.Element(_tag_props)
            obj = ET.SubElement(mod, _tag_object)
            _set_label_clsid(obj, 'options', _tag_prop_bag)
            _set_property(mod, _tag_id, 8, str(self.general.id))
            obj.append(props)
            props = mod

        if props is None:
            raise OmniError("Failed to create CaptureTemplate")
        return ET.tostring(props).replace('\n', '')


def get_graph_template_names():
    """ Returns the list of Graph Template Names."""
    id_graph_names = omniscript.get_id_graph_names()
    return id_graph_names.values() if id_graph_names is not None else None
