"""OmniEngine class.
"""
#Copyright (c) Savvius, Inc. 2013-2019. All rights reserved.

import os
import sys
import logging
import ctypes
import socket
import struct
import time
import xml.etree.ElementTree as ET

import omniscript
import config

from accesscontrollist import AccessControlList
from adapter import Adapter
from adapterconfiguration import AdapterConfiguration
from alarm import Alarm
from analysismodule import AnalysisModule
from auditlog import AuditLog, AuditLogEntry
from capture import Capture
from capturesession import CaptureSession
from capturetemplate import CaptureTemplate
from enginestatus import EngineStatus
from eventlog import EventLog, EventLogEntry
from fileinformation import FileInformation
from filter import Filter
from forensicfile import ForensicFile
from forensicsearch import ForensicSearch
from forensictemplate import ForensicTemplate
from graphtemplate import GraphTemplate
from hardwareoptions import HardwareOptions
from hostplatform import HostPlatform
from omnidatatable import OmniDataTable
from omnierror import OmniError
from omniid import OmniId
from peektime import PeekTime
from statscontext import StatsContext

from adapter import find_adapter
from analysismodule import find_analysis_module
from capture import find_capture
from filter import find_filter
from filter import read_filter_file
from forensicsearch import find_forensic_search

from adapter import _create_adapter_list
from alarm import _create_alarm_list
from analysismodule import _create_analysis_module_list
from capture import _create_capture_list
from capturesession import _create_capture_session_list
from filter import _create_filter_list
from forensicfile import _create_forensic_file_list
from forensicsearch import _create_forensic_search_list
from graphtemplate import _create_graph_template_list
from hardwareoptions import _create_hardware_options_list, _store_hardware_options_list


# Get Stats types
STATS_NODE = 1
STATS_PROTOCOL = 2
STATS_SUMMARY = 3

find_attributes = ['name', 'id']
_attrib_capture_tags = ['Name', 'ID']

WRITE_OP_CREATE = 0
WRITE_OP_WRITE = 1
WRITE_OP_CLOSE = 2

DIAGNOSTIC_DEFAULT = ''
DIAGNOSTIC_RAID = 'adapters'
DIAGNOSTIC_DRIVES = 'drives'
DIAGNOSTIC_PROCESSES = 'processes'
DIAGNOSTIC_MESSAGES = 'log'

FILTER_MODIFY_ADD = 1
FILTER_MODIFY_MODIFY = 2
FILTER_MODIFY_DELETE = 3


def _xml_capture_list(capture):
    """Returns an xml string of the capture ids."""
    capturelist = capture if isinstance(capture, list) else [capture]
    xml = ''
    for c in capturelist:
        if isinstance(c, basestring):
            xml += '<capture id="%s" />' % str(OmniId(c))
        elif isinstance(c, OmniId):
            xml += '<capture id="%s" />' % str(c)
        elif isinstance(c, Capture):
            xml += '<capture id="%s" />' % str(c.id)
        else:
            raise TypeError("capture must be or contain a GUID.")
    return xml


def _xml_capture_session_list(session):
    """Returns an xml string of the session ids."""
    sessionlist = session if isinstance(session, list) else [session]
    xml = ''
    for s in sessionlist:
        if isinstance(s, (int, long, basestring)):
            xml += '<CaptureSession>%d</CaptureSession>' % s
        elif isinstance(s, CaptureSession):
            xml += '<CaptureSession>%d</CaptureSession>' % s.session_id
        else:
            raise TypeError("session must be or contain an integer session id.")
    return xml


def _xml_filter_list(filterlist):
    """Returns a list of Filter objects."""
    itemlist = filterlist if isinstance(filterlist, list) else [filterlist]
    filters = []
    for item in itemlist:
        if isinstance(item, basestring):
            f = Filter({'Name':''})
            f.id = str(OmniId(item))
        elif isinstance(item, OmniId):
            f = Filter({'Name':''})
            f.id = str(item)
        elif isinstance(item, Filter):
            f = item
        else:
            raise TypeError("filter_id must be or contain a GUID.")
        filters.append(f)
    return filters


class OmniEngine(object):
    """The OmniEngine class provides access to an OmniEngie.
    The function
    :func:`connect() <omniscript.omniscript.OmniScript.connect>`
    returns an OmniEngine object.
    """

    logger = None
    """The logging object for the engine."""

    host = ''
    """The address of the host system."""

    port = 6367
    """The TCP port used to communicate with the host system."""

    timeout = 600000
    """The default timeout, in milliseconds, for issuing commands.
    The default is 10 minutes.
    """

    _os_encoding = 'utf-32'
    """The encoding to use on this system (sys.platform).
    """

    _platform = None
    """Inforamation about the host platform (HostPlatform)."""

    _filter_list = None
    """Cached Filter List: (
    :class:`Filter <omniscript.filter.Filter>`,
    :class:`PeekTime <omniscript.peektime.PeekTime>`).
    """

    _filter_timeout = 120 * 1000000000
    """The timeout, in nanoseconds, for refreshing the Filter List.
    Default 2 minutes.
    """

    def __init__(self, omni, logger, host, port, platform=None):
        self._omni = omni
        self._os_encoding = OmniEngine._os_encoding if sys.platform != 'win32' else 'utf-16'
        protocol = platform.get_protocol() if platform else 0
        self._connection = omni._new_connection(protocol)
        self.logger = logger
        self.host = host if host else 'localhost'
        self.port = int(port) if port else OmniEngine.port
        self.timeout = OmniEngine.timeout
        self._platform = platform
        self._filter_list = None
        self._filter_timeout = OmniEngine._filter_timeout
        self._last_status = None
    
    def __repr__(self):
        if self._last_status is None:
            return 'OmniEngine'
        else:
            return 'OmniEngine: %s' % self._last_status.name

    def _debug(self, parameters, timeout=1000):
        """Issue a debug command.
        This command can crash the engine, so the default timeout is 1 second.
        The crash parameter is '-crash'.
        """
        xml = '<Parameters>' + parameters + '</Parameters>'
        request = r'<request>' + xml + r'</request>'
        response = self._issue_xml_string_result(config.OMNI_DEBUG_COMMAND, request, timeout)
        omniscript._parse_command_response(response)

    def _get_stats(self, context, command_id):
        return self._omni._get_stats(context, command_id)

    def _issue_command(self, command_id, request, length, timeout=0):
        _timeout = timeout if timeout > 0 else self.timeout
        return self._omni._issue_command(self._connection, command_id,
                                        _timeout, request, length)

    def _issue_xml_command(self, command_id, request, timeout=0):
        _timeout = timeout if timeout > 0 else self.timeout
        return self._omni._issue_xml_command(self._connection, command_id,
                                            _timeout, request)

    def _issue_xml_string_result(self, command_id, request, timeout=0):
        _timeout = timeout if timeout > 0 else self.timeout
        return self._omni._issue_xml_string_result(self._connection, command_id,
                                                  _timeout, request)

    def add_filter(self, omnifilter):
        """Add one or more filters to the engine's filter set.

        Args:
            omnifilter (str, Filter, or list): the filter to add.
        """
        class_name_ids = omniscript.get_class_name_ids()
        itemlist = []
        filterlist = omnifilter if isinstance(omnifilter, list) else [omnifilter]
        for f in filterlist:
            if isinstance(f, basestring):
                itemlist.append(Filter(criteria=f))
            elif isinstance(f, Filter):
                itemlist.append(f)
            else:
                raise TypeError("omnifilter must be or contain a Filter.")
        if len(itemlist) == 0:
            return None

        mf = ET.Element('modify-filters')
        mi = ET.SubElement(mf, 'mutable-items',
                           {'clsid':str(class_name_ids['Filter'])})
        for i in itemlist:
            item = ET.SubElement(mi, 'item', {'type':str(FILTER_MODIFY_ADD)})
            i._store(item)
        xml = ET.tostring(mf).replace('\n', '')
        response = self._issue_xml_command(config.OMNI_MODIFY_FILTERS, xml)
        prop = omniscript._parse_command_response(response, 'prop')
        if prop is None:
            return None
        return self.find_filter(prop.text, 'id')

    def add_forensic_file(self, source, destination=None):
        """Add a packet file to the system's file set.

        Args:
            source (str): the name of the local file to add.
            destination (optional str): the path, relative to the engine's Data
            Folder, and name of the file. If not specified, then the file
            will be stored in the Data Folder.

        Note:
            Call sync_forensic_database() after adding packet files to make
            the new files availble to forensic searches.
        """
        self.send_file(source, destination)

    def clear_audit_log(self):
        """Clearn the OmniEngine's Audit Log.
        """
        self._issue_xml_command(config.OMNI_CLEAR_AUDIT_LOG, None)

    def connect(self,
                auth = '',
                domain = '',
                user = '',
                password = '',
                timeout = 30000):
        """Estabilish a connection to an OmniEngine.

        Args:
            auth (str): the authorization type: Default or Third Party;
            or 0, 1; or None to auto-detect.
            domain (str): the domain of the user's account.
            user (str): the name of the user's account.
            password (str): the password of the user's account.
            timeout (int): the timeout in milliseconds. Default is 30 seconds.
            Peek Error Codes:
            - 0x80090308 :
            - 0x8009030C : Username & password are incorrect.
            - 0x80004005 : General Failure
            - 0x8004294D : Connection attempt refused. Engine not running?

        Returns:
            Success: True or False.
        """
        _auth = HostPlatform.get_auth(self._platform, auth)
        _domain = '' if not domain else domain
        _account = '' if not user else user
        _password = '' if not password else password
        _timeout = '30000' if not timeout else str(timeout)
        credentials = ', '.join([self.host, str(self.port), _auth, _domain,
                                 _account, _password, _timeout])
        offset = 4 if sys.platform == 'linux2' else 2
        encoded = credentials.encode(self._os_encoding)
        request = encoded[offset:]
        response = self._issue_command(config.OMNI_CONNECT, request, len(request))
        if response is None:
            return False
        result = omniscript._get_command_response(response)
        return (result == 0)

    def convert_capture_file(self, source, destination, format):
        """Convert pkt file to a differnet format."""
        self.logger.error("Converting packet files is not supported.")
        #return self._connection.ConvertPktFile(source, destination, format)
        return False

    def create_capture(self, template):
        """Create a new Capture from a
        :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`
        object.
        
        Args:
            template(str or
            :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`
            ): the capture template.

        Returns:
            A :class:`Capture <omniscript.capture.Capture>` 
            object of the new capture or None.
        """
        xml = None
        if isinstance(template, basestring):
            xml = template
        elif isinstance(template, CaptureTemplate):
            xml = template.to_xml(self, True)
        request = r'<request>' + xml + r'</request>'
        response = self._issue_xml_string_result(config.OMNI_CREATE_CAPTURE, request)
        prop = omniscript._parse_command_response(response, 'prop')
        if prop is None:
            return None
        return self.find_capture(prop.text, 'id')

    def create_forensic_search(self, template):
        """Create a new Forensic Search from a
        :class:`ForensicTemplate 
        <omniscript.forensictemplate.ForensicTemplate>`
        object.

        Args:
            template(str or
            :class:`ForensicTemplate 
            <omniscript.forensictemplate.ForensicTemplate>`
            ): the settings of the search.

        Returns:
            A :class:`ForensicSearch 
            <omniscript.forensicsearch.ForensicSearch>`
            object or None."""
        criteria = None
        if isinstance(template, basestring):
            criteria = template
        elif isinstance(template, ForensicTemplate):
            criteria = template.to_xml(self)
        response = self._issue_xml_command(config.OMNI_CREATE_FILE_VIEW, criteria)
        id = omniscript._parse_command_response(response, 'ID')
        return self.find_forensic_search(id.text, 'id')

    def delete_adapter(self, adapter):
        """Delete an Adapter from the OmniEngine.

        Args:
            adapter (str,
            :class:`OmniId <omniscript.omniid.OmniId>` or
            :class:`Capture <omniscript.adapter.Adapter>`
            ): the adapter's id or an Adapter object.
            Or a list of adapters.
        """
        eal = self.get_adapter_list()
        itemlist = []
        adapterlist = adapter if isinstance(adapter, list) else [adapter]
        for a in adapterlist:
            if isinstance(a, basestring):
                ao = find_adapter(eal, a)
                if ao is not None:
                    itemlist.append(a)
            elif isinstance(a, Adapter):
                itemlist.append(a)
            else:
                raise TypeError("adapter must be an Adapter name or an Adapter object.")
        if len(adapterlist) == 0:
            return None
        da = ET.Element('delete-adapters')
        ads = ET.SubElement(da, 'adapters')
        for i in itemlist:
            item = ET.SubElement(ads, 'adapter')
            ET.SubElement(item, 'adapter-type').text = str(i.adapter_type)
            ET.SubElement(item, 'adapter-id').text = str(i.id)
        xml = ET.tostring(da).replace('\n', '')
        return self._issue_xml_command(config.OMNI_DELETE_ADAPTERS, xml)

    def delete_all_capture_sessions(self):
        """ Delete all the Capture Sessions from the engine.

        Note that 'Capture Sessions' are different from Captures.
        See the Details tab at the bottom of an OmniEngine's Forensics tab.
        """
        return self._issue_xml_command(config.OMNI_DELETE_ALL_CAPTURE_SESSIONS, None)

    def delete_capture(self, capture, retry=3):
        """Delete a Capture from the OmniEngine.

        Args:
            capture (str,
            :class:`OmniId <omniscript.omniid.OmniId>` or
            :class:`Capture <omniscript.capture.Capture>`
            ): the capture's id or a Capture object.
            Or a list of captures.
        """
        cl = _xml_capture_list(capture)
        if len(cl) > 0:
            xml = \
                "<delete-captures><captures>%s</captures></delete-captures>" \
                % cl
            if retry < 0:
                retry = 0
            for _ in range(0, (retry + 1)):
                try:
                    response = self._issue_xml_command(config.OMNI_DELETE_CAPTURES, xml)
                    omniscript._parse_command_response(response)
                except omniscript.OmniError:
                    pass
                else:
                    break

    def delete_capture_session(self, session):
        """Delete a Capture Sessionfrom the OmniEngine.

        Args:
            session (int, str,
            :class:`CaptureSession <omniscript.capturesession.CaptureSession>`
            ): the session's id or a CaptureSession object.
            Or a list of sessions.
        """
        cl = _xml_capture_session_list(session)
        if len(cl) > 0:
            xml = "<delete-session><CaptureSessions>%s</CaptureSessions></delete-session>" % cl
            response = self._issue_xml_command(config.OMNI_DELETE_CAPTURE_SESSION, xml)
            omniscript._parse_command_response(response)

    def delete_file(self, files):
        """Delete files from the OmniEngine.

        Returns:
            True on success, otherwise False.
        """
        xml = "<delete-files><files>"
        if isinstance(files, basestring):
            xml += "<file name='%s' />" % files
        elif isinstance(files, ForensicFile):
            xml += "<file name='%s' />" % files.path
        elif isinstance(files, FileInformation):
            xml += "<file name='%s' />" % files.name
        elif isinstance(files, list):
            if len(files) == 0:
                return True
            for f in files:
                if isinstance(f, basestring):
                    xml += "<file name='%s' />" % f
                elif isinstance(f, ForensicFile):
                    xml += "<file name='%s' />" % f.path
                elif isinstance(f, FileInformation):
                    xml += "<file name='%s' />" % f.name
        else:
            return False
        xml += "</files></delete-files>"
        response = self._issue_xml_command(config.OMNI_DELETE_FILES, xml)
        omniscript._parse_command_response(response)
        
    def delete_filter(self, omnifilter):
        """Delete one or more filters from the OmniEngine's filter set.

        Args:
            omnifilter (str,
            :class:`OmniId <omniscript.omniid.OmniId>` or
            :class:`Filter <omniscript.filter.Filter>`
            ): the id of the filter or a Filter object.
        """
        class_name_ids = omniscript.get_class_name_ids()
        itemlist = []
        filterlist = omnifilter if isinstance(omnifilter, list) else [omnifilter]
        for f in filterlist:
            if isinstance(f, basestring):
                itemlist.append(Filter(criteria=f))
            elif isinstance(f, Filter):
                itemlist.append(f)
            else:
                raise TypeError("omnifilter must be or contain a Filter.")
        if len(filterlist) == 0:
            return None

        mf = ET.Element('modify-filters')
        mi = ET.SubElement(mf, 'mutable-items', {'clsid':str(class_name_ids['Filter'])})
        for i in itemlist:
            item = ET.SubElement(mi, 'item', {'type':str(FILTER_MODIFY_DELETE)})
            i._store(item)
        xml = ET.tostring(mf).replace('\n', '')
        return self._issue_xml_command(config.OMNI_MODIFY_FILTERS, xml)

    def delete_forensic_search(self, search):
        """Delete a Forensic Search from the OmniEngine.
        
        Args:
            search (str,
            :class:`OmniId <omniscript.omniid.OmniId>`, or
            :class:`ForensicSearch <omniscript.forensicsearch.ForensicSearch>`
            ): the id of the Forensic Search or a ForensicSearch object.
        """
        xml = "<delete-file-view>"
        if isinstance(search, basestring):
            xml += "<ID>"+ search + "</ID>"
        elif isinstance(search, OmniId):
            xml += "<ID>"+ str(search) + "</ID>"
        elif isinstance(search, ForensicSearch):
            xml += "<ID>"+ str(search.id) + "</ID>"
        elif isinstance(search, list):
            if len(search) == 0:
                return True
            for s in search:
                if isinstance(s, basestring):
                    xml += "<ID>"+ s + "</ID>"
                elif isinstance(s, OmniId):
                    xml += "<ID>"+ str(s) + "</ID>"
                elif isinstance(s, ForensicSearch):
                    xml += "<ID>"+ str(s.id) + "</ID>"
        else:
            return False
        xml += "</delete-file-view>"
        return self._issue_xml_command(config.OMNI_DELETE_FILE_VIEW, xml)

    def disconnect(self):
        """Disconnect from the OmniEngine"""
        self._issue_command(config.OMNI_DISCONNECT, None, 0)

    def find_adapter(self, value, attrib = find_attributes[0]):
        """Find an :class:`Adapter <omniscript.adapter.Adapter>`
        in the OmniEngine's list of adapters.

        Args:
            value (str or :class:`Adapter <omniscript.adapter.Adapter>`
            ): the search key.
            attrib ('name' or 'id'): what attribute to search on.

        Returns:
            An :class:`Adapter <omniscript.adapter.Adapter>`
            object of the adapter.

        Note:
            If value is an :class:`Adapter <omniscript.adapter.Adapter>`,
            then the search is performed on the Adapter's id.
        """
        adapters = self.get_adapter_list()
        return find_adapter(adapters, value, attrib)

    def find_capture(self, value, attrib = find_attributes[0]):
        """Find a :class:`Capture <omniscript.capture.Capture>`
        in the OmniEngine's list of captures.

        Args:
            value (str or :class:`Capture <omniscript.capture.Capture>`
            ): the search key.
            attrib ('name' or 'id'): what attribute to search on.

        Returns:
            A :class:`Capture <omniscript.capture.Capture>` object of the
            capture or None.

        Note:
            If value is a :class:`Capture <omniscript.capture.Capture>`
            , then the search is performed on the Capture's id.
        """
        captures = self.get_capture_list()
        return find_capture(captures, value, attrib)

    def find_filter(self, value, attrib = find_attributes[0]):
        """Find a :class:`Filter <omniscript.filter.Filter>`
        in the OmniEngine's filter set.

        Args:
            value (str or :class:`Filter <omniscript.filter.Filter>`
            ): the search key.
            attrib ('name' or 'id'): what attribute to search on.

        Returns:
            A :class:`Filter <omniscript.filter.Filter>` object of the filter
            or None.
        """
        filters = self.get_filter_list()
        return find_filter(filters, value, attrib)

    def find_forensic_search(self, value, attrib = find_attributes[0]):
        """Find a :class:`ForensicSearch 
        <omniscript.forensicsearch.ForensicSearch>` in the OmniEngine's list 
        of forensic searches.

        Args:
            value (str,
            :class:`ForensicSearch <omniscript.forensicsearch.ForensicSearch>`
            ): the search key.
            attrib ('name' or 'id'): what attribute to search on.

        Returns:
            A :class:`ForensicSearch <omniscript.forensicsearch.ForensicSearch>`
            object of the forensic search or None.
        """
        searches = self.get_forensic_search_list()
        return find_forensic_search(searches, value, attrib)

    def get_access_control_list(self):
        """Get the Access Control List.

        Returns a :class:`AccessControlList <omniscript.accesscontrollist.AccessControlList>`
        object.
        """
        response = self._issue_xml_command(config.OMNI_GET_ACL, None)
        return AccessControlList(self, response)

    def get_adapter_configuration(self, adapter, adapter_type = None):
        """Get the configuration of an adapter.
        
        Args:
            adapter (Adapter object or string).
            adapter_type (string), required if adapter is a string.

        Returns:
            A
            :class:`AdapterConfiguration <omniscript.adapterconfiguration.AdapterConfiguration>`.
        """
        _id = None
        _type = None
        if isinstance(adapter, Adapter):
            _id = adapter.id
            _type = adapter.adapter_type
        elif isinstance(adapter, basestring):
            _id = adapter
            _type = adapter_type
        request = '<request>' + \
                    '<adapter-id>' + str(_id) + '</adapter-id>' + \
                    '<adapter-type>' + str(_type) + '</adapter-type>' + \
                  '</request>'
        response = self._issue_xml_command(config.OMNI_GET_ADAPTER_CONFIG, request)
        msg = omniscript._parse_command_response(response, 'msg')
        return AdapterConfiguration(msg)

    def get_adapter_list(self):
        """Get the OmniEngine's list of
        :class:`Adapter <omniscript.adapter.Adapter>`.
        
        Returns:
            A list of :class:`Adapter <omniscript.adapter.Adapter>` objects.
        """
        response = self._issue_xml_command(config.OMNI_GET_ADAPTER_LIST, None)
        return _create_adapter_list(self, response)

    def get_alarm_list(self):
        """Get the OmniEngine's list of alarms.
        
        Returns:
            A list of :class:`Alarm <omniscript.alarm.Alarm>` objects.
        """
        response = self._issue_xml_command(config.OMNI_GET_ALARMS, None)
        return _create_alarm_list(response)

    def get_analysis_module_list(self):
        """Get the OmniEngine's list of Analysis Modules.

        Returns:
            A list of :class:`AnalysisModule <omniscript.analysismodule.AnalysisModule>` objects.
        """
        response = self._issue_xml_command(config.OMNI_GET_INSTALLED_PLUGINS, None)
        return _create_analysis_module_list(self, response)

    def get_audit_log(self, first, count):
        """Get the OmniEngine's Audit Log.

        Returns:
            A :class:`AuditLog <omniscript.auditlog.AuditLog>` object.
        """
        request = '<request><properties>' + \
                    '<prop name="Limit" type="19">%d</prop>' % (count) + \
                    '<prop name="Offset" type="19">%d</prop>' % (first) + \
                '</properties></request>'
        response = self._issue_xml_command(config.OMNI_GET_AUDIT_LOG, request)
        counts = omniscript._parse_command_response(response, 'counts')
        entrys = omniscript._parse_command_response(response, 'messages')
        return AuditLog(self, counts, entrys)

    def get_capture_list(self):
        """Get the OmniEngine's list of
        :class:`Capture <omniscript.capture.Capture>`.
        
        Returns:
            A list of :class:`Capture <omniscript.capture.Capture>` objects.
        """
        response = self._issue_xml_command(config.OMNI_GET_CAPTURE_LIST, None)
        return _create_capture_list(self, response)

    def get_capture_session_list(self):
        """Get the OmniEngine's list of
        :class:`CaptureSession <omniscript.capturesession.CaptureSession>`.
        
        Returns:
            A list of :class:`CaptureSession <omniscript.capturesession.CaptureSession>` objects.
        """
        datatable = OmniDataTable()
        response = self._issue_command(config.OMNI_GET_CAPTURE_SESSIONS, None, 0)
        datatable.load(response)
        return _create_capture_session_list(datatable)

    def get_capture_template(self, capt):
        """Get the Capture Template of an existing capture.

        Returns:
            A :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`
            object.
        """
        if isinstance(capt, Capture):
            id = str(capt.id)
        else:
            id = str(capt)
        request = '<request><prop name="id" type="8">%s</prop></request>' % id
        response = self._issue_xml_command(config.OMNI_GET_CAPTURE_OPTIONS, request)
        props = omniscript._parse_command_response(response, 'properties')
        return CaptureTemplate(node=props, engine=self)

    def get_diagnostics_info(self, info_type):
        """Get diagnostic information from a Linux OmniEngine.

        :param str type: The type of information to retrieve.
        :return: The diagnostic information.
        :rtype: str

        Args:
            type (string): type of information: default, raid, drives, processes
            or messages.

        Returns:
            The diagnostic information as a string.
        """
        arg = ''
        if len(info_type) > 0:
            arg = '-' + info_type
        xml = '<request><Parameters>' + arg + '</Parameters></request>'
        response = self._issue_xml_command(config.OMNI_GET_DIAGNOSTICS_INFO, xml)
        txt = ''.join(response)
        omniscript._parse_command_response(txt[txt.find('<?xml'):])
        result = txt[:txt.find('<?xml')].rstrip('\n\0')
        return result.split('\n')
    
    def get_event_log(self, first, count, capture=None):
        """Get the OmniEngine's Event Log.

        Args:
            first (int): the index of the first entry to retrieve.
            count (int): the number of entries to retrieve.
            capture (OmniId, str, Capture, CaptureSession): Get entries for just this 'capture'.

        Returns:
            A :class:`EventLog <omniscript.eventlog.EventLog>` object.
        """
        if isinstance(capture, OmniId):
            _id = capture
        elif isinstance(capture, basestring):
            _id = OmniId(capture)
        elif isinstance(capture, Capture):
            _id = capture.id
        elif isinstance(capture, CaptureSession):
            _id = capture.capture_id
        else:
            _id = None

        request = ET.Element('request')
        props = ET.SubElement(request, 'properties')
        ET.SubElement(props, "prop", {"name": "Informational", "type": "11"}).text = "-1"
        ET.SubElement(props, "prop", {"name": "Limit", "type": "19"}).text = str(count)
        ET.SubElement(props, "prop", {"name": "Major", "type": "11"}).text = "-1"
        ET.SubElement(props, "prop", {"name": "Messages", "type": "11"}).text = "-1"
        ET.SubElement(props, "prop", {"name": "Minor", "type": "11"}).text = "-1"
        ET.SubElement(props, "prop", {"name": "Offset", "type": "19"}).text = str(first)
        ET.SubElement(props, "prop", {"name": "Severe", "type": "11"}).text = "-1"
        if _id:
            ET.SubElement(props, "prop", {"name": "ContextID", "type": "8"}).text = str(_id)
        xml = ET.tostring(request).replace('\n', '')
        response = self._issue_xml_command(config.OMNI_GET_LOG_MSGS, xml)
        counts = omniscript._parse_command_response(response, 'counts')
        entrys = omniscript._parse_command_response(response, 'messages')
        return EventLog(self, counts, entrys)

    def get_file(self, source, destination=None, chunk_size=(5*1024*1024)):
        """Get a file from the OmniEngine.
        
        Args:
            source (str): name of the file to get. If not fully qualified
                          then source will be relative to the engine's
                          data folder.
            destination (str): the local file name of the file being
                               retrieved.
            chunk_size (int): the size of each chunk being transfered. Default
                              is 5MB.

        Returns:
            The number of bytes in the file.
        """
        if isinstance(source, ForensicFile):
            _source = source.path
        elif isinstance(source, FileInformation):
            if source.is_folder():
                raise OmniError("Source is a folder.")
            _source = source.name
        else:
            _source = source
        if destination is None or len(destination) == 0:
            destination = os.path.basename(_source)
        chunk_size = int(chunk_size)
        start = 0
        end = chunk_size
        result = 0
        with open(destination, 'wb') as fle:
            while result == 0:
                fi = ET.Element('get-file')
                ET.SubElement(fi, 'file', {'name':_source,
                                           'start-offset':str(start),
                                           'end-offset':str(end)})
                xml = ET.tostring(fi).replace('\n', '')
                response = self._issue_xml_command(config.OMNI_GET_FILE, xml)
                result, chunk = struct.unpack_from("=II", response, 0)
                if result == 0:
                    fle.write(response[8:])
                    start += chunk
                    end += chunk
                    if chunk < chunk_size or chunk == 0:
                        break
        return start
    
    def get_file_list(self, path='', mask = "*", recursive=True, folders=False, from_data=True):
        """Get a list of files and their attributes.
        
        Args:
            path (str): The path of the file list to get Default is the Data
            Folder.
            mask (str): The wild-card mask. Default is '*'.
            recursive(bool) : If true then the files of sub-folders are
            included.
            folders(bool) : if true then the folders are included.
            from_data(bool) : if true then 'path' is under the engine' data
            folder.

        Returns:
            A list of
            :class:`FileInformation <omniscript.fileinformation.FileInformation>`
            objects.
        """
        path_type = 'relative-path' if from_data else 'absolute-path'
        gfl = ET.Element('get-file-list')
        ET.SubElement(gfl, 'file-type',
                      {'mask':mask, path_type:path,
                       'recursive':str(1 if recursive else 0),
                       'include-dirs':str(1 if folders else 0)})
        xml = ET.tostring(gfl).replace('\n', '')
        response = self._issue_xml_command(config.OMNI_GET_FILE_LIST, xml)
        files = omniscript._parse_command_response(response, 'files')
        if (files is None):
            raise OmniError('Invalid command response: response tag not found.')
        lst = [FileInformation(name=files.attrib['path'],
                               flags=config.FILE_FLAGS_FOLDER)]
        for file in files:
            lst.append(FileInformation(elem=file))
        return lst
    
    def get_filter_list(self, refresh=True):
        """Get the OmniEngine's :class:`Filter <omniscript.filter.Filter>`
        set.
        
        Args:
            refresh(bool): If True will force a refresh, else refresh if the
                           timeout has expired.
        Returns:
            A list of :class:`Filter <omniscript.filter.Filter>` objects.
        """
        do_refresh = refresh
        if not refresh and self._filter_list:
            delta_time = PeekTime() - self._filter_list[1]
            do_refresh = delta_time.value > self._filter_timeout
        if not self._filter_list or do_refresh:
            response = self._issue_xml_command(config.OMNI_GET_FILTERS, None)
            filter_list = _create_filter_list(response)
            self._filter_list = (filter_list, PeekTime())
        return self._filter_list[0]

    def get_forensic_file_list(self):
        """Get the OmniEngine's list of files used for Forensic Searches.
        
        Returns:
            A list of
            :class:`ForensicFile <omniscript.forensicfile.ForensicFile>`
            objects.
        """
        datatable = OmniDataTable()
        # TODO: replace with issue_command.
        response = self._issue_xml_command(config.OMNI_GET_PACKETFILE_TABLE, None)
        datatable.load(response)
        return _create_forensic_file_list(datatable)

    def get_forensic_search_list(self):
        """Get the OmniEngine's list of Forensic Searches.
        
        Returns:
            A list of
            :class:`ForensicSearch <omniscript.forensicsearch.ForensicSearch>`
            objects.
        """
        response = self._issue_xml_string_result(config.OMNI_GET_FILE_VIEW_LIST, None)
        return _create_forensic_search_list(self, response)

    def get_graph_template_list(self):
        """Get the OmniEngine's list of
        :class:`GraphTemplate <omniscript.graphtemplate.GraphTemplate>`.
        
        Returns:
            A list of :class:`GraphTemplate <omniscript.graphtemplate.GraphTemplate>`
            objects.
        """
        response = self._issue_xml_command(config.OMNI_GET_GRAPH_TEMPLATES, None)
        return _create_graph_template_list(response)

    def get_hardware_options_list(self):
        """Returns the list of
        :class:`HardwareOptions <omniscript.hardwareoptions.HardwareOptions>`
        of the engine.
        """

        xml = '<request><ids/></request>'
        response = self._issue_xml_command(config.OMNI_GET_HARDWARE_OPTIONS, xml)
        return _create_hardware_options_list(self, response)

    def get_host_platform(self):
        """Returns the
        :class:`HostPlatform <omniscript.hostplatform.HostPlatform>`
        of the engine.
        """

        return self._platform

    def get_status(self):
        """Get the OmniEngine's current status.
        
        Returns:
            The OmniEngine's status as an
            :class:`EngineStatus <omniscript.enginestatus.EngineStatus>`
            object.
        """
        response = self._issue_xml_command(config.OMNI_GET_STATUS, None)
        props = omniscript._parse_command_response(response, 'properties')
        self._last_status = EngineStatus(self, self.logger, props)
        return self._last_status

    def get_version(self):
        """Get the OmniEngine's version string.
        
        Returns:
            The OmniEngine's version as a string.
        """
        data = self._issue_xml_command(config.OMNI_GET_VERSION, None)
        return ctypes.string_at(data)

    def is_connected(self):
        """Get the connection status of the OmniEngine object.
        
        Returns:
            True if OmniEngine object is connected to an OmniEngine,
            otherwise False.
        """
        return self._omni._is_connected(self._connection)

    def modify_capture(self, template, capture=None):
        """Update the settings of a Capture.

        Args:
            template a
            :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`.
            capture (
            :class:`Capture <omniscript.capture.Capture>`
            ): the id of the capture as a string or
            :class:`OmniId <omniscript.omniid.OmniId>`
            or a Capture object. If None the Id of the template is used.

        Returns:
            The updated Capture object or None if not found.
        """
        if not isinstance(template, CaptureTemplate):
            return None
        _template = template
        if isinstance(capture, basestring):
            _template.general.id = OmniId(capture)
        elif isinstance(capture, OmniId):
            _template.general.id = capture
        elif isinstance(capture, Capture):
            _template.general.id = capture.id
        if not _template.general.id:
            raise TypeError("The template or the capture must be or contain a GUID.")
        xml = _template.to_xml(self, False, False, True)
        request = r'<request>' + xml + r'</request>'
        response = self._issue_xml_string_result(config.OMNI_SET_CAPTURE_OPTIONS, request)
        prop = omniscript._parse_command_response(response, 'prop')
        if prop is None:
            return None
        return self.find_capture(prop.text, 'id')

    def rename_adapter(self, adapter_type, id, new_name):
        """Rename an Adapter

        Args:
            adapter_type (int): the type of adapter: Plugin=3.
            id (str): the adapter's identifier
            new_name(str): the new name for the adapter.
        """
        ra = ET.Element('rename-adapter')
        ET.SubElement(ra, 'adapter-type').text = str(adapter_type)
        ET.SubElement(ra, 'adapter-id').text = str(id)
        ET.SubElement(ra, 'adapter-desc').text = new_name
        xml = ET.tostring(ra).replace('\n', '')
        return self._issue_xml_command(config.OMNI_SET_ADAPTER_NAME, xml)

    def restart(self, seconds_delay=1):
        """Restart the engine. The engine will be disconnected.
        OmniScript needs time to close the connection to the engine.
        The seconds_delay allows for a longer delay. Setting the delay to
        zero seconds may cause a false positive when
        :func:`is_connected() <omniscript.omniscript.OmniEngine.is_connected>`
        is called.
        """
        response = self._issue_command(config.OMNI_RESTART_ENGINE, None, 0)
        try:
            omniscript._parse_command_response(response)
        except:
            pass
        time.sleep(seconds_delay)
        self.disconnect()

    def reset_capture(self, capture):
        """Reset a Capture, or list of Captures, to reset all the statistics and
        delete all the packets.

        Args:
            capture (str or
            :class:`Capture <omniscript.capture.Capture>`
            ): the id of the capture as a string or
            :class:`OmniId <omniscript.omniid.OmniId>`
            or a Capture object.

        Note:
            The Capture objects will need to be refreshed.
        """
        cl = _xml_capture_list(capture)
        if len(cl) > 0:
            xml = "<request><captures>%s</captures></request>" % cl
            return self._issue_xml_command(config.OMNI_CLEAR_CAPTURES, xml)
        return None

    def save_all_packets(self, obj, filename):
        """Save all the packets of a Capture for Forensic Search to file in the engine's Data Folder.
        """
        if isinstance(obj, basestring):
            object_id = str(OmniId(obj))
        elif isinstance(obj, OmniId):
            object_id = str(obj)
        elif isinstance(obj, (Capture, ForensicSearch)):
            object_id = str(obj.id)
        else:
            raise TypeError("obj must be or contain a GUID.")

        sc = ET.Element('save-all-packets')
        ET.SubElement(sc, 'attributes', {'capture-id':object_id,
                                         'filename':filename})
        xml = ET.tostring(sc).replace('\n', '')
        return self._issue_xml_command(config.OMNI_SAVE_CAPTURE_FILE, xml)

    def send_file(self, source, destination=None, chunk_size=(5*1024*1024)):
        """Send a file to the engine.

        Args:
            source (str): the name of the local file to send.
            destination (str): the path, relative to the engine's Data
            Folder, and name of the file.
            chunk_size(int): the size of each chunk of the transfer. Default is
            5MB.
        """
        if destination is None or len(destination) == 0:
            destination = os.path.basename(source)
        stream_format = 1
        operation = WRITE_OP_CREATE
        with open(source, 'rb') as fle:
            fle.seek(0, os.SEEK_END)
            file_size = fle.tell()
            fle.seek(0, os.SEEK_SET)

            #Send the Create operation.
            name = destination
            name_len = len(destination)
            # format: '=' native byte order, no padding, 'I' unsigned int,
            # 'Q' unsigned 64-bit int, 's' char[] with the number of 
            # characters preceding it: 'Hello World' is 11s
            buf = struct.pack('=III%ssQ' % name_len, stream_format, operation,
                              name_len, name, file_size)
            response = self._issue_command(config.OMNI_SET_FILE, buf, len(buf))
            if len(response) == 8:
                _, file_id = struct.unpack("II", response[:8])
            else:
                # err = omniscript.string_from_unicode(response)
                omniscript._parse_command_response(response)
                return

            #Send the Write operations.
            operation = WRITE_OP_WRITE
            chunk_size = int(chunk_size)
            pos = fle.tell()
            while pos < file_size:
                chunk = min(chunk_size, (file_size - pos))
                data = fle.read(chunk)
                buf = struct.pack("=IIIQQ%ss" % chunk, stream_format,
                                  operation, file_id, pos, chunk, data)
                #Response is an empty string.
                self._issue_command(config.OMNI_SET_FILE, buf, len(buf))
                pos = fle.tell()

            #Send the Close file operation
            operation = WRITE_OP_CLOSE
            buf = struct.pack('=III', stream_format, operation, file_id)
            #Response is an empty string.
            self._issue_command(config.OMNI_SET_FILE, buf, len(buf))

    def send_plugin_message(self, plugin_id, capture_id, message):
        if not plugin_id:
            plugin_id = OmniId()
        if not capture_id:
            capture_id = OmniId()

        if isinstance(message, str):
            bs = list(bytearray(message.encode('utf-16')))
            msg = "".join(map(chr, bs[2:]))
            msg_len = len(msg) * 2
        else:
            msg = message
            msg_len = len(message) * 2

        # buf_len = (4 + 16 + 16 + 4 + 4 + 8 + 8) + msg_len
        # Plugin Message Header: 44 bytes
        # Framework Peek Message Header: 16 bytes (text length, binary length)
        buf = struct.pack('=I16s16sIiQQ%ss' % msg_len,
                          44, capture_id.bytes_le(), plugin_id.bytes_le(),
                          (msg_len + 16), 0, msg_len, 0, msg)
        return self._issue_command(config.OMNI_PLUGIN_MESSAGE, buf, len(buf))

    def set_adapter_configuration(self, adapter_config):
        """Set the configuration of an adapter."""
        request = ET.Element('request')
        adapter_config._store(request)

        xml = ET.tostring(request).replace('\n', '')
        response = self._issue_xml_command(config.OMNI_SET_ADAPTER_CONFIG, xml)
        if len(response) > 0:
            omniscript._parse_command_response(response)

    def set_access_control_list(self, acl, merge=False):
        """Set the engine's Access Control List to the ACL provided.
        If the merge paramter is True, then the provided ACL is merged with the 
        existing ACL. Otherwise the provided ACL will replace the existing ACL.
        """
        request = ET.Element('request')
        set_acl = ET.SubElement(request, 'set-acl',{'merge':['0','1'][merge]})
        acl.store(set_acl)

        xml = ET.tostring(request).replace('\n', '')
        response = self._issue_xml_command(config.OMNI_SET_ACL, xml)
        if len(response) > 0:
            omniscript._parse_command_response(response)

    def set_filter_list_timeout(self, timeout):
        """Set the Filter List refresh timeout in seconds."""
        self._filter_timeout = int(timeout) * 1000000000 # convert to nanoseconds.
         
    def start_capture(self, capture):
        """Signal a Capture, or list of Captures, to begin capturing packets.

        Args:
            capture (str or
            :class:`Capture <omniscript.capture.Capture>`
            ): the id of the capture as a string or
            :class:`OmniId <omniscript.omniid.OmniId>`
            or a Capture object.

        Note:
            The status of the capture will not change if the capture is
            already capturing packets.
        """
        cl = _xml_capture_list(capture)
        if len(cl) > 0:
            xml = "<request><captures>%s</captures></request>" % cl
            return self._issue_xml_command(config.OMNI_START_CAPTURES, xml)
   
    def stop_capture(self, capture):
        """Signal a Capture, or list of Captures, to stop capturing packets.

        Args:
            capture (str or
            :class:`Capture <omniscript.capture.Capture>`
            ): the id of the capture as a string or
            :class:`OmniId <omniscript.omniid.OmniId>`
            or a Capture object.

        Note:
            The status of the capture will not change if the capture is
            already idle.
        """
        cl = _xml_capture_list(capture)
        if len(cl) > 0:
            xml = "<request><captures>%s</captures></request>" % cl
            return self._issue_xml_command(config.OMNI_STOP_CAPTURES, xml)

    def set_hardware_options(self, options):
        """Set the hardware options."""
        request = ET.Element('request')
        _store_hardware_options_list(request, options)

        xml = ET.tostring(request).replace('\n', '')
        response = self._issue_xml_command(config.OMNI_SET_HARDWARE_OPTIONS, xml)
        if len(response) > 0:
            omniscript._parse_command_response(response)

    def sync_forensic_database(self):
        """Synchronize the OmniEngine's forensic database with the Packet
        Files on the system.
        """
        self._issue_xml_command(config.OMNI_SYNC_DATABASE, None)

    def update_filter(self, omnifilter):
        """Update one or more existing filters in the engine's filter set.

        Args:
            omnifilter (str, Filter or list): the filter to update.
        """
        class_name_ids = omniscript.get_class_name_ids()
        criterialist = []
        filterlist = omnifilter if isinstance(omnifilter, list) else [omnifilter]
        for f in filterlist:
            if isinstance(f, basestring):
                criterialist.append(Filter(criteria=f))
            elif isinstance(f, Filter):
                criterialist.append(f)
            else:
                raise TypeError("omnifilter must be or contain a Filter.")
        if len(filterlist) == 0:
            return None

        mf = ET.Element('modify-filters')
        mi = ET.SubElement(mf, 'mutable-items',
                           {'clsid':str(class_name_ids['Filter'])})
        for criteria in criterialist:
            item = ET.SubElement(mi, 'item', {'type':str(FILTER_MODIFY_MODIFY)})
            criteria._store(item)
        xml = ET.tostring(mf).replace('\n', '')
        response = self._issue_xml_command(config.OMNI_MODIFY_FILTERS, xml)
        prop = omniscript._parse_command_response(response, 'prop')
        if prop is None:
            return None
        return self.find_filter(prop.text, 'id')
