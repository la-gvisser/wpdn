"""Capture class.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

import os
import sys
import logging
import struct
import xml.etree.ElementTree as ET

import config
import omniscript

from adapter import Adapter
from callstatistic import CallStatistic
from expertquery import ExpertQuery
from expertresult import ExpertResult
from mediainfo import MediaInfo
from nodestatistic import NodeStatistic
from omniid import OmniId
from packet import Packet
from peektime import PeekTime
from protocolstatistic import ProtocolStatistic
from readstream import ReadStream
from statscontext import StatsContext
from summarystatistic import SummaryStatistic

from expertresult import _create_expert_result_list
from packet import _create_packet_list


find_attribs = ['name', 'id']

status_map = {
    0 : 'Idle',
    1 : 'Capturing',
    256 : 'Idle Start Active',
    257 : 'Wait Start',
    8192 : 'Idle Stop Active',
    8193 : 'Capturing Stop Active',
    8448 : 'Idle Start and Stop Active',
    8449 : 'Start Stop Active'
}


def _summary_xml_to_stats_list(xml):
    type_dict = [[''], ['date'], ['time'], ['duration'], ['packets'],
                 ['bytes'], ['packets', 'bytes'], ['int'], ['double']]
    element = ET.fromstring(xml)
    lst = []
    snapshot = element.find('summarystats/snapshot')
    for stat in snapshot:
        stat_type = int(stat.attrib['type'])
        s = {'id':stat.attrib['id'],
             'parent':stat.attrib['parent'],
             'type':stat_type,
             'flags':int(stat.attrib['flags'])}
        if stat_type >= 1 and stat_type <= len(type_dict):
            for t in type_dict[stat_type]:
                s[t] = stat.attrib[t]
        lst.append(SummaryStatistic(s))
    return lst


_capt_prop_dict = {
    'Adapter' : 'adapter_name',
    'Adapter Type' : 'adapter_type',
    'Alarms Enabled' : 'option_alarms',
    'Alarms-Info' : 'alarms_info',
    'Alarms-Major' : 'alarms_major',
    'Alarms-Minor' : 'alarms_minor',
    'Alarms-Severe' : 'alarms_severe',
    'Analysis Dropped Packets' : 'analysis_dropped_packets',
    'Buffer Capacity' : 'buffer_size',
    'Capacity Available' : 'buffer_available',
    'Capacity Used' : 'buffer_used',
    'Comment' : 'comment',
    'Creator' : 'creator',
    'Creator-SID' : 'creator_sid',
    'Data Start Time' : 'data_start_time',
    'Data Stop Time' : 'data_stop_time',
    'Distributed Capture' : 'option_distributed',
    'Duplicate Packets Discarded' : 'duplicate_packets_discarded',
    'Duration' : 'duration',
    'Expert Enabled' : 'option_expert',
    'Filter Mode' : 'filter_mode',
    'Filters Enabled' : 'option_filters',
    'First Packet' : 'first_packet',
    'Flows Dropped' : 'flows_dropped',
    'Graphs-Count' : 'graphs_count',
    'Graphs-Enabled' : 'option_graphs',
    'GroupID' : 'group_id',
    'Hidden' : 'hidden',
    'ID' : 'id',
    'Indexing Enabled' : 'option_indexing',
    'Link Speed' : 'link_speed',
    'LoggedOnUser-Sid' : 'logged_on_user_sid',
    'Media Subtype' : 'media_sub_type',
    'Media Type' : 'media_type',
    'Mod By' : 'last_modified_by',
    'Mod Type' : 'last_modification',
    'Name' : 'name',
    'Packet Buffer Enabled' : 'option_packet_buffer',
    'Packet Count' : 'packet_count',
    'Packets Analyzed' : 'packets_analyzed',
    'Packets Dropped' : 'packets_dropped',
    'Packets Filtered' : 'packets_filtered',
    'Packets Received' : 'packets_received',
    'Plugins Enabled' : 'plugin_list',
    'Reset Count' : 'reset_count',
    'Spotlight Capture' : 'option_spotlight_capture',
    'Start Time' : 'start_time',
    'Status' : 'status',
    'Stop Time' : 'stop_time',
    'Timeline Stats Enabled' : 'option_timeline_stats',
    'Trigger Count' : 'trigger_count',
    'Trigger Duration' : 'trigger_duration',
    'Voice Enabled' : 'option_voice',
    'Web Enabled' : 'option_web'
    }


class Capture(object):
    """The Capture class has the attributes of a capture.
    The functions :func:`create_capture() 
    <omniscript.omniengine.OmniEngine.create_capture>`
    and :func:`find_capture() <omniscript.omniengine.OmniEngine.find_capture>`
    return a Capture object.
    The function :func:`get_capture_list()
    <omniscript.omniengine.OmniEngine.get_capture_list>`
    returns a list of Capture objects.
    """

    _engine = None
    """The engine this capture belongs to."""

    adapter = None
    """The :class:`Adapter <omniscript.adapter.Adapter>` object of the
    capture.
    """

    adapter_name = ''
    """The name of the capture's adapter."""

    adapter_type = 0
    """The type of adapter being used by the capture."""

    alarms_info = 0
    """The number of Information alarms that have been triggered."""

    alarms_major = 0
    """The number of Major alarms that have been triggered."""

    alarms_minor = 0
    """The number of Minor alarms that have been triggered."""

    alarms_severe = 0
    """The number of Severe alarms that have been triggered."""

    analysis_dropped_packets = 0
    """The number of packets that were not analysied. (Need to confirm.)"""

    buffer_available = 0
    """The amount of unused space (bytes) in the capture's buffer."""

    buffer_size = 0
    """The size of the capture's buffer in bytes."""

    buffer_used = 0
    """The number of bytes in use of the capture's buffer."""

    comment = ''
    """The capture's comment."""

    creator = ''
    """The creator of the capture."""

    creator_sid = ''
    """The capture creator's security identifier."""

    data_start_time = 0
    """The start time of the data."""

    data_stop_time = 0
    """The stop time of the data."""

    duplicate_packets_discarded = 0
    """The number of discarded packets because they are duplicates."""

    duration = 0
    """The length of time the capture has been capturing in nanoseconds."""

    filter_mode = 0
    """The filter mode."""

    first_packet = 0
    """The index of the first packet in the capture's packet buffer."""

    flows_dropped = 0
    """The number of flows dropped."""

    graphs_count = 0
    """The number of graphs being used by the capture."""

    group_id = None
    """The Global Unique Identifier of the capture group."""

    id = None
    """The Global Unique Identifier of the Capture."""

    last_modification = ''
    """The last modification made to the capture."""

    last_modified_by = ''
    """The name of the user that last modified the capture."""

    link_speed = 0
    """The link speed, in bits per second, of the capture's adapter."""

    logged_on_user_sid = ''
    """The currenlty logged on user."""

    media_info = None
    """The :class:`MediaInfo <omniscript.mediainfo.MediaInfo>` object of the
    capture.
    """

    media_type = 0
    """The media type of the capture's adapter."""

    media_sub_type = 0
    """The media sub type of the capture's adapter."""

    name = ''
    """The name of the capture."""

    packet_count = 0
    """The current number of packets in the capture's packet buffer."""

    packets_analyzed = 0
    """The number of packets analyzed."""

    packets_filtered = 0
    """The number of packets that have been accepted by the capture's
    filters.
    """

    packets_received = 0
    """The number of packets the capture has received."""

    packets_dropped = 0
    """The number of packets dropped."""

    plugin_list = []
    """The Analysis Modules (plugins) being used by the capture."""

    reset_count = 0
    """The number of times the capture has been reset."""

    start_time = None
    """The Start Time of the capture as
    :class:`PeekTime <omniscript.peektime.PeekTime>`.
    """

    status = 0
    """The status of the capture: idle, capturing... """

    stop_time = None
    """The Stop Time of the capture as
    :class:`PeekTime <omniscript.peektime.PeekTime>`.
    """

    trigger_count = 0
    """The number of Triggers that have been triggered."""

    trigger_duration = 0
    """The duration of the trigger events."""

    option_alarms = False
    """Are alarms enabled?"""

    option_distributed = False
    """Is the capture distributed?"""

    option_expert = False
    """Is Expert Processing enabled?"""

    option_filters = False
    """Are Filters enabled."""

    option_graphs = False
    """Are graphs enabled?"""

    option_hidden = False
    """Is the capture hidden?"""

    option_indexing = False
    """Is Indexing enabled?
    Indexing is enabled only if CTD and at least one Indexing option is enabled.
    """

    option_packet_buffer = True
    """Does the capture have a capture buffer."""

    option_spotlight_capture = False
    """Is this the Spotlight Capture?"""

    option_timeline_stats = False
    """Are Timeline Statistics enabled?"""

    option_voice = False
    """Are Voice Statistics enabled?"""

    option_web = False
    """Are Web Statistics enabled?"""

    def __init__(self, engine, node=None):
        self._engine = engine
        self.logger = engine.logger
        self._context = None

        self.adapter = Capture.adapter
        self.adapter_name = Capture.adapter_name
        self.adapter_type = Capture.adapter_type
        self.alarms_info = Capture.alarms_info
        self.alarms_major = Capture.alarms_major
        self.alarms_minor = Capture.alarms_minor
        self.alarms_severe = Capture.alarms_severe
        self.analysis_dropped_packets = Capture.analysis_dropped_packets
        self.buffer_available = Capture.buffer_available
        self.buffer_size = Capture.buffer_size
        self.buffer_used = Capture.buffer_used
        self.comment = Capture.comment
        self.creator = Capture.creator
        self.creator_sid = Capture.creator_sid
        self.data_start_time = Capture.data_start_time
        self.data_stop_time = Capture.data_stop_time
        self.duplicate_packets_discarded = Capture.duplicate_packets_discarded
        self.duration = Capture.duration
        self.filter_mode = Capture.filter_mode
        self.first_packet = Capture.first_packet
        self.flows_dropped = Capture.flows_dropped
        self.graphs_count = Capture.graphs_count
        self.group_id = Capture.group_id
        self.id = Capture.id
        self.last_modified_by = Capture.last_modified_by
        self.last_modification = Capture.last_modification
        self.link_speed = Capture.link_speed
        self.logged_on_user_sid = Capture.logged_on_user_sid
        self.media_info = Capture.media_info
        self.media_type = Capture.media_type
        self.media_sub_type = Capture.media_sub_type
        self.name = Capture.name
        self.packet_count = Capture.packet_count
        self.packets_analyzed = Capture.packets_analyzed
        self.packets_dropped = Capture.packets_dropped
        self.packets_filtered = Capture.packets_filtered
        self.packets_received = Capture.packets_received
        self.plugin_list = []
        self.reset_count = Capture.reset_count
        self.start_time = Capture.start_time
        self.status = Capture.status
        self.stop_time = Capture.stop_time
        self.trigger_count = Capture.trigger_count
        self.trigger_duration = Capture.trigger_duration
        self.option_alarms = Capture.option_alarms
        self.option_distributed = Capture.option_distributed
        self.option_expert = Capture.option_expert
        self.option_filters = Capture.option_filters
        self.option_graphs = Capture.option_graphs
        self.option_hidden = Capture.option_hidden
        self.option_indexing = Capture.option_indexing
        self.option_packet_buffer = Capture.option_packet_buffer
        self.option_spotlight_capture = Capture.option_spotlight_capture
        self.option_timeline_stats = Capture.option_timeline_stats
        self.option_voice = Capture.option_voice
        self.option_web = Capture.option_web
        #Load the XML if provided
        if node is not None:
            self._load(node)

    def __repr__(self):
        return 'Capture: %s' % self.name

    def _get_properties(self):
        request = '<request><prop name=\"id\" type=\"8\">' + str(self.id) + '</prop></request>'
        response = self._engine._issue_xml_command(config.OMNI_GET_CAPTURE_PROPS, request)
        return omniscript._parse_command_response(response, 'captureproperties')

    def _get_statistics(self):
        """Get the Statistics, a binary blob of data, and cache them."""
        if self.id is None:
            self.logger.error("Failed to get the id for Capture: %s",
                              self.name)
            return None
        #self._context = self._engine._api_issue_command(
        #                   config.OMNI_GET_STATS_CONTEXT, str(self.id), 0)
        request = struct.pack('16sQ', self.id.bytes_le(), 0)
        data = self._engine._issue_command(config.OMNI_GET_STATS,
                                           request, 24)
        #with open(r"c:\temp\stat_data.bin", 'wb') as fle:
        #    fle.write(data)
        self._context = StatsContext(data.raw)

    def _load(self, node):
        for prop in node.findall('prop'):
            k = prop.get('name')
            v = prop.text
            a = _capt_prop_dict.get(k)
            if a is not None:
                if hasattr(self, a):
                    if isinstance(getattr(self, a), basestring):
                        setattr(self, a, v if v else '')
                    elif isinstance(getattr(self, a), bool):
                        setattr(self, a, (int(v) != 0) if v else False)
                    elif isinstance(getattr(self, a), (int, long)):
                        setattr(self, a, int(v) if v else 0)
                    elif getattr(self, a) is None:
                        if (a == 'group_id') or (a == 'id'):
                            setattr(self, a, OmniId(v))
                        elif (a == 'start_time') or (a == 'stop_time'):
                            setattr(self, a, PeekTime(v))
                        elif a == 'plugin_list':
                            setattr(self, a,
                                    [OmniId(id) for id in v.split(',')])
        for obj in node.findall('obj'):
            kind = obj.get('name')
            if kind == 'AdapterInfo':
                self.adapter = Adapter(self._engine, obj)
            elif kind == 'MediaInfo':
                self.media_info = MediaInfo(obj)

    @property
    def engine(self):
        """The engine that this capture belongs to."""
        return self._engine

    def format_status(self):
        """Convert capture status from integer to descriptive string."""
        status = status_map[self.status]
        if status is None:
            return "Unknown"
        return status

    def get_application_stats(self, refresh=False):
        """Returns a list of
        :class:`ApplicationStatistic <omniscript.applicationstatistic.ApplicationStatistic>`
        objects.

        Args:
            refresh (boolean): refresh the statistics cache. Default is False.
        """
        if self._context is None or refresh:
            self._get_statistics()
        if self._context is None:
            self.logger.error("Failed to get statistics context " \
                              "for capture: %s.", self.name)
            return None
        return self._context.get_application_statistics()

    def get_call_stats(self, refresh=False):
        """Returns a 
        :class:`CallStatistic <omniscript.callstatistic.CallStatistic>`
        object.
        Note that only one CallStatistic object is returned.

        Args:
            refresh (boolean): refresh the statistics cache. Default is False.
        """
        if self._context is None or refresh:
            self._get_statistics()
        if self._context is None:
            self.logger.error("Failed to get statistics context " \
                              "for capture: %s.", self.name)
            return None
        return self._context.get_call_statistics()
    
    def get_capture_template(self):
        """Get the
        :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`
        of this capture.

        Returns:
            A :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`
            object.
        """
        return self._engine.get_capture_template(self)

    def get_country_stats(self, refresh=False):
        """Returns a list of
        :class:`CountryStatistic <omniscript.countrystatistic.CountryStatistic>`
        objects.

        Args:
            refresh (boolean): refresh the statistics cache. Default is False.
        """
        if self._context is None or refresh:
            self._get_statistics()
        if self._context is None:
            self.logger.error("Failed to get statistics context " \
                              "for capture: %s.", self.name)
            return None
        return self._context.get_country_statistics()

    def get_node_stats(self, refresh=False):
        """Returns a list of
        :class:`NodeStatistic <omniscript.nodestatistic.NodeStatistic>`
        objects.

        Args:
            refresh (boolean): refresh the statistics cache. Default is False.
        """
        if self._context is None or refresh:
            self._get_statistics()
        if self._context is None:
            self.logger.error("Failed to get statistics context " \
                              "for capture: %s.", self.name)
            return None
        return self._context.get_node_statistics()

    def get_packets(self, indices):
        """Returns a list of
        :class:`Packet <omniscript.packet.Packet>`
        objects.

        Notes:
            The first packet number is 0. The attribute first_packet is the
            packet number of the first packet in the packet buffer.

        Args:
            numbers (list): a list of integers or tuples.
            numbers (integer): the number of the to retrieve.
            numbers (tupel): the number of the first packet and the number of
            the last packet to retrieve.

        Example:
            get_packets([(0,3),(10,11),20]) gets packets: 0,1,2,3,10,11,20.
            get_packets(0) gets the first packet in the buffer.
            get_packets((1,3) gets packets: 1,2,3
        """
        pairs = []
        if isinstance(indices, list):
            for i in indices:
                if isinstance(i, (int, long)):
                    pairs.append((i,i))
                elif isinstance(i, tuple):
                    pairs.append(i)
        elif isinstance(indices, (int, long)):
            pairs.append((indices,indices))
        elif isinstance(indices, tuple):
            pairs.append(indices)
        count = len(pairs) * 2
        if count == 0:
            return None
        self.refresh()
        indexes = (item for sublist in pairs for item in sublist)
        request = struct.pack('16s%sI' % (1 + count), self.id.bytes_le(), count, *indexes)
        response = self._engine._issue_command(config.OMNI_GET_PACKETS,
                                               request, len(request))
        return _create_packet_list(response, self.first_packet)

    def get_protocol_stats(self, refresh=False):
        """Returns a list of
        :class:`ProtocolStatistic 
        <omniscript.protocolstatistic.ProtocolStatistic>`
        objects.

        Args:
            refresh (boolean): refresh the statistics cache. Default is False.
        """
        if self._context is None or refresh:
            self._get_statistics()
        if self._context is None:
            self.logger.error("Failed to get statistics context for " \
                              "capture: %s.", self.name)
            return None
        return self._context.get_protocol_statistics()

    def get_stats_context(self, refresh=False):
        """Returns a 
        :class:`StatsContext 
        <omniscript.statscontext.StatsContext>`
        object.

        Args:
            refresh (boolean): refresh the statistics context. Default is False.
        """
        if self._context is None or refresh:
            self._get_statistics()
        if self._context is None:
            self.logger.error("Failed to get statistics context for " \
                              "capture: %s.", self.name)
            return None
        return self._context

    def get_summary_stats(self, refresh=False):
        """Returns a list of
        :class:`SummaryStatistic 
        <omniscript.summarystatistic.SummaryStatistic>`
        objects.

        Args:
            refresh (boolean): refresh the statistics cache. Default is False.
        """
        if self._context is None or refresh:
            self._get_statistics()
        if self._context is None:
            self.logger.error("Failed to get statistics context for " \
                              "capture: %s.", self.name)
            return None
        return self._context.get_summary_statistics()

    def is_capturing(self, refresh=True):
        """Returns True if the capture is capturing.

        Args:
            refresh (boolean): refresh the capture's status. Default is True.
        """
        if refresh:
            self.refresh()
        return ((self.status & config.CAPTURE_STATUS_CAPTURING) != 0)

    def is_idle(self, refresh=True):
        """Returns True if the capture is not capturing.

        Args:
            refresh (boolean): refresh the capture's status. Default is True.
        """
        return not self.is_capturing(refresh)

    def is_spotlight_capture(self):
        """Returns True if the capture is the Spotlight Capture."""
        return self.option_spotlight_capture

    def is_start_trigger_active(self, refresh=True):
        """Returns True if the capture has an active Start Trigger.

        Args:
            refresh (boolean): refresh the capture's status. Default is True.
        """
        if refresh:
            self.refresh()
        return ((self.status & config.CAPTURE_STATUS_START_ACTIVE) != 0)

    def is_stop_trigger_active(self, refresh=True):
        """Returns True if the capture has an active Stop Trigger.

        Args:
            refresh (boolean): refresh the capture's status. Default is True.
        """
        if refresh:
            self.refresh()
        return ((self.status & config.CAPTURE_STATUS_STOP_ACTIVE) != 0)

    def modify(self, template):
        """Modify the settings with those in the template.
        """
        return self._engine.modify_capture(template, self)

    def query_expert(self, query_list):
        """Query the Expert with one or more
        :class:`ExpertQuery <omniscript.expertquery.ExpertQuery>` objects.
        Submit more than one query to get a snapshot at the same point in time.
        See the :ref:`expert-tables-section` section for the list of tables and columns.

        Returns a list of
        :class:`ExpertResult <omniscript.expertresult.ExpertResult>` objects.
        Match the result to the query by the table name.
        """
        querys = query_list if isinstance(query_list, list) else [query_list]
        request = ET.Element('request')
        msg = ET.SubElement(request, 'msg', {'capture-id':str(self.id)})
        for q in querys:
            q._store(msg)
        xml = ET.tostring(msg).replace('\n', '')
        response = self._engine._issue_xml_command(config.OMNI_EXPERT_EXECUTE_QUERY, xml)
        querys = omniscript._parse_command_response(response, 'msg')
        return _create_expert_result_list(querys)

    def refresh(self):
        """Refresh the properties of this object.
        The statitics cache is cleared.
        """
        props = self._get_properties()
        if props is not None:
            self.start_time = Capture.start_time
            self.stop_time = Capture.stop_time
            self.plugin_list = []
            self._load(props)

    def refresh_stats(self):
        """Refresh the statistics cache."""
        self._get_statistics()

    def reset(self):
        """Reset the capture's statistics and delete all its packets.
        This capture object will be refreshed.
        """
        if self.id is None:
            self.logger.error("Failed to get the id for Capture: %s", self.name)
            return False
        # <captures>
        #   <capture id="{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}" />
        # </captures>
        request = r'<request><captures><capture id="' + str(self.id) + \
                  r'"/></captures></request>'
        response = self._engine._issue_xml_string_result(
                    config.OMNI_CLEAR_CAPTURES, request)
        omniscript._parse_command_response(response)
        self.refresh()

    def save_all_packets(self, filename):
        """Save the packets of the capture to a file."""
        return self._engine.save_all_packets(self, filename)

    def select_related(self, packets, criteria=omniscript.SELECT_BY_SOURCE_DESTINATION, unique=True):
        """Returns a list of packet numbers for the packets and criteria specified.
        If unique is True then only unique packet numbers are returned. Otherwise the
        list may contain duplicates.
        Example: packet 1 matches packets 1,2,5 and packet 3 matches packets 2,3,4,6.
        Unique returns [1,2,3,4,5,6], non-unique returns [1,2,5,2,3,4,6].
        """
        _packets = packets if isinstance(packets, list) else [packets]
        magic = 0x4E8B3899
        version = 1
        logical_addr = 1
        packet_count = len(packets)
        buf = struct.pack('=II16sIII%dQ' % packet_count, magic, version, 
                          self.id.bytes_le(), criteria, logical_addr,
                          packet_count, *packets)
        response = self._engine._issue_command(config.OMNI_SELECT_RELATED,
                                               buf, len(buf))
        try:
            omniscript._parse_command_response(response)
        except omniscript.OmniError as oe:
            raise oe
        except:
            # Only errors are parseable.
            # But ET.parse(data) will throw an exception
            #   catch the exception and process the success.
            pass
        stream = ReadStream(response)
        pkt_count = stream.read_uint()
        if unique:
            return list(set(stream.read_ulong() for i in range(pkt_count)))
        else:
            return [stream.read_ulong() for i in range(pkt_count)]

    def start(self):
        """Start the capturing packets. Returns True if the capture is
        capturing packets.
        """
        if self.id is None:
            self.logger.error("Failed to get the id for Capture: %s", self.name)
            return False
        # <captures>
        #   <capture id="{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}" />
        # </captures>
        request = r'<request><captures><capture id="' + str(self.id) + \
                  r'"/></captures></request>'
        response = self._engine._issue_xml_string_result(
                    config.OMNI_START_CAPTURES, request)
        omniscript._parse_command_response(response)

    def stop(self):
        """Stop the capturing packets. Returns True if the capture is
        stopped.
        """
        if self.id is None:
            self.logger.error("Failed to get the id for Capture: %s", self.name)
            return False
        # <captures>
        #   <capture id="{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}" />
        # </captures>
        request = r'<request><captures><capture id="' + str(self.id) + \
                  r'"/></captures></request>'
        response = self._engine._issue_xml_string_result(
                        config.OMNI_STOP_CAPTURES, request)
        omniscript._parse_command_response(response)


def _create_capture_list(engine, xml_list):
    lst = []
    captureinfo = omniscript._parse_command_response(xml_list, 'captures')
    if captureinfo is not None:
        for info in captureinfo.findall('capture'):
            lst.append(Capture(engine, info))
    lst.sort(key=lambda x: x.name)
    return lst


def find_capture(captures, value, attrib = find_attribs[0]):
    """Finds a capture in the list"""
    if (not captures) or (attrib not in find_attribs):
        return None

    if len(captures) == 0:
        return None

    if isinstance(value, Capture):
        _value = value.id
        attrib = 'id'
    else:
        _value = value

    return next((i for i in captures if getattr(i, attrib) == _value), None)
