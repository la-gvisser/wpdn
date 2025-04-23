// =============================================================================
// <copyright file="ConsoleFilter.cs" company="Savvius, Inc.">
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
    using System.Net;
    using System.Net.NetworkInformation;
    using System.Xml.Linq;
    using System.Text;

    public class ConsoleFilter
    {
        public static OmniId FilterObjId = Guid.Parse("{22353029-A733-4FCC-8AC0-782DA33FA464}");

        public static OmniId NullNodeId = Guid.Parse("{B4298A64-5A40-4F5F-ABCD-B14BA0F8D9EB}");
        public static OmniId AddressNodeId = Guid.Parse("{D2ED5346-496C-4EA0-948E-21CDDA1ED723}");
        public static OmniId AltAddressNodeId = Guid.Parse("{2D2D9B91-08BF-44CF-B240-F0BBADBF21B5}");
        public static OmniId AnalysisModuleNodeId = Guid.Parse("{D0329C21-8B27-489F-84D7-C7B783634A6A}");
        public static OmniId ChannelNodeId = new OmniId(Guid.Parse("{6E8DAF74-AF0D-4CD3-865D-D559A5798C5B}"), "{6E8DAF74-AF0D-4cd3-865D-D559A5798C5B}");
        public static OmniId ErrorNodeId = Guid.Parse("{D0BDFB3F-F72F-4AEF-8E17-B16D4D3543FF}");
        public static OmniId LengthNodeId = Guid.Parse("{CF190294-C869-4D67-93F2-9A53FDFAE77D}");
        public static OmniId LogicalOperatorNodeId = new OmniId(Guid.Parse("{D8B5CE02-90CA-48AC-8188-468AC293B9C6}"), "{D8B5CE02-90CA-48ac-8188-468AC293B9C6}");
        public static OmniId PatternNodeId = Guid.Parse("{47D49D7C-8219-40D5-9E5D-8ADEAACC644D}");
        public static OmniId PortNodeId = Guid.Parse("{297D404D-3610-4A18-95A2-22768B554BED}");
        public static OmniId ProtocolNodeId = Guid.Parse("{F4342DAD-4A56-4ABA-9436-6E3C30DAB1C8}");
        public static OmniId TCPDumpNodeId = Guid.Parse("{11FC8E5E-B21E-446B-8024-39E41E6865E1}");
        public static OmniId ValueNodeId = Guid.Parse("{838F0E57-0D9F-4095-9C12-F1040C84E428}");
        public static OmniId WANDirectionNodeId = new OmniId(Guid.Parse("{90BAE500-B97B-42B0-9886-0947F34001EF}"), "{90BAE500-B97B-42b0-9886-0947F34001EF}");
        public static OmniId WirelessNodeId = Guid.Parse("{899E11AD-1849-40BA-9454-9F03798B3A6C}");

        public static NamedId NullNode = new NamedId(FilterNode.NullNodeName, NullNodeId);
        public static NamedId AddressNode = new NamedId(FilterNode.AddressNodeName, AddressNodeId);
        public static NamedId AnalysisModuleNode = new NamedId(FilterNode.AnalysisModuleNodeName, AnalysisModuleNodeId);
        public static NamedId ChannelNode = new NamedId(FilterNode.ChannelNodeName, ChannelNodeId);
        public static NamedId ErrorNode = new NamedId(FilterNode.ErrorNodeName, ErrorNodeId);
        public static NamedId LengthNode = new NamedId(FilterNode.LengthNodeName, LengthNodeId);
        public static NamedId PatternNode = new NamedId(FilterNode.PatternNodeName, PatternNodeId);
        public static NamedId PortNode = new NamedId(FilterNode.PortNodeName, PortNodeId);
        public static NamedId ProtocolNode = new NamedId(FilterNode.ProtocolNodeName, ProtocolNodeId);
        public static NamedId TCPDumpNode = new NamedId(FilterNode.TCPDumpNodeName, TCPDumpNodeId);
        public static NamedId ValueNode = new NamedId(FilterNode.ValueNodeName, ValueNodeId);
        public static NamedId WANDirectionNode = new NamedId(FilterNode.WANDirectionNodeName, WANDirectionNodeId);
        public static NamedId WirelessNode = new NamedId(FilterNode.WirelessNodeName, WirelessNodeId);

        private static readonly Dictionary<String, OmniId> ConsoleNodeNames = new Dictionary<String, OmniId>()
        {
            { FilterNode.AddressNodeName, AddressNodeId },
            { FilterNode.AnalysisModuleNodeName, AnalysisModuleNodeId },
            { FilterNode.ChannelNodeName, ChannelNodeId },
            { FilterNode.ErrorNodeName, ErrorNodeId },
            { FilterNode.LengthNodeName, LengthNodeId },
            { FilterNode.PatternNodeName, PatternNodeId },
            { FilterNode.PortNodeName, PortNodeId },
            { FilterNode.ProtocolNodeName, ProtocolNodeId },
            { FilterNode.TCPDumpNodeName, TCPDumpNodeId },
            { FilterNode.ValueNodeName, ValueNodeId },
            { FilterNode.WANDirectionNodeName, WANDirectionNodeId },
            { FilterNode.WirelessNodeName,  WirelessNodeId}
        };

        public static readonly FilterNode.DataTypeList ConsoleNodeIds = new FilterNode.DataTypeList()
        {
            new FilterNode.DataIdType(NullNodeId, new FilterNode.NamedType(FilterNode.NullNodeName, FilterNode.DataTypes.Null)),
            new FilterNode.DataIdType(AddressNodeId, new FilterNode.NamedType(FilterNode.AddressNodeName, FilterNode.DataTypes.Address)),
            new FilterNode.DataIdType(AltAddressNodeId, new FilterNode.NamedType(FilterNode.AddressNodeName, FilterNode.DataTypes.Address)),
            new FilterNode.DataIdType(AnalysisModuleNodeId, new FilterNode.NamedType(FilterNode.AnalysisModuleNodeName, FilterNode.DataTypes.AnalysisModule)),
            new FilterNode.DataIdType(ChannelNodeId, new FilterNode.NamedType(FilterNode.ChannelNodeName, FilterNode.DataTypes.Channel)),
            new FilterNode.DataIdType(ErrorNodeId, new FilterNode.NamedType(FilterNode.ErrorNodeName, FilterNode.DataTypes.Error)),
            new FilterNode.DataIdType(LengthNodeId, new FilterNode.NamedType(FilterNode.LengthNodeName, FilterNode.DataTypes.Length)),
            new FilterNode.DataIdType(LogicalOperatorNodeId, new FilterNode.NamedType(FilterNode.LogicalOperatorNodeName, FilterNode.DataTypes.LogicalOperator)),
            new FilterNode.DataIdType(PatternNodeId, new FilterNode.NamedType(FilterNode.PatternNodeName, FilterNode.DataTypes.Pattern)),
            new FilterNode.DataIdType(PortNodeId, new FilterNode.NamedType(FilterNode.PortNodeName, FilterNode.DataTypes.Port)),
            new FilterNode.DataIdType(ProtocolNodeId, new FilterNode.NamedType(FilterNode.ProtocolNodeName, FilterNode.DataTypes.Protocol)),
            new FilterNode.DataIdType(TCPDumpNodeId, new FilterNode.NamedType(FilterNode.TCPDumpNodeName, FilterNode.DataTypes.TCPDump)),
            new FilterNode.DataIdType(ValueNodeId, new FilterNode.NamedType(FilterNode.ValueNodeName, FilterNode.DataTypes.Value)),
            new FilterNode.DataIdType(WANDirectionNodeId, new FilterNode.NamedType(FilterNode.WANDirectionNodeName, FilterNode.DataTypes.WANDirection)),
            new FilterNode.DataIdType(WirelessNodeId, new FilterNode.NamedType(FilterNode.WirelessNodeName, FilterNode.DataTypes.Wireless))
        };

        public class ConsoleFilterNode
            : FilterNode
        {
            public ConsoleFilterNode(NodeTypes type, XElement node, List<NamedId> plugins)
                : base(type, node, plugins)
            {
            }

            public override void Load(NodeTypes type, XElement node, List<NamedId> plugins)
            {
                OmniId clsid = Guid.Parse(node.Attribute("clsid").Value);
                DataIdType dataIdType = ConsoleNodeIds.Find(clsid);

                this.type = type;
//                this.clsid = dataIdType.id; // The id in the dataTypeList has the exact guid format.

                foreach (XElement elem in node.Elements())
                {
                    String name = elem.Name.ToString();
                    switch (name)
                    {
                        case "filternode":
                            this.inverted = OmniScript.PropBoolean(elem.Attribute("inverted").Value);
                            XAttribute comment = elem.Attribute("comment");
                            if (comment != null)
                            {
                                this.comment = comment.Value.ToString();
                            }

                            switch (dataIdType.namedType.type)
                            {
                                case DataTypes.Address:
                                    this.data = new AddressData(dataIdType.id, elem);
                                    break;

                                case DataTypes.AnalysisModule:
                                    this.data = new AnalysisModuleData(dataIdType.id, elem, plugins, false);
                                    break;

                                case DataTypes.Channel:
                                    this.data = new ChannelData(dataIdType.id, elem);
                                    break;

                                case DataTypes.Error:
                                    this.data = new ErrorData(dataIdType.id, elem);
                                    break;

                                case DataTypes.Length:
                                    this.data = new LengthData(dataIdType.id, elem);
                                    break;

                                case DataTypes.Pattern:
                                    this.data = new PatternData(dataIdType.id, elem);
                                    break;

                                case DataTypes.Port:
                                    this.data = new PortData(dataIdType.id, elem);
                                    break;

                                case DataTypes.Protocol:
                                    this.data = new ProtocolData(dataIdType.id, elem);
                                    break;

                                case DataTypes.TCPDump:
                                    this.data = new TCPDumpData(dataIdType.id, elem);
                                    break;

                                case DataTypes.Value:
                                    this.data = new ValueData(dataIdType.id, elem);
                                    break;

                                case DataTypes.WANDirection:
                                    this.data = new WANDirectionData(dataIdType.id, elem);
                                    break;

                                case DataTypes.Wireless:
                                    this.data = new WirelessData(dataIdType.id, elem);
                                    break;

                                default:
                                    // error.
                                    break;
                            }
                            break;

                        case "andnode":
                            ConsoleFilterNode andNode = new ConsoleFilterNode(NodeTypes.And, elem, plugins);
                            this.nodes.Add(andNode);
                            break;

                        case "ornode":
                            ConsoleFilterNode orNode = new ConsoleFilterNode(NodeTypes.Or, elem, plugins);
                            this.nodes.Add(orNode);
                            break;
                    }
                }
            }

            public override XElement Store()
            {
                XElement filterNode = new XElement("filternode",
                    new XAttribute("inverted", OmniScript.PropBoolean(this.inverted)),
                    new XAttribute("comment", (this.comment == null) ? "" : WebUtility.HtmlEncode(this.comment)),
                    this.data.Store());

                XElement node = new XElement(FilterNode.NodeTypeNames[this.type],
                    new XAttribute("clsid", this.data.Id.ToString("X")),
                    filterNode);

                foreach (FilterNode filter in this.nodes)
                {
                    node.Add(filter.Store());
                }
                return node;
            }
        }

        /// <summary>
        /// Gets or sets the filters identifier.
        /// </summary>
        public uint Id { get; set; }

        /// <summary>
        /// Gets or sets the filter's name.
        /// </summary>
        public String Name { get; set; }

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
        /// Gets or sets the filter's last modification timestamp.
        /// </summary>
        public PeekTime Modified { get; set; }

        /// <summary>
        /// The criteria of the filter.
        /// </summary>
        public FilterNode Root { get; set; }

        public ConsoleFilter()
        {
        }

        public void Load(XElement node, List<NamedId> plugins)
        {
            if (node.Name.ToString() != "filterobj") return;
            XElement filter = node.Element("filter");
            IEnumerable<XAttribute> attribs = filter.Attributes();
            foreach (XAttribute attrib in attribs)
            {
                String name = attrib.Name.ToString();
                switch (name)
                {
                    case "id":
                        this.Id = Convert.ToUInt32(attrib.Value);
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

            XElement rootNode = filter.Element("rootnode");
            if (rootNode != null)
            {
                this.Root = new ConsoleFilterNode(FilterNode.NodeTypes.Criteria, rootNode, plugins);
            }
        }

        public void Load(XElement node)
        {
            this.Load(node, null);
        }

        public XElement Store()
        {
            XElement filter = new XElement("filter",
                new XAttribute("id", this.Id),
                new XAttribute("name", this.Name),
                new XAttribute("comment", this.Comment),
                new XAttribute("color", this.Color),
                new XAttribute("group", this.Group));
            if (this.Created != null)
            {
                filter.Add(new XAttribute("created", this.Created));
            }
            if (this.Modified != null)
            {
                filter.Add(new XAttribute("modified", this.Modified));
            }
            if (this.Root != null)
            {
                filter.Add(this.Root.Store());
            }

            XElement node = new XElement("filterobj",
                new XAttribute("clsid", FilterObjId),
                filter);

            return node;
        }

        public void Store(XElement node)
        {
            node.Add(this.Store());
        }

        public String ToString(String format)
        {
            if ((format == "X") || (format == "x"))
            {
                return this.Store().ToString(SaveOptions.DisableFormatting);
            }

            String str = "Name: " + this.Name + "\n";
            str += "Comment: " + this.Comment + "\n";
            if (this.Root != null)
            {
                str += this.Root.ToString("  ");
            }
            return str;
        }

        public override String ToString()
        {
            return this.ToString(null);
        }
    }
}
