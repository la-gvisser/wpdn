Adapter UI Sample Readme
========================

This project will not build as is. A new GUID must be generated, and g_PluginId 
in OmniPlugin.cpp and the null GUID in AdapterUISample.he set to the new GUID.

Open the Adapter UI Sample Remote Adapter plugin solution in Visual Stuido 2013, 
select x64 and Debug. Build the soulution. The plugin will be installed into:
C:\Program File\Savvius\Capture Engine\Plugins

Launch the Engine and it will load the plugin and create a COmniPlugin object.
Several of COmniPlugin's methods will be called:
- COmniPlugin()
- OnLoad()
- OnReadPrefs()

Launch Peek and connect to the Engine and the select the Adapters view.
Click on the Insert button at the top right of the view and the Configure
Adapters dialog is displayed. The name of the plugin should be listed: 
AdapterUISample. Click on AdapterUISample and click the Configure button
(or double-click AdapterUISample from the list). The Adapter UI Sample
Options dialog is displayed. The contents of this dialog come from the
plugin and the source is in Plugin Files/Options/index.html. The web 
page show:
- The name and version of the plugin.
- The last message sent to the plugin and the Ready to close checkbox.
- The name and id of the adapter (which is blank when creating adapters).
- The New Adapter section for creating new adapters.
- The Remote File System browser.

If the Ready to close checkbox is check then Peek will refuse to dismiss 
the dialog when OK is clicked. This is to allow the plugin to complete 
the current operation before the dialog is dismissed.

Plugin Messages
---------------
Click the Refresh link. The edit control will display "<list />" and the 
text box at the bottom of the dialog will add a line 
"<list><adapterlist /></list>". Clicking Refresh causes the web page to
send a message to the instance of the plugin that was created when the 
Engine was launched. The COmniPlugin::OnProcessPluginMessage() method 
is called with the contents of the message "<list />". The method 
creates a message object and calls the ProcessList() method to handle the 
message. The resposne "<list><adapterlist /></list>" is returned to the
web page.

Create an Adapter
-----------------
Enter a name for a new adapter, My Adapter, into the Title edit control on 
the Adapter UI Sample Options dialog and click the Create link. The web page 
sends a message, "<create><adapter>My Adapter</adapter></create>" to the 
plugin on the engine. The COmniPlugin::OnProcessPluginMessage() method is 
called which then calls the ProcessCreate() method. The ProcessCreate() 
method extracts the name for the new adapter, calls CreaetAdapter() and 
returns the results of creating the adapter. The web page receives the 
response and displays it, "Ok".

The COmniPlugin::CreateAdapter() method creates a new CAdapter, calls 
AddAdapter() and adds the new adapter to the plugin's list of adapters.

The COmniPlugin::AddAdapter() method creates a new CAdapterConfig object and  
configures it. Then it get's the Engine's Adapter Manager and adds a the 
new adapter.

Under the Hood
==============

View the source for CAdapterManager::AddAdapter(). Three objects are passed 
in by value, but notice that the method calls the GetPtr() method on all three 
objects. The pointers are then passed to a method of CAdapterManager's 
m_spAdapterManager. The three pointers and m_spAdapterManager are all 
interface pointers (a pointer to a class which defines a list of abstract 
methods definitions). The underlying technology is Savvius's Helium, a clone 
of Microsot's Component Object Model (COM). Helium uses .he files instead of 
the Windows Registry to publish component interfaces. The source of the plugin 
is divided into three parts: Helium, Framework and Plugin. Helium is all the 
interface definition header files. Framework is primarily a collection of 
wrapper-classes, such as CAdapterManager, that hold the interface pointer and 
wrap all the Helium interface methods with more natural method calls. See the
Wrappers? section below.

When Peek requests the Engine's list of adapters, the Engine returns uses the 
same Adapter Manager object that was used in the paragraph above.

The Adapter
===========
The object that the COmniPlugin will create in response to the Create command 
is an instance of a COmniAdapter. The majority of COmniAdapter's methods 
perform housekeeping. /* The workhorse method is HandlePacket(), which is 
actually implemented in the parent class CPeekAdapter. HandlePacket() receives 
a packet and passes it to each of the adapter's Packet Handlers. The author 
needs to investigate this method because it is not used in the sample. */ A 
Packet Handler is basically a Capture. Multiple captures may use the same 
adapter. When a capture is created it registers with the adapter by adding it's 
Packet Handler by calling the adapter's AddPacketHandler() method.

When the adapter's StartCapture() method is called, if the adapter is not 
currently capturing packets (running) then it starts up a thread that collects 
packets from outside of the engine. The AdapterUISample adapter the thread is 
created and called on the PacketThread function. The PacketThread function 
calls the COmniAdapter's Run() method. The Run() method creates a CPacket and 
calls the HandlePacket() method of each of the Packet Handlers in it's list of 
Packet Handlers.

To Do
=====
You will need to customize COmniAdapter::Run() to collect the packets from your 
adapter or what every your souce of packets. Then call the HandlePacket() 
method of each of it's Packet Handlers. /* CPeekAdapter::HandlePacket() may 
also be used to call all the Packet Handlers. */

You may customize the code in the Plugin Files folder. In general these are 
named COmniObject. The COmniObject classes are subclasses of the corresponding 
CPeekObject. Do not alter the files in the Helium folder. And please contact 
Savvius before altering files in the Framework folder.

You may want or need to modify COmniPlugin::OnLoad or OnReadPrefs() to read-in 
a configuration file that contains a list of the adapters your plugin has 
created. That way when the Engine is launched and your plugin is created, the 
plugin can recreate the apapters. Update the configuration file after 
successfully creating or deleteing an adapter (and any other time the plugin's 
list of adapters is modifed). Don't wait for OnWritePrefs() or OnUnload() to be 
called when the engine is shutdown.

Wrappers?
=========
The solution include Framework and Helium folders. These folders contain the 
Framework source code and Helium include files. Savvius's Helium is a clone of 
Microsoft's Component Object Model (COM). Savvius created Helium in order to 
port the Engine from Windows to Linux. The Framework was created to hide 
Helium/COM code from plugin developers. The Framwork also creates a plugin 
life cycle that is more appropriate for complex plugins.

The convention for a COM function call is to return a result code. This creates 
a certain level of consistency, but also a certain amount of complexity to the 
code that uses COM. For instance, to it requires three lines of code to retrieve 
the number of items from a COM container:  
  size_t count;
  int result = container.GetCount(&count);
  if ( FAILED(result)) throw exception;  
As opposed to the simpler:
  size_t count = container.GetCount();
The Framework wraps the COM/Helium interfaces in a C++ class to both provide 
simplified function calls and group cooperative interfaces into a single class.

The Engine treats plugins as light weight objects; instead of resetting a 
plugin, the Engine will create a new instance of a plugin and delete the old 
instance. When the Engine creates an instance of a plugin a CRemotePlugin is 
created instead of a COmniPlugin. The CRemotePlugin will manage the life cycle 
of the COmniPlugin so that it is created at start-up and destroyed when the 
Engine shuts down. When the Engine to creates a plugin for a Capture the 
CRemotePlugin will have the COmniPlugin create a COmniEngineContext.
