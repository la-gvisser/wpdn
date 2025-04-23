"""CallStatistic class.

The :class:`Capture <omniscript.capture.Capture>` function
:func:`get_call_stats() <omniscript.capture.Capture.get_call_stats>`
returns a CallStatistic objects.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

import os
import sys
import logging
import time

import omniscript

from omnierror import OmniError
from peektime import PeekTime


class CallCodec(object):
    """A VoIP Call Codec"""

    index = 0
    """The enumeration index of the codec."""

    name = ''
    """The name of the codec."""

    samples = None
    """The samples of the codec. A list of
    :class:`CallSample <omniscript.callstatistic.CallSample>`
    objects.
    """

    def __init__(self, stream):
        self.index = CallCodec.index
        self.name = CallCodec.name
        self.samples = []
        if stream is not None:
            self._load(stream)

    def _load(self, stream):
        self.index = stream.read_uint()
        self.name = stream.read_unicode()
        sample_count = stream.read_uint()
        for s in range(sample_count):
            sample = CallSample(stream)
            self.samples.append(sample)


class CallCodecQuality(object):
    """Quality statistics of VoIP Calls by codec version 2."""

    interval = 0
    """The time interval of the statistic."""

    start_time = None
    """The begining time, as
   :class:`PeekTime <omniscript.peektime.PeekTime>`,
   of the statistics.
   """

    codecs = None
    """A list of 
    :class:`CallCodec <omniscript.callstatistic.CallCodec>`
    objects.
    """

    qualities = None
    """A list of 
    :class:`CallQuality <omniscript.callstatistic.CallQuality>`
    objects.
    """

    version = 0
    """The version of the CallCodecQuality.interval
    Version may be 1 or 2. Version 1 does not have list
    of qualities and the qualities property will be set
    to None.
    """
 
    def __init__(self, stream, version):
        self.interval = CallCodecQuality.interval
        self.start_time = PeekTime(0)
        self.codecs = []
        self.qualities = [] if version == 2 else None
        self.version = version
        if stream is not None:
            self._load(stream)

    def _load(self, stream):
        self.interval = stream.read_uint()
        self.start_time = PeekTime(stream.read_ulong())
        codec_count = stream.read_uint()
        for c in range(codec_count):
            codec = CallCodec(stream)
            self.codecs.append(codec)
        if self.version == 2:
            quality_count = stream.read_uint()
            for q in range(quality_count):
                quality = CallQuality(stream)
                self.qualities.append(quality)


class CallQuality(object):
    """The 
    :class:`CallQuality <omniscript.callstatistic.CallQuality>`
    class contains a list of these samples.
    """

    good = 0
    """The number of calls with good quality."""

    fair = 0
    """The number of calls with fair quality."""

    poor = 0
    """The number of calls with poor quality."""

    bad = 0
    """The number of calls with bad quality."""

    unknown = 0
    """The number of calls with unknown quality."""

    def __init__(self, stream):
        self.good = CallQuality.good
        self.fair = CallQuality.fair
        self.poor = CallQuality.poor
        self.bad = CallQuality.bad
        self.unknown = CallQuality.unknown
        if stream is not None:
            self._load(stream)

    def _load(self, stream):
        self.good = stream.read_uint()
        self.fair = stream.read_uint()
        self.poor = stream.read_uint()
        self.bad = stream.read_uint()
        self.unknown = stream.read_uint()


class CallSample(object):
    """The 
    :class:`CallCodec <omniscript.callstatistic.CallCodec>`
    class contains a list of these samples.
    """

    calls = 0
    """The number of calls."""

    flows = 0
    """The number of flows."""

    mos_average = 0
    """The average MOS score."""

    mos_minimum = 0
    """The minimum MOS score."""

    mos_maximum = 0
    """The maximum MOS score."""


    def __init__(self, stream):
        self.calls = CallSample.calls
        self.flows = CallSample.flows
        self.mos_average = CallSample.mos_average
        self.mos_minimum = CallSample.mos_minimum
        self.mos_maximum = CallSample.mos_maximum
        if stream is not None:
            self._load(stream)

    def _load(self, stream):
        self.calls = stream.read_uint()
        self.flows = stream.read_uint()
        self.mos_average = stream.read_ushort()
        self.mos_minimum = stream.read_ushort()
        self.mos_maximum = stream.read_ushort()
        # Unused
        stream.read_ushort()


class CallUtilization(object):
    """Call Utilization statistics."""

    interval = 0
    """The time interval of the statistic."""

    start_time = None
    """The begining time, as
    :class:`PeekTime <omniscript.peektime.PeekTime>`,
    of the statistics."""

    samples = None
    """The utilization samples."""

    def __init__(self, stream):
        self.interval = CallUtilization.interval
        self.start_time = PeekTime(0)
        self.samples = []
        if stream is not None:
            self._load(stream)

    def _load(self, stream):
        version = stream.read_uint()
        if version != 1:
            raise OmniError('Unsupported Call Utilization version.', )
        self.interval = stream.read_uint()
        self.start_time = PeekTime(stream.read_ulong())
        sample_count = stream.read_uint()
        for i in range(sample_count):
            self.samples.append(stream.read_double())


class CallStatistic(object):
    """The CallStatistic class has the statistic of VoIP Calls.
    The statistics include Quality Distribution, Utilization and Quality.
    """

    all_calls = None
    """Quality Distribution of all calls.
    A Map with 'Good', 'Fair', 'Poor' and 'Bad' counts.
    """

    open_calls = None
    """Quality Distribution of all calls.
    A Map with 'Good', 'Fair', 'Poor' and 'Bad' counts.
    """

    utilizations = None
    """List of 
    :class:`CallUtilization <omniscript.callstatistic.CallUtilization>`
    objects.
    """

    qualities = None
    """List of 
    :class:`CallCodecQuality <omniscript.callstatistic.CallCodecQuality>`
    objects.
    """

    def __init__(self, stream, offset=0):
        # [Good], [Fair], [Poor], [Bad]
        self.all_calls = {'Good':0, 'Fair':0, 'Poor':0, 'Bad':0}
        self.open_calls = {'Good':0, 'Fair':0, 'Poor':0, 'Bad':0}
        self.utilizations = []
        self.qualities = []
        if stream is not None:
            self._load(stream, offset)

    def _load(self, stream, offset):
        class_name_ids = omniscript.get_class_name_ids()
        if offset > 0:
            stream.seek(offset, os.SEEK_SET)
        clsid = stream.read_guid()
        objid = stream.read_guid()
        if clsid != class_name_ids['CallStats']:
            raise OmniError('Call Statistics not found.')
        version = stream.read_uint()
        if version != 1:
            raise OmniError('Unsupported Call Statistics version.')
        
        # Load Call Quality Distribution
        self.all_calls['Good'] = stream.read_uint()
        self.all_calls['Fair'] = stream.read_uint()
        self.all_calls['Poor'] = stream.read_uint()
        self.all_calls['Bad'] = stream.read_uint()

        self.open_calls['Good'] = stream.read_uint()
        self.open_calls['Fair'] = stream.read_uint()
        self.open_calls['Poor'] = stream.read_uint()
        self.open_calls['Bad'] = stream.read_uint()

        # Load Call Utilization
        version = stream.read_uint()
        if version != 1:
            raise OmniError('Unsupported Call Statistics version.')
        utilization_count = stream.read_uint()
        for i in range(utilization_count):
            utilization = CallUtilization(stream)
            self.utilizations.append(utilization)

        # Load Call Codec Quality
        version = stream.read_uint()
        if version == 1 or version == 2:
            quality_count = stream.read_uint()
            for i in range(quality_count):
                quality = CallCodecQuality(stream, version)
                self.qualities.append(quality)
        else:
            raise OmniError('Unsupported Call Statistics version.')

    def get(self):
        return self
