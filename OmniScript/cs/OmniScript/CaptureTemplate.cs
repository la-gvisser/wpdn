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
    using System.IO;
    using System.Linq;
    using System.Xml.Linq;

    public class CaptureTemplate
    {
        public const String tagClsid = "clsid";
        public const String tagEnabled = "enabled";
        public const String tagAltEnabled = "Enabled";
        public const String tagId = "id";
        public const String tagName = "name";
        public const String tagObject = "obj";
        public const String tagProps = "properties";
        public const String tagProp = "prop";
        public const String tagType = "type";
        public const String tagPropBag = "SimplePropBag";

        public class CaptureProperty
        {
            static public void AddNull(XElement node)
            {
                node.Add(new XAttribute("null", "1"));
            }

            static public bool BooleanProp(XElement node)
            {
                return (Convert.ToInt32(node.Value) != 0);
            }

            static public bool BooleanAttribute(XElement node, String attribute)
            {
                XAttribute attrib = node.Attribute(attribute);
                Int32 value = (attrib != null) ? Convert.ToInt32(attrib.Value) : 0;
                return (value != 0);
            }

            static public String BooleanAttribute(bool value)
            {
                return ((value) ? "1" : "0");
            }

            static public XAttribute BooleanAttribute(String name, bool value)
            {
                return new XAttribute(name, BooleanAttribute(value));
            }

            public String PropObjectName { get; set; }   // XML name attribute.
            public String PropClassIdName { get; set; } // XML clsid attribute.
            public String PropRootName { get; set; }    // XML name of the first element.
            public bool Empty { get; set; }

            public CaptureProperty()
            {
                this.Empty = true;
            }

            public CaptureProperty(String objectName, String classIdName, String rootName)
            {
                this.PropObjectName = objectName;
                this.PropClassIdName = classIdName;
                this.PropRootName = rootName;
                this.Empty = true;
            }

            public XElement CreateObj()
            {
                XElement obj = new XElement("obj",
                    new XAttribute("name", this.PropObjectName));
                if (this.Empty)
                {
                    CaptureProperty.AddNull(obj);
                }
                else
                {
                    obj.Add(new XAttribute("clsid", OmniScript.ClassNameIds[this.PropClassIdName]));
                }
                return obj;
            }

            public bool PreLoad(XElement obj)
            {
                this.Empty = false;
                XAttribute nil = obj.Attribute("null");
                if (nil != null)
                {
                    this.Empty = (Convert.ToUInt32(nil.Value) != 0);
                }
                return this.Empty;
            }
        }

        public class AdapterSettings : CaptureProperty
        {
            public const String ObjectName = "AdapterSettings";
            public const String ClassIdName = tagPropBag;
            public const String RootName  = tagProps;

            public AdapterTypes Type { get; set; }
            public String Name { get; set; }
            public uint Limit { get; set; }
            public uint Mode { get; set; }
            public double Speed { get; set; }

            public AdapterSettings()
                : base(AdapterSettings.ObjectName, AdapterSettings.ClassIdName,
                    AdapterSettings.RootName)
            {
                this.Name = "Local Area Connection";
            }

            public void Load(XElement obj)
            {
                if (base.PreLoad(obj)) return;

                var adptName = from prop in obj.Element("properties").Elements("prop")
                               where (String)prop.Attribute("name").Value == "Name"
                               select prop;
                this.Name = adptName.ElementAtOrDefault<XElement>(0).Value;
            }

            public void SetAdapter(Adapter adapter)
            {
                if (adapter == null)
                {
                    this.Empty = true;
                }
                else
                {
                    this.Empty = false;
                    switch (adapter.AdapterType)
                    {
                        case AdapterTypes.NIC:
                            this.Type = AdapterTypes.NIC;
                            this.Name = adapter.Name;
                            break;

                        case AdapterTypes.File:
                            {
                                FileAdapter fileAdapter = adapter as FileAdapter;
                                this.Type = AdapterTypes.File;
                                this.Name = fileAdapter.Name;
                                this.Limit = fileAdapter.Limit;
                                this.Mode = fileAdapter.Mode;
                                this.Speed = fileAdapter.Speed;
                            }
                            break;

                        case AdapterTypes.Plugin:
                            break;

                        default:
                            throw new OmniException("Unsupported adapter type.");
                    }
                }
            }

            public XElement Store(OmniEngine engine)
            {
                XElement obj = base.CreateObj();
                if (!this.Empty)
                {
                    XElement props = obj.Element(AdapterSettings.RootName);
                    if (props == null)
                    {
                        props = new XElement(AdapterSettings.RootName);
                        obj.Add(props);
                    }
                    props.RemoveAll();
                    props.Add(OmniScript.Prop("Name", 8, this.Name));
                    props.Add(OmniScript.Prop("Type", 3, this.Type.ToString("D")));
                    if (engine != null)
                    {
                        switch (this.Type)
                        {
                            case AdapterTypes.NIC:
                                if (engine != null)
                                {
                                    Adapter adapter = engine.FindAdapterByName(this.Name);
                                    if (adapter != null)
                                    {
                                        props.Add(OmniScript.Prop("Enumerator", 8, adapter.Id));
                                    }
                                }
                                break;

                            case AdapterTypes.File:
                                props.Add(OmniScript.Prop("ReplayLimit", 19, this.Limit.ToString()));
                                props.Add(OmniScript.Prop("ReplayTimeStampMode", 3, this.Mode.ToString()));
                                props.Add(OmniScript.Prop("ReplaySpeed", 8, this.Speed.ToString("F1")));
                                break;
                        }
                    }
                }
                return obj;
            }
        };

        public class AlarmSettings : CaptureProperty
        {
            public const String ObjectName = "AlarmConfig";
            public const String ClassIdName = "AlarmConfig";
            public const String RootName = "Alarmconfig";

            public List<OmniId> Alarms { get; set; }

            public AlarmSettings()
                : base(AlarmSettings.ObjectName, AlarmSettings.ClassIdName,
                    AlarmSettings.RootName)
            {
                this.Alarms = new List<OmniId>();
            }

            public void Load(XElement obj)
            {
                if (base.PreLoad(obj)) return;

                XElement root = obj.Element(AlarmSettings.RootName);
                IEnumerable<XElement> alarms = root.Element("Alarms").Elements("Alarm");
                foreach (XElement alarm in alarms)
                {
                    XAttribute id = alarm.Attribute("id");
                    if (id != null)
                    {
                        this.Alarms.Add(Guid.Parse(id.Value));
                    }
                }
            }

            public XElement Store()
            {
                XElement obj = base.CreateObj();
                if (!this.Empty)
                {
                    XElement alarms = new XElement("Alarms");
                    foreach (Guid alarm in this.Alarms)
                    {
                        alarms.Add(new XElement("Alarm",
                            new XAttribute("id", alarm)));
                    }
                    obj.Add(new XElement(this.PropRootName, alarms));
                }
                return obj;
            }

            public void Store(XElement obj)
            {
                obj.Add(this.Store());
            }
        };

        public class AnalysisSettings : CaptureProperty
        {
            public class StatsLimits
            {
                public enum LimitIds
                {
                    Node,
                    Detail,
                    Protocol
                };

                static public readonly String ClassIdName;
                static public readonly OmniId ClassId;
                static public Dictionary<String, LimitIds> LimitIdMap;
                static public Dictionary<LimitIds, String> LimitLabelMap;

                static StatsLimits()
                {
                    ClassIdName = "PeekStatsLimitSettings";
                    ClassId = OmniScript.ClassNameIds[ClassIdName];
                    
                    LimitIdMap = new Dictionary<String, LimitIds>()
                    {
                        {"Node Statistics", LimitIds.Node},
                        {"Node/Protocol Detail Statistics", LimitIds.Detail},
                        {"Protocol Statistics", LimitIds.Protocol}
                    };

                    LimitLabelMap = LimitIdMap.ToDictionary(x => x.Value, x => x.Key);
                }


                static public XElement Store(LimitIds id)
                {
                    XElement nullProp = new XElement("prop",
                        new XAttribute("name", LimitLabelMap[id]),
                        new XAttribute("type", 3));
                    nullProp.Value = "0";
                    return nullProp;
                }

                public String Name { get; set; }
                public bool Enabled { get; set; }
                public uint Limit { get; set; }
                public uint Severity { get; set; }
                public bool OptionNotify { get; set; }
                public bool OptionReset { get; set; }

                public StatsLimits()
                {
                    this.Enabled = false;
                    this.Limit = 100000;
                    this.Severity = 3;
                    this.OptionNotify = true;
                    this.OptionReset = false;
                }

                public StatsLimits(String name)
                    : this()
                {
                    this.Name = name;
                }

                public StatsLimits(XElement prop)
                    : this()
                {
                    this.Load(prop);
                }

                public void Load(XElement prop)
                {
                    if (!prop.HasAttributes) return;

                    this.Name = prop.Attribute("name").Value;

                    XElement limits = prop.Element("PeekStatsLimitSettings");
                    if (limits == null) return;

                    this.Enabled = BooleanAttribute(limits, "Enabled");
                    this.Limit = Convert.ToUInt32(limits.Attribute("Limit").Value);
                    this.Severity = Convert.ToUInt32(limits.Attribute("Severity").Value);
                    this.OptionNotify = BooleanAttribute(limits, "Notify");
                    this.OptionReset = BooleanAttribute(limits, "Reset");
                }

                public XElement Store()
                {
                    return new XElement("obj",
                        new XAttribute("clsid", ClassId),
                        new XAttribute("name", this.Name),
                        new XElement("PeekStatsLimitSettings",
                            BooleanAttribute("Enabled", this.Enabled),
                            new XAttribute("Limit", this.Limit.ToString()),
                            BooleanAttribute("Notify", this.OptionNotify),
                            BooleanAttribute("Reset", this.OptionReset),
                            new XAttribute("Severity", this.Severity.ToString())));
                }
            }

            public const String ObjectName = "PerfConfig";
            public const String ClassIdName = tagPropBag;
            public const String RootName = tagProps;

            public StatsLimits NodeLimit { get; set; }
            public StatsLimits NodeProtocolDetailLimit { get; set; }
            public StatsLimits ProtocolLimit { get; set; }
            public bool OptionAlarms { get; set; }
            public bool OptionAnalysisModules { get; set; }
            public bool OptionApplication { get; set; }
            public bool OptionCountry { get; set; }
            public bool OptionError { get; set; }
            public bool OptionExpert { get; set; }
            public bool OptionNetwork { get; set; }
            public bool OptionSize { get; set; }
            public bool OptionSummary { get; set; }
            public bool OptionTopTalker { get; set; }
            public bool OptionTraffic { get; set; }
            public bool OptionVoiceVideo { get; set; }
            public bool OptionWirelessChannel { get; set; }
            public bool OptionWirelessNode { get; set; }

            public AnalysisSettings()
                : base(AnalysisSettings.ObjectName, AnalysisSettings.ClassIdName,
                    AnalysisSettings.RootName)
            {
                this.NodeLimit = new StatsLimits("Node Statistics");
                this.NodeLimit.Enabled = false;
                this.NodeProtocolDetailLimit = new StatsLimits("Node/Protocol Detail Statistics");
                this.NodeProtocolDetailLimit.Enabled = false;
                this.ProtocolLimit = new StatsLimits("Protocol Statistics");
                this.ProtocolLimit.Enabled = false;
                this.OptionAlarms = false;
                this.OptionAnalysisModules = false;
                this.OptionApplication = false;
                this.OptionCountry = false;
                this.OptionError = false;
                this.OptionExpert = false;
                this.OptionNetwork = false;
                this.OptionSize = false;
                this.OptionSummary = false;
                this.OptionTopTalker = false;
                this.OptionTraffic = false;
                this.OptionVoiceVideo = false;
                this.OptionWirelessChannel = false;
                this.OptionWirelessNode = false;
                this.Empty = false;
            }

            public void Load(XElement obj)
            {
                if (base.PreLoad(obj)) return;

                XElement root = obj.Element(AnalysisSettings.RootName);
                IEnumerable<XElement> props = root.Elements("prop");
                foreach (XElement prop in props)
                {
                    String name = prop.Attribute("name").Value.ToString();
                    switch (name)
                    {
                        case "Alarms":
                            this.OptionAlarms = BooleanProp(prop);
                            break;

                        case "Analysis Modules":
                            this.OptionAnalysisModules = BooleanProp(prop);
                            break;

                        case "Application Statistics":
                            this.OptionApplication = BooleanProp(prop);
                            break;

                        case "Country Statistics":
                            this.OptionCountry = BooleanProp(prop);
                            break;

                        case "Error Statistics":
                            this.OptionError = BooleanProp(prop);
                            break;

                        case "Expert Analysis":
                            this.OptionExpert = BooleanProp(prop);
                            break;

                        case "Network Statistics":
                            this.OptionNetwork = BooleanProp(prop);
                            break;

                        case "Node Statistics":
                            this.NodeLimit = new StatsLimits("Node Statistics");
                            break;

                        case "Node/Protocol Detail Statistics":
                            this.NodeProtocolDetailLimit = new StatsLimits("Node/Protocol Detail Statistics");
                            break;

                        case "Protocol Statistics":
                            this.ProtocolLimit = new StatsLimits("Protocol Statistics");
                            break;

                        case "Size Statistics":
                            this.OptionSize = BooleanProp(prop);
                            break;

                        case "Summary Statistics":
                            this.OptionSummary = BooleanProp(prop);
                            break;

                        case "Top Talker Statistics":
                            this.OptionTopTalker = BooleanProp(prop);
                            break;

                        case "Traffic History Statistics":
                            this.OptionTraffic = BooleanProp(prop);
                            break;

                        case "Voice and Video Analysis":
                            this.OptionVoiceVideo = BooleanProp(prop);
                            break;

                        case "Wireless Channel Statistics":
                            this.OptionWirelessChannel = BooleanProp(prop);
                            break;

                        case "Wireless Node Statistics":
                            this.OptionWirelessNode = BooleanProp(prop);
                            break;
                    }
                }

                IEnumerable<XElement> objs = obj.Element("properties").Elements("obj");
                foreach (XElement propobj in objs)
                {
                    String name = propobj.Attribute("name").Value.ToString();
                    StatsLimits.LimitIds id = StatsLimits.LimitIdMap[name];
                    switch (id)
                    {
                        case StatsLimits.LimitIds.Node: //"Node Statistics":
                            this.NodeLimit = new StatsLimits(propobj);
                            break;

                        case StatsLimits.LimitIds.Detail:
                            this.NodeProtocolDetailLimit = new StatsLimits(propobj); //.Load(propobj);
                            break;

                        case StatsLimits.LimitIds.Protocol:
                            this.ProtocolLimit = new StatsLimits(propobj); // .Load(propobj);
                            break;
                    }
                }
            }

            public void SetAll(bool enable = true)
            {
                this.NodeLimit = new StatsLimits("Node Statistics");
                this.NodeLimit.Enabled = enable;
                this.NodeProtocolDetailLimit = new StatsLimits("Node/Protocol Detail Statistics");
                this.NodeProtocolDetailLimit.Enabled = enable;
                this.ProtocolLimit = new StatsLimits("Protocol Statistics");
                this.ProtocolLimit.Enabled = enable;

                this.OptionAlarms = enable;
                this.OptionAnalysisModules = enable;
                this.OptionApplication = enable;
                this.OptionCountry = enable;
                this.OptionError = enable;
                this.OptionExpert = enable;
                this.OptionNetwork = enable;
                this.OptionSize = enable;
                this.OptionSummary = enable;
                this.OptionTopTalker = enable;
                this.OptionTraffic = enable;
                this.OptionVoiceVideo = enable;
                this.OptionWirelessChannel = enable;
                this.OptionWirelessNode = enable;
            }

            public XElement Store()
            {
                XElement obj = base.CreateObj();
                if (!this.Empty)
                {
                    obj.Add(new XElement(AnalysisSettings.RootName,
                        OmniScript.Prop("Alarms", this.OptionAlarms),
                        OmniScript.Prop("Analysis Modules", this.OptionAnalysisModules),
                        OmniScript.Prop("Application Statistics", this.OptionApplication),
                        OmniScript.Prop("Country Statistics", this.OptionCountry),
                        OmniScript.Prop("Error Statistics", this.OptionError),
                        OmniScript.Prop("Expert Analysis", this.OptionExpert),
                        OmniScript.Prop("Network Statistics", this.OptionNetwork),
                        OmniScript.Prop("Size Statistics", this.OptionSize),
                        OmniScript.Prop("Summary Statistics", this.OptionSummary),
                        OmniScript.Prop("Top Talker Statistics", this.OptionTopTalker),
                        OmniScript.Prop("Traffic History Statistics", this.OptionTraffic),
                        OmniScript.Prop("Voice and Video Analysis", this.OptionVoiceVideo),
                        OmniScript.Prop("Wireless Channel Statistics", this.OptionWirelessChannel),
                        OmniScript.Prop("Wireless Node Statistics", this.OptionWirelessNode),
                        (this.NodeLimit != null && this.NodeLimit.Enabled)
                            ? this.NodeLimit.Store()
                            : OmniScript.Prop("Node Statistics", false),
                        (this.NodeProtocolDetailLimit != null && this.NodeProtocolDetailLimit.Enabled)
                            ? this.NodeProtocolDetailLimit.Store()
                            : OmniScript.Prop("Node/Protocol Detail Statistics", false),
                        (this.ProtocolLimit != null && this.ProtocolLimit.Enabled)
                            ? this.ProtocolLimit.Store()
                            : OmniScript.Prop("Protocol Statistics", false)));
                }
                return obj;
            }

            public void Store(XElement obj)
            {
                obj.Add(this.Store());
            }
        };

        public class FilterSettings : CaptureProperty
        {
            public const String ObjectName = "FilterConfig";
            public const String ClassIdName = "FilterConfig";
            public const String RootName = "filterconfig";

            public enum Modes
            {
                AcceptMatchingAny = 1,
                RejectMatching = 3,
                AcceptMatchingAll
            };

            public Modes Mode { get; set; }
            public List<String> Filters { get; set; }
            public List<OmniId> IdList { get; set; }

            private void UpdateEmpty()
            {
                this.Empty = (this.IdList.Count == 0) && (this.Filters.Count == 0);
            }

            public FilterSettings()
                : base(FilterSettings.ObjectName, FilterSettings.ClassIdName,
                    FilterSettings.RootName)
            {
                this.Mode = Modes.AcceptMatchingAny;
                this.Filters = new List<String>();
                this.IdList = new List<OmniId>();
                this.Empty = true;
            }

            public void Load(XElement obj, OmniEngine engine=null)
            {
                if (base.PreLoad(obj)) return;

                FilterList filterList = null;
                if (engine != null)
                {
                    filterList = engine.GetFilterList();
                }

                XElement config = obj.Element(RootName);
                if (config != null)
                {
                    XAttribute mode = config.Attribute("mode");
                    this.Mode = (mode != null) ? (Modes)Convert.ToUInt32(mode.Value) : Modes.AcceptMatchingAny;
                    IEnumerable<XElement> filters = config.Element("filters").Elements("filter");
                    foreach (XElement filter in filters)
                    {
                        XAttribute idAttribute = filter.Attribute("id");
                        if (idAttribute != null)
                        {
                            Guid id = Guid.Parse(idAttribute.Value);

                            Filter fltr = null;
                            if (filterList != null)
                            {
                                fltr = filterList.Find(id);
                                if (fltr != null)
                                {
                                    this.Filters.Add(fltr.Name);
                                }
                            }
                            if (fltr == null)
                            {
                                this.IdList.Add(id);
                            }
                        }
                    }
                }
                this.UpdateEmpty();
            }

            public void SetFilters(List<String> filters)
            {
                this.Filters.Clear();
                this.Filters.AddRange(filters);
                this.UpdateEmpty();
            }

            public XElement Store(OmniEngine engine)
            {
                this.UpdateEmpty();

                XElement obj = base.CreateObj();
                if (!this.Empty)
                {
                    this.Store(obj, engine);
                }
                return obj;
            }

            public void Store(XElement obj, OmniEngine engine)
            {
                this.UpdateEmpty();

                if (this.Empty && (engine == null))
                {
                    // set null.
                    return;
                }

                XElement props = new XElement("filters");

                if (this.IdList.Count > 0)
                {
                    foreach (OmniId id in IdList)
                    {
                        props.Add(new XElement("filter",
                            new XAttribute("id", id)));
                    }
                }
                else
                {
                    FilterList filters = engine.GetFilterList();
                    foreach (String name in this.Filters)
                    {
                        Filter filter = filters.Find(name);
                        if (filter != null)
                        {
                            props.Add(new XElement("filter",
                                new XAttribute("id", filter.Id)));
                        }
                    }
                }

                uint mode = (uint)this.Mode;
                obj.Add(new XElement(this.PropRootName,
                    new XAttribute("mode", mode.ToString()),
                    props));
            }
        };

        public class GeneralSettings : CaptureProperty
        {
            public const String ObjectName = "GeneralSettings";
            public const String ClassIdName = tagPropBag;
            public const String RootName = tagProps;

            public ulong BufferSize { get; set; }
            public String Comment { get; set; }
            public String Directory { get; set; }
            public String FilePattern { get; set; }
            public ulong FileSize { get; set; }
            public OmniId GroupId { get; set; }
            public String GroupName { get; set; }
            public OmniId Id { get; set; }
            public uint KeepLastFilesCount { get; set; }
            public ulong MaxFileAge { get; set; }
            public ulong MaxTotalFileSize { get; set; }
            public String Name { get; set; }
            public String Owner { get; set; }
            public uint ReservedSize { get; set; }
            public uint SliceLength { get; set; }
            public uint TapTimestamps { get; set; }
            public bool OptionCaptureToDisk { get; set; }
            public bool OptionContinuousCapture { get; set; }
            public bool OptionDeduplicate { get; set; }
            public bool OptionFileAge { get; set; }
            public bool OptionKeepLastFiles { get; set; }
            public bool OptionPriorityCtd { get; set; }
            public bool OptionSaveAsTemplate { get; set; }
            public bool OptionSlicing { get; set; }
            public bool OptionStartCapture { get; set; }
            public bool OptionTimelineAppStats { get; set; }
            public bool OptionTimelineStats { get; set; }
            public bool OptionTimelineTopStats { get; set; }
            public bool OptionTimelineVoipStats { get; set; }
            public bool OptionTotalFileSize { get; set; }

            public GeneralSettings()
                : base(GeneralSettings.ObjectName, GeneralSettings.ClassIdName,
                    GeneralSettings.RootName)
            {
                this.BufferSize = 100 * OmniScript.bytesInAMegabyte;
                this.Comment = null;
                this.Directory = null;
                this.FilePattern = null;
                this.FileSize = 256 * OmniScript.bytesInAMegabyte;
                this.GroupId = Guid.Empty;
                this.GroupName = null;
                this.Id = Guid.Empty;
                this.KeepLastFilesCount = 10;
                this.MaxFileAge = OmniScript.secondsInADay;
                this.MaxTotalFileSize = 10 * OmniScript.bytesInAGigabyte;
                this.Name = null;
                this.Owner = null;
                this.SliceLength = 128;
                this.TapTimestamps = 0;
                this.OptionCaptureToDisk = false;
                this.OptionContinuousCapture = true;
                this.OptionDeduplicate = false;
                this.OptionFileAge = false;
                this.OptionKeepLastFiles = false;
                this.OptionPriorityCtd = false;
                this.OptionSaveAsTemplate = false;
                this.OptionSlicing = false;
                this.OptionStartCapture = false;
                this.OptionTimelineAppStats = false;
                this.OptionTimelineStats = false;
                this.OptionTimelineTopStats = false;
                this.OptionTimelineVoipStats = false;
                this.OptionTotalFileSize = false;
                this.Empty = false;
            }

            public void Load(XElement obj)
            {
                if (base.PreLoad(obj)) return;

                IEnumerable<XElement> props = obj.Element(GeneralSettings.RootName).Elements("prop");
                foreach (XElement prop in props)
                {
                    switch (prop.Attribute("name").Value)
                    {
                        case "BufferSize":
                            this.BufferSize = Convert.ToUInt64(prop.Value);
                            break;

                        case "CaptureID":
                            this.Id = Guid.Parse(prop.Value);
                            break;

                        case "CaptureToDisk":
                            this.OptionCaptureToDisk = OmniScript.PropBoolean(prop.Value);
                            break;

                        case "Comment":
                            this.Comment = prop.Value;
                            break;

                        case "ContinuousCapture":
                            this.OptionContinuousCapture = OmniScript.PropBoolean(prop.Value);
                            break;

                        case "CtdDir":
                            this.Directory = prop.Value;
                            break;

                        case "CtdFilePattern":
                            this.FilePattern = prop.Value;
                            break;

                        case "CtdFileSize":
                            this.FileSize = Convert.ToUInt64(prop.Value);
                            break;

                        case "CtdKeepLastFiles":
                            this.OptionKeepLastFiles = OmniScript.PropBoolean(prop.Value);
                            break;

                        case "CtdKeepLastFilesCount":
                            this.KeepLastFilesCount = Convert.ToUInt32(prop.Value);
                            break;

                        case "CtdMaxTotalFileSize":
                            this.MaxTotalFileSize = Convert.ToUInt64(prop.Value);
                            break;

                        case "CtdPriority":
                            this.OptionPriorityCtd = OmniScript.PropBoolean(prop.Value);
                            break;

                        case "CtdUseMaxFileAge":
                            this.OptionFileAge = OmniScript.PropBoolean(prop.Value);
                            break;

                        case "CtdUseMaxTotalFileSize":
                            this.OptionTotalFileSize = OmniScript.PropBoolean(prop.Value);
                            break;

                        case "CtdMaxFileAge":
                            this.MaxFileAge = Convert.ToUInt64(prop.Value);
                            break;

                        case "Deduplicate":
                            this.OptionDeduplicate = OmniScript.PropBoolean(prop.Value);
                            break;

                        case "EnableTimelineStats":
                            this.OptionTimelineStats = OmniScript.PropBoolean(prop.Value);
                            break;

                        case "EnableAppStats":
                            this.OptionTimelineAppStats = OmniScript.PropBoolean(prop.Value);
                            break;

                        case "EnableTopStats":
                            this.OptionTimelineTopStats = OmniScript.PropBoolean(prop.Value);
                            break;

                        case "EnableVoIPStats":
                            this.OptionTimelineVoipStats = OmniScript.PropBoolean(prop.Value);
                            break;

                        case "GroupID":
                            this.GroupId = Guid.Parse(prop.Value);
                            break;

                        case "GroupName":
                            this.GroupName = prop.Value;
                            break;

                        case "Name":
                            this.Name = prop.Value;
                            break;

                        case "Owner":
                            this.Owner = prop.Value;
                            break;

                        case "SaveAsTemplate":
                            this.OptionSaveAsTemplate = OmniScript.PropBoolean(prop.Value);
                            break;

                        case "SliceLength":
                            this.SliceLength = Convert.ToUInt32(prop.Value);
                            break;

                        case "Slicing":
                            this.OptionSlicing = OmniScript.PropBoolean(prop.Value);
                            break;

                        case "StartCapture":
                            this.OptionStartCapture = OmniScript.PropBoolean(prop.Value);
                            break;

                        case "TapTimestamps":
                            this.TapTimestamps = Convert.ToUInt32(prop.Value);
                            break;
                    }
                }
            }

            public void SetTimeline(bool enable = true)
            {
                this.OptionTimelineAppStats = enable;
                this.OptionTimelineStats = enable;
                this.OptionTimelineTopStats = enable;
                this.OptionTimelineVoipStats = enable;
            }

            public XElement Store()
            {
                XElement obj = base.CreateObj();
                if (!this.Empty)
                {
                    XElement props = obj.Element(GeneralSettings.RootName);
                    if (props == null)
                    {
                        props = new XElement(GeneralSettings.RootName);
                        obj.Add(props);
                    }

                    props.RemoveAll();
                    props.Add(OmniScript.Prop("BufferSize", 21, this.BufferSize.ToString()));
                    if (!OmniId.IsNullOrEmpty(this.Id))
                    {
                        props.Add(OmniScript.Prop("CaptureID", 8, this.Id.ToString()));
                    }
                    props.Add(OmniScript.Prop("CaptureToDisk", this.OptionCaptureToDisk));

                    if (!String.IsNullOrEmpty(this.Comment))
                    {
                        props.Add(OmniScript.Prop("Comment", 8, this.Comment));
                    }

                    props.Add(OmniScript.Prop("ContinuousCapture", this.OptionContinuousCapture));

                    if (!String.IsNullOrEmpty(this.Directory))
                    {
                        props.Add(OmniScript.Prop("CtdDir", 8, this.Directory));
                    }

                    if (String.IsNullOrEmpty(this.FilePattern))
                    {
                        props.Add(OmniScript.Prop("CtdFilePattern", 8, this.Name));
                    }
                    else
                    {
                        props.Add(OmniScript.Prop("CtdFilePattern", 8, this.FilePattern));
                    }

                    props.Add(OmniScript.Prop("CtdFileSize", 21, this.FileSize.ToString()));
                    props.Add(OmniScript.Prop("CtdKeepLastFiles", this.OptionKeepLastFiles));
                    props.Add(OmniScript.Prop("CtdKeepLastFilesCount", 19, this.KeepLastFilesCount.ToString()));
                    props.Add(OmniScript.Prop("CtdMaxFileAge", 21, this.MaxFileAge.ToString()));
                    props.Add(OmniScript.Prop("CtdMaxTotalFileSize", 21, this.MaxTotalFileSize.ToString()));
                    props.Add(OmniScript.Prop("CtdPriority", this.OptionPriorityCtd));
                    props.Add(OmniScript.Prop("CtdUseMaxFileAge", this.OptionFileAge));
                    props.Add(OmniScript.Prop("CtdUseMaxTotalFileSize", this.OptionTotalFileSize));
                    props.Add(OmniScript.Prop("Deduplicate", this.OptionDeduplicate));
                    props.Add(OmniScript.Prop("EnableAppStats", this.OptionTimelineAppStats));
                    props.Add(OmniScript.Prop("EnableTimelineStats", this.OptionTimelineStats));
                    props.Add(OmniScript.Prop("EnableTopStats", this.OptionTimelineTopStats));
                    props.Add(OmniScript.Prop("EnableVoIPStats", this.OptionTimelineVoipStats));
                    if (!OmniId.IsNullOrEmpty(this.GroupId))
                    {
                        props.Add(OmniScript.Prop("GroupID", 8, this.GroupId.ToString()));
                    }
                    if (!String.IsNullOrEmpty(this.GroupName))
                    {
                        props.Add(OmniScript.Prop("GroupName", 8, this.GroupName));
                    }
                    if (!String.IsNullOrEmpty(this.Name))
                    {
                        props.Add(OmniScript.Prop("Name", 8, this.Name));
                    }
                    if (!String.IsNullOrEmpty(this.Owner))
                    {
                        props.Add(OmniScript.Prop("Owner", 8, this.Owner));
                    }
                    props.Add(OmniScript.Prop("SaveAsTemplate", this.OptionSaveAsTemplate));
                    props.Add(OmniScript.Prop("SliceLength", 19, this.SliceLength.ToString()));
                    props.Add(OmniScript.Prop("Slicing", this.OptionSlicing));
                    props.Add(OmniScript.Prop("StartCapture", this.OptionStartCapture));
                    props.Add(OmniScript.Prop("TapTimestamps", 3, this.TapTimestamps.ToString()));
                }
                return obj;
            }

            public void Store(XElement obj)
            {
                obj.Add(this.Store());
            }
        };

        public class GraphsSettings : CaptureProperty
        {
            public const String ObjectName = "GraphSettings";
            public const String ClassIdName = "GraphSettings";
            public const String RootName = "graphdata";

            public bool Enabled { get; set; }
            private uint _interval;
            public uint Interval
            {
                get { return this._interval; }
                set
                {
                    if ((value >= 1) && (value <= 86400))
                    {
                        this._interval = value;
                    }
                    else
                    {
                        SystemException ex = null;
                        throw new ArgumentOutOfRangeException(@"Interval must be between 1 and 86400.", ex);
                    }
                }
            }
            private OmniScript.IntervalUnits _intervalUnit;
            public OmniScript.IntervalUnits IntervalUnit
            {
                get { return this._intervalUnit; }
                set {
                    if ((value >= OmniScript.IntervalUnits.Seconds) && (value <= OmniScript.IntervalUnits.Days))
                    {
                        this._intervalUnit = value;
                    }
                    else
                    {
                        SystemException ex = null;
                        throw new ArgumentOutOfRangeException(@"IntervalUnit must be an OmniScript.IntervalUnitss.", ex);
                    }
                }
            }
            public uint FileCount { get; set; }
            public ulong FileBufferSize { get; set; }
            private uint _recentHours;
            public uint RecentHours
            {
                get { return this._recentHours; }
                set {
                    if ((value >= 1) && (value <= 744))
                    {
                        this._recentHours = value;
                    }
                    else
                    {
                        SystemException ex = null;
                        throw new ArgumentOutOfRangeException(@"RecentHours must be between 1 and 744.", ex);
                    }
                }
            }
            public List<OmniId> Graphs { get; set; }
            public bool OptionPreserveFiles { get; set; }

            public GraphsSettings()
                : base(GraphsSettings.ObjectName, GraphsSettings.ClassIdName,
                    GraphsSettings.RootName)
            {
                this.Enabled = false;
                this.Interval = 15;                                    // OmniPeek default.
                this.IntervalUnit = OmniScript.IntervalUnits.Seconds;  // OmniPeek default.
                this.FileCount = 0;
                this.OptionPreserveFiles = false;
                this.RecentHours = 1;                                  // OmniPeek default.
                this.Graphs = new List<OmniId>();
                this.Empty = false;
            }

            public static List<OmniId> DefaultGraphsList()
            {
                List<OmniId> list = new List<OmniId>();

                list.Add(Guid.Parse("{D9E6C4FC-E6C7-4AA5-A438-398D07FBB954}"));
                list.Add(Guid.Parse("{FA79B6F2-FA7D-4DCD-8624-08ECC809C377}"));
                list.Add(Guid.Parse("{F1051F9A-73E1-4E68-A237-FFD0F13D9C4D}"));
                list.Add(Guid.Parse("{D0C2629A-7D37-4734-A43C-E077AF4F9DD1}"));
                list.Add(Guid.Parse("{04AC1D7F-910F-4562-9583-EF287BB455D5}"));
                list.Add(Guid.Parse("{F8BA4FF6-8D97-4901-9589-51995F89B215}"));
                list.Add(Guid.Parse("{091E0BA4-6AFC-472B-AA7C-293BE750068B}"));
                list.Add(Guid.Parse("{51B465E2-D00A-434B-9950-1575BB74DACC}"));
                list.Add(Guid.Parse("{CB14E82C-82E2-40B0-93B6-46854CA4CDAF}"));
                list.Add(Guid.Parse("{3002FB3C-B429-459A-994E-25C057CD9B75}"));
                list.Add(Guid.Parse("{22D3622F-FE9B-41E0-ADED-698321103220}"));
                list.Add(Guid.Parse("{23973FE7-561A-42ED-91C2-262B29BAC563}"));
                list.Add(Guid.Parse("{6BC54E72-336B-4667-B26B-0340034E2C58}"));
                list.Add(Guid.Parse("{90249607-62A8-4D21-93EA-BB36D23BFED6}"));
                list.Add(Guid.Parse("{A48F249F-9325-432F-AA7B-CD305A2F9B8B}"));
                list.Add(Guid.Parse("{1E983AC0-BA62-4AFB-BFCC-BE8822198038}"));
                list.Add(Guid.Parse("{EA628C31-C810-4B0F-9C8C-B26E1C8485B2}"));
                list.Add(Guid.Parse("{6D5270FF-DF00-4E71-8ED0-6F22BED9F99B}"));
                list.Add(Guid.Parse("{C40EBB17-8F04-4132-923E-BE28F48481B8}"));
                list.Add(Guid.Parse("{F6DCF6EE-7FA5-475A-BE8F-6FFA11071324}"));
                list.Add(Guid.Parse("{FA9CEB0D-77AD-464B-8D3A-5BF32A68D0F7}"));
                list.Add(Guid.Parse("{32B150F1-666F-4284-98C2-7614B45AA62B}"));
                list.Add(Guid.Parse("{00224414-E06F-475F-9FD5-93496F44EB39}"));
                list.Add(Guid.Parse("{4ED3C029-2265-4DDE-9C63-C8000B471502}"));
                list.Add(Guid.Parse("{5B70CF97-6093-494C-AF8F-7086D6B34E5C}"));
                list.Add(Guid.Parse("{A3A1A6D3-EB01-4AC4-A61A-04B2614CFBC2}"));
                list.Add(Guid.Parse("{73BA4154-CC2B-47F5-9823-225FF8337955}"));
                list.Add(Guid.Parse("{B1E79720-80A0-4734-B59D-FEEDDB5F2230}"));

                return list;
            }

            public void Load(XElement obj)
            {
                if (base.PreLoad(obj)) return;

                XElement graphData = obj.Element(GraphsSettings.RootName);
                IEnumerable<XAttribute> attributes = graphData.Attributes();
                foreach (XAttribute attribute in attributes)
                {
                    switch (attribute.Name.ToString())
                    {
                        case "Enabled":
                            this.Enabled = OmniScript.PropBoolean(attribute.Value);
                            break;

                        case "Interval":
                            this.Interval = Convert.ToUInt32(attribute.Value);
                            break;

                        case "IntervalUnit":
                            this.IntervalUnit = (OmniScript.IntervalUnits)Convert.ToUInt32(attribute.Value);
                            break;

                        case "FileCnt":
                            this.FileCount = Convert.ToUInt32(attribute.Value);
                            break;

                        case "FileBufferSize":
                            this.FileBufferSize = Convert.ToUInt64(attribute.Value);
                            break;

                        case "PreserveFiles":
                            this.OptionPreserveFiles = OmniScript.PropBoolean(attribute.Value);
                            break;

                        case "memory":
                            this.RecentHours = Convert.ToUInt32(attribute.Value);
                            break;
                    }

                    if (this.IntervalUnit == 0)
                    {
                        this.IntervalUnit = OmniScript.IntervalUnits.Seconds;
                    }

                    if (this.Interval == 0)
                    {
                        this.Interval = 1;
                    }
                }

                IEnumerable<XElement> templates = graphData.Element("templates").Elements("template");
                foreach (XElement template in templates)
                {
                    XAttribute attrib = template.Attribute("id");
                    if (attrib != null)
                    {
                        Guid id = Guid.Parse(attrib.Value);
                        this.Graphs.Add(id);
                    }
                }
            }

            public XElement Store()
            {
                XElement obj = base.CreateObj();
                if (!this.Empty)
                {
                    if ((this.Interval == 0) || (this.IntervalUnit == 0))
                    {
                        SystemException ex = null;
                        throw new ArgumentOutOfRangeException(@"Invalid Interval or Interval Unit value.", ex);
                    }
                    XElement graphData = new XElement(GraphsSettings.RootName,
                        new XAttribute("Enabled", OmniScript.PropBoolean(this.Enabled)),
                        new XAttribute("Interval", this.Interval.ToString()),
                        new XAttribute("IntervalUnit", ((uint)(this.IntervalUnit)).ToString()),
                        new XAttribute("FileCnt", this.FileCount.ToString()),
                        new XAttribute("FileBufferSize", this.FileBufferSize.ToString()),
                        new XAttribute("PreserveFiles", BooleanAttribute(this.OptionPreserveFiles)),
                        new XAttribute("memory", this.RecentHours.ToString()));
                    XElement templates = new XElement("templates");

                    List<OmniId> list = this.Graphs;
                    if (list.Count == 0)
                    {
                        list = GraphsSettings.DefaultGraphsList();
                    }
                    foreach (OmniId id in list)
                    {
                        templates.Add(new XElement("template",
                            new XAttribute("id", id)));
                    }

                    graphData.Add(templates);
                    obj.Add(graphData);
                }
                return obj;
            }

            public void Store(XElement obj)
            {
                obj.Add(this.Store());
            }
        }

        public class PluginsSettings : CaptureProperty
        {
            public const String ObjectName = "PluginsList";
            public const String ClassIdName = "PropertyList";
            public const String RootName = tagProps;

            public const String ConfigObjectName = "PluginsConfig";
            public const String ConfigClassIdName = "PluginsConfig";
            public const String ConfigRootName = "plugins";

            public PluginSettingsList PluginList { get; set; }

            public PluginsSettings()
                : base(PluginsSettings.ObjectName, PluginsSettings.ClassIdName,
                    PluginsSettings.RootName)
            {
                this.PluginList = new PluginSettingsList();
                this.Empty = false;
            }
            public void Add(PluginSettings settings)
            {
                this.PluginList.Add(settings);
            }

            public void Add(AnalysisModule plugin)
            {
                this.PluginList.Add(plugin);
            }
            public void Add(AnalysisModuleList plugins)
            {
                this.PluginList.Add(plugins);
            }

            public void Load(XElement plugins, XElement config)
            {
                if (base.PreLoad(plugins)) return;
                XElement node = plugins.Element(PluginSettingsList.PluginRootName);
                XElement configNode = config.Element(PluginSettingsList.ConfigRootName);
                this.PluginList.Load(node, configNode);
            }

            public void Set(AnalysisModuleList plugins)
            {
                this.PluginList.Set(plugins);
            }

             public List<XElement> Store()
             {
                 List<XElement> objs = new List<XElement>();

                 XElement obj = new XElement("obj",
                     new XAttribute("name", ObjectName),
                     new XAttribute("clsid", OmniScript.ClassNameIds[ClassIdName]));
                 XElement configObj = new XElement("obj",
                     new XAttribute("name", ConfigObjectName),
                     new XAttribute("clsid", OmniScript.ClassNameIds[ConfigClassIdName]));
                 this.Store(obj, configObj);

                 objs.Add(configObj);
                 objs.Add(obj);

                 return objs;
             }

            public void Store(XElement obj, XElement configObj)
            {
                this.PluginList.Store(obj, configObj);
            }
        };

        public class RepeatTriggerSettings
        {
            public const String PropName = "RepeatTrigger";

            public uint Value { get; set; }

            public RepeatTriggerSettings()
            {
                this.Value = 0;
            }

            public void Load(XElement prop)
            {
                this.Value = Convert.ToUInt32(prop.Value);
            }

            public XElement Store()
            {
                XElement prop = new XElement("prop",
                    new XAttribute("name", RepeatTriggerSettings.PropName));
                if (prop.Attribute("Type") == null)
                {
                    prop.Add(new XAttribute("type", "3"));
                }
                prop.Value = this.Value.ToString();
                return prop;
            }

            public void Store(XElement properties)
            {
                properties.Add(this.Store());
            }
        };

        public class StatisticsOutputSettings : CaptureProperty
        {
            public const String ObjectName = "StatsOutput";
            public const String ClassIdName = "StatsOutput";
            public const String RootName = "statsoutput";

            public bool Enabled { get; set; }
            public uint FilesToKeep { get; set; }
            public uint Interval { get; set; }          // Output Interval in seconds.
            public uint NewSetInterval { get; set; }    // New Set Interval in seconds.
            public String ReportPath { get; set; }
            public int ReportType { get; set; }
            public String UserPath { get; set; }
            public bool OptionNotify { get; set; }
            public bool OptionAlignNewSet { get; set; }
            public bool OptionAlignOutput { get; set; }
            public bool OptionNewSet { get; set; }
            public bool OptionScheduled { get; set; }
            public bool OptionKeepFiles { get; set; }
            public bool OptionResetOutput { get; set; }
            
            public StatisticsOutputSettings()
                : base(StatisticsOutputSettings.ObjectName, StatisticsOutputSettings.ClassIdName,
                    StatisticsOutputSettings.RootName)
            {
                this.Enabled = false;
                this.FilesToKeep = 10;
                this.Interval = OmniScript.secondsInAHour;
                this.NewSetInterval = OmniScript.secondsInADay;
                this.ReportPath = "Report";
                this.ReportType = 4;
                this.UserPath = "";
                this.OptionNotify = false;
                this.OptionAlignNewSet = false;
                this.OptionAlignOutput = false;
                this.OptionNewSet = false;
                this.OptionScheduled = false;
                this.OptionKeepFiles = false;
                this.OptionResetOutput = false;
            }

            public void Load(XElement obj)
            {
                if (base.PreLoad(obj)) return;
                XElement statsout = obj.Element(StatisticsOutputSettings.RootName);
                IEnumerable<XAttribute> attribs = statsout.Attributes();
                foreach (XAttribute attrib in attribs)
                {
                    uint interval = OmniScript.secondsInAHour;
                    OmniScript.IntervalUnits units = OmniScript.IntervalUnits.Seconds;
                    uint intervalNewSet = OmniScript.secondsInADay;
                    OmniScript.IntervalUnits unitsNewSet = OmniScript.IntervalUnits.Seconds;

                    String name = attrib.Name.ToString();
                    switch (name)
                    {
                        case "enabled":
                            this.Enabled = OmniScript.PropBoolean(attrib.Value);
                            break;

                        case "userPath":
                            this.UserPath = attrib.Value;
                            break;

                        case "reportPath":
                            this.ReportPath = attrib.Value;
                            break;

                        case "outputInterval":
                            interval = Convert.ToUInt32(attrib.Value);
                            break;

                        case "outputIntervalUnit":
                            units = (OmniScript.IntervalUnits)Convert.ToUInt32(attrib.Value);
                            break;

                        case "notify":
                            this.OptionNotify = OmniScript.PropBoolean(attrib.Value);
                            break;

                        case "alignOutput":
                            this.OptionAlignOutput = OmniScript.PropBoolean(attrib.Value);
                            break;

                        case "newSetEnabled":
                            this.OptionNewSet = OmniScript.PropBoolean(attrib.Value);
                            break;

                        case "newSetInterval":
                            this.NewSetInterval = Convert.ToUInt32(attrib.Value);
                            break;

                        case "newSetIntervalUnit":
                            unitsNewSet = (OmniScript.IntervalUnits)Convert.ToUInt32(attrib.Value);
                            break;

                        case "scheduled":
                            this.OptionScheduled = OmniScript.PropBoolean(attrib.Value);
                            break;

                        case "alignNewSet":
                            this.OptionAlignNewSet = OmniScript.PropBoolean(attrib.Value);
                            break;

                        case "keep":
                            this.FilesToKeep = Convert.ToUInt32(attrib.Value);
                            break;

                        case "keepEnabled":
                            this.OptionKeepFiles = OmniScript.PropBoolean(attrib.Value);
                            break;

                        case "ResetOutput":
                            this.OptionResetOutput = OmniScript.PropBoolean(attrib.Value);
                            break;

                        case "outputType":
                            this.ReportType = Convert.ToInt32(attrib.Value);
                            break;
                    }
                    this.Interval = interval * OmniScript.IntervalMultiplier[(int)units];
                    this.NewSetInterval = intervalNewSet * OmniScript.IntervalMultiplier[(int)unitsNewSet];
                }
            }

            public XElement Store()
            {
                XElement obj = base.CreateObj();
                if (!this.Empty)
                {
                    this.Store(obj);
                }
                return obj;
            }

            public void Store(XElement obj)
            {
                obj.Add(new XElement(StatisticsOutputSettings.RootName,
                    new XAttribute("enabled", BooleanAttribute(this.Enabled)),
                    new XAttribute("userPath", this.UserPath),
                    new XAttribute("reportPath", this.ReportPath),
                    new XAttribute("outputInterval", this.Interval.ToString()),
                    new XAttribute("outputIntervalUnit", "1"),
                    new XAttribute("notify", BooleanAttribute(this.OptionNotify)),
                    new XAttribute("alignOutput", BooleanAttribute(this.OptionAlignOutput)),
                    new XAttribute("newSetEnabled", BooleanAttribute(this.OptionNewSet)),
                    new XAttribute("newSetInterval", this.NewSetInterval.ToString()),
                    new XAttribute("newSetIntervalUnit", "1"),
                    new XAttribute("scheduled", BooleanAttribute(this.OptionScheduled)),
                    new XAttribute("alignNewSet", BooleanAttribute(this.OptionAlignNewSet)),
                    new XAttribute("keep", this.FilesToKeep.ToString()),
                    new XAttribute("keepEnabled", BooleanAttribute(this.OptionKeepFiles)),
                    new XAttribute("ResetOutput", BooleanAttribute(this.OptionResetOutput)),
                    new XAttribute("outputType", this.ReportType.ToString())));
            }
        };

        public class StatisticsOutputPreferencesSettings : CaptureProperty
        {
            public class NodeStats
            {
                public bool Enabled { get; set; }
                public bool OptionDetails { get; set; }
                public bool OptionHierarchy { get; set; }
                public bool OptionName { get; set; }
                public bool OptionPackets { get; set; }
                public bool OptionFirstTime { get; set; }
                public bool OptionLastTime { get; set; }
                public bool OptionBroadcastBytes { get; set; }
                public bool OptionBroadcastPackets { get; set; }
                public bool OptionMulticastBytes { get; set; }
                public bool OptionMulticastPackets { get; set; }
                public bool OptionMinimumSize { get; set; }
                public bool OptionMaximumSize { get; set; }
                public bool OptionPhysical { get; set; }
                public bool OptionIP { get; set; }
                public bool OptionIPv6 { get; set; }
                public bool OptionDEC { get; set; }
                public bool OptionApple { get; set; }
                public bool OptionIPX { get; set; }

                public NodeStats()
                {
                    this.Enabled = false;
                    this.OptionDetails = true;
                    this.OptionHierarchy = true;
                    this.OptionName = true;
                    this.OptionPackets = true;
                    this.OptionFirstTime = true;
                    this.OptionLastTime = true;
                    this.OptionBroadcastBytes = true;
                    this.OptionBroadcastPackets = true;
                    this.OptionMulticastBytes = true;
                    this.OptionMulticastPackets = true;
                    this.OptionMinimumSize = true;
                    this.OptionMaximumSize = true;
                    this.OptionPhysical = true;
                    this.OptionIP = true;
                    this.OptionIPv6 = true;
                    this.OptionDEC = true;
                    this.OptionApple = true;
                    this.OptionIPX = true;
                }

                public void Load(XElement node)
                {
                    if (node == null) return;
                    IEnumerable<XAttribute> attribs = node.Attributes();
                    foreach (XAttribute attrib in attribs)
                    {
                        String name = attrib.Name.ToString();
                        switch (name)
                        {
	                        case "OutputNodeStats":
		                        this.Enabled = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "OutputNodeDetails":
                                this.OptionDetails = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "OutputNodeHierarchy":
                                this.OptionHierarchy = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "NodeName":
                                this.OptionName = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "NodePackets":
                                this.OptionPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "NodeFirstTime":
                                this.OptionFirstTime = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "NodeLastTime":
                                this.OptionLastTime = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "NodeBroadBytes":
                                this.OptionBroadcastBytes = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "NodeBroadPackets":
                                this.OptionBroadcastPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "NodeMultiBytes":
                                this.OptionMulticastBytes = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "NodeMultiPackets":
                                this.OptionMulticastPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "NodeMinSize":
                                this.OptionMinimumSize = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "NodeMaxSize":
                                this.OptionMaximumSize = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "NodeTypePhysical":
                                this.OptionPhysical = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "NodeTypeIP":
                                this.OptionIP = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "NodeTypeIPv6":
                                this.OptionIPv6 = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "NodeTypeDEC":
                                this.OptionDEC = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "NodeTypeApple":
                                this.OptionApple = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "NodeTypeIPX":
                                this.OptionIPX = OmniScript.PropBoolean(attrib.Value);
		                        break;
                        }
                    }
                }

                public XElement Store()
                {
                    XElement nodeStats = new XElement("NodeStats",
                        new XAttribute("OutputNodeStats", BooleanAttribute(this.Enabled)),
                        new XAttribute("OutputNodeDetails", BooleanAttribute(this.OptionDetails)),
                        new XAttribute("OutputNodeHierarchy", BooleanAttribute(this.OptionHierarchy)),
                        new XAttribute("NodeName", BooleanAttribute(this.OptionName)),
                        new XAttribute("NodePackets", BooleanAttribute(this.OptionPackets)),
                        new XAttribute("NodeFirstTime", BooleanAttribute(this.OptionFirstTime)),
                        new XAttribute("NodeLastTime", BooleanAttribute(this.OptionLastTime)),
                        new XAttribute("NodeBroadBytes", BooleanAttribute(this.OptionBroadcastBytes)),
                        new XAttribute("NodeBroadPackets", BooleanAttribute(this.OptionBroadcastPackets)),
                        new XAttribute("NodeMultiBytes", BooleanAttribute(this.OptionMulticastBytes)),
                        new XAttribute("NodeMultiPackets", BooleanAttribute(this.OptionMulticastPackets)),
                        new XAttribute("NodeMinSize", BooleanAttribute(this.OptionMinimumSize)),
                        new XAttribute("NodeMaxSize", BooleanAttribute(this.OptionMaximumSize)),
                        new XAttribute("NodeTypePhysical", BooleanAttribute(this.OptionPhysical)),
                        new XAttribute("NodeTypeIP", BooleanAttribute(this.OptionIP)),
                        new XAttribute("NodeTypeIPv6", BooleanAttribute(this.OptionIPv6)),
                        new XAttribute("NodeTypeDEC", BooleanAttribute(this.OptionDEC)),
                        new XAttribute("NodeTypeApple", BooleanAttribute(this.OptionApple)),
                        new XAttribute("NodeTypeIPX", BooleanAttribute(this.OptionIPX)));
                    return nodeStats;
                }

                public void Store(XElement node)
                {
                    node.Add(this.Store());
                }
            }

            public class ProtocolStats
            {
	            public bool Enabled { get; set; }
                public bool OptionPackets { get; set; }
                public bool OptionPath { get; set; }

                public ProtocolStats()
                {
	                this.Enabled = false;
                    this.OptionPackets = true;
                    this.OptionPath = true;
                }

                public void Load(XElement node)
                {
                    if (node == null) return;
                    IEnumerable<XAttribute> attribs = node.Attributes();
                    foreach (XAttribute attrib in attribs)
                    {
                        String name = attrib.Name.ToString();
                        switch (name)
                        {
	                        case "OutputProtocolStats":
		                        this.Enabled = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "ProtoPackets":
                                this.OptionPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "ProtoPath":
                                this.OptionPath = OmniScript.PropBoolean(attrib.Value);
		                        break;
                        }
                    }
                }

                public void Store(XElement node)
                {
                    node.Add(new XElement("ProtocolStats",
                        new XAttribute("OutputProtocolStats", BooleanAttribute(this.Enabled)),
                        new XAttribute("ProtoPackets", BooleanAttribute(this.OptionPackets)),
                        new XAttribute("ProtoPath", BooleanAttribute(this.OptionPath))));
                }
            }

            public class SummaryStats
            {
	            public bool Enabled { get; set; }
                public bool OptionPackets { get; set; }
                public bool OptionPctPackets { get; set; }
                public bool OptionPctBytes { get; set; }
                public bool OptionPacketsPerSecond { get; set; }
                public bool OptionBytesPerSecond { get; set; }

                public SummaryStats()
                {
	                this.Enabled = false;
                    this.OptionPackets = true;
                    this.OptionPctPackets = true;
                    this.OptionPctBytes = true;
                    this.OptionPacketsPerSecond = true;
                    this.OptionBytesPerSecond = true;
                }

                public void Load(XElement node)
                {
                    if (node == null) return;
                    IEnumerable<XAttribute> attribs = node.Attributes();
                    foreach (XAttribute attrib in attribs)
                    {
                        String name = attrib.Name.ToString();
                        switch (name)
                        {
	                        case "OutputSnapshots":
		                        this.Enabled = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "SumPackets":
                                this.OptionPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "SumPctPackets":
                                this.OptionPctPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "SumPctBytes":
                                this.OptionPctBytes = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "SumPacketspSec":
                                this.OptionPacketsPerSecond = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "SumBytespSec":
                                this.OptionBytesPerSecond = OmniScript.PropBoolean(attrib.Value);
		                        break;
                        }
                    }
                }

                public void Store(XElement node)
                {
                    node.Add(new XElement("SummaryStats",
                        new XAttribute("OutputSnapshots", BooleanAttribute(this.Enabled)),
                        new XAttribute("SumPackets", BooleanAttribute(this.OptionPackets)),
                        new XAttribute("SumPctPackets", BooleanAttribute(this.OptionPctPackets)),
                        new XAttribute("SumPctBytes", BooleanAttribute(this.OptionPctBytes)),
                        new XAttribute("SumPacketspSec", BooleanAttribute(this.OptionPacketsPerSecond)),
                        new XAttribute("SumBytespSec", BooleanAttribute(this.OptionBytesPerSecond))));
                }
            }

            public class ExpertStats
            {
	            public bool Enabled { get; set; }
                public bool OptionFlows { get; set; }
                public bool OptionApplications { get; set; }

                public ExpertStats()
                {
                    this.Enabled = false;
                    this.OptionFlows = true;
                    this.OptionApplications = true;
                }

                public void Load(XElement node)
                {
                    if (node == null) return;
                    IEnumerable<XAttribute> attribs = node.Attributes();
                    foreach (XAttribute attrib in attribs)
                    {
                        String name = attrib.Name.ToString();
                        switch (name)
                        {
	                        case "OutputExpertStats":
		                        this.Enabled = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "ExpertFlows":
                                this.OptionFlows = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "ExpertApplications":
                                this.OptionApplications = OmniScript.PropBoolean(attrib.Value);
		                        break;
                        }
                    }
                }

                public void Store(XElement node)
                {
                    node.Add(new XElement("ExpertStats",
                        new XAttribute("OutputExpertStats", BooleanAttribute(this.Enabled)),
                        new XAttribute("ExpertFlows", BooleanAttribute(this.OptionFlows)),
                        new XAttribute("ExpertApplications", BooleanAttribute(this.OptionApplications))));
                }
            }

            public class WLANStats
            {
	            public bool Enabled { get; set; }
                public bool OptionName { get; set; }
                public bool OptionType { get; set; }
                public bool OptionFirstTimeSent { get; set; }
                public bool OptionLastTimeSent { get; set; }
                public bool OptionFirstTimeReceived { get; set; }
                public bool OptionLastTimeReceived { get; set; }
                public bool OptionPacketsSent { get; set; }
                public bool OptionPacketsReceived { get; set; }
                public bool OptionBroadcastPackets { get; set; }
                public bool OptionBroadcastBytes { get; set; }
                public bool OptionMulticastPackets { get; set; }
                public bool OptionMulticastBytes { get; set; }
                public bool OptionMinimumSizeSent { get; set; }
                public bool OptionMaximumSizeSent { get; set; }
                public bool OptionAverageSizeSent { get; set; }
                public bool OptionMaximimSizeReceived { get; set; }
                public bool OptionMinimumSizeReceived { get; set; }
                public bool OptionAverageSizeReceived { get; set; }
                public bool OptionRetryPackets { get; set; }
                public bool OptionBeaconPackets { get; set; }
                public bool OptionWEPPackets { get; set; }
                public bool OptionICVErrorPackets { get; set; }
                public bool OptionESSID { get; set; }
                public bool OptionChannel { get; set; }
                public bool OptionWEPKey { get; set; }
                public bool OptionSignalMinimum { get; set; }
                public bool OptionSignalMaximum { get; set; }
                public bool OptionSignalCurrent { get; set; }
                public bool OptionSignalMinimumdBm { get; set; }
                public bool OptionSignalMaximumdBm { get; set; }
                public bool OptionSignalCurrentdBm { get; set; }
                public bool OptionNoiseMinimum { get; set; }
                public bool OptionNoiseMaximum { get; set; }
                public bool OptionNoiseCurrent { get; set; }
                public bool OptionNoiseMinimumdBm { get; set; }
                public bool OptionNoiseMaximumdBm { get; set; }
                public bool OptionNoiseCurrentdBm { get; set; }
                public bool OptionDuration { get; set; }
                public bool OptionEncryption { get; set; }
                public bool OptionAuth { get; set; }
                public bool OptionPrivacy { get; set; }
                public bool OptionBeaconESSID { get; set; }
                public bool OptionAssociations { get; set; }
                public bool OptionTrust { get; set; }

                public WLANStats()
                {
	                this.Enabled = false;
	                this.OptionName = true;
	                this.OptionType = true;
	                this.OptionFirstTimeSent = true;
	                this.OptionLastTimeSent = true;
	                this.OptionFirstTimeReceived = true;
	                this.OptionLastTimeReceived = true;
	                this.OptionPacketsSent = true;
	                this.OptionPacketsReceived = true;
	                this.OptionBroadcastPackets = true;
	                this.OptionBroadcastBytes = true;
	                this.OptionMulticastPackets = true;
	                this.OptionMulticastBytes = true;
	                this.OptionMinimumSizeSent = true;
	                this.OptionMaximumSizeSent = true;
	                this.OptionAverageSizeSent = true;
	                this.OptionMaximimSizeReceived = true;
	                this.OptionMinimumSizeReceived = true;
	                this.OptionAverageSizeReceived = true;
	                this.OptionRetryPackets = true;
	                this.OptionBeaconPackets = true;
	                this.OptionWEPPackets = true;
	                this.OptionICVErrorPackets = true;
	                this.OptionESSID = true;
	                this.OptionChannel = true;
	                this.OptionWEPKey = true;
	                this.OptionSignalMinimum = true;
	                this.OptionSignalMaximum = true;
	                this.OptionSignalCurrent = true;
	                this.OptionSignalMinimumdBm = true;
	                this.OptionSignalMaximumdBm = true;
	                this.OptionSignalCurrentdBm = true;
	                this.OptionNoiseMinimum = true;
	                this.OptionNoiseMaximum = true;
	                this.OptionNoiseCurrent = true;
	                this.OptionNoiseMinimumdBm = true;
	                this.OptionNoiseMaximumdBm = true;
	                this.OptionNoiseCurrentdBm = true;
	                this.OptionDuration = true;
	                this.OptionEncryption = true;
	                this.OptionAuth = true;
	                this.OptionPrivacy = true;
	                this.OptionBeaconESSID = true;
	                this.OptionAssociations = true;
	                this.OptionTrust = true;
                }

                public void Load(XElement node)
                {
                    if (node == null) return;
                    IEnumerable<XAttribute> attribs = node.Attributes();
                    foreach (XAttribute attrib in attribs)
                    {
                        String name = attrib.Name.ToString();
                        switch (name)
                        {
	                        case "OutputWLANStats":
		                        this.Enabled = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANName":
		                        this.OptionName = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANType":
		                        this.OptionType = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANFirstTimeSent":
		                        this.OptionFirstTimeSent = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANLastTimeSent":
		                        this.OptionLastTimeSent = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANFirstTimeReceived":
		                        this.OptionFirstTimeReceived = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANLastTimeReceived":
		                        this.OptionLastTimeReceived = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANPacketsSent":
		                        this.OptionPacketsSent = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANPacketsReceived":
		                        this.OptionPacketsReceived = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANBroadcastPackets":
		                        this.OptionBroadcastPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANBroadcastBytes":
		                        this.OptionBroadcastBytes = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANMulticastPackets":
		                        this.OptionMulticastPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANMulticastBytes":
		                        this.OptionMulticastBytes = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANMinSizeSent":
		                        this.OptionMaximumSizeSent = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANMaxSizeSent":
		                        this.OptionMaximumSizeSent = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANAvgSizeSent":
		                        this.OptionAverageSizeSent = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANMaxSizeReceived":
		                        this.OptionMaximimSizeReceived = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANMinSizeReceived":
		                        this.OptionMinimumSizeReceived = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANAvgSizeReceived":
		                        this.OptionAverageSizeReceived = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANRetryPackets":
		                        this.OptionRetryPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANBeaconPackets":
		                        this.OptionBeaconPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANWEPPackets":
		                        this.OptionWEPPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANICVErrorPackets":
		                        this.OptionICVErrorPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANESSID":
		                        this.OptionESSID = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANChannel":
		                        this.OptionChannel = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANWEPKey":
		                        this.OptionWEPKey = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANSignalMin":
		                        this.OptionSignalMinimum = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANSignalMax":
		                        this.OptionSignalMaximum = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANSignalCur":
		                        this.OptionSignalCurrent = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANSignalMindBm":
		                        this.OptionSignalMinimumdBm = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANSignalMaxdBm":
		                        this.OptionSignalMaximumdBm = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANSignalCurdBm":
		                        this.OptionSignalCurrentdBm = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANNoiseMin":
		                        this.OptionNoiseMinimum = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANNoiseMax":
		                        this.OptionNoiseMaximum = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANNoiseCur":
		                        this.OptionNoiseCurrent = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANNoiseMindBm":
		                        this.OptionNoiseCurrentdBm = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANNoiseMaxdBm":
		                        this.OptionNoiseMaximumdBm = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANNoiseCurdBm":
		                        this.OptionNoiseCurrentdBm = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANDuration":
		                        this.OptionDuration = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANEncryption":
		                        this.OptionEncryption = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANAuth":
		                        this.OptionAuth = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANPrivacy":
		                        this.OptionPrivacy = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANBeaconESSID":
		                        this.OptionBeaconESSID = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANAssociations":
		                        this.OptionAssociations = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WLANTrust":
		                        this.OptionTrust = OmniScript.PropBoolean(attrib.Value);
		                        break;
                        }
                    }
                }

                public void Store(XElement node)
                {
                    node.Add(new XElement("WLANStats",
                        new XAttribute("OutputWLANStats", BooleanAttribute(this.Enabled)),
                        new XAttribute("WLANName", BooleanAttribute(this.OptionName)),
                        new XAttribute("WLANType", BooleanAttribute(this.OptionType)),
                        new XAttribute("WLANFirstTimeSent", BooleanAttribute(this.OptionFirstTimeSent)),
                        new XAttribute("WLANLastTimeSent", BooleanAttribute(this.OptionLastTimeSent)),
                        new XAttribute("WLANFirstTimeReceived", BooleanAttribute(this.OptionFirstTimeReceived)),
                        new XAttribute("WLANLastTimeReceived", BooleanAttribute(this.OptionLastTimeReceived)),
                        new XAttribute("WLANPacketsSent", BooleanAttribute(this.OptionPacketsSent)),
                        new XAttribute("WLANPacketsReceived", BooleanAttribute(this.OptionPacketsReceived)),
                        new XAttribute("WLANBroadcastPackets", BooleanAttribute(this.OptionBroadcastPackets)),
                        new XAttribute("WLANBroadcastBytes", BooleanAttribute(this.OptionBroadcastBytes)),
                        new XAttribute("WLANMulticastPackets", BooleanAttribute(this.OptionMulticastPackets)),
                        new XAttribute("WLANMulticastBytes", BooleanAttribute(this.OptionMulticastBytes)),
                        new XAttribute("WLANMinSizeSent", BooleanAttribute(this.OptionMaximumSizeSent)),
                        new XAttribute("WLANMaxSizeSent", BooleanAttribute(this.OptionMaximumSizeSent)),
                        new XAttribute("WLANAvgSizeSent", BooleanAttribute(this.OptionAverageSizeSent)),
                        new XAttribute("WLANMaxSizeReceived", BooleanAttribute(this.OptionMaximimSizeReceived)),
                        new XAttribute("WLANMinSizeReceived", BooleanAttribute(this.OptionMinimumSizeReceived)),
                        new XAttribute("WLANAvgSizeReceived", BooleanAttribute(this.OptionAverageSizeReceived)),
                        new XAttribute("WLANRetryPackets", BooleanAttribute(this.OptionRetryPackets)),
                        new XAttribute("WLANBeaconPackets", BooleanAttribute(this.OptionBeaconPackets)),
                        new XAttribute("WLANWEPPackets", BooleanAttribute(this.OptionWEPPackets)),
                        new XAttribute("WLANICVErrorPackets", BooleanAttribute(this.OptionICVErrorPackets)),
                        new XAttribute("WLANESSID", BooleanAttribute(this.OptionESSID)),
                        new XAttribute("WLANChannel", BooleanAttribute(this.OptionChannel)),
                        new XAttribute("WLANWEPKey", BooleanAttribute(this.OptionWEPKey)),
                        new XAttribute("WLANSignalMin", BooleanAttribute(this.OptionSignalMinimum)),
                        new XAttribute("WLANSignalMax", BooleanAttribute(this.OptionSignalMaximum)),
                        new XAttribute("WLANSignalCur", BooleanAttribute(this.OptionSignalCurrent)),
                        new XAttribute("WLANSignalMindBm", BooleanAttribute(this.OptionSignalMinimumdBm)),
                        new XAttribute("WLANSignalMaxdBm", BooleanAttribute(this.OptionSignalMaximumdBm)),
                        new XAttribute("WLANSignalCurdBm", BooleanAttribute(this.OptionSignalCurrentdBm)),
                        new XAttribute("WLANNoiseMin", BooleanAttribute(this.OptionNoiseMinimum)),
                        new XAttribute("WLANNoiseMax", BooleanAttribute(this.OptionNoiseMaximum)),
                        new XAttribute("WLANNoiseCur", BooleanAttribute(this.OptionNoiseCurrent)),
                        new XAttribute("WLANNoiseMindBm", BooleanAttribute(this.OptionNoiseCurrentdBm)),
                        new XAttribute("WLANNoiseMaxdBm", BooleanAttribute(this.OptionNoiseMaximumdBm)),
                        new XAttribute("WLANNoiseCurdBm", BooleanAttribute(this.OptionNoiseCurrentdBm)),
                        new XAttribute("WLANDuration", BooleanAttribute(this.OptionDuration)),
                        new XAttribute("WLANEncryption", BooleanAttribute(this.OptionEncryption)),
                        new XAttribute("WLANAuth", BooleanAttribute(this.OptionAuth)),
                        new XAttribute("WLANPrivacy", BooleanAttribute(this.OptionPrivacy)),
                        new XAttribute("WLANBeaconESSID", BooleanAttribute(this.OptionBeaconESSID)),
                        new XAttribute("WLANAssociations", BooleanAttribute(this.OptionAssociations)),
                        new XAttribute("WLANTrust", BooleanAttribute(this.OptionTrust))));
                }
            }

            public class WirelessChannelStats
            {
	            public bool Enabled { get; set; }
                public bool OptionFrequency { get; set; }
                public bool OptionBand { get; set; }
                public bool OptionDataPackets { get; set; }
                public bool OptionDataBytes { get; set; }
                public bool OptionManagementPackets { get; set; }
                public bool OptionManagementBytes { get; set; }
                public bool OptionControlPackets { get; set; }
                public bool OptionControlBytes { get; set; }
                public bool OptionLocalPackets { get; set; }
                public bool OptionLocalBytes { get; set; }
                public bool OptionFromDSPackets { get; set; }
                public bool OptionFromDSBytes { get; set; }
                public bool OptionToDSPackets { get; set; }
                public bool OptionToDSBytes { get; set; }
                public bool OptionDS2DSPackets { get; set; }
                public bool OptionDS2DSBytes { get; set; }
                public bool OptionRetryPackets { get; set; }
                public bool OptionRetryBytes { get; set; }
                public bool OptionProtectedPackets { get; set; }
                public bool OptionProtectedBytes { get; set; }
                public bool OptionOrderPackets { get; set; }
                public bool OptionOrderBytes { get; set; }
                public bool OptionCRCPackets { get; set; }
                public bool OptionICVErrorPackets { get; set; }
                public bool OptionSignalMinimum { get; set; }
                public bool OptionSignalMaximum { get; set; }
                public bool OptionSignalCurrent { get; set; }
                public bool OptionSignalAvgerage { get; set; }
                public bool OptionSignaldBmMinimum { get; set; }
                public bool OptionSignaldBmMaximum { get; set; }
                public bool OptionSignaldBmCurrent { get; set; }
                public bool OptionSignaldBmAvgerage { get; set; }
                public bool OptionNoiseMinimum { get; set; }
                public bool OptionNoiseMaximum { get; set; }
                public bool OptionNoiseCurrent { get; set; }
                public bool OptionNoiseAvgerage { get; set; }
                public bool OptionNoisedBmMinimum { get; set; }
                public bool OptionNoisedBmMaximum { get; set; }
                public bool OptionNoisedBmCurrent { get; set; }
                public bool OptionNoisedBmAvgerage { get; set; }

                public WirelessChannelStats()
                {
	                this.Enabled = false;
	                this.OptionFrequency = true;
	                this.OptionBand = true;
	                this.OptionDataPackets = true;
	                this.OptionDataBytes = true;
	                this.OptionManagementPackets = true;
	                this.OptionManagementBytes = true;
	                this.OptionControlPackets = true;
	                this.OptionControlBytes = true;
	                this.OptionLocalPackets = true;
	                this.OptionLocalBytes = true;
	                this.OptionFromDSPackets = true;
	                this.OptionFromDSBytes = true;
	                this.OptionToDSPackets = true;
	                this.OptionToDSBytes = true;
	                this.OptionDS2DSPackets = true;
	                this.OptionDS2DSBytes = true;
	                this.OptionRetryPackets = true;
	                this.OptionRetryBytes = true;
	                this.OptionProtectedPackets = true;
	                this.OptionProtectedBytes = true;
	                this.OptionOrderPackets = true;
	                this.OptionOrderBytes = true;
	                this.OptionCRCPackets = true;
	                this.OptionICVErrorPackets = true;
	                this.OptionSignalMinimum = true;
	                this.OptionSignalMaximum = true;
	                this.OptionSignalCurrent = true;
	                this.OptionSignalAvgerage = true;
	                this.OptionSignaldBmMinimum = true;
	                this.OptionSignaldBmMaximum = true;
	                this.OptionSignaldBmCurrent = true;
	                this.OptionSignaldBmAvgerage = true;
	                this.OptionNoiseMinimum = true;
	                this.OptionNoiseMaximum = true;
	                this.OptionNoiseCurrent = true;
	                this.OptionNoiseAvgerage = true;
	                this.OptionNoisedBmMinimum = true;
	                this.OptionNoisedBmMaximum = true;
	                this.OptionNoisedBmCurrent = true;
	                this.OptionNoisedBmAvgerage = true;
                }

                public void Load(XElement node)
                {
                    if (node == null) return;
                    IEnumerable<XAttribute> attribs = node.Attributes();
                    foreach (XAttribute attrib in attribs)
                    {
                        String name = attrib.Name.ToString();
                        switch (name)
                        {
	                        case "OutputWChannelsStats":
		                        this.Enabled = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelFrequency":
		                        this.OptionFrequency = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelBand":
		                        this.OptionBand = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelDataPackets":
		                        this.OptionDataPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelDataBytes":
		                        this.OptionDataBytes = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelMngtPackets":
		                        this.OptionManagementPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelMngtBytes":
		                        this.OptionManagementBytes = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelCtrlPackets":
		                        this.OptionControlPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelCtrlBytes":
		                        this.OptionControlBytes = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelLocalPackets":
		                        this.OptionLocalPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelLocalBytes":
		                        this.OptionLocalBytes = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelFromDSPackets":
		                        this.OptionFromDSPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelFromDSBytes":
		                        this.OptionFromDSBytes = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelToDSPackets":
		                        this.OptionToDSPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelToDSBytes":
		                        this.OptionToDSBytes = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelDS2DSPackets":
		                        this.OptionDS2DSPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelDS2DSBytes":
		                        this.OptionDS2DSBytes = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelRetryPackets":
		                        this.OptionRetryPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelRetryBytes":
		                        this.OptionRetryBytes = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelProtectedPackets":
		                        this.OptionProtectedPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelProtectedBytes":
		                        this.OptionProtectedBytes = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelOrderPackets":
		                        this.OptionOrderPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelOrderBytes":
		                        this.OptionOrderBytes = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelCRCPackets":
		                        this.OptionCRCPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelICVErrorPackets":
		                        this.OptionICVErrorPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelSignalMin":
		                        this.OptionSignalMinimum = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelSignalMax":
		                        this.OptionSignalMaximum = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelSignalCur":
		                        this.OptionSignalCurrent = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelSignalAvg":
		                        this.OptionSignalAvgerage = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelSignaldBmMin":
		                        this.OptionSignaldBmMinimum = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelSignaldBmMax":
		                        this.OptionSignaldBmMaximum = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelSignaldBmCur":
		                        this.OptionSignaldBmCurrent = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelSignaldBmAvg":
		                        this.OptionSignaldBmAvgerage = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelNoiseMin":
		                        this.OptionNoiseMinimum = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelNoiseMax":
		                        this.OptionNoiseMaximum = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelNoiseCur":
		                        this.OptionNoiseCurrent = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelNoiseAvg":
		                        this.OptionNoiseAvgerage = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelNoisedBmMin":
		                        this.OptionNoisedBmMinimum = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelNoisedBmMax":
		                        this.OptionNoisedBmMaximum = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelNoisedBmCur":
		                        this.OptionNoisedBmCurrent = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "WChannelNoisedBmAvg":
		                        this.OptionNoisedBmAvgerage = OmniScript.PropBoolean(attrib.Value);
		                        break;
                        }
                    }
                }

                public void Store(XElement node)
                {
                    node.Add(new XElement("WChannelStats",
                        new XAttribute("OutputWChannelsStats", BooleanAttribute(this.Enabled)),
                        new XAttribute("WChannelFrequency", BooleanAttribute(this.OptionFrequency)),
                        new XAttribute("WChannelBand", BooleanAttribute(this.OptionBand)),
                        new XAttribute("WChannelDataPackets", BooleanAttribute(this.OptionDataPackets)),
                        new XAttribute("WChannelDataBytes", BooleanAttribute(this.OptionDataBytes)),
                        new XAttribute("WChannelMngtPackets", BooleanAttribute(this.OptionManagementPackets)),
                        new XAttribute("WChannelMngtBytes", BooleanAttribute(this.OptionManagementBytes)),
                        new XAttribute("WChannelCtrlPackets", BooleanAttribute(this.OptionControlPackets)),
                        new XAttribute("WChannelCtrlBytes", BooleanAttribute(this.OptionControlBytes)),
                        new XAttribute("WChannelLocalPackets", BooleanAttribute(this.OptionLocalPackets)),
                        new XAttribute("WChannelLocalBytes", BooleanAttribute(this.OptionLocalBytes)),
                        new XAttribute("WChannelFromDSPackets", BooleanAttribute(this.OptionFromDSPackets)),
                        new XAttribute("WChannelFromDSBytes", BooleanAttribute(this.OptionFromDSBytes)),
                        new XAttribute("WChannelToDSPackets", BooleanAttribute(this.OptionToDSPackets)),
                        new XAttribute("WChannelToDSBytes", BooleanAttribute(this.OptionToDSBytes)),
                        new XAttribute("WChannelDS2DSPackets", BooleanAttribute(this.OptionDS2DSPackets)),
                        new XAttribute("WChannelDS2DSBytes", BooleanAttribute(this.OptionDS2DSBytes)),
                        new XAttribute("WChannelRetryPackets", BooleanAttribute(this.OptionRetryPackets)),
                        new XAttribute("WChannelRetryBytes", BooleanAttribute(this.OptionRetryBytes)),
                        new XAttribute("WChannelProtectedPackets", BooleanAttribute(this.OptionProtectedPackets)),
                        new XAttribute("WChannelProtectedBytes", BooleanAttribute(this.OptionProtectedBytes)),
                        new XAttribute("WChannelOrderPackets", BooleanAttribute(this.OptionOrderPackets)),
                        new XAttribute("WChannelOrderBytes", BooleanAttribute(this.OptionOrderBytes)),
                        new XAttribute("WChannelCRCPackets", BooleanAttribute(this.OptionCRCPackets)),
                        new XAttribute("WChannelICVErrorPackets", BooleanAttribute(this.OptionICVErrorPackets)),
                        new XAttribute("WChannelSignalMin", BooleanAttribute(this.OptionSignalMinimum)),
                        new XAttribute("WChannelSignalMax", BooleanAttribute(this.OptionSignalMaximum)),
                        new XAttribute("WChannelSignalCur", BooleanAttribute(this.OptionSignalCurrent)),
                        new XAttribute("WChannelSignalAvg", BooleanAttribute(this.OptionSignalAvgerage)),
                        new XAttribute("WChannelSignaldBmMin", BooleanAttribute(this.OptionSignaldBmMinimum)),
                        new XAttribute("WChannelSignaldBmMax", BooleanAttribute(this.OptionSignaldBmMaximum)),
                        new XAttribute("WChannelSignaldBmCur", BooleanAttribute(this.OptionSignaldBmCurrent)),
                        new XAttribute("WChannelSignaldBmAvg", BooleanAttribute(this.OptionSignaldBmAvgerage)),
                        new XAttribute("WChannelNoiseMin", BooleanAttribute(this.OptionNoiseMinimum)),
                        new XAttribute("WChannelNoiseMax", BooleanAttribute(this.OptionNoiseMaximum)),
                        new XAttribute("WChannelNoiseCur", BooleanAttribute(this.OptionNoiseCurrent)),
                        new XAttribute("WChannelNoiseAvg", BooleanAttribute(this.OptionNoiseAvgerage)),
                        new XAttribute("WChannelNoisedBmMin", BooleanAttribute(this.OptionNoisedBmMinimum)),
                        new XAttribute("WChannelNoisedBmMax", BooleanAttribute(this.OptionNoisedBmMaximum)),
                        new XAttribute("WChannelNoisedBmCur", BooleanAttribute(this.OptionNoisedBmCurrent)),
                        new XAttribute("WChannelNoisedBmAvg", BooleanAttribute(this.OptionNoisedBmAvgerage))));
                }
            }

            public class WirelessDataRates
            {
	            public bool Enabled { get; set; }
                public bool OptionPackets { get; set; }
                public bool OptionBytes { get; set; }

                public WirelessDataRates()
                {
	                this.Enabled = false;
                    this.OptionPackets = true;
                    this.OptionBytes = true;
                }

                public void Load(XElement node)
                {
                    if (node == null) return;
                    IEnumerable<XAttribute> attribs = node.Attributes();
                    foreach (XAttribute attrib in attribs)
                    {
                        String name = attrib.Name.ToString();
                        switch (name)
                        {
	                        case "OutputDataRates":
		                        this.Enabled = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "DataRatesPackets":
                                this.OptionPackets = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "DataRatesBytes":
                                this.OptionBytes = OmniScript.PropBoolean(attrib.Value);
		                        break;
                        }
                    }
                }

                public void Store(XElement node)
                {
                    node.Add(new XElement("WirelessDataRates",
                        new XAttribute("OutputDataRates", BooleanAttribute(this.Enabled)),
                        new XAttribute("DataRatesPackets", BooleanAttribute(this.OptionPackets)),
                        new XAttribute("DataRatesBytes", BooleanAttribute(this.OptionBytes))));
                }
            }

            public class Graphs
            {
                public bool Enabled { get; set; }

                public Graphs()
                {
                    this.Enabled = false;
                }

                public void Load(XElement node)
                {
                    if (node == null) return;
                    IEnumerable<XAttribute> attribs = node.Attributes();
                    foreach (XAttribute attrib in attribs)
                    {
                        String name = attrib.Name.ToString();
                        switch (name)
                        {
	                        case "OutputGraphs":
		                        this.Enabled = OmniScript.PropBoolean(attrib.Value);
		                        break;
                        }
                    }
                }

                public void Store(XElement node)
                {
                    node.Add(new XElement("Graphs",
                        new XAttribute("OutputGraphs", BooleanAttribute(this.Enabled))));
                }
            }

            public class AdditionalReports
            {
                public bool OptionAdditionalNode { get; set; }
                public bool OptionConversationsOnly { get; set; }
                public bool OptionExecutiveSummary { get; set; }
                public bool OptionExpertLog { get; set; }
                public bool OptionExpertSummary { get; set; }
                public bool OptionVoIP { get; set; }

                public AdditionalReports()
                {
                    this.OptionAdditionalNode = false;
                    this.OptionConversationsOnly = false;
                    this.OptionExecutiveSummary = false;
                    this.OptionExpertLog = true;
                    this.OptionExpertSummary = false;
                    this.OptionVoIP = true;
                }

                public void Load(XElement node)
                {
                    if (node == null) return;
                    IEnumerable<XAttribute> attribs = node.Attributes();
                    foreach (XAttribute attrib in attribs)
                    {
                        String name = attrib.Name.ToString();
                        switch (name)
                        {
	                        case "OutputAdditionalNode":
		                        this.OptionAdditionalNode = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "OutputConversationsOnly":
		                        this.OptionConversationsOnly = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "OutputExecutiveSummary":
		                        this.OptionExecutiveSummary = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "OutputExpertLog":
		                        this.OptionExpertLog = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "OutputExpertSummary":
		                        this.OptionExpertSummary = OmniScript.PropBoolean(attrib.Value);
		                        break;

	                        case "OutputVoIP":
		                        this.OptionVoIP = OmniScript.PropBoolean(attrib.Value);
		                        break;
                        }
                    }
                }

                public void Store(XElement node)
                {
                    node.Add(new XElement("AdditionalReports",
                        new XAttribute("OutputAdditionalNode", BooleanAttribute(this.OptionAdditionalNode)),
                        new XAttribute("OutputConversationsOnly", BooleanAttribute(this.OptionConversationsOnly)),
                        new XAttribute("OutputExecutiveSummary", BooleanAttribute(this.OptionExecutiveSummary)),
                        new XAttribute("OutputExpertLog", BooleanAttribute(this.OptionExpertLog)),
                        new XAttribute("OutputExpertSummary", BooleanAttribute(this.OptionExpertSummary)),
                        new XAttribute("OutputVoIP", BooleanAttribute(this.OptionVoIP))));
                }
            }

            public const String ObjectName = "StatsOutputPrefs";
            public const String ClassIdName = "StatsOutputPrefs";
            public const String RootName = "StatsOutputPrefs";

            public int ReportType { get; set; }
            public NodeStats Node { get; set; }
            public ProtocolStats Protocol { get; set; }
            public SummaryStats Summary { get; set; }
            public ExpertStats Expert { get; set; }
            public WLANStats WLAN { get; set; }
            public WirelessChannelStats Wireless { get; set; }
            public WirelessDataRates Rates { get; set; }
            public Graphs Graph { get; set; }
            public AdditionalReports Reports { get; set; }

            public StatisticsOutputPreferencesSettings()
                : base(StatisticsOutputPreferencesSettings.ObjectName,
                    StatisticsOutputPreferencesSettings.ClassIdName,
                    StatisticsOutputPreferencesSettings.RootName)
            {
                this.ReportType = 3;
                this.Node = new NodeStats();
                this.Protocol = new ProtocolStats();
                this.Summary = new SummaryStats();
                this.Expert = new ExpertStats();
                this.WLAN = new WLANStats();
                this.Wireless = new WirelessChannelStats();
                this.Rates = new WirelessDataRates();
                this.Graph = new Graphs();
                this.Reports = new AdditionalReports();
                this.Empty = false;
            }

            public void Load(XElement obj)
            {
                if (base.PreLoad(obj)) return;
                XElement outputprefs = obj.Element("StatsOutputPrefs");

                XAttribute attrib = outputprefs.Attribute("ReportType");
                this.ReportType = (attrib != null)
                    ? Convert.ToInt32(attrib.Value)
                    : 0;

                foreach (XElement pref in outputprefs.Elements())
                {
                    String name = pref.Name.ToString();
                    switch (name)
                    {
                        case "NodeStats":
                            this.Node.Load(pref);
                            break;

                        case "ProtocolStats":
                            this.Protocol.Load(pref);
                            break;

                        case "SummaryStats":
                            this.Summary.Load(pref);
                            break;

                        case "ExpertStats":
                            this.Expert.Load(pref);
                            break;

                        case "WLANStats":
                            this.WLAN.Load(pref);
                            break;

                        case "WChannelsStats":
                            this.Wireless.Load(pref);
                            break;

                        case "WirelessDataRates":
                            this.Rates.Load(pref);
                            break;

                        case "Graphs":
                            this.Graph.Load(pref);
                            break;

                        case "AdditionalReports":
                            this.Reports.Load(pref);
                            break;
                    }
                }
            }

            public XElement Store()
            {
                XElement obj = base.CreateObj();
                if (!this.Empty)
                {
                    this.Store(obj);
                }
                return obj;
            }

            public void Store(XElement obj)
            {
                XElement outputprefs = new XElement("StatsOutputPrefs",
                    new XAttribute("ReportType", this.ReportType.ToString()));

                this.Node.Store(outputprefs);
                this.Protocol.Store(outputprefs);
                this.Summary.Store(outputprefs);
                this.Expert.Store(outputprefs);
                this.WLAN.Store(outputprefs);
                this.Wireless.Store(outputprefs);
                this.Rates.Store(outputprefs);
                this.Graph.Store(outputprefs);
                this.Reports.Store(outputprefs);

                obj.Add(outputprefs);
            }
        };

        public class TriggerSettings : CaptureProperty
        {
            public class DateLimit
            {
                static public readonly String TagName;
                static public readonly String ClassIdName;
                static public readonly String RootName;
                static public readonly OmniId ClassId;

                static DateLimit()
                {
                    TagName = "triggereventobj";
                    ClassIdName = "TimeTriggerEvent";
                    RootName = "triggerevent";
                    ClassId = OmniScript.ClassNameIds[DateLimit.ClassIdName];
                }

                public bool Enabled { get; set; }
                public bool OptionDate { get; set; }
                public bool OptionElapsedTime { get; set; }
                public PeekTime Time { get; set; }

                public DateLimit()
                {
                    this.Enabled = false;
                    this.OptionDate = false;
                    this.OptionElapsedTime = false;
                    this.Time = null;
                }

                public void Load(XElement node)
                {
                    if (node == null) return;
                    
                    XElement triggerSettings = node.Element(DateLimit.RootName);
                    if (triggerSettings == null) return;

                    this.Enabled = OmniScript.PropIsEnabled(triggerSettings);
                    this.OptionDate = OmniScript.PropBoolean(triggerSettings.Attribute("usedate").Value);
                    this.OptionElapsedTime = OmniScript.PropBoolean(triggerSettings.Attribute("useelapsedtime").Value);
                    XAttribute time = triggerSettings.Attribute("time");
                    this.Time = (time != null) ? new PeekTime(Convert.ToUInt64(time.Value)) : null;
                }

                public XElement Store()
                {
                    XElement vent = new XElement(DateLimit.RootName,
                        new XAttribute("enabled", BooleanAttribute(this.Enabled)),
                        new XAttribute("usedate", BooleanAttribute(this.OptionDate)),
                        new XAttribute("useelapsedtime", BooleanAttribute(this.OptionElapsedTime)),
                        new XAttribute("time", (this.Time != null) ? this.Time.ToString() : "0"));
                    XElement TriggerSettings = new XElement(DateLimit.TagName,
                        new XAttribute("clsid", DateLimit.ClassId),
                        vent);
                    return TriggerSettings;
                }

                public void Store(XElement node)
                {
                    node.Add(this.Store());
                }
            }

            public class FilterLimit
            {
                public class FilterPack
                {
                    static public readonly String TagName;
                    static public readonly String ClassIdName;
                    static public readonly String RootName;
                    static public readonly OmniId ClassId;

                    static FilterPack()
                    {
                        TagName = "packetfilterobj";
                        ClassIdName = "FilterPack";
                        RootName = "filterpack";
                        ClassId = OmniScript.ClassNameIds[FilterPack.ClassIdName];
                    }

                    public List<OmniId> FilterList { get; set; }

                    public FilterPack()
                    {
                        this.FilterList = new List<OmniId>();
                    }

                    public FilterPack(XElement obj)
                        : this()
                    {
                        this.Load(obj);
                    }

                    public void Load(XElement obj)
                    {
                        XElement root = obj.Element(FilterPack.RootName);
                        IEnumerable<XElement> items = obj.Elements("item");
                        foreach (XElement item in items)
                        {
                            XAttribute id = item.Attribute("id");
                            if (id != null)
                            {
                                this.FilterList.Add(Guid.Parse(id.Value));
                            }
                        }
                    }

                    public XElement Store()
                    {
                        XElement pack = new XElement(FilterPack.RootName);
                        foreach (Guid id in this.FilterList)
                        {
                            pack.Add(new XElement("item",
                                new XAttribute("id", id.ToString())));
                        }
                        return new XElement(FilterPack.TagName,
                            new XAttribute("clsid", FilterPack.ClassId.ToString()),
                            pack);
                    }
                }

                static public readonly String TagName;
                static public readonly String ClassIdName;
                static public readonly String RootName;
                static public readonly OmniId ClassId;

                static FilterLimit()
                {
                    TagName = "triggereventobj";
                    ClassIdName = "FilterTriggerEvent";
                    RootName = "triggerevent";
                    ClassId = OmniScript.ClassNameIds[FilterLimit.ClassIdName];
                }

                public bool Enabled { get; set; }
                public uint Mode { get; set; }
                public FilterPack Filters { get; set; }

                public FilterLimit()
                {
                    this.Enabled = false;
                    this.Mode = 0;
                    this.Filters = null;
                }

                public void Load(XElement node)
                {
                    if (node == null) return;

                    XElement triggerSettings = node.Element(FilterLimit.RootName);
                    if (triggerSettings == null) return;
                    
                    this.Enabled = OmniScript.PropIsEnabled(triggerSettings);
                    XAttribute mode = triggerSettings.Attribute("mode");
                    this.Mode = (mode != null) ? Convert.ToUInt32(mode.Value) : 0;
                    XElement pack = triggerSettings.Element(FilterPack.TagName);
                    if (pack != null)
                    {
                        this.Filters = new FilterPack(pack);
                    }
                }

                public XElement Store()
                {
                    XElement limit = new XElement(FilterLimit.RootName,
                        new XAttribute("enabled", BooleanAttribute(this.Enabled)),
                        new XAttribute("mode", this.Mode.ToString()));
                    if (this.Filters != null)
                    {
                        limit.Add(this.Filters.Store());
                    }
                    return new XElement(FilterLimit.TagName,
                        new XAttribute("clsid", FilterLimit.ClassId),
                        limit);
                }

                public void Store(XElement node)
                {
                    node.Add(this.Store());
                }
            }

            public class CaptureLimit
            {
                static public readonly String TagName;
                static public readonly String ClassIdName;
                static public readonly String RootName;
                static public readonly OmniId ClassId;

                static CaptureLimit()
                {
                    TagName = "triggereventobj";
                    ClassIdName = "BytesCapturedTriggerEvent";
                    RootName = "triggerevent";
                    ClassId = OmniScript.ClassNameIds[CaptureLimit.ClassIdName];
                }

                public bool Enabled { get; set; }
                public ulong Bytes { get; set; }

                public CaptureLimit()
                {
                    this.Enabled = false;
                    this.Bytes = 0;
                }

                public void Load(XElement node)
                {
                    if (node == null) return;

                    XElement triggerSettings = node.Element(CaptureLimit.RootName);
                    if (triggerSettings == null) return;
                    
                    this.Enabled = OmniScript.PropIsEnabled(triggerSettings);
                    XAttribute bytes = triggerSettings.Attribute("bytescaptured");
                    this.Bytes = (bytes != null) ? Convert.ToUInt32(bytes.Value) : 0;
                }

                public XElement Store()
                {
                    XElement vent = new XElement(CaptureLimit.RootName,
                        new XAttribute("enabled", BooleanAttribute(this.Enabled)),
                        new XAttribute("bytescaptured", this.Bytes.ToString()));
                    XElement TriggerSettings = new XElement(CaptureLimit.TagName,
                        new XAttribute("clsid", CaptureLimit.ClassId),
                        vent);
                    return TriggerSettings;
                }

                public void Store(XElement node)
                {
                    node.Add(this.Store());
                }
            }

            public const String ClassIdName = "Trigger";
            public const String RootName = "trigger";

            public String Name { get; set; }
            public bool Enabled { get; set; }
            public uint Severity { get; set; }
            public CaptureLimit Captured { get; set; }
            public FilterLimit Filter { get; set; }
            public DateLimit Time { get; set; }
            public bool OptionNotify { get; set; }
            public bool OptionToggleCapture { get; set; }

            public TriggerSettings(String name)
                : base(name, TriggerSettings.ClassIdName,
                    TriggerSettings.RootName)
            {
                this.Name = name;
                this.Enabled = false;
                this.Severity = 0;
                this.Captured = new CaptureLimit();
                this.Filter = new FilterLimit();
                this.Time = new DateLimit();
                this.OptionNotify = true;
                this.OptionToggleCapture = true;
                this.Empty = false;
            }

            public void Load(XElement obj)
            {
                if (base.PreLoad(obj)) return;

                XElement trigger = obj.Element(TriggerSettings.RootName);
                if (trigger == null) return;

                this.Enabled = OmniScript.PropIsEnabled(trigger);
                XAttribute severity = trigger.Attribute("severity");
                this.Severity = (severity != null) ? Convert.ToUInt32(severity.Value) : 0;
                XAttribute notify = trigger.Attribute("notify");
                this.OptionNotify = (notify != null) ? OmniScript.PropBoolean(notify.Value) : false;
                this.OptionToggleCapture = OmniScript.PropBoolean(trigger.Attribute("togglecapture").Value);

                XElement triggerEvents = trigger.Element("triggerevents");
                if (triggerEvents != null)
                {
                    IEnumerable<XElement> events = triggerEvents.Elements("triggereventobj");
                    foreach (XElement vent in events)
                    {
                        XAttribute clsid = vent.Attribute("clsid");
                        if (clsid != null)
                        {
                            Guid id = Guid.Parse(clsid.Value);
                            if (id == CaptureLimit.ClassId)
                            {
                                this.Captured = new CaptureLimit();
                                this.Captured.Load(vent);
                            }
                            else if (id == FilterLimit.ClassId)
                            {
                                this.Filter = new FilterLimit();
                                this.Filter.Load(vent);
                            }
                            else if (id == DateLimit.ClassId)
                            {
                                this.Time = new DateLimit();
                                this.Time.Load(vent);
                            }
                        }
                    }
                }
            }

            public XElement Store()
            {
                XElement obj = base.CreateObj();
                if (!this.Empty)
                {
                    XElement triggerSettings = new XElement(TriggerSettings.RootName,
                        new XAttribute("enabled", BooleanAttribute(this.Enabled)),
                        new XAttribute("notify", BooleanAttribute(this.OptionNotify)),
                        new XAttribute("severity", this.Severity.ToString()),
                        new XAttribute("togglecapture", BooleanAttribute(this.OptionToggleCapture)));
                    XElement events = new XElement("triggerevents");
                    if (this.Captured != null)
                    {
                        this.Captured.Store(events);
                    }
                    if (this.Filter != null)
                    {
                        this.Filter.Store(events);
                    }
                    if (this.Time != null)
                    {
                        this.Time.Store(events);
                    }

                    if (events.HasElements)
                    {
                        triggerSettings.Add(events);
                    }
                    obj.Add(triggerSettings);
                }
                return obj;
            }

            public void Store(XElement obj)
            {
                obj.Add(this.Store());
            }
        };

        public class VoIPSettings : CaptureProperty
        {
            public const String ObjectName = "VoIPConfig";
            public const String ClassIdName = "SimplePropBag";
            public const String RootName = "properties";

            public uint MaxCalls { get; set; }
            public uint Severity { get; set; }
            public bool OptionNotify { get; set; }
            public bool OptionStopAnalysis { get; set; }

            public VoIPSettings()
                : base(VoIPSettings.ObjectName, VoIPSettings.ClassIdName,
                    VoIPSettings.RootName)
            {
                this.MaxCalls = 2000;
                this.Severity = 3;
                this.OptionNotify = true;
                this.OptionStopAnalysis = true;
                this.Empty = false;
            }

            public void Load(XElement obj)
            {
                if (base.PreLoad(obj)) return;
                IEnumerable<XElement> props = obj.Element(VoIPSettings.RootName).Elements("prop");
                foreach (XElement prop in props)
                {
                    String name = prop.Attribute("name").Value.ToString();
                    switch (name)
                    {
                        case "MaxCalls":
                            this.MaxCalls = Convert.ToUInt32(prop.Value);
                            break;

                        case "Notify":
                            this.OptionNotify = OmniScript.PropBoolean(prop.Value);
                            break;

                        case "Severity":
                            this.Severity = Convert.ToUInt32(prop.Value);
                            break;

                        case "StopAnalysis":
                            this.OptionStopAnalysis = OmniScript.PropBoolean(prop.Value);
                            break;
                    }
                }
            }

            public XElement Store()
            {
                XElement obj = base.CreateObj();
                if (!this.Empty)
                {
                    this.Store(obj);
                }
                return obj;
            }

            public void Store(XElement obj)
            {
                obj.Add(new XElement(VoIPSettings.RootName,
                    OmniScript.Prop("MaxCalls", 19, this.MaxCalls.ToString()),
                    OmniScript.Prop("Notify", this.OptionNotify),
                    OmniScript.Prop("Severity", 19, this.Severity.ToString()),
                    OmniScript.Prop("StopAnalysis", this.OptionStopAnalysis)));
            }
        };

        public const String ObjectName = "CaptureTemplate";
        public const String ClassIdName = tagPropBag;
        public const String RootName = tagProps;

        private const String tagAdapterSettings = "AdapterSettings";

        public String FileName { get; set; }
        public AdapterSettings Adapter { get; set; }
        public AlarmSettings Alarms { get; set; }
        public AnalysisSettings Analysis { get; set; }
        public FilterSettings Filter { get; set; }
        public GeneralSettings General { get; set; }
        public GraphsSettings Graphs { get; set; }
        public PluginsSettings Plugins { get; set; }
        public StatisticsOutputSettings StatisticsOutput { get; set; }
        public StatisticsOutputPreferencesSettings StatisticsOutputPreferences { get; set; }
        public RepeatTriggerSettings RepeatTrigger { get; set; }
        public TriggerSettings StartTrigger { get; set; }
        public TriggerSettings StopTrigger { get; set; }
        public VoIPSettings Voip { get; set; }

        public String AdapterName
        {
            get { return Adapter.Name; }
            set { Adapter.Name = value; }
        }
        public String Name
        {
            get { return General.Name; }
            set { General.Name = value; }
        }
        public OmniId Id
        {
            get { return General.Id; }
            set { General.Id = value; }
        }

        public CaptureTemplate()
        {
            this.FileName = null;
            this.Adapter = new AdapterSettings();
            this.Alarms = new AlarmSettings();
            this.Analysis = new AnalysisSettings();
            this.Filter = new FilterSettings();
            this.General = new GeneralSettings();
            this.Graphs = new GraphsSettings();
            this.Plugins = new PluginsSettings();
            this.StatisticsOutput = new StatisticsOutputSettings();
            this.StatisticsOutputPreferences = new StatisticsOutputPreferencesSettings();
            this.RepeatTrigger = new RepeatTriggerSettings();
            this.StartTrigger = new TriggerSettings("StartTrigger");
            this.StopTrigger = new TriggerSettings("StopTrigger");
            this.Voip = new VoIPSettings();
        }

        public CaptureTemplate(String filename)
            : this()
        {
            if (!String.IsNullOrEmpty(filename))
            {
                this.FileName = filename;
                XDocument doc = XDocument.Load(this.FileName);
                XElement root = doc.Root;
                switch (root.Name.ToString())
                {
                    case "CaptureTemplate":
                        this.Load(root);
                        break;

                    case "autorestart":
                        XElement template = root.Element(CaptureTemplate.ObjectName);
                        this.Load(template);
                        break;
                }
            }
        }

        public CaptureTemplate(XElement template, OmniEngine engine=null)
            : this()
        {
            this.Load(template, engine);
        }

        public void Load(XElement template, OmniEngine engine=null)
        {
            if (template == null) return;

            XElement properties = null;
            if (template.Name == "properties")
            {
                properties = template;
            }
            else
            {
                properties = template.Element("properties");
            }

            XElement plugins = null;
            XElement pluginsConfig = null;

            IEnumerable<XElement> objs = properties.Elements("obj");
            foreach (XElement obj in objs)
            {
                String objectName = obj.Attribute("name").Value;
                switch (objectName)
                {
                    case AdapterSettings.ObjectName:
                        this.Adapter.Load(obj);
                        break;

                    case AlarmSettings.ObjectName:
                        this.Alarms.Load(obj);
                        break;

                    case AnalysisSettings.ObjectName:
                        this.Analysis.Load(obj);
                        break;

                    case FilterSettings.ObjectName:
                        this.Filter.Load(obj, engine);
                        break;

                    case GeneralSettings.ObjectName:
                        General.Load(obj);
                        break;

                    case GraphsSettings.ObjectName:
                        this.Graphs.Load(obj);
                        break;

                    case PluginsSettings.ConfigObjectName:
                        pluginsConfig = obj;
                        break;

                    case PluginsSettings.ObjectName:
                        plugins = obj;
                        break;

                    case "StartTrigger":
                        this.StartTrigger.Load(obj);
                        break;

                    case StatisticsOutputSettings.ObjectName:
                        this.StatisticsOutput.Load(obj);
                        break;

                    case StatisticsOutputPreferencesSettings.ObjectName:
                        this.StatisticsOutputPreferences.Load(obj);
                        break;

                    case "StopTrigger":
                        this.StopTrigger.Load(obj);
                        break;

                    case VoIPSettings.ObjectName:
                        this.Voip.Load(obj);
                        break;
                }
            }

            this.Plugins.Load(plugins, pluginsConfig);

            IEnumerable<XElement> props = properties.Elements("prop");
            foreach (XElement prop in props)
            {
                XAttribute attrib = prop.Attribute("name");
                if (attrib != null)
                {
                    String propName = attrib.Value;
                    switch (propName)
                    {
                        case "RepeatTrigger":
                            this.RepeatTrigger.Load(prop);
                            break;
                    }
                }
            }
        }

        public void Reset()
        {
            this.FileName = null;
            this.Adapter = new AdapterSettings();
            this.Alarms = new AlarmSettings();
            this.Analysis = new AnalysisSettings();
            this.Filter = new FilterSettings();
            this.Plugins = new PluginsSettings();
            this.StatisticsOutput = new StatisticsOutputSettings();
            this.StatisticsOutputPreferences = new StatisticsOutputPreferencesSettings();
            this.General = new GeneralSettings();
            this.Graphs = new GraphsSettings();
            this.RepeatTrigger = new RepeatTriggerSettings();
            this.StartTrigger = new TriggerSettings("StartTrigger");
            this.StopTrigger = new TriggerSettings("StopTrigger");
            this.Voip = new VoIPSettings();
        }

        public void SetAdapter(Adapter adapter)
        {
            this.Adapter.SetAdapter(adapter);
        }

        public void SetAll(bool enable = true)
        {
            this.General.SetTimeline(enable);
            this.Analysis.SetAll(enable);
        }

        public void SetFilters(List<String> filters)
        {
            this.Filter.SetFilters(filters);
        }

        public XElement Store(OmniEngine engine, bool modify=false)
        {
            XElement properties = new XElement(CaptureTemplate.RootName,
                this.Adapter.Store(engine),
                this.Alarms.Store(),
                this.Filter.Store(engine),
                this.General.Store(),
                this.Graphs.Store(),
                this.Analysis.Store(),          // PerfConfig
                this.Plugins.Store(),           // returns a list of XElements.
                this.RepeatTrigger.Store(),
                this.StartTrigger.Store(),
                this.StatisticsOutput.Store(),
                this.StatisticsOutputPreferences.Store(),
                this.StopTrigger.Store(),
                this.Voip.Store());

            if (modify)
            {
                XElement mod = new XElement(CaptureTemplate.RootName,
                    new XElement(OmniScript.Prop(tagId, 8, this.Id.ToString())),
                    new XElement(tagObject,
                        new XAttribute(tagName, "options"),
                        new XAttribute(tagClsid, OmniScript.ClassNameIds[tagPropBag]),
                        properties));

                return mod;
            }

            return properties;
        }

        public void Store(XElement node, OmniEngine engine)
        {
            node.Add(this.Store(engine));
        }

        public void Store(String filename, OmniEngine engine)
        {
            XElement template = new XElement(CaptureTemplate.ObjectName,
                new XAttribute(tagClsid, OmniScript.ClassNameIds[CaptureTemplate.ClassIdName]),
                new XAttribute("version", "1.0"),
                this.Store(engine));
            String xml = template.ToString(SaveOptions.DisableFormatting);

            using (StreamWriter outfile = new StreamWriter(filename))
            {
                outfile.Write(xml);
            }
        }

        public String ToString(OmniEngine engine, bool modify=false)
        {
            return this.Store(engine, modify).ToString(SaveOptions.DisableFormatting);
        }
    }
}
