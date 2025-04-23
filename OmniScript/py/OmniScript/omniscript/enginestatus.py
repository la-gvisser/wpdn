"""EngineStatus class.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

import os
import sys
import logging

import config
import omniscript

from peektime import PeekTime


_engine_prop_dict = {
    'AdapterCount' : 'adapter_count',
    'AlarmCount' : 'alarm_count',
    'Captures' : 'capture_count',
    'CaptureSessionCount' : 'capture_session_count',
    'CpuCount' : 'cpu_count',
    'CpuType' : 'cpu_type',
    'DataFolder' : 'data_folder',
    'engine-type' : 'engine_type',
    'FileCount' : 'file_count',
    'FileVersion' : 'file_version',
    'FilterCount' : 'filter_count',
    'FiltersModTime' : 'filters_modification_time',
    'ForensicSearches' : 'forensic_search_count',
    'GraphCount' : 'graph_count',
    'LogTotalCount' : 'log_total_count',
    'MemAvailPhys' : 'memory_available_physical',
    'MemTotalPhys' : 'memory_total_physical',
    'Name' : 'name',
    'NameCount' : 'name_table_count',
    'NotificationCount' : 'notification_count',
    'OperatingSystem' : 'operating_system',
    'OS' : 'os',
    'Platform' : 'platform',
    'ProductVersion' : 'product_version',
    'StorageTotal' : 'storage_total',
    'StorageUsed' : 'storage_used',
    'Time' : 'time',
    'TimeZoneBias' : 'time_zone_bias',
    'Uptime' : 'uptime',
    }


def _parse_prop(a, v):
    result = None
    if a == 'storage_free_space':
        result = []
        for i in v.split(';'):
            k, v = i.strip(' ').split(' ', 1)
            # 69.4 GB -> 74,517,053,440 bytes
            cnt, unt, fs = v.split(' ', 2)
            cnt = float(cnt) * config.BYTES_IN_KILOBYTE # 1.23 -> 1230.0
            cnt = int(cnt)                              # -> 1230 kB
            if unt == 'MB':
                cnt = cnt * config.KILOBYTES_IN_MEGABYTE
            elif unt == 'GB':
                cnt = cnt * config.KILOBYTES_IN_GIGABYTE
            elif unt == 'TB':
                cnt = cnt * config.KILOBYTES_IN_TERABYTE
            result.append([k, cnt, fs])
    return result


class EngineStatus(object):
    """The Status of an :class:`OmniEngine <omniscript.omniengine.OmniEngine>`
    object.
    """

    adapter_count = 0
    """The number of adapters on the system."""

    alarm_count = 0
    """The number of alarms configured on the OmniEngine."""

    capture_count = 0
    """The number of Captures created on the OmniEngine."""

    capture_session_count = 0
    """The number of Capture Session on the OmniEngine."""

    cpu_count = 0
    """The number of CPUs on the system."""

    cpu_type = ''
    """The type of CPU on the system."""

    data_folder = ''
    """The default data folder of the OmniEngine."""

    engine_type = ''
    """The type of OmniEngine: OmniEngine Enterprise..."""

    file_count = 0
    """The number of Packet files in the OmniEngine's database."""

    file_version = ''
    """The File Version property of the OmniEngine."""

    filter_count = 0
    """The number of Filters on the OmniEngine."""

    filters_modification_time = None
    """The last time the Filters where modified."""

    forensic_search_count = 0
    """The number of Forensic Searches on the OmniEngine."""

    graph_count = 0
    """The number of Graphs on the OmniEngine."""

    host = ''
    """The address of the host system."""

    log_total_count = 0
    """The total number of Log entires on the OmniEngine."""

    memory_available_physical = 0
    """The amount of physical memory in bytes available on the system."""

    memory_total_physical = 0
    """The amount of physical memory in bytes on the system."""

    name = ''
    """The name of the OmniEngine."""

    name_table_count = 0
    """The number of Name Table entires (need to confirm) on the OmniEngine."""

    notification_count = 0
    """The number of Notifications on the OmniEngine."""

    operating_system = ''
    """The full name of the Operating System that the OmniEngine is
    running on.
    """

    os = ''
    """The short name of the Operating System that the OmniEngine is
    running on: Windows or Linux.
    """

    platform = 'localhost'
    """The platform of the system, Windows only: x64, win32."""

    port = 6367
    """The TCP Port of the connection to the host system."""

    product_version = ''
    """The Product Version property of the OmniEngine."""

    storage_available = 0
    """The amount of disk storage in bytes available on the system."""

    storage_total = 0
    """The total amount of disk storage in bytes on the system."""

    storage_used = 0
    """The amount of disk storage in bytes used on the system."""

    time = None
    """The current time, as
    :class:`PeekTime <omniscript.peektime.PeekTime>`,
    of day in UTC on the system."""

    time_zone_bias = 0
    """The time zone bias of the system."""

    uptime = 0
    """The number of nanoseconds the OmniEngine has been running."""

    def __init__(self, engine, logger, node=None):
        self._engine = engine
        self.logger = logger

        self.adapter_count = EngineStatus.adapter_count
        self.alarm_count = EngineStatus.alarm_count
        self.capture_count = EngineStatus.capture_count
        self.capture_session_count = EngineStatus.capture_session_count
        self.cpu_count = EngineStatus.cpu_count
        self.cpu_type = EngineStatus.cpu_type
        self.data_folder = EngineStatus.data_folder
        self.engine_type = EngineStatus.engine_type
        self.file_count = EngineStatus.file_count
        self.file_version = EngineStatus.file_version
        self.filter_count = EngineStatus.filter_count
        self.filters_modification_time = EngineStatus.filters_modification_time
        self.forensic_search_count = EngineStatus.forensic_search_count
        self.graph_count = EngineStatus.graph_count
        self.host = EngineStatus.host
        self.log_total_count = EngineStatus.log_total_count
        self.memory_available_physical = EngineStatus.memory_available_physical
        self.memory_total_physical = EngineStatus.memory_total_physical
        self.name = EngineStatus.name
        self.name_table_count = EngineStatus.name_table_count
        self.notification_count = EngineStatus.notification_count
        self.operating_system = EngineStatus.operating_system
        self.os = EngineStatus.os
        self.platform = EngineStatus.platform
        self.port = EngineStatus.port
        self.product_version = EngineStatus.product_version
        self.storage_available = EngineStatus.storage_available
        self.storage_total = EngineStatus.storage_total
        self.storage_used = EngineStatus.storage_used
        self.time = EngineStatus.time
        self.time_zone_bias = EngineStatus.time_zone_bias
        self.uptime = EngineStatus.uptime
        #Set provided values
        self.host = engine.host
        self.port = engine.port
        #Parse the dictionary.
        if node is not None:
            self._load(node)

    def __repr__(self):
        return 'EngineStatus: %s' % self.name

    def _load(self, node):
        """Set attributes from a dictionary."""
        for prop in node.findall('prop'):
            k = prop.get('name')
            v = prop.text
            a = _engine_prop_dict.get(k)
            if a is not None:
                if hasattr(self, a):
                    if isinstance(getattr(self, a), basestring):
                        setattr(self, a, v if v else '')
                    elif isinstance(getattr(self, a), (int, long)):
                        setattr(self, a, int(v) if v else 0)
                    elif isinstance(getattr(self, a), list):
                        setattr(self, a, _parse_prop(a, v))
                    if getattr(self, a) is None:
                        setattr(self, a, PeekTime(v))
            self.cpu_type = ' '.join(self.cpu_type.split())
            if self.storage_total > self.storage_used:
                self.storage_available = self.storage_total - self.storage_used

    def refresh(self):
        """Refresh the attributes of this object."""
        response = self._engine._issue_xml_command(config.OMNI_GET_STATUS, None)
        props = omniscript._parse_command_response(response, 'properties')
        if props is not None:
            self.filters_modification_time = EngineStatus.filters_modification_time
            self.time = EngineStatus.time
            self._load(props)
