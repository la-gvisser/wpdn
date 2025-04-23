"""StatsContext class.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

# pylint: disable-msg=w0614

import os

import config

from applicationstatistic import ApplicationStatistic
from callstatistic import CallStatistic
from countrystatistic import CountryStatistic
from mediaspec import *
from nodestatistic import NodeStatistic
from omnierror import OmniError
from peektime import PeekTime
from protocolstatistic import ProtocolStatistic
from readstream import ReadStream
from statslimit import StatsLimit
from summarystatistic import SummaryStatistic

ENTITY_MAGIC = 0x6865
ENTITY_V2 = 0x0200
ENTITY_V2_COUNT = 12
ENTITY_V3 = 0x0300
ENTITY_V3_COUNT = 23
ENTITY_V4 = 0x0400
ENTITY_V4_COUNT = 24
ENTITY_V5 = 0x0500
ENTITY_V5_COUNT = 25

ENTITY_V2_MASK = 0x0FFFFFFF
ENTITY_V3_MASK = 0x00FFFFFF
ENTITY_V4_MASK = 0x00FFFFFF
ENTITY_V5_MASK = 0x00FFFFFF

ENTITY_MASK = {
    ENTITY_V2 : ENTITY_V2_MASK,
    ENTITY_V3 : ENTITY_V3_MASK,
    ENTITY_V4 : ENTITY_V4_MASK,
    ENTITY_V5 : ENTITY_V5_MASK,
}

ENTITY_V2_SHIFT = 28
ENTITY_V3_SHIFT = 24
ENTITY_V4_SHIFT = 24
ENTITY_V5_SHIFT = 24

ENTITY_SHIFT = {
    ENTITY_V2 : ENTITY_V2_SHIFT,
    ENTITY_V3 : ENTITY_V3_SHIFT,
    ENTITY_V4 : ENTITY_V4_SHIFT,
    ENTITY_V5 : ENTITY_V5_SHIFT
}

ENTITY_ETHERNET_PROTOCOL = 0
ENTITY_LSAP = 1
ENTITY_SNAP = 2
ENTITY_MAC_CONTROL = 3
ENTITY_PROTOSPEC = 4
ENTITY_ETHERNET = 5
ENTITY_IP = 6
ENTITY_APPLETALK = 7
ENTITY_DECNET = 8
ENTITY_IPV6 = 9
ENTITY_WIRELESS = 10
ENTITY_IPX = 11
ENTITY_IP_PORT = 12
ENTITY_TCP_PORT_PAIR = 13
ENTITY_WAN_PPP_PROTO = 14
ENTITY_WAN_FRAMERELAY_PROTO = 15
ENTITY_WAN_X25_PROTO = 16
ENTITY_WAN_X25E_PROTO = 17
ENTITY_WAN_U200_PROTO = 18
ENTITY_WAN_IPARS_PROTO = 19
ENTITY_WAN_Q931_PROTO = 20
ENTITY_TOKENRING = 21
ENTITY_WAN_DLCI = 22
ENTITY_APPLICATION_ID = 23
ENTITY_PROTOSPEC_HIERARCHY = 24

LIMITS_MAGIC = 0x6D696C73
LIMITS_V1 = 1
LIMITS_V2 = 2

SNAPSHOT_MAGIC = 0x6dd7
SNAPSHOT_V1 = 0x0100
SNAPSHOT_V2 = 0x0200

TUPLES_MAGIC = 0x6966
TUPLES_V1 = 0x0100
TUPLES_V1_COUNT = 7
TUPLES_V2 = 0x0200
TUPLES_V2_COUNT = 9

TUPLES_ETHERNET = 0
TUPLES_APPLETALK = 1
TUPLES_IP = 2
TUPLES_DECNET = 3
TUPLES_IPV6 = 4
TUPLES_WIRELESS = 5
TUPLES_IPX = 6
TUPLES_WAN_DLCI = 7
TUPLES_TOKENRING = 8

ENTITY_TYPE = 0
ENTITY_CLASS = 1
_entity_type_class = [
    #  0 ENTITY_ETHERNET_PROTOCOL
    [ config.MEDIA_SPEC_ETHERNET_PROTOCOL, config.MEDIA_CLASS_PROTOCOL ],       
    #  1 ENTITY_LSAP
    [ config.MEDIA_SPEC_LSAP, config.MEDIA_CLASS_PROTOCOL ],                    
    #  2 ENTITY_SNAP
    [ config.MEDIA_SPEC_SNAP, config.MEDIA_CLASS_PROTOCOL ],                    
    #  3 ENTITY_MAC_CONTROL
    [ config.MEDIA_SPEC_MAC_CONTROL, config.MEDIA_CLASS_PROTOCOL ],             
    #  4 ENTITY_PROTOSPEC
    [ config.MEDIA_SPEC_PROTO_SPEC, config.MEDIA_CLASS_PROTOCOL ],              
    #  5 ENTITY_ETHERNET
    [ config.MEDIA_SPEC_ETHERNET_ADDRESS, config.MEDIA_CLASS_ADDRESS ],         
    #  6 ENTITY_IP
    [ config.MEDIA_SPEC_IP_ADDRESS, config.MEDIA_CLASS_ADDRESS ],               
    #  7 ENTITY_APPLETALK
    [ config.MEDIA_SPEC_APPLETALK_ADDRESS, config.MEDIA_CLASS_ADDRESS ],        
    #  8 ENTITY_DECNET
    [ config.MEDIA_SPEC_DECNET_ADDRESS, config.MEDIA_CLASS_ADDRESS ],           
    #  9 ENTITY_IPV6
    [ config.MEDIA_SPEC_IPV6_ADDRESS, config.MEDIA_CLASS_ADDRESS ],             
    # 10 ENTITY_WIRELESS
    [ config.MEDIA_SPEC_WIRELESS_ADDRESS, config.MEDIA_CLASS_ADDRESS ],         
    # 11 ENTITY_IPX
    [ config.MEDIA_SPEC_IPX_ADDRESS, config.MEDIA_CLASS_ADDRESS ],              
    # 12 ENTITY_IP_PORT
    [ config.MEDIA_SPEC_IP_PORT, config.MEDIA_CLASS_PORT ],                     
    # 13 ENTITY_TCP_PORT_PAIR
    [ config.MEDIA_SPEC_TCP_PORT_PAIR, config.MEDIA_CLASS_PORT ],               
    # 14 ENTITY_WAN_PPP_PROTO
    [ config.MEDIA_SPEC_WAN_PPP_PROTOCOL, config.MEDIA_CLASS_PROTOCOL ],        
    # 15 ENTITY_WAN_FRAMERELAY_PROTO
    [ config.MEDIA_SPEC_WAN_FRAMERELAY_PROTOCOL, config.MEDIA_CLASS_PROTOCOL ], 
    # 16 ENTITY_WAN_X25_PROTO
    [ config.MEDIA_SPEC_WAN_X25_PROTOCOL, config.MEDIA_CLASS_PROTOCOL ],        
    # 17 ENTITY_WAN_X25E_PROTO
    [ config.MEDIA_SPEC_WAN_X25E_PROTOCOL, config.MEDIA_CLASS_PROTOCOL ],       
    # 18 ENTITY_WAN_U200_PROTO
    [ config.MEDIA_SPEC_WAN_U200_PROTOCOL, config.MEDIA_CLASS_PROTOCOL ],       
    # 19 ENTITY_WAN_IPARS_PROTO
    [ config.MEDIA_SPEC_WAN_IPARS_PROTOCOL, config.MEDIA_CLASS_PROTOCOL ],      
    # 20 ENTITY_WAN_Q931_PROTO
    [ config.MEDIA_SPEC_WAN_Q931_PROTOCOL, config.MEDIA_CLASS_PROTOCOL ],       
    # 21 ENTITY_TOKENRING
    [ config.MEDIA_SPEC_TOKENRING_ADDRESS, config.MEDIA_CLASS_ADDRESS ],        
    # 22 ENTITY_WAN_DLCI
    [ config.MEDIA_SPEC_WAN_DLCI_ADDRESS, config.MEDIA_CLASS_ADDRESS ],
    # 23 ENTITY_APPLICATION_ID
    [ config.MEDIA_SPEC_APPLICATION_ID, config.MEDIA_CLASS_PROTOCOL ],
    # 24 ENTITY_PROTOSPEC_HIERARCHY
    [ config.MEDIA_SPEC_PROTOSPEC_HIERARCHY, config.MEDIA_CLASS_PROTOCOL ]
    ]


class EntityTable(object):
    version = 0
    """version: ENTITY_V2, ENTITY_V3, ENTITY_V4 or ENTITY_V5"""

    tables = None
    """A dictionary of tables { ENTITY_type:table }"""

    def __init__(self, stream):
        self.version = EntityTable.version
        self.tables = []
        if stream is not None:
            self._load(stream)

    def _load(self, stream):
        magic = stream.read_ushort()
        if magic != ENTITY_MAGIC:
            raise OmniError('Invalid Entity Constant.')
        self.version = stream.read_ushort()
        if self.version == ENTITY_V5:
            table_count = ENTITY_V5_COUNT
        elif self.version == ENTITY_V4:
            table_count = ENTITY_V4_COUNT
        elif self.version == ENTITY_V3:
            table_count = ENTITY_V3_COUNT
        elif self.version == ENTITY_V2:
            table_count = ENTITY_V2_COUNT
        else:
            raise OmniError('Unsupported Entity version.')
        for i in range(table_count):
            count = stream.read_uint()
            sub_size = stream.read_ushort()
            if sub_size > 64:
                raise OmniError('Unsupported Entity sub table count.')
            items = None
            if i == ENTITY_PROTOSPEC and self.version > ENTITY_V2:
                if count > 0:
                    items = []
                    for _ in range(count):
                        sub_item = []
                        parent_count = stream.read_uint()
                        if parent_count == 0:
                            parent_count = 1
                        for _ in range(parent_count):
                            sub_item.append(MediaSpec(
                                _entity_type_class[i][ENTITY_CLASS],
                                _entity_type_class[i][ENTITY_TYPE],
                                stream.read_ushort()))
                        items.append(sub_item)
            else:
                if (count * sub_size) > 0:
                    items = []
                    for _ in range(count):
                        items.append(MediaSpec(
                            _entity_type_class[i][ENTITY_CLASS],
                            _entity_type_class[i][ENTITY_TYPE],
                            stream.read(sub_size)))
            self.tables.append(items)

    def get_entity(self, id):
        table = self.get_table(id)
        if table is not None:
            index = self.get_index(id)
            return table[index]
        return None

    def get_index(self, id):
        return id & ENTITY_MASK[self.version]

    def get_type(self, id):
        return id >> ENTITY_SHIFT[self.version]

    def get_type_index(self, id):
        return self.get_index(id), self.get_type(id)

    def get_table(self, id):
        _type = self.get_type(id)
        if _type < len(self.tables):
            return self.tables[_type]
        return None


class ApplicationStatistics(object):
    limit = None
    items = None

    def __init__(self, entities, stream, offset=0):
        self.limit = ApplicationStatistics.limit
        self.items = []
        if stream is not None:
            self._load(entities, stream, offset)

    def _load(self, entities, stream, offset):
        class_name_ids = omniscript.get_class_name_ids()
        if offset > 0:
            stream.seek(offset, os.SEEK_SET)
        clsid = stream.read_guid()
        stream.read_guid()          # objid
        if clsid != class_name_ids['ApplicationStats']:
            raise OmniError('Application Statistics not found.')
        # Load the limit
        self.limit = StatsLimit(None, stream)
        # Load statistics
        count = stream.read_uint()
        for _ in range(count):
            id = stream.read_uint()
            entity = entities.get_entity(id)
            self.items.append(ApplicationStatistic(entity, stream=stream))

    def get(self):
        return self.items


class CountryStatistics(object):
    items = None

    def __init__(self, stream, offset=0):
        self.items = []
        if stream is not None:
            self._load(stream, offset)

    def _load(self, stream, offset):
        class_name_ids = omniscript.get_class_name_ids()
        if offset > 0:
            stream.seek(offset, os.SEEK_SET)
        clsid = stream.read_guid()
        stream.read_guid()          # objid
        if clsid != class_name_ids['CountryStats']:
            raise OmniError('Country Statistics not found.')
        # Load statistics
        count = stream.read_uint()
        for _ in range(count):
            self.items.append(CountryStatistic(stream))

    def get(self):
        return self.items


class NodeStatistics(object):
    resets = 0
    time_limit = 0
    items = None

    def __init__(self, entities, stream, offset=0):
        self.resets = NodeStatistics.resets
        self.time_limit = NodeStatistics.time_limit
        self.items = []
        if stream is not None:
            self._load(entities, stream, offset)

    def _load(self, entities, stream, offset):
        class_name_ids = omniscript.get_class_name_ids()
        if offset > 0:
            stream.seek(offset, os.SEEK_SET)
        clsid = stream.read_guid()
        stream.read_guid()          # objid
        if clsid != class_name_ids['NodeStats']:
            raise OmniError('Node Statistics not found.')
        # Load Limits
        magic = stream.read_uint()
        if magic == LIMITS_MAGIC:
            version = stream.read_ushort()
        else:
            stream.seek(4, os.SEEK_CUR)
            version = LIMITS_V1
            self.resets = 0
            self.time_limit = 0
        if version == LIMITS_V2:
            self.resets = stream.read_uint()
            self.time_limit = stream.read_ulong()
        elif version == LIMITS_V1:
            self.resets = stream.read_uint()
            self.time_limit = 0
        else:
            raise OmniError('Unsupported Node Statistics version.')
        # Load statistics
        count = stream.read_uint()
        for _ in range(count):
            id = stream.read_uint()
            entity = entities.get_entity(id)
            self.items.append(NodeStatistic(entity, stream=stream))

    def get(self):
        return self.items


class ProtocolStatistics(object):
    version = 0
    resets = 0
    time_limit = 0
    items = None

    def __init__(self, entities, stream, offset=0):
        self.version = ProtocolStatistics.version
        self.resets = ProtocolStatistics.resets
        self.time_limit = ProtocolStatistics.time_limit
        self.items = []
        if stream is not None:
            self._load(entities, stream, offset)

    def _load(self, entities, stream, offset):
        class_name_ids = omniscript.get_class_name_ids()
        if offset > 0:
            stream.seek(offset, os.SEEK_SET)
        clsid = stream.read_guid()
        stream.read_guid()          #objid
        if clsid != class_name_ids['ProtocolStats']:
            raise OmniError('Protocol Statistics not found.')
        # Load Limits
        magic = stream.read_uint()
        if magic == LIMITS_MAGIC:
            self.version = stream.read_ushort()
        else:
            stream.seek(4, os.SEEK_CUR)
            self.version = LIMITS_V1
        if self.version == LIMITS_V2:
            self.resets = stream.read_uint()
            self.time_limit = stream.read_ulong()
        elif self.version == LIMITS_V1:
            self.resets = stream.read_uint()
            self.time_limit = 0
        else:
            raise OmniError('Unsupported Protocol Statistics version.')
        # Load statistics
        count = stream.read_uint()
        if entities is not None:
            for _ in range(count):
                id = stream.read_uint()
                layers = entities.get_entity(id)
                self.items.append(ProtocolStatistic(layers=layers,
                                                    stream=stream))

    def get(self):
        return self.items


class StatisticsTable(object):
    _entity_table = None
    stream = None
    application_offset = 0
    application_stats = None
    call_offset = 0
    call_stats = None
    country_offset = 0
    country_stats = None
    node_offset = 0
    node_stats = None
    protocol_offset = 0
    protocol_stats = None
    summary_offset = 0
    summary_stats = None

    def __init__(self, entities, stream):
        self._entity_table = entities
        self.stream = stream
        self.application_offset = StatisticsTable.application_offset
        self.application_stats = StatisticsTable.application_stats
        self.call_offset = StatisticsTable.call_offset
        self.call_stats = StatisticsTable.call_stats
        self.country_offset = StatisticsTable.country_offset
        self.country_stats = StatisticsTable.country_stats
        self.node_stats = StatisticsTable.node_stats
        self.protocol_offset = StatisticsTable.protocol_offset
        self.protocol_stats = StatisticsTable.protocol_stats
        self.summary_offset = StatisticsTable.summary_offset
        self.summary_stats = StatisticsTable.summary_stats
        if self.stream is not None:
            self._load()

    def _load(self):
        class_name_ids = omniscript.get_class_name_ids()
        if self.stream is None:
            return
        self.stream.tell()          # pos
        self.application_offset = self.stream.find(
            class_name_ids['ApplicationStats'].bytes_le())
        self.call_offset = self.stream.find(
            class_name_ids['CallStats'].bytes_le())
        self.country_offset = self.stream.find(
            class_name_ids['CountryStats'].bytes_le())
        self.node_offset = self.stream.find(
            class_name_ids['NodeStats'].bytes_le())
        self.protocol_offset = self.stream.find(
            class_name_ids['ProtocolStats'].bytes_le())
        self.summary_offset = self.stream.find(
            class_name_ids['SummaryStats'].bytes_le())

        if __debug__:
            offsets = []
            offsets.append(self.stream.find(
                class_name_ids['DashboardStats'].bytes_le()))
            offsets.append(self.stream.find(
                class_name_ids['NetworkStats'].bytes_le()))
            offsets.append(self.stream.find(
                class_name_ids['ErrorStats'].bytes_le()))
            offsets.append(self.node_offset)
            offsets.append(self.stream.find(
                class_name_ids['NodeStatsHierarchy'].bytes_le()))
            offsets.append(self.protocol_offset)
            offsets.append(self.stream.find(
                class_name_ids['ProtocolStatsByID'].bytes_le()))
            offsets.append(self.stream.find(
                class_name_ids['ConversationStats'].bytes_le()))
            offsets.append(self.stream.find(
                class_name_ids['SizeStats'].bytes_le()))
            offsets.append(self.summary_offset)
            offsets.append(self.stream.find(
                class_name_ids['ExpertStats'].bytes_le()))

    def get_application_statistics(self):
        if self.application_stats is None:
            if self.stream is None or self.application_offset <= 0:
                return None
            self.application_stats = ApplicationStatistics(self._entity_table,
                                                           self.stream,
                                                           self.application_offset)
        return self.application_stats.get()

    def get_call_statistics(self):
        if self.call_stats is None:
            if self.stream is None or self.call_offset <= 0:
                return None
            self.call_stats = CallStatistic(self.stream,
                                            self.call_offset)
        return self.call_stats.get()

    def get_country_statistics(self):
        if self.country_stats is None:
            if self.stream is None or self.country_offset <= 0:
                return None
            self.country_stats = CountryStatistics(self.stream,
                                            self.country_offset)
        return self.country_stats.get()

    def get_node_statistics(self):
        if self.node_stats is None:
            if self.stream is None or self.node_offset <= 0:
                return None
            self.node_stats = NodeStatistics(self._entity_table,
                                             self.stream,
                                             self.node_offset)
        return self.node_stats.get()

    def get_protocol_statistics(self):
        if self.protocol_stats is None:
            if self.stream is None or self.protocol_offset <= 0:
                return None
            self.protocol_stats = ProtocolStatistics(self._entity_table,
                                                     self.stream,
                                                     self.protocol_offset)
        return self.protocol_stats.get()

    def get_summary_statistics(self):
        if self.summary_stats is None:
            if self.stream is None or self.summary_offset <= 0:
                return None
            self.summary_stats = SummaryStatistics(self.stream,
                                                   self.summary_offset)
        return self.summary_stats.get()


class SummarySnapshot(object):
    id = None
    name = None
    items = None

    def __init__(self, stream):
        self.id = SummarySnapshot.id
        self.name = SummarySnapshot.name
        self.items = {}
        if stream is not None:
            self._load(stream)

    def _load(self, stream):
        magic = stream.read_ushort()
        if magic == SNAPSHOT_MAGIC:
            version = stream.read_ushort()
        else:
            stream.seek(-2, os.SEEK_CUR)
            version = SNAPSHOT_V1
        if version == SNAPSHOT_V2:
            self.id = stream.read_guid()
            self.name = stream.read_unicode()
            count = stream.read_uint()
            _stat_names = {}
            for _ in range(count):
                ss = SummaryStatistic(stream=stream, names=_stat_names)
                if ss.parent == '':
                    _stat_names[ss.id] = ss.name
                else:
                    if ss.parent not in self.items:
                        self.items[ss.parent] = {}
                    self.items[ss.parent][ss.name] = ss

    def get(self):
        return self.items


class SummaryStatistics(object):
    current_id = None
    id = None
    snapshots = None

    def __init__(self, stream, offset=0):
        self.current_id = SummaryStatistics.current_id
        self.id = SummaryStatistics.id
        self.snapshots = []
        if stream is not None:
            self._load(stream, offset)

    def _load(self, stream, offset=0):
        class_name_ids = omniscript.get_class_name_ids()
        if offset > 0:
            stream.seek(offset, os.SEEK_SET)
        clsid = stream.read_guid()
        stream.read_guid()          # objid
        if clsid != class_name_ids['SummaryStats']:
            raise OmniError('Summary Statistics not found.')
        self.current_id = stream.read_guid()
        count = stream.read_uint()
        self.id = stream.read_guid()
        for _ in range(count):
            self.snapshots.append(SummarySnapshot(stream))

    def get(self):
        if self.snapshots is not None and len(self.snapshots) > 0:
            return self.snapshots[0].get()
        return None


class TuplesTable(object):
    version = 0
    table_count = 0
    sub_tables = None

    def __init__(self, stream):
        self.version = TuplesTable.version
        self.table_count = TuplesTable.table_count
        self.sub_tables = []
        if stream is not None:
            self._load(stream)

    def _load(self, stream):
        magic = stream.read_ushort()
        if magic == TUPLES_MAGIC:
            self.version = stream.read_ushort()
        else:
            stream.seek(-2, os.SEEK_CUR)
            self.version = TUPLES_V1
        if self.version == TUPLES_V2:
            self.table_count = TUPLES_V2_COUNT
        elif self.version == TUPLES_V1:
            self.table_count = TUPLES_V1_COUNT
        else:
            raise OmniError('Unsupported Tuple Table version.')
        for _ in range(self.table_count):
            count = stream.read_uint()
            for _ in range(count):
                stream.read_uint()      # src
                stream.read_uint()      # dst
                stream.read_uint()      # protocol


class StatsContext(object):
    """Holds all the statistics."""

    request_time = None
    """The timestamp of the last refresh."""

    media_type = 0
    """The Adapter's media type."""

    media_sub_type = 0
    """The Adapter's media sub type."""

    link_speed = 0
    """The Adapter's link speed."""

    start_time = None
    """The Start Time of the capture
    :class:`PeekTime <omniscript.peektime.PeekTime>`.
    """

    end_time = None
    """The End Time of the capture as
    :class:`PeekTime <omniscript.peektime.PeekTime>`.
    """

    total_packets = 0
    """The total number of packets in the capture."""

    total_bytes = 0
    """The total number of bytes in the capture."""

    _entity_table = None
    _tuples_table = None
    _stats_table = None

    def __init__(self, data=None):
        self.request_time = StatsContext.request_time
        self.media_type = StatsContext.media_type
        self.media_sub_type = StatsContext.media_sub_type
        self.link_speed = StatsContext.link_speed
        self.start_time = StatsContext.start_time
        self.end_time = StatsContext.end_time
        self.total_packets = StatsContext.total_packets
        self.total_bytes = StatsContext.total_bytes
        self._entity_table = None
        self._tuples_table = None
        self._stats_table = None
        if data is not None:
            self._load(data)

    def _load(self, data):
        stream = ReadStream(data)
        self.request_time = PeekTime(stream.read_ulong())
        self.media_type = stream.read_uint()
        self.media_sub_type = stream.read_uint()
        ls_32 = stream.read_uint()
        if ls_32 == 0xABCD1234:
            # 64-bit link speed follows.
            self.link_speed = stream.read_ulong()
        else:
            self.link_speed = ls_32
        self.start_time = PeekTime(stream.read_ulong())
        self.end_time = PeekTime(stream.read_ulong())
        self.total_packets = stream.read_ulong()
        self.total_bytes = stream.read_ulong()
        self._entity_table = EntityTable(stream)
        self._tuples_table = TuplesTable(stream)
        self._stats_table = StatisticsTable(self._entity_table, stream)

    def get_application_statistics(self):
        if self._stats_table is not None:
            return self._stats_table.get_application_statistics()
        return None

    def get_call_statistics(self):
        if self._stats_table is not None:
            return self._stats_table.get_call_statistics()
        return None

    def get_country_statistics(self):
        if self._stats_table is not None:
            return self._stats_table.get_country_statistics()
        return None

    def get_node_statistics(self):
        if self._stats_table is not None:
            return self._stats_table.get_node_statistics()
        return None

    def get_protocol_statistics(self):
        if self._stats_table is not None:
            return self._stats_table.get_protocol_statistics()
        return None

    def get_summary_statistics(self):
        if self._stats_table is not None:
            return self._stats_table.get_summary_statistics()
        return None
