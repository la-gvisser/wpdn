// =============================================================================
// <copyright file="OmniPrinters.cs" company="Savvius, Inc.">
//  Copyright (c) 2015-2018 Savvius, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace OmniPrinters
{
    using System;
    using Savvius.Omni.OmniScript;

    public class OmniPrinters
    {
        private static String EnabledString(bool value)
        {
            return (value) ? "Enabled" : "Disabled";
        }

        public static OmniLogger log = new OmniLogger();

        public static void PrintAdapter(Adapter adapter, int tab=0)
        {
            OmniPrinters.log.WriteLine("Adapter");
            if (adapter == null)
            {
                OmniPrinters.log.WriteLine("  No adapter");
                return;
            }
            String fmtP = "{0," + (18+tab) + "}: {1}";

            OmniPrinters.log.WriteLine(fmtP, "Name", adapter.Name);
            OmniPrinters.log.WriteLine(fmtP, "Id", adapter.Id);
            OmniPrinters.log.WriteLine(fmtP, "Type", adapter.AdapterType);
            OmniPrinters.log.WriteLine(fmtP, "Address", adapter.Address);
            OmniPrinters.log.WriteLine(fmtP, "Description", adapter.Description);
            OmniPrinters.log.WriteLine(fmtP, "Link Speed", adapter.LinkSpeed);
            OmniPrinters.log.WriteLine(fmtP, "Media Type", adapter.MediaType);
            OmniPrinters.log.WriteLine(fmtP, "Media SubType", adapter.MediaSubType);

            if (adapter is NDISAdapter)
            {
                NDISAdapter ndis = (NDISAdapter)adapter;
                OmniPrinters.log.WriteLine(fmtP, "", "NDIS Adapter");
                OmniPrinters.log.WriteLine(fmtP, "Features", ndis.Features);
                OmniPrinters.log.WriteLine(fmtP, "Device Name", ndis.DeviceName);
                OmniPrinters.log.WriteLine(fmtP, "Interface Features", ndis.InterfaceFeatures);
                OmniPrinters.log.WriteLine(fmtP, "OmniPeek API", ndis.OmniPeekAPI);
            }
            else if (adapter is PluginAdapter)
            {
                PluginAdapter plugin = (PluginAdapter)adapter;
                OmniPrinters.log.WriteLine(fmtP, "", "Plugin Adapter");
                OmniPrinters.log.WriteLine(fmtP, "Plugin", plugin.Plugin);

            }
            else if (adapter is FileAdapter)
            {
                FileAdapter file = (FileAdapter)adapter;
                OmniPrinters.log.WriteLine(fmtP, "", "File Adapter");
                OmniPrinters.log.WriteLine(fmtP, "Limit", file.Limit);
                OmniPrinters.log.WriteLine(fmtP, "Mode", file.Mode);
                OmniPrinters.log.WriteLine(fmtP, "Speed", file.Speed);
            }
            else if (adapter is PcapAdapter)
            {
                PcapAdapter pcap = (PcapAdapter)adapter;
                OmniPrinters.log.WriteLine(fmtP, "", "Pcap Adapter");
                OmniPrinters.log.WriteLine(fmtP, "Features", pcap.Features);
                OmniPrinters.log.WriteLine(fmtP, "Device Name", pcap.DeviceName);
                OmniPrinters.log.WriteLine(fmtP, "Interface Features", pcap.InterfaceFeatures);
                OmniPrinters.log.WriteLine(fmtP, "OmniPeek API", pcap.OmniPeekAPI);
            }
            OmniPrinters.log.WriteLine("");
        }

        public static void PrintAdapterList(AdapterList adapters, int tab=0)
        {
            foreach(Adapter adapter in adapters)
            {
                PrintAdapter(adapter, tab);
                OmniPrinters.log.WriteLine("");
            }
            OmniPrinters.log.WriteLine("");
        }

        public static void PrintAlarm(Alarm alarm, int tab=0)
        {
            OmniPrinters.log.WriteLine("Alarm");
            if (alarm == null)
            {
                OmniPrinters.log.WriteLine("  No alarm");
                return;
            }
            String fmtP = "{0," + (10+tab) + "}: {1}";
            OmniPrinters.log.WriteLine(fmtP, "Name", alarm.Name);
            OmniPrinters.log.WriteLine(fmtP, "Id", alarm.Id);
            OmniPrinters.log.WriteLine(fmtP, "Created", alarm.Created.ToDateTime());
            OmniPrinters.log.WriteLine(fmtP, "Modified", alarm.Modified.ToDateTime());
            OmniPrinters.log.WriteLine(fmtP, "Track Type", alarm.TrackType);
        }

        public static void PrintAlarmList(AlarmList alarms, int tab=0)
        {
            foreach (Alarm alarm in alarms)
            {
                PrintAlarm(alarm, tab);
                OmniPrinters.log.WriteLine("");
            }
            OmniPrinters.log.WriteLine("");
        }

        public static void PrintAnalysisModule(AnalysisModule plugin, int tab=0)
        {
            OmniPrinters.log.WriteLine("Analysis Module");
            if (plugin == null)
            {
                OmniPrinters.log.WriteLine("  No analysis module");
                return;
            }
            String fmtP = "{0," + (4+tab) + "}: {1}";
            OmniPrinters.log.WriteLine(fmtP, "Id", plugin.Id);
            OmniPrinters.log.WriteLine(fmtP, "Name", plugin.Name);
        }

        public static void PrintAnalysisModuleList(AnalysisModuleList plugins, int tab=0)
        {
            foreach (AnalysisModule plugin in plugins)
            {
                PrintAnalysisModule(plugin, tab);
                OmniPrinters.log.WriteLine("");
            }
            OmniPrinters.log.WriteLine("");
        }

        public static void PrintCapture(Capture capture, int tab=0)
        {
            OmniPrinters.log.WriteLine("Capture");
            if (capture == null)
            {
                OmniPrinters.log.WriteLine("  No capture");
                return;
            }

            String fmtP = "{0," + (27+tab) + "}: {1}";

            OmniPrinters.log.WriteLine(fmtP, "Id", capture.Id);
            OmniPrinters.log.WriteLine(fmtP, "Name", capture.Name);
            OmniPrinters.log.WriteLine(fmtP, "Status", capture.FormatStatus());
            OmniPrinters.log.WriteLine(fmtP, "Comment", capture.Comment);
            OmniPrinters.log.WriteLine(fmtP, "Creator", capture.Creator);
            OmniPrinters.log.WriteLine(fmtP, "Creator-SID", capture.CreatorSId);
            OmniPrinters.log.WriteLine(fmtP, "LoggedOnUser-Sid", capture.LoggedOnUserSId);
            OmniPrinters.log.WriteLine(fmtP, "Last Modified By", capture.LastModifiedBy);
            OmniPrinters.log.WriteLine(fmtP, "Last Modification", capture.LastModification);
            OmniPrinters.log.WriteLine(fmtP, "Adapter", capture.Adapter);
            OmniPrinters.log.WriteLine(fmtP, "Adapter Type", capture.AdapterType);
            OmniPrinters.log.WriteLine(fmtP, "Link Speed", capture.LinkSpeed);
            OmniPrinters.log.WriteLine(fmtP, "Media Type", capture.MediaType);
            OmniPrinters.log.WriteLine(fmtP, "Media Subtype", capture.MediaSubType);
            OmniPrinters.log.WriteLine(fmtP, "Buffer Size", capture.BufferSize);
            OmniPrinters.log.WriteLine(fmtP, "Buffer Available", capture.BufferAvailable);
            OmniPrinters.log.WriteLine(fmtP, "Buffer Used", capture.BufferUsed);
            OmniPrinters.log.WriteLine(fmtP, "Filter Mode", capture.FilterMode);
            OmniPrinters.log.WriteLine(fmtP, "Plugins Enabled", capture.OptionPlugins);
            OmniPrinters.log.WriteLine(fmtP, "Start Time", capture.StartTime.ToDateTime());
            OmniPrinters.log.WriteLine(fmtP, "Stop Time", capture.StopTime.ToDateTime());
            OmniPrinters.log.WriteLine(fmtP, "Duration", capture.Duration);
            OmniPrinters.log.WriteLine(fmtP, "Reset Count", capture.ResetCount);
            OmniPrinters.log.WriteLine(fmtP, "Packets Received", capture.PacketsReceived);
            OmniPrinters.log.WriteLine(fmtP, "Packets Filtered", capture.PacketsFiltered);
            OmniPrinters.log.WriteLine(fmtP, "Packet Count", capture.PacketCount);
            OmniPrinters.log.WriteLine(fmtP, "Analysis Dropped Packets", capture.AnalysisDroppedPackets);
            OmniPrinters.log.WriteLine(fmtP, "Duplicate Packets Discarded", capture.DuplicatePacketsDiscarded);
            OmniPrinters.log.WriteLine(fmtP, "Packets Analyzed", capture.PacketsAnalyzed);
            OmniPrinters.log.WriteLine(fmtP, "Packets Dropped", capture.PacketsDropped);
            OmniPrinters.log.WriteLine(fmtP, "First Packet", capture.FirstPacket);
            OmniPrinters.log.WriteLine(fmtP, "Alarms-Info", capture.AlarmsInfo);
            OmniPrinters.log.WriteLine(fmtP, "Alarms-Minor", capture.AlarmsMinor);
            OmniPrinters.log.WriteLine(fmtP, "Alarms-Major", capture.AlarmsMajor);
            OmniPrinters.log.WriteLine(fmtP, "Alarms-Severe", capture.AlarmsSevere);
            OmniPrinters.log.WriteLine(fmtP, "Trigger Count", capture.TriggerCount);
            OmniPrinters.log.WriteLine(fmtP, "Trigger Duration", capture.TriggerDuration);
            OmniPrinters.log.WriteLine(fmtP, "Alarms Enabled", capture.OptionAlarms);
            OmniPrinters.log.WriteLine(fmtP, "Expert Enabled", capture.OptionExpert);
            OmniPrinters.log.WriteLine(fmtP, "Filters Enabled", capture.OptionFilters);
            OmniPrinters.log.WriteLine(fmtP, "Graphs Enabled", capture.OptionGraphs);
            OmniPrinters.log.WriteLine(fmtP, "Hidden", capture.OptionHidden);
            OmniPrinters.log.WriteLine(fmtP, "Packet Buffer Enabled", capture.OptionPacketBuffer);
            OmniPrinters.log.WriteLine(fmtP, "Timeline Stats Enabled", capture.OptionTimelineStats);
            OmniPrinters.log.WriteLine(fmtP, "Voice Enabled", capture.OptionVoice);
            OmniPrinters.log.WriteLine(fmtP, "Web Enabled", capture.OptionWeb);
        }

        public static void PrintCaptureList(CaptureList captures, int tab=0)
        {
            foreach (Capture capture in captures)
            {
                PrintCapture(capture, tab);
                OmniPrinters.log.WriteLine("");
            }
            OmniPrinters.log.WriteLine("");
        }

        public static void PrintCaptureTemplate(CaptureTemplate template, int tab=0)
        {
            OmniPrinters.log.WriteLine("Capture Template");
            if (template == null)
            {
                OmniPrinters.log.WriteLine("  No capture template");
                return;
            }

            String fmtP = "{0," + (23+tab) + "}: {1}";
            String fmtId = "{0," + (23+tab+4) + "}: {1}";

            OmniPrinters.log.WriteLine(fmtP, "Name", template.Name);
            OmniPrinters.log.WriteLine(fmtP, "Id", template.Id);
            OmniPrinters.log.WriteLine(fmtP, "AdapterName", template.AdapterName);

            OmniPrinters.log.WriteLine("Adapter:");
            OmniPrinters.log.WriteLine(fmtP, "Name", template.Adapter.Name);

            OmniPrinters.log.WriteLine("Alarms:");
            foreach(OmniId id in template.Alarms.Alarms)
            {
                OmniPrinters.log.WriteLine(fmtId, "Id", id);
            }

            OmniPrinters.log.WriteLine("Analysis:");
            if (template.Analysis.NodeLimit != null)
            {
                PrintStatsLimit(template.Analysis.NodeLimit, (tab + 4));
            }
            if (template.Analysis.NodeProtocolDetailLimit != null)
            {
                PrintStatsLimit(template.Analysis.NodeProtocolDetailLimit, (tab + 4));
            }
            if (template.Analysis.ProtocolLimit != null)
            {
                PrintStatsLimit(template.Analysis.ProtocolLimit, (tab + 4));
            }
            OmniPrinters.log.WriteLine(fmtP, "OptionAlarms", template.Analysis.OptionAlarms);
            OmniPrinters.log.WriteLine(fmtP, "OptionAnalysisModules", template.Analysis.OptionAnalysisModules);
            OmniPrinters.log.WriteLine(fmtP, "OptionApplication", template.Analysis.OptionApplication);
            OmniPrinters.log.WriteLine(fmtP, "OptionError", template.Analysis.OptionError);
            OmniPrinters.log.WriteLine(fmtP, "OptionExpert", template.Analysis.OptionExpert);
            OmniPrinters.log.WriteLine(fmtP, "OptionNetwork", template.Analysis.OptionNetwork);
            OmniPrinters.log.WriteLine(fmtP, "OptionSize", template.Analysis.OptionSize);
            OmniPrinters.log.WriteLine(fmtP, "OptionSummary", template.Analysis.OptionSummary);
            OmniPrinters.log.WriteLine(fmtP, "OptionTraffic", template.Analysis.OptionTraffic);
            OmniPrinters.log.WriteLine(fmtP, "OptionVoiceVideo", template.Analysis.OptionVoiceVideo);
            OmniPrinters.log.WriteLine(fmtP, "OptionWirelessChannel", template.Analysis.OptionWirelessChannel);
            OmniPrinters.log.WriteLine(fmtP, "OptionWirelessNode", template.Analysis.OptionWirelessNode);

            OmniPrinters.log.WriteLine("Filters");
            OmniPrinters.log.WriteLine(fmtP, "Mode", template.Filter.Mode);
            foreach (String name in template.Filter.Filters)
            {
                OmniPrinters.log.WriteLine(fmtP, "Name:", name);
            }

            OmniPrinters.log.WriteLine("General");
            OmniPrinters.log.WriteLine(fmtP, "Buffer Size", template.General.BufferSize);
            OmniPrinters.log.WriteLine(fmtP, "Comment", template.General.Comment);
            OmniPrinters.log.WriteLine(fmtP, "Directory", template.General.Directory);
            OmniPrinters.log.WriteLine(fmtP, "FilePattern", template.General.FilePattern);
            OmniPrinters.log.WriteLine(fmtP, "FileSize", template.General.FileSize);
            OmniPrinters.log.WriteLine(fmtP, "GroupId", template.General.GroupId);
            OmniPrinters.log.WriteLine(fmtP, "GroupName", template.General.GroupName);
            OmniPrinters.log.WriteLine(fmtP, "KeepLastFilesCount", template.General.KeepLastFilesCount);
            OmniPrinters.log.WriteLine(fmtP, "MaxFileAge", template.General.MaxFileAge);
            OmniPrinters.log.WriteLine(fmtP, "MaxTotalFileSize", template.General.MaxTotalFileSize);
            OmniPrinters.log.WriteLine(fmtP, "Owner", template.General.Owner);
            OmniPrinters.log.WriteLine(fmtP, "ReservedSize", template.General.ReservedSize);
            OmniPrinters.log.WriteLine(fmtP, "SliceLength", template.General.SliceLength);
            OmniPrinters.log.WriteLine(fmtP, "TapTimestamps", template.General.TapTimestamps);
            OmniPrinters.log.WriteLine(fmtP, "OptionCaptureToDisk", template.General.OptionCaptureToDisk);
            OmniPrinters.log.WriteLine(fmtP, "OptionContinuousCapture", template.General.OptionContinuousCapture);
            OmniPrinters.log.WriteLine(fmtP, "OptionDeduplicate", template.General.OptionDeduplicate);
            OmniPrinters.log.WriteLine(fmtP, "OptionFileAge", template.General.OptionFileAge);
            OmniPrinters.log.WriteLine(fmtP, "OptionKeepLastFiles", template.General.OptionKeepLastFiles);
            OmniPrinters.log.WriteLine(fmtP, "OptionPriorityCtd", template.General.OptionPriorityCtd);
            OmniPrinters.log.WriteLine(fmtP, "OptionSaveAsTemplate", template.General.OptionSaveAsTemplate);
            OmniPrinters.log.WriteLine(fmtP, "OptionSlicing", template.General.OptionSlicing);
            OmniPrinters.log.WriteLine(fmtP, "OptionStartCapture", template.General.OptionStartCapture);
            OmniPrinters.log.WriteLine(fmtP, "OptionTimelineAppStats", template.General.OptionTimelineAppStats);
            OmniPrinters.log.WriteLine(fmtP, "OptionTimelineStats", template.General.OptionTimelineStats);
            OmniPrinters.log.WriteLine(fmtP, "OptionTimelineTopStats", template.General.OptionTimelineTopStats);
            OmniPrinters.log.WriteLine(fmtP, "OptionTimelineVoipStats", template.General.OptionTimelineVoipStats);
            OmniPrinters.log.WriteLine(fmtP, "OptionTotalFileSize", template.General.OptionTotalFileSize);

            OmniPrinters.log.WriteLine("Graphs");
            OmniPrinters.log.WriteLine(fmtP, "Enabled:", EnabledString(template.Graphs.Enabled));
            OmniPrinters.log.WriteLine(fmtP, "Interval", template.Graphs.Interval);
            OmniPrinters.log.WriteLine(fmtP, "FileCount", template.Graphs.FileCount);
            OmniPrinters.log.WriteLine(fmtP, "OptionPreserveFiles", template.Graphs.OptionPreserveFiles);
            OmniPrinters.log.WriteLine("Graphs:");
            foreach (OmniId id in template.Graphs.Graphs)
            {
                OmniPrinters.log.WriteLine(fmtId, "Id", id);
            }

            OmniPrinters.log.WriteLine("Plugins:");
            foreach (PluginSettings plugin in template.Plugins.PluginList)
            {
                OmniPrinters.log.WriteLine(fmtP, "Name", plugin.Name);
                if (!String.IsNullOrEmpty(plugin.Settings))
                {
                    String settings = plugin.Settings.Replace("\r\n", "\\n");
                    OmniPrinters.log.WriteLine(fmtP, "Settings", settings);
                }
            }
            OmniPrinters.log.WriteLine("PluginsConfigSettings:");

            OmniPrinters.log.WriteLine("RepeatTrigger");
            OmniPrinters.log.WriteLine(fmtP, "Value", template.RepeatTrigger.Value);

            OmniPrinters.log.WriteLine("StatisticsOutput:       " + EnabledString(template.StatisticsOutput.Enabled));
            
            OmniPrinters.log.WriteLine("StatisticsOutputPreferences");
            OmniPrinters.log.WriteLine(fmtP, "ReportType", template.StatisticsOutputPreferences.ReportType);

            OmniPrinters.log.WriteLine("Start Trigger:          " + EnabledString(template.StartTrigger.Enabled));
            OmniPrinters.log.WriteLine(fmtP, "Enabled", EnabledString(template.StartTrigger.Enabled));
            OmniPrinters.log.WriteLine(fmtP, "Severity", template.StartTrigger.Severity);
            OmniPrinters.log.WriteLine(fmtP, "Date Limit", EnabledString(template.StartTrigger.Time.Enabled));
            OmniPrinters.log.WriteLine(fmtP, "Filter Limit", EnabledString(template.StartTrigger.Filter.Enabled));
            OmniPrinters.log.WriteLine(fmtP, "Capture Limit", EnabledString(template.StartTrigger.Captured.Enabled));
            OmniPrinters.log.WriteLine(fmtP, "OptionNotify", template.StartTrigger.OptionNotify);
            OmniPrinters.log.WriteLine(fmtP, "OptionToggle Capture", template.StartTrigger.OptionToggleCapture);

            OmniPrinters.log.WriteLine("Stop Trigger:           " + EnabledString(template.StopTrigger.Enabled));
            OmniPrinters.log.WriteLine(fmtP, "Enabled", EnabledString(template.StopTrigger.Enabled));
            OmniPrinters.log.WriteLine(fmtP, "Severity", template.StopTrigger.Severity);
            OmniPrinters.log.WriteLine(fmtP, "Date Limit", EnabledString(template.StopTrigger.Time.Enabled));
            OmniPrinters.log.WriteLine(fmtP, "Filter Limit", EnabledString(template.StopTrigger.Filter.Enabled));
            OmniPrinters.log.WriteLine(fmtP, "Capture Limit", EnabledString(template.StopTrigger.Captured.Enabled));
            OmniPrinters.log.WriteLine(fmtP, "OptionNotify", template.StopTrigger.OptionNotify);
            OmniPrinters.log.WriteLine(fmtP, "OptionToggle Capture", template.StopTrigger.OptionToggleCapture);

            OmniPrinters.log.WriteLine("VoIP:");
            OmniPrinters.log.WriteLine(fmtP, "MaxCalls", template.Voip.MaxCalls);
            OmniPrinters.log.WriteLine(fmtP, "Severity", template.Voip.Severity);
            OmniPrinters.log.WriteLine(fmtP, "OptionNotify", template.Voip.OptionNotify);
            OmniPrinters.log.WriteLine(fmtP, "OptionStopAnalysis", template.Voip.OptionStopAnalysis);
        }

        public static void PrintCaptureTemplateList(CaptureTemplateList templates, int tab=0)
        {
            foreach (CaptureTemplate template in templates)
            {
                PrintCaptureTemplate(template, tab);
                OmniPrinters.log.WriteLine("");
            }
            OmniPrinters.log.WriteLine("");
        }

        public static void PrintEngineStatus(EngineStatus status, int tab=0)
        {
            OmniPrinters.log.WriteLine("Engine Status");
            if (status == null)
            {
                OmniPrinters.log.WriteLine("  No engine status");
                return;
            }
            String fmtP = "{0," + (25+tab) + "}: {1}";
            OmniPrinters.log.WriteLine(fmtP, "Name", status.Name);
            OmniPrinters.log.WriteLine(fmtP, "Adapter Count", status.AdapterCount);
            OmniPrinters.log.WriteLine(fmtP, "Alarm Count", status.AlarmCount);
            OmniPrinters.log.WriteLine(fmtP, "Capture Count", status.CaptureCount);
            OmniPrinters.log.WriteLine(fmtP, "CPU Count", status.CPUCount);
            OmniPrinters.log.WriteLine(fmtP, "CPU Type", status.CPUType);
            OmniPrinters.log.WriteLine(fmtP, "Data Folder", status.DataFolder);
            OmniPrinters.log.WriteLine(fmtP, "Engine Type", status.EngineType);
            OmniPrinters.log.WriteLine(fmtP, "File Count", status.FileCount);
            OmniPrinters.log.WriteLine(fmtP, "File Version", status.FileVersion);
            OmniPrinters.log.WriteLine(fmtP, "Filter Count", status.FilterCount);
            OmniPrinters.log.WriteLine(fmtP, "Filter Modification Time", status.FilterModificationTime.ToDateTime());
            OmniPrinters.log.WriteLine(fmtP, "Forensic Search Count", status.ForensicSearchCount);
            OmniPrinters.log.WriteLine(fmtP, "Graph Count", status.GraphCount);
            OmniPrinters.log.WriteLine(fmtP, "Host", status.Host);
            OmniPrinters.log.WriteLine(fmtP, "Log Total Count", status.LogTotalCount);
            OmniPrinters.log.WriteLine(fmtP, "Memory Available Physical", status.MemoryAvailablePhysical);
            OmniPrinters.log.WriteLine(fmtP, "Memory Total Physical", status.MemoryTotalPhysical);
            OmniPrinters.log.WriteLine(fmtP, "Name Table Count", status.NameTableCount);
            OmniPrinters.log.WriteLine(fmtP, "Notification Count", status.NotificationCount);
            OmniPrinters.log.WriteLine(fmtP, "OS", status.OS);
            OmniPrinters.log.WriteLine(fmtP, "Platform", status.Platform);
            OmniPrinters.log.WriteLine(fmtP, "Port", status.Port);
            OmniPrinters.log.WriteLine(fmtP, "Product Version", status.ProductVersion);
            OmniPrinters.log.WriteLine(fmtP, "Storage Available", status.StorageAvailable);
            OmniPrinters.log.WriteLine(fmtP, "Storage Total", status.StorageTotal);
            OmniPrinters.log.WriteLine(fmtP, "Storage Used", status.StorageUsed);
            OmniPrinters.log.WriteLine(fmtP, "Time", status.Time.ToDateTime());
            OmniPrinters.log.WriteLine(fmtP, "Time Zone Bias", status.TimeZoneBias);
            OmniPrinters.log.WriteLine(fmtP, "Up Time", status.UpTime.ToDuration());
        }

        public static void PrintEventLog(EventLog events, int tab=0)
        {
            Console.WriteLine("Event Log");
            if (events == null)
            {
                Console.WriteLine("  No event log");
                return;
            }
            String fmtP = "{0," + (13+tab) + "}: {1}";
            if (events.ContextId != null)
            {
                Console.WriteLine(fmtP, "Context Id", events.ContextId.ToString());
            }
            if (events.SearchString != null)
            {
                Console.WriteLine(fmtP, "Search", events.SearchString);
            }
            Console.WriteLine(fmtP, "Total Count", events.Count);
            if (events.First != null)
            {
                Console.WriteLine(fmtP, "First", events.First.ToDateTime());
            }
            if (events.Last != null)
            {
                Console.WriteLine(fmtP, "Last", events.Last.ToDateTime());
            }
            Console.WriteLine(fmtP, "Informational", events.Informational);
            Console.WriteLine(fmtP, "Minor", events.Minor);
            Console.WriteLine(fmtP, "Major", events.Major);
            Console.WriteLine(fmtP, "Severe", events.Severe);
            if (events.EntryList != null)
            {
                Console.WriteLine(fmtP, "Entry Count", events.EntryList.Count);
                Console.WriteLine();
                PrintEventLogEntryList(events.EntryList, 4);
            }
        }

         public static void PrintEventLogEntry(EventLogEntry entry, int tab=0)
        {
            Console.WriteLine("Event Log Entry");
            if (entry == null)
            {
                Console.WriteLine("  No event log entry");
                return;
            }
            String fmtP = "{0," + (10+tab) + "}: {1}";
            Console.WriteLine(fmtP, "Index", entry.Index);
            if (entry.ContextId != null)
            {
                Console.WriteLine(fmtP, "Context Id", entry.ContextId.ToString());
            }
            if (entry.SourceId != null)
            {
                Console.WriteLine(fmtP, "Source Id", entry.SourceId.ToString());
            }
            Console.WriteLine(fmtP, "Source Key", entry.SourceKey);
            Console.WriteLine(fmtP, "Severity", entry.Severity);
            if (entry.Timestamp != null)
            {
                Console.WriteLine(fmtP, "Timestamp", entry.Timestamp.ToDateTime());
            }
            if (!String.IsNullOrEmpty(entry.Message))
            {
                Console.WriteLine(fmtP, "Message", entry.Message);
            }
        }

        public static void PrintEventLogEntryList(EventLogEntryList entrys, int tab=0)
        {
            foreach (EventLogEntry entry in entrys)
            {
                PrintEventLogEntry(entry, tab);
                Console.WriteLine();
            }
            Console.WriteLine();
        }

        public static void PrintFileInformation(FileInformation file, int tab=0)
        {
            OmniPrinters.log.WriteLine("File Information");
            if (file == null)
            {
                OmniPrinters.log.WriteLine("  No file information");
                return;
            }
            String fmtP = "{0," + (9+tab) + "}: {1}";
            OmniPrinters.log.WriteLine(fmtP, "Name", file.Name);
            OmniPrinters.log.WriteLine(fmtP, "Size", file.Size);
            OmniPrinters.log.WriteLine(fmtP, "Flags", file.Flags.ToString("X8"));
            OmniPrinters.log.WriteLine(fmtP, "Directory", (file.IsFolder()) ? "True" : "False");
            if (file.Modified != null)
            {
                OmniPrinters.log.WriteLine(fmtP, "Modified", file.Modified.ToDateTime());
            }
        }

        public static void PrintFileInformationList(FileInformationList files, int tab=0)
        {
            foreach (FileInformation file in files)
            {
                PrintFileInformation(file, tab);
                OmniPrinters.log.WriteLine("");
            }
            OmniPrinters.log.WriteLine("");
        }

        public static void PrintFilter(Filter filter, int tab=0)
        {
            if (filter == null)
            {
                OmniPrinters.log.WriteLine("  No filter");
                return;
            }
            OmniPrinters.log.WriteLine(filter.ToString(tab));
        }

        public static void PrintFilterList(FilterList filters, int tab=0)
        {
            foreach (Filter filter in filters)
            {
                PrintFilter(filter, tab);
                OmniPrinters.log.WriteLine("");
            }
            OmniPrinters.log.WriteLine("");
        }

        public static void PrintForensicFile(ForensicFile file, int tab=0)
        {
            OmniPrinters.log.WriteLine("Forensic File");
            if (file == null)
            {
                OmniPrinters.log.WriteLine("  No forensic file");
                return;
            }
            String fmtP = "{0," + (14+tab) + "}: {1}";
            OmniPrinters.log.WriteLine(fmtP, "Name", file.Name);
            OmniPrinters.log.WriteLine(fmtP, "Path", file.Path);
            OmniPrinters.log.WriteLine(fmtP, "Status", file.Status);
            OmniPrinters.log.WriteLine(fmtP, "Index", file.FileIndex);
            OmniPrinters.log.WriteLine(fmtP, "Session Id", file.SessionId);
            OmniPrinters.log.WriteLine(fmtP, "Size", file.Size);
            OmniPrinters.log.WriteLine(fmtP, "Media Type", file.MediaType);
            OmniPrinters.log.WriteLine(fmtP, "Media Sub Type", file.MediaSubType);
            OmniPrinters.log.WriteLine(fmtP, "Adapter", file.AdapterName);
            OmniPrinters.log.WriteLine(fmtP, "Link Speed", file.LinkSpeed);
            OmniPrinters.log.WriteLine(fmtP, "Capture", file.CaptureName);
            OmniPrinters.log.WriteLine(fmtP, "Capture Id", file.CaptureId);
            OmniPrinters.log.WriteLine(fmtP, "Count", file.PacketCount);
            OmniPrinters.log.WriteLine(fmtP, "Dropped", file.DroppedPacketCount);
            if (file.StartTime != null)
            {
                OmniPrinters.log.WriteLine(fmtP, "Start", file.StartTime.ToDateTime());
            }
            if (file.EndTime != null)
            {
                OmniPrinters.log.WriteLine(fmtP, "End", file.EndTime.ToDateTime());
            }
            OmniPrinters.log.WriteLine(fmtP, "Timezone Bias", file.TimeZoneBias);
        }

        public static void PrintForensicFileList(ForensicFileList files, int tab=0)
        {
            foreach (ForensicFile file in files)
            {
                PrintForensicFile(file, tab);
                OmniPrinters.log.WriteLine("");
            }
            OmniPrinters.log.WriteLine("");
        }

        public static void PrintForensicSearch(ForensicSearch search, int tab=0)
        {
            OmniPrinters.log.WriteLine("Forensic Search");
            if (search == null)
            {
                OmniPrinters.log.WriteLine("  No forensic search");
                return;
            }
            String fmtP = "{0," + (24+tab) + "}: {1}";
            OmniPrinters.log.WriteLine(fmtP, "Name", search.Name);
            OmniPrinters.log.WriteLine(fmtP, "Id", search.Id);
            OmniPrinters.log.WriteLine(fmtP, "Adapter", search.Adapter);
            OmniPrinters.log.WriteLine(fmtP, "Capture", search.CaptureName);
            OmniPrinters.log.WriteLine(fmtP, "Session Id", search.SessionId);
            OmniPrinters.log.WriteLine(fmtP, "Creator", search.Creator);
            OmniPrinters.log.WriteLine(fmtP, "Creator SID", search.CreatorSid);
            OmniPrinters.log.WriteLine(fmtP, "Duration", search.Duration);
            OmniPrinters.log.WriteLine(fmtP, "Status", search.Status);
            OmniPrinters.log.WriteLine(fmtP, "First Packet", search.FirstPacket);
            OmniPrinters.log.WriteLine(fmtP, "Link Speed", search.LinkSpeed);
            OmniPrinters.log.WriteLine(fmtP, "Load Progress", search.LoadProgress);
            OmniPrinters.log.WriteLine(fmtP, "Load Percent", search.LoadPercent);
            OmniPrinters.log.WriteLine("Media Info");
            OmniPrinters.log.WriteLine(fmtP, "Media Type", search.MediaType);
            OmniPrinters.log.WriteLine(fmtP, "Media Sub Type", search.MediaSubType);
            OmniPrinters.log.WriteLine(fmtP, "Modified", search.ModifiedBy);
            OmniPrinters.log.WriteLine(fmtP, "Modification Type", search.ModificationType);
            OmniPrinters.log.WriteLine(fmtP, "Open Result", search.OpenResult);
            OmniPrinters.log.WriteLine(fmtP, "Packet Count", search.PacketCount);
            OmniPrinters.log.WriteLine(fmtP, "Percent Progress", search.PercentProgress);
            OmniPrinters.log.WriteLine(fmtP, "Process Percent Progress", search.ProcessPercentProgress);
            OmniPrinters.log.WriteLine(fmtP, "Process Progress", search.ProcessProgress);
            OmniPrinters.log.WriteLine(fmtP, "Progress", search.Progress);
            OmniPrinters.log.WriteLine(fmtP, "Start Time", search.StartTime.ToDateTime());
            OmniPrinters.log.WriteLine(fmtP, "Stop Time", search.StopTime.ToDateTime());
            OmniPrinters.log.WriteLine(fmtP, "Option Expert", search.OptionExpert);
            OmniPrinters.log.WriteLine(fmtP, "Option Graphs", search.OptionGraphs);
            OmniPrinters.log.WriteLine(fmtP, "Option Log", search.OptionLog);
            OmniPrinters.log.WriteLine(fmtP, "Option Packet Buffer", search.OptionPacketBuffer);
            OmniPrinters.log.WriteLine(fmtP, "Option Voice", search.OptionVoice);
            OmniPrinters.log.WriteLine(fmtP, "Option Web", search.OptionWeb);
        }

        public static void PrintForensicSearchList(ForensicSearchList searches, int tab=0)
        {
            foreach (ForensicSearch search in searches)
            {
                PrintForensicSearch(search, tab);
                OmniPrinters.log.WriteLine("");
            }
            OmniPrinters.log.WriteLine("");
        }

        public static void PrintGraphTemplate(GraphTemplate graph, int tab=0)
        {
            OmniPrinters.log.WriteLine("Graph Template");
            if (graph == null)
            {
                OmniPrinters.log.WriteLine("  No graph template");
                return;
            }
            String fmtP = "{0," + (4+tab) + "}: {1}";
            OmniPrinters.log.WriteLine(fmtP, "Name", graph.Name);
        }

        public static void PrintGraphTemplateList(GraphTemplateList graphs, int tab=0)
        {
            foreach (GraphTemplate graph in graphs)
            {
                PrintGraphTemplate(graph, tab);
                OmniPrinters.log.WriteLine("");
            }
            OmniPrinters.log.WriteLine("");
        }

        public static void PrintStatsLimit(CaptureTemplate.AnalysisSettings.StatsLimits limits, int tab=0)
        {
            if (limits == null)
            {
                OmniPrinters.log.WriteLine("No Limits");
                return;
            }
            String fmtP = "{0," + (12+tab) + "}: {1}";
            OmniPrinters.log.WriteLine(fmtP, "Name", limits.Name);
            OmniPrinters.log.WriteLine(fmtP, "Enabled", limits.Enabled);
            OmniPrinters.log.WriteLine(fmtP, "Limit", limits.Limit);
            OmniPrinters.log.WriteLine(fmtP, "Severity", limits.Severity);
            OmniPrinters.log.WriteLine(fmtP, "OptionNotify", limits.OptionNotify);
            OmniPrinters.log.WriteLine(fmtP, "OptionReset", limits.OptionReset);
        }
    }
}
