"""MediaInfo class.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

import os
import sys
import xml.etree.ElementTree as ET

import config


class MediaInfo(object):
    """The MediaInfo class.
    """

    media_type = config.MEDIA_TYPE_802_3
    """The media type, one of the MEDIA_TYPE constants."""

    media_sub_type = config.MEDIA_SUB_TYPE_NATIVE
    """The media sub type, one of the MEDIA_SUB_TYPE constants."""

    domain = config.MEDIA_DOMAIN_NONE
    """The media domain, one of the MEDIA_DOMAIN constants."""

    link_speed = 0
    """The media's link speed in bits per second."""

    def __init__(self, node=None):
        self.media_type = MediaInfo.media_type
        self.media_sub_type = MediaInfo.media_sub_type
        self.domain = MediaInfo.domain
        self.link_speed = MediaInfo.link_speed
        #
        if node is not None:
            self.load(node)

    def __repr__(self):
        return 'MediaInfo: %d' % self.media_type

    def load(self, node):
        mi = node.find('MediaInfo')
        if mi is not None:
            self.media_type = int(mi.findtext('MediaType', '0'))
            self.media_sub_type = int(mi.findtext('MediaSubType', '0'))
            self.domain = int(mi.findtext('MediaDomain', '0'))
            self.link_speed = int(mi.findtext('LinkSpeed', '0'))
