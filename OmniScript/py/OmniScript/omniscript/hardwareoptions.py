"""HardwareOptions class.
"""
#Copyright (c) Savvius, Inc. 2018. All rights reserved.

import os
import sys
import logging
import xml.etree.ElementTree as ET

import omniscript
import config

from omniid import OmniId
from omnierror import OmniError
from peektime import PeekTime

find_attribs = ['name', 'id']
scanning_find_attribs = ['number', 'frequency', 'band']

class WEPKey(object):
    """A WEP Key."""

    data = None
    """The key value."""

    bit_length = 0
    """The number of bits in the key."""

    #XML tags
    _tag_class_name = "Key"
    _tag_bit_length = "KeyBitLen"
    _tag_data = "KeyData"

    def __init__(self, node):
        self.data = WEPKey.data
        self.bit_length = WEPKey.bit_length
        self._load(node)

    def _load(self, node):
        if node is not None:
            self.data = node.findtext(WEPKey._tag_data, '')
            self.bit_length = int(node.findtext(WEPKey._tag_bit_length, 0))

    def _store(self, node):
        _key = ET.SubElement(node, WEPKey._tag_class_name)
        ET.SubElement(_key, WEPKey._tag_bit_length).text = str(self.bit_length)
        ET.SubElement(_key, WEPKey._tag_data).text = self.data


class WEPKeySet(object):
    """Wireless WEP Key Set."""

    algorithm = 0
    """the alogorithm of the key set."""

    keys = None
    """The list of keys."""

    name = None
    """The name of the key set."""

    #XML tags
    _tag_wep_keys = "WEPKeySet"
    _tag_algorithm = "Algorithm"
    _tag_key = "Key"
    _tag_keys = "Keys"
    _tag_name = "Name"

    def __init__(self, filename=None, node=None):
        self.algorithm = WEPKeySet.algorithm
        self.keys = []
        self.name = WEPKeySet.name
        if filename is not None: self._load(filename)
        if node is not None: self._load(node)

    def _load(self, node):
        if node is None:
            return
        if isinstance(node, basestring):
            wep_keys = ET.parse(node)
        else:
            wep_keys = node.find(WEPKeySet._tag_wep_keys)
        if wep_keys is not None:
            self.algorithm = int(wep_keys.findtext(WEPKeySet._tag_algorithm, 0))
            self.name = wep_keys.findtext(WEPKeySet._tag_name, None)
            _keys = wep_keys.find(WEPKeySet._tag_keys)
            if _keys is not None:
                for key in _keys.findall(WEPKeySet._tag_key):
                    _wep_key = WEPKey(key)
                    self.keys.append(_wep_key)

    def _store(self, node):
        wep_keys = ET.SubElement(node, WEPKeySet._tag_wep_keys)
        ET.SubElement(wep_keys, WEPKeySet._tag_algorithm).text = str(self.algorithm)
        if self.name:
            ET.SubElement(wep_keys, WEPKeySet._tag_name).text = self.name
        _keys = ET.SubElement(wep_keys, WEPKeySet._tag_keys)
        for key in self.keys:
            key._store(_keys)

    def to_xml(self):
        wep_keys = ET.Element(WEPKeySet._tag_wep_keys)
        ET.SubElement(wep_keys, WEPKeySet._tag_algorithm).text = str(self.algorithm)
        if self.name:
            ET.SubElement(wep_keys, WEPKeySet._tag_name).text = self.name
        _keys = ET.SubElement(wep_keys, WEPKeySet._tag_keys)
        for key in self.keys:
            key._store(_keys)
        return ET.tostring(wep_keys).replace('\n', '')


class ChannelScanEntry(object):
    """Channel Scanning Entry."""

    channel_number = 0
    """The channel number."""

    channel_frequency = 0
    """The frequency of the channel."""

    channel_band = 0
    """The band of the channel.
    One of the WIRELESS_BAND constants.
    """

    duration = 0
    """The duration, in milliseconds, to scan this channel."""

    enabled = False
    """Is this entry enabled?"""

    #XML Tags
    _tag_settings = "ChannelScanningSetting"
    _tag_channel_number = "ChNum"
    _tag_channel_frequency = "ChFreq"
    _tag_channel_band = "ChBand"
    _tag_duration = "Duration"
    _tag_enabled = "Enabled"

    def __init__(self, node):
        self.channel_number = ChannelScanEntry.channel_number
        self.channel_frequency = ChannelScanEntry.channel_frequency
        self.channel_band = ChannelScanEntry.channel_band
        self.duration = ChannelScanEntry.duration
        self.enabled = ChannelScanEntry.enabled
        self._load(node)

    def __repr__(self):
        return 'Channel Scan Entry: %d, %d, %s' % \
             (self.channel_number, self.channel_frequency,
              omniscript._wireless_band_id_names[self.channel_band])

    def _load(self, node):
        if node is None:
            return
        self.channel_number = int(node.findtext(ChannelScanEntry._tag_channel_number, 0))
        self.channel_frequency = int(node.findtext(ChannelScanEntry._tag_channel_frequency, 0))
        self.channel_band = int(node.findtext(ChannelScanEntry._tag_channel_band, 0))
        self.duration = int(node.findtext(ChannelScanEntry._tag_duration, 0))
        self.enabled = int(node.findtext(ChannelScanEntry._tag_enabled, 0)) != 0

    def _store(self, node):
        settings = ET.SubElement(node, ChannelScanEntry._tag_settings)
        ET.SubElement(settings, ChannelScanEntry._tag_channel_number).text = str(self.channel_number)
        ET.SubElement(settings, ChannelScanEntry._tag_channel_frequency).text = str(self.channel_frequency)
        ET.SubElement(settings, ChannelScanEntry._tag_channel_band).text = str(self.channel_band)
        ET.SubElement(settings, ChannelScanEntry._tag_duration).text = str(self.duration)
        ET.SubElement(settings, ChannelScanEntry._tag_enabled).text = str(int(self.enabled))


class HardwareOptions(object):
    """Information about hardware."""

    id = None
    """The id (GUID/UUID) of the hardware item as a
    :class:`OmniId <omniscript.omniid.OmniId>`.
    """

    name = ""
    """The name of the hardware item."""

    comment = ""
    """The comment of the hardware item."""

    color = 0
    """The comment of the hardware item."""

    created = ""
    """The date and time of when of the hardware item
    was created as a 
    :class:`PeekTime <omniscript.peektime.PeekTime>`.
    """

    modified = ""
    """The date and time of when hardware item was last
    modified as a 
    :class:`PeekTime <omniscript.peektime.PeekTime>`.
    """

    _engine = None
    """The engine the hardware belongs to as a 
    :class:`OmniEngine <omniscript.omniengine.OmniEngine>`.
    """

    #XML Tags
    _tag_class_name = "HardwareOptions"
    _tag_id = "ID"
    _tag_name = "Name"
    _tag_comment = "Comment"
    _tag_color = "Color"
    _tag_created = "Created"
    _tag_modified = "Modified"

    def __init__(self, engine):
        self._engine = engine
        self.id = HardwareOptions.id
        self.name = HardwareOptions.name
        self.comment = HardwareOptions.comment
        self.color = HardwareOptions.color
        self.created = HardwareOptions.created
        self.modified = HardwareOptions.modified

    def __repr__(self):
        return 'Hardware Options: %s' % (self.name)

    def _load(self, node):
        if node is None:
            return
        self.id = OmniId(node.findtext(HardwareOptions._tag_id, ''))
        self.name = node.findtext(HardwareOptions._tag_name, '')
        self.comment = node.findtext(HardwareOptions._tag_comment, '')
        self.color = int(node.findtext(HardwareOptions._tag_color, 0))
        self.created = PeekTime(node.findtext(HardwareOptions._tag_created, ''))
        self.modified = PeekTime(node.findtext(HardwareOptions._tag_modified, ''))

    def _store(self, node):
        ET.SubElement(node, HardwareOptions._tag_id).text = str(self.id)
        ET.SubElement(node, HardwareOptions._tag_name).text = str(self.name)
        ET.SubElement(node, HardwareOptions._tag_comment).text = str(self.comment)
        ET.SubElement(node, HardwareOptions._tag_color).text = str(self.color)
        ET.SubElement(node, HardwareOptions._tag_created).text = str(self.created)
        ET.SubElement(node, HardwareOptions._tag_modified).text = str(self.modified)


class WirelessHardwareOptions(HardwareOptions):
    """Information about wireless hardware."""

    configuration = 0
    """The configuration index of the adapter.
    One of the WIRELESS_CONFIGURATION constants.
    """

    channel_number = 0
    """The number of the channel the wireless adapter is tuned to."""

    channel_frequency = 0
    """The frequency the wireless adapter is tuned to."""

    channel_band = 0
    """The band the wireless adapter is tuned to.
    One of the WIRELESS_BAND constants.
    """

    essid = ""
    """The ESSID the wireless adapter has joined"""

    bssid = ""
    """The BSSID the wireless adapter has joined"""

    channel_scanning = None
    """The channel scanning settings as a list of
    :class:`ChannelScanEntry <omniscript.hardwareoptions.ChannelScanEntry>`.
    """

    key_set = None
    """The key set the wireless adapter is using."""

    #XML Tags
    _tag_class_name = "WirelessHardwareOptions"
    _tag_configuration = "Config"
    _tag_channel_number = "ChNum"
    _tag_channel_frequency = "ChFreq"
    _tag_channel_band = "ChBand"
    _tag_essid = "ESSID"
    _tag_bssid = "BSSID"
    _tag_entry_list = "ChannelScanningSettings"
    _tag_entry = "ChannelScanningSetting"
    _tag_key_set = "KeySet"
    _tag_key_set_class = "WEPKeySet"

    def __init__(self, engine, node):
        HardwareOptions.__init__(self, engine)
        self.configuration = WirelessHardwareOptions.configuration
        self.channel_number = WirelessHardwareOptions.channel_number
        self.channel_frequency = WirelessHardwareOptions.channel_frequency
        self.channel_band = WirelessHardwareOptions.channel_band
        self.essid = WirelessHardwareOptions.essid
        self.bssid = WirelessHardwareOptions.bssid
        self.channel_scanning = []
        self.key_set = WirelessHardwareOptions.key_set
        self._load(node)

    def __repr__(self):
        return 'Wireless Hardware Options: %d %d %d' % \
            (self.channel_number, self.channel_frequency, self.channel_band)

    def _load(self, node):
        if node is None:
            return
        HardwareOptions._load(self, node)
        self.configuration = int(node.findtext(WirelessHardwareOptions._tag_configuration, 0))
        self.channel_number = int(node.findtext(WirelessHardwareOptions._tag_channel_number, 0))
        self.channel_frequency = int(node.findtext(WirelessHardwareOptions._tag_channel_frequency, 0))
        self.channel_band = int(node.findtext(WirelessHardwareOptions._tag_channel_band, 0))
        self.essid = node.findtext(WirelessHardwareOptions._tag_essid, '')
        self.bssid = node.findtext(WirelessHardwareOptions._tag_bssid, '')
        key_set = node.find(WirelessHardwareOptions._tag_key_set)
        if key_set is not None:
            # clsid = OmniId(key_set.attrib['clsid'])
            self.key_set = WEPKeySet(node=key_set)
        entry_list = node.find(WirelessHardwareOptions._tag_entry_list)
        if entry_list is not None:
            for entry in entry_list.findall(WirelessHardwareOptions._tag_entry):
                self.channel_scanning.append(ChannelScanEntry(entry))

    def _store(self, node):
        class_name_ids = omniscript.get_class_name_ids()
        HardwareOptions._store(self, node)
        ET.SubElement(node, WirelessHardwareOptions._tag_configuration).text = str(self.configuration)
        ET.SubElement(node, WirelessHardwareOptions._tag_channel_number).text = str(self.channel_number)
        ET.SubElement(node, WirelessHardwareOptions._tag_channel_frequency).text = str(self.channel_frequency)
        ET.SubElement(node, WirelessHardwareOptions._tag_channel_band).text = str(self.channel_band)
        ET.SubElement(node, WirelessHardwareOptions._tag_essid).text = str(self.essid)
        ET.SubElement(node, WirelessHardwareOptions._tag_bssid).text = str(self.bssid)
        if self.key_set:
            key_set = ET.SubElement(node, WirelessHardwareOptions._tag_key_set,
                                    {'clsid': str(class_name_ids[WirelessHardwareOptions._tag_key_set_class])})
            self.key_set._store(key_set)
        if self.channel_scanning:
            entry_list = ET.SubElement(node, WirelessHardwareOptions._tag_entry_list)
            for entry in self.channel_scanning:
                entry._store(entry_list)

    def find_channel_scanning_entry(self, value, attrib=scanning_find_attribs[0]):
        """Searches the Channel Scanning list for matching entries.
        Find by 'number', 'frequency' or 'band'.
        Returns a list of 
        :class:`ChannelScanEntry <omniscript.hardwareoptions.ChannelScanEntry>`
        objects.
        """
        _attrib = 'channel_' + attrib
        return [x for i, x in enumerate(self.channel_scanning) if getattr(x, _attrib) == value]

    def set_channel(self, value, frequency=None, band=None):
        """Set the channel number, frequency and band by either a
        :class:`ChannelScanEntry <omniscript.hardwareoptions.ChannelScanEntry>`
        or the individual number, frequency and band.
        """
        if isinstance(value, ChannelScanEntry):
            self.channel_number = value.channel_number
            self.channel_frequency = value.channel_frequency
            self.channel_band = value.channel_band
            self.config = 0
        elif isinstance(value, (basestring, int, long)):
            self.channel_number = int(value)
            self.channel_frequency = int(frequency)
            self.channel_band = int(band)
            self.config = 0
        else:
            raise TypeError("The type of 'value' is not supported.")

    def set_channel_scanning(self, enable=True):
        if enable:
            self.configuration = config.WIRELESS_CONFIGURATION_CHANNEL_SCANNING
        else:
            self.configuration = config.WIRELESS_CONFIGURATION_SINGLE_CHANNEL

    def to_xml(self):
        class_name_ids = omniscript.get_class_name_ids()
        hw_options = ET.Element(WirelessHardwareOptions._tag_class_name)
        HardwareOptions._store(self, hw_options)
        ET.SubElement(hw_options, WirelessHardwareOptions._tag_configuration).text = str(self.configuration)
        ET.SubElement(hw_options, WirelessHardwareOptions._tag_channel_number).text = str(self.channel_number)
        ET.SubElement(hw_options, WirelessHardwareOptions._tag_channel_frequency).text = str(self.channel_frequency)
        ET.SubElement(hw_options, WirelessHardwareOptions._tag_channel_band).text = str(self.channel_band)
        ET.SubElement(hw_options, WirelessHardwareOptions._tag_essid).text = str(self.essid)
        ET.SubElement(hw_options, WirelessHardwareOptions._tag_bssid).text = str(self.bssid)
        if self.key_set:
            key_set = ET.SubElement(hw_options, WirelessHardwareOptions._tag_key_set,
                                    {'clsid': str(class_name_ids[WirelessHardwareOptions._tag_key_set_class])})
            self.key_set._store(key_set)
        if self.channel_scanning:
            entry_list = ET.SubElement(hw_options, WirelessHardwareOptions._tag_entry_list)
            for entry in self.channel_scanning:
                entry._store(entry_list)
        return ET.tostring(hw_options).replace('\n', '')


def _create_hardware_options_list(engine, xml_list):
    class_name_ids = omniscript.get_class_name_ids()
    lst = []
    collection = omniscript._parse_command_response(
        xml_list, 'hardware-options-collection')
    if collection is not None:
        for obj in collection.findall('obj'):
            if obj is not None:
                clsid = OmniId(obj.attrib['clsid'])
                if clsid == class_name_ids[WirelessHardwareOptions._tag_class_name]:
                    inst = obj.find(WirelessHardwareOptions._tag_class_name)
                    lst.append(WirelessHardwareOptions(engine, inst))
    lst.sort(key=lambda x: x.name)
    return lst


def _store_hardware_options_list(request, options_list):
    class_name_ids = omniscript.get_class_name_ids()
    collection = ET.SubElement(request, 'hardware-options-collection')

    if isinstance(options_list, HardwareOptions):
        lst = [options_list]
    else:
        lst = options_list
    for options in lst:
        obj = ET.SubElement(collection, 'obj', {'clsid': str(class_name_ids[options._tag_class_name])})
        inst = ET.SubElement(obj, options._tag_class_name)
        options._store(inst)
    return request


def find_hardware_options(options, value, attrib=find_attribs[0]):
    """Finds hardware options in the list. Returns None if not found."""
    if (not options) or (attrib not in find_attribs):
        return None

    if len(options) == 0:
        return None

    if isinstance(options, HardwareOptions):
        _value = value.id
        attrib = 'id'
    else:
        _value = value

    return next((i for i in options if getattr(i, attrib) == _value), None)
