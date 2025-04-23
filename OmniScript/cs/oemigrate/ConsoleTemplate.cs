// =============================================================================
// <copyright file="ConsoleTemplate.cs" company="Savvius, Inc.">
//  Copyright (c) 2015 Savvius, Inc. All rights reserved.
//  Copyright (c) 2013 WildPackets, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System;
    using System.Runtime.InteropServices;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Xml.Linq;
    using System.Text;
    using System.Globalization;
    using Savvius.Omni.OmniScript;

    public class ConsoleTemplate
    {
        private static String BooleanString(bool value)
        {
            return ((value) ? "TRUE" : "FALSE");
        }

        private static bool BooleanValue(String value)
        {
            return (value.ToUpper() == "TRUE");
        }

        private static void AddBoolean(XElement node, String element, String name, bool enabled)
        {
            node.Add(MakeBoolean(element, name, enabled));
        }

        private static void AddEnabled(XElement node, String name, bool enabled)
        {
            AddBoolean(node, name, "Enabled", enabled);
        }

        private static bool IsEnabled(XElement node)
        {
            if (node != null)
            {
                XAttribute enabled = node.Attribute("Enabled");
                if (enabled != null)
                {
                    return BooleanValue(enabled.Value);
                }
            }
            return false;
        }

        private static XElement MakeBoolean(String element, String name, bool enabled)
        {
            return new XElement(element, new XAttribute(name, BooleanString(enabled)));
        }

        private static XElement MakeEnabled(String name, bool enabled)
        {
            return MakeBoolean(name, "Enabled", enabled);
        }

        public class PeekId
        {
            public Guid Id { set; get; }

            public PeekId()
            {
            }

            public void Load(XElement node)
            {
                // id  = "0xb143669a, 0x7f95, 0x4cd6, 0xaa, 0xc4, 0x01, 0x91, 0xbf, 0xf6, 0xa6, 0x99"
                // id1 = "b143669a, 7f95, 4cd6, aa, c4, 01, 91, bf, f6, a6, 99"
                // id2 = "b143669a7f954cd6aac40191bff6a699"
                String id = node.Attribute("ID").Value.ToString();
                // All elements have leading zeros: 0x01 (not 0x1).
                String id1 = id.Replace("0x", "");
                String id2 = id1.Replace(", ", "");
                Id = Guid.Parse(id2);
            }

            public XAttribute Store()
            {
                String id = this.Id.ToString("x");
                String id1 = id.Replace("{", "");
                String id2 = id1.Replace("}", "");
                String id3 = id2.Replace(",", ", ");
                return new XAttribute("ID", id3);
            }

            public override String ToString()
            {
                return this.Id.ToString("B").ToUpper();
            }
        }

        public class ConfigFilter
        {
            public String Mode { get; set; }
            public String MatchMode { get; set;}
            public List<String> filterList { get; set; }

            public ConfigFilter()
            {
                this.filterList = new List<String>();
            }

            public void Load(XElement capture)
            {
                XElement filters = capture.Element("Filters");
                if (filters != null)
                {
                    this.Mode = filters.Attribute("Mode").Value.ToString();
                    this.MatchMode = filters.Attribute("MatchMode").Value.ToString();
                    IEnumerable<XElement> filterList = filters.Elements("Filter");
                    foreach (XElement filter in filterList)
                    {
                        this.filterList.Add(filter.Attribute("Name").Value.ToString());
                    }
                }
            }

            public void SetFilters(List<String> filters)
            {
                this.filterList.Clear();
                this.filterList.AddRange(filters);
            }

            public List<XElement> Store()
            {
                List<XElement> list = new List<XElement>();
                if (this.filterList.Count > 0)
                {
                    XElement filters = new XElement("Filters",
                        new XAttribute("Mode", this.Mode),
                        new XAttribute("MatchMode", this.MatchMode));
                    foreach (String name in this.filterList)
                    {
                        filters.Add(new XElement("Filter", new XAttribute("Name", name)));
                    }
                    list.Add(filters);
                }
                return list;
            }
        };

        public class EnabledPlugin
        {
            public bool Enabled { get; set; }
            public PeekId Id { get; set; }

            public EnabledPlugin()
            {
                this.Enabled = false;
                this.Id = new PeekId();
            }

            public EnabledPlugin(XElement node)
                : this()
            {
                this.Load(node);
            }

            public void Load(XElement node)
            {
                this.Enabled = IsEnabled(node);
                this.Id.Load(node);
            }

            public XElement Store()
            {
                XElement plugin = MakeEnabled("Plugin", this.Enabled);
                plugin.Add(this.Id.Store());
                return plugin;
            }
        }

        public class PluginSettings
        {
            public PeekId Id { get; set; }
            public String Settings { get; set; }

            public PluginSettings()
            {
                this.Id = new PeekId();
            }

            public void Load(XElement plugin)
            {
                Id.Load(plugin);
                this.Settings = plugin.FirstNode.ToString(SaveOptions.DisableFormatting);
            }

            public void Store(XElement plugins)
            {
                XElement settings = XElement.Parse(this.Settings);
                XElement plugin = new XElement("Plugins", Id.Store(),
                    settings);
                plugins.Add(plugin);
            }
        };

        public class SettingsAdapter
        {
            public String Name { get; set; }
            public uint Type { get; set; }
            public String Id { get; set; }

            public SettingsAdapter()
            {
                this.Name = "";
                this.Type = 0;
                this.Id = "";
            }

            public void Load(XElement capture)
            {
                LoadAdapter(capture.Element("Adapter"));
            }

            public void LoadAdapter(XElement node)
            {
                XElement adapter = node.Element("Adapter");
                if (adapter != null)
                {
                    XAttribute name = adapter.Attribute("AdapterName");
                    if (name != null)
                    {
                        this.Name = name.Value.ToString();
                    }
                    XAttribute type = adapter.Attribute("AdapterType");
                    if (type != null)
                    {
                        this.Type = Convert.ToUInt32(type.Value);
                    }
                    XAttribute id = adapter.Attribute("AdapterID");
                    if (id != null)
                    {
                        this.Id = id.Value.ToString();
                    }
                }
            }

            public XElement Store()
            {
                XElement adapter = new XElement("Adapter");
                StoreAdapter(adapter);
                return adapter;
            }

            public void StoreAdapter(XElement node)
            {
                XElement adapter = new XElement("Adapter");
                adapter.Add(new XAttribute("AdapterType", this.Type.ToString()));
                if (!String.IsNullOrEmpty(this.Name))
                {
                    adapter.Add(new XAttribute("AdapterName", this.Name));
                }
                if (!String.IsNullOrEmpty(this.Id))
                {
                    adapter.Add(new XAttribute("AdapterID", this.Id));
                }
                node.Add(adapter);
            }
        };

        public class SettingsGeneral
        {
            public ulong BufferSize { get; set; }
            public bool CaptureToDisk { get; set; }
            public bool ContinuousCapture { get; set; }
            public String CtdFilePattern { get; set; }
            public ulong CtdFileSize { get; set; }
            public bool CtdKeepLastFiles { get; set; }
            public uint CtdKeepLastFilesCount { get; set; }
            public ulong CtdMaxTotalFileSize { get; set; }
            public ulong CtdReservedSize { get; set; }
            public bool CtdUseMaxFileAge { get; set; }
            public bool CtdUseMaxTotalFileSize { get; set; }
            public ulong CtdMaxFileAge { get; set; }
            public String CaptureName { get; set; }
            public bool SaveAsTemplate { get; set; }
            public uint SliceLength { get; set; }
            public bool Slicing { get; set; }
            public bool StartCapture { get; set; }

            public bool Deduplicate { get; set; }
            public String Location { get; set; }
            public SettingsAdapter SendAdapter { get; set; }

            public SettingsGeneral()
            {
                this.BufferSize = 4 * OmniScript.bytesInAMegabyte;
                this.CaptureToDisk = false;
                this.ContinuousCapture = true;
                this.CtdFilePattern = null;
                this.CtdFileSize = 2 * OmniScript.bytesInAGigabyte;
                this.CtdKeepLastFiles = false;
                this.CtdKeepLastFilesCount = 10;
                this.CtdMaxTotalFileSize = 10 * OmniScript.bytesInAGigabyte;
                this.CtdReservedSize = 16384;
                this.CtdUseMaxFileAge = false;
                this.CtdUseMaxTotalFileSize = false;
                this.CtdMaxFileAge = OmniScript.secondsInADay;
                this.CaptureName = null;
                this.SaveAsTemplate = false;
                this.SliceLength = 128;
                this.Slicing = false;
                this.StartCapture = false;

                this.Deduplicate = false;
                this.SendAdapter = new SettingsAdapter();
            }

            public void Load(XElement capture)
            {
                this.CaptureName = capture.Element("Name").Value.ToString();

                XElement buffer = capture.Element("Buffer");
                this.BufferSize = Convert.ToUInt64(buffer.Attribute("Size").Value);
                XAttribute slice = buffer.Attribute("Slice");
                this.Slicing = (slice != null);
                if (this.Slicing)
                {
                    this.SliceLength = Convert.ToUInt32(slice.Value);
                }
                XAttribute deduplicate = buffer.Attribute("Deduplicate");
                if (deduplicate != null)
                {
                    this.Deduplicate = (Convert.ToUInt32(deduplicate.Value) != 0);
                }

                XElement continuous = capture.Element("Continuous");
                if (continuous != null)
                {
                    this.ContinuousCapture = (continuous.Attribute("UseRing").Value == "TRUE");
                    XElement saveToDisk = continuous.Element("SaveToDisk");
                    this.CaptureToDisk = (saveToDisk != null);
                    if (this.CaptureToDisk)
                    {
                        this.CtdFilePattern = saveToDisk.Attribute("Path").Value.ToString();
                        this.CtdFileSize = Convert.ToUInt64(saveToDisk.Attribute("FileSize").Value);
                        XAttribute wrapFiles = saveToDisk.Attribute("WrapFiles");
                        if (wrapFiles != null)
                        {
                            this.CtdKeepLastFiles = true;
                            this.CtdKeepLastFilesCount = Convert.ToUInt32(wrapFiles.Value);
                        }
                        XAttribute saveInterval = saveToDisk.Attribute("SaveInterval");
                        if (saveInterval != null)
                        {
                            this.CtdUseMaxFileAge = true;
                            this.CtdMaxFileAge = Convert.ToUInt64(saveInterval.Value);
                        }
                    }
                }
                else
                {
                    this.ContinuousCapture = false;
                }

                XElement location = capture.Element("Location");
                if (location != null)
                {
                    this.Location = location.Value.ToString();
                }

                this.SendAdapter.LoadAdapter(capture.Element("SendAdapter"));
            }

            public List<XElement> Store()
            {
                List<XElement> list = new List<XElement>();
                list.Add(new XElement("Name", this.CaptureName));

                XElement buffer = new XElement("Buffer",
                    new XAttribute("Size", this.BufferSize));
                if (this.Slicing)
                {
                    buffer.Add(new XAttribute("Slice", this.SliceLength));
                }
                if (this.Deduplicate)
                {
                    buffer.Add(new XAttribute("Deduplicate", "1"));
                }
                list.Add(buffer);

                if (this.ContinuousCapture)
                {
                    XElement continuous = new XElement("Continuous",
                        new XAttribute("UseRing", "TRUE"));
                    if (this.CaptureToDisk)
                    {
                        XElement saveToDisk = new XElement("SaveToDisk");

                        if (this.CtdFilePattern != null)
                        {
                            saveToDisk.Add(new XAttribute("Path", this.CtdFilePattern));
                        }
                        if (this.CtdFileSize != 0)
                        {
                            saveToDisk.Add(new XAttribute("FileSize", this.CtdFileSize));
                        }
                        if (this.CtdKeepLastFiles)
                        {
                            saveToDisk.Add(new XAttribute("WrapFiles", this.CtdKeepLastFilesCount));
                        }
                        if (this.CtdUseMaxFileAge)
                        {
                            saveToDisk.Add(new XAttribute("SaveInterval", this.CtdMaxFileAge.ToString()));
                        }
                        if (saveToDisk.HasAttributes)
                        {
                            continuous.Add(saveToDisk);
                        }
                    }
                    list.Add(continuous);
                }

                if (!String.IsNullOrEmpty(this.Location))
                {
                    list.Add(new XElement("Location", this.Location));
                }

                XElement sendAdapter = new XElement("SendAdapter");
                this.SendAdapter.StoreAdapter(sendAdapter);
                list.Add(sendAdapter);
                return list;
            }
        };

        public class SettingsPlugins
        {
            public List<PluginSettings> PluginList { get; set; }

            public SettingsPlugins()
            {
                this.PluginList = new List<PluginSettings>();
            }

            public void Load(XElement capture)
            {
                XElement pluginsNode = capture.Element("Plugins");
                if (pluginsNode != null)
                {
                    IEnumerable<XElement> plugins = pluginsNode.Elements("Plugins");
                    foreach (XElement plugin in plugins)
                    {
                        PluginSettings settings = new PluginSettings();
                        settings.Load(plugin);
                        PluginList.Add(settings);
                    }
                }
            }

            public List<XElement> Store()
            {
                List<XElement> list = new List<XElement>();
                if (this.PluginList.Count > 0)
                {
                    XElement plugins = new XElement("Plugins");
                    foreach (PluginSettings settings in this.PluginList)
                    {
                        settings.Store(plugins);
                    }
                    list.Add(plugins);
                }
                return list;
            }
        };

        public class SettingsSelective
        {
            public class StatsLimit
            {
                public bool Enabled { get; set; }
                public uint Count { get; set; }
                public bool Notify { get; set; }
                public uint Severity { get; set; }
                public bool DoLimit { get; set; }
                public bool Stop { get; set; }
                public bool Reset { get; set; }

                public StatsLimit()
                {
                    this.Enabled = false;
                    this.Count = 0;
                    this.Notify = false;
                    this.Severity = 0;
                    this.DoLimit = false;
                    this.Stop = false;
                    this.Reset = false;
                }

                public void Load(XElement node, String name)
                {
                    XElement stat = node.Element(name);
                    if (stat != null)
                    {
                        this.Enabled = IsEnabled(stat);

                        XElement limit = stat.Element("StatsLimit");
                        if (limit != null)
                        {
                            this.Count = Convert.ToUInt32(limit.Attribute("LimitCount").Value);
                            this.Notify = BooleanValue(limit.Attribute("Notify").Value);
                            this.Severity = Convert.ToUInt32(limit.Attribute("Severity").Value);
                            this.DoLimit = BooleanValue(limit.Attribute("DoLimit").Value);
                            this.Stop = BooleanValue(limit.Attribute("Stop").Value);
                            this.Reset = BooleanValue(limit.Attribute("Reset").Value);
                        }
                    }
                }

                public void Store(XElement node, String name)
                {
                    XElement stat = MakeEnabled(name, this.Enabled);
                    XElement limit = new XElement("StatsLimit");
                    limit.Add(new XAttribute("LimitCount", this.Count.ToString()));
                    limit.Add(new XAttribute("Notify", BooleanString(this.Notify)));
                    limit.Add(new XAttribute("Severity", this.Severity.ToString()));
                    limit.Add(new XAttribute("DoLimit", BooleanString(this.DoLimit)));
                    limit.Add(new XAttribute("Stop", BooleanString(this.Stop)));
                    limit.Add(new XAttribute("Reset", BooleanString(this.Reset)));

                    stat.Add(limit);
                    node.Add(stat);
                }
            };

            public class VoipOptions
            {
                public bool Enabled { get; set; }
                public uint MaxCalls { get; set; }
                public bool Notify { get; set; }
                public uint Severity { get; set; }
                public bool Stop { get; set; }

                public VoipOptions()
                {
                    this.Enabled = false;
                    this.MaxCalls = 0;
                    this.Notify = false;
                    this.Severity = 0;
                    this.Stop = false;
                }

                public void Load(XElement node, String name)
                {
                    XElement stat = node.Element(name);
                    if (stat != null)
                    {
                        this.Enabled = IsEnabled(stat);

                        XElement options = stat.Element("VoIPOptions");
                        if (options != null)
                        {
                            this.MaxCalls = Convert.ToUInt32(options.Attribute("MaxCalls").Value);
                            this.Notify = BooleanValue(options.Attribute("Notify").Value);
                            this.Severity = Convert.ToUInt32(options.Attribute("Severity").Value);
                            this.Stop = BooleanValue(options.Attribute("StopAnalysis").Value);
                        }
                    }
                }

                public void Store(XElement node, String name)
                {
                    XElement stat = MakeEnabled(name, this.Enabled);
                    XElement options = new XElement("VoIPOptions");
                    options.Add(new XAttribute("MaxCalls", this.MaxCalls.ToString()));
                    options.Add(new XAttribute("Notify", BooleanString(this.Notify)));
                    options.Add(new XAttribute("Severity", this.Severity.ToString()));
                    options.Add(new XAttribute("StopAnalysis", BooleanString(this.Stop)));

                    stat.Add(options);
                    node.Add(stat);
                }
            };

            public StatsLimit Conversation { get; set; }
            public bool CaptureToDisk { get; set; }
            public bool Error { get; set; }
            public bool Expert { get; set; }
            public bool Graphs { get; set; }
            public bool History { get; set; }
            public bool History2 { get; set; }
            public StatsLimit Node { get; set; }
            public bool Node2 { get; set; }
            public bool Network { get; set; }
            public bool PacketList { get; set; }
            public bool PeerMap { get; set; }
            public bool Plugins { get; set; }
            public List<EnabledPlugin> EnabledPlugins { get; set; }
            public StatsLimit ProtocolById { get; set; }
            public bool ProtocolByInstance { get; set; }
            public bool Size { get; set; }
            public bool Summary { get; set; }
            public bool TrafficHistory { get; set; }
            public VoipOptions Voice { get; set; }
            public bool WebView { get; set; }
            public bool WirelessNode { get; set; }
            public bool WirelessChannel { get; set; }

            public SettingsSelective()
            {
                this.Conversation = new StatsLimit();
                this.CaptureToDisk = false;
                this.Error = false;
                this.Expert = false;
                this.Graphs = false;
                this.History = false;
                this.History2 = false;
                this.Node = new StatsLimit();
                this.Node2 = false;
                this.Network = false;
                this.PacketList = false;
                this.PeerMap = false;
                this.Plugins = false;
                this.EnabledPlugins = new List<EnabledPlugin>();
                this.ProtocolById = new StatsLimit();
                this.ProtocolByInstance = false;
                this.Size = false;
                this.Summary = false;
                this.TrafficHistory = false;
                this.Voice = new VoipOptions();
                this.WebView = false;
                this.WirelessNode = false;
                this.WirelessChannel = false;
            }

            public void Load(XElement capture)
            {
                XElement selective = capture.Element("Selective");
                if (selective != null)
                {
                    this.Conversation.Load(selective, "ConversationStats");
                    this.CaptureToDisk = IsEnabled(selective.Element("CaptureToDisk"));
                    this.Error = IsEnabled(selective.Element("ErrorStats"));
                    this.Expert = IsEnabled(selective.Element("Expert"));
                    this.Graphs = IsEnabled(selective.Element("Graphs"));
                    this.History = IsEnabled(selective.Element("HistoryStats"));
                    this.History2 = IsEnabled(selective.Element("HistoryStats2"));
                    this.Node.Load(selective, "NodeStats");
                    this.Node2 = IsEnabled(selective.Element("NodeStats2"));
                    this.Network = IsEnabled(selective.Element("NetworkStats"));
                    this.PacketList = IsEnabled(selective.Element("PacketList"));
                    this.PeerMap = IsEnabled(selective.Element("PeerMap"));
                    this.Plugins = IsEnabled(selective.Element("Plugins"));
                    IEnumerable<XElement> plugins = selective.Element("Plugins").Elements("Plugin");
                    foreach (XElement plugin in plugins)
                    {
                        this.EnabledPlugins.Add(new EnabledPlugin(plugin));
                    }
                    this.ProtocolById.Load(selective, "ProtocolStatsByID");
                    this.ProtocolByInstance = IsEnabled(selective.Element("ProtocolStatsByInstance"));
                    this.Size = IsEnabled(selective.Element("SizeStats"));
                    this.Summary = IsEnabled(selective.Element("SummaryStats"));
                    this.TrafficHistory = IsEnabled(selective.Element("TrafficHistoryStats"));
                    this.Voice.Load(selective, "Voice");
                    this.WebView = IsEnabled(selective.Element("WebView"));
                    this.WirelessNode = IsEnabled(selective.Element("WirelessNodeStats"));
                    this.WirelessChannel = IsEnabled(selective.Element("WirelessChannelStats"));
                }
            }

            public XElement Store()
            {
                XElement selective = new XElement("Selective");
                this.Conversation.Store(selective, "ConversationStats");
                AddEnabled(selective, "CaptureToDisk", this.CaptureToDisk);
                AddEnabled(selective, "ErrorStats", this.Error);
                AddEnabled(selective, "Expert", this.Expert);
                AddEnabled(selective, "Graphs", this.Graphs);
                AddEnabled(selective, "HistoryStats", this.History);
                AddEnabled(selective, "HistoryStats2", this.History2);
                this.Node.Store(selective, "NodeStats");
                AddEnabled(selective, "NodeStats2", this.Node2);
                AddEnabled(selective, "NetworkStats", this.Network);
                AddEnabled(selective, "PacketList", this.PacketList);
                AddEnabled(selective, "PeerMap", this.PeerMap);
                AddEnabled(selective, "Plugins", this.Plugins);
                XElement plugins = selective.Element("Plugins");
                foreach (EnabledPlugin plugin in this.EnabledPlugins)
                {
                    plugins.Add(plugin.Store());
                }
                this.ProtocolById.Store(selective, "ProtocolStatsByID");
                AddEnabled(selective, "ProtocolStatsByInstance", this.ProtocolByInstance);
                AddEnabled(selective, "SizeStats", this.Size);
                AddEnabled(selective, "SummaryStats", this.Summary);
                AddEnabled(selective, "TrafficHistoryStats", this.TrafficHistory);
                this.Voice.Store(selective, "Voice");
                AddEnabled(selective, "WebView", this.WebView);
                AddEnabled(selective, "WirelessNodeStats", this.WirelessNode);
                AddEnabled(selective, "WirelessChannelStats", this.WirelessChannel);
                return selective;
            }
        };

        public class OutputStats
        {
            public class FileSet
            {
                public bool Enabled { get; set; }
                public bool ScheduleNewSet { get; set; }
                public uint Interval { get; set; }
                private OmniScript.IntervalUnits _intervalUnits { get; set; }
                public OmniScript.IntervalUnits IntervalUnits
                {
                    get { return this._intervalUnits; }
                    set
                    {
                        if ((value >= OmniScript.IntervalUnits.Seconds) && (value <= OmniScript.IntervalUnits.Days))
                        {
                            this._intervalUnits = (OmniScript.IntervalUnits)value;
                        }
                        else
                        {
                            SystemException ex = null;
                            throw new ArgumentOutOfRangeException(@"Interval Unit must be an OmniScript.IntervalUnits.", ex);
                        }
                    }
                }
                public bool AlignToTime { get; set; }
                public bool ResetOnNew { get; set; }
                public bool KeepRecent { get; set; }
                public uint KeepCount { get; set; }

                public FileSet(XElement stats = null)
                {
                    this.Enabled = false;
                    this.ScheduleNewSet = false;
                    this.Interval = 1;
                    this.IntervalUnits = OmniScript.IntervalUnits.Hours;
                    this.AlignToTime = false;
                    this.ResetOnNew = false;
                    this.KeepRecent = false;
                    this.KeepCount = 1;

                    if (stats != null)
                    {
                        this.Load(stats);
                    }
                }

                public void Load(XElement stats)
                {
                    XElement fileset = stats.Element("NewFileSet");
                    if (fileset != null)
                    {
                        this.Enabled = IsEnabled(fileset);
                        this.ScheduleNewSet = BooleanValue(fileset.Attribute("ScheduleNewSet").Value);
                        this.Interval = Convert.ToUInt32(fileset.Attribute("Interval").Value);
                        this.IntervalUnits = OmniScript.InvervalUnitsMap[fileset.Attribute("IntervalUnits").Value.ToString().ToLower()];
                        this.AlignToTime = BooleanValue(fileset.Attribute("AlignToTime").Value);
                        this.ResetOnNew = BooleanValue(fileset.Attribute("ResetOnNewFileSet").Value);
                        this.KeepRecent = BooleanValue(fileset.Attribute("KeepMostRecent").Value);
                        this.KeepCount = Convert.ToUInt32(fileset.Attribute("MostRecentCount").Value);
                    }
                }

                public XElement Store()
                {
                    return new XElement("NewFileSet",
                        new XAttribute("Enabled", BooleanString(this.Enabled)),
                        new XAttribute("ScheduleNewSet", BooleanString(this.ScheduleNewSet)),
                        new XAttribute("Interval", this.Interval),
                        new XAttribute("IntervalUnits", this.IntervalUnits.ToString().ToLower()),
                        new XAttribute("ResetOnNewFileSet", BooleanString(this.ResetOnNew)),
                        new XAttribute("KeepMostRecent", BooleanString(this.KeepRecent)),
                        new XAttribute("MostRecentCount", this.KeepCount));
                }
            }

            public bool Enabled { get; set; }
            public String Path { get; set; }
            public String Format { get; set; }
            public String Type { get; set; }
            public uint Interval { get; set; }
            private OmniScript.IntervalUnits _intervalUnits { get; set; }
            public OmniScript.IntervalUnits IntervalUnits
            {
                get { return _intervalUnits; }
                set
                {
                    if ((value >= OmniScript.IntervalUnits.Seconds) && (value <= OmniScript.IntervalUnits.Days))
                    {
                        this._intervalUnits = (OmniScript.IntervalUnits)value;
                    }
                    else
                    {
                        SystemException ex = null;
                        throw new ArgumentOutOfRangeException(@"Interval Unit must be an OmniScript.IntervalUnits.", ex);
                    }
                }
            }
            public bool AlignToTime { get; set; }
            public bool Log { get; set; }
            public bool Clear { get; set; }
            public FileSet NewFileSet { get; set; }

            public OutputStats(XElement capture = null)
            {
                this.Enabled = false;
                this.Path = null;
                this.Format = null;
                this.Type = null;
                this.Interval = 1;
                this.IntervalUnits = OmniScript.IntervalUnits.Hours;
                this.AlignToTime = false;
                this.Log = false;
                this.Clear = false;
                this.NewFileSet = null;

                if (capture != null)
                {
                    Load(capture);
                }
            }

            public void Load(XElement capture)
            {
                XElement stats = capture.Element("StatsOutputEx");
                if (stats != null)
                {
                    this.Enabled = IsEnabled(stats);
                    this.Path = stats.Attribute("Path").Value;
                    this.Format = stats.Attribute("ReportType").Value;
                    this.Type = stats.Attribute("ReportTypeType").Value;
                    this.Interval = Convert.ToUInt32(stats.Attribute("Interval").Value);
                    this.IntervalUnits = OmniScript.InvervalUnitsMap[stats.Attribute("IntervalUnits").Value.ToString().ToLower()];
                    this.AlignToTime = BooleanValue(stats.Attribute("AlignToTime").Value);
                    this.Log = BooleanValue(stats.Attribute("Log").Value);
                    this.Clear = BooleanValue(stats.Attribute("Clear").Value);

                    XElement fileset = stats.Element("NewFileSet");
                    if (fileset != null)
                    {
                        this.NewFileSet = new FileSet(stats);
                    }
                }
            }

            public XElement Store()
            {
                if (this.Enabled)
                {
                    XElement stats = new XElement("StatsOutputEx",
                        new XAttribute("Enabled", BooleanString(this.Enabled)),
                        new XAttribute("Path", this.Path),
                        new XAttribute("ReportType", this.Format),
                        new XAttribute("ReportTypeType", this.Type),
                        new XAttribute("Interval", this.Interval),
                        new XAttribute("IntervalUnits", this.IntervalUnits.ToString().ToLower()),
                        new XAttribute("AlignToTime", BooleanString(this.AlignToTime)),
                        new XAttribute("Log", BooleanString(this.Log)),
                        new XAttribute("Clear", BooleanString(this.Clear)));

                    if (this.NewFileSet != null)
                    {
                        stats.Add(this.NewFileSet.Store());
                    }

                    return stats;
                }
                return null;
            }
        }

        public class Trigger
        {
            public String Type { get; set; }
            public bool Enabled { get; set; }
            public bool Notify { get; set; }
            public uint Severity { get; set; }
            public ulong Time { get; set; }
            public bool UseDate { get; set; }
            public ulong BytesCaptured { get; set; }
            public uint FilterMode { get; set; }
            public List<String> Filters { get; set; }

            public Trigger(XElement capture = null)
            {
                this.Enabled = false;
                this.Notify = false;
                this.Severity = 0;
                this.Time = 0;
                this.UseDate = false;
                this.BytesCaptured = 0;
                this.FilterMode = 0;
                this.Filters = null;

                if (capture != null)
                {
                    Load(capture);
                }
            }

            public void Load(XElement capture)
            {
                this.Type = capture.Attribute("Type").Value.ToString();
                XAttribute notify = capture.Attribute("NotifySeverity");
                this.Notify = (notify != null);
                if (this.Notify)
                {
                    this.Severity = Convert.ToUInt32(notify.Value);
                }
                XElement enabled = capture.Element("Enabled");
                this.Enabled = (enabled != null);
                this.Time = 0;
                XElement time = capture.Element("Time");
                if (time != null)
                {
                    String str = time.Attribute("High").Value.ToString();
                    ulong high = Convert.ToUInt32(str.Substring(2), 16);
                    str = time.Attribute("Low").Value.ToString();
                    ulong low = Convert.ToUInt32(str.Substring(2), 16);
                    this.Time = (high << 32) | low;
                    this.UseDate = (time.Element("UseDate") != null);
                }
                this.BytesCaptured = 0;
                XElement bytes = capture.Element("BytesCaptured");
                if (bytes != null)
                {
                    this.BytesCaptured = Convert.ToUInt64(bytes.Value);
                }
                XElement filters = capture.Element("Filters");
                if (filters != null)
                {
                    String mode = filters.Attribute("Mode").Value;
                    if (mode == "Accept")
                    {
                        this.FilterMode = 1;
                    }
                    this.Filters = new List<String>();
                    IEnumerable<XElement> filter = filters.Elements("Filter");
                    foreach (XElement f in filter)
                    {
                        this.Filters.Add(f.Attribute("Name").Value.ToString());
                    }
                }
            }

            public XElement Store()
            {
                XElement trigger = new XElement("Trigger",
                    new XAttribute("Type", this.Type),
                    new XAttribute("NotifySeverity", this.Severity.ToString()));
                if (this.Enabled)
                {
                    trigger.Add(new XElement("Enabled"));
                }
                if (this.Time > 0)
                {
                    String high = String.Format("0x{0}", (this.Time >> 32).ToString("x"));
                    String low = String.Format("0x{0}", (this.Time & 0xFFFFFFFF).ToString("x"));
                    XElement time = new XElement("Time", new XAttribute("High", high), new XAttribute("Low", low));
                    if (this.UseDate)
                    {
                        time.Add(new XElement("UseDate"));
                    }
                    trigger.Add(time);
                }
                if (this.Filters != null)
                {
                    XElement filters = new XElement("Filters",
                        new XAttribute("Mode", "Accept"));
                    foreach (String f in this.Filters)
                    {
                        filters.Add(new XElement("Filter",
                            new XAttribute("Name", f)));
                    }
                    trigger.Add(filters);
                }
                return trigger;
            }
        };

        public class TriggerList
            : List<Trigger>
        {
            public Trigger Start
            {
                get
                {
                    foreach (Trigger t in this)
                    {
                        if (t.Type.ToLower() == "start")
                        {
                            return t;
                        }
                    }
                    return null;
                }
            }
            public Trigger Stop
            {
                get
                {
                    foreach (Trigger t in this)
                    {
                        if (t.Type.ToLower() == "stop")
                        {
                            return t;
                        }
                    }
                    return null;
                }
            }

            public void Load(XElement capture)
            {
                IEnumerable<XElement> triggers = capture.Elements("Trigger");
                foreach (XElement trigger in triggers)
                {
                    this.Add(new Trigger(trigger));
                }
            }

            public void Store(XElement capture)
            {
                foreach (Trigger t in this)
                {
                    capture.Add(new XElement("Trigger", t.Store()));
                }
            }
        };

        public String FileName { get; set; }
        public SettingsAdapter AdapterSettings { get; set; }
        public SettingsGeneral GeneralSettings { get; set; }
        public ConfigFilter FilterConfig { get; set; }
        public Trigger StartTrigger { get; set; }
        public Trigger StopTrigger { get; set; }
        public SettingsSelective Selective { get; set; }
        public SettingsPlugins Plugins { get; set; }
        public OutputStats StatsOutput { get; set; }

        public String AdapterName
        {
            get { return AdapterSettings.Name; }
            set { AdapterSettings.Name = value; }
        }
        public String CaptureName
        {
            get { return GeneralSettings.CaptureName; }
            set { GeneralSettings.CaptureName = value; }
        }

        public ConsoleTemplate()
        {
            this.AdapterSettings = new SettingsAdapter();
            this.GeneralSettings = new SettingsGeneral();
            this.FilterConfig = new ConfigFilter();
            this.StartTrigger = new Trigger();
            this.StopTrigger = new Trigger();
            this.Selective = new SettingsSelective();
            this.Plugins = new SettingsPlugins();
            this.StatsOutput = new OutputStats();
        }

        public ConsoleTemplate(XElement template)
            : this()
        {
            if (template != null)
            {
                Load(template);
            }
        }

        public ConsoleTemplate(String filename)
            : this()
        {
            if (!String.IsNullOrEmpty(filename))
            {
                this.FileName = filename;
                this.Load(filename);
            }
        }

        public void Load(XElement capture)
        {
            this.GeneralSettings.Load(capture);
            this.AdapterSettings.Load(capture);
            this.FilterConfig.Load(capture);
            this.Selective.Load(capture);
            this.Plugins.Load(capture);
            this.StatsOutput.Load(capture);

            TriggerList triggers = new TriggerList();
            triggers.Load(capture);
            this.StartTrigger = triggers.Start;
            this.StopTrigger = triggers.Stop;
        }

        public void Load(String filename)
        {
            XDocument doc = XDocument.Load(filename);
            XElement root = doc.Root;
            XElement capture = root.Element("CaptureWindow");
            Load(capture);
        }

        public void SetFilters(List<String> filters)
        {
            this.FilterConfig.SetFilters(filters);
        }

        public XElement Store()
        {
            XElement capture = new XElement("CaptureWindow",
                this.GeneralSettings.Store(),
                this.AdapterSettings.Store(),
                this.FilterConfig.Store(),
                this.StatsOutput.Store());
            if (this.StartTrigger != null)
            {
                capture.Add(this.StartTrigger.Store());
            }
            if (this.StartTrigger != null)
            {
                capture.Add(this.StopTrigger.Store());
            }
            capture.Add(this.Selective.Store());
            capture.Add(this.Plugins.Store());
            return capture;
        }

        public void Store(String filename)
        {
            XDocument doc = new XDocument(new XElement("CaptureSettings",
                new XAttribute("Version", "1.0"),
                this.Store()));
            doc.Save(filename);            
        }

        public void Store(CaptureTemplate template)
        {
            template.AdapterName = this.AdapterName;

            template.General.BufferSize = this.GeneralSettings.BufferSize;
            template.General.OptionCaptureToDisk = this.Selective.CaptureToDisk;
            template.General.OptionContinuousCapture = this.GeneralSettings.ContinuousCapture;
            template.General.FilePattern = this.GeneralSettings.CtdFilePattern;
            // template.General.FileSize = this.GeneralSettings.CtdFileSize;
            template.General.OptionKeepLastFiles = this.GeneralSettings.CtdKeepLastFiles;
            template.General.KeepLastFilesCount = this.GeneralSettings.CtdKeepLastFilesCount;
            template.General.FileSize = this.GeneralSettings.CtdMaxTotalFileSize;
            template.General.OptionFileAge = this.GeneralSettings.CtdUseMaxFileAge;
            template.General.OptionTotalFileSize = this.GeneralSettings.CtdUseMaxTotalFileSize;
            template.General.MaxFileAge = this.GeneralSettings.CtdMaxFileAge;
            template.General.Name = this.GeneralSettings.CaptureName;
            template.General.OptionSaveAsTemplate = this.GeneralSettings.SaveAsTemplate;
            template.General.SliceLength = this.GeneralSettings.SliceLength;
            template.General.OptionSlicing = this.GeneralSettings.Slicing;
            template.General.OptionStartCapture = this.GeneralSettings.StartCapture;

            template.Filter.Filters = this.FilterConfig.filterList;
        }
    }
}
