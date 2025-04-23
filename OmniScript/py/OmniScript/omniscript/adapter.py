"""Adapter class.
"""
#Copyright (c) Savvius, Inc. 2013-2018. All rights reserved.

import os
import sys
import logging
import xml.etree.ElementTree as ET

import omniscript
import config

from omniaddress import format_ethernet

adapter_types = ['Network Interface Card', 'File Adapter', 'Plugin Adapter']

find_attribs = ['name', 'id', 'device_name']

_adapt_prop_dict = {
    'AdapterFeatures':'features',
    'AdapterType':'type',
    'Address':'address',
    'Description':'description',
    'DeviceName':'device_name',
    'Identifier':'id',
    'InterfaceFeatures':'interface_features',
    'LinkSpeed':'link_speed',
    'MediaSubType':'media_sub_type',
    'MediaType':'media_type',
    'WildPacketsAPI':'wildpackets_api'
}


class Adapter(object):
    """The Adapter class has the attributes of an adapter.
    The 
    :func:`get_adapter_list() 
    <omniscript.omniengine.OmniEngine.get_adapter_list>`
    function returns a list of Adapter objects.
    """

    id = ''
    """The adapter's identifier, on Windows is a four digit code that is
    engine specific.
    """

    features = 0
    """Features of the adapter. Bit fields."""

    adapter_type = 0
    """The type of adapter. Any of the ADDRESS TYPE constants."""

    address = '00:00:00:00:00:00'
    """The Ethernet Address of the adapter."""

    description = ''
    """The decscription/name of the adapter."""

    device_name = ''
    """The device name of the adapter: Broadcom, Intel..."""

    interface_features = 0
    """Interface Features of the adapter. Bit fields."""

    link_speed = 0
    """The link speed of the adapter in bits per second."""

    media_type = 0
    """The Media Type of the adapter."""

    media_sub_type = 0
    """The Media Sub Type of the adapter."""

    wildpackets_api = False
    """Does the adapter support the WildPackets' API."""

    def __init__(self, engine, element):
        self._engine = engine
        self.logger = engine.logger
        
        ndis = element.find('ndisadapterinfo')
        if ndis is not None:
            self.adapter_type = config.ADAPTER_TYPE_NIC
            self.address = format_ethernet(ndis.findtext('address',
                                                         '000000000000'))
            self.features = int(ndis.findtext('adapterfeatures', '0'))
            self.description = ndis.findtext('description', '')
            self.device_name = ndis.findtext('title', '')
            self.id = ndis.findtext('enumerator', '')
            self.interface_features = int(ndis.findtext('interfacefeatures',
                                                        '0'))
            self.link_speed = int(ndis.findtext('linkspeed', '0'))
            self.media_type = int(ndis.findtext('ndismediatype', '0'))
            self.media_sub_type = int(ndis.findtext('ndisphysmedium', '0'))
            api = int(ndis.findtext('validadvanced', '0'))
            self.wildpackets_api = (api != 0)
            return
        pcap = element.find('PcapAdapterInfo')
        if pcap is not None:
            self.adapter_type = config.ADAPTER_TYPE_NIC
            self.address = format_ethernet(pcap.findtext('Address',
                                                         '000000000000'))
            self.features = int(pcap.findtext('Flags', '0'))
            self.description = pcap.findtext('Description', '')
            self.device_name = self.description
            self.id = pcap.findtext('ID', '')
            self.interface_features = 0
            self.link_speed = int(pcap.findtext('LinkSpeed', '0'))
            self.media_type = int(pcap.findtext('MediaType', '0'))
            self.media_sub_type = int(pcap.findtext('MediaSubType', '0'))
            self.wildpackets_api = False
            return
        dpdk = element.find('DPDKAdapterInfo')
        if dpdk is not None:
            self.adapter_type = config.ADAPTER_TYPE_NIC
            self.address = format_ethernet(dpdk.findtext('Address',
                                                         '000000000000'))
            self.features = int(dpdk.findtext('Flags', '0'))
            self.description = dpdk.findtext('Description', '')
            self.device_name = self.description
            self.id = dpdk.findtext('ID', '')
            self.interface_features = 0
            self.link_speed = int(dpdk.findtext('LinkSpeed', '0'))
            self.media_type = int(dpdk.findtext('MediaType', '0'))
            self.media_sub_type = int(dpdk.findtext('MediaSubType', '0'))
            self.wildpackets_api = False
            return
        plugin = element.find('pluginadapterinfo')
        if plugin is not None:
            self.adapter_type = config.ADAPTER_TYPE_PLUGIN
            self.address = format_ethernet(plugin.findtext('address',
                                                           '000000000000'))
            self.features = 0
            self.description = plugin.findtext('description', '')
            self.device_name = self.description
            self.id = plugin.findtext('identifier', '')
            self.interface_features = 0
            self.link_speed = int(plugin.findtext('linkspeed', '0'))
            self.media_type = int(plugin.findtext('mediatype', '0'))
            self.media_sub_type = int(plugin.findtext('mediasubtype', '0'))
            self.wildpackets_api = False
            self.plugin = plugin.findtext('plugin', '')
            self.title = plugin.findtext('title', '')
            return

    def __repr__(self):
        return 'Adapter: %s' % self.description

    @property
    def name(self):
        """The name/description of the adapter. (Read Only)"""
        return self.description

    def rename(self, new_name):
        self._engine.rename_adapter(self.adapter_type, self.id, new_name)


def _create_adapter_list(engine, xml_list):
    lst = []
    adapterinfo = omniscript._parse_command_response(xml_list, 
                                                    'adapterinfolist')
    if adapterinfo is not None:
        for info in adapterinfo.findall('adapterinfoobj'):
            lst.append(Adapter(engine, info))
    lst.sort(key=lambda x: x.description)
    return lst


def find_adapter(adapters, value, attrib=find_attribs[0]):
    """Finds an adapter in the list."""
    if (not adapters) or (attrib not in find_attribs):
        return None

    if len(adapters) == 0:
        return None

    if isinstance(value, Adapter):
        _value = value.id
        attrib = 'id'
    else:
        _value = value

    return next((i for i in adapters if getattr(i, attrib) == _value), None)
