"""OmniScript class.
"""
#Copyright (c) Savvius, Inc. 2013-2018. All rights reserved.

# " ".join(['%02X' % ord(b) for b in data])

# pylint: disable-msg=w0614

VERSION = "2.0"
MIN_PYTHON = "2.6"

import os
import sys
import logging
import ctypes
import inspect
import socket
import struct
import xml.etree.ElementTree as ET

from config import *
from omniaddress import *
from savestatistics import *

from omniengine import OmniEngine
from accesscontrollist import AccessControlList, AccessControlPolicy, AccessControlUser
from adapter import Adapter
from adapterconfiguration import AdapterConfiguration
from alarm import Alarm
from analysismodule import AnalysisModule
from applicationstatistic import ApplicationStatistic
from auditlog import AuditLog, AuditLogEntry
from callstatistic import CallStatistic
from capture import Capture
from capturesession import CaptureSession
from capturetemplate import CaptureTemplate
from countrystatistic import CountryStatistic
from decodedpacket import DecodedPacket
from diagnostic import Diagnostic
from enginestatus import EngineStatus
from eventlog import EventLog, EventLogEntry
from expertquery import ExpertQuery
from expertresult import ExpertResult
from fileadapter import FileAdapter
from filter import Filter
from filternode import FilterNode
from forensicfile import ForensicFile
from forensicsearch import ForensicSearch
from forensictemplate import ForensicTemplate
from graphtemplate import GraphTemplate
from hardwareoptions import HardwareOptions, WirelessHardwareOptions
from hostplatform import HostPlatform
from mediainfo import MediaInfo
from mediaspec import MediaSpec
from nodestatistic import NodeStatistic
from omnidatatable import OmniDataTable
from omnierror import OmniError
from omniid import OmniId
from omniport import OmniPort
from packet import Packet
from peektime import PeekTime
from protocolstatistic import ProtocolStatistic
from readstream import ReadStream
from statscontext import StatsContext
from statslimit import StatsLimit
from summarystatistic import SummaryStatistic
from tcpdump import TCPDump, TCPDumpTemplate
from hardwareoptions import HardwareOptions, WirelessHardwareOptions

from adapter import find_adapter
from alarm import find_alarm
from analysismodule import find_analysis_module
from capture import find_capture
from capturetemplate import get_graph_template_names
from filter import find_filter
from filter import read_filter_file
from forensicsearch import find_forensic_search
from graphtemplate import find_graph_template
from hardwareoptions import find_hardware_options
from omniaddress import parse_ip_address
from protocolstatistic import flatten
from hardwareoptions import find_hardware_options


"""OmniApi specific OmniEngine Command Ids"""
OMNI_CONNECT = 1
OMNI_DISCONNECT = 2
OMNI_IS_CONNECTED = 3

# Dictionary of OmniId (GUID) to class name.
_id_class_names = None

# Dictionary of class name to OmniId (GUID).
_class_name_ids = None

# Dictionary of country codes to names.
_co_code_names = None

# Dictionary of country names to codes.
_co_name_codes = None

# Dictionary of OmniId (GUID) to expert description.
_id_expert_names = None

# Dictionary of OmniId (GUID) to statistics and other names.
# Names are not unique, so a reverse dictionary cannont be built.
_id_stat_names = None

# Dictionary of ProtoSpec Ids and protocol names and short names.
_id_protocol_names = None
_id_protocol_short_names = None

# Dictionary of short names to ProtoSpec Ids.
_protocol_short_name_ids = None

# Dictionary of OmniId (GUID) to graph names.
_id_graph_names = {}

# 0-Unknonw, 1-Windows, 2-Linux
_auth_values = ['Default', 'Default', 'ThirdPartyAuth']

# Dictionary of wireless band names.
_wireless_band_id_names = {
        config.WIRELESS_BAND_ALL: 'All',
        config.WIRELESS_BAND_GENERIC: 'Generic',
        config.WIRELESS_BAND_B: 'b',
        config.WIRELESS_BAND_A: 'a',
        config.WIRELESS_BAND_G: 'g',
        config.WIRELESS_BAND_N: 'n',
        config.WIRELESS_BAND_TURBOA: 'Turbo-a',
        config.WIRELESS_BAND_TURBOG: 'Turbo-g',
        config.WIRELESS_BAND_SUPERG: 'Super-g',
        config.WIRELESS_BAND_LICENSEDA1MHZ: 'Licensed a 1 MHz',
        config.WIRELESS_BAND_LICENSEDA5MHZ: 'Licensed a 5 MHz',
        config.WIRELESS_BAND_LICENSEDA10MHZ: 'Licensed a 10 MHz',
        config.WIRELESS_BAND_LICENSEDA15MHZ: 'Licensed a 15 MHz',
        config.WIRELESS_BAND_LICENSEDA20MHZ: 'Licensed a 20 MHz',
        config.WIRELESS_BAND_PRIMARYAC0: 'Primary ac 0',
        config.WIRELESS_BAND_PRIMARYAC1: 'Primary ac 1',
        config.WIRELESS_BAND_PRIMARYAC2: 'Primary ac 2',
        config.WIRELESS_BAND_PRIMARYAC3: 'Primary ac 3',
        config.WIRELESS_BAND_UNKNOWN5: 'Unknown 5',
        config.WIRELESS_BAND_UNKNOWN6: 'Unknown 6',
        config.WIRELESS_BAND_UNKNOWN7: 'Unknown 7',
        config.WIRELESS_BAND_UNKNOWN8: 'Unknown 8',
        config.WIRELESS_BAND_UNKNOWN9: 'Unknown 9',
        config.WIRELESS_BAND_N20MHZ: 'n 20 MHz',
        config.WIRELESS_BAND_N40MHZ: 'n 40 MHz',
        config.WIRELESS_BAND_N40MHZLOW: 'n 40 MHz Low',
        config.WIRELESS_BAND_N40MHZHIGH: 'n 40 MHz High'
    }


def _add_spec(spec, id_protocol_names, id_protocol_short_names):
    spec_type = spec.get('Type')
    if spec_type is not None:
        for sub_spec in spec.iter('PSpec'):
            id = sub_spec.find('PSpecID')
            if id is not None:
                name = sub_spec.attrib['Name']
                index = int(id.text)
                id_protocol_names[index] = name
                sname = sub_spec.find('SName')
                id_protocol_short_names[index] = sname.text \
                    if sname is not None else name
    else:
        id = spec.find('PSpecID')
        if id is not None:
            name = spec.attrib['Name']
            index = int(id.text)
            id_protocol_names[index] = name
            sname = spec.find('SName')
            id_protocol_short_names[index] = sname.text \
                if sname is not None else name


def _load_class_ids():
    id_class_names = {}
    _dirname = os.path.dirname(__file__)
    with open(os.path.join(_dirname, "_class_ids.txt"), 'r') as clsdct:
        for line in clsdct:
          if len(line) > 0:
              k, v = line.split(' ', 1)
              id_class_names[OmniId(k)] = v.strip()
    return id_class_names


def _load_co_code_names():
    co_code_names = {}
    co_code_names['Unknown'] = 'Unknown'
    co_code_names['Private'] = 'Private Network'
    co_code_names['Multicast'] = 'Multicast'
    _dirname = os.path.dirname(__file__)
    with open(os.path.join(_dirname, "_co_codes.txt"), 'r') as codct:
        for line in codct:
            if len(line) > 0:
              if ':' in line:
                tup = line.split(':', 1)[0]
              else:
                tup = line
              code, name = tup.strip().split('\t', 1)
              if code not in co_code_names.keys():
                co_code_names[code] = name.strip()
    return co_code_names


def _load_expert_names():
    id_expert_names = {}
    _dirname = os.path.dirname(__file__)
    experts = ET.parse(os.path.join(_dirname, "expertdescriptions.xml"))
    root = experts.getroot()
    _tag = root.tag
    namespace = ''
    if _tag[0] == '{':
        namespace = _tag[:_tag.find('}')+1]
    for expert in root.iter(namespace+'problem'):
        guid = expert.find(namespace+'guid')
        if guid is not None:
            name = expert.find(namespace+'name')
            if name is not None:
                id_expert_names[OmniId(guid.text.strip())] = name.text.strip()
    return id_expert_names


def _load_pspecs():
    id_protocol_names = {}
    id_protocol_short_names = {}
    _dirname = os.path.dirname(__file__)
    pspecs = ET.parse(os.path.join(_dirname, "pspecs.xml"))
    root = pspecs.getroot()
    for pspec in root.iter('PSpec'):
        _add_spec(pspec, id_protocol_names, id_protocol_short_names)
    return id_protocol_names, id_protocol_short_names


def _load_stat_ids():
    id_stat_names = {}
    _dirname = os.path.dirname(__file__)
    with open(os.path.join(_dirname, "_stat_ids.txt"), 'r') as statdct:
        for line in statdct:
            if len(line) > 0:
                k, v = line.split(' ', 1)
                id_stat_names[OmniId(k)] = v.strip()
    return id_stat_names


def _parse_command_response(response, tag_name=None):
    root = ET.fromstring(response)
    msg = root.find('msg')
    if msg is None:
        raise OmniError('Invalid command response: no msg tag found.')
    # int() converts hr to a long
    hr = int(msg.get('hr', '0x80004005'), 0)
    if hr & 0x80000000L:
        raise OmniError('Command failed: 0x%X' % hr, hr)
    if tag_name is None:
        return None
    if tag_name is 'msg':
        return msg
    return msg.find(tag_name)


def _get_command_response(response):
    if response is None:
        return int('0x80004005')
    root = ET.fromstring(response)
    msg = root.find('msg')
    if msg is None:
        raise OmniError('Invalid command response: no msg tag found.')
    # int() converts hr to a long
    return int(msg.get('hr', '0x80004005'), 0)


def get_class_name_ids():
    """Returns a dictionary with the key being the string name of a class
    and the value the :class:`OmniId <omniscript.omniid.OmniId>` GUID of the class.
    """
    global _class_name_ids
    if _class_name_ids is None:
        id_names = get_id_class_names()
        if id_names is not None:
            _class_name_ids = dict((v,k) for k,v in id_names.iteritems())
    return _class_name_ids


def get_country_code_names():
    """Returns a dictionary with the key being the integer country code and
    the value the string name of the country.
    """
    global _co_code_names
    if _co_code_names is None:
        _co_code_names = _load_co_code_names()
    return _co_code_names


def get_country_name_codes():
    """Returns a dictionary with the key being the string country name and
    the value the integer country code.
    """
    global _co_name_codes
    if _co_name_codes is None:
        code_names = get_country_code_names()
        if code_names is not None:
            _co_name_codes = dict((v,k) for k,v in code_names.iteritems())
    return _co_name_codes


def get_id_class_names():
    """Returns a dictionary with the key being the 
    :class:`OmniId <omniscript.omniid.OmniId>` GUID of a class
    and the value the string name of the class.
    """
    global _id_class_names
    if _id_class_names is None:
        _id_class_names = _load_class_ids()
    return _id_class_names


def get_id_expert_names():
    """Returns a dictionary with the key being the 
    :class:`OmniId <omniscript.omniid.OmniId>` GUID of a class
    and the value the string name of the expert.
    """
    global _id_expert_names
    if _id_expert_names is None:
        _id_expert_names = _load_expert_names()
    return _id_expert_names


def get_id_graph_names():
    """Returns a dictionary with the key being the
    :class:`OmniId <omniscript.omniid.OmniId>` GUID of a class
    and the value the string name of the class.
    """
    return _id_graph_names


def get_id_protocol_names():
    """Returns a dictionary with the key being the integer protospec id of
    a protocol and the value the string name of the protocol.
    """
    global _id_protocol_names
    global _id_protocol_short_names
    if _id_protocol_names is None:
        _id_protocol_names, _id_protocol_short_names = _load_pspecs()
    return _id_protocol_names


def get_id_protocol_short_names():
    """Returns a dictionary with the key being the integer protospec id of
    a protocol and the value the string short name of the protocol.
    """
    global _id_protocol_names
    global _id_protocol_short_names
    if _id_protocol_short_names is None:
        _id_protocol_names, _id_protocol_short_names = _load_pspecs()
    return _id_protocol_short_names


def get_id_stat_names():
    """Returns a dictionary with the key being the
    :class:`OmniId <omniscript.omniid.OmniId>` GUID of
    a statistic and the value the string name of the statstic.
    And the Access Control List descriptions.
    """
    global _id_stat_names
    if _id_stat_names is None:
        _id_stat_names = _load_stat_ids()
    return _id_stat_names


def get_protocol_short_name_ids():
    """Returns a dictionary with the key being the string short name of
    a protocol and the value the :class:`OmniId <omniscript.omniid.OmniId>`
    GUID of the protocol.
    """
    global _protocol_short_name_ids
    if _protocol_short_name_ids is None:
        id_names = get_id_protocol_short_names()
        _protocol_short_name_ids = dict((v,k) for k,v in id_names.iteritems())
    return _protocol_short_name_ids


def get_wireless_band_id_names():
    """Returns a dictionary with the names of the wireless bands."""
    # global _wireless_band_id_names
    return _wireless_band_id_names

def string_from_unicode(value):
    """Returns the string version of an array of Unicode bytes."""
    if (len(value) > 2) and (value[1] == '\x00'):
        u_str = ctypes.wstring_at(value)
        return u_str.encode('utf8')
    return value


def _clean_response(response):
    length = response.find('</peek>')
    if length > 0:
        length += len('</peek>')
    return response[0:length]


def propertybag_to_dictionary(props):
    """Convert a PeekDNX Property Bag into a Dictionary."""
    dct = {}
    if props is not None:
        for prop in props.findall('prop'):
            dct[prop.attrib['name']] = prop.text
    return dct


class LogFormatter(logging.Formatter):
    _all_levels = [
        logging.CRITICAL,
        logging.ERROR,
        logging.WARNING,
        logging.INFO,
        logging.DEBUG]
    levels = [logging.CRITICAL, logging.ERROR, logging.INFO, logging.DEBUG]
    width = 8

    def __repr__(self):
        return 'Logging object'

    def format(self, record):
        """ Return RCapture sytle logging string. """
        msg = record.msg % record.args
        return "%s: %s - %s" % (record.name,
                                record.levelname.ljust(self.width), msg)

    @classmethod
    def logging_level(cls, level):
        """Convert RCapture verbose level to a logging level.
        Pass through logging levels. Default is loggin.INFO.
        """
        if level >= 0 and level <= len(cls.levels):
            return cls.levels[level]
        if level in cls._all_levels:
            return level
        return cls.levels[2]


class _disable_file_system_redirection:
    if sys.platform == 'win32':
        _disable = ctypes.windll.kernel32.Wow64DisableWow64FsRedirection
        _revert = ctypes.windll.kernel32.Wow64RevertWow64FsRedirection
        def __enter__(self):
            self.old_value = ctypes.c_long()
            self.success = self._disable(ctypes.byref(self.old_value))
        def __exit__(self, type, value, traceback):
            if self.success:
                self._revert(self.old_value)


class OmniScript(object):
    """The OmniScript class is used to connect to an OmniEngine."""

    def __init__(self, verbose=logging.INFO, flags=0x00):
        """ OmniEngine Peek DNX Interface """
        # Load OmniAPI
        init_path = os.getcwd()
        install_path = os.path.dirname(
            os.path.abspath(inspect.getsourcefile(OmniScript)))
        os.chdir(install_path)
        self._omniapi = None
        if sys.platform == 'win32':
            self._oleaut = ctypes.oledll.LoadLibrary("oleaut32")
            with _disable_file_system_redirection():
                api_path = os.path.join(install_path, "omniapi.dll")
                # windll : __stdcall
                self._omniapi = ctypes.windll.LoadLibrary(api_path)
        elif sys.platform == 'linux2':
            api_path = os.path.join(install_path, "libomniapi.so")
            # cdll : __cdecl
            self._omniapi = ctypes.cdll.LoadLibrary(api_path)
        if self._omniapi is None:
            os.chdir(init_path)
            return
        # If the ProtoSpecs bin file has not been created, then pass in 0x02.
        # When 1 is passed in, CoInitialize() is called.
        # PythonWin will hang when passing if CoInitialize() is called.
        # Flags: 0x01 call CoInitialize.
        #        0x02 Create ProtoSpecs.
        #        0x04 Create Expert.
        #        0x08 Create Service Container.
        self._initialize = self._omniapi.Initialize
        self._initialize.argtypes = [ctypes.c_uint32]

        self._initialize(flags)
        os.chdir(init_path)

        self._new_omni_connection = self._omniapi.NewOmniConnection
        self._new_omni_connection.argtypes = [ctypes.c_uint64]
        self._new_omni_connection.restype = ctypes.c_uint64

        # IssueCommand and IssueXmlCommand both return strings.
        self._api_issue_command = self._omniapi.IssueCommand
        self._api_issue_command.argtypes = [
            ctypes.c_uint64,
            ctypes.c_uint32,
            ctypes.c_uint32,
            ctypes.c_void_p,
            ctypes.c_uint32,
            ctypes.POINTER(ctypes.c_int)] 
        self._api_issue_command.restype = ctypes.c_void_p

        self._api_issue_xml_command = self._omniapi.IssueXmlCommand
        self._api_issue_xml_command.argtypes = [
            ctypes.c_uint64,
            ctypes.c_uint32,
            ctypes.c_uint32,
            ctypes.c_wchar_p,
            ctypes.POINTER(ctypes.c_int)]
        self._api_issue_xml_command.restype = ctypes.c_void_p

        self._release_response = self._omniapi.ReleaseResponse
        self._release_response.argtypes = [ctypes.c_uint64, ctypes.c_void_p]

        # Logging
        self.logger = logging.getLogger('OmniScript')
        self.logger.setLevel(LogFormatter.logging_level(verbose))
        self._console_logger = logging.StreamHandler()
        self._console_logger.setLevel(LogFormatter.logging_level(verbose))
        self._console_formatter = LogFormatter()
        self._console_logger.setFormatter(self._console_formatter)
        if len(self.logger.handlers) == 0:
            self.logger.addHandler(self._console_logger)
        self._file_logger = None
        self.set_logging_level(verbose)

    def __getitem__(self, key):
        return self.__dict__[key]

    def __repr__(self):
        return '{%s}' % str(', '.join('%s : %s' % (k, repr(v)) 
                                      for (k, v) in self.__dict__.iteritems()))

    def __setitem__(self, key, item):
        self.__dict__[key] = item

    def _disconnect(self, connection):
        """Disconnect from the OmniEngine."""
        self._issue_command(connection, config.OMNI_DISCONNECT, 0, None, 0)

    def _get_platform(self, host, port):
        _host = host if host else 'localhost'
        _port = int(port) if port else OmniEngine.port
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((_host, _port))
            data = sock.recv(12)
            sock.close()
            sock = None
            if len(data) >= 12:
                stream = ReadStream(data)
                _magic = stream.read_ushort()
                _version = stream.read_ushort()
                _os = stream.read_byte()
                # _auth = stream.read_byte()
                # _param = stream.read_ushort()
                # _reserved = stream.read_uint()
                return HostPlatform(_magic, _version, _os)
            return None
        except:
            pass
        return None

    def _new_connection(self, optional):
        return self._new_omni_connection(optional)

    def _is_connected(self, connection):
        """Return True if connected to an OmniEngine."""
        response = self._issue_command(connection, config.OMNI_IS_CONNECTED,
                                       0, None, 0)
        result = _get_command_response(response)
        return (result == 0)

    def _issue_command(self, connection, command_id, timeout, request, length):
        """Issue a command to the OmniEngine.

        Returns a ctypes string buffer with the results.
        """
        if (self._api_issue_command is None) or (connection is None):
            return None
        try:
            count = ctypes.c_int32(0)
            response = self._api_issue_command(connection, command_id,
                                               timeout, request, length,
                                               ctypes.byref(count))
            if count.value > 0:
                data = ctypes.create_string_buffer(count.value)
                ctypes.memmove(data, response, count.value)
                self._release_response(connection, response)
                return data
#        except WindowsError as error:
#            # MS Visual C++ Compiler generated error.
#            # 'E' for Exception
#            # 0x6D7363 is 'msc'
#            # _api_issue_command may have return void.
#            if error.winerror == -529697949:
#                return None
        except RuntimeError:
            pass
        except:
            pass
        return None

    def _issue_xml_command(self, connection, command_id, timeout, request):
        """Issue an XML based command to the OmniEngine.

        Returns a ctypes string buffer with the results.
        """
        if (self._api_issue_xml_command is None) or (connection is None):
            return None
        try:
            count = ctypes.c_int32(0)
            response = self._api_issue_xml_command(connection, command_id,
                                                   timeout, request,
                                                   ctypes.byref(count))
            if count.value > 0:
                data = ctypes.create_string_buffer(count.value)
                ctypes.memmove(data, response, count.value)
                self._release_response(connection, response)
                return data
#        except WindowsError as error:
#            # MS Visual C++ Compiler generated error.
#            # 'E' for Exception
#            # 0x6D7363 is 'msc'
#            if error.winerror == -529697949:
#                raise WindowsError(error)
        except RuntimeError:
            pass
        except:
            pass
        return None

    def _issue_xml_string_result(self, connection, command_id, timeout, request):
        """Issue an XML based command to the OmniEngine.

        Returns:
            Response as a string.
        """
        response = self._issue_xml_command(connection, command_id, timeout, request)
        dirty = ctypes.string_at(response)
        return _clean_response(dirty)

    def connect(self,
                host = 'localhost',
                port = 6367,
                auth = 'Default',
                domain = '',
                user = '',
                password = '',
                timeout = 30000):
        """Estabilish a connection to an OmniEngine.

        Args:
            host (str): the IP Address or name of the system hosting the
            OmniEngine.
            port (int): the IP Port of the OmniEngine.
            auth (str): the authorization type: Default or Third Party.
            domain (str): the domain of the user's account.
            user (str): the name of the user's account.
            password (str): the password of the user's account.
            timeout (int): the timeout in milliseconds. Default is 30 seconds.

        Returns:
            An :class:`OmniEngine <omniscript.omniengine.OmniEngine>` object.
        """
        try:
            # OmniAPI connection
            engine = self.create_engine(host, port)
            if not engine.connect(auth, domain, user, password):
                raise OmniError('Connect failed')
        except RuntimeError:
            self.logger.error('Engine is off-line or inaccessible.')
            return None
        return engine

    def create_engine(self, host=None, port=None):
        """Create a new OmniEngine object.
        """
        platform = self._get_platform(host, port)
        return OmniEngine(self, self.logger, host, port, platform)

    def restart_engine(self, engine, seconds_delay=1):
        """Issue the restart command to the engine.
        The connection to the engine will be closed.
        The seconds_delay allows for a longer delay. Setting the delay to
        zero seconds may cause a false positive when
        :func:`is_connected() <omniscript.omniscript.OmniEngine.is_connected>`
        is called.
        """
        if engine:
            engine.restart(seconds_delay)

    def critical(self, msg, *args, **kwargs):
        """Log a critical message."""
        self.logger.critical(msg, *args, **kwargs)

    def error(self, msg, *args, **kwargs):
        """Log an error message."""
        self.logger.error(msg, *args, **kwargs)

    def info(self, msg, *args, **kwargs):
        """Log a informational message."""
        self.logger.info(msg, *args, **kwargs)

    def debug(self, msg, *args, **kwargs):
        """Log a debugging message."""
        self.logger.debug(msg, *args, **kwargs)

    def set_log_file(self, filename, mode='a'):
        """Start logging to a file."""
        self._file_logger = logging.FileHandler(filename, mode)
        self._file_logger.setLevel(self._console_logger.level)
        self._file_formatter = LogFormatter()
        self._file_logger.setFormatter(self._file_formatter)
        self.logger.addHandler(self._file_logger)

    def set_logging_level(self, verbose):
        """Set the logging level. Either an RCapture verbose level (0-3),
        or a logging level (for instance: logging.CRITICAL-logging.DEBUG).
        """
        _level = LogFormatter.logging_level(verbose)
        self.logger.setLevel(_level)
        self._console_logger.setLevel(_level)
        if self._file_logger:
            self._file_logger.setLevel(_level)
