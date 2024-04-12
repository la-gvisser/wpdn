CMIFilter 1.0.1 Readme

Welcome to the CMIFilter for Windows!

This file includes important, updated information for users of the CMIFilter. The
information is more current, in some cases, than that provided in the User Manual. Please
read this information carefully.

All references in this document to EtherPeek apply equally to the versions of EtherPeek
that are compatible with this version of the CMIFilter.

All references in this document to CMIFilter.dll refer to both the CMIFilter9.dll and 
CMIFilter10.dll.   The CMIFilter9.dll works with EtherPeek 5.1, and the CMIFilter10.dll 
works with EtherPeek 6.0.

The WildPackets CMIFilter filters out packets that match the connection criteria and extracts 
ethernet packets from those packets and inserts the extracted packet into the buffer.  The 
CMIFilter also provides the following functionality:
- The CMIFilter reassembles fragmented packets before extracting the inner packet.
- The CMIFilter must be specified as the node of an advanced filter in order to 
  function.
- The CMIFilter has options that appear in the Options tab and per Capture Window.
- The options in the options tab are the defaults used to initialize the options 
  in the per capture window options.
- The options in the options tab are saved to CMIFilterPrefs.xml when the program is quit.
- The options in the options tab are read from CMIFilterPrefs.xml when the program is run.
- There is an option to turn enable the filter.  When this option is enabled the data 
  in other sections that are enabled are verified.
- There is an option to replace the MAC header.
- There is an option to insert a VLAN header.
- There is an option to save orphan fragments at a specified interval.

INSTALLATION NOTES
IMPORTANT!
If you are upgrading from an earlier version of the CMIFilter, we recommend 
removing it before installing this release.	 

In order to install the CMIFilter, the Peek application must already be installed.  
Before installing the CMIFilter ensure that EtherPeek is not running. 

The CMIFilter consists of a single plug-in dll.   The plugin
is installed into the EtherPeek plugins directory.   

UNINSTALLATION NOTES
IMPORTANT:  
To remove the WildPackets CMIFilter, quit EtherPeek and move or delete the
CMIFilter.dll from the plugins directory

SYSTEM REQUIREMENTS
the CMIFilter requires EtherPeek be installed on the client machine.  Therefore, 
the  recommended system requirements for the CMIFilter are those that meet the 
system requirements of the Peek application. 

TECHNICAL TIPS AND ADDITIONAL PRODUCT INFORMATION
 1. The CMIFilter 1.0.1 does not work with OmniPeek, AiroPeek, TokenPeek or 
    WANPeek.
  
--------------------------------------------------------------------------------

WildPackets, Inc.
1340 Treat Boulevard, Suite 500
Walnut Creek, California 94597 USA
(925) 937-3200
http://www.wildpackets.com

Copyright © 2002-2005 WildPackets, Inc.
All rights reserved.
