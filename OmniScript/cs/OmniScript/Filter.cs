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

    public class Filter
    {
        public static OmniId AddressNodeId = OmniScript.ClassNameIds["AddressFilterNode"];
        public static OmniId AnalysisModuleNodeId = OmniScript.ClassNameIds["PluginFilterNode"];
        public static OmniId ApplicationNodeId = OmniScript.ClassNameIds["ApplicationFilterNode"];
        public static OmniId ChannelNodeId = OmniScript.ClassNameIds["ChannelFilterNode"];
        public static OmniId ErrorNodeId = OmniScript.ClassNameIds["ErrorFilterNode"];
        public static OmniId LengthNodeId = OmniScript.ClassNameIds["LengthFilterNode"];
        public static OmniId LogicalNodeId = OmniScript.ClassNameIds["LogicalFilterNode"];
        public static OmniId PatternNodeId = OmniScript.ClassNameIds["PatternFilterNode"];
        public static OmniId PortNodeId = OmniScript.ClassNameIds["PortFilterNode"];
        public static OmniId ProtocolNodeId = OmniScript.ClassNameIds["ProtocolFilterNode"];
        public static OmniId TCPDumpNodeId = OmniScript.ClassNameIds["BpfFilterNode"];
        public static OmniId TimeRangeNodeId = OmniScript.ClassNameIds["TimeRangeFilterNode"];
        public static OmniId ValueNodeId = OmniScript.ClassNameIds["ValueFilterNode"];
        public static OmniId WANDirectionNodeId = OmniScript.ClassNameIds["DirectionFilterNode"];
        public static OmniId WirelessNodeId = OmniScript.ClassNameIds["WirelessFilterNode"];

        public static NamedId AddressNode = new NamedId(FilterNode.AddressNodeName, AddressNodeId);
        public static NamedId AnalysisModuleNode = new NamedId(FilterNode.AnalysisModuleNodeName, AnalysisModuleNodeId);
        public static NamedId ApplicationNode = new NamedId(FilterNode.ApplicationNodeName, ApplicationNodeId);
        public static NamedId ChannelNode = new NamedId(FilterNode.ChannelNodeName, ChannelNodeId);
        public static NamedId ErrorNode = new NamedId(FilterNode.ErrorNodeName, ErrorNodeId);
        public static NamedId LengthNode = new NamedId(FilterNode.LengthNodeName, LengthNodeId);
        public static NamedId PatternNode = new NamedId(FilterNode.PatternNodeName, PatternNodeId);
        public static NamedId PortNode = new NamedId(FilterNode.PortNodeName, PortNodeId);
        public static NamedId ProtocolNode = new NamedId(FilterNode.ProtocolNodeName, ProtocolNodeId);
        public static NamedId TCPDumpNode = new NamedId(FilterNode.TCPDumpNodeName, TCPDumpNodeId);
        public static NamedId TimeRangeNode = new NamedId(FilterNode.TimeRangeNodeName, TimeRangeNodeId);
        public static NamedId ValueNode = new NamedId(FilterNode.ValueNodeName, ValueNodeId);
        public static NamedId WANDirectionNode = new NamedId(FilterNode.WANDirectionNodeName, WANDirectionNodeId);
        public static NamedId WirelessNode = new NamedId(FilterNode.WirelessNodeName, WirelessNodeId);

        private static readonly Dictionary<String, OmniId> ConsoleNodeNames = new Dictionary<String, OmniId>()
        {
            { FilterNode.AddressNodeName, AddressNodeId },
            { FilterNode.AnalysisModuleNodeName, AnalysisModuleNodeId },
            { FilterNode.ApplicationNodeName, ApplicationNodeId },
            { FilterNode.ChannelNodeName, ChannelNodeId },
            { FilterNode.ErrorNodeName, ErrorNodeId },
            { FilterNode.LengthNodeName, LengthNodeId },
            { FilterNode.PatternNodeName, PatternNodeId },
            { FilterNode.PortNodeName, PortNodeId },
            { FilterNode.ProtocolNodeName, ProtocolNodeId },
            { FilterNode.TCPDumpNodeName, TCPDumpNodeId },
            { FilterNode.ValueNodeName, ValueNodeId },
            { FilterNode.TimeRangeNodeName, TimeRangeNodeId },
            { FilterNode.WANDirectionNodeName, WANDirectionNodeId },
            { FilterNode.WirelessNodeName,  WirelessNodeId}
        };

        public static readonly FilterNode.DataTypeList EngineNodeIds = new FilterNode.DataTypeList()
        {
            new FilterNode.DataIdType(AddressNodeId, new FilterNode.NamedType(FilterNode.AddressNodeName, FilterNode.DataTypes.Address)),
            new FilterNode.DataIdType(AnalysisModuleNodeId, new FilterNode.NamedType(FilterNode.AnalysisModuleNodeName, FilterNode.DataTypes.AnalysisModule)),
            new FilterNode.DataIdType(ApplicationNodeId, new FilterNode.NamedType(FilterNode.ApplicationNodeName, FilterNode.DataTypes.Application)),
            new FilterNode.DataIdType(ChannelNodeId, new FilterNode.NamedType(FilterNode.ChannelNodeName, FilterNode.DataTypes.Channel)),
            new FilterNode.DataIdType(ErrorNodeId, new FilterNode.NamedType(FilterNode.ErrorNodeName, FilterNode.DataTypes.Error)),
            new FilterNode.DataIdType(LengthNodeId, new FilterNode.NamedType(FilterNode.LengthNodeName, FilterNode.DataTypes.Length)),
            new FilterNode.DataIdType(PatternNodeId, new FilterNode.NamedType(FilterNode.PatternNodeName, FilterNode.DataTypes.Pattern)),
            new FilterNode.DataIdType(PortNodeId, new FilterNode.NamedType(FilterNode.PortNodeName, FilterNode.DataTypes.Port)),
            new FilterNode.DataIdType(ProtocolNodeId, new FilterNode.NamedType(FilterNode.ProtocolNodeName, FilterNode.DataTypes.Protocol)),
            new FilterNode.DataIdType(TCPDumpNodeId, new FilterNode.NamedType(FilterNode.TCPDumpNodeName, FilterNode.DataTypes.TCPDump)),
            new FilterNode.DataIdType(TimeRangeNodeId, new FilterNode.NamedType(FilterNode.TimeRangeNodeName, FilterNode.DataTypes.TimeRange)),
            new FilterNode.DataIdType(ValueNodeId, new FilterNode.NamedType(FilterNode.ValueNodeName, FilterNode.DataTypes.Value)),
            new FilterNode.DataIdType(WANDirectionNodeId, new FilterNode.NamedType(FilterNode.WANDirectionNodeName, FilterNode.DataTypes.WANDirection)),
            new FilterNode.DataIdType(WirelessNodeId, new FilterNode.NamedType(FilterNode.WirelessNodeName, FilterNode.DataTypes.Wireless))
        };

#if NONE
        public class OmniFilterNode
            : FilterNode
        {
            public OmniFilterNode(NodeTypes type, XElement node, List<NamedId> plugins)
                : base(type, node, plugins)
            {
            }

            public OmniFilterNode(NodeTypes type, List<NamedId> plugins)
                : base(type, null, plugins)
            {
            }

            public override void Load(NodeTypes type, XElement node, List<NamedId> plugins)
            {
                OmniId clsid = Guid.Parse(node.Attribute("clsid").Value);
                DataIdType dataIdType = EngineNodeIds.Find(clsid);

                this.type = type;

                XElement filterNode = node.Element("filternode");

                this.inverted = OmniScript.PropBoolean(filterNode.Attribute("inverted").Value);

                XAttribute comment = filterNode.Attribute("comment");
                if (comment != null)
                {
                    this.comment = comment.Value.ToString();
                }

                switch (dataIdType.namedType.type)
                {
                    case DataTypes.Address:
                        this.data = new AddressData(dataIdType.id, filterNode);
                        break;

                    case DataTypes.AnalysisModule:
                        this.data = new AnalysisModuleData(dataIdType.id, filterNode, plugins);
                        break;

                    case DataTypes.Channel:
                        this.data = new ChannelData(dataIdType.id, filterNode);
                        break;

                    case DataTypes.Error:
                        this.data = new ErrorData(dataIdType.id, filterNode);
                        break;

                    case DataTypes.Length:
                        this.data = new LengthData(dataIdType.id, filterNode);
                        break;

                    case DataTypes.Pattern:
                        this.data = new PatternData(dataIdType.id, filterNode);
                        break;

                    case DataTypes.Port:
                        this.data = new PortData(dataIdType.id, filterNode);
                        break;

                    case DataTypes.Protocol:
                        this.data = new ProtocolData(dataIdType.id, filterNode);
                        break;

                    case DataTypes.TCPDump:
                        this.data = new TCPDumpData(dataIdType.id, filterNode);
                        break;

                    case DataTypes.Value:
                        this.data = new ValueData(dataIdType.id, filterNode);
                        break;

                    case DataTypes.WANDirection:
                        this.data = new WANDirectionData(dataIdType.id, filterNode);
                        break;

                    case DataTypes.Wireless:
                        this.data = new WirelessData(dataIdType.id, filterNode);
                        break;

                    default:
                        // error.
                        break;
                }

                foreach (XElement elem in filterNode.Elements())
                {
                    String name = elem.Name.ToString();
                    switch (name)
                    {
                        case "andnode":
                            OmniFilterNode andNode = new OmniFilterNode(NodeTypes.And, elem, plugins);
                            this.nodes.Add(andNode);
                            break;

                        case "ornode":
                            OmniFilterNode orNode = new OmniFilterNode(NodeTypes.Or, elem, plugins);
                            this.nodes.Add(orNode);
                            break;
                    }
                }
            }

            public override XElement Store()
            {
                XElement filterNode = new XElement("filternode",
                    new XAttribute("inverted", OmniScript.PropBoolean(this.inverted)),
                    this.data.Store());
                if (this.comment != null)
                {
                    filterNode.Add(new XAttribute("comment", WebUtility.HtmlEncode(this.comment)));
                }

                foreach (FilterNode filter in this.nodes)
                {
                    // Filter can be null when a Peek Plugin does not have an Engine counterpart.
                    // Need to display this.
                    if (filter != null)
                    {
                        filterNode.Add(filter.Store());
                    }
                }

                XElement node = new XElement(FilterNode.NodeTypeNames[this.type],
                    new XAttribute("clsid", this.ClassId.ToString("X")),
                    filterNode);

                return node;
            }
        }
#endif

        public OmniLogger logger;

        /// <summary>
        /// Gets or sets the filter's color.
        /// </summary>
        public uint Color { get; set; }

        /// <summary>
        /// Gets or sets the filter's comment.
        /// </summary>
        public String Comment { get; set; }

        /// <summary>
        /// Gets or sets the filter's creation timestamp.
        /// </summary>
        public PeekTime Created { get; set; }

        /// <summary>
        /// Gets or sets the filter's group.
        /// </summary>
        public String Group { get; set; }

        /// <summary>
        /// Gets or sets the global identifier.
        /// </summary>
        public OmniId Id { get; set; }

        /// <summary>
        /// Gets or sets the filter's last modification timestamp.
        /// </summary>
        public PeekTime Modified { get; set; }

        /// <summary>
        /// Gets or sets the filter's name.
        /// </summary>
        public String Name { get; set; }

        /// <summary>
        /// The criteria of the filter.
        /// </summary>
        public FilterNode Criteria { get; set; }

        public Filter(OmniLogger logger, String name, Object criteria=null)
        {
            this.logger = logger;
            this.Id = Guid.NewGuid();
            this.Name = name;
            if (criteria is String)
            {
                XDocument doc = XDocument.Parse((String)criteria);
                this.Load(doc.Root);
            }
            else if (criteria is XElement)
            {
                this.Load((XElement)criteria);
            }
        }

        public Filter(OmniLogger logger)
            : this(logger, null)
        {
        }

        public void Load(XElement node, List<NamedId> plugins=null)
        {
            bool engine = true; // Does node contain and Engine Filter?
            if (node == null) return;
            XElement filter = null;
            String nodeName = node.Name.ToString();
            if (nodeName == "filter")
            {
                filter = node;
            }
            else if (nodeName == "filterobj")   // from Filter List.
            {
                filter = node.Element("filter");
            }
            if (filter == null) return;

            IEnumerable<XAttribute> attribs = filter.Attributes();
            foreach (XAttribute attrib in attribs)
            {
                String name = attrib.Name.ToString();
                switch (name)
                {
                    case "id":
                        Guid id = Guid.Empty;
                        engine = Guid.TryParse(attrib.Value, out id);
                        this.Id = id;
                        break;

                    case "color":
                        this.Color = Convert.ToUInt32(attrib.Value);
                        break;

                    case "comment":
                        this.Comment = attrib.Value;
                        break;

                    case "created":
                        this.Created = new PeekTime(attrib.Value);
                        break;

                    case "group":
                        this.Group = attrib.Value;
                        break;

                    case "modified":
                        this.Modified = new PeekTime(attrib.Value);
                        break;

                    case "name":
                        this.Name = attrib.Value;
                        break;
                }
            }

            if ((this.Id == null) || (this.Id == Guid.Empty))
            {
                this.Id = Guid.NewGuid();
            }

            XElement rootNode = filter.Element("rootnode");
            if (rootNode != null)
            {
                this.Criteria = (engine)
                    ? OmniFilterNode.ParseOmniFilter(rootNode)
                    : OmniFilterNode.ParseConsoleFilter(rootNode);
            }
        }

        public XElement Store()
        {
            XElement filter = new XElement("filter",
                new XAttribute("id", this.Id),
                new XAttribute("name", this.Name));
            if (!String.IsNullOrEmpty(this.Comment))
            {
                filter.Add(new XAttribute("comment", this.Comment));
            }
            if (this.Color != 0)
            {
                filter.Add(new XAttribute("color", this.Color));
            }
            if (!String.IsNullOrEmpty(this.Group))
            {
                filter.Add(new XAttribute("group", this.Group));
            }
            if (this.Created != null)
            {
                filter.Add(new XAttribute("created", this.Created));
            }
            if (this.Modified != null)
            {
                filter.Add(new XAttribute("modified", this.Modified));
            }
            if (this.Criteria != null)
            {
                OmniFilterNode.StoreOmniFilter(filter, "rootnode", this.Criteria);
            }
            return filter;
        }

        public void Store(XElement node)
        {
            node.Add(this.Store());
        }

        public String ToString(int pad)
        {
            String str = "Filter: " + this.Name + Environment.NewLine;
            str += "    Id: " + this.Id + Environment.NewLine;
            if (!FilterNode.IsNullOrEmpty(this.Criteria))
            {
                str += "Criteria:" + Environment.NewLine;
                String operation = (this.Criteria.OrNode == null) ? "" : "or: ";
                str += this.Criteria.ToString((pad), operation);
            }
            return str;
        }

        public String ToString(String format)
        {
            if ((format == "X") || (format == "x"))
            {
                return this.Store().ToString(SaveOptions.DisableFormatting);
            }

            String str = "Name: " + this.Name + Environment.NewLine;
            str += "Comment: " + this.Comment + Environment.NewLine;
            if (!FilterNode.IsNullOrEmpty(this.Criteria))
            {
                str += this.Criteria.ToString();
            }
            return str;
        }

        public override String ToString()
        {
            return this.ToString(null);
        }
    }
}
