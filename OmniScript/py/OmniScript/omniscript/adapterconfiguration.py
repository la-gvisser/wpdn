"""AdapterConfiguration class.
"""
#Copyright (c) Savvius, Inc. 2018. All rights reserved.

import os
import sys
import logging
import xml.etree.ElementTree as ET

import omniscript

from omniid import OmniId
from omnierror import OmniError


class AdapterConfiguration(object):
    """Adapter Configuration class."""

    id = None
    """The adapter's identifier, on Windows is a four digit code that is
    engine specific, on Linux is the device name: 'eth0'.
    """

    adapter_type = 0
    """The type of adapter. Any of the ADDRESS TYPE constants."""

    link_speed = 0
    """The link speed of the adapter in bits per second."""

    default_link_speed = 0
    """The default link speed of the adapter in bits per second."""

    ring_buffer_size = 0
    """The size of the adapter's ring buffer in bytes."""

    ids = None
    """The configuration ids for the current settings."""

    #XML Tags
    _tag_id = "adapter-id"
    _tag_type = "adapter-type"
    _tag_class = "AdapterConfig"
    _tag_obj = "adapter-config"
    _tag_config = "adapterconfig"
    _tag_link_speed = "linkspeed"
    _tag_default_link_speed = "defaultlinkspeed"
    _tag_ring_buffer_size = "ringbuffersize"
    _tag_ids = "ids"
    _tag_id_ = "id"

    def __init__(self, node):
        self.id = AdapterConfiguration.id
        self.adapter_type = AdapterConfiguration.link_speed
        self.link_speed = AdapterConfiguration.link_speed
        self.default_link_speed = AdapterConfiguration.default_link_speed
        self.ring_buffer_size = AdapterConfiguration.ring_buffer_size
        self.ids = []
        self._load(node)

    def __repr__(self):
        return 'Adapter Configuration: %s' % (self.id)

    def _load(self, node):
        if node is None:
            return
        self.id = node.findtext(AdapterConfiguration._tag_id, '')
        self.adapter_type = int(node.findtext(AdapterConfiguration._tag_type, 0))
        obj = node.find(AdapterConfiguration._tag_obj)
        if obj is not None:
            config = obj.find(AdapterConfiguration._tag_config)
            if config is not None:
                self.link_speed = int(config.findtext(AdapterConfiguration._tag_link_speed, 0))
                self.default_link_speed = \
                    int(config.findtext(AdapterConfiguration._tag_default_link_speed, 0))
                self.ring_buffer_size = \
                    int(config.findtext(AdapterConfiguration._tag_ring_buffer_size, 0))
                ids = config.find(AdapterConfiguration._tag_ids)
                if ids is not None:
                    self.ids = []
                    for id in ids.findall(AdapterConfiguration._tag_id_):
                        self.ids.append(OmniId(id.text))

    def _store(self, node):
        class_name_ids = omniscript.get_class_name_ids()
        ET.SubElement(node, AdapterConfiguration._tag_id).text = str(self.id)
        ET.SubElement(node, AdapterConfiguration._tag_type).text = str(self.adapter_type)
        obj = ET.SubElement(node, AdapterConfiguration._tag_obj,
                            {'clsid': str(class_name_ids[AdapterConfiguration._tag_class])})
        config = ET.SubElement(obj, AdapterConfiguration._tag_config)
        ET.SubElement(config, AdapterConfiguration._tag_link_speed).text = str(self.link_speed)
        ET.SubElement(config, AdapterConfiguration._tag_default_link_speed).text = \
            str(self.default_link_speed)
        ET.SubElement(config, AdapterConfiguration._tag_ring_buffer_size).text = \
            str(self.ring_buffer_size)
        ids = ET.SubElement(config, AdapterConfiguration._tag_ids)
        for id in self.ids:
            ET.SubElement(ids, AdapterConfiguration._tag_id_).text = str(id)
