"""tcpdump class.
"""
#Copyright (c) Savvius, Inc. 2015. All rights reserved.

# " ".join(['%02X' % ord(b) for b in data])

import os
import sys
import logging
import xml.etree.ElementTree as ET
import xml.dom.minidom as MD

import omniscript
import config

from omnierror import OmniError
from peektime import PeekTime
from readstream import ReadStream

TCPDUMP_TRUE = 'true'
TCPDUMP_FALSE = 'false'

# Responses
# Type 1:
# Type 2: Success
# Type 3:
# Type 4: Error <reason></reason>
# Type 5: Network Interface List
# Type 6: Processing...


def _to_tcpdump_boolean(value):
    if isinstance(value, basestring):
        if int(value):
            return TCPDUMP_TRUE
        return TCPDUMP_FALSE
    if value:
        return TCPDUMP_TRUE
    return TCPDUMP_FALSE


def pretty_print(title, msg):
    print(title)
    doc = MD.parseString(msg)
    xml = doc.toprettyxml(indent='  ')
    print(xml)


class TCPDumpHost(object):
    host = "localhost"
    """An identifier for the host: IP Address or name."""

    port = 22
    """The IP Port of the connection."""

    user = ""
    """The user name on the host."""

    key_type = 1
    """The key type: 1-password, 3-private key. The set_credentials()
    method will set this attribute.
    """

    key = ""
    """The key data."""

    def __init__(self, host, port=0):
        self.host = host
        self.port = port if port > 0 else TCPDumpHost.port
        self.user = TCPDumpHost.user
        self.key_type = TCPDumpHost.key_type
        self.key = TCPDumpHost.key

    def set_credentials(self, user, password=None, private_key=None):
        self.user = user
        if isinstance(password, basestring):
            self.key_type = 1
            self.key = password
        elif isinstance(private_key, basestring):
            self.key_type = 3
            self.key = private_key
        else:
            raise TypeError('Must specify a password or private_key')

    def store_elements(self, node):
        ET.SubElement(node, 'hostname').text = self.host
        ET.SubElement(node, 'port').text = str(self.port)
        ET.SubElement(node, 'username').text = self.user
        ET.SubElement(node, 'auth-data',
                      {'type' : str(self.key_type)}).text = self.key

    def store(self, node):
        elem = ET.SubElement(node, 'host')
        self.store_elements(elem)


class TCPDumpNetwork(object):
    """Information about a Network.
    """

    media_type = "ethernet"
    """The media type of the network."""

    monitor_media_type = "unknown"
    """Thew wireless media type."""

    name = None
    """The name of the network."""

    speed = 1000000000
    """The link speed of the network in bits per second."""

    def __init__(self, name, speed=0, media_type=None, monitor_media_type=None):
        self.media_type = media_type if media_type else TCPDumpNetwork.media_type
        self.monitor_media_type = monitor_media_type if monitor_media_type else TCPDumpNetwork.monitor_media_type
        self.name = name
        self.speed = speed if speed > 0 else TCPDumpNetwork.speed

    def store(self, node):
        elem = ET.SubElement(node, 'network-interface',
                             {'media-type':self.media_type,
                              'monitor-mode-media-type':self.monitor_media_type})
        ET.SubElement(elem, 'name').text = self.name
        ET.SubElement(elem, 'speed').text = str(self.speed)


class TCPDumpNetworkInterface(object):
    """Information about a Network Interface.
    """

    name = ""
    """The name of the interface. For example: eth0."""

    speed = 1000000000
    """The link speed of the interface in bits per second."""

    option_sudo = False
    """Is sudo required?"""

    def __init__(self, name, link_speed, sudo):
        self.name = name
        self.speed = link_speed
        self.option_sudo = sudo


class TCPDumpTemplate(object):
    """The information needed to create a TCPDump Remote Adapter
    on a host system.
    """

    interface_name = ""
    """The name of the interface."""

    media_type = "ethernet"
    """The media type of the interface."""

    monitor_media_type = "unknown"
    """The wireless media thpe."""

    session_filter = "adapter"
    """The session filter. Appears to always be 'adapter'."""

    slice_length = 65535
    """The slice length. Default is 65535 (no slicing)."""

    speed = 1000000000
    """The link speed of the interface in bits per second."""

    user_filter = None
    """User filter."""

    option_monitor_mode = False
    """Enable wireless monitor mode?"""

    option_promiscuous_mode = True
    """Enable promiscuous mode?"""

    option_sudo = False
    """Is sudo required?"""

    def __init__(self, interface, slice_length=65535, speed=1000000000, sudo=False, user_filter=None):
        if isinstance(interface, TCPDumpNetworkInterface):
            self.interface_name = interface.name
            self.speed = interface.speed
            self.option_sudo = interface.option_sudo
        elif isinstance(interface, basestring):
            self.interface_name = interface
            self.speed = speed
            self.option_sudo = sudo
        else:
            raise TypeError('interface must be a TCPDUMPNetowrkInterface or a string.')
        self.media_type = TCPDumpTemplate.media_type
        self.monitor_media_type = TCPDumpTemplate.monitor_media_type
        self.session_filter = TCPDumpTemplate.session_filter
        self.slice_length = slice_length
        self.user_filter = user_filter
        self.option_monitor_mode = TCPDumpTemplate.option_monitor_mode
        self.option_promiscuous_mode = TCPDumpTemplate.option_promiscuous_mode

    def store(self, node):
        elem = ET.SubElement(node, 'tcpdump-options')
        ET.SubElement(elem, 'session-filter').text = self.session_filter
        ET.SubElement(elem, 'promiscuous-mode').text = _to_tcpdump_boolean(self.option_promiscuous_mode)
        ET.SubElement(elem, 'monitor-mode').text = _to_tcpdump_boolean(self.option_monitor_mode)
        ET.SubElement(elem, 'requires-sudo').text = _to_tcpdump_boolean(self.option_sudo)
        ET.SubElement(elem, 'slice-length').text = str(self.slice_length)
        ET.SubElement(elem, 'user-filter').text = self.user_filter if self.user_filter else ""
        network = TCPDumpNetwork(self.interface_name, self.speed, self.media_type, self.monitor_media_type)
        network.store(elem)


class TCPDump(object):
    """A TCPDump object used to recreate TCPDump Remote Adapters.
    """

    engine = None
    """The :class:`OmniEngine <omniscript.omniengine.OmniEngine>`
    to create the adpater on.
    """
    host = None
    """A :class:`TCPDumpHost <omniscript.tcpdump.TCPDumpHost>` object.
    """

    _plugin_id = None
    _query_id = None

    def __init__(self, engine, host="localhost", port=22):
        self.engine = engine
        self.host = TCPDumpHost(host, port)
        self._plugin_id = TCPDump._plugin_id
        self._query_id = TCPDump._query_id
        if engine:
            aml = engine.get_analysis_module_list()
            tcpdump = omniscript.find_analysis_module(aml, 'tcpdump')
            if not tcpdump:
                raise OmniError('The engine does not have the tcpdump plugin installed.')
            self._plugin_id = tcpdump.id

    def __repr__(self):
        return 'TCPDump: %s:%s' % (self.host, str(self.host.port))

    def _issue_cmd(self, request_type):
        if not self._plugin_id:
            raise OmniError('The engine does not have the tcpdump plugin installed.')
        if not self._query_id:
            raise OmniError('Must login first.')

        message_id = PeekTime().value / 10000
        _message = ET.Element('message-envelope',
                                  {'xmlns' : 'http://www.w3.org/1999/xhtml',
                                   'id' : str(message_id)})
        _request = ET.SubElement(_message, 'request', {'type' : str(request_type)})
        ET.SubElement(_request, 'query-id').text = self._query_id
        _xml = ET.tostring(_message).replace('\n', '')
        #pretty_print("Request:", _xml)
        resp = self.engine.send_plugin_message(self._plugin_id, None, _xml)
        stream = omniscript.ReadStream(resp)
        stream.read_ulong()         # text_count
        stream.read_ulong()         # binary_count
        result = stream.read_uint()
        if result != 0:
            raise OmniError('TCPDump Plugin Message failed 0x%X.' % result, result)
        stream.read_guid()      # plugin_id
        outer_count_high = stream.read_uint()
        if outer_count_high == 0:
            # No XML results.
            cmd_result = stream.read_uint()
            if cmd_result != 0:
                raise OmniError('TCPDump Query failed 0x%X.' % cmd_result, result)
        stream.read_uint()      # outer_count_lo
        inner_count = stream.read_ulong()
        stream.read_ulong()     # reserved
        xml = stream.read_string(inner_count)
        #pretty_print("Response:", xml)
        msg = ET.fromstring(xml)
        response = msg.find('response')
        if response is None:
            raise OmniError('Invalid command response: no response tag found.')
        return msg

    def create_adapter(self, template, user_host=None, password=None, private_key=None):
        if not self._plugin_id:
            raise OmniError('The engine does not have the tcpdump plugin installed.')
        message_id = PeekTime().value / 10000
        _message = ET.Element('message-envelope',
                                  {'xmlns' : 'http://www.w3.org/1999/xhtml',
                                   'id' : str(message_id)})
        _request = ET.SubElement(_message, 'request', {'type' : '3'})
        if not user_host and not password and not private_key:
            self.host.store(_request)
        elif isinstance(user_host, TCPDumpHost):
            user_host.store(_request)
        elif isinstance(user_host, basestring):
            _host = TCPDumpHost(self.host.host, self.host.port)
            _host.set_credentials(user_host, password, private_key)
            _host.store(_request)
        else:
            raise TypeError('user_host must be a TCPDumpHost or a string.')
        template.store(_request)
        _xml = ET.tostring(_message) #.replace('\n', '') need \n for private key. 
        #pretty_print("Request:", _xml)
        resp = self.engine.send_plugin_message(self._plugin_id, None, _xml)
        stream = omniscript.ReadStream(resp)
        stream.read_ulong()     # text_count
        stream.read_ulong()     # binary_count
        result = stream.read_uint()
        if result != 0:
            raise OmniError('TCPDump Plugin Message failed 0x%X.' % result, result)
        stream.read_guid()      # plugin_id
        outer_count_high = stream.read_uint()
        if outer_count_high == 0:
            # No XML results.
            cmd_result = stream.read_uint()
            if cmd_result != 0:
                raise OmniError('TCPDump Create Adapter failed 0x%X.' % cmd_result, result)
        stream.read_uint()      # outer_count_lo
        inner_count = stream.read_ulong()
        stream.read_ulong()     #reserved
        xml = stream.read_string(inner_count)
        #pretty_print("Response:", xml)

        msg = ET.fromstring(xml)
        response = msg.find('response')
        if response is None:
            raise OmniError('Invalid command response: no response tag found.')
        resp_type = int(response.attrib.get('type', '0'))
        if resp_type == 2:
            # Empty response is expected.
            return "tcpdump %s:%s" % (self.host.host, template.interface_name)
        return None

    def get_adapter_list(self):
        interface_list = []
        msg = None
        response = None
        resp_type = 6
        while resp_type == 6:
            msg = self._issue_cmd(2)
            if msg is not None:
                response = msg.find('response') # Found in _issue_cmd
                resp_type = int(response.attrib.get('type', '0'))
                if resp_type == 6:
                    response.find('query-id')           # query_id
                    step_elem = response.find('step')
                    if step_elem is not None:
                        int(step_elem.text)             # step
                    max_elem = response.find('max-step')
                    if max_elem is not None:
                        int(max_elem.text)              # max
                    response.find('progress')           # progress
            else:
                break
        if resp_type == 5:
            if msg is not None and response is not None:
                response.find('query-id')       # query_id
                sudo = False
                sudo_elem = response.find('requires-sudo')
                if sudo_elem is not None:
                    sudo = sudo_elem.text == 'true'
                interfaces = response.find('network-interfaces')
                for interface in interfaces:
                    name = interface.find('name').text
                    speed = interface.find('speed').text
                    interface_list.append(TCPDumpNetworkInterface(name, speed, sudo))
        return interface_list

    def login(self, user, password=None, private_key=None):
        if not self._plugin_id:
            raise OmniError('The engine does not have the tcpdump plugin installed.')
        self.host.set_credentials(user, password, private_key)
        message_id = PeekTime().value / 10000
        _message = ET.Element('message-envelope',
                                  {'xmlns' : 'http://www.w3.org/1999/xhtml',
                                   'id' : str(message_id)})
        _request = ET.SubElement(_message, 'request', {'type' : '1'})
        self.host.store_elements(_request)
        _xml = ET.tostring(_message)    #.replace('\n', '') need \n for private key.
        #pretty_print("Request:", _xml)
        resp = self.engine.send_plugin_message(self._plugin_id, None, _xml)
        stream = omniscript.ReadStream(resp)
        stream.read_ulong()     # text_count
        stream.read_ulong()     # binary_count
        result = stream.read_uint()
        if result != 0:
            raise OmniError('TCPDump Plugin Message failed 0x%X.' % result, result)
        stream.read_guid()      # plugin_id
        outer_count_high = stream.read_uint()
        if outer_count_high == 0:
            # No XML results.
            cmd_result = stream.read_uint()
            if cmd_result != 0:
                raise OmniError('TCPDump Login failed 0x%X.' % cmd_result, result)
        stream.read_uint()      # outer_count_lo
        inner_count = stream.read_ulong()
        stream.read_ulong()     # reserved
        xml = stream.read_string(inner_count)
        #pretty_print("Response:", xml)

        msg = ET.fromstring(xml)
        response = msg.find('response')
        if response is None:
            raise OmniError('Invalid command response: no response tag found.')
        resp_type = int(response.attrib.get('type', '0'))
        if resp_type == 3:
            query_id = response.find('query-id')
            if query_id is not None:
                self._query_id = query_id.text
                #print("Query Id: %s\n" % self._query_id)
            else:
                raise OmniError('Did not find Query Id.')

    def set_credentials(self, user, password=None, private_key=None):
        self.host.set_credentials(user, password, private_key)

