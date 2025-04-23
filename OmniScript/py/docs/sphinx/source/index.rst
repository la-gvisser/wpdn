Welcome to OmniScript 2.0 documentation
=======================================

Savvius OmniScript 2.0 for Python.

Contents:

.. toctree::
   :maxdepth: 3

   omniscript


Tutorial
========

Installing OmniScript
*********************

**Prerequisites:**

* Windows OS
* Python 2.7 32-bit, see: https://www.python.org/
* Ez Setup: http://peak.telecommunity.com/dist/ez_setup.py

**Installation Steps:**

1. Download the latest OmniScript zipped file from MyPeek: https://mypeek.savvius.com

2. Unzip the OmniScript-0.x.x.zip, which contains OmniScript-0.x.x-py2.7.egg file to your file system

   Example:

   c:\\temp\\OmniScript-0.5.3-py2.7.egg

3. Open a Windows Command Prompt and navigate to the directory where you extracted the zipped file
   to, e.g. c:\\temp\\OmniScript-0.5.3-py2.7.egg

4. Enter the following:

   >>> cd c:\temp
   >>> python.exe ez_setup.py OmniScript-0.5.3-py2.7.egg

5. It will install OmniScript to $PYTHON_INSTALL_DIR\\Lib\\site-packages\\OmniScript-0.x.x-py2.7.egg .
   If you are upgrading, it will update your installation automatically.

6. You can verify that it updated the file, $PYTHON_INSTALL_DIR\\Lib\\site-packages\\easy-install.pth, with the following::

       import sys; sys.__plen = len(sys.path)
       ...
       ./omniscript-0.5.3-py2.7.egg

7. To confirm your OmniScript installation works, launch cmd.exe and type in the following commands:

   >>> import omniscript
   >>> print 'OmniScript Version:', omniscript.__version__, 'Build:', omniscript.__build__
   ...
   OmniScript Version: 0.5.3 Build: 50


The Basics
**********

The first steps are to import omniscript, display the version, create an 
:class:`OmniScript <omniscript.omniscript.OmniScript>` object and connect to an
OmniEngine. 

Display OmniScript Version
--------------------------

    >>> import omniscript
    >>> print('OmniScript: version %s.%s' % (omniscript.__version__, omniscript.__build__))
    OmniScript: version 0.5.3.50

Connect to an engine
--------------------
The :func:`connect() <omniscript.omniscript.OmniScript.connect>` 
function returns an :class:`OmniEngine <omniscript.omniengine.OmniEngine>`
object

Connect to an engine:
    >>> omni = omniscript.OmniScript()
    >>> engine = omni.create_engine('192.168.1.100')
    >>> engine.connect(username='user', password='password')
    True

Or supply all the arguments:
    >>> engine.connect('Default', None, 'user', 'password')

Alternate ways to connect, first pass in the arguments as a tuple/list, the 
order of the arguments must be as specified by the function:

    >>> windows_credentials = ['192.168.1.100', 6367, 'Default', None, 'user', 'password']
    >>> engine = omni.connect(*windows_credentials)

Or pass in the arguments as a dictionary, the order does not matter (in this example they are sorted by argument name):

    >>> linux_credentials = {'auth':'Third Party','domain':None,'host':'192.168.1.123','password':'password','port':6367,'user':'guest'}
    >>> engine = omni.connect(**linux_credentials)

Check engine connection and get engine's version
------------------------------------------------

Check the connection and display the the engine's version:
    >>> if engine.is_connected():
    >>>     print('Connected')
    >>> else:
    >>>     print('Not connected!')
    >>>     exit(0)
    >>> ver = engine.get_version()
    >>> print('OmniEngine: version %s' % ver)
    Connected
    OmniEngine: version 11.1.0.0

Get list of captures
--------------------

Retrieve the list of captures on the engine and display each capture's name and status. The
:func:`get_capture_list() <omniscript.omniengine.OmniEngine.get_capture_list>` 
function returns a list of :class:`Capture <omniscript.capture.Capture>` objects:

    >>> cl = engine.get_capture_list()
    >>> for c in cl:
    >>>     print("%s : %s" % (c.name, c.format_status()))
    user - Capture 1 : Idle
    user - Capture 2 : Capturing
    user - Capture 3 : Capturing

The :class:`Capture <omniscript.capture.Capture>` class supplies over
40 attributes for each capture. Display some of the attributes of the 
second capture:

    >>> def print_capture_info(cap):
    ...   print('Capture: %s' % cap.name)
    ...   print('Adapter: %s' % cap.adapter)
    ...   print('Adapter Type: %d' % cap.adapter_type)
    ...   print('Media Type: %d, Media Sub Type: %d' % (cap.media_type, cap.media_sub_type))
    ...   print('Packet Count: %d' % cap.packet_count)
    ...   print('Packets Dropped: %d' % cap.packets_dropped)
    ...
    >>> print_capture_info(cl[1])
    Adapter: Local Area Connection
    Adapter Type: 1
    Media Type: 0, Media Sub Type: 0
    Packet Count: 1367
    Packets Dropped: 0

The Capture's attributes are 'static', they are not automatically updated.
(This is also true of the
:class:`Capture <omniscript.capture.Capture>` objects in the list returned by 
:func:`get_capture_list() <omniscript.omniengine.OmniEngine.get_capture_list>`.)
The script may need to periodically refresh the capture's attributes. The 
:func:`refresh() <omniscript.capture.Capture.refresh>` function will update the 
object with fresh attributes:

    >>> capt = engine.find_capture(c2.name)
    >>> print("%s: Packet Count is %d, Dropped is %d" % (capt.name, capt.packet_count, capt.packets_dropped))
    user - Capture 2: Packet Count is 1483027, Dropped is 0
    >>> capt.refresh()
    >>> print("%s: Packet Count is %d, Dropped is %d" % (capt.name, capt.packet_count, capt.packets_dropped))
    user - Capture 2: Packet Count is 1483863, Dropped is 0

The Capture function is_capturing() will refresh the object and return True if the Capture is capturing.

Capture Templates
*****************
Create Capture
--------------
Create captures with the
:class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>` class and
the OmniEngine function
:func:`create_capture() <omniscript.omniengine.OmniEngine.create_capture>`:

    >>> template = omniscript.CaptureTemplate()
    >>> template.general.name = "Python Capture"
    >>> template.general.option_capture_to_disk = "True"
    >>> template.general.file_pattern = "Python Capture-"
    >>> capt = engine.create_capture(template)
    >>> capt.start()
    >>> capt.refresh()

Timeline Stats and Analysis Options
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
To enable all analysis modules and timeline stats, set the following to the
:class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`:

    >>> template.analysis.set_all(True) ## Enables all Analysis Modules
    >>> template.general.set_all(True) ## Enables all Timeline Stats

To selectively enable analysis and stats, set each option individually to the
:class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`:

    >>> template.general.option_timeline_app_stats = True
    >>> template.general.option_timeline_stats = True
    >>> template.general.option_timeline_top_stats = True
    >>> template.general.option_timeline_voip_stts = True
    >>> template.analysis.option_analysis_modules = True
    >>> template.analysis.option_summary = True
    >>> template.analysis.option_node_protocol_detail_limit = 200000
    >>> template.analysis.node_limit.enabled = True
    >>> template.analysis.protocol_limit.enabled = True
    >>> template.analysis.option_alarms = True
    >>> template.analysis.option_application = True
    >>> template.analysis.option_compass = True
    >>> template.analysis.option_country = True
    >>> template.analysis.option_error = True
    >>> template.analysis.option_expert = True
    >>> template.analysis.option_network = True
    >>> template.analysis.option_size = True
    >>> template.analysis.option_top_talker = True
    >>> template.analysis.option_traffic = True
    >>> template.analysis.option_voice_video = True


A :class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>` object's
attributes may be initialized with an existing capture template file. And then 
the individual attributes/setting may be customized:

    >>> template = omniscript.CaptureTemplate("Classic-CTD.ctf")
    >>> template.general.name = "Python CTD"
    >>> template.general.buffer_size = 134217728
    >>> template.general.options_capture_to_disk = True
    >>> template.general.option_start_capture = True
    >>> capt = engine.create_capture(template)

Note: the attributes of the
:class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>` object
have PEP 8 compliant names. OmniPeek's Capture Options dialog General view's
'Buffer size' control, is the buffer_size attribute. If you examine the XML,
look for the tag 'BufferSize'. For both the Python attribute and XML tag the
Buffer Size is specified in bytes, while the control in the Capture Options
dialog is specified in megabytes. To convert from megabytes to bytes, multiply
the value by 1,048,576.

Note: only the general attribute (a
:class:`GeneralSettings <omniscript.capturetemplate.GeneralSettings>`),
adapter attribute (a
:class:`AdapterSettings <omniscript.capturetemplate.AdapterSettings>`),
filter attribute (a list of
:class:`FilterSettings <omniscript.capturetemplate.FilterSettings>`), and the 
start_trigger and stop_trigger (both are
:class:`TriggerSettings <omniscript.capturetemplate.TriggerSettings>`)
are customizable. All other parts of the capture template will have defaults or
the values in the supplied by the capture template file.

Note: when rendering a
:class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>` object to
text, if a capture template file was initially provided then that file will be
loaded into an :mod:`ElementTree <xml.etree.ElementTree>` object, updated and
the XML rendered to text.

Filters Options
^^^^^^^^^^^^^^^
:class:`FilterSettings <omniscript.capturetemplate.FilterSettings>` may also be
applied to a capture template;
:class:`FilterSettings <omniscript.capturetemplate.FilterSettings>` are in
'list' format and the values are case-sensitive (must appear in same exactly as
they do in OmniPeek):

    >>> template = omniscript.CaptureTemplate()
    >>> template.filter.filters = ['HTTP', 'mDNS Multicast', 'Service Discovery']

Packet File Indexing Options (aka Bloom Filters)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
:class:`IndexingSettings <omniscript.capturetemplate.IndexingSettings>` may also 
be applied to a capture template.
Note: when creating a Capture from a Capture Template, CTD must be enabled to 
enable :class:`IndexingSettings <omniscript.capturetemplate.IndexingSettings>`. 
If CTD is not enabled in the CaptureTemplate then 
:class:`IndexingSettings <omniscript.capturetemplate.IndexingSettings>` will 
always be disabled.

Note: :func:`CaptureTemplate's set_all() 
<omniscript.capturetemplate.CaptureTemplate.set_all()>` does not enable 
:class:`IndexingSettings <omniscript.capturetemplate.IndexingSettings>`. 
To enable all 
:class:`IndexingSettings <omniscript.capturetemplate.IndexingSettings>`, use
:func:`IndexingSettings' 
set_all(True) <omniscript.capturetemplate.IndexingSettings.set_all()>`.

To enable all :class:`IndexingSettings <omniscript.capturetemplate.IndexingSettings>`:

    >>> template = omniscript.CaptureTemplate()
    >>> template.general.options_capture_to_disk = True ## Requires CTD enabled
    >>> template.indexing.set_all() ## Enables all indexing options

To selectively enable each 
:class:`IndexingSettings <omniscript.capturetemplate.IndexingSettings>`:

    >>> template = omniscript.CaptureTemplate()
    >>> template.general.options_capture_to_disk = True ## Requires CTD enabled
    >>> template.indexing.option_application = True ## Enables 'Application' indexing
    >>> template.indexing.option_country     = True ## Enables 'Country' indexing
    >>> template.indexing.option_ethernet    = True ## Enables 'MAC Address' indexing
    >>> template.indexing.option_ipv4        = True ## Enables 'IP Address' indexing
    >>> template.indexing.option_ipv6        = True ## Enables 'IPv6 Address' indexing
    >>> template.indexing.option_mpls        = True ## Enables 'MPLS' indexing
    >>> template.indexing.option_port        = True ## Enables 'Port' indexing
    >>> template.indexing.option_protospec   = True ## Enables 'Protocol' indexing
    >>> template.indexing.option_vlan        = True ## Enables 'VLAN' indexing

Note: When performing a Forensic Search on a 
:class:`Capture <omniscript.capture.Capture>` with
:class:`IndexingSettings <omniscript.capturetemplate.IndexingSettings>` enabled, 
the :class:`ForensicTemplate <omniscript.forensictemplate.ForensicTemplate>` must
be enabled with :class:`option_indexing <omniscript.forensictemplate.ForensicTemplate>`.
See section, :ref:`my-reference-fs-bloom-label`

Modify Capture
--------------
Modify a capture with the OmniEngine's
:func:`modify_capture() <omniscript.omniengine.OmniEngine.modify_capture()>`
or the Capture's
:func:`modify() <omniscript.capture.Capture.modify()>`

    >>> # Find old capture first:
    >>> old_capture = engine.find_capture("myCapture")
    >>> # Get old capture's template:
    >>> new_capturetemplate = old_capture.get_capture_template()
    >>> # Apply changes to new capture template:
    >>> new_capturetemplate.general.name = 'Modified Capture'
    >>> new_capturetemplate.general.option_capture_to_disk = False
    >>> new_capturetemplate.adapter.name = 'eth0'
    >>> new_capturetemplate.analysis.set_all(False)
    >>> # Issue modify_capture by passing in new template and old capture's object:
    >>> engine.modify_capture(new_capturetemplate, old_capture)


Capture's Statistics Reports
****************************
New in OmniScript 0.5.0 is the ability to create reports from the statistics
classes:
:class:`NodeStatistic <omniscript.nodestatistic.NodeStatistic>`,
:class:`ProtocolStatistic <omniscript.protocolstatistic.ProtocolStatistic>` and
:class:`SummaryStatistic <omniscript.summarystatistic.SummaryStatistic>`.

:class:`SummaryStatistic <omniscript.summarystatistic.SummaryStatistic>`
------------------------------------------------------------------------
:class:`SummaryStatistic <omniscript.summarystatistic.SummaryStatistic>` returns
a list of SummaryStatistic objects.  
Examples on accessing 
:class:`SummaryStatistic <omniscript.summarystatistic.SummaryStatistic>`:

    >>> ## get_summary_stats: Returns a list of SummaryStatistics objects:
        >>> summary = engine.find_capture("myCapture").get_summary_stats()
        >>> print summary
        ...
        {'Flow Tracker': {'Flows (current)': SummaryStatistic: Flows (current) = 2277, 
        'Max': SummaryStatistic: Max = 50000, 'Flows (all)': SummaryStatistic: 
        Flows (all) = 2277, 'Flows (recycled)':
        ...

        >>> ## To get a single summary stat value:
        >>> summary = engine.find_capture("myCapture").get_summary_stats()
        >>> print('Network: Total Packets', summary['Network']['Total Packets'].value)
        ...
        ('Network: Total Packets', 2342859)

        >>> ## To get summary stats from a category, e.g. Counts:
        >>> summary = engine.find_capture("myCapture").get_summary_stats()
        >>> counts = summary['Counts']
        >>> for key, value in counts.iteritems():
        >>>     print('Counts:', value.name, value.value)
        ...
        ('Counts:', 'IP Addresses', 456)
        ('Counts:', 'AppleTalk Addresses', 0)
        ('Counts:', 'IPX Addresses', 0)
        ('Counts:', 'IPv6 Addresses', 81)
        ('Counts:', 'Physical Addresses', 234)
        ('Counts:', 'Protocols', 63)
        ('Counts:', 'DECnet Addresses', 0)

:class:`NodeStatistic <omniscript.nodestatistic.NodeStatistic>`
---------------------------------------------------------------
:class:`NodeStatistic <omniscript.nodestatistic.NodeStatistic>` returns
a list of NodeStatistic objects. Each NodeStatistic object contains a list of
attributes.  For a list of the attributes, click on
:class:`NodeStatistic <omniscript.nodestatistic.NodeStatistic>`.
Examples on accessing 
:class:`NodeStatistic <omniscript.nodestatistic.NodeStatistic>`:

    >>> ## get_node_stats: Returns a list of NodeStatistic objects:
        >>> node = engine.find_capture("myCapture").get_node_stats()
        >>> print node
        ...
        [NodeStatistic: 00:10:49:00:42:3F, NodeStatistic: 10.4.2.122, 
        NodeStatistic: 00:10:49:19:31:80, NodeStatistic: 10.4.58.30, 
        NodeStatistic: 00:50:56:97:AB:D6, NodeStatistic: 10.4.2.207
        ...

        >>> ## Iterating through a list of NodeStatistic objects and finding a
        >>> ## matching node:
        >>> mynode = next(n for n in nodes if n.name == "74.125.224.105")
        >>> print 'Name/Node:', mynode.name
        >>> print 'Total Bytes:', mynode.total_bytes
        >>> print 'Packets Sent:', mynode.packets_sent
        >>> print 'Packets Received:', mynode.packets_received
        >>> print 'Broadcast/Multicast Packets:', mynode.broadcast_multicast_packets
        ...
        Name/Node: 74.125.224.105
        Total Bytes: 33538
        Packets Sent: 24
        Packets Received: 7
        Broadcast/Multicast Packets: 0
        ...

:class:`ProtocolStatistic <omniscript.protocolstatistic.ProtocolStatistic>`
---------------------------------------------------------------------------
:class:`ProtocolStatistic <omniscript.protocolstatistic.ProtocolStatistic>` returns
a list of ProtocolStatistic objects. Each ProtocolStatistic object contains a list of
attributes.  For a list of the attributes, click on
:class:`ProtocolStatistic <omniscript.protocolstatistic.ProtocolStatistic>`.
Examples on accessing
:class:`ProtocolStatistic <omniscript.protocolstatistic.ProtocolStatistic>`:

        >>> ## get_protocol_stats: Returns a list of ProtocolStatistic objects:
        >>> prots = engine.find_capture("myCapture").get_protocol_stats()
        >>> ## calling 'flatten' method to generate a 'Flat' view of protocols
        >>> ## like in OmniPeek:
        >>> p_flat = omniscript.protocolstatistic.flatten(prots, True)
        >>>
        >>> for p in p_flat:
        >>>     print 'Protocol:', p.name
        >>>     print 'Bytes:', p.bytes
        >>>     print 'Packets:', p.packets
        ...
        Protocol: 802.1
        Bytes: 1920
        Packets: 30
        Protocol: ARP Request
        Bytes: 11520
        Packets: 180
        Protocol: ARP Response
        Bytes: 4736
        Packets: 74
        ...

        >>> ## Iterating through a list of ProtocolStatistic objects and finding a
        >>> ## matching protocol:
        >>> myprot = next(p for p in prots if p.name == "HTTPS")
        >>> print 'Protocol:', myprot.name
        >>> print 'Bytes:', myprot.bytes
        >>> print 'Packets:', myprot.packets
        ...
        Protocol: HTTPS
        Bytes: 16648547
        Packets: 18667
        ...

:class:`ApplicationStatistic <omniscript.applicationstatistic.ApplicationStatistic>`
------------------------------------------------------------------------------------
:class:`ApplicationStatistic <omniscript.applicationstatistic.ApplicationStatistic>` returns
a list of ApplicationStatistic objects. Each ApplicationStatistic object contains a list of
attributes.  For a list of the attributes, click on
:class:`ApplicationStatistic <omniscript.applicationstatistic.ApplicationStatistic>`.
Examples on accessing
:class:`ApplicationStatistic <omniscript.applicationstatistic.ApplicationStatistic>`:

        >>> ## get_application_stats: Returns a list of ApplicationStatistic objects:
        >>> applications = engine.find_capture("myCapture").get_application_stats()
        >>>
        >>> for a in applications:
        >>>    print "Application: ", a.name, "Number of packets: ", a.packets
        ...
        Application:  RDP Number of packets:  1080
        Application:  CIFS Number of packets:  19
        Application:  TCP Number of packets:  782
        Application:  UDP Number of packets:  201
        ...

        >>> ## Iterating through a list of ApplicationStatistic objects and finding a
        >>> ## matching application, e.g. TCP:
        >>> tcpApp = next(n for n in applications if n.name == "TCP")
        >>> print 'Number of packets:', tcpApp.packets
        ...
        Number of packets: 4569
        ...


:class:`CountryStatistic <omniscript.countrystatistic.CountryStatistic>`
------------------------------------------------------------------------
:class:`CountryStatistic <omniscript.countrystatistic.CountryStatistic>` returns
a list of CountryStatistic objects. Each CountryStatistic object contains a list of
attributes.  For a list of the attributes, click on
:class:`CountryStatistic <omniscript.countrystatistic.CountryStatistic>`.
Examples on accessing
:class:`CountryStatistic <omniscript.countrystatistic.CountryStatistic>`:

        >>> ## get_country_stats: Returns a list of CountryStatistic objects:
        >>> country = engine.find_capture("myCapture").get_country_stats()
        >>>
        >>> for c in country:
        >>>     print 'Country Code:', c.country_code
        ...
        Country Code: 18259
        Country Code: 512
        Country Code: 21333
        Country Code: 256
        ...

        >>> ## Iterating through a list of CountryStatistic objects and finding a
        >>> ## matching country (e.g. Private Network's code is 256):
        >>> privNetwork = next(n for n in country if n.country_code == 256)
        >>> print 'Packets from this country:', privNetwork.packets_from
        >>> print 'Packets to this country:', privNetwork.packets_to
        ...
        Packets from this country: 4569
        Packets to this country: 4663
        ...

To write the statistic reports (equivalent to 'Save Summary Statistics', 'Save Node
Statistics', and 'Save Protocol Statistics' in OmniPeek) to a text file by calling
these functions:

* :func:`save_summarystats() <omniscript.savestatistics.save_summarystats()>`
* :func:`save_nodestats() <omniscript.savestatistics.save_nodestats()>`
* :func:`save_protocolstats() <omniscript.savestatistics.save_protocolstats()>`

Examples on calling functions (
:func:`save_summarystats() <omniscript.savestatistics.save_summarystats()>`,
:func:`save_nodestats() <omniscript.savestatistics.save_nodestats()>`,
:func:`save_protocolstats() <omniscript.savestatistics.save_protocolstats()>`)
for writing/saving the statistic reports to a text file:

        >>> ## Writing SummaryStatistics report:
        >>> sums_file = 'c:\\automation\\tests\\sumstats.csv'
        >>> omniscript.savestatistics.save_summarystats(engine.find_capture("myCapture"), sums_file)

        >>> # Writing NodeStatistics report:
        >>> nodes_file = 'c:\\automation\\tests\\nodestats.csv'
        >>> omniscript.savestatistics.save_nodestats(engine.find_capture("myCapture"), nodes_file)

        >>> # Writing ProtocolStatistics report:
        >>> prots_file = 'c:\\automation\\tests\\protstats.csv'
        >>> omniscript.savestatistics.save_protocolstats(engine.find_capture("myCapture"), prots_file)

File Operations
***************
OmniScript has the ability to list, retrieve and send files from the OmniEngine's 
host operating system. 

:func:`get_file() <omniscript.omniengine.OmniEngine.get_file()>`
----------------------------------------------------------------
To retrieve (or download) any file from the OmniEngine's host operating system, use
:func:`get_file() <omniscript.omniengine.OmniEngine.get_file()>` function. 

Args:
  1) source (str): name of the file to get. If not fully qualified then source will
     be relative to the engine's Data Folder
  2) destination (str, optional): defaults to the current directory; otherwise,
     the path and file name to be saved as

Example:

    >>> engine.get_file('Capture.pkt')

Will copy the file C:\\Program Data\\Savvius\\OmniEngine\\Capture.pkt from
the OmniEngine's host file system to the to the scripts current working directory 
which is usually the file C:\\Python27\\Capture.pkt.

:func:`get_file_list()` and :func:`get_forensic_file_list()`
------------------------------------------------------------
To get a list of any files, on the file system, and their attributes, use
:func:`get_file_list() <omniscript.omniengine.OmniEngine.get_file_list()>` function.
However, if you want to get a list of files used in Forensic Searches or CTD files 
(i.e. files listed in OmniPeek's 'Files' tab), then use
:func:`get_forensic_file_list() <omniscript.omniengine.OmniEngine.get_forensic_file_list()>`
instead.

How do :func:`get_file_list() <omniscript.omniengine.OmniEngine.get_file_list()>` and
:func:`get_forensic_file_list() <omniscript.omniengine.OmniEngine.get_forensic_file_list()>`
differ?

- :func:`get_file_list() <omniscript.omniengine.OmniEngine.get_file_list()>` returns 
  a list of :class:`FileInformation <omniscript.fileinformation.FileInformation>` objects 
  which only have attributes of name, date of modification and file size

- :func:`get_forensic_file_list() <omniscript.omniengine.OmniEngine.get_forensic_file_list()>` 
  returns a list of :class:`ForensicFile <omniscript.forensicfile.ForensicFile>` objects 
  which have attributes about the Capture the file is associated with (e.g. capture_name, 
  packet_count, start_time, end_time, etc.)

Example for :func:`get_file_list() <omniscript.omniengine.OmniEngine.get_file_list()>`:

From an
:class:`OmniEngine <omniscript.omniengine.OmniEngine>` object get the list of
files in the OmniEngine's Data Folder:

    >>> fl = engine.get_file_list()
    >>> for fi in fl:
    >>>     print('%s %s, size: %d' % ('*' if fi.is_folder() else ' ', fi.name, fi.size))
    File List:
    * C:\ProgramData\Savvius\OmniEngine\, size: 0
      omni.db, size: 152616960
      omniauditlog.db, size: 151552
      omniauditlog.db-shm, size: 32768
      omniauditlog.db-wal, size: 2220712
      omnilog.db, size: 212992
      omnilog.db-shm, size: 32768
      omnilog.db-wal, size: 663352
      omnitrace.log, size: 60944347
      Python Capture\Python Capture-2014-07-01T12.34.56.000.pkt, size: 65535

Notice that the first entry is a Folder, which by default is the OmniEngine's
Data Folder.

Example for
:func:`get_forensic_file_list() <omniscript.omniengine.OmniEngine.get_forensic_file_list()>`:

        >>> for ff in engine.get_forensic_file_list():
        >>>     print ff.path
        >>>     print 'Packet count:', ff.packet_count
        >>>     print 'Size:', ff.size
        ...
        \var\lib\omni\data\myCapture\myCapture-2014-12-19T15.40.44.575.pkt
        Packet Count: 12491
        Size: 16775740

:func:`delete_file()  <omniscript.omniengine.OmniEngine.delete_file()>`
-----------------------------------------------------------------------
To delete files from the OmniEngine's host operating system, use the
:func:`delete_file()  <omniscript.omniengine.OmniEngine.delete_file()>` function.

Example on deleting all 'ForensicFiles' (or CTD files) by capture_name:

        >>> for f in engine.get_forensic_file_list():
        >>>     if 'myCapture' in [f.capture_name]:
        >>>         engine.delete_file([f.path])

Example on deleting a single CTD file by filename:

        >>> engine.delete_file('wireless-sample.pkt')

:func:`send_file()  <omniscript.omniengine.OmniEngine.send_file()>`
-------------------------------------------------------------------
To upload a file to the OmniEngine's host operating system, use the
:func:`send_file()  <omniscript.omniengine.OmniEngine.send_file()>` function.

Args:
   1) source (string): the name of the local file to send
   2) destination (string, optional): defaults to the engine's Data Folder; otherwise,
      the path and name of the file

Example:

        >>> my_file = 'sample.pkt'
        >>> engine.send_file(my_file)


File Adapter
************
the :class:`FileAdapter <omniscript.fileadapter.FileAdapter>` class,
uses a Capture File as the source of the packets that are captured by the
Capture. the object allows you to limit the number of times the file is repeated
(or replayed), the replay speed and the timestamping mode.

Create a
:class:`FileAdapter <omniscript.fileadapter.FileAdapter>` class object, set it
as the adapter for the capture template, then create, start and refresh the
capture:

    >>> file_adpt = omniscript.FileAdapter(r'c:\temp\capture.pkt')
    >>> template.set_adapter(file_adpt)
    >>> file_capt = engine.create_capture(template)
    >>> file_capt.start()
    >>> file_capt.refresh()

Alternately, Capture Template may be configured with a file adapter without a 
:class:`FileAdapter <omniscript.fileadapter.FileAdapter>` object. At a minimum
set the two attributes type and name in the 
:class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`'s adapter
attribute:

    >>> template.adapter.type = omniscript.ADAPTER_TYPE_FILE
    >>> template.adapter.name = r'c:\temp\capture.pkt'

Filters and FilterNodes
***********************
OmniScript allows a script to manage the Filters on the OmniEngine by creating new filters,
finding and deleting existing filters. A Filter, the :class:`Filter <omniscript.filter.Filter>` 
class consists of FilterNode(s) :class:`FilterNode <omniscript.filternode.FilterNode>`.

Create a :class:`Filter <omniscript.filter.Filter>` class object and a
:class:`FilterNode <omniscript.filternode.FilterNode>` class object.  Then set the
:class:`FilterNode <omniscript.filternode.FilterNode>` as the criteria for the
filter:

        >>> ## Example: Create Filter where:
        >>> ##              - EthernetAddress = '00:50:56:97:*.*' to any address,
        >>> ##                    both directions
        >>> ##              AND:
        >>> ##              - Protocol = 'IGMP'
        >>> ##
        >>> my_filter               = omniscript.Filter('!My_Test_Filter')
        >>> ## creating AddressNode with wildcards for the octets:
        >>> addr_node               = omniscript.AddressNode()
        >>> addr_node.address_1     = omniscript.EthernetAddress('00:50:56:97:*:*')
        >>> addr_node.accept_1_to_2 = True
        >>> addr_node.accept_2_to_1 = True
        >>> addr_node.inverted      = False ## (optional, default is False)
        >>> ## creating ProtocolNode:
        >>> protocol_node           = omniscript.ProtocolNode()
        >>> protocol_node.set_protocol('IGMP')
        >>> ## linking AddressNode and ProtocolNode:
        >>> addr_node.and_node      = protocol_node
        >>> ## applying criteria to filter and adding filter to engine:
        >>> my_filter.criteria      = addr_node
        >>> engine.add_filter(my_filter)
 
Find filter and delete filter:

        >>> ## Find filter:
        >>> found_filter = engine.find_filter('!My_Test_Filter')
        >>> print found_filter
        >>> ## Delete filter:
        >>> engine.delete_filter(found_filter)

Forensic Searches
*****************
To create a forensic search, first instantiate a 
:class:`ForensicTemplate <omniscript.forensictemplate.ForensicTemplate>` object.
To see a list of the forensic search options, see
:class:`ForensicTemplate <omniscript.forensictemplate.ForensicTemplate>` attributes.
Then instantiate a :class:`ForensicSearch <omniscript.forensicsearch.ForensicSearch>`
object by passing in the 
:class:`ForensicTemplate <omniscript.forensictemplate.ForensicTemplate>` instance.

Performing Forensic Search
--------------------------
Example on performing a Forensic Search on an engine Capture:

        >>> capture = engine.find_capture("myCapture")
        >>> 
        >>> ## Creating the ForensicTemplate:
        >>> f_template = omniscript.ForensicTemplate()
        >>> f_template.name = 'myForensicSearch'
        >>> f_template.start_time = capture.start_time # Requires PeekTime format
        >>> f_template.end_time = capture.stop_time # Requires PeekTime format
        >>> f_template.option_packets = True
        >>> f_template.option_summary = True
        >>> f_template.option_expert = True
        >>> f_template.option_graphs = True
        >>> f_template.option_history = True
        >>> f_template.option_log = True
        >>> f_template.option_network = True
        >>> f_template.option_node = True
        >>> f_template.option_node_protocol_detail = True
        >>> f_template.option_plugins = True
        >>> f_template.option_protocol = True
        >>> f_template.option_size = True
        >>> f_template.option_top_talkers = True
        >>> f_template.option_voice = True
        >>> f_template.option_web = True
        >>> f_template.option_wireless_channel = True
        >>> f_template.option_wireless_node = True
        >>> 
        >>> ## Creating the ForensicSearch:
        >>> f_search = engine.create_forensic_search(f_template)
        >>> 
        >>> ## Wait for ForensicSearch's status to complete:
        >>> while f_search.status != 2:  # 2 for Complete
        >>>     time.sleep(5)
        >>>     f_search.refresh()
        >>> 
        >>> print('Forensic_Search Packet Count=', f_search.packet_count)
        >>> 
        >>> ## Get statistics:
        >>> sums = f_search.get_summary_stats()
        >>> prots = f_search.get_protocol_stats()
        >>> nodes = f_search.get_node_stats()
        >>> 
        >>> print sums

.. _my-reference-fs-bloom-label:

Forensic Search on Packet File Indexed Capture
----------------------------------------------
Note: When performing a Forensic Search on a
:class:`Capture <omniscript.capture.Capture>` with
:class:`IndexingSettings <omniscript.capturetemplate.IndexingSettings>` enabled,
the :class:`ForensicTemplate <omniscript.forensictemplate.ForensicTemplate>` must
be enabled with :class:`option_indexing <omniscript.forensictemplate.ForensicTemplate>`:

    >>> f_template = omniscript.ForensicTemplate()
    >>> f_template.capture_name = 'Capture1'
    >>> f_template.option_indexing = True ## Must enable Indexing


Forensic Search on Uploaded Files
---------------------------------
Example on performing a Forensic Search on an uploaded file (wireless packets):

        >>> ## Upload file to engine:
        >>> engine.send_file(my_file)
        >>> 
        >>> ## Get list of forensic files, need to iterate through for matching filename:
        >>> file_list = next(f for f in engine.get_forensic_file_list() if f.name == my_file)
        >>> 
        >>> print 'forensic file: path =', [file_list.path]   
        >>> print 'forensic file: size =', [file_list.size]
        >>> print 'forensic file: file # of packets =', [file_list.packet_count]
        >>> 
        >>> ## Creating the ForensicTemplate:
        >>> f_template = omniscript.ForensicTemplate()
        >>> f_template.name = my_file
        >>> f_template.files = [file_list.path]
        >>> f_template.option_packets = True
        >>> f_template.option_summary = True
        >>> # Other available options:
        >>> f_template.option_expert = True
        >>> f_template.option_graphs = True
        >>> f_template.option_history = True
        >>> f_template.option_log = True
        >>> f_template.option_network = True
        >>> f_template.option_node = True
        >>> f_template.option_node_protocol_detail = True
        >>> f_template.option_plugins = True
        >>> f_template.option_protocol = True
        >>> f_template.option_size = True
        >>> f_template.option_top_talkers = True
        >>> f_template.option_voice = True
        >>> f_template.option_web = True
        >>> f_template.option_wireless_channel = True
        >>> f_template.option_wireless_node = True
        >>> # By Default, forensic searches will assume media_type is 'Ethernet'.
        >>> # For Wireless, Frame Relay or PPP files, need to specify media_type and
        >>> # media_sub_types.
        >>> f_template.media_type = file_list.media_type
        >>> f_template.media_sub_type = file_list.media_sub_type
        >>> 
        >>> ## Creating the ForensicSearch:
        >>> f_search = engine.create_forensic_search(f_template)
        >>> 
        >>> ## Wait for ForensicSearch's status to complete but timeout after 30 secs:
        >>> timeout = time.time() + 30 # 30 secs from now
        >>> while f_search.status != 2:  # 2 for Complete
        >>>     time.sleep(1)
        >>>     f_search.refresh()
        >>>     if time.time() > timeout:
        >>>          print('Forensic search did not complete within max time of 30 secs')
        >>>          break
        >>> 
        >>> print('Forensic_Search Packet Count=', f_search.packet_count)
        >>> 
        >>> ## Get statistics:
        >>> summary = f_search.get_summary_stats()
        >>> protocols = f_search.get_protocol_stats()
        >>> nodes = f_search.get_node_stats()
        >>> 
        >>> ## Print 'Wireless' category from Summary Statistics:
        >>> wireless = summary['Wireless']
        >>> for key, value in wireless.iteritems():
        >>>     print('Wireless:', value.name, value.value)
        >>> 
        >>> ## Delete the forensic search:
        >>> engine.delete_forensic_search(f_template)


PeekTime Conversions
********************
OmniEngine uses :class:`PeekTime <omniscript.peektime.PeekTime>` to store all timestamps in **UTC**
format.  
PeekTime is the number of nanoseconds since midnight January 1, 1601.

Converting PeekTime to and from Python ctime:
    Use :func:`PeekTime.ctime() <omniscript.peektime.PeekTime.ctime>`

    Examples:

    >>> print 'PeekTime ctime value typed in manually:', omniscript.PeekTime(13083186172000000000).ctime()
    ...
    PeekTime ctime value typed in manually: Tue Aug 04 19:22:52 2015

    >>>
    OmniScript classes already imported 'PeekTime' so can just call on the function names directly like this:
    >>> capture = engine.find_capture('eth0')
    >>> print capture.start_time.ctime()
    ...
    Capture Start Time using PeekTime.ctime():  Thu Aug 13 15:52:25 2015    


Python System Time (or Epoch):
    Use :func:`PeekTime.time() <omniscript.peektime.PeekTime.time>`

    Examples:

    >>> ## Return Python System Time, aka Epoch, from PeekTime.time():
    >>> print capture.start_time.time()
    ... 
    1439477545
    >>> ## Convert Python System Time, aka Epoch, to PeekTime: 
    >>> print omniscript.PeekTime.system_time_to_peek_time(1439477545)

String-formatted Time (e.g. '08/04/2015 16:36:17'):

    Examples:

    >>> ## Our products display timestamps in the following string format: "08/04/2015 16:36:17"
    >>> ## To convert PeekTime to the above string format, first convert to Epoch:
    >>> import time, omniscript
    >>>
    >>> epoched_capture_start_time = capture.start_time.time()
    >>> print time.strftime('%Y/%m/%d %H:%M:%S', time.localtime(epoched_capture_start_time))
    ...
    2015/08/13 15:52:25
    >>>
    >>> ## To convert string-formatted time to PeekTime, first convert to Epoch time:
    >>> d = '08/04/2015 16:36:17'
    >>> f = '%m/%d/%Y %H:%M:%S'
    >>> epoch = int(time.mktime(time.strptime(d,f)))
    >>> print 'Epoch time:', epoch
    >>> print 'PeekTime:', omniscript.PeekTime.system_time_to_peek_time(epoch)
    ...
        Epoch time: 1438702577
        PeekTime: 13083176177000000000


Trigger Settings
****************
To set trigger events on captures, the
:class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>` instance
should include :class:`TriggerSettings <omniscript.capturetemplate.TriggerSettings>`.

Time-based triggers require the following input parameter formats:

* Absolute time triggers: inputted in :class:`PeekTime <omniscript.peektime.PeekTime>`
  format, which is the number of nanoseconds since January 1, 1601

* Elapsed time triggers: inputted in nanoseconds

Example on setting Time-based triggers:

        >>> import omniscript
        >>> import time
        >>>
        >>> ## Trigger by Absolute Time:
        >>> start_time = time.time() + 30
        >>> peek_start_time = omniscript.PeekTime.system_time_to_peek_time(start_time)
        >>> end_time = time.time() + 120
        >>> peek_end_time = omniscript.PeekTime.system_time_to_peek_time(end_time)
        >>> 
        >>> abs_template = omniscript.CaptureTemplate()
        >>> abs_template.general.name = "Absolute-time Trigger"
        >>> abs_template.start_trigger.enabled = True
        >>> abs_template.start_trigger.time.enabled = True
        >>> abs_template.start_trigger.time.time = peek_start_time
        >>> abs_template.stop_trigger.enabled = True
        >>> abs_template.stop_trigger.time.enabled = True
        >>> abs_template.stop_trigger.time.time = peek_end_time
        >>> abs_capt = engine.create_capture(abs_template)
        >>> abs_capt.start()
        >>>
        >>> ## Trigger by Elapsed Time with Repeats:
        >>> elps_template = omniscript.CaptureTemplate()
        >>> elps_template.general.name = "Elapsed-time Trigger"
        >>> elps_template.start_trigger.enabled = True
        >>> elps_template.start_trigger.time.enabled = True
        >>> elps_template.start_trigger.time.option_use_elapsed = True
        >>> elps_template.start_trigger.time.time = 10000000000 ## requires in nanoseconds
        >>> elps_template.stop_trigger.enabled = True
        >>> elps_template.stop_trigger.time.enabled = True
        >>> elps_template.stop_trigger.time.option_use_elapsed = True
        >>> elps_template.stop_trigger.time.time = 30000000000 ## requires in nanoseconds
        >>> ## Repeat the trigger:
        >>> elps_template.repeat_trigger = 1 # 1 = enabled
        >>> elps_capt = engine.create_capture(elps_template)
        >>> elps_capt.start()

Example on setting Filter-based triggers:

        >>> flt_template = omniscript.CaptureTemplate()
        >>> flt_template.general.name = "Filter Trigger"
        >>> flt_template.start_trigger.enabled = True
        >>> flt_template.start_trigger.filter.enabled = True
        >>> flt_template.start_trigger.filter.filters = ['my_trigger'] ## list of filter names 
        >>> flt_capt = engine.create_capture(flt_template)
        >>> flt_capt.start()

Example on setting Bytes-Captured triggers:

        >>> bytes_tmpl = omniscript.CaptureTemplate()
        >>> bytes_tmpl.general.name = "Bytes Trigger"
        >>> bytes_tmpl.start_trigger.enabled = True
        >>> bytes_tmpl.start_trigger.captured.enabled = True
        >>> bytes_tmpl.start_trigger.captured.bytes.captured.bytes = 104857600     
        >>> bytes_capt = engine.create_capture(bytes_tmpl)
        >>> bytes_capt.start()

.. _query-expert-tutorial:

Query Expert (Flows, etc)
*************************
Query the Expert to get flow and problem information. Create and configure one 
or more :class:`ExpertQuery <omniscript.expertquery.ExpertQuery>`
objects and pass them to
:func:`query_expert() <omniscript.capture.Capture.query_expert>`.
Submit more than one query at a time so the results are all from the same time period.
See :ref:`expert-tables-section` for the list of tables and columns.
The results are a list of :class:`ExpertResult <omniscript.expertresult.ExpertResult>`,
one for each :class:`ExpertQuery <omniscript.expertquery.ExpertQuery>`.

Example get the first 100 flows and problems:

        >>> querys = [omniscript.ExpertQuery('STREAM'), omniscript.ExpertQuery('EVENT_LOG')]
        >>> querys[0].columns = ['STREAM_ID', 'CLIENT_ADDRESS', 'CLIENT_PORT', 'SERVER_ADDRESS', 'SERVER_PORT']
        >>> querys[0].order = ['STREAM_ID']
        >>> querys[0].row_count = 100
        >>> querys[1].columns = ['PROBLEM_ID', 'STREAM_ID', 'MESSAGE']
        >>> querys[1].order = ['PROBLEM_ID']
        >>> querys[1].where = ['informational', 'minor', 'major', 'severe']
        >>> querys[1].row_count = 100
        >>> results = capt.query_expert(querys)
        >>> for r in results[0].rows:
        >>>     cip = omniscript.parse_ip_address(r['CLIENT_ADDRESS'])
        >>>     sip = omniscript.parse_ip_address(r['SERVER_ADDRESS'])
        >>>     print('%d [%s]:%d [%s]:%d' % (r['STREAM_ID'], str(cip), r['CLIENT_PORT'], str(sip), r['SERVER_PORT']))
        >>> for r in results[1].rows:
        >>>     print('%3d %3d %s' % (r['PROBLEM_ID'], r['STREAM_ID'], r['MESSAGE']))

Note that the STREAM_ID will ensure that each row is unique. If STREAM_ID is not included and there are multiple rows 
that all have the same column values, then only one row is returned.

Wireless Adapter Management
***************************
Managing wireless adapters is a multi-step process.

First retrieve the Hardware Options and get the first 
:class:`WirelessHardwareOptions <omniscript.hardwareoptions.WirelessHardwareOptions>` object:

        >>> hwo_list = engine.get_hardware_options_list()
        >>> if not hwo_list:
        >>>     exit(0)
        >>> ho = hwo_list[0]
        >>> if not isinstance(ho, omniscript.WirelessHardwareOptions):
        >>>     exit(0)

The :class:`WirelessHardwareOptions <omniscript.hardwareoptions.WirelessHardwareOptions>`'s 
channel_scanning attribute is the list of all valid channels as 
:class:`ChannelScanEntry <omniscript.hardwareoptions.ChannelScanEntry>` objects.

The find_channel_scanning_entry 
:func:`find_channel_scanning_entry() <omniscript.hardwareoptions.WirelessHardwareOptions.find_channel_scanning_entry>`
function will search the :class:`WirelessHardwareOptions <omniscript.hardwareoptions.WirelessHardwareOptions>`
channel_scanning list for all the matching entries. The search can be done by:
channel, frequency or band. Set the channel and disable channel scanning.

        >>> ch2_list = ho.find_channel_scanning_entry(2, 'channel')
        >>> ch2 = ch2_list[0]
        >>> ho.set_channel(ch2)
        >>> ho.set_channel_scanning(False)

Then find the :class:`Adapter <omniscript.adapter.Adapter>` named 'Wi-Fi and get its 
:class:`AdapterConfiguration <omniscript.adapterconfiguration.AdapterConfiguration>`:

        >>> al = engine.get_adapter_list()
        >>> wa = engine.find_adapter("Wi-Fi")
        >>> if wa is None:
        >>>     exit(0)
        >>> ac = engine.get_adapter_configuration(wa)

Finally commit the changes:

        >>> engine.set_hardware_options(ho)
        >>> engine.set_adapter_configuration(ac)

To setup and enable channel scanning, get the 
:class:`WirelessHardwareOptions <omniscript.hardwareoptions.WirelessHardwareOptions>`
and :class:`AdapterConfiguration <omniscript.adapterconfiguration.AdapterConfiguration>`,
modify the channel scanning entries as desired, enable channel scanning and commit
the changes:

        >>> for cs in ho.channel_scanning:
        >>>     if cs.channel_band == omniscript.WIRELESS_BAND_N20MHZ:
        >>>         cs.enabled = True
        >>>         cs.duration = 500     # in milliseconds
        >>>     else:
        >>>         cs.enabled = False
        >>>         cs.duration = 0
        >>> ho.set_channel_scanning(True)
        >>> engine.set_hardware_options(ho)
        >>> engine.set_adapter_configuration(ac)

TCPDump Remote Adapters
***********************
Create a TCPDump Remote Adapter using the tcpdump Analysis Module.

        >>> tcpdump = omniscript.TCPDump(engine, "192.168.1.100")
        >>> tcpdump.login("account", "password")
        >>> interface_list = tcpdump.get_adapter_list()
        >>> interface = next((i for i in interface_list if i.name == "eth0"), None)
        >>> template = omniscript.TCPDumpTemplate(interface)
        >>> new_adapter_name = tcpdump.create_adapter(template)
        >>> print new_adapter_name
        tcpdump 192.168.1.100:eth0

Find the new adapter in the engine's adapter list:

        >>> adapter_list = engine.get_adapter_list()
        >>> new_adapter = omniscript.find_adapter(adapter_list, new_adapter_name)
        >>> print new_adapter.name
        tcpdump 192.168.1.100:eth0

To rename the new adapter:

        >>> new_adapter.rename("My New Adapter")

To delete all the adapters created by Analysis Modules:

        >>> atd = [a for a in eal if a.adapter_type == omniscript.ADAPTER_TYPE_PLUGIN]
        >>> engine.delete_adapter(atd)

If the tcpdump Analysis Module is not installed on the engine, then an OmniError 
exception will be raised.

        >>> tcpdump = omniscript.TCPDump(engine, "192.168.1.200")
        The engine does not have the tcpdump plugin installed.

Display the Attributes of an Object
***********************************

To display the complete list of an object's attributes (e.g.
:class:`Adapter <omniscript.adapter.Adapter>`,
:class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>`,
:class:`EngineStatus <omniscript.enginestatus.EngineStatus>`, etc):

    >>> import omniscript
    >>> 
    >>> omni = omniscript.OmniScript()
    >>> engine = omni.connect('192.168.1.100', 6367, 'Default', None, 'user', 'password')
    >>> al = engine.get_adapter_list()
    >>> adapter = al[0]
    >>> 
    >>> for key, value in adapter.__dict__.iteritems():
    ...     print key, ':', value
    ... 
    media_sub_type : 0
    description : Local Area Connection
    _engine : OmniEngine
    savvius_api : True
    device_name : Broadcom Gigabit Controller
    link_speed : 1000000000
    interface_features : 0
    address : 00:23:AE:80:AE
    media_type : 0
    logger : Logging object
    type : 1
    id : 0007
    features : 0

To get a single attribute from :class:`Adapter <omniscript.adapter.Adapter>`:

    >>> print('Adapter address: %s' % adapter.address)
    Adapter address: 00:23:AE:80:AE


Converting 'pkt' files to Decoded Text files - pkt2dcd.exe
**********************************************************

Included in OmniScript is a Windows executable program, pkt2dcd.exe, for 
converting Savvius' 'pkt' trace files into decoded packets in plaintext 
format.
This is the equivalent of 'Save all Packets' as 'Decoded Packets (txt)' in
OmniPeek.

The executable is installed at:

        $PYTHONPATH\\Lib\\site-packages\\OmniScript-0.5.3-py2.7.egg\\omniscript

Usage is: pkt2dcd.exe [infile.pkt] [outfile.txt]

Example on calling 'pkt2dcd.exe' within Python:

        >>> import os, sys
        >>> import omniscript
        >>> import subprocess
        >>>
        >>> ## Generating filepath to 'pkt2dcd.exe':
        >>> pgm = next(p for p in sys.path if "omniscript" in p)
        >>> pgm += "\\omniscript\\pkt2dcd.exe"
        >>>
        >>> args = [pgm, "f:\\Downloads\\sample.pkt", "f:\\Downloads\\output.txt"]
        >>> subprocess.call(args)

.. _expert-tables-section:

Expert Tables
=============

Tables
******

===============  =================
Name             Description
===============  =================
STREAM           The Flow table
EVENT_LOG        The Event Log
EVENT_COUNTS     The Event Counts
HEADER_COUNTERS  The Header Counts
===============  =================


Columns
*******

=================================================  =======
Name                                               Type
=================================================  =======
NONE                                               None
ACCEPT_MATCHING                                    integer
APDEX                                              integer
APDEX_SAMPLE_COUNT                                 integer
APPLICATION                                        integer
APPLICATION_CONFIDENCE                             integer
AUX1                                               integer
BIT_RATE                                           integer
BYTE_COUNT                                         integer
CALL_ID                                            integer
CALL_STATUS                                        integer
CALLEE_ADDRESS                                     integer
CALLEE_PORT                                        integer
CALLER_ADDRESS                                     integer
CALLER_PORT                                        integer
CLIENT_ADDRESS                                     integer
CLIENT_CITY                                        integer
CLIENT_COUNTRY                                     integer
CLIENT_COUNTRY_CODE                                integer
CLIENT_END_TIME                                    integer
CLIENT_HOP_COUNT                                   integer
CLIENT_LATITUDE                                    integer
CLIENT_LONGITUDE                                   integer
CLIENT_PORT                                        integer
CLIENT_SENT_BYTE_COUNT                             integer
CLIENT_SENT_PACKET_COUNT                           integer
CLIENT_START_TIME                                  integer
CLIENT_TCP_WINDOW_MAX                              integer
CLIENT_TCP_WINDOW_MIN                              integer
CODEC                                              integer
COLUMN_ATTRIBUTES                                  integer
COLUMN_ID                                          string
COLUMN_ID_EVENT_LAYER_ID                           integer
COLUMN_ID_EVENT_LAYER_VALUE                        integer
COLUMN_NAME                                        integer
CONTENT_TYPE                                       integer
CONTEXT                                            integer
CONTROL_FLOW_COUNT                                 integer
CONTROL_PACKET_COUNT                               integer
DEGRADATION_CAUSE                                  integer
DELAY                                              integer
DELAY_BEST                                         integer
DELAY_SAMPLE_COUNT                                 integer
DELAY_WORST                                        integer
DEST_ADDRESS                                       integer
DEST_PORT                                          integer
DURATION                                           integer
END_CAUSE                                          integer
END_TIME                                           integer
EVENT_ASSIST_LEFT                                  integer
EVENT_ASSIST_LOGSCALE                              integer
EVENT_ASSIST_RIGHT                                 integer
EVENT_ASSIST_SHOWN                                 integer
EVENT_CONFIGURE_SHOWN                              integer
EVENT_FORMAT                                       integer
EVENT_GROUP                                        integer
EVENT_GUID                                         integer
EVENT_MESSAGE                                      integer
EVENT_MINPERIOD_MAX                                integer
EVENT_MINPERIOD_MIN                                integer
EVENT_MINPERIOD_SHOWN                              integer
EVENT_MINPERIOD_UNITS                              integer
EVENT_MULTIPLIER                                   integer
EVENT_NAME                                         integer
EVENT_SENSITIVITY                                  integer
EVENT_SERIAL_NUMBER                                integer
EVENT_SEVERITY_MAX                                 integer
EVENT_SUB_GROUP                                    integer
EVENT_TIME                                         integer
EVENT_VALUE_MAX                                    integer
EVENT_VALUE_MIN                                    integer
EVENT_VALUE_SHOWN                                  integer
EVENT_VALUE_UNIT                                   integer
FLAGS                                              integer
FLOW_INDEX                                         integer
FLOW_TYPE                                          integer
FROM                                               integer
FROM_ABBREV                                        integer
GATEKEEPER_ADDRESS                                 integer
GATEKEEPER_PORT                                    integer
GATEWAY_ASSIGNED_CALL_ID                           integer
HIGHLIGHT                                          boolean
HOP_COUNT                                          integer
HOST                                               integer
ICON_STATE                                         integer
INDEX                                              integer
IP_PROTOCOL                                        integer
IS_FROM_CLIENT                                     integer
JITTER                                             integer
MEDIA_EXTRACTABLE                                  integer
MEDIA_FLOW_COUNT                                   integer
MEDIA_FLOW_STATUS                                  integer
MEDIA_FRAME_COUNT                                  integer
MEDIA_PACKET_COUNT                                 integer
MEDIA_TYPE                                         integer
MESSAGE                                            string
MOS_A                                              integer
MOS_AV                                             integer
MOS_CQ                                             integer
MOS_LOW                                            integer
MOS_LQ                                             integer
MOS_NOM                                            integer
MOS_PQ                                             integer
MOS_V                                              integer
NAME                                               string
NODE_1                                             integer
NODE_2                                             integer
NODEPAIR                                           integer
ONE_WAY_DELAY                                      integer
OTHER_PACKET_NUMBER                                integer
PACKET_COUNT                                       integer
PACKET_LOSS_PERCENT                                integer
PACKET_NUMBER                                      integer
PAGE_REQUEST_ID                                    integer
PDD                                                integer
PROBLEM_COUNT                                      integer
PROBLEM_ID                                         integer
PROBLEM_SUMMARY_LIST                               integer
PROTOCOL                                           integer
PROTOCOL_LAYER                                     integer
PROTOSPEC                                          integer
R_FACTOR_CONVERSATIONAL                            integer
R_FACTOR_G107                                      integer
R_FACTOR_LISTENING                                 integer
R_FACTOR_NOMINAL                                   integer
REFERER                                            integer
RELATIVE_TIME                                      integer
REQUEST_HEADER                                     integer
REQUEST_ID                                         integer
REQUEST_ID_LIST                                    integer
REQUEST_PAYLOAD_BYTE_COUNT                         integer
RESPONSE_CODE                                      integer
RESPONSE_HEADER                                    integer
RESPONSE_PAYLOAD_BYTE_COUNT                        integer
RESPONSE_TEXT                                      integer
RTCP_FLAG_LIST                                     integer
RTCP_PACKET_NUMBER_LIST                            integer
RTCP_TIME_LIST                                     integer
RTP_BYTE_COUNT_LIST                                integer
RTP_FLAG_LIST                                      integer
RTP_JITTER_LIST                                    integer
RTP_MEDIA_TYPE_LIST                                integer
RTP_PACKET_NUMBER_LIST                             integer
RTP_Q1_LIST                                        integer
RTP_Q2_LIST                                        integer
RTP_Q3_LIST                                        integer
RTP_Q4_LIST                                        integer
RTP_Q5_LIST                                        integer
RTP_Q6_LIST                                        integer
RTP_Q7_LIST                                        integer
RTP_TIME_LIST                                      integer
SEQUENCE                                           integer
SEQUENCE_METHOD                                    integer
SEQUENCE_START                                     integer
SERVER_ADDRESS                                     integer
SERVER_CITY                                        integer
SERVER_COUNTRY                                     integer
SERVER_COUNTRY_CODE                                integer
SERVER_END_TIME                                    integer
SERVER_HOP_COUNT                                   integer
SERVER_LATITUDE                                    integer
SERVER_LONGITUDE                                   integer
SERVER_PORT                                        integer
SERVER_SENT_BYTE_COUNT                             integer
SERVER_SENT_PACKET_COUNT                           integer
SERVER_START_TIME                                  integer
SERVER_TCP_WINDOW_MAX                              integer
SERVER_TCP_WINDOW_MIN                              integer
SETTINGS_ENABLED                                   integer
SETTINGS_GROUP_ID                                  integer
SETTINGS_GROUP_MAX_STREAM_COUNT                    integer
SETTINGS_GROUP_NAME                                integer
SETTINGS_MINPERIOD                                 integer
SETTINGS_SENSITIVITY                               integer
SETTINGS_SEVERITY                                  integer
SETTINGS_VALUE                                     integer
SETUP_TIME                                         integer
SGNALING_MSG_INDEX                                 integer
SIGNALING_FLOW_COUNT                               integer
SIGNALING_PACKET_COUNT                             integer
SIGNALING_TYPE                                     integer
SOURCE_ADDRESS                                     integer
SOURCE_PORT                                        integer
SSRC                                               integer
START_TIME                                         integer
STATUS                                             integer
STREAM_COUNT                                       integer
STREAM_ID                                          integer
TABLE_ATTRIBUTES                                   integer
TABLE_ID                                           integer
TABLE_NAME                                         string
TCP_FLAGS                                          integer
TCP_STATUS                                         integer
THREE_WAY_HANDSHAKE_TIME                           integer
THROUGHPUT_CLIENT_TO_SERVER_BITS_PER_SECOND        integer
THROUGHPUT_CLIENT_TO_SERVER_BITS_PER_SECOND_BEST   integer
THROUGHPUT_CLIENT_TO_SERVER_BITS_PER_SECOND_WORST  integer
THROUGHPUT_CLIENT_TO_SERVER_SAMPLE_COUNT           integer
THROUGHPUT_SERVER_TO_CLIENT_BITS_PER_SECOND        integer
THROUGHPUT_SERVER_TO_CLIENT_BITS_PER_SECOND_BEST   integer
THROUGHPUT_SERVER_TO_CLIENT_BITS_PER_SECOND_WORST  integer
THROUGHPUT_SERVER_TO_CLIENT_SAMPLE_COUNT           integer
TIME                                               integer
TO                                                 integer
TO_ABBREV                                          integer
TREE_STATE                                         boolean
TYPE                                               integer
URI                                                integer
VALUE                                              integer
VSAQ                                               integer
VSMQ                                               integer
VSPQ                                               integer
VSTQ                                               integer
WIRELESS_RETRY_PERCENT                             integer
=================================================  =======


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
