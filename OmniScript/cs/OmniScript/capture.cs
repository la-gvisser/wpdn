// =============================================================================
// <copyright file="Adapter.cs" company="LiveAction, Inc.">
//  Copyright (c) 2018-2021 Savvius, Inc. All rights reserved.
//  Copyright (c) 2015-2018 Savvius, Inc. All rights reserved.
//  Copyright (c) 2013-2015 WildPackets, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System;
    using System.Collections.Generic;
    using System.Xml.Linq;

    public class Capture
    {
        public static readonly Dictionary<uint, String> statusDictionay = new Dictionary<uint, String>
        {
            {0, "Idle"},
            {1, "Capturing"},
            {256, "Idle Start Active"},
            {257, "Wait Start"},
            {8192, "Idle Stop Active"},
            {8193, "Capturing Stop Active"},
            {8448, "Idle Start and Stop Active"},
            {8449, "Start Stop Active"}
        };

        public OmniEngine Engine { get; set; }
        public OmniApi Connection { get; set; }
        public OmniLogger Logger { get; set; }
        private StatsContext Context { get; set; }

        // AdapterInfo - not parsed.
        // MediaInfo - not parsed.

        /// <summary>
        /// Gets or sets the capture's adapter.
        /// </summary>
        public Adapter Adapter { get; set; }

        /// <summary>
        /// Gets or sets the name of the adapter.
        /// </summary>
        public String AdapterName { get; set; }

        /// <summary>
        /// Gets or sets the type of the adapter.
        /// </summary>
        public uint AdapterType { get; set; }

        /// <summary>
        /// Gets or sets the alarms informative level.
        /// </summary>
        public uint AlarmsInfo { get; set; }

        /// <summary>
        /// Gets or sets the alarms informative level.
        /// </summary>
        public uint AlarmsMajor { get; set; }

        /// <summary>
        /// Gets or sets the alarms informative level.
        /// </summary>
        public uint AlarmsMinor { get; set; }

        /// <summary>
        /// Gets or sets the alarms informative level.
        /// </summary>
        public uint AlarmsSevere { get; set; }

        /// <summary>
        /// Gets or sets the number of analysis dropped packets.
        /// </summary>
        public ulong AnalysisDroppedPackets { get; set; }

        /// <summary>
        /// Gets or sets the available capture buffer capacity.
        /// </summary>
        public ulong BufferAvailable { get; set; }

        /// <summary>
        /// Gets or sets the capture buffer capacity.
        /// </summary>
        public ulong BufferSize { get; set; }

        /// <summary>
        /// Gets or sets the capture buffer in use.
        /// </summary>
        public ulong BufferUsed { get; set; }

        /// <summary>
        /// Gets or sets the comment.
        /// </summary>
        public String Comment { get; set; }

        /// <summary>
        /// Gets or sets the creator of the capture as Domain\User.
        /// </summary>
        public String Creator { get; set; }

        /// <summary>
        /// Gets or sets the creator's Security Id.
        /// </summary>
        public String CreatorSId { get; set; }

        /// <summary>
        /// Gets or sets the number of discarded duplicate packets.
        /// </summary>
        public ulong DuplicatePacketsDiscarded { get; set; }

        /// <summary>
        /// Gets or sets the number of seconds/nanoseconds the capture has been capturing.
        /// </summary>
        public ulong Duration { get; set; }

        /// <summary>
        /// Gets or sets the filter mode.
        /// </summary>
        public uint FilterMode { get; set; }

        /// <summary>
        /// Gets or sets the packet number of the first packet in the capture buffer.
        /// </summary>
        public ulong FirstPacket { get; set; }

        /// <summary>
        /// Gets or sets the number of graphs used by the capture.
        /// </summary>
        public ulong GraphsCount { get; set; }

        /// <summary>
        /// Gets or sets the global identifier capture's Group.
        /// </summary>
        public OmniId GroupId { get; set; }

        /// <summary>
        /// Gets or sets the global identifier.
        /// </summary>
        public OmniId Id { get; set; }

        /// <summary>
        /// Gets or sets the last modification's user name as Domain\User.
        /// </summary>
        public String LastModifiedBy { get; set; }

        /// <summary>
        /// Gets or sets the last modification type.
        /// </summary>
        public String LastModification { get; set; }

        /// <summary>
        /// Gets or sets the link speed.
        /// </summary>
        public ulong LinkSpeed { get; set; }

        /// <summary>
        /// Gets or sets the logged on user's security id.
        /// </summary>
        public String LoggedOnUserSId { get; set; }

        /// <summary>
        /// Gets or sets the media information.
        /// </summary>
        public Object MediaInfo { get; set; }

        /// <summary>
        /// Gets or sets the media type.
        /// </summary>
        public uint MediaType { get; set; }

        /// <summary>
        /// Gets or sets the media sub-type.
        /// </summary>
        public uint MediaSubType { get; set; }

        /// <summary>
        /// Gets or sets the capture's name.
        /// </summary>
        public String Name { get; set; }

        /// <summary>
        /// Gets or sets the number of packets in the Capture Buffer.
        /// If none of the Analysis Options are enabled then a Capture Buffer
        /// is not created and the Packet Count will always be zero.
        /// </summary>
        public ulong PacketCount { get; set; }

        /// <summary>
        /// Gets or sets the number of packets analyzed.
        /// </summary>
        public ulong PacketsAnalyzed { get; set; }

        /// <summary>
        /// Gets or sets the number of packets dropped.
        /// </summary>
        public ulong PacketsDropped { get; set; }

        /// <summary>
        /// Gets or sets the number of packets Filtered.
        /// </summary>
        public ulong PacketsFiltered { get; set; }

        /// <summary>
        /// Gets or sets the number of packets received.
        /// </summary>
        public ulong PacketsReceived { get; set; }

        /// <summary>
        /// Gets or sets the number of times the capture has been reset.
        /// </summary>
        public uint ResetCount { get; set; }

        /// <summary>
        /// Gets or sets the start time as a PeekTime.
        /// </summary>
        public PeekTime StartTime { get; set; }

        /// <summary>
        /// Gets or sets the status.
        /// </summary>
        public uint Status { get; set; }

        /// <summary>
        /// Gets or sets the stop time as a PeekTime.
        /// </summary>
        public PeekTime StopTime { get; set; }

        /// <summary>
        /// Gets or sets the number of times a trigger has be triggered.
        /// </summary>
        public uint TriggerCount { get; set; }

        /// <summary>
        /// Gets or sets the duration of the last trigger in nanoseconds.
        /// </summary>
        public ulong TriggerDuration { get; set; }

        /// <summary>
        /// Gets or sets the alarms enabled flag.
        /// </summary>
        public bool OptionAlarms { get; set; }

        /// <summary>
        /// Gets or sets the the Expert enabled flag.
        /// </summary>
        public bool OptionExpert { get; set; }

        /// <summary>
        /// Gets or sets the filters enabled flag.
        /// </summary>
        public bool OptionFilters { get; set; }

        /// <summary>
        /// Gets or sets the graphs enabled flag.
        /// </summary>
        public bool OptionGraphs { get; set; }

        /// <summary>
        /// Gets or sets the hidden flag.
        /// </summary>
        public bool OptionHidden { get; set; }

        /// <summary>
        /// Gets or sets the packet buffer enabled flag.
        /// </summary>
        public bool OptionPacketBuffer { get; set; }

        /// <summary>
        /// Gets or sets the plug-ins enabled flag.
        /// </summary>
        public bool OptionPlugins { get; set; }

        /// <summary>
        /// Gets or sets the Timeline statistics enabled flag.
        /// </summary>
        public bool OptionTimelineStats { get; set; }

        /// <summary>
        /// Gets or sets the voice enabled flag.
        /// </summary>
        public bool OptionVoice { get; set; }

        /// <summary>
        /// Gets or sets the web enabled flag.
        /// </summary>
        public bool OptionWeb { get; set; }

        public Capture(OmniEngine engine, XElement node=null)
        {
            if (engine != null)
            {
                this.Engine = engine;
                this.Connection = engine.Connection;
                this.Logger = engine.Logger;
                this.Context = null;
            }
            this.Load(node);
        }

        public void Copy(Capture other)
        {
            this.Context = null;
            this.Id = other.Id;
            this.Adapter = other.Adapter;
            this.AdapterType = other.AdapterType;
            this.AlarmsInfo = other.AlarmsInfo;
            this.AlarmsMajor = other.AlarmsMajor;
            this.AlarmsMinor = other.AlarmsMinor;
            this.AlarmsSevere = other.AlarmsSevere;
            this.AnalysisDroppedPackets = other.AnalysisDroppedPackets;
            this.BufferSize = other.BufferSize;
            this.BufferAvailable = other.BufferAvailable;
            this.BufferUsed = other.BufferUsed;
            this.Comment = other.Comment;
            this.Creator = other.Creator;
            this.CreatorSId = other.CreatorSId;
            this.DuplicatePacketsDiscarded = other.DuplicatePacketsDiscarded;
            this.Duration = other.Duration;
            this.FilterMode = other.FilterMode;
            this.FirstPacket = other.FirstPacket;
            this.LastModification = other.LastModification;
            this.LastModifiedBy = other.LastModifiedBy;
            this.LinkSpeed = other.LinkSpeed;
            this.LoggedOnUserSId = other.LoggedOnUserSId;
            this.MediaSubType = other.MediaSubType;
            this.MediaType = other.MediaType;
            this.Name = other.Name;
            this.PacketCount = other.PacketCount;
            this.PacketsAnalyzed = other.PacketsAnalyzed;
            this.PacketsDropped = other.PacketsDropped;
            this.PacketsFiltered = other.PacketsFiltered;
            this.PacketsReceived = other.PacketsReceived;
            this.ResetCount = other.ResetCount;
            this.StartTime = other.StartTime;
            this.Status = other.Status;
            this.StopTime = other.StopTime;
            this.TriggerCount = other.TriggerCount;
            this.TriggerDuration = other.TriggerDuration;
            this.OptionAlarms = other.OptionAlarms;
            this.OptionExpert = other.OptionExpert;
            this.OptionFilters = other.OptionFilters;
            this.OptionGraphs = other.OptionGraphs;
            this.OptionHidden = other.OptionHidden;
            this.OptionPacketBuffer = other.OptionPacketBuffer;
            this.OptionPlugins = other.OptionPlugins;
            this.OptionTimelineStats = other.OptionTimelineStats;
            this.OptionVoice = other.OptionVoice;
            this.OptionWeb = other.OptionWeb;
        }

        public String FormatStatus()
        {
            return Capture.statusDictionay[this.Status];
        }

        public CallStatistics GetCallStats()
        {
            if (this.Context == null)
            {
                this.GetStatsContext();
            }
            if (this.Context != null)
            {
                return this.Context.GetCallStatistics();
            }
            return null;
        }

        public CaptureTemplate GetCaptureTemplate()
        {
            return this.Engine.GetCaptureTemplate(this.Id);
        }

        public EventLog GetEventLog(uint first, uint count, String search=null)
        {
            return this.Engine.GetEventLog(first, count, this.Id, search);
        }

        public Dictionary<String, String> GetFlowStats()
        {
            return null;
            //if (this.context == null)
            //{
            //    this.connection.GetStatsContext(this.Id, ref this.context);
            //}
            //String stats = null;
            //this.context.GetFlow or GetConversation(ref stats);
            //return SummaryXmlToDictionary(stats);
        }

        public NodeStatistics GetNodeStats()
        {
            if (this.Context == null)
            {
                this.GetStatsContext();
            }

            if (this.Context == null) return null;

            return this.Context.GetNodeStatistics();
        }

        public ProtocolStatistics GetProtocolStats()
        {
            if (this.Context == null)
            {
                this.GetStatsContext();
            }

            if (this.Context == null) return null;

            return this.Context.GetProtocolStatistics();
        }

        private XElement GetProperties()
        {
            String request = "<request><prop name=\"id\" type=\"8\">" + this.Id + "</prop></request>";
            String response = this.Connection.XmlCommand(OmniApi.OMNI_GET_CAPTURE_PROPS, request);
            return OmniApi.ParseResponse(response, "captureproperties");
        }

        public SummaryStatistics GetSummaryStats()
        {
            if (this.Context == null)
            {
                this.GetStatsContext();
            }

            if (this.Context == null) return null;

            return this.Context.GetSumaryStatistics();
        }

        /// <summary>
        /// Get a capture's Stats Context.
        /// </summary>
        private void GetStatsContext()
        {
            byte[] request = new byte[24];
            this.Id.ToByteArray().CopyTo(request, 0);
            byte[] response = this.Connection.Command(OmniApi.OMNI_GET_STATS, request);
            //File.WriteAllBytes("C:\\Temp\\stats_cs.bin", response);
            this.Context = new StatsContext(response);
        }


        public bool IsCapturing()
        {
            return ((this.Status & 1) != 0);
        }

        public void Load(XElement node)
        {
            if ((node == null) || !((node.Name == "capture") || (node.Name == "captureproperties"))) return;

            IEnumerable<XElement> props = node.Elements();
            foreach (XElement prop in props)
            {
                XAttribute name = prop.Attribute("name");
                if (name == null) continue;

                // XAttribute type = prop.Attribute("type");

                switch (name.Value)
                {
                    case "Adapter":
                        this.AdapterName = prop.Value;
                        break;

                    case "AdapterInfo":
                        {
                            // There is only one element, but First and FirstOrDefault don't compile.
                            foreach (XElement element in prop.Elements())
                            {
                                this.Adapter = AdapterList.AdapterFactory(element, this.Engine);
                                break;
                            }
                        }
                        break;

                    case "Adapter Type":
                        this.AdapterType = Convert.ToUInt32(prop.Value);
                        break;

                    case "Alarms Enabled":
                        this.OptionAlarms = (prop.Value != "0");
                        break;

                    case "Alarms-Info":
                        this.AlarmsSevere = Convert.ToUInt32(prop.Value);
                        break;

                    case "Alarms-Major":
                        this.AlarmsMajor = Convert.ToUInt32(prop.Value);
                        break;

                    case "Alarms-Minor":
                        this.AlarmsMinor = Convert.ToUInt32(prop.Value);
                        break;

                    case "Alarms-Severe":
                        this.AlarmsSevere = Convert.ToUInt32(prop.Value);
                        break;

                    case "Analysis Dropped Packets":
                        this.AnalysisDroppedPackets = Convert.ToUInt64(prop.Value);
                        break;

                    case "Buffer Capacity":
                        this.BufferSize = Convert.ToUInt64(prop.Value);
                        break;

                    case "Capacity Available":
                        this.BufferAvailable = Convert.ToUInt64(prop.Value);
                        break;

                    case "Capacity Used":
                        this.BufferUsed = Convert.ToUInt64(prop.Value);
                        break;

                    case "Comment":
                        this.Comment = prop.Value;
                        break;

                    case "Creator":
                        this.Creator = prop.Value;
                        break;

                    case "Creator-SID":
                        this.CreatorSId = prop.Value;
                        break;

                    case "Duplicate Packets Discarded":
                        this.DuplicatePacketsDiscarded = Convert.ToUInt64(prop.Value);
                        break;

                    case "Duration":
                        this.Duration = Convert.ToUInt64(prop.Value);
                        break;

                    case "Expert Enabled":
                        this.OptionExpert = (prop.Value != "0");
                        break;

                    case "Filter Mode":
                        this.FilterMode = Convert.ToUInt32(prop.Value);
                        break;

                    case "Filters Enabled":
                        this.OptionFilters = (prop.Value != "0");
                        break;

                    case "First Packet":
                        this.FirstPacket = Convert.ToUInt64(prop.Value);
                        break;

                    case "Graphs-Enabled":
                        this.OptionGraphs = (prop.Value != "0");
                        break;

                    case "Hidden":
                        this.OptionHidden = (prop.Value != "0");
                        break;

                    case "ID":
                        this.Id = Guid.Parse(prop.Value);
                        break;

                    case "Link Speed":
                        this.LinkSpeed = Convert.ToUInt64(prop.Value);
                        break;

                    case "LoggedOnUser-Sid":
                        this.LoggedOnUserSId = prop.Value;
                        break;

                    case "MediaInfo":
                        // TODO: Implement Class MediaInfo.
                        this.MediaInfo = new Object();
                        break;

                    case "Media Subtype":
                        this.MediaSubType = Convert.ToUInt32(prop.Value);
                        break;

                    case "Media Type":
                        this.MediaType = Convert.ToUInt32(prop.Value);
                        break;

                    case "Mod By":
                        this.LastModifiedBy = prop.Value;
                        break;

                    case "Mod Type":
                        this.LastModification = prop.Value;
                        break;

                    case "Name":
                        this.Name = prop.Value;
                        break;

                    case "Packet Buffer Enabled":
                        this.OptionPacketBuffer = (prop.Value != "0");
                        break;

                    case "Packet Count":
                        this.PacketCount = Convert.ToUInt64(prop.Value);
                        break;

                    case "Packets Analyzed":
                        this.PacketsAnalyzed = Convert.ToUInt64(prop.Value);
                        break;

                    case "Packets Dropped":
                        this.PacketsDropped = Convert.ToUInt64(prop.Value);
                        break;

                    case "Packets Filtered":
                        this.PacketsFiltered = Convert.ToUInt64(prop.Value);
                        break;

                    case "Packets Received":
                        this.PacketsReceived = Convert.ToUInt64(prop.Value);
                        break;

                    case "Plugins Enabled":
                        this.OptionPlugins = (prop.Value != "0");
                        break;

                    case "Reset Count":
                        this.ResetCount = Convert.ToUInt32(prop.Value);
                        break;

                    case "Start Time":
                        this.StartTime = new PeekTime(Convert.ToUInt64(prop.Value));
                        break;

                    case "Status":
                        this.Status = Convert.ToUInt32(prop.Value);
                        break;

                    case "Stop Time":
                        this.StopTime = new PeekTime(Convert.ToUInt64(prop.Value));
                        break;

                    case "Timeline Stats Enabled":
                        this.OptionTimelineStats = (prop.Value != "0");
                        break;

                    case "Trigger Count":
                        this.TriggerCount = Convert.ToUInt32(prop.Value);
                        break;

                    case "Trigger Duration":
                        this.TriggerDuration = Convert.ToUInt64(prop.Value);
                        break;

                    case "Voice Enabled":
                        this.OptionVoice = (prop.Value != "0");
                        break;

                    case "Web Enabled":
                        this.OptionWeb = (prop.Value != "0");
                        break;
                }
            }
        }

        public bool Modify(CaptureTemplate template)
        {
            Capture capture = this.Engine.ModifyCapture(template, this);
            if ((capture == null) || (capture.Name != this.Name) || (capture.Id != this.Id))
            {
                return false;
            }

            this.Copy(capture);
            this.Context = null;    // Will refresh on next Stats call.
            return true;
        }

        public bool Refresh()
        {
            XElement props = this.GetProperties();
            if (props != null)
            {
                this.Load(props);
                this.Context = null;    // Will refresh on next Stats call.
                return true;
            }
            Capture capture = this.Engine.FindCapture(this.Name);
            if ((capture == null) || (capture.Name != this.Name) || (capture.Id != this.Id))
            {
                return false;
            }

            this.Copy(capture);
            this.Context = null;    // Will refresh on next Stats call.
            return true;
        }

        public void Reset()
        {
            this.Engine.ResetCapture(this.Id.ToString());
        }

        public void SaveAllPackets(String filename)
        {
            this.Engine.SaveAllPackets(this.Id, filename);
        }

        public void Start()
        {
            this.Engine.StartCapture(this.Id.ToString());
        }

        public void Stop()
        {
            this.Engine.StopCapture(this.Id.ToString());
        }

#if EXPOSE_UNUSED
        public SummaryStats SummaryXmlToDictionary(String xml)
        {
            SummaryStats summary = new SummaryStats();

            XDocument doc = XDocument.Parse(xml);
            XElement snapshot = doc.Root.Element("summarystats").Element("snapshot");
            IEnumerable<XElement> stats = snapshot.Elements("stat");
            foreach (XElement stat in stats)
            {
                String parent = stat.Attribute("parent").Value;
                Dictionary<String, String> group = summary.GetKey(parent);

                String id = stat.Attribute("id").Value;
                String str = stat.Attribute("type").Value;
                int type = Convert.ToInt32(str);
                String value = null;
                switch (type)
                {
                    case 0:     // none
                        break;

                    case 1:     // date
                        value = stat.Attribute("date").Value;
                        break;

                    case 2:     // time
                        value = stat.Attribute("time").Value;
                        break;

                    case 3:     // duration
                        value = stat.Attribute("duration").Value;
                        break;

                    case 4:     // packets
                        value = stat.Attribute("packets").Value;
                        break;

                    case 5:     // bytes
                        value = stat.Attribute("bytes").Value;
                        break;

                    case 6:     // packets, bytes
                        value = stat.Attribute("packets").Value + "," + stat.Attribute("bytes").Value;
                        break;

                    case 7:     // int
                        value = stat.Attribute("int").Value;
                        break;

                    case 8:     // double
                        value = stat.Attribute("double").Value;
                        break;
                }
                group.Add(id, value);
            }
            return summary;
        }

        public StatList XmlToStatList(String statname, String xml)
        {
            StatList result = new StatList();

            XDocument doc = XDocument.Parse(xml);
            XElement stattype = doc.Root.Element(statname);
            IEnumerable<XElement> stats = stattype.Elements("stat");
            foreach (XElement statElement in stats)
            {
                Dictionary<String, String> stat = new Dictionary<String, String>();

                IEnumerable<XElement> props = statElement.Elements("prop");
                foreach (XElement propElement in props)
                {
                    String name = propElement.Attribute("name").Value;
                    String str = propElement.Attribute("type").Value;
                    int type = Convert.ToInt32(str);
                    String value = propElement.Value;
                    stat[name] = value;
                }
                result.Add(stat);
            }
            return result;
        }
#endif
    }
}
