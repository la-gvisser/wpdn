"""Alarm class.
"""
#Copyright (c) WildPackets, Inc. 2013-2015. All rights reserved.

import os
import sys
import logging
import xml.etree.ElementTree as ET

import omniscript
import config

from omniid import OmniId
from peektime import PeekTime


find_attribs = ['name', 'id']


class Alarm(object):
    """The Alarm class has the attributes of an alarm.
    The 
    :func:`get_alarm_list() 
    <omniscript.omniengine.OmniEngine.get_alarm_list>`
    function returns a list of Alarm objects.
    """

    created = None
    """The time and date the alarm was created as
    :class:`PeekTime <omniscript.peektime.PeekTime>`.
    """

    id = OmniId()
    """The alarms's identifier."""

    modified = None
    """The time and date of the last modification as
    :class:`PeekTime <omniscript.peektime.PeekTime>`.
    """

    name = ''
    """The name of the alarm."""

    track_type = config.ALARM_TRACK_TYPE_UNDEFINED
    """The Track Type of the alarm."""

    def __init__(self, criteria):
        self.id = OmniId(True) if criteria is None else Alarm.id
        self.name = Alarm.name
        self.created = PeekTime() if criteria is None else Alarm.created
        self.modified = PeekTime() if criteria is None else Alarm.modified
        self.track_type = Alarm.track_type
        if isinstance(criteria, basestring):
            xml = ET.parse(criteria)
            self._load(xml)
        elif isinstance(criteria, ET.Element):
            self._load(criteria)

    def __repr__(self):
        return 'Alarm: %s' % self.name

    def _load(self, element):
        alarm = element.find('alarmnode')
        if alarm is not None:
            for attrib in alarm.items():
                if attrib[0] == 'id':
                    self.id = OmniId(attrib[1])
                elif attrib[0] == 'created':
                    self.created = PeekTime(attrib[1])
                elif attrib[0] == 'modified':
                    self.modified = PeekTime(attrib[1])
                elif attrib[0] == 'name':
                    self.name = attrib[1]
                elif attrib[0] == 'tracktype':
                    self.track_type = int(attrib[1])
            if self.id is None:
                self.id = OmniId(None)


def _create_alarm_list(xml_list):
    lst = []
    alarms = omniscript._parse_command_response(xml_list, 'Alarms')
    if alarms is not None:
        for obj in alarms.findall('Alarmobj'):
            lst.append(Alarm(criteria=obj))
    lst.sort(key=lambda x: x.name)
    return lst


def find_alarm(alarms, value, attrib=find_attribs[0]):
    """Finds an alarm in the list"""
    if (not alarms) or (attrib not in find_attribs):
        return None

    if len(alarms) == 0:
        return None

    if isinstance(value, Alarm):
        _value = value.id
        attrib = 'id'
    else:
        _value = value

    return next((i for i in alarms if getattr(i, attrib) == _value), None)
