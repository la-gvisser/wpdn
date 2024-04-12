# FolderAdapter Changelog

Current Version: 1.12.0.0

Changes in 1.12.0.0:
  * Switch to Cef Prefs for the Adapter Options dialog

Changes in 1.11.2.0:
  * Build platform upgraded from Ubuntu 18.04 to 20.04

Changes in 1.11.1.0:
  * Added support for Omni 21.1 to make multiple adapters

Changes in 1.11.0.0:
  * Removed support for "Recorded" packet speed

Changes in 1.10.0.0:
  * Added support for pcapng and pcapng.gz files.
  * Also supported are files with pcap and pcap.gz file extentions that contain pcapng data.
  * Also supported are files with pcapng and pcapng.gz file extensions that contain pcap data.

Changes in 1.9.0.0:
  * Fixed Folder Adapter to support Application Statistics.

Changes in 1.8.0.0:
  * Added support for pcap.gz files.
  * Fixed Folder Adapter Stats object life cycle to account for different behavior of Engine 12.5 from Engine 11.3.

Changes in 1.7.0.0:
  * Added support for pcap files with nanosecond timestamps.
  * Minor performance improvements (some tests that were performed per packet are now down when the file is opened.)

Changes in v1.6.1.0:
  * Replaced the retry counter with First Retry timestamp. On the first failure the First Retry timestamp is set to Now. At the end of the Packet Reading loop, if the First Retry timeout is not zero, and the difference between it and the current time is more than 10 seconds, then the loop exits and the file is closed.

Changes in 1.6.0.0
  * Added retry counter that increments each time reading a packet fails. After 10 tries the file is closed and handled per the Options settings.
 
Changes in 1.5.0.0:
  * The timestamp of the inserted packet is the packet from the file.
  * This also addresses the ‘no timestamp’ issue.
  * Fixed the issue with CPeekTime where Now() assigned to a 64-bit variable only receives a 32-bit value by adding operator UInt64().
  * In CaptureFile.cpp/h renamed tLongTime to tTimeLong making it unique from tLongTime in PeekTime.
  * Added operator UInt64() to tLongTime.
 
Changes in 1.5.0.0:
  * Display name of the adapter is set to the updated path of the folder being monitored.