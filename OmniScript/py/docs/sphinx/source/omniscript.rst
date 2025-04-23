OmniScript 2.0
==============

Change History
--------------

**OmniScript** 2.0 v0.5.3
^^^^^^^^^^^^^^^^^^^^^^^^^

*New in 0.5.3*

The :class:`AccessControlList <omniscript.accesscontrollist.AccessControlList>` class.

The :class:`AdapterConfiguration <omniscript.adapterconfiguration.AdapterConfiguration>` class.

The :class:`ApplicationStatistic <omniscript.applicationstatistic.ApplicationStatistic>` class.

The :class:`AuditLog <omniscript.auditlog.AuditLog>` class.

The :class:`CallStatistic <omniscript.callstatistic.CallStatistic>` class.

The :class:`CountryStatistic <omniscript.countrystatistic.CountryStatistic>` class.

The :class:`EventLog <omniscript.eventlog.EventLog>` class.

The :class:`FilterNode <omniscript.filternode.FilterNode>` class.

The :class:`HardwareOptions <omniscript.hardwareoptions.HardwareOptions>` class.

The :class:`HostPlatform <omniscript.hostplatform.HostPlatform>` class.

The :class:`TCPDump <omniscript.tcpdump.TCPDump>` class.

The :func:`save_summarystats() <omniscript.savestatistics.save_summarystats()>`
function.

The :func:`save_nodestats() <omniscript.savestatistics.save_nodestats()>`
function.

The :func:`save_protocolstats() <omniscript.savestatistics.save_protocolstats()>`
function.


**OmniScript** 2.0 v0.5.2
^^^^^^^^^^^^^^^^^^^^^^^^^

*New in 0.5.2*

The :class:`FileAdapter <omniscript.fileadapter.FileAdapter>` class.

The :class:`FileInformation <omniscript.fileinformation.FileInformation>` class.

The :class:`Filter <omniscript.filter.Filter>` class.

The :class:`NodeStatistic <omniscript.nodestatistic.NodeStatistic>`,
:class:`ProtocolStatistic <omniscript.protocolstatistic.ProtocolStatistic>` and
:class:`SummaryStatistic <omniscript.summarystatistic.SummaryStatistic>` classes
have Report generation methods.


**OmniScript** 2.0 v0.4.6
^^^^^^^^^^^^^^^^^^^^^^^^^

*New in 0.4.6*
:class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>` Start and Stop Triggers are complete.

Added:

* ForensicSearch :func:`refresh() <omniscript.forensicsearch.ForensicSearch.refresh>`
* EngineStatus :func:`refresh() <omniscript.enginestatus.EngineStatus.refresh>`

Classes that have 'id' (GUID, UUID, Global Universal Identifiers) are now :class:`OmniId <omniscript.omniid.OmniId>` objects.

Classes that have 'time' attributes are now :class:`PeekTime <omniscript.peektime.PeekTime>` objects.

- Display them in a friendly format with: print capture.start_time.ctime()

When appropriate, OmniScript classes represent (display) themselves as:
<class name>: <object name>


**OmniScript** 2.0 v0.4.5
^^^^^^^^^^^^^^^^^^^^^^^^^

*New in 0.4.5*
:class:`CaptureTemplate <omniscript.capturetemplate.CaptureTemplate>` Start and Stop Triggers


OmniScript Classes
==================

AccessControlList
-----------------

.. automodule:: omniscript.accesscontrollist
    :members:
    :undoc-members:
    :show-inheritance:

Adapter
-------

.. automodule:: omniscript.adapter
    :members:
    :undoc-members:
    :show-inheritance:

AdapterConfiguration
--------------------

.. automodule:: omniscript.adapterconfiguration
    :members:
    :undoc-members:
    :show-inheritance:

Alarm
-----

.. automodule:: omniscript.alarm
    :members:
    :undoc-members:
    :show-inheritance:

AnalysisModule
--------------

.. automodule:: omniscript.analysismodule
    :members:
    :undoc-members:
    :show-inheritance:

ApplicationStatistic
--------------------

.. automodule:: omniscript.applicationstatistic
    :members:
    :undoc-members:
    :show-inheritance:

AuditLog
--------

.. automodule:: omniscript.auditlog
    :members:
    :undoc-members:
    :show-inheritance:

CallStatistic
-------------

.. automodule:: omniscript.callstatistic
    :members:
    :undoc-members:
    :show-inheritance:

Capture
-------

.. automodule:: omniscript.capture
    :members:
    :undoc-members:
    :show-inheritance:

CaptureSession
--------------

.. automodule:: omniscript.capturesession
    :members:
    :undoc-members:
    :show-inheritance:

CaptureTemplate
---------------

.. automodule:: omniscript.capturetemplate
    :members:
    :undoc-members:
    :show-inheritance:

CountryStatistic
----------------

.. automodule:: omniscript.countrystatistic
    :members:
    :undoc-members:
    :show-inheritance:

DecodedPacket
-------------

.. automodule:: omniscript.decodedpacket
    :members:
    :undoc-members:
    :show-inheritance:

Diagnostic
----------

.. automodule:: omniscript.diagnostic
    :members:
    :undoc-members:
    :show-inheritance:

EngineStatus
------------

.. automodule:: omniscript.enginestatus
    :members:
    :undoc-members:
    :show-inheritance:

EventLog
--------

.. automodule:: omniscript.eventlog
    :members:
    :undoc-members:
    :show-inheritance:

ExpertQuery
-----------

.. automodule:: omniscript.expertquery
    :members:
    :undoc-members:
    :show-inheritance:

ExpertResult
------------

.. automodule:: omniscript.expertresult
    :members:
    :undoc-members:
    :show-inheritance:

FileAdapter
-----------

.. automodule:: omniscript.fileadapter
    :members:
    :undoc-members:
    :show-inheritance:

FileInformation
---------------

.. automodule:: omniscript.fileinformation
    :members:
    :undoc-members:
    :show-inheritance:

Filter
------

.. automodule:: omniscript.filter
    :members:
    :undoc-members:
    :show-inheritance:

FilterNode
----------

.. automodule:: omniscript.filternode
    :members:
    :undoc-members:
    :show-inheritance:

ForensicFile
------------

.. automodule:: omniscript.forensicfile
    :members:
    :undoc-members:
    :show-inheritance:

ForensicSearch
--------------

.. automodule:: omniscript.forensicsearch
    :members:
    :undoc-members:
    :show-inheritance:

ForensicTemplate
----------------

.. automodule:: omniscript.forensictemplate
    :members:
    :undoc-members:
    :show-inheritance:

GraphTemplate
--------------

.. automodule:: omniscript.graphtemplate
    :members:
    :undoc-members:
    :show-inheritance:

HardwareOptions
---------------

.. automodule:: omniscript.hardwareoptions
    :members:
    :undoc-members:
    :show-inheritance:

HostPlatform
--------------

.. automodule:: omniscript.hostplatform
    :members:
    :undoc-members:
    :show-inheritance:

MediaInfo
---------

.. automodule:: omniscript.mediainfo
    :members:
    :undoc-members:
    :show-inheritance:

MediaSpec
---------

.. automodule:: omniscript.mediaspec
    :members:
    :undoc-members:
    :show-inheritance:

NodeStatistic
-------------

.. automodule:: omniscript.nodestatistic
    :members:
    :undoc-members:
    :show-inheritance:

OmniAddress
-----------

.. automodule:: omniscript.omniaddress
    :members:
    :undoc-members:
    :show-inheritance:

.. automodule:: omniscript.omnidatatable
    :members:
    :undoc-members:
    :show-inheritance:

OmniEngine
----------

.. automodule:: omniscript.omniengine
    :members:
    :undoc-members:
    :show-inheritance:

OmniDataTable
-------------

OmniError
---------

.. automodule:: omniscript.omnierror
    :members:
    :undoc-members:
    :show-inheritance:

OmniId
------

.. automodule:: omniscript.omniid
    :members:
    :undoc-members:
    :show-inheritance:

OmniPort
--------

.. automodule:: omniscript.omniport
    :members:
    :undoc-members:
    :show-inheritance:

OmniScript
----------

.. automodule:: omniscript.omniscript
    :members:
    :undoc-members:
    :show-inheritance:

Packet
------

.. automodule:: omniscript.packet
    :members:
    :undoc-members:
    :show-inheritance:

PeekTime
--------

.. automodule:: omniscript.peektime
    :members:
    :undoc-members:
    :show-inheritance:

ProtocolStatistic
-----------------

.. automodule:: omniscript.protocolstatistic
    :members:
    :undoc-members:
    :show-inheritance:

ReadStream
----------

.. automodule:: omniscript.readstream
    :members:
    :undoc-members:
    :show-inheritance:

StatsContext
------------

.. automodule:: omniscript.statscontext
    :members:
    :undoc-members:
    :show-inheritance:

StatsLimit
----------

.. automodule:: omniscript.statslimit
    :members:
    :undoc-members:
    :show-inheritance:

SummaryStatistic
----------------

.. automodule:: omniscript.summarystatistic
    :members:
    :undoc-members:
    :show-inheritance:

TCPDump
-------

.. autoclass:: omniscript.tcpdump.TCPDump
    :members: 
    :undoc-members:
    :inherited-members:
    :show-inheritance:

TCPDumpHost
-----------

.. autoclass:: omniscript.tcpdump.TCPDumpHost
    :members: 
    :undoc-members:
    :show-inheritance:

TCPDumpNetwork
--------------

.. autoclass:: omniscript.tcpdump.TCPDumpNetwork
    :members: 
    :undoc-members:
    :show-inheritance:

TCPDumpNetworkInterface
-----------------------

.. autoclass:: omniscript.tcpdump.TCPDumpNetworkInterface
    :members: 
    :undoc-members:
    :show-inheritance:

TCPDumpTemplate
---------------

.. autoclass:: omniscript.tcpdump.TCPDumpTemplate
    :members: 
    :undoc-members:
    :inherited-members:
    :show-inheritance:

Callable Functions by User
==========================

These are the modules that are callable by a Python script writer that is using the OmniScript Package.

savestatistics
--------------

.. automodule:: omniscript.savestatistics
    :members:
    :inherited-members:
    :undoc-members:
    :show-inheritance:
