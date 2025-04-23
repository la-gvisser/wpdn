"""OmniId class.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

import os
import sys
import logging
import uuid

class OmniId(object):
    """A uuid.UUID with a default string format:
    {ABCDEF01-2345-6789-ABCD-0123456789AB}
    """

    id = None
    """The UUID (GUID)."""

    null_id = uuid.UUID(int=0)

    def __init__(self, value=None, create=False, bytes_le=None):
        """Initialize the OmniId

        Args:
            value(str or bool) : optional initial value or flag to generate
                                 a new GUID.
        """
        if value is not None:
            if isinstance(value, basestring):
                self.id = uuid.UUID(value)
            elif isinstance(value, bool) and value:
                self.id = uuid.uuid4()
            else:
                self.id = OmniId.null_id
        elif bytes_le is not None:
            self.id = uuid.UUID(bytes_le=bytes_le)
        else:
            self.id = OmniId.null_id

    def __cmp__(self, other):
        if isinstance(other, OmniId):
            return self.id.__cmp__(other.id)
        if isinstance(other, basestring):
            return self.id.__cmp__(uuid.UUID(other))

    def __hash__(self):
        return self.id.__hash__()

    def __repr__(self):
        if self.id is None:
            return ''
        return '{' + str(self.id).upper() + '}'

    def bytes_le(self):
        """Return the id as a 16-byte string in little-endian format"""
        return self.id.bytes_le

    @staticmethod
    def is_id(value):
        if isinstance(value, OmniId):
            return True
        if isinstance(value, basestring):
            if len(value) == 38:
                if value[0] != '{': return False
                if value[9] != '-': return False
                if value[14] != '-': return False
                if value[19] != '-': return False
                if value[24] != '-': return False
                if value[37] != '}': return False
                return True
        return False

    def parse(self, value):
        """Parse the value into the id."""
        self.id = uuid.UUID(value)

    def new(self):
        """Create a new UUID/GUID."""
        self.id = uuid.uuid4()
