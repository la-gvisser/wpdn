# =============================================================================
#    forenics.py
#        Main project file.
# =============================================================================
#    Copyright (c) WildPackets, Inc. 2010-2014. All rights reserved.

VERSION = "2.0"
MIN_PYTHON = "2.6"


from optparse import OptionParser, OptionGroup
import os
import platform
import pythoncom
import re
import sys
import time
import omniscript


def validate_platform():
    """Validate python version is correct."""
    strMessage = "Error: Invalid Python version. This utility will only work " \
                 "with 32 bit versions of Python %s or greater." % MIN_PYTHON
    
    # determine if python is 64 bit
    is_32bits = True
    if platform.architecture()[0] == '64bit':
        is_32bits = False
    
    # determine invalid python version
    valid_version = True
    version = platform.python_version()
    if version < MIN_PYTHON:
        valid_version = False
    
    #New python3 print (synatx)
    if not is_32bits or not valid_version:
        print ((strMessage))
        exit(-1)
    return 


def format_com_message(msg, e):
    """Formats com errors into something readable"""
    err_msg = "%s; " % (msg)
                
    if e.excepinfo is not None:
        if e.excepinfo[2] is not None:
            err_msg += e.excepinfo[2] + "\n"
    else:
        err_msg += " %d %s\n" % (e.hresult, e.strerror)
    
    return err_msg


# limit modes enum
class LimitMode:
    NONE = 0
    PACKETS = 1
    BYTES = 2
    BUFFER = 3
    

class ForensicError(Exception):
    """Exception class indicating that something has gone drastically wrong
    with the search.
    """
    pass


class Forensics(object):
    """Forensics Search"""
    def __init__(self, **kwargs):     
        self.output_file = kwargs.get('output_file', None)
        self.output_folder = kwargs.get('output_folder', None)
        self.verbosity = kwargs.get('verbosity', False)
        self.engine = None
        
        # connect parameters
        self.host = kwargs.get('host', 'localhost')
        self.port = kwargs.get('port', 6367)
        self.auth = kwargs.get('auth', 'Default')
        self.domain = kwargs.get('domain', '')
        self.user = kwargs.get('user', '')
        self.password = kwargs.get('password', '')
        
        # search parameters
        self.query_name = kwargs.get('name', None)
        self.file_name = kwargs.get('file', None)
        self.filter_name = kwargs.get('filter', None)
        self.start_time = kwargs.get('start_time', None)
        self.end_time = kwargs.get('end_time', None)
        self.limit = kwargs.get('limit', None)
        self.limit_size = kwargs.get('limit_size', 0)
            
        if self.verbosity:
            print("I: Initializing Forensics objects")
                
        # init OmniScript
        try:
            self.omniscript = omniscript.OmniScript()
        except pythoncom.com_error as error:
            print("%s" % format_com_message("Failed to create OmniScript object.", error))
            sys.exit(1)
        
        # init RemoteFileViewQuery
        try:
            self.query = omniscript.ForensicTemplate()
        except pythoncom.com_error as error:
            print("%s" % format_com_message("Failed to create the Forensic Search Template", error))
            sys.exit(1)
        
        # construct query with default parameters
        self.construct_query()        

               
    def __getattr__(self, name, *args):
        """global getter for omniscript"""
        try: 
            return getattr(self.omniscript, name, *args)
        except pythoncom.com_error as error:
            e = format_com_message("Error", error)
            raise ForensicError(e)
    
    
    def _has_file(self, file_name):
        """Verify that a file exists on the engine."""
        if self.verbosity:
            print("I: Verify %s exists on engine" % file_name)
       
        if not self.is_connected():
            self.connect()

        try:
            file_list = self.engine.get_forensic_file_list()
        except pythoncom.com_error as error:
            e = format_com_message("Failed to get Files View list", error)
            raise ForensicError(e)
                
        for file in file_list:
            if file_name == file.name:
                return file
        return None
        
        
    def _has_filter(self, filter_name):
        """Verify that a filter exists on the engine."""
        
        if self.verbosity:
            print("I: Verify %s exists on engine" % filter_name)
        
        if not self.is_connected():
            self.connect()
            
        try:
            filter_obj = self.engine.find_filter(filter_name)
        except pythoncom.com_error as error:
            e = format_com_message("Failed to get Filters list", error)
            raise ForensicError(e)
        return filter_obj
  
            
    def _format_time(self, str_time):
        """Returns UTC time."""
        if self.verbosity:
            print("I: Format date/time values")

        try:
            return time.mktime( time.strptime(str_time, "%m/%d/%Y %H:%M:%S") )
        except ValueError as e:
            raise ForensicError(e)


    def is_connected(self):
        if self.engine is None:
            return False
        return self.engine.is_connected()

        
    def connect(self):
        """connects to engine"""
        if self.verbosity:
            print("I: Connect to OmniEngine")
            
        try:           
            self.engine = self.omniscript.connect(
                self.host, self.port, self.auth, self.domain, self.user,
                self.password, 30000 )
        except pythoncom.com_error as error:
            e = format_com_message("Failed to connect to engine %s:%u" % (self.host, self.port), error)
            raise ForensicError(e)

    
    def list_files(self):
        """lists all files from engine"""
        if self.verbosity:
            print("I: List all files on OmniEngine")

        if not self.is_connected():
            self.connect()
                        
        try:
            file_list = self.engine.get_forensic_file_list()
        except pythoncom.com_error as error:
            e = format_com_message("Failed to get file list", error)
            raise ForensicError(e)
            
        print("Adapter Address,Adapter Name,Capture ID,Capture Name,File Name,File Path,File Size,Link Speed,Media Sub Type,Media Type,Packet Count,Session End Time,Session Start Time,Time Zone Bias")
        for file in file_list:
             print("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s" % 
                  (file.adapter_address,
                   file.adapter_name,
                   file.capture_id,
                   file.capture_name,
                   file.name,
                   file.path,
                   file.size,
                   file.link_speed,
                   file.media_sub_type,
                   file.media_type,
                   file.packet_count,
                   file.end_time,
                   file.start_time,
                   file.time_zone_bias) )


    def set_query_file(self, file_name):
        """sets file to search against"""
        if self.verbosity:
            print("I: Set file search parameter")
            
        if file_name is None:
            return 
        
        file = self._has_file(file_name)
        
        if file:
            try:
                self.query.add_file(file)
            except pythoncom.com_error as error:
                e = format_com_message("Failed to set file", error)
                raise ForensicError(e)
        else:
            raise ForensicError("Search file not found - '%s'" % file_name)


    def set_query_filter(self, filter_name):
        """sets query filter"""
        if self.verbosity:
            print("I: Set filter search parameter")
            
        if filter_name is None:
            return
        
        filter_obj = self._has_filter(filter_name)

        if filter_obj:
            try:
                self.query.add_filter(filter_obj.name)
            except pythoncom.com_error as error:
                e = format_com_message("Failed to set filter", error)
                raise ForensicError(e)
        else:
            raise ForensicError("Search filter not found - '%s'" % filter_name)

    
    def set_query_starttime(self, str_time):
        """sets query start time"""
        if self.verbosity:
            print("I: Set query start time parameter")
            
        if str_time is None:
            return
    
        utc_start_time = self._format_time(str_time)

        try:
            self.query.start_time = omniscript.PeekTime(
                omniscript.PeekTime.system_time_to_peek_time(utc_start_time))
        except pythoncom.com_error as error:
            e = format_com_message("Failed to set query start time", error)
            raise ForensicError(e)


    def set_query_endtime(self, str_time):
        """sets query end time"""
        if self.verbosity:
            print("I: Set query end time parameter")
            
        if str_time is None:
            return
    
        utc_end_time = self._format_time(str_time)

        try:
            self.query.end_time = omniscript.PeekTime(
                omniscript.PeekTime.system_time_to_peek_time(utc_end_time))
        except pythoncom.com_error as error:
            e = format_com_message("Failed to set query end time", error)
            raise ForensicError(e)
    
    
    def set_query_limits(self, limit, size):
        """sets query limit"""
        if self.verbosity:
            print("I: Set query limit parameters")
            
        if limit is None:
            return
        
        try:
            self.query.set_limit(limit.lower(), size)
        except pythoncom.com_error as error:
            e = format_com_message("Failed to set query limit", error)
            raise ForensicError(e)
    
    
    def set_query_analysis_options(self):
        """disable all analysis options but packets"""
        if self.verbosity:
            print("I: Disable all analysis options")
            
        try:
            self.query.option_packets = True

            self.query.option_error = False
            self.query.option_expert = False
            self.query.option_graphs = False
            self.query.option_history = False
            self.query.option_log = False
            self.query.option_network = False
            self.query.option_node = False
            self.query.option_node_protocol_detail = False
            self.query.option_plugins = False
            self.query.option_protocol = False
            self.query.option_size = False
            self.query.option_summary = False
            self.query.option_top_talkers = False
            self.query.option_voice = False
            self.query.option_web = False
            self.query.option_wireless_channel = False
            self.query.option_wireless_node = False
        except pythoncom.com_error as error:
            e = format_com_message("Failed to set query end time", error)
            raise ForensicError(e)
            
            
    def set_query_name(self, name):
        """sets the name of the query"""
        if self.verbosity:
            print("I: Set query name parameter")
            
        try:
            self.query.name = name
        except pythoncom.com_error as error:
            e = format_com_message("Failed to set query name", error)
            raise ForensicError(e)
        
        
    def construct_query(self):
        """constructs query from parameters"""
        if self.verbosity:
            print("I: Constructing query")
            
        try:
            if self.query_name is not None:
                self.set_query_name(self.query_name)
                
            self.set_query_analysis_options()
            
            if self.file_name is not None:
                self.set_query_file(self.file_name)
            
            if self.filter_name is not None:
                self.set_query_filter(self.filter_name)
            
            if self.start_time is not None:
                self.set_query_starttime(self.start_time)
                value = self.query.start_time.ctime()
            
            if self.end_time is not None:
                self.set_query_endtime(self.end_time)
                value = self.query.end_time.ctime()
            
            if self.limit is not None:
                self.set_query_limits(self.limit, self.limit_size)
        except ForensicError as error:
            print ("Error: %s\n" % error)
            sys.exit(1)
    
            
    def execute(self):
        """executes the query"""         
        try:
            if self.end_time < self.start_time:
                raise ForensicError("Search end time is less than search start time." )
            
            # connect to engine 
            if not self.is_connected():
                self.connect()
            
            # perform search
            if self.verbosity:
                print("I: Execute query")

            print("Performing forensic search...")
            try:    
                search = self.engine.create_forensic_search(self.query)
            except pythoncom.com_error as error:
                e = format_com_message("Forensic search failed", error)
                raise ForensicError(e)

            # wait for the search to complete.
            while search.status != omniscript.STATUS_COMPLETE:
                search.refresh()

            # log the number of packets.
            if self.verbosity:
                print("I: Found %d packets" % search.packet_count)
            
            # save packets locally
            try:              
                search.save_packets(self.output_file)
                
                print("Saved search results to %s" % self.output_file)
            except pythoncom.com_error as error:
                e = format_com_message("Forensic search returned no results", error)
                raise ForensicError(e)
        
            # delete search results
            try:
                self.engine.delete_forensic_search(search)
            except pythoncom.com_error as error:
                e = format_com_message("Failed to delete query results", error)
                raise ForensicError(e)

        except ForensicError as error:
            print ("Error: %s\n" % error)
            sys.exit(1)

def build_opt_parser():
    # used for default search name and file name
    unique_name = time.strftime("Search%Y-%m-%dT%H.%M.%S", time.localtime())
    
    usage = "usage: %prog [options]"
    parser = OptionParser(usage=usage)
    
    parser.add_option("-v", "--verbosity",
                      action="store_true", dest="verbosity", default=False,
                      help='Verbosity level; [Default: Minimal output]')
    
    # OmniEngine connect options
    connect_group = OptionGroup(parser, "Connection Options")
    connect_group.add_option("-o", "--host",
                             action="store", dest="host", default='localhost',
                             help="IP Address of OmniEngine [Default: 'localhost']")
    connect_group.add_option("-p", "--port",
                             action="store", dest="port", default=6367,
                             help="Port number of OmniEngine [Default: '6367']")
    connect_group.add_option("-a", "--auth",
                             action="store", dest="auth",
                             choices=['Default', 'Third Party'], default='Default',
                             help="Authentication Method, 'Default' or 'Third Party' [Default: 'Default']")
    connect_group.add_option("-d", "--domain",
                             action="store", dest="domain", default="",
                             help="Domain [Default: None]")
    connect_group.add_option("-u", "--user",
                             action="store", dest="user", default="",
                             help="User [Default: Local Account]")
    connect_group.add_option("-w", "--password",
                             action="store", dest="password", default="",
                             help="Password [Default: Local Account Password]")    
    parser.add_option_group(connect_group)

    # Forensic search options
    forensic_group = OptionGroup(parser, "Forensic Search Options")
    forensic_group.add_option("--name",
                              action="store", dest="name", default=unique_name,
                              help="Search result name; [Default: %s]" % unique_name)
    forensic_group.add_option("--file",
                              action="store", dest="file", default=None,
                              help="File - File to search; 'Packet2011-02-25T13.07.55.459.pkt'")
    forensic_group.add_option("--start_time",
                              action="store", dest="start_time", default=None,
                              help="Time Range - Start time to search by (Month/Day/Year Hour:Minute:Second); '5/31/2010 12:01:00'")
    forensic_group.add_option("--end_time",
                              action="store", dest="end_time", default=None,
                              help="Time Range - End time to search by (Month/Day/Year Hour:Minute:Second); '12/24/2010 23:59:59'")
    forensic_group.add_option("--filter",
                              action="store", dest="filter", default=None,
                              help="Filters - Name of filter to search by; 'HTTP'")
    forensic_group.add_option("--limit",
                              action="store", dest="limit", default='None',
                              choices=['None', 'Packets', 'Bytes', 'Buffer'],
                              help="Limits - Limit options; 'None', 'Packets', 'Bytes' or 'Buffer'")
    forensic_group.add_option("--limit_size",
                              action="store", dest="limit_size", default=0,
                              help="Limits - Limit size of results: count or MB")
    forensic_group.add_option("--output_file", 
                              action = "store", dest = "output_file",
                              default = "%s\\%s.pkt" % (os.environ['HOMEPATH'], unique_name),
                              help="Output dir for saving forensic results; [Default: %s\\%s.pkt]" % (os.environ['HOMEPATH'], unique_name) )
    forensic_group.add_option("--output_folder", 
                              action = "store", dest = "output_folder",
                              default = "%s\\" % (os.environ['HOMEPATH']),
                              help="Output dir for saving forensic results; [Default: %s\\]" % (os.environ['HOMEPATH']) )
    parser.add_option_group(forensic_group)
    return parser
                
            
def main(argv=None):
    if argv is None:
        argv = sys.argv

    validate_platform()

    parser = build_opt_parser()

    # Parse arguments
    (options, args) = parser.parse_args(argv)

    if len(argv) < 2:
        parser.print_help()
        return False
    
    # Make sure a start and end time have been entered
    if ((options.start_time and not options.end_time) or
        (options.end_time and not options.start_time)):
        print("Error: Must enter start and end times")
        return False
    
    # Init search object
    search = Forensics(**options.__dict__)

    # Execute the search.
    search.execute()

    # Return True to continue, False to Stop chains of commands.
    return True

if __name__ == "__main__":
    main()
