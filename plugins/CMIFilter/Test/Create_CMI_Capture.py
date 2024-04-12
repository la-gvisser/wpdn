import sys
import os
import time
import getopt
import omniscript

tests = {
    'cmi-1' : ('CMI_before_No_Mac.pkt', 'CMI-Capture-CMI-No-Mac.xml', 'CMI Filter Capture - CMI No Mac'),
    'cmi-2' : ('CMI_before_With_Mac.pkt', 'CMI-Capture-CMI-With-Mac.xml', 'CMI Filter Capture - CMI With Mac'),
    'pcli' : ('PCLI_before_No_Mac.pkt', 'CMI-Capture-PCLI.xml', 'CMI Filter Capture - PCLI No Mac'),
    'securityid' : ('SecurityId_before_With_Mac.pkt', 'CMI-Capture-SecurityId.xml', 'CMI Filter Capture - SecurityId With Mac'),
    'ericsson' : ('Ericsson_before_No_Mac.pkt', 'CMI-Capture-Ericsson.xml', 'CMI Filter Capture - Ericsson No Mac'),
    'arris' : ('Arris_before_No_Mac.pkt', 'CMI-Capture-Arris.xml', 'CMI Filter Capture - Arris No Mac'),
    'juniper' : ('Juniper_ip_before_No_Mac.pkt', 'CMI-Capture-Juniper.xml', 'CMI Filter Capture - Juniper No Mac'),
    'nokia' : ('Nokia_before_No_Mac.pkt', 'CMI-Capture-Nokia.xml', 'CMI Filter Capture - Nokia No Mac'),
    'nokia-1' : ('Nokia_before_No_Mac.pkt', 'CMI-Capture-Nokia-1.xml', 'CMI Filter Capture - Nokia No Mac - 1')
    }

packet_file = 'CMI_before_No_Mac.pkt'
capture_template = 'CMI-Capture-CMI.xml'
capture_name = 'CMI Filter Capture - CMI'

adapter_speed = 0.0

option_run = False
option_delete = False

filter_name = ' CMI Filter'
plugin_name = 'CMIFilterOE'

# -----------------------------------------------------------------------------
def parse_args(arg_list):
    global tests
    global packet_file, capture_template, capture_name, adapter_speed
    global option_run, option_delete
    
    try:
        # a letter followed by a colon takes an argument.
        (opts, args) = getopt.getopt(arg_list, "rdt:s:?")
    except getopt.GetoptError as error:
        # print(help information and exit:)
        sys.exit(2)

    for opt, arg in opts:
        if opt in ("-r"):
            option_run = True
        elif opt in ("-d"):
            option_delete = True
        elif opt in ("-t"):
            if arg.lower() in tests.keys():
                tup = tests[arg.lower()]
                packet_file = tup[0]
                capture_template = tup[1]
                capture_name = tup[2]
            else:
                print('Invalid test option: %s' % (arg))
                sys.exit(2)
        elif opt in ("-s"):
            adapter_speed = float(arg)


# -----------------------------------------------------------------------------
omni = omniscript.OmniScript()

## Connect to engine
engine = omni.create_engine()
engine.connect()
if not engine.is_connected():
    print('Not connected to an engine.')
    exit(0)

parse_args(sys.argv[1:])

wd = os.path.dirname(os.path.realpath(__file__)) + '\\'
packet_pathname = wd + packet_file

plugins = engine.get_analysis_module_list()
plugin_filter_oe = omniscript.find_analysis_module(plugins, plugin_name)
if not plugin_filter_oe:
    print('%s is not installed' % (plugin_name))
    exit(0)

filter_list = engine.get_filter_list()
filter = omniscript.find_filter(filter_list, filter_name)
if not filter:
    filter = omniscript.Filter(filter_name)
    filter.comment = 'Python created Filter.'
    plugin_node = omniscript.PluginNode()
    plugin_node.add_analysis_module(plugin_filter_oe)
    filter.criteria = plugin_node
    engine.add_filter(filter)

## Create the file adapter
print('Packet File: %s' % (packet_pathname))
if adapter_speed:
    print('Adapter Speed: %f' % (adapter_speed))
file_adapter = omniscript.FileAdapter(packet_pathname)
file_adapter.limit = 1
file_adapter.speed = adapter_speed

## Create capture template
print('Capture Template: %s' % (capture_template))
print('Capture Name: %s' % (capture_name))
template = omniscript.CaptureTemplate(capture_template)
template.general.name = capture_name
template.general.comment = 'Python created Capture.'
template.set_adapter(file_adapter)
template.add_filter(filter)
template.analysis.set_all(False)
template.analysis.option_analysis_modules = True
template.plugins.modules.append(plugin_filter_oe.name)

## Create the capture
capture = engine.create_capture(template)

if option_run:
    ## Start, stop, and refresh capture
    capture.start()
    while (capture.status & 0x0001) and (capture.packets_filtered < 1000):
        time.sleep(3)
        capture.refresh()
    capture.stop()
    capture.refresh()

    ## Print engine status
    print(capture.format_status())

if option_delete:
    engine.delete_capture(capture)
