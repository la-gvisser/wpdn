"""FileInformation class.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

import os
import sys
import logging
import xml.etree.ElementTree as ET

import omniscript
import config
from peektime import PeekTime


class FileInformation(object):
    """Information about a file.
    """

    flags = 0
    """The attribute flags of the file may be checked against the
    FILE FLAGS constants.
    """

    modified = None
    """The date and time, as
    :class:`PeekTime <omniscript.peektime.PeekTime>`,
    that the file was last modified."""

    name = ''
    """The fully qualified name of the file."""

    size = 0
    """The size of the file in bytes."""

    #XML Tags
    _tag_root_name = "file"
    _tag_flags = "attr"
    _tag_modified = "mod"
    _tag_name = "name"
    _tag_size = "size"

    def __init__(self, name='', flags=0, elem=None):
        self.flags = flags
        self.modified = FileInformation.modified
        self.name = name
        self.size = FileInformation.size
        if elem is not None:
            self._load(elem)

    def _load(self, elem):
        """Load the File Informaiont from an ETree SubElement."""
        if elem.tag == FileInformation._tag_root_name:
            info = elem
        else:
            info = elem.find(FileInformation._tag_root_name)
        if info is not None:
            self.flags = int(info.attrib[FileInformation._tag_flags])
            self.modified = PeekTime(
                info.attrib[FileInformation._tag_modified])
            self.name = info.attrib[FileInformation._tag_name]
            self.size = int(info.attrib[FileInformation._tag_size])

    def __repr__(self):
        return 'FileInformation: %s' % self.name

    def is_folder(self):
        """Returns True if the information is for a folder."""
        return (self.flags & config.FILE_FLAGS_FOLDER) != 0

def file_information_list_to_file_list(fil):
    """Transform a list of
    :class:`FileInformation <omniscript.fileinformation.FileInformation>`
    into a list of strings of fully qualified file names.
    """
    fl = []
    path = ''
    for f in fil:
        if f.is_folder():
            path = f.name
        else:
            fl.append(path + f.name)
    return fl
