# =============================================================================
#    RCapture.py
#        Main project file.
# =============================================================================
#    Copyright (c) WildPackets, Inc. 2013. All rights reserved.

VERSION = "2.1"
MIN_PYTHON = "2.6"

import os
import sys
import platform
import logging
import getopt
import uuid
import re
import tempfile

import time

from xml.dom import minidom
import xml.etree.ElementTree as ET

# Pretty Print a Capture Template
#   ET.tostring(obj)
# Single Line
#   ET.tostring(obj).replace("\n", "")

import omniscript


def validate_platform():
    """ Validate python version is correct. """
    errorMessage = "Error: Invalid Python version. This utility will only work "
    errorMessage += "with 32 bit versions of Python %s or greater." % MIN_PYTHON

    # determine if python is 64 bit
    is_32bits = True
    if platform.architecture()[0] == "64bit":
        is_32bits = False

    # determine invalid python version
    valid_version = True
    version = platform.python_version()
    if version < MIN_PYTHON:
        valid_version = False

    #New python3 print(synatx)
    if not is_32bits or not valid_version:
        print(errorMessage)
        exit(-1)
    return


def summary_values(stat):
    type_dict = [[""], ["date"], ["time"], ["duration"], ["packets"], ["bytes"],
                ["packets", "bytes"], ["int"], ["double"]]
    values = None
    _type = stat["type"]
    if _type >= 1 and _type <= len(type_dict):
        values = {}
        for t in type_dict[_type]:
            values[t] = stat[t]
    return values


class CCaptureMgr:
    """ The Capture Manager """
    def __init__(self,
                 host = "localhost",
                 port = 6367,
                 auth = "Default",
                 domain = "",
                 user = "",
                 password = ""):
        # Initialize the runtime parameters.
        self._omniscript = omniscript.OmniScript()
        self._engine = None

        # Logging
        self._verbose = 1
        self.logger = self._omniscript.logger

        # engine connect information
        self.engine_id = host
        self.engine_port = port
        self.auth = auth
        self.domain = domain
        self.user_name = user
        self.password = password

        # capture information
        self.capture = None
        self.adapter = None
        self.capture_template = None   # "capturetemplate.xml"
        self.adapter_index = -1        # convert adapter properties to a dictionary.
        self.adapter_description = ""
        self.capture_name = ""
        self.capture_seconds = 0
        self.capture_minutes = 0
        self.capture_buffer = 64
        self.filename = ""
        self.file_format = "pkt"
        self.save_to_disk = ""
        self.wireless_channel_number = 0
        self.wireless_channel_frequency = 0
        self.reserved_size = 0
        self.save_every = 0
        self.disk_file = 0

        # stats information
        self.stats_node = False
        self.stats_flow = False
        self.stats_summary = False
        self.stats_protocol = False
        self.csv = False

        # capture commands
        self.capture_create = False
        self.capture_start = False
        self.capture_stop = False
        self.capture_save = False
        self.capture_delete = False

        self.adapters_list = False
        self.captures_list = False
        self.filters_list = False
        self.packets_list = False
        self.wireless_channel_set = False
        self.filter_create = False
        self.filter_delete = False
        self.filter_list = []
        self.create_filter_list = []
        self.filter_name = ""
        self.sessions_delete_all = False
        self.help_display = False

    def clean_up(self):
        """ Clean up before exiting. """
        if self._engine is not None:
            self._engine.disconnect()

    def connect(self):
        """ Connent to the OmniEngine. """
        engine = self._omniscript.connect(self.engine_id, self.engine_port,
                                          self.auth, self.domain,
                                          self.user_name, self.password)
        if engine is None:
            self.logger.error("Engine is off-line or inaccessible.")
            return False
        self._engine = engine
        return True

    def create_capture(self):
        """ Create a new Capture from the run-time settings """
        if (self.adapter_index < 0) and (not self.adapter_description):
            self.logger.error("Invalid or missing Adapter Index (-a) or Description (-q).")
            return False
           
        if not self.capture_name:
            self.logger.error("Invalid or missing capture name (-n).")
            return False

        if not self.is_connected():
            self.logger.error("Create Capture: no connection")
            return False

        self.capture = self._engine.find_capture(self.capture_name)
        if self.capture is not None:
            self.logger.error("The capture already exists: '%s'", self.capture_name)
            return False

        self.logger.debug("create_capture 1")

        template = omniscript.CaptureTemplate(self.capture_template)
        template.general.name = self.capture_name

        # Get the Adapter information.
        adapter = None
        if self.adapter_index >= 0:
            adapter_list = self._engine.get_adapter_list()
            if self.adapter_index >= len(adapter_list):
                self.logger.error("Invalid adapter index: %d. Must be between 0 and %d", self.adapter_index, len(adapter_list))
                return False
            adapter = adapter_list[self.adapter_index]
        elif self.adapter_description:
            adapter = self._engine.find_adapter(self.adapter_description)

        if adapter is None:
            self.logger.error("Could not find the adapter.")
            return False

        template.set_adapter(adapter)
        if self.logger.isEnabledFor(logging.DEBUG):
            self.logger.debug("Using adapter:")
            for attr in adapter_attributes:
                self.logger.debug("%18s : %s" % (attr, adapter[attr]))

        # Set the filter list, vs. add to the filter list specified in capture_template.
        template.set_filters(self.filter_list)

        self.logger.debug("Created capture template: %s", self.capture_name)
        if self.filename:
            template.save(self.filename)
            self.logger.info("Saved capture template to: %s", self.filename)

        self.capture = self._engine.create_capture(template)
        if self.capture is None:
            self.logger.error("Failed to create new capture: %s.", self.capture_name)
            return False

        if self._verbose > 0:
            print("Created capture: %s" % self.capture_name)
        return True

    def create_bpf_filter(self, name, criteria):
        """ Create a new BPF/TCP Dump filter and add it to the engine.
        Return the new Filter object or an existing Filter object.
        """
        # Does the filter already exist?
        # ToDo: Should this fail, the filter might not match.
        filter_obj = self._engine.find_filter(name)
        if filter_obj:
            self.logger.info("Filter already exists: %s - %s", name, criteria)
            return filter_obj

        filter_obj = omniscript.Filter(name)
        if filter_obj is None:
            self.logger.error("Failed to create filter: %s - %s", name, criteria)
            return None

        tcp_dump_node = omniscript.TCPDumpNode()
        tcp_dump_node.filter = criteria
        filter_obj.criteria = tcp_dump_node

        self._engine.add_filter(filter_obj)

        if self._verbose > 0:
            print("Created BPF filter: %s - %s." % (name, criteria))
        return self._engine.find_filter(name)

    def create_ip_filter(self, name, ip_address):
        """ Create a new simple address filter and add it to the engine.
        Return the new Filter object or an existing Filter object.
        """
        # Does the filter already exist?
        # ToDo: Should this fail, the filter might not match.
        filter_obj = self._engine.find_filter(name)
        if filter_obj:
            self.logger.info("Filter already exists: %s - %s", name, ip_address)
            return filter_obj

        filter_obj = omniscript.Filter(name)
        if not filter_obj:
            self.logger.error("Failed to create filter: %s - %s", name, ip_address)
            return None

        address_node = omniscript.AddressNode()
        address_node.address_1 = omniscript.IPv4Address(ip_address)
        address_node.address_2 = omniscript.IPv4Address('0.0.0.0')
        filter_obj.criteria = address_node

        self._engine.add_filter(filter_obj)

        if self._verbose > 0:
            print("Created address filter: %s - %s." % (name, ip_address))
        return self._engine.find_filter(name)

    def create_filters(self):
        """ Create new filters from create_filter_list.
        Add new filters to filter_list list
        """
        for filter_spec in self.create_filter_list:
            filter_obj = None
            kind, name, criteria = filter_spec.split(":", 2)
            if kind == "i":
                filter_obj = self.create_ip_filter(name, criteria)
            elif kind == "b":
                filter_obj = self.create_bpf_filter(name, criteria)
            if filter_obj is None:
                continue
            self.filter_list.append(name)
        return True

    def delete_all_sessions(self):
        """ Delete all the capture sessions. """
        if not self.is_connected():
            self.logger.error("Create Capture: no connection.")
            return False

        self._engine.delete_all_capture_sessions()
        if self._verbose > 0:
            print("Deleted all the capture sessions.")
        return True

    def delete_capture(self):
        """ Delete the named capture. Return Faslse on failure. """
        if not self.capture_name:
            self.logger.error("Invalid or missing capture name (-n).")
            return False

        if not self.is_connected():
            self.logger.error("Create Capture: no connection.")
            return False

        self.capture = self._engine.find_capture(self.capture_name)
        if self.capture is None:
            self.logger.error("Did not find the capture: %s", self.capture_name)
            return False

        self._engine.delete_capture(self.capture)

        if self._verbose > 0:
            print("Deleted the capture: %s." % self.capture_name)
        return True

    def delete_filter(self, filterName):
        """ Delete the named filter. Return False on failure. """
        if not self.is_connected():
            self.logger.error("Delete Filter: no connection")
            return False

        filter_obj = self._engine.find_filter(filterName)
        if not filter_obj:
            self.logger.error("Failed to find Filter %s in the local Filter Collection.", filterName)
            self.logger.error("Failed to delete the Filter: %s", filterName)
            return False

        self._engine.delete_filter(filter_obj)

        if self._verbose > 0:
            print("Deleted the filter: %s" % filterName)
        return True
    
    def display_help(self):
        """ Display command line options """
        #               1         2         3         4         5         6         7         8
        #      12345678901234567890123456789012345678901234567890123456789012345678901234567890
        print("RCapture")
        print("  [-c <command>]     : Capture command: create, start, stop, save or delete.")
        print("  [--template <template> ]: Capture template (optional)")
        print("  [-e <engine IP>]   : IP address of engine. Default is localhost.")
        print("  [-p <engine Port>] : Port number of the engine. Default is 6367.")
        print("  [-t <auth>]        : Authentication type: 'Default' or 'Third Party'. Default is 'Default'.")
        print("  [-d <domain]       : Domain of the credentials.")
        print("  [-u <username>     : User account name.")
        print("  [-w <password>]    : User account password.")
        print("  [-a <index>]       : The adapter index. Use -l (the letter el) to list adapters.")
        print("  [-q <description>] : The adapter description. (-a takes precedence.)")
        print("  [--wireless_channel <channel>]: Set the wireless adapter channel by channel number.")
        print("  [--wireless_frequency <frequency>]: Set the wireless adapter channel by channel frequency.")
        print("  [-n <capture name>]: Capture name.")
        print("  [-o <filename>]    : Filename for template or saved packets. (See --format when saving capture)")
        print("  [--format <format> ]: Save formats: pkt, enc, pcap or raw.")
        print("  [-b <size>         : Capture buffer size in megabytes. Default is 64MB.")
        print("  [-f <path>]        : Save To Disk filename template.")
        print("  [-r <hours>        : Restart - The number of hours to start a new file. Default is 24 hours.")
        print("  [-m <size>         : Save to Disk file size in megabytes. Default is Capture Buffer size or 1MB.")
        print("  [--reserve <size GB>]: Amount of disk space to use for capture, in GB. (Timeline only)")
        print("  [-h <filter name>] : Add named filter to the capture.")
        print("  [-i <name:ip>]     : Create named IP address filter.")
        print("  [-I <name:bpf>]    : Create named BPF/TCP Dump filter.")
        print("  [-j <filter name>] : Delete the named filter.")
        print("  [--sec <seconds>]  : Seconds between start and stop.")
        print("  [--min <minutes>]  : Minutes between start and stop.")
        print("  [-l]               : List the adapters and quit.")
        print("  [-g]               : List the captures and quit.")
        print("  [-y]               : List the filters and quit.")
        print("  [-k]               : List packets and quit.")
        print("  [-s <mode[,mode]>] : Statistics command: node, flow, summary and protocol. Require capture name (-n).")
        print("                         Combinations are allowed. For example: -s node,protocol")
        print("  [-z]               : Delete all capture sessions.")
        print("  [--csv]            : Print the stats as csv output.")
        print("  [-v <level>]       : Verbose mode: 0=none, 1=error, 2=informational, 3=debug. Default is 1.")
        print("  [-?]               : Display this message and quit.")

    def flow_stats(self):
        """ Display the flow statistics for the current capture. """
        if not self.capture_name:
            self.logger.error("Invalid or missing capture name (-n).")
            return False
       
        if not self.is_connected():
            self.logger.error("Flow Stats: no connection.")
            return False

        self.capture = self._engine.find_capture(self.capture_name)
        if self.capture is None:
            self.logger.debug("Did not find the capture: %s", self.capture_name)
            return False

        captureId = self.capture.id
        if captureId is None:
            self.logger.error("Failed to get the id for capture: %s", self.capture_name)
            self.logger.error("Failed to get flow stats for capture: %s", self.capture_name)
            return False

        #statsContext = self._connection.GetStatsContext(captureId)
        #if statsContext is None:
        #    self.logger.error("Failed to get statistics context for capture: %s.", self.capture_name)
        #    return False

        #convList = statsContext.GetFlows()
        #if convList is None:
        #    print("No flow statistics for Capture: %s" % self.capture_name)
        #    return False
        #
        #if self.csv:
        #    format = "%s,%s,%d,%d,%d,%d,%s"
        #else:
        #    format = "%-16s %-16s %16d %16d %16d %16d %s"
        #    print("\n\nFlow statistics for capture - %s:\n" % self.capture_name)
        #    print(format % ("Source IP",
        #                    "Destination IP",
        #                    "Packets In",
        #                    "Packets Out",
        #                    "Bytes In",
        #                    "Bytes Out",
        #                    "Protocol"))
        #    print("-" * 110)
        #
        #for conv in convList:
        #    print(format % (conv["SrcIPAddress"],
        #                    conv["DestIPAddress"],
        #                    int(conv["PacketsReceived"]),
        #                    int(conv["PacketsSent"]),
        #                    int(conv["BytesReceived"]),      
        #                    int(conv["BytesSent"]),
        #                    conv["Protocol"]))
        #return True
        self.logger.error("Flow Stats is not implemented.")
        return True

    def is_connected(self):
        """ Return True if connected to an OmniEngine. """
        if self._engine is None:
            return False
        return self._engine.is_connected()

    def list_adapter(self, a, long=False):
        if a is None:
            print("No adapter")
            return

        print("Name: %s" % (a.name))
        print("Id: %s" % (a.id))
        if long: print("Features: %d" % (a.features))
        print("Type: %d" % (a.adapter_type))
        if long: print("Address: %s" % (str(a.address)))
        print("Description: %s" % (a.description))
        if long: print("Device Name: %s" % (a.device_name))
        if long: print("Interface Features: %s" % (a.interface_features))
        if long: print("Link Speed: %d" % (a.link_speed))
        if long: print("Media Type: %d" % (a.media_type))
        if long: print("Media Sub Type: %d" % (a.media_sub_type))
        if long: print("WildPackets API: %s" % (a.wildpackets_api))
        print

    def list_adapters(self):
        """ Display the OmniEngine"s adapter list. """
        if not self.is_connected():
            self.logger.error("List Adapters: no connection.")
            return

        adapters = self._engine.get_adapter_list()
        if adapters is None:
            self.logger.error("Failed to get the adapter list.")
            return

        if self._verbose > 0:
            print("Total adapters: %u" % len(adapters))
            for i, adpt in enumerate(adapters):
                print("Adapter %u: " % i)
                self.list_adapter(adpt, (self._verbose < 2))

    def list_capture(self, c, long=False):
        if c is None:
            print("No capture")
            return

        print("Name: %s" % (c.name))
        print("Status: %s" % (c.format_status()))
        print("Comment: %s" % (c.comment))
        print("Id: %s" % (c.id))
        if long: print("Creator: %s" % (c.creator))
        if long: print("Creator Sid: %s" % (c.creator_sid))
        if long: print("Logged on User SID: %s" % (c.logged_on_user_sid))
        if long: print("Last Modified By: %s" % (c.last_modified_by))
        if long: print("Last Modification: %s" % (c.last_modification))
        print("Adapter: %s" % (c.adapter))
        if long: print("Adapter Type: %s" % (c.adapter_type))
        if long: print("Link Speed: %s" % (c.link_speed))
        if long: print("Media Type: %s" % (c.media_type))
        if long: print("Media Sub Type: %s" % (c.media_sub_type))
        print("Buffer Size: %s" % (c.buffer_size))
        if long: print("Buffer Available: %s" % (c.buffer_available))
        if long: print("Buffer Used: %s" % (c.buffer_used))
        if long: print("Filter Mode: %s" % (c.filter_mode))
        if long: print("Graphs Count: %s" % (c.graphs_count))
        if long: print("Plugin List: %s" % (c.plugin_list))
        if long: print("Start Time: %s" % (c.start_time.ctime()))
        if long: print("Stop Time: %s" % (c.stop_time.ctime()))
        if long: print("Duration: %s" % (c.duration))
        if long: print("Reset Count: %s" % (c.reset_count))
        if long: print("Packets Received: %s" % (c.packets_received))
        if long: print("Packets Filtered: %s" % (c.packets_filtered))
        print("Packet Count: %s" % (c.packet_count))
        if long: print("Analysis Dropped Packets: %s" % (c.analysis_dropped_packets))
        if long: print("Duplicate Packets Discarded: %s" % (c.duplicate_packets_discarded))
        if long: print("Packets Analyzed: %s" % (c.packets_analyzed))
        if long: print("Packets Dropped: %s" % (c.packets_dropped))
        if long: print("First Packet: %s" % (c.first_packet))
        if long: print("Alarms Info: %s" % (c.alarms_info))
        if long: print("Alarms Minor: %s" % (c.alarms_minor))
        if long: print("Alarms Major: %s" % (c.alarms_major))
        if long: print("Alarms Severe: %s" % (c.alarms_severe))
        if long: print("Trigger Count: %s" % (c.trigger_count))
        if long: print("Trigger Duration: %s" % (c.trigger_duration))
        if long: print("Option Alarms: %s" % (c.option_alarms))
        if long: print("Option Expert: %s" % (c.option_expert))
        if long: print("Option Filters: %s" % (c.option_filters))
        if long: print("Option Graphs: %s" % (c.option_graphs))
        if long: print("Option Hidden: %s" % (c.option_hidden))
        if long: print("Option Packet Buffer: %s" % (c.option_packet_buffer))
        if long: print("Option Timeline Stats: %s" % (c.option_timeline_stats))
        if long: print("Option Voice: %s" % (c.option_voice))
        if long: print("Option Web: %s" % (c.option_web))
        print

    def list_captures(self):
        """ Display the OmniEngine"s captures. """
        if not self.is_connected():
            self.logger.error("List Captures: no connection.")
            return

        # if capture name is provided return status for only that capture
        if self.capture_name:
            self.capture = self._engine.find_capture(self.capture_name)
            if self.capture is None:
                self.logger.debug("Did not find capture %s", self.capture_name)
                return
            self.list_capture(self.capture, False)
            if self._verbose <= 2:
                print("%27s : %s" % ("Status", self.capture.format_status()))
            
        # print all captures
        else:
            capture_list = self._engine.get_capture_list();
            if (capture_list is None):
                self.logger.error("Failed to get the capture list.")
                return
            print("Total captures: %u" % len(capture_list))
            for i, capt in enumerate(capture_list):
                print("Capture %u: " % i)
                self.list_capture(capt, (self._verbose >= 2))

    def list_filter(self, f, long=False):
        print("Name: %s, Id: %s\n" % (f.name, f.id))

    def list_filters(self):
        """ Display the OmniEngine"s filters. """
        if not self.is_connected():
            self.logger.error("List Filters: no connection.")
            return

        filter_list = self._engine.get_filter_list()
        if filter_list is None:
            self.logger.error("Failed to get the filter list.")
            return

        print("Total filters: %u" % len(filter_list))
        for i, fltr in enumerate(filter_list):
            print("Filter %u: " % i)
            self.list_filter(fltr)

    def list_packets(self):
        """ Display the packets of the current capture. """
        if not self.is_connected():
            self.logger.error("List Packets: no connection.")
            return

        # if capture name is provided return status for only that capture
        if self.capture_name:
            self.capture = self._engine.find_capture(self.capture_name)
            if self.capture is None:
                self.logger.error("Did not find capture: %s" % self.capture_name)
                return

            print("%s: %s" % (self.capture.name, self.capture.format_status()))
        else:
            capture_list = self._engine.get_capture_list()
            if capture_list is None:
                self.logger.error("Failed to get the capture list.")
                return

            for capture in capture_list:
                print("%s: %s" % (capture.name, capture.format_status()))

    def node_stats(self):
        """ Display the node statistics for the current capture. """
        if not self.capture_name:
            self.logger.error("Invalid or missing capture name (-n).")
            return False

        if not self.is_connected():
            self.logger.error("Node Stats: no connection.")
            return False
       
        self.capture = self._engine.find_capture(self.capture_name)
        if self.capture is None:
            self.logger.error("Did not find the capture: %s", self.capture_name)
            return False

        node_list = self.capture.get_node_stats()
        if node_list is None:
            self.logger.error("No node statistics for capture: %s", self.capture_name)
            return False

        if self.csv:
            format = "%s,%d,%d,%d,%d"
        else:
            format = "%-16s %16d %16d %16d %16d"
            print("\n\nNode Statistics for capture - %s:\n" % self.capture_name)
            print("%-16s %16s %16s %16s %16s" % ("Node",
                                                 "Packets Received",
                                                 "Packets Sent",
                                                 "Bytes Received",
                                                 "Bytes Sent"))
            print( "-" * 84)
       
        for node in node_list:
            if node.address_type == omniscript.ADDRESS_TYPE_IPV4:
                print(format % (node.node,
                                node.packets_received,
                                node.packets_sent,
                                node.bytes_received,
                                node.bytes_sent))
        for node in node_list:
            if node.address_type == omniscript.ADDRESS_TYPE_IPV6:
                print(format % (node.node,
                                node.packets_received,
                                node.packets_sent,
                                node.bytes_received,
                                node.bytes_sent))
        for node in node_list:
            if node.address_type == omniscript.ADDRESS_TYPE_ETHERNET:
                print(format % (node.node,
                                node.packets_received,
                                node.packets_sent,
                                node.bytes_received,
                                node.bytes_sent))
        return True

    def parse_arguments(self, argList):
        """ Parse the command line arguments. """
        try:
            (opts, args) = getopt.getopt(argList,
                                         "lgykzc:o:e:p:t:s:d:u:w:a:q:n:b:h:i:I:j:f:r:m:v:?",
                                         ["wireless_channel=",
                                         "wireless_frequency=",
                                         "csv",
                                         "format=",
                                         "template=",
                                         "reserve=",
                                         "sec=",
                                         "min="])
        except getopt.GetoptError as error:
            # print(help information and exit:)
            self.logger.error(str(error)) # will print something like "option -a not recognized")
            self.display_help()
            sys.exit(2)

        for opt, arg in opts:
            if opt in ("-c"):
                if (arg == "create"):
                    self.capture_create = True
                elif (arg == "start"):
                    self.capture_start = True
                elif (arg == "stop"):
                    self.capture_stop = True
                elif (arg == ("delete")):
                    self.capture_delete = True
                elif (arg == ("save")):
                    self.capture_save = True
                else:
                    self.logger.error("Missing capture command.")
                    break

            elif opt in ("-o"):
                self.filename = arg

            elif opt in ("-e"):
                self.engine_id = arg

            elif opt in ("-p"):
                self.engine_port = int(arg)

            elif opt in ("-t"):
                self.auth = arg

            elif opt in ("-d"):
                self.domain = arg

            elif opt in ("-u"):
                self.user_name = arg

            elif opt in ("-w"):
                self.password = arg

            elif opt in ("-a"):
                self.adapter_index = int(arg)

            elif opt in ("-q"):
                self.adapter_description = arg

            elif opt in ("-n"):
                self.capture_name = arg

            elif opt in ("-b"):
                self.capture_buffer = int(arg)

            elif opt in ("-h"):
                self.filter_list.append(arg)

            elif opt in ("-i"):
                self.filter_create = True;
                self.create_filter_list.append("i:" + arg)

            elif opt in ("-I"):
                self.filter_create = True;
                self.create_filter_list.append("b:" + arg)

            elif opt in ("-j"):
                self.filter_delete = True
                self.m_FilterName = arg

            elif opt in ("-f"):
                self.save_to_disk = arg

            elif opt in ("-r"):
                self.save_every = arg

            elif opt in ("-m"):
                self.disk_file = arg

            elif opt in ("-l"):
                self.adapters_list = True

            elif opt in ("-g"):
                self.captures_list = True

            elif opt in ("-y"):
                self.filters_list = True

            elif opt in ("-k"):
                self.packets_list = True

            elif opt in ("-s"):
                for stat in arg.split(","):
                    if (stat == "node"):
                        self.stats_node = True
                    elif (stat == "flow"):
                        self.stats_flow = True
                    elif (stat == "summary"):
                        self.stats_summary = True
                    elif (stat == "protocol"):
                        self.stats_protocol = True
                    else:
                        self.logger.error("Invalid statistics command - %s", stat)
                        break

            elif opt in ("-z"):
                self.sessions_delete_all = True

            elif opt in ("-v"):
                self._verbose = int(arg)
                self._omniscript.set_logging_level(self._verbose)

            elif opt in ("--csv"):
                self.csv = True

            elif opt in ("--wireless_channel"):
                self.wireless_channel_set = True
                self.wireless_channel_number = int(arg)

            elif opt in ("--wireless_frequency"):
                self.wireless_channel_set = True
                self.wireless_channel_frequency = int(arg)

            elif opt in ("--format"):
                self.file_format = str(arg).lower()

            elif opt in ("--template"):
                self.capture_template = arg

            elif opt in ("--reserve"):
                if int(arg) < 16:
                    size = 16
                else:
                    size = (int(arg) / 8) * 8
                self.reserved_size = int(size)

            elif opt in ("--sec"):
                self.capture_seconds = int(arg)

            elif opt in ("--min"):
                self.capture_minutes = int(arg)

            elif opt in ("-?"):
                self.help_display = True
                break;

            else:
                self.logger.error("Unkown argument: %s", opt)
                self.help_display = True

        self.help_display = self.help_display or (len(opts) == 0) # if no opts, then display help

    def process(self):
        """ Process the command line arguments. """
        if self.help_display:
            self.display_help();
            return True

        if self._omniscript is None:
            self.logger.error("Failed to start OmniScript.")
            return False

        self.logger.debug("Process 1")

        # connect or quit
        if not self.connect():
            return False

        self.logger.debug("Process 2")
        self.logger.info("Connected to OmniEngine: %s:%u.", self.engine_id, self.engine_port)
        self.logger.debug("Process 3")

        if self.adapters_list:
            self.logger.info("List Adapters")
            self.list_adapters()
            return True

        if self.captures_list:
            self.logger.info("List Captures")
            self.list_captures()
            return True

        if self.filters_list:
            self.logger.info("List Filters")
            self.list_filters()
            return True

        if self.packets_list:
            self.logger.info("List Packets")
            self.list_packets()
            return True

        if self.filter_create:
            self.logger.info("Create Filters")
            if not self.create_filters():
                return False

        if self.capture_create:
            self.logger.info("Create Capture")
            if not self.create_capture():
                return False

        if self.capture_start:
            self.logger.info("Start Capture")
            if not self.start_capture():
                return False

        if self.capture_minutes:
            self.logger.info("Capture for %d minutes", self.capture_minutes)
            time.sleep(self.capture_minutes * 60 * 1000)

        if self.capture_seconds:
            self.logger.info("Capture for %d seconds", self.capture_seconds)
            time.sleep(self.capture_seconds * 1000)

        if self.capture_stop:
            self.logger.info("Stop Capture")
            if not self.stop_capture():
                return False

        if self.capture_save:
            self.logger.info("Save Packets")
            if not self.save_capture():
                return False

        if self.capture_delete:
            self.logger.info("Delete Capture")
            if not self.delete_capture():
                return False

        if self.stats_node:
            self.logger.info("Node Stats")
            if not self.node_stats():
                return False

        if self.stats_flow:
            self.logger.info("Flow Stats")
            if not self.flow_stats():
                return False

        if self.stats_summary:
            self.logger.info("Summary Stats")
            if not self.summary_stats():
                return False

        if self.stats_protocol:
            self.logger.info("Protocol Stats")
            if not self.protocol_stats():
                return false

        if self.filter_delete:
            self.logger.info("Delete Filter")
            nResult = self.delete_filter(self.m_FilterName)
            if nResult:
                return nResult

        if self.wireless_channel_set:
            self.logger.info("Set Wireless Channel")
            nResult = self.set_wireless_channel()
            if nResult:
                return nResult

        if self.sessions_delete_all:
            self.logger.info("Delete All Capture Sessions")
            if not self.delete_all_sessions():
                return False

        return True

    def protocol_stats(self):
        """ Display the protocol statistics for the current capture. """
        if not self.capture_name:
            self.logger.error("Invalid or missing capture name (-n).")
            return False
       
        if not self.is_connected():
            self.logger.error("Protocol Stats: no connection.")
            return False

        self.capture = self._engine.find_capture(self.capture_name)
        if self.capture is None:
            self.logger.error("Did not find the Capture %s", self.capture_name)
            return False

        protocol_list = self.capture.get_protocol_stats()
        if protocol_list is None:
            self.logger.error("No protocol statistics for Capture - %s", self.capture_name)
            return False
       
        if self.csv:
            format = "%s,%s,%s"
        else:
            format = "%-30s %16s %16s"
            print("\n\nProtocol Statistics for capture - %s:\n" % self.capture_name)
            print(format % ("Protocol",
                            "Packets",
                            "Bytes"))
            print("-" * 84)

        for protocol in protocol_list:
            print(format % (protocol.protocol,
                            protocol.packets,
                            protocol.bytes))
        return True

    def save_capture(self):
        """ Save packets from the current capture to the current file name. """
        if not self.filename:
            self.logger.error("Invalid or missing file name (-o).")
            return False

        if not self.is_connected():
            self.logger.error("Save Capture: no connection")
            return False

        self.logger.debug("Saving packets to %s", self.filename)

        self.capture = self._engine.find_capture(self.capture_name)
        if self.capture is None:
            self.logger.debug("Did not find the Capture %s", self.capture_name)
            return False

        if self.capture.is_capturing():
            self.logger.debug("The Capture %s is still capturing.", self.capture_name);
            return False

        self.logger.debug("Capture Name = %s", self.capture.name)

        if self.capture.id:
            self.logger.debug("Capture Id = %s", self.capture.id)
       
        fmt, convert = -1, False
        # determine format
        if self.file_format == "enc":
            fmt, convert = 0, True
            supported_formats = "(\.enc)$"
            if re.search(supported_formats, self.filename, re.IGNORECASE) is None:
                self.filename += ".enc"
        elif self.file_format == "pcap":
            fmt, convert = 1, True
            supported_formats = "(\.cap|\.pcap|\.dmp|\.appcap|\.appcapz)$"
            if re.search(supported_formats, self.filename, re.IGNORECASE) is None:
                self.filename += ".pcap"
        elif self.file_format == "raw":
            fmt, convert = 2, True
            supported_formats = "(\.txt)$"
            if re.search(supported_formats, self.filename, re.IGNORECASE) is None:
                self.filename += ".txt"
        elif self.file_format == "pkt":
            pkt_formats = "(\.pkt|\.apc|\.wpc|\.wpz)$"
            if re.search(pkt_formats, self.filename, re.IGNORECASE) is not None:
                self.filename = re.sub(pkt_formats, ".pkt", self.filename, re.IGNORECASE)
            else:
                self.filename += ".pkt"
        else:
            self.logger.error(
                "Error: Invalid save format.\n\n" +
                "Valid save formats are:\n" +
                "  pkt (WildPackets Packet File)\n" +
                "  enc (NG Sniffer DOS File)\n" +
                "  pcap (libpcap)\n" +
                "  raw (Raw Packet Data)\n")
            return False

        # determine file name
        if convert:
            temp_file = tempfile.TemporaryFile().name + ".pkt"
        else:
            temp_file = self.filename
           
        # save pkt file on the engine.
        # copy the file to the local system.
        # delete the file on the engine.
        self._engine.save_all_packets(self.capture, temp_file)
        self._engine.get_file(temp_file)
        self._engine.delete_file(temp_file)

        if self._verbose > 0:
            print("Saved packets to %s" % self.filename)
        return True

    def set_wireless_channel(self):
        """ Set the run-time wireless channel. """
        if (self.adapter_index < 0) and (self.adapter_description == ""):
            self.logger.error("Invalid or missing Adapter Index (-a) or Description (-q).")
            return False
       
        if (self.wireless_channel_number <= 0) and (self.wireless_channel_frequency <= 0):
            self.logger.error("Invalid or missing Wireless Channel Number (--wireless_channel) or Frequency (--wireless_frequency).")
            return False
       
        if not self.is_connected():
            self.logger.error("Set Wireless Channel: no connection." )
            return False
       
        """ retrieve peek remote adapter """
        adapter_list = self._connection.get_adapter_sist()
        if adapter_list is None:
            self.logger.error("Failed to get the adapter list." )
            return False
       
        if self.adapter_index > len(apapterList):
            self.logger.error("Invalid adapter index: %d, must be in the range of 0 to %d.", self.adapter_index, len(apapterList))
            return False

        adapter = None
        if self.adapter_index > -1:
            adapter = adapter_list[self.adapter_index]
        else:
            for adpt in adapter_list:
                adptDescription = adapter.description
                if self.adapter_description == adptDescription:
                    adapter = adpt
                    break
       
        if adapter is None:
            self.logger.error("Could not retrieve wireless adapter")
            return False
       
        """ set wireless channel """
        #self._connection.set_wireless_channel(adapter,
        #                                     self.wireless_channel_number,
        #                                     self.wireless_channel_frequency )
        #print("Wireless Channel changed")
        #return True
        if self._verbose > 0:
            print("Set Wireless Channel is not Implemented.")
        return False

    def start_capture(self):
        """ Start the indicated capture. """
        if not self.capture_name:
            self.logger.error("Invalid or missing capture name (-n).")
            return False

        if not self.is_connected():
            self.logger.error("Start Capture: no connection")
            return False

        # Does the capture already exist?
        self.capture = self._engine.find_capture(self.capture_name)
        if self.capture is None:
            self.logger.error("The capture does not exist: %s.", self.capture_name)
            return False

        if self.capture.is_capturing():
            if self._verbose > 0:
                print("the capture is already capturing: %s." % self.capture_name)
            return True
           
        # Otherwise start capturing.
        self.capture.start()

        # Recheck the status of the capture.
        self.capture.refresh()
        if not self.capture.is_capturing():
            self.logger.error("Failed to start capturing on capture: %s", self.capture_name)
            return False

        if self._verbose > 0:
            print("The capture is capturing: %s." % self.capture_name)
        return True

    def stop_capture(self):
        """ Stop the indicated capture. """
        if not self.capture_name:
            self.logger.error("Invalid or missing capture name (-n).")
            return False

        if not self.is_connected():
            self.logger.error("Stop Capture: no connection")
            return False

        # Does the capture already exist?
        self.capture = self._engine.find_capture(self.capture_name)
        if self.capture is None:
            self.logger.error("The capture does not exist: %s.", self.capture_name)
            return False

        if not self.capture.is_capturing():
            if self._verbose > 0:
                print("The capture is already idle: %s." % self.capture_name)
            return True

        self.capture.stop()
       
        # Recheck the status of the capture.
        self.capture.refresh()
        if self.capture.is_capturing():
            self.logger.error("Failed to stop the capture: %s", self.capture_name)
            return False

        if self._verbose > 0:
            print("The capture is idle: %s." % self.capture_name)
        return True

    def summary_stats(self):
        """ Display the summary statistics for the current capture. """
        if not self.capture_name:
            self.logger.error("Invalid or missing capture name (-n).")
            return False
       
        if not self.is_connected():
            self.logger.error("Summary Stats: no connection")
            return False

        self.capture = self._engine.find_capture(self.capture_name)
        if self.capture is None:
            self.logger.error("Did not find the capture: %s", self.capture_name)
            return False

        summary_stats = self.capture.get_summary_stats()
        if summary_stats is None:
            self.logger.error("Failed to parse summary stats for capture: %s", self.capture_name)
            return False

        if self.csv:
            format = "%s,%s,%s,%s,%s"
        else:
            print("\n\nSummary Statistics for capture '%s':" % self.capture_name)
            format = "%-50s %16s %16s %16s"

        groups = list(g for g, gg in summary_stats.iteritems())
        groups.sort()
        for g in groups:
            if not self.csv:
                print("\n" + format % (g, "Packets", "Bytes", "Value"))
                print("-" * 101)
            for k, stat in summary_stats[g].iteritems():
                packets, bytes, value = stat.packets_bytes_value()
                print(format % (stat.name, packets, bytes, value))
        return True

   
def main(args):
    """ Parse and process the command line arguments. """
    validate_platform()

    nResult = 0
    capture_mgr = CCaptureMgr()

    capture_mgr.parse_arguments(args);
    nResult = capture_mgr.process()

    capture_mgr.clean_up()
    return nResult


# ----------------------------------------------------------------------------
if __name__ == "__main__":
    args = sys.argv[1:]
    main(args)
