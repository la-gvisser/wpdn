// =============================================================================
// <copyright file="CaptureManager.cs" company="Savvius, Inc.">
//  Copyright (c) 2015 Savvius, Inc. All rights reserved.
//  Copyright (c) 2013 WildPackets, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace RCapture
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Text;
    using System.Threading;
    using Savvius.Omni.OmniScript;

    /// <summary>
    /// CaptureManager
    /// </summary>
    class CaptureManager
    {
        /// <summary>
        /// The object that holds the connection with OmniScript.
        /// </summary>
        public OmniScript omni;
        public OmniEngine engine;

        public OmniLogger logger;

        private Capture capture;

        /// <summary>
        /// Gets or sets the verbose level.
        /// </summary>
        public uint Verbose { get; set; }

        public OmniScript.EngineAddress Address { get; set; }
        public OmniScript.EngineCredentials Credentials { get; set; }

        public String CaptureTemplate { get; set; }
        public uint AdapterIndex { get; set; }
        public String AdapterDescription { get; set; }
        public String CaptureName { get; set; }
        public uint CaptureSeconds { get; set; }
        public uint CaptureMinutes { get; set; }
        public uint CaptureBuffer { get; set; }
        public String Filename { get; set; }
        public String FileFormat { get; set; }
        public String SaveToDisk { get; set; }
        public bool SetWirelessChannel { get; set; }
        public uint WirelessChannelNumber { get; set; }
        public uint WirelessChannelFrequency { get; set; }
        public uint ReservedSize { get; set; }
        public uint SaveEvery { get; set; }
        public uint DiskFile { get; set; }

        public bool NodeStats { get; set; }
        public bool FlowStats { get; set; }
        public bool SummaryStats { get; set; }
        public bool ProtocolStats { get; set; }
        public bool CSV { get; set; }

        public bool CreateCapture { get; set; }
        public bool StartCapture { get; set; }
        public bool StopCapture { get; set; }
        public bool SaveCapture { get; set; }
        public bool DeleteCapture { get; set; }
        public bool ResetCapture { get; set; }

        public bool ListAdapters { get; set; }
        public bool ListCaptures { get; set; }
        public bool ListFilters { get; set; }
        public bool CreateFilter { get; set; }
        public bool DeleteFilter { get; set; }
        public String FilterName { get; set; }
        public List<String> CreateFilters { get; set; }
        public List<String> AddFilters { get; set; }

        public bool DisplayHelp { get; set; }
        
        /// <summary>
        /// Initializes a new instance of the CaptureManager class.
        /// </summary>
        public CaptureManager()
        {
            this.omni = new OmniScript();
            this.logger = new OmniLogger();
            this.capture = null;

            this.Verbose = 1;

            this.Address = new OmniScript.EngineAddress();
            this.Credentials = new OmniScript.EngineCredentials();

            this.CaptureTemplate = "";
            this.AdapterIndex = 0xFFFFFFFF;
            this.AdapterDescription = "";
            this.CaptureName = "";
            this.CaptureSeconds = 0;
            this.CaptureMinutes = 0;
            this.CaptureBuffer = 64;
            this.Filename = "";
            this.FileFormat = "pkt";
            this.SaveToDisk = "";
            this.SetWirelessChannel = false;
            this.WirelessChannelNumber = 0;
            this.WirelessChannelFrequency = 0;
            this.ReservedSize = 0;

            this.NodeStats = false;
            this.FlowStats = false;
            this.SummaryStats = false;
            this.ProtocolStats = false;
            this.CSV = false;

            this.CreateCapture = false;
            this.StartCapture = false;
            this.StopCapture = false;
            this.SaveCapture = false;
            this.DeleteCapture = false;
            this.ResetCapture = false;

            this.ListAdapters = false;
            this.ListCaptures = false;
            this.ListFilters = false;
            this.CreateFilter = false;
            this.DeleteFilter = false;
            this.FilterName = "";
            this.AddFilters = new List<String>();
            this.CreateFilters = new List<String>();

            this.DisplayHelp = false;
        }

        static String GetProgramFilesX86()
        {
            String progFiles = System.Environment.GetEnvironmentVariable("ProgramFiles");
            String progFilesX86 = System.Environment.GetEnvironmentVariable("ProgramFiles(x86)");
            if (progFilesX86 == null)
            {
                return progFiles;
            }
            if (progFiles == progFilesX86)
            {
                progFiles = System.Environment.GetEnvironmentVariable("ProgramW6432");
            }

            return progFilesX86;
        }

        public Capture CaptureCreate()
        {
            if (this.AdapterIndex == 0xFFFF && String.IsNullOrEmpty(this.AdapterDescription))
            {
                this.logger.Error("Invalid or missing Adapter Index (-a) or Description (-q).");
                return null;
            }

            if (String.IsNullOrEmpty(this.CaptureName))
            {
                this.logger.Error("Invalid or missing capture name (-n).");
                return null;
            }

            if (!this.IsConnected())
            {
                this.logger.Critical("No connection.");
            }

            Capture capture = this.engine.FindCapture(this.CaptureName);
            if (capture != null)
            {
                this.logger.Error(String.Format("The capture already exists: '{0}'", this.CaptureName));
                return capture;
            }

            CaptureTemplate template = new CaptureTemplate(this.CaptureTemplate);
            template.Name = this.CaptureName;

            // Get the Adapter information.
            Adapter adapter = null;
            if (this.AdapterIndex != 0xFFFF)
            {
                AdapterList adapterList = this.engine.GetAdapterList();
                if (this.AdapterIndex > adapterList.Count)
                {
                    this.logger.Error("Invalid adapter index: d. Must be between 0 and d");
                    return null;
                }
                adapter = adapterList[(int)this.AdapterIndex];
            }
            else
            {
                adapter = this.engine.FindAdapterByName(this.AdapterDescription);
            }

            if (adapter == null)
            {
                this.logger.Critical("Could not find the adapter.");
                return null;
            }

            template.AdapterName = adapter.Name;
            if (this.logger.IsLogging(OmniLogger.Verboseness.Info))
            {
                this.logger.Info("Using Adapter:");
                List(adapter);
            }

            template.SetFilters(this.AddFilters);

            capture = this.engine.CreateCapture(template);

            return capture;
        }

        public void CaptureDelete()
        {
            if (String.IsNullOrEmpty(this.CaptureName))
            {
                this.logger.Error("Invalid or missing capture name (-n).");
                return;
            }

            if (!this.IsConnected())
            {
                this.logger.Critical("No connection.");
                return;
            }

            this.capture = this.engine.FindCapture(this.CaptureName);
            if (this.capture == null)
            {
                this.logger.Error("The capture does not exist: " + this.CaptureName);
                return;
            }

            this.engine.DeleteCapture(this.capture);
        }

        public void CaptureReset()
        {
            if (String.IsNullOrEmpty(this.CaptureName))
            {
                this.logger.Error("Invalid or missing capture name (-n).");
                return;
            }

            if (!this.IsConnected())
            {
                this.logger.Critical("No connection.");
                return;
            }

            this.capture = this.engine.FindCapture(this.CaptureName);
            if (this.capture == null)
            {
                this.logger.Error("The capture does not exist: " + this.CaptureName);
                return;
            }

            this.capture.Reset();
        }

        public void CaptureSave()
        {
            if (String.IsNullOrEmpty(this.Filename))
            {
                this.logger.Error("Invalid or missing file name (-o).");
                return;
            }

            if (String.IsNullOrEmpty(this.CaptureName))
            {
                this.logger.Error("Invalid or missing capture name (-n).");
                return;
            }

            if (!this.IsConnected())
            {
                this.logger.Critical("No connection.");
                return;
            }

            this.logger.Debug("Saving packets to " + this.CaptureName);

            this.capture = this.engine.FindCapture(this.CaptureName);
            if (this.capture == null)
            {
                this.logger.Error("The capture does not exist: " + this.CaptureName);
                return;
            }

            if (this.capture.IsCapturing())
            {
                this.logger.Debug("The Capture is still capturing: " + this.CaptureName);
                return;
            }

            this.logger.Debug("Capture Name = " + this.CaptureName);

            if (this.capture.Id != null)
            {
                this.logger.Debug("Capture Id = " + this.capture.Id);
            }

            bool convert = false;

            // determine format
            if (this.FileFormat == "enc")
            {
                convert = true;
                Path.ChangeExtension(this.Filename, ".enc");
            }
            else if (this.FileFormat == "pcap")
            {
                convert = true;
                Path.ChangeExtension(this.Filename, ".pcap");
            }
            else if (this.FileFormat == "raw")
            {
                convert = true;
                Path.ChangeExtension(this.Filename, ".txt");
            }
            else if (this.FileFormat == "pkt")
            {
                Path.ChangeExtension(this.Filename, ".pkt");
            }
            else
            {
                this.logger.Error(
                    "Error: Invalid save format." + Environment.NewLine + Environment.NewLine +
                    "Valid save formats are:" + Environment.NewLine +
                    "  pkt (OmniPeek Packet File)" + Environment.NewLine +
                    "  enc (NG Sniffer DOS File)" + Environment.NewLine +
                    "  pcap (libpcap)" + Environment.NewLine +
                    "  raw (Raw Packet Data)" + Environment.NewLine); 
                return;
            }

            // determine file name
            String temp_file = null;
            if (convert)
            {
                temp_file = Path.GetTempFileName() + ".pkt";
            }
            else
            {
                temp_file = this.Filename;
            }
           
            // save pkt file locally, convert to desired format
            // delete intermediate pkt file
            this.capture.SaveAllPackets(temp_file);
            if (convert)
            {
//                this.engine.ConvertCaptureFile(temp_file, this.Filename, fmt);
            }

            this.logger.Info("Saved packets to " + this.Filename);

            // clean up temp file
            if (convert && File.Exists(temp_file))
            {
                File.Delete(temp_file);
            }
        }

        public void CaptureStart()
        {
            if (String.IsNullOrEmpty(this.CaptureName))
            {
                this.logger.Error("Invalid or missing capture name (-n).");
                return;
            }

            if (!this.IsConnected())
            {
                this.logger.Critical("No connection.");
                return;
            }

            this.capture = this.engine.FindCapture(this.CaptureName);
            if (this.capture == null)
            {
                this.logger.Error("The capture does not exist: " + this.CaptureName);
                return;
            }

            if (this.capture.IsCapturing())
            {
                this.logger.Info("The capture is already capturing.");
                return;
            }

            this.capture.Start();

            this.capture.Refresh();
            if (!this.capture.IsCapturing())
            {
                this.logger.Info("Failed to start capturing on capture: " + this.CaptureName);
                return;
            }

            this.logger.Info("The capture is capturing: " + this.CaptureName);
        }

        public void CaptureStop()
        {
            if (String.IsNullOrEmpty(this.CaptureName))
            {
                this.logger.Error("Invalid or missing capture name (-n).");
                return;
            }

            if (!this.IsConnected())
            {
                this.logger.Critical("No connection.");
                return;
            }

            this.capture = this.engine.FindCapture(this.CaptureName);
            if (this.capture == null)
            {
                this.logger.Error("The capture does not exist: " + this.CaptureName);
                return;
            }

            if (!this.capture.IsCapturing())
            {
                this.logger.Info("The capture is already idle.");
                return;
            }

            this.capture.Stop();

            this.capture.Refresh();
            if (!this.capture.IsCapturing())
            {
                this.logger.Info("Failed to stop capturing on capture: " + this.CaptureName);
                return;
            }

            this.logger.Info("The capture is idle: " + this.CaptureName);
        }

        public bool Connect()
        {
            this.engine = this.omni.CreateEngine(this.Address);
            if (this.engine == null) return false;
            this.engine.Connect(this.Credentials);
            return this.engine.IsConnected();
        }

        public Filter FilterCreate(String name, String ipAddress)
        {
            if (!this.IsConnected())
            {
                this.logger.Critical("No connection.");
            }

            Filter filter = this.engine.FindFilter(name);
            if (filter != null)
            {
                this.logger.Error("Filter already exists.");
                return filter;
            }

            filter = new Filter(this.logger, name);
            if (filter == null)
            {
                this.logger.Error("Failed to create new filter.");
                return filter;
            }

            this.logger.Error("Filter building has not been implemented.");
//             filter.Id = Guid.NewGuid();
//             filter.Address1 = ipAddress;

            this.engine.AddFilter(filter);

            return filter;
        }

        public void FiltersCreate()
        {
            if (!this.IsConnected())
            {
                this.logger.Critical("No connection.");
            }

            foreach (String spec in this.CreateFilters)
            {
                this.logger.Info(spec);
                String[] args = spec.Split(':');
                if (args.Length < 2)
                {
                    this.logger.Critical("Invalid filter specification.");
                    continue;
                }
                String name = args[0];
                String ipAddress = args[1];
                Filter filter = FilterCreate(name, ipAddress);
                if (filter != null)
                {
                    this.AddFilters.Add(name);
                }
            }
        }

        public void FilterDelete()
        {
            if (!this.IsConnected())
            {
                this.logger.Critical("No connection.");
            }

            Filter filter = this.engine.FindFilter(this.FilterName);

            this.engine.DeleteFilter(filter);

            this.logger.Info("Deleted the filter: " + this.FilterName);
        }

        static void Help()
        {
            //              1         2         3         4         5         6         7         8
            //     12345678901234567890123456789012345678901234567890123456789012345678901234567890
            Console.WriteLine("RCapture");
            Console.WriteLine("  [-c <command>]     : Capture command: create, start, stop, save, reset or delete.");
            Console.WriteLine("  [--template <template> ]: Capture template (optional)");
            Console.WriteLine("  [-e <engine IP>]   : IP address of engine. Default is localhost.");
            Console.WriteLine("  [-p <engine Port>] : Port number of the engine. Default is 6367.");
            Console.WriteLine("  [-t <auth>]        : Authentication type: 'Default' or 'Third Party'. Default is 'Default'.");
            Console.WriteLine("  [-d <domain]       : Domain of the credentials.");
            Console.WriteLine("  [-u <username>     : User account name.");
            Console.WriteLine("  [-w <password>]    : User account password.");
            Console.WriteLine("  [-a <index>]       : The adapter index. Use -l (the letter el) to list adapters.");
            Console.WriteLine("  [-q <description>] : The adapter description. (-a takes precedence.)");
            Console.WriteLine("  [--wireless_channel <channel>]: Set the wireless adapter channel by channel number.");
            Console.WriteLine("  [--wireless_frequency <frequency>]: Set the wireless adapter channel by channel frequency.");
            Console.WriteLine("  [-n <capture name>]: Capture name.");
            Console.WriteLine("  [-o <filename>]    : Filename for template or saved packets. (See --format when saving capture)");
            Console.WriteLine("  [--format <format> ]: Save formats: pkt, enc, pcap or raw.");
            Console.WriteLine("  [-b <size>         : Capture buffer size in megabytes. Default is 64MB.");
            Console.WriteLine("  [-f <path>]        : Save To Disk filename template.");
            Console.WriteLine("  [-r <hours>        : Restart - The number of hours to start a new file. Default is 24 hours.");
            Console.WriteLine("  [-m <size>         : Save to Disk file size in megabytes. Default is Capture Buffer size or 1MB.");
            Console.WriteLine("  [--reserve <size GB>]: Amount of disk space to use for capture, in GB. (Timeline only)");
            Console.WriteLine("  [-h <filter name>] : Add named filter to the capture.");
            Console.WriteLine("  [-i <name:ip>]     : Create named IP address filter.");
            Console.WriteLine("  [-j <filter name>] : Delete the named filter.");
            Console.WriteLine("  [--sec <seconds>]  : Seconds between start and stop.");
            Console.WriteLine("  [--min <minutes>]  : Minutes between start and stop.");
            Console.WriteLine("  [-l]               : List the adapters and quit.");
            Console.WriteLine("  [-g]               : List the captures and quit.");
            Console.WriteLine("  [-y]               : List the filters and quit.");
            Console.WriteLine("  [-s <mode[,mode]>] : Statistics command: node, flow, summary and protocol. Require capture name (-n).");
            Console.WriteLine("                         Combinations are allowed. For example: -s node,protocol");
            Console.WriteLine("  [--csv]            : Print the stats as csv output.");
            Console.WriteLine("  [-v <level>]       : Verbose mode: 0=none, 1=error, 2=informational, 3=debug. Default is 1.");
            Console.WriteLine("  [-?]               : Display this message and quit.");
        }

        public bool IsConnected()
        {
            if (this.engine == null) return false;
            return this.engine.IsConnected();
        }

        public static void List(Adapter adapter)
        {
            adapter.logger.Log("  Adapter");
            adapter.logger.Log("    Name:              " + adapter.Name);
            adapter.logger.Log("    Id:                " + adapter.Id);
            adapter.logger.Log("    Type:              " + adapter.AdapterType);
            adapter.logger.Log("    Address:           " + adapter.Address);
            adapter.logger.Log("    Description:       " + adapter.Description);
            adapter.logger.Log("    Link Speed:        " + adapter.LinkSpeed);
            adapter.logger.Log("    Media Type:        " + adapter.MediaType);
            adapter.logger.Log("    Media SubType:     " + adapter.MediaSubType);

            if (adapter is NDISAdapter)
            {
                NDISAdapter ndis = (NDISAdapter)adapter;
                adapter.logger.Log("    Type:              NDIS Adapter");
                adapter.logger.Log("    Features:          " + ndis.Features);
                adapter.logger.Log("    Device Name:       " + ndis.DeviceName);
                adapter.logger.Log("   Interface Features: " + ndis.InterfaceFeatures);
                adapter.logger.Log("    OmniPeek API:      " + ndis.OmniPeekAPI);
            }
            else if (adapter is PluginAdapter)
            {
                PluginAdapter plugin = (PluginAdapter)adapter;
                adapter.logger.Log("    Type:              " + "Plugin Adapter");
                adapter.logger.Log("    Plugin:            " + plugin.Plugin);

            }
            else if (adapter is FileAdapter)
            {
                FileAdapter file = (FileAdapter)adapter;
                adapter.logger.Log("    Type:              File Adapter");
                adapter.logger.Log("    Limit:             " + file.Limit);
                adapter.logger.Log("    Mode:              " + file.Mode);
                adapter.logger.Log("    Speed:             " + file.Speed);
            }
            else if (adapter is PcapAdapter)
            {
                PcapAdapter pcap = (PcapAdapter)adapter;
                adapter.logger.Log("    Type::              Pcap Adapter");
                adapter.logger.Log("    Features:           " + pcap.Features);
                adapter.logger.Log("    Device Name:        " + pcap.DeviceName);
                adapter.logger.Log("    Interface Features: " + pcap.InterfaceFeatures);
                adapter.logger.Log("    OmniPeek API:       " + pcap.OmniPeekAPI);
            }
        }

        public static void List(Capture capture)
        {
            capture.Logger.Log("  Capture");
            capture.Logger.Log("    Name:             " + capture.Name);
            capture.Logger.Log("    Adapter:          " + capture.Adapter);
            capture.Logger.Log("    Buffer Capacity:  " + capture.BufferSize);
            capture.Logger.Log("    Packets Received: " + capture.PacketsReceived);
            capture.Logger.Log("    Packets Dropped:  " + capture.PacketsDropped);
            capture.Logger.Log("    Packets Filtered: " + capture.PacketsFiltered);
            capture.Logger.Log("    Status:           " + capture.FormatStatus());
            capture.Logger.Log("    Start Time:       " + capture.StartTime);
            capture.Logger.Log("    Stop Time:        " + capture.StopTime);
            capture.Logger.Info("    Adapter Type:     " + capture.AdapterType);
            capture.Logger.Info("    Alarms Enabled:   " + capture.OptionAlarms);
            capture.Logger.Info("    Alarms-Info:      " + capture.AlarmsInfo);
            capture.Logger.Info("    Alarms-Major:     " + capture.AlarmsMajor);
            capture.Logger.Info("    Alarms-Minor:     " + capture.AlarmsMinor);
            capture.Logger.Info("    Alarms-Severe:    " + capture.AlarmsSevere);
            capture.Logger.Info("    Analysis Dropped Packets: " + capture.AnalysisDroppedPackets);
            capture.Logger.Info("    Capacity Available: " + capture.BufferAvailable);
            capture.Logger.Info("    Capacity Used:    " + capture.BufferUsed);
            capture.Logger.Info("    Comment:          " + capture.Comment);
            capture.Logger.Info("    Creator:          " + capture.Creator);
            capture.Logger.Info("    Creator-SID:      " + capture.CreatorSId);
            capture.Logger.Info("    Duplicate Packets Discarded: " + capture.DuplicatePacketsDiscarded);
            capture.Logger.Info("    Duration:         " + capture.Duration);
            capture.Logger.Info("    Expert Enabled:   " + capture.OptionExpert);
            capture.Logger.Info("    Filter Mode:      " + capture.FilterMode);
            capture.Logger.Info("    Filters Enabled:  " + capture.OptionFilters);
            capture.Logger.Info("    First Packet:     " + capture.FirstPacket);
            capture.Logger.Info("    Graphs Enabled:   " + capture.OptionGraphs);
            capture.Logger.Info("    Hidden:           " + capture.OptionHidden);
            capture.Logger.Info("    Id:               " + capture.Id);
            capture.Logger.Info("    Last Mod By:      " + capture.LastModifiedBy);
            capture.Logger.Info("    Last Mod:         " + capture.LastModification);
            capture.Logger.Info("    Link Speed:       " + capture.LinkSpeed);
            capture.Logger.Info("    LoggedOnUser-Sid: " + capture.LoggedOnUserSId);
            capture.Logger.Info("    Media Subtype:    " + capture.MediaSubType);
            capture.Logger.Info("    Media Type:       " + capture.MediaType);
            capture.Logger.Info("    Packet Buffer Enabled: " + capture.OptionPacketBuffer);
            capture.Logger.Info("    Packet Count:     " + capture.PacketCount);
            capture.Logger.Info("    Packets Analyzed: " + capture.PacketsAnalyzed);
            capture.Logger.Info("    Plugins Enabled:  " + capture.OptionPlugins);
            capture.Logger.Info("    Reset Count:      " + capture.ResetCount);
            capture.Logger.Info("    Timeline Stats Enabled:" + capture.OptionTimelineStats);
            capture.Logger.Info("    Trigger Count:    " + capture.TriggerCount);
            capture.Logger.Info("    Trigger Duration: " + capture.TriggerDuration);
            capture.Logger.Info("    Voice Enabled:    " + capture.OptionVoice);
            capture.Logger.Info("    Web Enabled:      " + capture.OptionWeb);
        }

        public static void List(Filter filter)
        {
            filter.logger.Log("  Filter");
            filter.logger.Log("    Name:      " + filter.Name);
            filter.logger.Log("    Comment:   " + filter.Comment);
            filter.logger.Log("    Group:     " + filter.Group);
            filter.logger.Info("    Created:   " + filter.Created);
            filter.logger.Info("    Modified:  " + filter.Modified);
            filter.logger.Info("    Color:     " + filter.Comment);
            filter.logger.Info("    Id:        " + filter.Id);
        }

        public void ListAdapterList()
        {
            if (!this.IsConnected())
            {
                this.logger.Critical("No connection.");
            }

            Console.WriteLine("Adapter List:");

            AdapterList list = this.engine.GetAdapterList();
            foreach (Adapter adapter in list)
            {
                List(adapter);
            }
        }

        public void ListCaptureList()
        {
            if (!this.IsConnected())
            {
                this.logger.Critical("No connection.");
            }

            this.logger.Log("Capture List:");

            CaptureList list = this.engine.GetCaptureList();
            if (this.CaptureName.Length > 0)
            {
                Capture capture = list.Find(this.CaptureName);
                if (capture != null)
                {
                    List(capture);
                }
            }
            foreach (Capture capture in list)
            {
                List(capture);
            }
        }

        public void ListFilterList()
        {
            if (!this.IsConnected())
            {
                this.logger.Critical("No connection.");
            }

            this.logger.Log("Filter List:");
            FilterList list = this.engine.GetFilterList();
            foreach (Filter filter in list)
            {
                List(filter);
            }
        }

        public void ParseCommandLine(String[] args)
        {
            IEnumerator e = args.GetEnumerator();
            String v;
            String a;
            uint i;
            while (e.MoveNext())
            {
                v = (String)e.Current;
                switch (v)
                {
                    case "-c":
                        if (!e.MoveNext()) break;
                        a = (String)e.Current;
                        switch (a)
                        {
                            case "create":
                                this.CreateCapture = true;
                                break;
                            case "start":
                                this.StartCapture = true;
                                break;
                            case "stop":
                                this.StopCapture = true;
                                break;
                            case "delete":
                                this.DeleteCapture = true;
                                break;
                            case "save":
                                this.SaveCapture = true;
                                break;
                            case "reset":
                                this.ResetCapture = true;
                                break;
                            default:
                                // error
                                break;
                        }
                        break;

                    case "-o":
                        if (!e.MoveNext()) break;
                        this.Filename = (String)e.Current;
                        break;

                    case "-e":
                        if (!e.MoveNext()) break;
                        this.Address.Host = (String)e.Current;
                        break;

                    case "-p":
                        if (!e.MoveNext()) break;
                        this.Address.Port = Convert.ToUInt32((String)e.Current);
                        break;

                    case "-t":
                        if (!e.MoveNext()) break;
                        this.Credentials.Auth = (String)e.Current;
                        break;

                    case "-d":
                        if (!e.MoveNext()) break;
                        this.Credentials.Domain = (String)e.Current;
                        break;

                    case "-u":
                        if (!e.MoveNext()) break;
                        this.Credentials.Account = (String)e.Current;
                        break;

                    case "-w":
                        if (!e.MoveNext()) break;
                        this.Credentials.Password = (String)e.Current;
                        break;

                    case "-a":
                        if (!e.MoveNext()) break;
                        this.AdapterIndex = Convert.ToUInt32((String)e.Current);
                        break;

                    case "-q":
                        if (!e.MoveNext()) break;
                        this.AdapterDescription = (String)e.Current;
                        break;

                    case "-n":
                        if (!e.MoveNext()) break;
                        this.CaptureName = (String)e.Current;
                        break;

                    case "-b":
                        if (!e.MoveNext()) break;
                        this.CaptureBuffer = Convert.ToUInt32((String)e.Current);
                        break;

                    case "-h":
                        if (!e.MoveNext()) break;
                        this.AddFilters.Add((String)e.Current);
                        break;

                    case "-i":
                        if (!e.MoveNext()) break;
                        this.CreateFilter = true;
                        this.CreateFilters.Add((String)e.Current);
                        break;

                    case "-j":
                        this.CreateFilter = true;
                        this.DeleteFilter = true;
                        this.FilterName = (String)e.Current;
                        break;

                    case "-f":
                        this.CreateFilter = true;
                        this.SaveToDisk = (String)e.Current;
                        break;

                    case "-r":
                        if (!e.MoveNext()) break;
                        this.SaveEvery = Convert.ToUInt32((String)e.Current);
                        break;

                    case "-m":
                        if (!e.MoveNext()) break;
                        this.DiskFile = Convert.ToUInt32((String)e.Current);
                        break;

                    case "-l":
                        this.ListAdapters = true;
                        break;

                    case "-g":
                        this.ListCaptures = true;
                        break;

                    case "-y":
                        this.ListFilters = true;
                        break;

                    //                     case "-k":
                    //                         this.ListPackets = true;
                    //                         break;

                    case "-s":
                        if (!e.MoveNext()) break;
                        a = (String)e.Current;
                        foreach (String stat in a.Split(','))
                        {
                            switch (stat)
                            {
                                case "node":
                                    this.NodeStats = true;
                                    break;
                                case "flow":
                                    this.FlowStats = true;
                                    break;
                                case "summary":
                                    this.SummaryStats = true;
                                    break;
                                case "protocol":
                                    this.ProtocolStats = true;
                                    break;
                                default:
                                    // error;
                                    break;
                            }
                        }
                        break;

                    case "-v":
                        if (!e.MoveNext()) break;
                        this.Verbose = Convert.ToUInt32((String)e.Current);
                        break;

                    case "--csv":
                        this.CSV = true;
                        break;

                    case "--wireless_channel":
                        if (!e.MoveNext()) break;
                        this.SetWirelessChannel = true;
                        this.WirelessChannelNumber = Convert.ToUInt32((String)e.Current);
                        break;

                    case "--wireless_frequency":
                        if (!e.MoveNext()) break;
                        this.SetWirelessChannel = true;
                        this.WirelessChannelFrequency = Convert.ToUInt32((String)e.Current);
                        break;

                    case "--format":
                        if (!e.MoveNext()) break;
                        this.FileFormat = (String)e.Current;
                        break;

                    case "--template":
                        if (!e.MoveNext()) break;
                        this.CaptureTemplate = (String)e.Current;
                        break;

                    case "--reserve":
                        if (!e.MoveNext()) break;
                        i = Convert.ToUInt32((String)e.Current);
                        if (i < 16)
                        {
                            this.ReservedSize = 16;
                        }
                        else
                        {
                            this.ReservedSize = (i / 8) * 8;
                        }
                        break;

                    case "--sec":
                        if (!e.MoveNext()) break;
                        this.CaptureSeconds = Convert.ToUInt32((String)e.Current);
                        break;

                    case "--min":
                        if (!e.MoveNext()) break;
                        this.CaptureMinutes = Convert.ToUInt32((String)e.Current);
                        break;

                    case "-?":
                        this.DisplayHelp = true;
                        break;
                }
            }
        }

        public void Process()
        {
            if (this.DisplayHelp)
            {
                Help();
                return;
            }

            if (this.omni == null)
            {
                this.logger.Critical("No OmniScript object.");
                return;
            }

            if (!this.Connect())
            {
                this.logger.Critical("Failed to connect.");
                return;
            }

            if (this.ListAdapters)
            {
                this.logger.Info("List Captures");
                this.ListAdapterList();
            }

            if (this.ListCaptures)
            {
                this.logger.Info("List Captures");
                this.ListCaptureList();
            }

            if (this.ListFilters)
            {
                this.logger.Info("List Filters");
                this.ListFilterList();
            }

            if (this.CreateFilter)
            {
                this.logger.Info("Create Filter");
                this.FiltersCreate();
            }

            if (this.CreateCapture)
            {
                this.logger.Info("Create Capture");
                this.CaptureCreate();
            }

            if (this.StartCapture)
            {
                this.logger.Info("Start Capture");
                CaptureStart();
            }

            if (this.CaptureMinutes != 0)
            {
                this.logger.Info("Capture for -n- minutes.");
                Thread.Sleep((int)this.CaptureMinutes * 60 * 1000); // Minutes to milliseconds.
            }

            if (this.CaptureSeconds != 0)
            {
                this.logger.Info("Capture for -n- seconds.");
                Thread.Sleep((int)this.CaptureSeconds * 1000);  // Seconds to milliseconds.
            }

            if (this.StopCapture)
            {
                this.logger.Info("Stop Capture");
                CaptureStop();
            }

            if (this.SaveCapture)
            {
                this.logger.Info("Save Capture");
                CaptureSave();
            }

            if (this.DeleteCapture)
            {
                this.logger.Info("Delete Capture");
                CaptureDelete();
            }

            if (this.NodeStats)
            {
                this.logger.Info("Node Capture");
                StatsNode();
            }

            if (this.FlowStats)
            {
                this.logger.Info("Flow Stats");
                //FlowStats();
            }

            if (this.SummaryStats)
            {
                this.logger.Info("Summary Stats");
                StatsSummary();
            }

            if (this.ProtocolStats)
            {
                this.logger.Info("Protocol Stats");
                StatsProtocol();
            }

            if (this.DeleteFilter)
            {
                this.logger.Info("Delete Filter");
                FilterDelete();
            }

            if (this.SetWirelessChannel)
            {
                this.logger.Info("Set Wireless Channel");
                //SetWirelessChannel();
            }

            if (this.ResetCapture)
            {
                this.logger.Info("Reset Capture");
                CaptureReset();
            }
        }

        public void StatsFlow()
        {
            if (String.IsNullOrEmpty(this.CaptureName))
            {
                this.logger.Error("Invalid or missing capture name (-n).");
                return;
            }
            if (!this.IsConnected())
            {
                this.logger.Critical("No connection.");
                return;
            }

            this.capture = this.engine.FindCapture(this.CaptureName);
            if (this.capture == null)
            {
                this.logger.Error("Did not find the capture: " + this.CaptureName);
                return;
            }

            this.logger.Critical("Flow stats is not implemented yet.");
            //SummaryStats stats = this.capture.GetFlowStats();
            //if (stats == null)
            //{
            //    this.logger.Error("Failed to parse summary stats for capture: " + this.CaptureName);
            //    return;
            //}
            //foreach (KeyValuePair<String, Dictionary<String, String>> group in stats)
            //{
            //    this.logger.Log("Group: " + group.Key);
            //    foreach (KeyValuePair<String, String> kvp in group.Value)
            //    {
            //        this.logger.Log(kvp.Key + " : " + kvp.Value);
            //    }
            //}
        }

        public void StatsNode()
        {
            if (String.IsNullOrEmpty(this.CaptureName))
            {
                this.logger.Error("Invalid or missing capture name (-n).");
                return;
            }
            if (!this.IsConnected())
            {
                this.logger.Critical("No connection.");
                return;
            }

            this.capture = this.engine.FindCapture(this.CaptureName);
            if (this.capture == null)
            {
                this.logger.Error("Did not find the capture: " + this.CaptureName);
                return;
            }

            List<NodeStatistic> stats = this.capture.GetNodeStats();
            if (stats == null)
            {
                this.logger.Error("Failed to parse node stats for capture: " + this.CaptureName);
                return;
            }

//             this.logger.Log("Node Stats:");
//             foreach (Dictionary<String, String> stat in stats)
//             {
//                 foreach (KeyValuePair<String, String> kvp in stat)
//                 {
//                     this.logger.Log(kvp.Key + " : " + kvp.Value);
//                 }
//                 this.logger.Log(" ");
//             }
        }

        public void StatsProtocol()
        {
            if (String.IsNullOrEmpty(this.CaptureName))
            {
                this.logger.Error("Invalid or missing capture name (-n).");
                return;
            }
            if (!this.IsConnected())
            {
                this.logger.Critical("No connection.");
                return;
            }

            this.capture = this.engine.FindCapture(this.CaptureName);
            if (this.capture == null)
            {
                this.logger.Error("Did not find the capture: " + this.CaptureName);
                return;
            }

            List<ProtocolStatistic> stats = this.capture.GetProtocolStats();
            if (stats == null)
            {
                this.logger.Error("Failed to parse protocol stats for capture: " + this.CaptureName);
                return;
            }
//             this.logger.Log("Protocol Stats:");
//             foreach (Dictionary<String, String> stat in stats)
//             {
//                 foreach (KeyValuePair<String, String> kvp in stat)
//                 {
//                     this.logger.Log(kvp.Key + " : " + kvp.Value);
//                 }
//                 this.logger.Log(" ");
//             }
        }

        public void StatsSummary()
        {
            if (String.IsNullOrEmpty(this.CaptureName))
            {
                this.logger.Error("Invalid or missing capture name (-n).");
                return;
            }
            if (!this.IsConnected())
            {
                this.logger.Critical("No connection.");
                return;
            }

            this.capture = this.engine.FindCapture(this.CaptureName);
            if (this.capture == null)
            {
                this.logger.Error("Did not find the capture: " + this.CaptureName);
                return;
            }

            SummaryStatistics stats = this.capture.GetSummaryStats();
            if (stats == null)
            {
                this.logger.Error("Failed to parse summary stats for capture: " + this.CaptureName);
                return;
            }
//             foreach (KeyValuePair<String, Dictionary<String, String>> group in stats)
//             {
//                 this.logger.Log("Group: " + group.Key);
//                 foreach (KeyValuePair<String, String> kvp in group.Value)
//                 {
//                     this.logger.Log(kvp.Key + " : " + kvp.Value);
//                 }
//             }
        }

        public String VersionGet()
        {
            if (!this.IsConnected())
            {
                this.logger.Critical("No connection.");
            }
            return this.engine.GetVersion();
        }
    }
}
