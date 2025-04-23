"""ForensicFile class.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

import os
import sys
import logging

from omniid import OmniId
from omnidatatable import OmniDataTable
from omnierror import OmniError
from peektime import PeekTime


_forensic_file_label_map = {
    'AdapterAddr':'adapter_address',
    'AdapterName':'adapter_name',
    'CaptureName':'capture_name',
    'CaptureID':'capture_id',
    'DroppedCount':'dropped_packet_count',
    'DroppedFlowCount':'dropped_flow_count',
    'DroppedPacketCount':'dropped_packet_count',
    'FileIndex':'file_index',
    'FileName':'name',
    'FileSize':'size',
    'LinkSpeed':'link_speed',
    'MediaSubType':'media_sub_type',
    'MediaType':'media_type',
    'PacketCount':'packet_count',
    'PartialPath':'path',
    'Status':'status',
    'Stream':'stream',
    'SessionEndTime':'end_time',
    'SessionID':'session_id',
    'SessionStartTime':'start_time',
    'TimeZoneBias':'time_zone_bias'
    }

class ForensicFile(object):
    """Information about a Forensic File."""

    adapter_address = ''
    """The Ethernet address of the adapter."""

    adapter_name = ''
    """The name of the adapter."""

    capture_id = None
    """The Id (GUID/UUID) of the Capture that created the file as a
    :class:`OmniId <omniscript.omniid.OmniId>` object.
    """

    capture_name = ''
    """The name of the Capture that created the file."""

    dropped_flow_count = 0
    """The number of IPFix dropped flows/inserts."""

    dropped_packet_count = 0
    """The number of dropped packets."""

    end_time = None
    """The timestamp of the last packet in the file
    :class:`PeekTime <omniscript.peektime.PeekTime>` object.
    """

    file_index = 0
    """The files index."""

    link_speed = 0
    """The link speed of the adapter."""

    media_type = 0
    """The Media Type of the adapter."""

    media_sub_type = 0
    """The Media Sub Type of the adapter."""

    name = ''
    """The name of the file."""

    packet_count = 0
    """The number of packets in the file."""

    path = ''
    """The fully qualified path and file name of the file on the system."""

    session_id = None
    """The Session Id of the file as a
    :class:`OmniId <omniscript.omniid.OmniId>` object.
    """

    size = 0
    """The number of bytes in the file."""

    start_time = None
    """The timestamp of the first packet in the file as a
    :class:`PeekTime <omniscript.peektime.PeekTime>` object.
    """

    status = 0
    """The status of the file."""

    stream = 0
    """The stream index."""

    time_zone_bias = 0
    """The timezone bias of the start_time and end_time and/or the system."""

    def __init__(self):
        self.adapter_address = ForensicFile.adapter_address
        self.adapter_name = ForensicFile.adapter_name
        self.capture_id = ForensicFile.capture_id
        self.capture_name = ForensicFile.capture_name
        self.dropped_flow_count = ForensicFile.dropped_flow_count
        self.dropped_packet_count = ForensicFile.dropped_packet_count
        self.end_time = ForensicFile.end_time
        self.file_index = ForensicFile.file_index
        self.link_speed = ForensicFile.link_speed
        self.media_type = ForensicFile.media_type
        self.media_sub_type = ForensicFile.media_sub_type
        self.name = ForensicFile.name
        self.packet_count = ForensicFile.packet_count
        self.path = ForensicFile.path
        self.session_id = ForensicFile.session_id
        self.size = ForensicFile.size
        self.start_time = ForensicFile.start_time
        self.status = ForensicFile.status
        self.stream = ForensicFile.stream
        self.time_zone_bias = ForensicFile.time_zone_bias

    def __repr__(self):
        if self.name is None:
            return 'ForensicFile'
        else:
            return 'ForensicFile: %s' % self.name

    def load(self, labels, values):
        """Load the ForensicFile information from the row of an
        :class:`OmniDataTable <omniscript.omnidatatabel.OmniDataTable>`.
        """
        if isinstance(values, list):
            for i, v in enumerate(values):
                if labels[i] in _forensic_file_label_map.keys():
                    a = _forensic_file_label_map[labels[i]]
                    if a is not None:
                        if hasattr(self, a):
                            if isinstance(getattr(self, a), basestring):
                                setattr(self, a, v if v else '')
                            elif isinstance(getattr(self, a), (int, long)):
                                setattr(self, a, int(v) if v else 0)
                            elif getattr(self, a) is None:
                                if a == 'capture_id':
                                    setattr(self, a, OmniId(v))
                                elif (a == 'start_time') or (a == 'end_time'):
                                    setattr(self, a, PeekTime(v))
                                elif (a == 'unknown_1') or (a == 'unknown_2'):
                                    setattr(self, a, v)


def _create_forensic_file_list(datatable):
    """Create a List of ForensicFile objects from an
    :class:`OmniDataTable <omniscript.omnidatatabel.OmniDataTable>`.
    """
    if isinstance(datatable, OmniDataTable):
        lst = []
        for r in datatable.rows:
            #if len(r) != len(_forensic_file_label_map):
            #    raise OmniError('DataTable has incorrect number of '
            #                    'columns: %d, should be: %d' %
            #                    (len(r), len(_forensic_file_label_map)))
            ff = ForensicFile()
            ff.load(datatable.labels, r)
            lst.append(ff)
        lst.sort(key=lambda x: x.name)
        return lst
    return None
