"""FileAdapter class.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

import os
import sys
import logging


class FileAdapter(object):
    """File Adapter class"""

    filename = ''
    """The name of the file to replay."""
    
    limit = 0
    """The number of times to replay a file adapter: 0 for infininte."""

    mode = 0
    """The timestamping mode of a file adapter."""

    speed = 1.0
    """The replay speed multiplier of a file adapter."""

    def __init__(self, filename = None, dct = None):
        self.filename = filename
        self.limit = FileAdapter.limit
        self.mode = FileAdapter.mode
        self.speed = FileAdapter.speed

        if dct is not None:
            for k, v in dct.iteritems():
                a = _adapt_prop_dict.get(k)
                if a is not None:
                    if hasattr(self, a):
                        if isinstance(getattr(self, a), basestring):
                            setattr(self, a, v if v else '')
                        elif isinstance(getattr(self, a), (int, long)):
                            setattr(self, a, int(v) if v else 0)
                        elif isinstance(getattr(self, a), float):
                            setattr(self, a, float(v) if v else 0.0)

    def __repr__(self):
        return 'FileAdapter: %s' % self.filename

    @property
    def name(self):
        """ The name file to replay. (Read Only)"""
        return self.filename

    @property
    def adapter_type(self):
        """ The type of adapter: ADAPTER_TYPE_FILE. (Read Only)"""
        return omniscript.ADAPTER_TYPE_FILE
