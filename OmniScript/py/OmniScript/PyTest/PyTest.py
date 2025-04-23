"""OmniScript 2.0 Python Test Script.
"""
#Copyright (c) Savvius, Inc. 2013-2018. All rights reserved.

# pylint: disable-msg=w0614

import os
import sys
import time
import urllib2
import struct
import tempfile
import logging
import omniscript

import xml.etree.ElementTree as ET

from omni_printers import *


platform = 'lh'

port = None
domain = None

if platform == 'lh':
    host = 'localhost'
    auth = None
    user = None
    pwd = None
    if sys.platform == 'linux2':
        user = "gary"

enable_test = True
#enable_test = False

test_legacy_connect = enable_test
unit_tests = enable_test
test_gets = enable_test
test_filters = enable_test
test_delete_all = enable_test
test_file_ops = enable_test
test_capt_create = enable_test
test_file_adapter = enable_test
test_packets = enable_test
test_capture_stats = enable_test
test_reports = enable_test
test_forensic_template = enable_test
test_forensic_stats = enable_test
test_miscellaneous = enable_test
test_diagnostics = enable_test
test_flows = enable_test
test_select_related = enable_test
test_audit_log = enable_test
test_event_log = enable_test
test_acl = enable_test
test_wireless = enable_test

# Last main test: True False

test_tcpdump_adapter = False
test_lauren_test = False

# Restart is broken
test_restart = False

_file_path = os.path.dirname(__file__)
packet_file_name = 'fortworth101.pkt'
shared_files = os.path.normpath(os.path.join(_file_path, r'../../../files'))
if not os.path.exists(os.path.join(shared_files, packet_file_name)):
    shared_files = os.path.normpath(os.path.join(_file_path, r'../../files'))
    if not os.path.exists(os.path.join(shared_files, packet_file_name)):
        shared_files = os.path.normpath(os.path.join(_file_path, r'../files'))
        if not os.path.exists(os.path.join(shared_files, packet_file_name)):
            shared_files = os.path.normpath(os.path.join(_file_path, r'files'))
            if not os.path.exists(os.path.join(shared_files, packet_file_name)):
                print('shared_files path not found.')
                sys.exit(1)
temp_path = tempfile.gettempdir() if sys.platform != "win32" else r"C:\Temp"
capture_name = 'Python Test Script'
adapter_capt_name = capture_name + ' - Fortworth'
save_all_name_engine = 'PythonTestScript_Engine.pkt'
save_all_name_capture = 'PythonTestScript_Capture.pkt'
filter_name = 'Python-Simple'
forensic_name = 'Python Forensic'
packet_file_name = 'fortworth101.pkt'
packet_ipv6_file_name = 'More IPv6.pkt'
packets_needed = 3190
result_file = 'results.pkt'
fs_refresh_count = 10
report_name = 'Stats Report Test'
report_file = 'stats_report_test.pkt'
report_count = 2921

fortworth_capture = [capture_name, packet_file_name, packets_needed]
ipv6_capture = [capture_name, packet_ipv6_file_name, 710]

node_report = ("Report_Nodes-ip_py.txt", "Report_Nodes-ipv6_py.txt", "Report_Nodes-eth_py.txt")
protocol_report = "Report_Protocols_py.txt"
summary_report = "Report_Summary_py.txt"

node_columns = [
    omniscript.NODE_COLUMN_BYTES_SENT,
    omniscript.NODE_COLUMN_BYTES_RECEIVED,

    omniscript.NODE_COLUMN_PACKETS_SENT,
    omniscript.NODE_COLUMN_PACKETS_RECEIVED,
    omniscript.NODE_COLUMN_BROADCAST_PACKETS,
    omniscript.NODE_COLUMN_BROADCAST_BYTES,
    omniscript.NODE_COLUMN_MULTICAST_PACKETS,
    omniscript.NODE_COLUMN_MULTICAST_BYTES,
    omniscript.NODE_COLUMN_MIN_SIZE_SENT,
    omniscript.NODE_COLUMN_MAX_SIZE_SENT,
    omniscript.NODE_COLUMN_MIN_SIZE_RECEIVED,
    omniscript.NODE_COLUMN_MAX_SIZE_RECEIVED,
    omniscript.NODE_COLUMN_FIRST_TIME_SENT,
    omniscript.NODE_COLUMN_LAST_TIME_SENT,
    omniscript.NODE_COLUMN_FIRST_TIME_RECEIVED,
    omniscript.NODE_COLUMN_LAST_TIME_RECEIVED
    ]

SECONDS_IN_A_MINUTE = 60
MINUTES_IN_A_HOUR = 60
SECONDS_IN_A_HOUR = SECONDS_IN_A_MINUTE * MINUTES_IN_A_HOUR
HOURS_IN_A_DAY = 24
SECONDS_IN_A_DAY = SECONDS_IN_A_HOUR * HOURS_IN_A_DAY

BYTES_IN_A_KILOBYTE = 1024
BYTES_IN_A_MEGABYTE = 1024 * 1024
BYTES_IN_A_GIGABYTE = 1024 * 1024 * 1024

E_TIMEOUT = 0x800705B4

BS = "\\"

use_file_db = True
try:
    OMNI_USEFILEDB = os.environ['OMNI_USEFILEDB']
    if len(OMNI_USEFILEDB) > 0:
        use_file_db = int(OMNI_USEFILEDB) != 0
except KeyError as err:
    pass


def connect_to_engine(engine, auth, domain, user, pwd):
    if not engine:
        return False
    if not engine.is_connected():
        result = engine.connect(auth, domain, user, pwd)
        if not result:
            return False
        return engine.is_connected()
    return True


def get_default_adapter(engine):
    al = engine.get_adapter_list()
    if len(al) == 1:
        return al[0]
    hp = engine.get_host_platform()
    if hp.os == omniscript.OPERATING_SYSTEM_WINDOWS:
        return omniscript.find_adapter(al, 'Local Area Connection')
    if hp.os == omniscript.OPERATING_SYSTEM_LINUX:
        return None
    return None


def create_capture(engine, capture_name, capture_file, packets_needed):
    time.sleep(1)
    failures = 0
    capt = None
    try:
        fl = engine.get_file_list()
        ffi = list(i for i in fl if i.name == capture_file)
        if len(ffi) == 0:
            sf_filename = os.path.normpath(os.path.join(shared_files, capture_file))
            engine.send_file(sf_filename)
            fl = engine.get_file_list()
            ffi = list(i for i in fl if i.name == capture_file)
            if len(ffi) == 0:
                print('*** Failed to send file to engine: %s' % capture_file)
                return None
    except:
        failures += 1

    try:
        cl = engine.get_capture_list()
        cli = list(i for i in cl if i.name == capture_name)
        engine.delete_capture(cli)
    except:
        failures += 1

    try:
        cf_path = fl[0].name + capture_file # do not use os.path.join.
        fa = omniscript.FileAdapter(cf_path)
        fa.limit = 1
        fa.speed = 0.0

        ct = omniscript.CaptureTemplate()
        ct.set_adapter(fa)
        ct.general.name = capture_name
        ct.general.option_continuous_capture = True
        ct.general.option_capture_to_disk = True
        ct.general.option_start_capture = False
        ct.set_all(True)
        capt = engine.create_capture(ct)
        if capt is None:
            return None
    except:
        failures += 1

    try:        
        capt.start()
        capt.refresh()
        while (capt.status & 0x0001) and (capt.packets_filtered < packets_needed):
            time.sleep(3)
            capt.refresh()
    except:
        failures += 1
    
    try:
        capt.stop()
        capt.refresh()
    except:
        failures += 1
        
    return capt


def query_expert_counts(engine):
    failures = 0

    try:
        ft = omniscript.ForensicTemplate()
        ft.name = 'ForensicSearch-Expert'
        ft.option_expert = True
        ft.option_packets = True
        ft.option_summary = True

        fs = engine.create_forensic_search(ft)

        query = omniscript.ExpertQuery('HEADER_COUNTERS')
        results = fs.query_expert(query)
        print_expert_result_list(results)
    except:
        failures += 1
    return failures


def query_expert_short(engine, context):
    failures = 0

    if not context:
        return 1

    try:
        query = [omniscript.ExpertQuery('STREAM'), omniscript.ExpertQuery('EVENT_LOG')]
        query[0].columns = ['STREAM_ID', 'CLIENT_ADDRESS', 'CLIENT_PORT', 'SERVER_ADDRESS', 'SERVER_PORT']
        query[0].order = ['STREAM_ID']
        query[0].row_count = 100

        query[1].columns = ['PROBLEM_ID', 'STREAM_ID', 'MESSAGE']
        query[1].order = ['PROBLEM_ID']
        query[1].where = ['informational', 'minor', 'major', 'severe']
        query[1].row_count = 100

        results = context.query_expert(query)
        for r in results[0].rows:
            cip = omniscript.parse_ip_address(r['CLIENT_ADDRESS'])
            sip = omniscript.parse_ip_address(r['SERVER_ADDRESS'])
            print('%d [%s]:%d [%s]:%d' % (r['STREAM_ID'], str(cip), r['CLIENT_PORT'], str(sip), r['SERVER_PORT']))
        for r in results[1].rows:
            print('%3d %3d %s' % (r['PROBLEM_ID'], r['STREAM_ID'], r['MESSAGE']))
        print
    except:
        failures += 1

    return failures


def query_expert_long(engine, context):
    failures = 0

    if not context:
        return 1

    try:
        # 4 Queries Peek Makes when updating Expert - Flows.
        q_stream = [
            omniscript.ExpertQuery('STREAM'),
            omniscript.ExpertQuery('EVENT_LOG'),
            omniscript.ExpertQuery('EVENT_COUNTS'),
            omniscript.ExpertQuery('HEADER_COUNTERS')
            ]
        q_stream[0].columns = [
            'TYPE',
            'HIGHLIGHT',
            'TREE_STATE',
            'EVENT_SEVERITY_MAX',
            'EVENT_TIME',
            'START_TIME',
            'END_TIME',
            'NODEPAIR',
            'FLOW_TYPE',
            'STREAM_ID',
            'PROBLEM_ID',
            'CLIENT_PORT',
            'SERVER_PORT',
            'THROUGHPUT_CLIENT_TO_SERVER_BITS_PER_SECOND',
            'THROUGHPUT_SERVER_TO_CLIENT_BITS_PER_SECOND',
            'THROUGHPUT_CLIENT_TO_SERVER_BITS_PER_SECOND_BEST',
            'THROUGHPUT_SERVER_TO_CLIENT_BITS_PER_SECOND_BEST',
            'THROUGHPUT_CLIENT_TO_SERVER_BITS_PER_SECOND_WORST',
            'THROUGHPUT_SERVER_TO_CLIENT_BITS_PER_SECOND_WORST',
            'THROUGHPUT_CLIENT_TO_SERVER_SAMPLE_COUNT',
            'THROUGHPUT_SERVER_TO_CLIENT_SAMPLE_COUNT',
            'CLIENT_START_TIME',
            'SERVER_START_TIME',
            'CLIENT_END_TIME',
            'SERVER_END_TIME',
            'CLIENT_HOP_COUNT',
            'SERVER_HOP_COUNT',
            'CLIENT_TCP_WINDOW_MIN',
            'SERVER_TCP_WINDOW_MIN',
            'CLIENT_TCP_WINDOW_MAX',
            'SERVER_TCP_WINDOW_MAX',
            'PROBLEM_SUMMARY_LIST',
            'CLIENT_ADDRESS',
            'SERVER_ADDRESS',
            'CLIENT_SENT_PACKET_COUNT',
            'SERVER_SENT_PACKET_COUNT',
            'CLIENT_SENT_BYTE_COUNT',
            'SERVER_SENT_BYTE_COUNT',
            'DELAY',
            'DELAY_BEST',
            'DELAY_WORST',
            'DELAY_SAMPLE_COUNT',
            'PROBLEM_COUNT',
            'PROTOCOL',
            'APPLICATION',
            'PACKET_COUNT',
            'BYTE_COUNT',
            'DURATION',
            'THREE_WAY_HANDSHAKE_TIME',
            'TCP_STATUS'
            ]
        q_stream[0].order = ['STREAM_ID', 'CLIENT_ADDRESS', 'PROBLEM_ID']
        q_stream[0].row_count = 72

        q_stream[1].columns = [
            'TYPE',
            'PROBLEM_ID',
            'STREAM_ID',
            'EVENT_SERIAL_NUMBER',
            'EVENT_SEVERITY_MAX',
            'EVENT_TIME',
            'PROTOCOL_LAYER',
            'MESSAGE',
            'SOURCE_ADDRESS',
            'DEST_ADDRESS',
            'SOURCE_PORT',
            'DEST_PORT',
            'PACKET_NUMBER',
            'OTHER_PACKET_NUMBER',
            'IS_FROM_CLIENT',
            'NODEPAIR',
            'PROTOSPEC',
            'REQUEST_ID',
            'CALL_ID',
            'FLOW_INDEX'
            ]
        q_stream[1].order = ['EVENT_TIME', 'PACKET_NUMBER', 'EVENT_SERIAL_NUMBER']
        q_stream[1].where = ['informational', 'minor', 'major', 'severe']
        q_stream[1].row_count = 24

        q_stream[2].columns = [
            'PROBLEM_ID',
            'EVENT_NAME',
            'PROBLEM_COUNT',
            'EVENT_SEVERITY_MAX',
            'PROTOCOL_LAYER',
            'START_TIME',
            'END_TIME'
            ]
        q_stream[2].order = ['PROBLEM_ID']
        q_stream[2].row_count = sys.maxsize
        q_stream[2].view_settings.time_precision = 'nanoseconds'
        q_stream[2].view_settings.option_address_names = False
        q_stream[2].view_settings.option_port_names = False

        q_stream[3].row_count = sys.maxsize
        q_stream[3].view_settings.time_precision = 'nanoseconds'
        q_stream[3].view_settings.option_address_names = False
        q_stream[3].view_settings.option_port_names = False

        if False:
            results = context.query_expert(q_stream)
            rs = next((r for r in results if r.table == 'STREAM'), None)
            if rs:
                for r in rs.rows:
                    cip = omniscript.parse_ip_address(r['CLIENT_ADDRESS'])
                    sip = omniscript.parse_ip_address(r['SERVER_ADDRESS'])
                    print('%d %s:%d %s%d' % (r['STREAM_ID'], str(cip), r['CLIENT_PORT'], str(sip), r['SERVER_PORT']))
        print
    except:
        failures += 1
    return failures


def do_unit_tests(engine):
    print('Unit Test')
    failures = 0

    # Testing OmniId
    id = omniscript.OmniId()
    id.new()
    txt = str(id)
    print('New id: %s' % txt)
    idp = omniscript.OmniId()
    idp.parse(txt)
    if id != idp:
        print('OmniId new/parse failure.')
        failures += 1

    # Testing PeekTime
    pt1 = omniscript.PeekTime()
    time.sleep(2)
    pt2 = omniscript.PeekTime()
    df = pt2 - pt1
    if df.value > (10 * 1000000000):
        print('*** Check PeekTime subtraction.')
        failures += 1

    # Check for new Expert Events.
    id_expert_names = omniscript.get_id_expert_names()
    id_stat_names = omniscript.get_id_stat_names()
    missing = []
    for k in id_expert_names.iterkeys():
        if not id_stat_names.has_key(k):
            missing.append(id_expert_names[k])
    if len(missing) > 0:
        failures += 1

    # Testing start/stop capture's xml_capture_list method.
    # fl = engine.get_filter_list()
    cl = engine.get_capture_list()
    captures = [i for i in cl if i.name[:7] == "Capture"]
    engine.delete_capture(captures)

    adpt = get_default_adapter(engine)

    ct = omniscript.CaptureTemplate()
    if adpt:
        ct.set_adapter(adpt)
    ct.general.option_continuous_capture = True
    ct.general.option_capture_to_disk = True
    ct.general.option_start_capture = False
    ct.general.option_timeline_app_stats = True
    ct.general.option_timeline_stats = True
    ct.general.option_timeline_top_stats = True
    ct.general.option_timeline_voip_stats = True
    ct.analysis.option_alarms = True
    ct.analysis.option_analysis_modules = True
    ct.analysis.option_application = True
    ct.analysis.option_compass = True
    ct.analysis.option_country = True
    ct.analysis.option_error = True
    ct.analysis.option_expert = True
    ct.analysis.option_network = True
    ct.analysis.node_protocol_detail_limit.enabled = True
    ct.analysis.node_limit.enabled = True
    ct.analysis.protocol_limit.enabled = True
    ct.analysis.option_size = True
    ct.analysis.option_summary = True
    ct.analysis.option_traffic = True
    ct.analysis.option_voice_video = True

    ct.general.name = "Capture 1"
    engine.create_capture(ct)
    ct.general.name = "Capture 2"
    engine.create_capture(ct)
    ct.general.name = "Capture 3"
    engine.create_capture(ct)

    cl = engine.get_capture_list()
    captures = [i for i in cl if i.name[:7] == "Capture"]
    engine.start_capture(captures[1])
    engine.start_capture(captures)
    engine.stop_capture(captures[0])
    engine.stop_capture(captures)
    engine.delete_capture(captures[2])
    engine.delete_capture(captures)

    # testing filter caching.
    fl1 = engine.get_filter_list()
    # fl1 is the cached list.
    fl2 = engine.get_filter_list(False)  # Should not refresh
    if fl1[0] != fl2[0]:
        print('*** Get Filter List cache failure.')
        failures += 1
    engine.set_filter_list_timeout(5)
    time.sleep(10)
    fl3 = engine.get_filter_list(False)  # Should refresh
    if fl1[0] == fl3[0]:
        print('*** Get Filter List cache failure.')
        failures += 1
    # fl3 is the cached list. fl1 and fl2 are stale.
    engine.set_filter_list_timeout(120)
    fl4 = engine.get_filter_list()  # Should refresh
    if fl3[0] == fl4[0]:
        print('*** Get Filter List force refresh failure.')
        failures += 1
    # fl4 is the cached list. fl1, fl2 and fl3 are stale.

    band_id_names = omniscript.get_wireless_band_id_names()
    for k, v in band_id_names.items():
        print(k, v)

    return failures


def do_gets(engine):
    print('Gets Test')
    failures = 0

    csl = engine.get_capture_session_list()
    print_capture_session_list(csl)
    if len(csl) > 0:
        engine.delete_capture_session(csl[0])

    es = engine.get_status()
    print_engine_status(es)

    es.refresh()
    print('Refresh Engine Status')
    print_engine_status(es)

    al = engine.get_adapter_list()
    print_adapter_list(al)
    if len(al) > 0:
        a = engine.find_adapter(al[0].name)
        if not a:
            print('*** Failed to find first adapter by name.')
            failures += 1
        a = engine.find_adapter(al[0].id, 'id')
        if not a:
            print('*** Failed to find first adapter by id.')
            failures += 1
        if len(al[0].device_name) > 0:
            a = engine.find_adapter(al[0].device_name, 'device_name')
            if not a:
                print('*** Failed to find first adapter by device name.')
                failures += 1

    alarms = engine.get_alarm_list()
    print_alarm_list(alarms)

    aml = engine.get_analysis_module_list()
    print_analysis_module_list(aml)

    cl = engine.get_capture_list()
    print_capture_list(cl)
    if len(cl) > 0:
        c = engine.find_capture(cl[0].name)
        if not c:
            print('*** Failed to find first capture by name.')
            failures += 1
        c = engine.find_capture(cl[0].id, 'id')
        if not c:
            print('*** Failed to find first capture by id.')
            failures += 1

    for c in cl:
        ct = engine.get_capture_template(c)
        print_capture_template(ct)

    fl = engine.get_filter_list()
    print_filter_list(fl)
    if len(fl) > 0:
        f = engine.find_filter(fl[0].name)
        if not f:
            print('*** Failed to find first filter by name.')
            failures += 1
        f = engine.find_filter(fl[0].id, 'id')
        if not f:
            print('*** Failed to find first filter by id.')
            failures += 1

    ffl = engine.get_forensic_file_list()
    print_forensic_file_list(ffl)

    fsl = engine.get_forensic_search_list()
    print_forensic_search_list(fsl)
    if len(fsl) > 0:
        fs = engine.find_forensic_search(fsl[0].name)
        if not fs:
            print('*** Failed to find first forensic search by name.')
            failures += 1
        fs = engine.find_forensic_search(fsl[0].id, 'id')
        if not fs:
            print('*** Failed to find first forensic search by id.')
            failures += 1
    if platform == 'l':
        engine.get_diagnostics_info('default')

    gtl = engine.get_graph_template_list()
    print_graph_template_list(gtl)

    al = engine.get_audit_log(0, 20)
    print_audit_log(al)
    al.get_next(10)
    print_audit_log(al)
#    al.get_next()
#    print_audit_log(al)
    return failures


def do_filters(engine, filter_name):
    print("Filters Test")
    failures = 0

    filters_xml = os.path.normpath(os.path.join(shared_files, 'filters.xml'))
    filters_flt = os.path.normpath(os.path.join(shared_files, 'filters.flt'))

    bin_filters = omniscript.read_filter_file(filters_xml)

    fl = engine.get_filter_list()
    fli = list(i for i in fl if i.name == filter_name)
    engine.delete_filter(fli)

    simple_name = 'Python-Simple'
    address_name = 'Python-Address'
    wildcard_name = 'Python-Wildcard'
    vlan_mpls_name = 'Python-VLAN-MPLS'
    complex_name = 'Python-Complex'
    filter_names = [simple_name, address_name, wildcard_name, vlan_mpls_name, complex_name]

    email_capture = '{A090DCAD-ACA7-4734-9F1C-FA7DD153F501}'

    # Delete Python Filters
    for name in filter_names:
        fli = list(i for i in fl if i.name == name)
        for f in fli:
            engine.delete_filter(f)

    # Simple Filter
    simple = omniscript.Filter(simple_name)
    simple.comment = 'Python created filter.'
    addr_node = omniscript.AddressNode()
    addr_node.address_1 = omniscript.IPv4Address('1.2.3.4')
    addr_node.address_2 = omniscript.IPv4Address('0.0.0.0', mask='0')
    addr_node.accept_1_to_2 = True
    addr_node.accept_2_to_1 = True
    simple.criteria= addr_node
    engine.add_filter(simple)
    print_filter(simple)
    print(simple.to_string(0))

    # Address Filter
    address = omniscript.Filter(address_name)
    address.comment = 'Python created filter.'
    addr_node = omniscript.AddressNode()
    addr_node.address_1 = omniscript.EthernetAddress('00:50:56:97:3a:6c')
    addr_node.address_2 = omniscript.EthernetAddress('00:50:56:97:4b:7d')
    addr_node.accept_1_to_2 = True
    addr_node.accept_2_to_1 = True
    address.criteria= addr_node
    engine.add_filter(address)
    print_filter(address)
    print(address.to_string(0))

    # Wildcard Filter
    wildcard = omniscript.Filter(wildcard_name)
    wildcard.comment = 'Python created filter.'
    addr_node = omniscript.AddressNode()
    addr_node.address_1 = omniscript.EthernetAddress('00:14:2F:*:*:*')
    addr_node.accept_1_to_2 = True
    addr_node.accept_2_to_1 = True
    wildcard.criteria= addr_node
    engine.add_filter(wildcard)
    print_filter(wildcard)
    print(wildcard.to_string(0))

    # VLAN MPLS Filter
    vlan_mpls = omniscript.Filter(vlan_mpls_name)
    vlan_mpls.comment = 'Python created filter.'
    vlan_mpls_node = omniscript.VlanMplsNode()
    #vlan_mpls_node.add_id('80')
    #vlan_mpls_node.add_id('100-110')
    vlan_mpls_node.add_ids(80, '100-110')
    #vlan_mpls_node.add_label('200-220')
    #vlan_mpls_node.add_label('1024')
    vlan_mpls_node.add_labels('200-220', 1024)
    vlan_mpls.criteria = vlan_mpls_node
    engine.add_filter(vlan_mpls)
    print_filter(vlan_mpls)
    print(vlan_mpls.to_string(0))

    # Complex Filter
    complex = omniscript.Filter(complex_name)
    complex.comment = 'Python created filter.'

    addr_node = omniscript.AddressNode()
    addr_node.address_1 = omniscript.IPv4Address('1.2.3.4')
    addr_node.address_2 = omniscript.IPv4Address('0.0.0.0')
    addr_node.accept_1_to_2 = True
    addr_node.accept_2_to_1 = True

    protocol_node = omniscript.ProtocolNode()
    protocol_node.set_protocol('HTTP')

    port_node = omniscript.PortNode()
    port_node.port_1 = 80
    port_node.accept_1_to_2 = True
    port_node.accept_2_to_1 = True

    pattern_node = omniscript.PatternNode()
    pattern_node.set_pattern('12345678')
    pattern_node.start_offset = 64
    pattern_node.end_offset = 1024

    value_node = omniscript.ValueNode()
    value_node.value = 8675309

    length_node = omniscript.LengthNode()
    length_node.minimum = 64
    length_node.maximum = 1024

    wireless_node = omniscript.WirelessNode()
    wireless_node.channel_band = omniscript.WIRELESS_BAND_ALL
    wireless_node.channel_number = 1
    wireless_node.channel_frequency = 2412
    wireless_node.data_rate = 11.0
    wireless_node.flags = \
        omniscript.WIRELESS_FLAG_20MHZ_LOWER | \
        omniscript.WIRELESS_FLAG_20MHZ_UPPER | \
        omniscript.WIRELESS_FLAG_40MHz
    wireless_node.signal_minimum = 20
    wireless_node.signal_maximum = 80
    wireless_node.noise_minimum = 0
    wireless_node.noise_maximum = 10

    channel_node = omniscript.ChannelNode()
    channel_node.channel = 3

    error_node = omniscript.ErrorNode()
    error_node.crc_errors = True
    error_node.frame_errors = True
    error_node.oversize_errors = True
    error_node.runt_errors = True

    plugin_node = omniscript.PluginNode()
    plugin_node.add_analysis_module(email_capture)

    wan_node = omniscript.WANDirectionNode()
    wan_node.direction = omniscript.WAN_DIRECTION_TO_DTE

    vlan_mpls_node = omniscript.VlanMplsNode()
    vlan_mpls_node.add_id(1024)
    vlan_mpls_node.add_label(8888)

    tcpdump_node = omniscript.BpfNode()
    tcpdump_node.filter = 'port(8080)'

    complex.criteria= addr_node
    addr_node.and_node = protocol_node
    addr_node.or_node = pattern_node
    pattern_node.or_node = value_node
    value_node.and_node = length_node
    length_node.and_node = wireless_node
    length_node.or_node = channel_node
    channel_node.and_node = error_node
    error_node.and_node = plugin_node
    error_node.or_node = wan_node
    plugin_node.and_node = vlan_mpls_node
    plugin_node.or_node = tcpdump_node

    engine.add_filter(complex)
    print_filter(complex)
    print(complex.to_string(0))

    try:
        fl = engine.get_filter_list()
        engine.delete_filter(fl)
        if len(fl) > 0:
            engine.add_filter(fl[0])
            engine.delete_filter(engine.get_filter_list())

        console_filters = omniscript.read_filter_file(filters_flt)
        print_filter_list(console_filters)
        for flt in console_filters:
            try:
                engine.add_filter(flt)
            except:
                print('Failed to add filter: %s' % flt.name)
        engine.delete_filter(engine.get_filter_list())
        engine.add_filter(console_filters)

        engine_filters = omniscript.read_filter_file(filters_xml)
        print_filter_list(engine_filters)
        for flt in engine_filters:
            try:
                engine.add_filter(flt)
            except:
                print('Failed to add filter: %s' % flt.name)
        engine.delete_filter(engine.get_filter_list())
        engine.add_filter(engine_filters)

    finally:
        engine.delete_filter(engine.get_filter_list())
        engine.add_filter(bin_filters)

    return failures


def do_delete_all(engine):
    print("Delete All Test")
    failures = 0

    engine.delete_all_capture_sessions()
    cl = engine.get_capture_list()
    engine.delete_capture(cl)
    fsl = engine.get_forensic_search_list()
    engine.delete_forensic_search(fsl)
    ffl = engine.get_forensic_file_list()
    engine.delete_file(ffl)

    tcl = engine.get_capture_list()
    if len(tcl) > 0:
        print('*** Failed to delete all captures.')
        failures += 1
    fsl = engine.get_forensic_search_list()
    if len(fsl) > 0:
        print('*** Failed to delete all Forensic Searches.')
        failures += 1
    tffl = engine.get_forensic_file_list()
    if len(tffl) > 0:
        print('*** Failed to delete all Forensic Files.')
        failures += 1
    return failures


def do_file_ops(engine, file_name, result_file):
    print("File Operations Test")
    failures = 0

    fl = engine.get_file_list()
    folder_count = 0
    for f in fl:
        if f.is_folder():
            folder_count += 1
    ffi = list(i for i in fl if i.name == file_name)
    for f in ffi:
        engine.delete_file(f.name)
    fl = engine.get_file_list()
    ffi = list(i for i in fl if i.name == file_name)
    if len(ffi) != 0:
        print('*** Failed to delete file from engine.')
        failures += 1
    sf_filename = os.path.normpath(os.path.join(shared_files, file_name))
    engine.send_file(sf_filename)
    fl = engine.get_file_list()
    ffi = list(i for i in fl if i.name == file_name)
    if len(ffi) == 0:
        print('*** Failed to send file to engine.')
        failures += 1
    engine.get_file(file_name, result_file)
    engine.delete_file(file_name)
    return failures


def do_capt_create(engine, capture_name, filter_name, needed_packets):
    print("Capture Creation Test")
    failures = 0

    capt = engine.find_capture(capture_name)
    while capt:
        engine.delete_capture(capt)
        capt = engine.find_capture(capture_name)

    gtl = engine.get_graph_template_list()
    gtl_sublist = []
    for i, v in enumerate(gtl):
        if i % 2:
            gtl_sublist.append(v)

    adpt = get_default_adapter(engine)

    ca = omniscript.CaptureTemplate()
    if adpt:
        ca.set_adapter(adpt)
    ca.general.name = capture_name
    ca.general.buffer_size = 10 * BYTES_IN_A_MEGABYTE
    ca.general.comment = "Python generated capture."
    ca.general.directory = os.path.join(temp_path, "Capture Files")
    ca.general.file_pattern = capture_name + '-'
    ca.general.file_size = 128 * BYTES_IN_A_MEGABYTE
    ca.general.keep_last_files_count = 5
    ca.general.max_file_age = SECONDS_IN_A_HOUR
    ca.general.max_total_file_size = BYTES_IN_A_GIGABYTE
    ca.general.slice_length = 256
    ca.general.tap_timestamps = omniscript.TAP_TIMESTAMPS_DEFAULT
    ca.general.option_capture_to_disk = True
    ca.set_all(True)
    ca.indexing.set_all(True)
    ca.plugins.set_all(True)
    ca.plugins.modules = []
    ca.filter.mode = omniscript.FILTER_MODE_ACCEPT_MATCHING_ANY
    ca.add_filter(filter_name)

    ca.general._GeneralSettings__apollo_capture_in_xml = True
    ca.general._option_apollo_capture = True

    print_capture_template(ca)

    capt = engine.create_capture(ca)
    if capt is None:
        print('Failed to create capture.')
        failures += 1
        return failures
    print_capture(capt)

    gca = capt.get_capture_template()
    gca.analysis.set_all(False)
    gca.plugins.set_all(False)
    gca.indexing.set_all(False)
    capt.modify(gca)
    capt.refresh()
    print_capture(capt)

    gca = capt.get_capture_template()
    gca.indexing.option_ipv4 = True
    capt.modify(gca)
    capt.refresh()
    tca = capt.get_capture_template()
    if not tca.indexing.option_ipv4:
        failures += 1
    if tca.indexing.option_application or tca.indexing.option_country or \
            tca.indexing.option_ethernet or tca.indexing.option_ipv6 or \
            tca.indexing.option_mpls or tca.indexing.option_port or \
            tca.indexing.option_protospec or tca.indexing.option_vlan:
        failures += 1
    print_capture(capt)

    engine.delete_capture(capt)
    capt = None

    capts = []
    for c in range(17):
        cx = omniscript.CaptureTemplate()
        if adpt:
            cx.set_adapter(adpt)
        cx.general.name = "%s - %2d" % (capture_name, c)
        cx.general.buffer_size = 10 * BYTES_IN_A_MEGABYTE
        cx.general.comment = "Python generated capture."
        cx.general.directory = os.path.join(temp_path, "Capture Files")
        cx.general.file_pattern = capture_name + '-'
        cx.general.file_size = 128 * BYTES_IN_A_MEGABYTE
        cx.general.keep_last_files_count = 5
        cx.general.max_file_age = SECONDS_IN_A_HOUR
        cx.general.max_total_file_size = BYTES_IN_A_GIGABYTE
        cx.general.slice_length = 256
        cx.general.tap_timestamps = omniscript.TAP_TIMESTAMPS_DEFAULT
        cx.general.option_capture_to_disk = True
        cx.general.option_continuous_capture = True
        cx.general.option_deduplicate = True
        cx.general.option_file_age = True
        cx.general.option_keep_last_files = True
        cx.general.option_priority_ctd = True
        cx.general.option_save_as_template = True
        cx.general.option_slicing = True
        cx.general.option_start_capture = False
        cx.general.option_timeline_app_stats = True
        cx.general.option_timeline_stats = True
        cx.general.option_timeline_top_stats = True
        cx.general.option_timeline_voip_stats = True
        cx.general.option_total_file_size = True

        cx.analysis.option_alarms = (c < 1)
        cx.analysis.option_analysis_modules = (c < 2)
        cx.analysis.option_application = (c < 3)
        cx.analysis.option_compass = (c < 4)
        cx.analysis.option_country = (c < 5)
        cx.analysis.option_error = (c < 6)
        cx.analysis.option_expert = (c < 7)
        cx.analysis.option_network = (c < 8)
        cx.analysis.node_protocol_detail_limit.enabled = (c < 9)
        cx.analysis.node_limit.enabled = (c < 10)
        cx.analysis.protocol_limit.enabled = (c < 11)
        cx.analysis.option_size = (c < 12)
        cx.analysis.option_summary = (c < 13)
        cx.analysis.option_traffic = (c < 14)
        cx.analysis.option_voice_video = (c < 15)

        obj = cx.analysis.node_protocol_detail_limit.to_xml()
        t = ET.tostring(obj)
        print(t)

        cx.graphs.enabled = True
        cx.graphs.interval = 10
        cx.graphs.file_count = 5
        cx.graphs.file_buffer_size = 20
        cx.graphs.hours_to_keep = 4
        cx.graphs.option_preserve_files = True
        cx.graphs.graphs = gtl_sublist
    
        cx.filter.mode = omniscript.FILTER_MODE_ACCEPT_MATCHING_ANY
        cx.add_filter(filter_name)

        cx.start_trigger.enabled = True
        cx.start_trigger.option_notify = True
        cx.start_trigger.severity = omniscript.SEVERITY_SEVERE
        cx.start_trigger.option_toggle_capture = True
        cx.start_trigger.captured.enabled = True
        cx.start_trigger.captured.bytes = 10 * 1024 * 1024
        cx.start_trigger.filter.enabled = True
        cx.start_trigger.filter.mode = omniscript.FILTER_MODE_ACCEPT_MATCHING_ALL
        cx.start_trigger.filter.filters = ['FTP']
        cx.start_trigger.time.enabled = True
        cx.start_trigger.time.option_use_elapsed = False
        cx.start_trigger.time.option_use_date = True
        cx.start_trigger.time.time = omniscript.PeekTime().value   # Now

        cx.stop_trigger.enabled = True
        cx.stop_trigger.option_notify = True
        cx.stop_trigger.severity = omniscript.SEVERITY_MAJOR
        cx.stop_trigger.option_toggle_capture = True
        cx.stop_trigger.captured.enabled = True
        cx.stop_trigger.captured.bytes = 1024 * 1024
        cx.stop_trigger.filter.enabled = True
        cx.stop_trigger.filter.mode = omniscript.FILTER_MODE_ACCEPT_MATCHING_ALL
        cx.stop_trigger.filter.filters = ['HTTP']
        cx.stop_trigger.time.enabled = True
        cx.stop_trigger.time.option_use_elapsed = True
        cx.stop_trigger.time.time = omniscript.PeekTime(120 * 1000000000L) # 120 seconds.

        try:
            capt = engine.create_capture(cx)
        except:
            pass

        if capt is None:
            print('Failed to create capture %d.' % (c))
            failures += 1
            continue
        print_capture(capt)
        cx_t = capt.get_capture_template()
        print_capture_template(cx_t)
        capts.append(capt)

    engine.delete_capture(capts)

    ct = omniscript.CaptureTemplate()
    if adpt:
        ct.set_adapter(adpt)
    ct.general.name = capture_name
    ct.general.buffer_size = 10 * BYTES_IN_A_MEGABYTE
    ct.general.comment = "Python generated capture."
    ct.general.directory = os.path.join(temp_path, "Capture Files")
    ct.general.file_pattern = capture_name + '-'
    ct.general.file_size = 128 * BYTES_IN_A_MEGABYTE
    ct.general.keep_last_files_count = 5
    ct.general.max_file_age = SECONDS_IN_A_HOUR
    ct.general.max_total_file_size = BYTES_IN_A_GIGABYTE
    ct.general.slice_length = 256
    ct.general.tap_timestamps = omniscript.TAP_TIMESTAMPS_DEFAULT
    ct.general.option_capture_to_disk = True
    ct.general.option_continuous_capture = True
    ct.general.option_deduplicate = True
    ct.general.option_file_age = True
    ct.general.option_keep_last_files = True
    ct.general.option_priority_ctd = True
    ct.general.option_save_as_template = True
    ct.general.option_slicing = True
    ct.general.option_start_capture = False
    ct.general.option_timeline_app_stats = True
    ct.general.option_timeline_stats = True
    ct.general.option_timeline_top_stats = True
    ct.general.option_timeline_voip_stats = True
    ct.general.option_total_file_size = True

    ct.analysis.option_alarms = True
    ct.analysis.option_analysis_modules = True
    ct.analysis.option_application = True
    ct.analysis.option_compass = True
    ct.analysis.option_country = True
    ct.analysis.option_error = True
    ct.analysis.option_expert = True
    ct.analysis.option_network = True
    ct.analysis.option_size = True
    ct.analysis.option_summary = True
    ct.analysis.option_traffic = True
    ct.analysis.option_voice_video = True
    ct.analysis.node_limit.enabled = True
    ct.analysis.protocol_limit.enabled = True
    ct.analysis.node_protocol_detail_limit.enabled = True

    ct.graphs.enabled = True
    ct.graphs.interval = 10
    ct.graphs.file_count = 5
    ct.graphs.file_buffer_size = 20
    ct.graphs.hours_to_keep = 4
    ct.graphs.option_preserve_files = True
    ct.graphs.graphs = gtl_sublist
    
    ct.filter.mode = omniscript.FILTER_MODE_ACCEPT_MATCHING_ANY
    ct.add_filter(filter_name)

    capt = engine.create_capture(ct)
    if capt is None:
        print('Failed to create capture.')
        failures += 1
        return failures
    print_capture(capt)
    gct = capt.get_capture_template()
    print_capture_template(gct)
    
    print('Start capturing')
    engine.start_capture(capt)
    if not capt.is_capturing():
        print('Failed to start the capture.')
        failures += 1

    sites = ['yahoo', 'google', 'apple', 'digg']
    for site in sites:
        try:
            url = r"http://%s.com" % site
            site_text = urllib2.urlopen(url).read()
            print(site_text)
        except:
            pass
    print('Stop capturing')
    engine.stop_capture(capt)
    if capt.is_capturing():
        print('Failed to stop the capture.')
        failures += 1

    print('Start capturing')
    capt.start()
    if not capt.is_capturing():
        print('Failed to re-start the capture.')
        failures += 1
    print(capt.format_status())
    for site in sites:
        if not capt.is_capturing():
            break
        print('Captured %d packets' % capt.packets_filtered)
        try:
            url = urllib2.urlopen('https://www.%s.com' % site)
        except urllib2.HTTPError:
            pass
    print('Stop capturing')
    capt.stop()
    if capt.is_capturing():
        print('Failed to re-stop the capture.')
        failures += 1
    print(capt.format_status())
    print('Capture has %d packets' % capt.packets_filtered)
    print('Delete the capture: %s' % capture_name)
    engine.delete_capture(capt)
    return failures


def do_file_adapter(engine, file_name, capture_name, needed_packets, save_all_names):
    print("File Adapters Test")
    failures = 0

    fl = engine.get_file_list()
    ffi = list(i for i in fl if i.name == file_name)
    wd = os.getcwd()
    print(wd)
    if len(ffi) == 0:
        sf_filename = os.path.normpath(os.path.join(shared_files, file_name))
        engine.send_file(sf_filename)
        fl = engine.get_file_list()
        ffi = list(i for i in fl if i.name == file_name)
        if len(ffi) == 0:
            print('*** Failed to send file to engine: %s' % file_name)
            failures += 1
            return failures

    cl = engine.get_capture_list()
    cli = list(i for i in cl if i.name == capture_name)
    engine.delete_capture(cli)

    cf_path = fl[0].name + file_name    # do not use os.path.join!
    fa = omniscript.FileAdapter(cf_path)
    fa.limit = 1
    fa.speed = 0.0

    ct = omniscript.CaptureTemplate()
    ct.general.buffer_size = 10 * BYTES_IN_A_MEGABYTE
    ct.general.name = capture_name
    ct.general.option_continuous_capture = True
    ct.general.option_capture_to_disk = True
    ct.general.option_start_capture = False
    ct.general.option_timeline_app_stats = True
    ct.general.option_timeline_stats = True
    ct.general.option_timeline_top_stats = True
    ct.general.option_timeline_voip_stats = True
    ct.analysis.option_alarms = True
    ct.analysis.option_analysis_modules = True
    ct.analysis.option_application = True
    ct.analysis.option_compass = True
    ct.analysis.option_country = True
    ct.analysis.option_error = True
    ct.analysis.option_expert = True
    ct.analysis.option_network = True
    ct.analysis.node_protocol_detail_limit.enabled = True
    ct.analysis.node_limit.enabled = True
    ct.analysis.protocol_limit.enabled = True
    ct.analysis.option_size = True
    ct.analysis.option_summary = True
    ct.analysis.option_traffic = True
    ct.analysis.option_voice_video = True
    ct.stop_trigger.enabled = True
    ct.stop_trigger.time.enabled = True
    ct.stop_trigger.time.option_use_elapsed = True
    ct.stop_trigger.time.time = (54+5) * 1000000000L # 60 seconds.
    ct.stop_trigger.captured.enabled = True
    ct.stop_trigger.captured.bytes = (970043 + 64)
    ct.set_adapter(fa)
    capt = engine.create_capture(ct)
    if capt is None:
        print('Failed to create capture.')
        failures += 1
        return failures
    print_capture(capt)
    capt.start()
    capt = engine.find_capture(capt)
    print(capt.format_status())
    while (capt.status & 0x0001) and (capt.packets_filtered < needed_packets):
        time.sleep(3)
        capt.refresh()
    capt.stop()
    capt.refresh()
    print(capt.format_status())
    print(capt.packets_filtered)
    
    ffl = engine.get_forensic_file_list()
    for ff in ffl:
        if ff.name == save_all_name_engine:
            engine.delete_file(save_all_name_engine)
        elif ff.name == save_all_name_capture:
            engine.delete_file(save_all_name_capture)

    ffl = engine.get_forensic_file_list()
    engine.sync_forensic_database()
    ffl = engine.get_forensic_file_list()

    engine.save_all_packets(capt, save_all_name_engine)
    capt.save_all_packets(save_all_name_capture)

    found_engine = False
    found_capture = False
    ffl = engine.get_forensic_file_list()
    for ff in ffl:
        if ff.name == save_all_names[0]:
            found_engine = True
        elif ff.name == save_all_names[1]:
            found_capture = True
    if found_engine or found_capture:
        print('Failure: file added to Forensic File List before sync.')
        failures += 1

    engine.sync_forensic_database()

    found_engine = False
    found_capture = False
    ffl = engine.get_forensic_file_list()
    for ff in ffl:
        if ff.name == save_all_names[0]:
            found_engine = True
        elif ff.name == save_all_names[1]:
            found_capture = True
    if not use_file_db:
        print('File Database disabled')
        if found_engine or found_capture:
            print('Failure: files incorrectly added to Forensic File List.')
            failures += 1
    else:
        if not found_engine or not found_capture:
            print('Failure: files not added to Forensic File List.')
            failures += 1

    ffl = engine.get_forensic_file_list()
    for ff in ffl:
        if ff.name == save_all_name_engine:
            engine.delete_file(save_all_names[0])
        elif ff.name == save_all_name_capture:
            engine.delete_file(save_all_names[1])
    engine.sync_forensic_database()
    return failures


def do_packets(engine, capture_name, packets_needed):
    print("Packets Test")
    failures = 0

    adpt = get_default_adapter(engine)

    capt = engine.find_capture(capture_name)
    if not capt:
        ct = omniscript.CaptureTemplate()
        if adpt:
            ct.set_adapter(adpt)
        ct.general.buffer_size = BYTES_IN_A_MEGABYTE
        ct.general.name = capture_name
        ct.general.option_continuous_capture = True
        ct.general.option_capture_to_disk = True
        ct.general.option_start_capture = True
        ct.set_all(True)
        capt = engine.create_capture(ct)
        if not capt:
            print('Failed to create capture.')
            failures += 1
            return failures
        while capt.packets_filtered < packets_needed:
            capt.refresh()
    capt.stop()
    capt.refresh()
    if capt.packets_filtered < packets_needed:
        print('Not enough packets.')
        failures += 1
        return failures
    pl = capt.get_packets((capt.first_packet,capt.first_packet + 9))
    if len(pl) != 10:
        print('Did not get 10 packets.')
    print_packet_list(pl)

    for p in pl:
        dp = omniscript.DecodedPacket(p)
        print(dp)

    if False:
        next_packet = capt.first_packet
        last_packet = capt.packet_count - 1
        with open(os.path.join(temp_path, "packet_info.txt"), 'w') as fle:
            while next_packet < last_packet:
                count = last_packet - next_packet
                if count > 49:
                    count = 49
                pl = capt.get_packets((next_packet, next_packet + count))
                for p in pl:
                    fle.write('%d : %s\n' % (p.number, p.timestamp.ctime()))
                next_packet += count + 1
    return failures

        
def do_capture_stats(engine, capture_name, packets_needed):
    print("Capture Statistics Test")
    failures = 0

    capt = engine.find_capture(capture_name)
    if not capt:
        ca = omniscript.CaptureTemplate()
        ca.general.name = capture_name
        ca.general.buffer_size = 10 * BYTES_IN_A_MEGABYTE
        ca.general.comment = "Python generated capture."
        ca.general.directory = os.path.join(temp_path, "Capture Files")
        ca.general.file_pattern = capture_name + '-'
        ca.general.file_size = 128 * BYTES_IN_A_MEGABYTE
        ca.general.keep_last_files_count = 5
        ca.general.max_file_age = SECONDS_IN_A_HOUR
        ca.general.max_total_file_size = BYTES_IN_A_GIGABYTE
        ca.general.slice_length = 256
        ca.general.tap_timestamps = omniscript.TAP_TIMESTAMPS_DEFAULT
        ca.general.option_capture_to_disk = True
        ca.set_all(True)
        ca.indexing.set_all(True)
        ca.plugins.set_all(True)

        capt = engine.create_capture(ca)
        if capt is None:
            print('Did not create the capture: %s' % capture_name)
            failures += 1
            return failures

        capt.start()
        sites = ['savvius.com', 'google.ca', 'google.co.uk', 'google.com.au', 'google.co.nz']
        sites = ['savvius.com', 'google.com', 'yahoo.com']
        while capt.packets_filtered < packets_needed:
            for site in sites:
                try:
                    #os.system("ping -n 1 " + "2.20.183.0")
                    url = r"http://%s" % site
                    site_text = urllib2.urlopen(url).read()
                    print(site_text)
                except:
                    pass
            capt.refresh()
        capt.stop()

    if capt.packets_filtered < packets_needed:
        print('Not enough packets.')
        failures += 1
        return failures
    
    context = capt.get_stats_context()
    if not context:
        print('Did not get Statistics Context.')
        failures += 1
        return failures

    apps = capt.get_application_stats()
    calls = capt.get_call_stats()
    countrys = capt.get_country_stats()
    nodes = capt.get_node_stats()
    protocols = capt.get_protocol_stats()
    summarys = capt.get_summary_stats()

    total_packets = 0
    total_bytes = 0
    if context:
        total_packets = context.total_packets
        total_bytes = context.total_bytes

    if summarys and summarys.has_key('Network'):
        total_packets = summarys['Network']['Total Packets'].value
        total_bytes = summarys['Network']['Total Bytes'].value

    if protocols and len(protocols) > 0:
        try:
            udps = next(p for p in protocols if p.name == "UDP")
        except:
            udps = None
        if udps is not None:
            print(udps)
    
    print_stats_context(context)
    print_application_stats(apps)
    print_call_stats(calls)
    print_country_stats(countrys)
    print_node_stats(nodes, total_packets, total_bytes)
    if nodes:
        print_node_stat(nodes[0])
    print_protocol_stats(protocols)
    if protocols:
        print_protocol_stat(protocols[0])
        flats = omniscript.protocolstatistic.flatten(protocols)
        print_protocol_stats(flats)
    print_summary_stats(summarys)
    if summarys:
        if isinstance(summarys, dict):
            for name, group in summarys.iteritems():
                print_summary_group(name, group)
                break
        elif isinstance(summarys, list):
            for summary in summarys:
                print_summary_stat(summary)
                break
    return failures
        

def do_reports(engine, capture_name, capture_file, packets_needed):
    print("Reports Test")
    failures = 0

    fl = engine.get_file_list()
    ffi = list(i for i in fl if i.name == capture_file)
    if len(ffi) == 0:
        sf_filename = os.path.normpath(os.path.join(shared_files, capture_file))
        engine.send_file(sf_filename)
    fl = engine.get_file_list()
    ffi = list(i for i in fl if i.name == capture_file)
    if len(ffi) == 0:
        print('*** Failed to send file to engine: %s' % capture_file)
        failures += 1
        return failures

    cl = engine.get_capture_list()
    cli = list(i for i in cl if i.name == capture_name)
    engine.delete_capture(cli)
        
    cf_path = fl[0].name + capture_file # do not use os.path.join
    fa = omniscript.FileAdapter(cf_path)
    fa.limit = 1
    fa.speed = 0.0
    ct = omniscript.CaptureTemplate()
    ct.set_adapter(fa)
    ct.general.buffer_size = 10 * BYTES_IN_A_MEGABYTE
    ct.general.name = capture_name
    ct.general.option_continuous_capture = True
    ct.general.option_capture_to_disk = False
    ct.general.option_start_capture = False
    ct.general.option_timeline_app_stats = True
    ct.general.option_timeline_stats = True
    ct.general.option_timeline_top_stats = True
    ct.general.option_timeline_voip_stats = True
    ct.analysis.option_alarms = True
    ct.analysis.option_analysis_modules = True
    ct.analysis.option_application = True
    ct.analysis.option_compass = True
    ct.analysis.option_country = True
    ct.analysis.option_error = True
    ct.analysis.option_expert = True
    ct.analysis.option_network = True
    ct.analysis.node_protocol_detail_limit.enabled = True
    ct.analysis.node_limit.enabled = True
    ct.analysis.protocol_limit.enabled = True
    ct.analysis.option_size = True
    ct.analysis.option_summary = True
    ct.analysis.option_traffic = True
    ct.analysis.option_voice_video = True
    ct.stop_trigger.enabled = True
    ct.stop_trigger.time.enabled = True
    ct.stop_trigger.time.option_use_elapsed = True
    ct.stop_trigger.time.time = 30 * 1000000000L # 10 seconds.
    ct.stop_trigger.captured.enabled = True
    ct.stop_trigger.captured.bytes = (2129405 + 64)
    capt = engine.create_capture(ct)
    if capt is None:
        print('Failed to create capture.')
        failures += 1
        return failures

    capt.start()
    capt.refresh()
    print(capt.format_status())
    while (capt.status & 0x0001) and (capt.packets_filtered < packets_needed):
        time.sleep(3)
        capt.refresh()
    capt.stop()
    capt.refresh()
    print(capt.format_status())
    print(capt.packets_filtered)
    
    apps = capt.get_application_stats()
    calls = capt.get_call_stats()
    nodes = capt.get_node_stats()
    protos = capt.get_protocol_stats()
    sums = capt.get_summary_stats()

    if apps:
        print(apps)

    if calls:
        print(calls)

    if nodes:
        print('\nNode Statistisc Report: IPv4')
        with open(node_report[0], "w") as report: 
            report.write(omniscript.NodeStatistic.report_header(node_columns))
            print(omniscript.NodeStatistic.report_header(node_columns, newline=False))
            for n in nodes:
                if n.is_spec(omniscript.MEDIA_CLASS_ADDRESS, omniscript.MEDIA_SPEC_IP_ADDRESS):
                    report.write(n.report(node_columns))
                    print n.report(node_columns, newline=False)
        print('\nNode Statistisc Report: IPv6')
        with open(node_report[1], "w") as report: 
            report.write(omniscript.NodeStatistic.report_header(node_columns))
            print(omniscript.NodeStatistic.report_header(node_columns, newline=False))
            for n in nodes:
                if n.is_spec(omniscript.MEDIA_CLASS_ADDRESS, omniscript.MEDIA_SPEC_IPV6_ADDRESS):
                    report.write(n.report(node_columns))
                    print(n.report(node_columns, newline=False))
        print('\nNode Statistisc Report: Ethernet')
        with open(node_report[2], "w") as report: 
            report.write(omniscript.NodeStatistic.report_header(node_columns))
            print(omniscript.NodeStatistic.report_header(node_columns, newline=False))
            for n in nodes:
                if n.is_spec(omniscript.MEDIA_CLASS_ADDRESS, omniscript.MEDIA_SPEC_ETHERNET_ADDRESS):
                    report.write(n.report(node_columns))
                    print(n.report(node_columns, newline=False))
    else:
        print('\nFailed to get node stats.')
        failures += 1

    if protos:
        print('\nProtocol Statistisc Report')
        with open (protocol_report, "w") as report: 
            report.write(omniscript.ProtocolStatistic.report_header())
            print(omniscript.ProtocolStatistic.report_header(newline=False))
            p_flat = omniscript.protocolstatistic.flatten(protos, True)
            for p in p_flat:
                report.write(p.report())
                print(p.report(newline=False))
    else:
        print('\nFailed to get protocol stats.')
        failures += 1

    gen_lbls = [
        'Start Date',
        'Start Time',
        'Duration',
        'Trigger Count'
        ,'Trigger Wait Time',
        'Dropped Packets',
        'Duplicate Packets Discarded']
    if sums:
        for lbl in gen_lbls:
            print(sums['General'][lbl].report(newline=False))
        print

        nt_lbls = [
            'Total Bytes',
            'Total Packets',
            'Total Broadcast',
            'Total Multicast',
            'Average Utilization (percent)',
            'Average Utilization (bits/s)',
            'Current Utilization (percent)',
            'Current Utilization (bits/s)']
        for lbl in nt_lbls:
            print(sums['Network'][lbl].report(newline=False))
        print

        print "\nSummary Statistisc Report"
        groups = list(g for g, gg in sums.iteritems())
        groups.sort()
        with open (summary_report, "w") as report: 
            report.write(omniscript.SummaryStatistic.report_header())
            print(omniscript.SummaryStatistic.report_header(newline=False))
            for g in groups:
                for _, v in sums[g].iteritems():
                    report.write(v.report())
                    print(v.report(newline=False))
    else:
        print('\nFailed to get summary stats.')
        failures += 1

    return failures


def do_forensic_template(engine, forensic_name):
    print("Forensic Template Test")
    failures = 0

    ffl = engine.get_forensic_file_list()
    print_forensic_file_list(ffl)

    ff = next((i for i in ffl if ((getattr(i, 'packet_count') > 1000) and (getattr(i, 'packet_count') < 10000))), None)
    if not ff:
        print('No forensic files or files are too large.')
        failures += 1
        return failures

    fsl = engine.get_forensic_search_list()
    engine.delete_forensic_search(fsl)
    
    fs = engine.find_forensic_search(forensic_name)
    while fs:
        fsid = fs.id
        engine.delete_forensic_search(fs)
        fs = engine.find_forensic_search(forensic_name)
        if fs and fs.id == fsid:
            print('Forensic Search not deleted: %s' % fs.name)
            failures += 1
            break

    fs_filter = omniscript.Filter('Forensic Search - HTTP or X-Windows')
    fs_filter.comment = 'Python created filter.'
    http_protocol_node = omniscript.ProtocolNode()
    http_protocol_node.set_protocol('HTTP')
    xwin_protocol_node = omniscript.ProtocolNode()
    xwin_protocol_node.set_protocol('X-Windows')
    http_protocol_node.or_node = xwin_protocol_node
    fs_filter.criteria = http_protocol_node

    ft = omniscript.ForensicTemplate()
    ft.name = forensic_name
    ft.add_file(ff.path)
    ft.filter = fs_filter
    ft.option_packets = True
    ft.option_indexing = True
    fs = engine.create_forensic_search(ft)
    timeout = 0
    while fs.status < 2:
        fs.refresh()
        timeout += 1
        if timeout > fs_refresh_count:
            break
    if fs.status < 2:
        failures += 1
        return failures

    failures += query_expert_counts(engine)

    print_forensic_search(fs)
#   failures += query_expert_short(engine, fs)

    ft_all = omniscript.ForensicTemplate()
    ft_all.name = forensic_name + ' All'
    ft_all.add_file(ff.path)
    ft_all.options_packets = True
    ft_all.options_log = True
    ft_all.set_all_analysis_options(True)
    fs_all = engine.create_forensic_search(ft_all)
    for _ in range(fs_refresh_count):
        fs.refresh()
    print_forensic_search(fs_all)

    failures += query_expert_long(engine, fs_all)

    return failures


def do_forensic_stats(engine, forensic_name, packets_needed):
    print("Forensic Statistics Test")
    failures = 0

    fs = engine.find_forensic_search(forensic_name)
    if not fs:
        print('Did not find forensic search: %s' % forensic_name)
        failures += 1
        return failures
    if fs.packet_count < packets_needed:
        print('Not enough packets.')
        failures += 1
        return failures
    
    context = fs.get_stats_context()
    if not context:
        print('Did not get Statistics Context.')
        failures += 1
        return failures

    apps = fs.get_application_stats()
    calls = fs.get_call_stats()
    countrys = fs.get_country_stats()
    nodes = fs.get_node_stats()
    protocols = fs.get_protocol_stats()
    summarys = fs.get_summary_stats()

    total_packets = 0
    total_bytes = 0
    if context:
        total_packets = context.total_packets
        total_bytes = context.total_bytes

    if summarys and summarys.has_key('Network'):
        total_packets = summarys['Network']['Total Packets'].value
        total_bytes = summarys['Network']['Total Bytes'].value

    if protocols and len(protocols) > 0:
        try:
            udps = next(p for p in protocols if p.name == "UDP")
        except:
            udps = None
        if udps:
            print(udps)
    
    print_stats_context(context)
    print_application_stats(apps)
    print_call_stats(calls)
    print_country_stats(countrys)
    print_node_stats(nodes, total_packets, total_bytes)
    if nodes and len(nodes) > 0:
        print_node_stat(nodes[0])
    print_protocol_stats(protocols)
    if protocols and len(protocols) > 0:
        print_protocol_stat(protocols[0])
        flats = omniscript.protocolstatistic.flatten(protocols)
        print_protocol_stats(flats)
    print_summary_stats(summarys)
    if summarys and len(summarys) > 0:
        if isinstance(summarys, dict):
            for name, group in summarys.iteritems():
                print_summary_group(name, group)
                break
        elif isinstance(summarys, list):
            for summary in summarys:
                print_summary_stat(summary)
                break
    return failures
        

def do_miscellaneous(engine):
    print("Miscellaneous Test")
    failures = 0

    class_ids = omniscript.omniscript.get_class_name_ids()
    co_names = omniscript.omniscript.get_country_code_names()
    co_codes = omniscript.omniscript.get_country_name_codes()
    id_classes = omniscript.omniscript.get_id_class_names()
    protocol_names = omniscript.omniscript.get_id_protocol_names()
    protocol_short_name = omniscript.omniscript.get_id_protocol_short_names()
    id_stats = omniscript.omniscript.get_id_stat_names()
    protocol_short_name_ids = omniscript.omniscript.get_protocol_short_name_ids()

    print(class_ids)
    print(co_names)
    print(co_codes)
    print(id_classes)
    print(protocol_names)
    print(protocol_short_name)
    print(id_stats)
    print(protocol_short_name_ids)

    cl = engine.get_capture_list()
    print(cl)

    #if len(cl) > 0:
    #    capture = cl[0]
    #    ## Forensic Template
    #    f_template = omniscript.ForensicTemplate()
    #    f_template.name = 'test_forensic_search'
    #    f_template.capture_name = capture.name
    #    f_template.start_time = capture.start_time
    #    f_template.end_time = capture.stop_time
    #    f_template.option_packets = True
    #    f_template.limit = 0
    #    fs = engine.create_forensic_search(f_template)
    #    engine.delete_forensic_search(fs)

    #ffl = engine.get_forensic_file_list()
    #if ffl and len(ffl) > 0:
    #    ff = ffl[0]
    #    ft = omniscript.ForensicTemplate()
    #    ft.name = 'Python Forensic Search'
    #    ft.add_file(ff.path)
    #    fs.start_time = ff.start_time
    #    fs.end_time = ff.stop_time
    #    ft.option_packets = True
    #    ft.limit = 0
    #    fs = engine.create_forensic_search(ft)
    #    for i in range(fs_refresh_count):
    #        fs.refresh()
    #    print_forensic_search(fs)

    emailCaptureId = None
    ml = engine.get_analysis_module_list()
    m = omniscript.find_analysis_module(ml, 'EmailCaptureOE')
    if m is not None:
        emailCaptureId = m.id

    if emailCaptureId:
        msg = "<GetAllOptions/>"
        buf = struct.pack('=QQ%ss' % len(msg), len(msg), 0, msg )
        response = engine.send_plugin_message(emailCaptureId, None, buf)
        if response and len(response) > 0:
            with open(os.path.join(temp_path, "getalloptions.bin"), "wb") as log:
                log.write(response)
            est_len = len(response) - 44
            (response_length, captId, plugId, result_size, result_code, result) = struct.unpack('I16s16sIi%ss' % est_len, response)
            print(response_length)
            print(captId)
            print(plugId)
            print(result_size)
            if result_code == 0 and len(result) > 0:
                xml_len = len(result) - 16
                (text_len, bin_len, xml) = struct.unpack('QQ%ss' % xml_len, result)
                print(text_len)
                print(bin_len)
                print(xml)

    timeout = engine.timeout
    engine.timeout = 1
    try:
        status = engine.get_status()
        print_engine_status(status)
    except omniscript.OmniError as error:
        print(error.message)
        if error.code != E_TIMEOUT:
            failures += 1
    except:
        print('Caught some other error.')

    engine.timeout = timeout

    return failures


def do_diagnostics(engine):
    print("Diagnostics Test")
    failures = 0

    #-help       - this screen
    #-admin      - Generate tech support info
    #-adapters   - Print RAID adapter info
    #-drives     - Print RAID drive info
    #-processes  - Print process list
    #-dmesg      - Print last 1000 lines dmesg
    #-log        - Print last 1000 lines of /var/log/syslog
    #-mount      - Print mounted filesystems
    #-dbhealth   - Report database health
    #-dbsize     - Print database size
    #-verbose    - Increase output verbosity
    cmds = ['admin', 'adapters', 'drives', 'processes',
            'dmesg', 'log', 'mount', 'dbhealth', 'dbsize',
            'dbadmin']

#    cmds = ['dbsize', 'dbhealth', 'dbadmin']
    hp = engine.get_host_platform()
    if hp and hp.os is  omniscript.OPERATING_SYSTEM_LINUX:
        for cmd in cmds:
            try:
                result = engine.get_diagnostics_info(cmd)
                obj = omniscript.Diagnostic.construct(cmd, result)
                print(obj)
            except:
                failures += 1

    return failures


def do_flows_test(engine, capt_args, ipv6_args):
    print("Flows Test")
    failures = 0

    if False:
        try:
            filename = 'Expert-Query-Flows-Response.xml'
            if os.path.exists(filename):
                root = ET.parse(filename)
                msg = root.getroot()
                eqs = [ExpertQuery(i) for i in msg.findall('query')]
                rss = [ExpertResult(i) for i in msg.findall('result-set')]
                print('%d %d' %(len(eqs), len(rss)))
                for e in eqs:
                    print('Query: %s\n  Columns: %d' % (e.name, len(e.columns)))
                print
                for r in rss:
                    print('Result: %s\n  Columns: %d\n  Rows: %d' % (r.name, len(r.columns), len(r.rows)))
            print
            return failures
        except:
            print
            return 1

    cap = create_capture(engine, *capt_args)
    if cap is None:
        failures += 1
        return failures
    failures += query_expert_short(engine, cap)

    cap = create_capture(engine, *capt_args)
    if cap is None:
        failures += 1
        return failures
    failures += query_expert_long(engine, cap)

    return failures


def do_select_related(engine, capt_args):
    print("Select Related Test")
    failures = 0

    cap = create_capture(engine, *capt_args)
    if not cap:
        failures += 1
        return failures
    related = cap.select_related([1,4], omniscript.SELECT_BY_CONVERSATION)
    print(related)
    return failures


def do_audit_log(engine):
    print("Audit Log Test")
    failures = 0

    al = engine.get_audit_log(0, 20)
    print_audit_log(al)
    al.get_next(10)
    print_audit_log(al)
    al.get(30, 5)
    print_audit_log_indexes(al)
    al.get_next(-10)
    print_audit_log_indexes(al)
    return failures


def do_event_log(engine):
    print("Event Log Test")
    failures = 0

    el = engine.get_event_log(0, 20)
    print_event_log(el)
    el.get_next(10)
    print_event_log(el)
    el.get(30, 5)
    print_event_log_indexes(el)
    el.get_next(-10)
    print_event_log_indexes(el)

    cl = engine.get_capture_list()
    if len(cl) == 0:
        print("No captures for Event Log testing.")
        return failures

    cap = cl[0]
    log_id = engine.get_event_log(0, 5, cap.id)
    print_event_log(log_id)

    log_str = engine.get_event_log(0, 5, str(cap.id))
    print_event_log(log_str)

    log_cap = engine.get_event_log(0, 5, cap)
    print_event_log(log_cap)
    return failures


def do_acl(engine):
    print("ACL Test")
    failures = 0

    acl = engine.get_access_control_list()
    print_access_control_list(acl)
    return failures


def do_wireless(engine):
    print("Wireless Test")
    failures = 0

    hwo_list = engine.get_hardware_options_list()
    if not hwo_list:
        return failures
    print_hardware_options_list(hwo_list, False)

    engine.set_hardware_options(hwo_list)
    engine.set_hardware_options(hwo_list)
    engine.set_hardware_options(hwo_list)
    engine.set_hardware_options(hwo_list)


    print_hardware_options_list(hwo_list)
    ho = hwo_list[0]
    if not isinstance(ho, omniscript.WirelessHardwareOptions):
        failures += 1
        return failures

    al = engine.get_adapter_list()
    print(al)
    wa = engine.find_adapter("Wi-Fi")
    if not wa:
        return failures

    ac = engine.get_adapter_configuration(wa)
    print_adapter_configuration(ac)

    ch2_list = ho.find_channel_scanning_entry(2)
    print ch2_list
    ch2 = ch2_list[0]
    ho.set_channel(ch2)
    ho.set_channel_scanning(False)

    engine.set_hardware_options(ho)
    engine.set_adapter_configuration(ac)

    t_hwo_list = engine.get_hardware_options_list()
    if not t_hwo_list:
        return failures
    t_ho = t_hwo_list[0]
    if t_ho.configuration != omniscript.WIRELESS_CONFIGURATION_SINGLE_CHANNEL:
        failures += 1
    if t_ho.channel_number != ch2.channel_number:
        failures += 1
    if t_ho.channel_frequency != ch2.channel_frequency:
        failures += 1
    if t_ho.channel_band != ch2.channel_band:
        failures += 1
    if failures > 0:
        return failures

    ho.set_channel_scanning(True)
    engine.set_hardware_options(ho)
    engine.set_adapter_configuration(ac)

    t_hwo_list = engine.get_hardware_options_list()
    if not t_hwo_list:
        return failures
    t_ho = t_hwo_list[0]

    ho.set_channel(ho.channel_scanning[0])
    ho.set_channel_scanning(False)
    engine.set_hardware_options(ho)
    engine.set_adapter_configuration(ac)
    return failures


def do_restart(engine, auth, domain, user, pwd):
    print("Restart Test")
    failures = 0

    if not engine.is_connected():
        print('do_restart - Engine not connected.')
        failures += 1
        if not connect_to_engine(engine, auth, domain, user, pwd):
            print('do_restart - Failed initial engine connection.')
            failures += 1
            return failures

    engine.restart()
    if engine.is_connected():
        print('do_restart - is_connected after restart failure.')
        failures += 1
    attempt = 0
    while not engine.is_connected():
        time.sleep(2)
        attempt += 1
        try:
            engine.connect(auth, domain, user, pwd)
        except:
            if attempt > 10:
                print('Failed to reconnect after restart: %d' % attempt)
                failures += 1
                return failures
    if not engine.is_connected():
        print('do_restart - is_connected after reconnect failure.')
        failures += 1
    try:
        al = engine.get_adapter_list()
        print(al)
    except:
        print('Failed to get adapter list after reset.')
        failures += 1
    return failures


def do_tcpdump_adapter(engine):
    print("TCP Dump Adapter Test")
    failures = 0

    remove_adapters = True
    tcpdump_test = [True, True, True, True]
    adapters_to_delete = []

    if remove_adapters:
        eal = engine.get_adapter_list()
        atd = [a for a in eal if a.adapter_type == omniscript.ADAPTER_TYPE_PLUGIN]
        engine.delete_adapter(atd)

    if tcpdump_test[0]:
        tcpdump = omniscript.TCPDump(engine, "10.4.2.78")  # optiplex Ubuntu 14.04
        tcpdump.login("root", "savvius")
        interface_list = tcpdump.get_adapter_list()
        interface = None
        for i in interface_list:
            if i.name == "eth0":
                interface = i
                break
        if interface:
            template = omniscript.TCPDumpTemplate(interface)
            new_adapter_name = tcpdump.create_adapter(template)
            if not new_adapter_name:
                failures += 1
            print("Created new adapter: %s" % new_adapter_name)
        else:
            failures += 1
        eal = engine.get_adapter_list()
        if eal:
            new_adapter = omniscript.find_adapter(eal, new_adapter_name)
            print_adapter(new_adapter)
            adapters_to_delete.append(new_adapter)

    if tcpdump_test[1]:
        tcpdump = omniscript.TCPDump(engine, "10.4.2.151")
        pk = open('10.4.2.151.tsadmin_key.txt').read()
        tcpdump.login("tsadmin", private_key=pk)
        # ens160, ens192, ens224, lo
        interface_list = tcpdump.get_adapter_list()
        interface = next((i for i in interface_list if i.name == 'ens192'), None)
        new_adapter_name = None
        if interface:
            template = omniscript.TCPDumpTemplate(interface)
            new_adapter_name = tcpdump.create_adapter(template)
            if not new_adapter_name:
                failures += 1
            print("Created new adapter: %s" % new_adapter)
        else:
            failures += 1
        eal = engine.get_adapter_list()
        if eal:
            new_adapter = omniscript.find_adapter(eal, new_adapter_name)
            print_adapter(new_adapter)
            adapters_to_delete.append(new_adapter)

    if tcpdump_test[2]:
        tcpdump = omniscript.TCPDump(engine, "10.4.2.78")
        template = omniscript.TCPDumpTemplate("eth1")
        new_adapter_name = tcpdump.create_adapter(template, "root", "savvius")
        if not new_adapter_name:
            failures += 1
        print("Created new adapter: %s" % new_adapter)
        eal = engine.get_adapter_list()
        if eal:
            new_adapter = omniscript.find_adapter(eal, new_adapter_name)
            print_adapter(new_adapter)
            adapters_to_delete.append(new_adapter)

    if tcpdump_test[3]:
        tcpdump = omniscript.TCPDump(engine, "10.4.2.151")
        pk = open('10.4.2.151.tsadmin_key.txt').read()
        tcpdump.set_credentials("tsadmin", private_key=pk)
        # ens160, ens192, ens224, lo
        template = omniscript.TCPDumpTemplate("ens160")
        new_adapter_name = tcpdump.create_adapter(template)
        if not new_adapter_name:
            failures += 1
        print("Created new adapter: %s" % new_adapter)
        eal = engine.get_adapter_list()
        if eal:
            new_adapter = omniscript.find_adapter(eal, new_adapter_name)
            print_adapter(new_adapter)
            adapters_to_delete.append(new_adapter)

    if len(adapters_to_delete) > 0:
        old_adapters = engine.get_adapter_list()
        if len(old_adapters) == 0:
            failures += 1
        else:
            engine.delete_adapter(adapters_to_delete)
            new_adapters = engine.get_adapter_list()
            if len(old_adapters) != len(adapters_to_delete) + len(new_adapters):
                failures += 1
    else:
        print("No adapters to delete.")
    return failures


def do_lauren_test(engine):
    print("Lauren Test")
    failures = 0

    ### Create capture template
    capTemplate = omniscript.CaptureTemplate("Lauren-CaptureTemplate.xml")
    capTemplate.adapter.name = "Local Area Connection"
    capTemplate.general.name = 'Lauren Test'
    capTemplate.general.file_pattern = 'Lauren Test-'
    capTemplate.add_filter('HTTP')
    capTemplate.filter.mode = 3  # REJECT Matching Any
    cap = engine.create_capture(capTemplate)
    print(cap)
 
    return failures


def main():
    # Create the OmniScript object.
    omni = omniscript.OmniScript(verbose=2, flags=0)

    omni.set_log_file(os.path.join(temp_path, "PyTest-log.log"))
    omni.info('PyTest: Begin')
    omni.info('OmniScript Version: %s Build %s' %
              (omniscript.__version__, omniscript.__build__))

    # Legacy connection style.
    if test_legacy_connect:
        legacy_engine = omni.connect(host, port, auth, domain, user, pwd)
        if not legacy_engine :
            omni.critical('Legacy connection failed. Failed to create an engine.')
            return

        ic = legacy_engine.is_connected()
        if not ic:
            omni.critical('Legacy connection failed. Not connected to an engine.')
            return

        legacy_engine.disconnect()
        ic = legacy_engine.is_connected()
        if ic:
            omni.critical('Disconnect after legacy connection from engine failed.')
            return
        legacy_engine = None

    # New connection style.
    engine = omni.create_engine(host, port)
    if not connect_to_engine(engine, auth, domain, user, pwd):
        print('Failed to connect to the engine')
        return

    ver = engine.get_version()
    omni.info('OmniEngine Version: %s' % ver)
    print

    adpt = get_default_adapter(engine)
    if adpt is None:
        pass

    failures = 0
    unit_tests_failures = 0
    test_gets_failures = 0
    test_filters_failures = 0
    test_delete_all_failures = 0
    test_file_ops_failures = 0
    test_capt_create_failures = 0
    test_file_adapter_failures = 0
    test_packets_failures = 0
    test_capture_stats_failures = 0
    test_reports_failures = 0
    test_forensic_template_failures = 0
    test_forensic_stats_failures = 0
    test_miscellaneous_failures = 0
    test_restart_failures = 0
    test_diagnostics_failures = 0
    test_flows_failures = 0
    test_select_related_failures = 0
    test_tcpdump_failures = 0
    test_audit_log_failures = 0
    test_event_log_failures = 0
    test_acl_failures = 0
    test_wireless_failures = 0

    messages = []

    if unit_tests:
        unit_tests_failures = do_unit_tests(engine)
        if unit_tests_failures:
            messages.append("Unit Test failures: %d" % unit_tests_failures)
        failures += unit_tests_failures

    if test_gets:
        test_gets_failures = do_gets(engine)
        if test_gets_failures:
            messages.append("Test Gets failures: %d" % test_gets_failures)
        failures += test_gets_failures
        
    if test_filters:
        test_filters_failures = do_filters(engine, filter_name)
        if test_filters_failures:
            messages.append("Test Filters failures: %d" % test_filters_failures)
        failures += test_filters_failures

    if test_delete_all:
        test_delete_all_failures = do_delete_all(engine)
        if test_delete_all_failures:
            messages.append("Test Delete All failures: %d" % test_delete_all_failures)
        failures += test_delete_all_failures

    if test_file_ops:
        test_file_ops_failures = do_file_ops(engine, packet_file_name, result_file)
        if test_file_ops_failures:
            messages.append("Test File Ops failures: %d" % test_file_ops_failures)
        failures += test_file_ops_failures
        
    if test_capt_create:
        test_capt_create_failures = do_capt_create(engine, capture_name, "HTTP", 1000)
        if test_capt_create_failures:
            messages.append("Test Capture Create failures: %d" % test_capt_create_failures)
        failures += test_capt_create_failures
        
    if test_file_adapter:
        save_all_names = [save_all_name_engine, save_all_name_capture]
        test_file_adapter_failures = do_file_adapter(engine, packet_file_name, capture_name,
                                    packets_needed, save_all_names)
        if test_file_adapter_failures:
            messages.append("Test File Adapter failures: %d" % test_file_adapter_failures)
        failures += test_file_adapter_failures
        
    if test_packets:
        test_packets_failures = do_packets(engine, capture_name, 1000)
        if test_packets_failures:
            messages.append("Test Packets failures: %d" % test_packets_failures)
        failures += test_packets_failures
        
    if test_capture_stats:
        test_capture_stats_failures = do_capture_stats(engine, capture_name, 1000)
        if test_capture_stats_failures:
            messages.append("Test Capture Stats failures: %d" % test_capture_stats_failures)
        failures += test_capture_stats_failures
        
    if test_reports:
        test_reports_failures = do_reports(engine, report_name, report_file, report_count)
        if test_reports_failures:
            messages.append("Test Reports failures: %d" % test_reports_failures)
        failures += test_reports_failures
        
    if test_forensic_template:
        test_forensic_template_failures = do_forensic_template(engine, forensic_name)
        if test_forensic_template_failures:
            messages.append("Test Forensic Template failures: %d" % test_forensic_template_failures)
        failures += test_forensic_template_failures

    if test_forensic_stats:
        test_forensic_stats_failures = do_forensic_stats(engine, forensic_name, 1000)
        if test_forensic_stats_failures:
            messages.append("Test Forensic Stats failures: %d" % test_forensic_stats_failures)
        failures += test_forensic_stats_failures

    if test_miscellaneous:
        test_miscellaneous_failures = do_miscellaneous(engine)
        if test_miscellaneous_failures:
            messages.append("Test Miscellaneous failures: %d" % test_miscellaneous_failures)
        failures += test_miscellaneous_failures

    if test_diagnostics:
        test_diagnostics_failures = do_diagnostics(engine)
        if test_diagnostics_failures:
            messages.append("Test Diagnostics failures: %d" % test_diagnostics_failures)
        failures += test_diagnostics_failures
        
    if test_flows:
        test_flows_failures = do_flows_test(engine, fortworth_capture, ipv6_capture)
        if test_flows_failures:
            messages.append("Test Flows failures: %d" % test_flows_failures)
        failures += test_flows_failures
        
    if test_select_related:
        test_select_related_failures = do_select_related(engine, fortworth_capture)
        if test_select_related_failures:
            messages.append("Test Select Related failures: %d" % test_select_related_failures)
        failures += test_select_related_failures

    if test_audit_log:
        test_audit_log_failures = do_audit_log(engine)
        if test_audit_log_failures:
            messages.append("Test Event Log failures: %d" % test_audit_log_failures)
        failures += test_audit_log_failures

    if test_event_log:
        test_event_log_failures = do_event_log(engine)
        if test_event_log_failures:
            messages.append("Test Event Log failures: %d" % test_event_log_failures)
        failures += test_event_log_failures

    if test_acl:
        test_acl_failures = do_acl(engine)
        if test_acl_failures:
            messages.append("Test ACL failures: %d" % test_acl_failures)
        failures += test_acl_failures

    if test_wireless:
        test_wireless_failures = do_wireless(engine)
        if test_wireless_failures:
            messages.append("Test Wireless failures: %d" % test_wireless_failures)
        failures += test_wireless_failures

    if test_restart:
        test_restart_failures = do_restart(engine, auth, domain, user, pwd)
        if test_restart_failures:
            messages.append("Test Restart failures: %d" % test_restart_failures)
        failures += test_restart_failures

    if test_tcpdump_adapter:
        test_tcpdump_failures = do_tcpdump_adapter(engine)
        if test_tcpdump_failures:
            messages.append("Test TCPDump failures: %d" % test_tcpdump_failures)
        failures += test_tcpdump_failures

    if test_lauren_test:
        do_lauren_test(engine)
        
    engine.disconnect()
    ic = engine.is_connected()
    if ic:
        omni.error('*** Failed to disconnected from the engine.')

    if failures == 0:
        omni.info('Success, no failures.')
    else:
        omni.error('Failed, %d failures.' % failures)
        for msg in messages:
            omni.error(msg)
    omni.info('PyTest: Done')


if __name__ == '__main__':
    main()
