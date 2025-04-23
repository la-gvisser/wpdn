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
    using System.Linq;
    using System.Text;
    using System.Xml.Linq;

    public class FilterNode
    {
        public enum DataTypes
        {
            Undefined,
            Null,
            Address,
            AnalysisModule,
            Application,
            Channel,
            Error,
            Length,
            LogicalOperator,
            Pattern,
            Port,
            Protocol,
            TCPDump,
            TimeRange,
            Value,
            WANDirection,
            Wireless
        }

        public static readonly String ClassName = "";
        public static readonly String DisplayName = "";
        public static readonly String[] Directions;

        public static int PadWidth = 4;

        public String DirectionString(bool first, bool second)
        {
            int left = (first) ? 1 : 0;
            int right = (second) ? 2 : 0;
            return Directions[left + right];
        }

        public static uint MakeMask(int byteCount, int bitLength = 32)
        {
            uint mask = 0;
            for (int i = 0; i < byteCount; i++)
            {
                mask = (mask << 1) | 1;
            }
            return mask << (bitLength - byteCount);
        }

        public struct DataIdType
        {
            public OmniId id;
            public NamedType namedType;

            public DataIdType(OmniId id, NamedType namedType)
            {
                this.id = id;
                this.namedType = namedType;
            }
        }

        public class DataTypeList
            : List<DataIdType>
        {
            public DataIdType Find(OmniId id)
            {
                return this.Find(
                    delegate(DataIdType dataId)
                    {
                        return dataId.id.Equals(id);
                    });
            }

            public DataIdType Find(DataTypes type)
            {
                return this.Find(
                    delegate(DataIdType dataId)
                    {
                        return dataId.namedType.type.Equals(type);
                    });
            }
        }

        public enum ErrorFlags
        {
            CRC = 0x002,
            Frame = 0x004,
            Oversize = 0x010,
            Runt = 0x020
        }

        public enum LogicalOps
        {
            OpAnd,
            OpOr,
            OpNot
        }

        public enum NodeTypes
        {
            Undefined,
            Criteria,
            And,
            Or
        }

        public enum PatternTypes
        {
            ASCII,
            Unicode,
            RawData,    // HexData
            EBCDIC,
            Regex
        }

        public enum ValueFlags
        {
            NetworkByteOrder = 0x01,
            Signed = 0x02,
            Hex = 0x04,
            Binary = 0x08
        }

        public enum ValueOperators
        {
            Equal = 0x01,
            GreaterThan = 0x02,
            GreaterThanOrEqual = (Equal | GreaterThan),
            LessThan = 0x04,
            LessThanOrEqual = (Equal | LessThan),
            NotEqual = (GreaterThan | LessThan)
        }

        public enum ValueTypes
        {
            UnsignedByte,
            SignedByte,
            UnsignedShort,
            SignedShort,
            UnsignedWord,
            SignedWord,
            UnsignedLong,
            SignedLong
        }

        public enum WANDirections
        {
            Undefined,
            ToDTE,
            ToDCE
        }

        public enum WirelessBands
        {
            All,
            Generic,
            B,
            A,
            G,
            BG = G,
            N = 6,
            TurboA,
            TurboG,
            SuperG,
            LicensedA1MHz = 100,
            LicensedA5MHz,
            LicensedA10MHz,
            LicensedA15MHz,
            LicensedA20MHz,
            PrimaryAC0 = 201,
            PrimaryAC1,
            PrimaryAC2,
            PrimaryAC3,
            Unknown5,
            Unknown6,
            Unknown7,
            Unknown8,
            Unknown9,
            N20MHz = 300,
            N40MHz,
            N40MHzLow,
            N40MHzHigh
        }

        public enum WirelessDataMask
        {
            Channel = 0x00001,
            DataRate = 0x00002,
            Signal = 0x00004,
            SignaldBm = 0x00008,
            Noise = 0x00010,
            NoisedBm = 0x00020,
            Encryption = 0x00040,
            DecryptionError = 0x00080,
            BSSID = 0x00100,
            SignaldBm1 = 0x00200,
            SignaldBm2 = 0x00400,
            SignaldBm3 = 0x00800,
            NoisedBm1 = 0x01000,
            NoisedBm2 = 0x02000,
            NoisedBm3 = 0x04000,
            FlagsN = 0x08000
        }

        public struct WirelessChannel
        {
            public short channel;
            public uint frequency;
            public WirelessBands band;
        }

        public struct NamedType
        {
            public static NamedType Empty
            {
                get { return new NamedType(null, DataTypes.Undefined); }
            }
            public String name;
            public DataTypes type;

            public NamedType(String name, DataTypes type)
            {
                this.name = name;
                this.type = type;
            }
        }

        public static readonly String NullNodeName = "Null Node";
        public static readonly String AddressNodeName = "Address Node";
        public static readonly String AnalysisModuleNodeName = "Analysis Module Node";
        public static readonly String ApplicationNodeName = "Application Node";
        public static readonly String ChannelNodeName = "Channel Node";
        public static readonly String CountryNodeName = "Country Node";
        public static readonly String ErrorNodeName = "Error Node";
        public static readonly String LengthNodeName = "Length Node";
        public static readonly String LogicalOperatorNodeName = "Logical Operator Node";
        public static readonly String PatternNodeName = "Pattern Node";
        public static readonly String PortNodeName = "Port Node";
        public static readonly String ProtocolNodeName = "Protocol Node";
        public static readonly String TCPDumpNodeName = "TCP Dump Node";
        public static readonly String TimeRangeNodeName = "Time Range Node";
        public static readonly String ValueNodeName = "Value Node";
        public static readonly String WANDirectionNodeName = "WAN Direction Node";
        public static readonly String WirelessNodeName = "Wireless Node";
 
        public static readonly Dictionary<NodeTypes, String> NodeTypeNames = new Dictionary<NodeTypes, String>()
        {
            {NodeTypes.Undefined, "undefined"},
            {NodeTypes.Criteria, "rootnode"},
            {NodeTypes.And, "andnode"},
            {NodeTypes.Or, "ornode"}
        };

        public static byte[] GetAsciiBytes(String value)
        {
            byte[] bytes = Encoding.ASCII.GetBytes(value);
            return bytes;
        }

        public static byte[] GetBytes(String value)
        {
            byte[] bytes = new byte[value.Length * sizeof(char)];
            System.Buffer.BlockCopy(value.ToCharArray(), 0, bytes, 0, bytes.Length);
            return bytes;
        }

        public static string GetString(byte[] bytes)
        {
            char[] chars = new char[bytes.Length / sizeof(char)];
            System.Buffer.BlockCopy(bytes, 0, chars, 0, bytes.Length);
            return new String(chars);
        }

        public static Byte[] HexStringToBytes(String value)
        {
            int byteCount = value.Length / 2;
            // assert byteCount * 2 == value.Length;
            byte[] result = new byte[byteCount];
            for (int i = 0; i < byteCount; i++)
            {
                result[i] = Convert.ToByte(value.Substring((i * 2), 2), 16);
            }
            return result;
        }

        public static String BytesToHexString(Byte[] value)
        {
            String result = "";
            foreach (Byte oct in value)
            {
                result += String.Format("{0:X2}", oct);
            }
            return result;
        }

        public static String AsciiHexToString(String ascii)
        {
            // var hex = System.Runtime.Remoting.Metadata.W3cXsd2001.SoapHexBinary.Parse(ascii);
            // return Encoding.ASCII.GetString(hex.Value);
            return "";
        }

        public static String UnicodeHexToString(String unicode)
        {
            byte[] bytes = GetBytes(unicode);
            return Encoding.Unicode.GetString(bytes);
        }

        public static String AsciiHexToEbcdic(String str)
        {
            Encoding ascii = Encoding.ASCII;
            Encoding ebcdic = Encoding.GetEncoding("IBM037");

            byte[] bytes = GetBytes(str);
            return GetString(Encoding.Convert(ebcdic, ascii, bytes));
        }

        public static bool FilterPropertyBool(XElement node, String element, String attribute, bool preset)
        {
            XElement innerNode = node.Element(element);
            if (innerNode != null)
            {
                XAttribute data = innerNode.Attribute(attribute);
                if ((data != null) && !String.IsNullOrEmpty(data.Value))
                {
                    return OmniScript.PropBoolean(data.Value);
                }
            }
            return preset;
        }

        public static byte[] FilterPropertyHex(XElement node, String element, String attribute)
        {
            XElement innerNode = node.Element(element);
            if (innerNode != null)
            {
                XAttribute data = innerNode.Attribute(attribute);
                if ((data != null) && !String.IsNullOrEmpty(data.Value))
                {
                    return HexStringToBytes(data.Value.ToString());
                }
            }
            return null;
        }

        public static uint FilterPropertyInteger(XElement node, String element, String attribute, uint preset)
        {
            XElement innerNode = node.Element(element);
            if (innerNode != null)
            {
                XAttribute data = innerNode.Attribute(attribute);
                if ((data != null) && !String.IsNullOrEmpty(data.Value))
                {
                    return Convert.ToUInt32(data.Value);
                }
            }
            return preset;
        }

        public static uint FilterPropertyIntegerHex(XElement node, String element, String attribute, uint preset)
        {
            XElement innerNode = node.Element(element);
            if (innerNode != null)
            {
                XAttribute data = innerNode.Attribute(attribute);
                if ((data != null) && !String.IsNullOrEmpty(data.Value))
                {
                    return Convert.ToUInt32(data.Value, 16);
                }
            }
            return preset;
        }

        public static ulong FilterPropertyLong(XElement node, String element, String attribute, ulong preset)
        {
            XElement innerNode = node.Element(element);
            if (innerNode != null)
            {
                XAttribute data = innerNode.Attribute(attribute);
                if ((data != null) && !String.IsNullOrEmpty(data.Value))
                {
                    return Convert.ToUInt64(data.Value);
                }
            }
            return preset;
        }

        public static int FilterPropertySigned(XElement node, String element, String attribute, int preset)
        {
            XElement innerNode = node.Element(element);
            if (innerNode != null)
            {
                XAttribute data = innerNode.Attribute(attribute);
                if ((data != null) && !String.IsNullOrEmpty(data.Value))
                {
                    return Convert.ToInt32(data.Value);
                }
            }
            return preset;
        }

        public static String FilterPropertyString(XElement node, String element, String attribute, String preset)
        {
            XElement innerNode = node.Element(element);
            if (innerNode != null)
            {
                XAttribute data = innerNode.Attribute(attribute);
                if (data != null)
                {
                    return data.Value.ToString();
                }
            }
            return preset;
        }

        public class FilterNodeList
            : List<FilterNode>
        {
            public FilterNodeList()
            {
            }
        }

        static FilterNode()
        {
            FilterNode.Directions =
                new String[] { "---", "<--", "-->", "<->" };
        }

        public String Comment { get; set; }
        public bool Inverted { get; set; }
        public FilterNode AndNode { get; set; }
        public FilterNode OrNode { get; set; }

        public FilterNode()
        {
            this.Comment = null;
            this.Inverted = false;
            this.AndNode = null;
            this.OrNode = null;
        }

        public FilterNode(XElement element)
            : this()
        {
            this.Load(element);
        }

        public virtual void Load(XElement node)
        {
            if (node == null) return;
            IEnumerable<XAttribute> attributes = node.Attributes();
            foreach (XAttribute attribute in attributes)
            {
                switch (attribute.Name.ToString())
                {
                    case "inverted":
                        this.Inverted = OmniScript.PropBoolean(attribute.Value);
                        break;

                    case "comment":
                        this.Comment = attribute.Value.ToString();
                        break;
                }
            }
        }

        public virtual void Store(XElement node)
        {
            node.Add(new XAttribute("inverted", OmniScript.PropBoolean(this.Inverted)));
            if (this.Comment != null)
            {
                node.Add(new XAttribute("comment", this.Comment));
            }
        }

        static public bool IsNullOrEmpty(FilterNode node)
        {
            return (node == null); // || ((node.AndNode == null) && (node.OrNode == null));
        }

        public virtual String ToString(int pad, String operation = "")
        {
            String str = "";
            if (this.AndNode != null)
            {
                str += Environment.NewLine + this.AndNode.ToString(pad + 1, "and: ");
            }
            if (this.OrNode != null)
            {
                str += Environment.NewLine + this.OrNode.ToString(pad, "or: ");
            }
            return str;
        }

        public override String ToString()
        {
            return ClassName;
        }
    }

    public class AddressFilterNode
        : FilterNode
    {
        new public static readonly String ClassName = "AddressFilterNode";
        new public static readonly String DisplayName = "Address";

        public OmniAddressList Address1 { get; set; }
        public OmniAddressList Address2 { get; set; }
        public bool Accept1To2 { get; set; }
        public bool Accept2To1 { get; set; }

        public AddressFilterNode()
            : base()
        {
            this.Address1 = null;
            this.Address2 = null;
            this.Accept1To2 = true;
            this.Accept2To1 = true;
        }

        public AddressFilterNode(XElement node)
            : base(node)
        {
            this.Load(node);
        }

        public override void Load(XElement node)
        {
            if (node == null) return;
            base.Load(node);

            if (node.Element("addr1") != null)
            {
                this.Address1 = OmniAddressList.Parse(node.Element("addr1"));
                this.Address2 = OmniAddressList.Parse(node.Element("addr2"));
            }
            else
            {
                this.Address1 = OmniAddressList.Parse(node.Element("saddr1"));
                this.Address2 = OmniAddressList.Parse(node.Element("saddr2"));
            }

            this.Accept1To2 = FilterPropertyBool(node, "accept1to2", "data", this.Accept1To2);
            this.Accept2To1 = FilterPropertyBool(node, "accept2to1", "data", this.Accept2To1);
        }

        public override void Store(XElement node)
        {
            if (node == null) return;
            base.Store(node);

            String[] nodeNames = { "addr1", "addr2" };

            if (this.Address1.Count > 1 || ((this.Address2 != null) && (this.Address2.Count > 1)))
            {
                nodeNames[0] = "saddr1";
                nodeNames[1] = "saddr2";
            }

            node.Add(this.Address1.Store(nodeNames[0]));
            if (this.Address2 == null)
            {
                OmniAddressList addrList = OmniAddressList.MakeAny(this.Address1.ElementType());
                node.Add(addrList.Store(nodeNames[1]));
            }
            else
            {
                node.Add(this.Address2.Store(nodeNames[1]));
            }
            node.Add(new XElement("accept1to2",
                new XAttribute("data", OmniScript.PropBoolean(this.Accept1To2))));
            node.Add(new XElement("accept2to1",
                new XAttribute("data", OmniScript.PropBoolean(this.Accept2To1))));
        }

        public override string ToString(int pad, String operation="")
        {
            if (this.Address1 == null) throw new OmniException("Must specify Address1.");
            String criteria = String.Format("{0}{1}{2}: {3}{4} {5} {6}",
                " ".PadRight(pad * FilterNode.PadWidth),
                operation,
                AddressFilterNode.DisplayName,
                (this.Inverted) ? "! " : "",
                this.Address1.ToString(),
                DirectionString(this.Accept1To2, this.Accept2To1),
                ((this.Address2 == null) || this.Address2.IsEmptyOrNull())
                    ? "Any" 
                    : this.Address2.ToString());
            return criteria + base.ToString(pad);
        }

//         public override string ToString(String tab)
//         {
//             return base.ToString(tab);
//         }

        public override string ToString()
        {
            return ClassName;
        }
    }

    public class ApplicationFilterNode
        : FilterNode
    {
        new public static readonly String ClassName = "ApplicationFilterNode";
        new public static readonly String DisplayName = "Application";

        // Omni stores a Media Spec
        public String Application { get; set; }

        public ApplicationFilterNode()
            : base()
        {
            this.Application = "";
        }

        public ApplicationFilterNode(XElement node)
            : base(node)
        {
            if (node == null) return;
            this.Load(node);
        }

        public override void Load(XElement node)
        {
            if (node == null) return;
            base.Load(node);

            String asciihex = FilterPropertyString(node, "application", "data", "");
            String application = AsciiHexToString(asciihex);
            this.Application = application.Replace('\0', ' ').Trim();
        }

        public override void Store(XElement node)
        {
            base.Store(node);

            byte[] bytes = GetAsciiBytes(this.Application);
            String data = BytesToHexString(bytes);
            data += "00";
 
            node.Add(new XElement("application",
                new XAttribute("class", "1"),
                new XAttribute("type", "8"),
                new XAttribute("data", data)));
        }

        public override string ToString(int pad, String operation = "")
        {
            String criteria = String.Format("{0}{1}{2}: {3}{4}",
                " ".PadRight(pad * FilterNode.PadWidth),
                operation,
                ApplicationFilterNode.DisplayName,
                (this.Inverted) ? "! " : "",
                this.Application);
            return criteria + base.ToString(pad);
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }

    public class BpfFilterNode
        : FilterNode
    {
        new public static readonly String ClassName = "BpfFilterNode";
        new public static readonly String DisplayName = "BPF";

        public String Filter { get; set; }

        public BpfFilterNode()
            : base()
        {
        }

        public BpfFilterNode(XElement node)
            : base(node)
        {
            this.Load(node);
        }

        public override void Load(XElement node)
        {
            if (node == null) return;
            base.Load(node);

            this.Filter = FilterPropertyString(node, "bpfdata", "data", "");
        }

        public override void Store(XElement node)
        {
            if (node == null) return;
            base.Store(node);

            node.Add(new XElement("bpfdata", new XAttribute("data", this.Filter)));
        }

        public override string ToString(int pad, String operation = "")
        {
            String criteria = String.Format("{0}{1}{2}: {3}{4}",
                " ".PadRight(pad * FilterNode.PadWidth),
                operation,
                BpfFilterNode.DisplayName,
                (this.Inverted) ? "! " : "",
                this.Filter);
            return criteria + base.ToString(pad);
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }

    public class ChannelFilterNode
        : FilterNode
    {
        new public static readonly String ClassName = "ChannelFilterNode";
        new public static readonly String DisplayName = "Channel";

        public uint Channel { get; set; }

        public ChannelFilterNode()
            : base()
        {
            this.Channel = 0;
        }

        public ChannelFilterNode(XElement node)
            : base(node)
        {
            this.Load(node);
        }

        public override void Load(XElement node)
        {
            if (node == null) return;
            base.Load(node);

            this.Channel = FilterPropertyInteger(node, "channel", "data", this.Channel);
        }

        public override void Store(XElement node)
        {
            base.Store(node);

            node.Add(new XElement("channel", new XAttribute("data", this.Channel.ToString())));
        }

        public override string ToString(int pad, String operation = "")
        {
            String criteria = String.Format("{0}{1}{2}: {3}{4}",
                " ".PadRight(pad * FilterNode.PadWidth),
                operation,
                ChannelFilterNode.DisplayName,
                (this.Inverted) ? "! " : "",
                this.Channel);
            return criteria + base.ToString(pad);
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }

    public class CountryFilterNode
        : FilterNode
    {
        new public static readonly String ClassName = "CountryFilterNode";
        new public static readonly String DisplayName = "Country";

        public String Country1 { get; set; }
        public String Country2 { get; set; }
        public bool Accept1To2 { get; set; }
        public bool Accept2To1 { get; set; }


        public CountryFilterNode()
            : base()
        {
            this.Country1 = null;
            this.Country2 = null;
            this.Accept1To2 = true;
            this.Accept2To1 = true;
        }

        public CountryFilterNode(XElement node)
            : base(node)
        {
            this.Load(node);
        }

        public override void Load(XElement node)
        {
            if (node == null) return;
            base.Load(node);

            this.Country1 = FilterPropertyString(node, "country1", "data", "");
            this.Country2 = FilterPropertyString(node, "country2", "data", "");
            this.Accept1To2 = FilterPropertyBool(node, "accept1to2", "data", this.Accept1To2);
            this.Accept2To1 = FilterPropertyBool(node, "accept2to1", "data", this.Accept2To1);
        }

        public override void Store(XElement node)
        {
            if (node == null) return;
            base.Store(node);

            node.Add(new XElement("country1", new XAttribute("data", this.Country1)));
            if (this.Country2 == null)
            {
                node.Add(new XElement("country2", new XAttribute("data", "")));
            }
            else
            {
                node.Add(new XElement("country2", new XAttribute("data", this.Country2)));
            }
            node.Add(new XElement("accept1to2",
                new XAttribute("data", OmniScript.PropBoolean(this.Accept1To2))));
            node.Add(new XElement("accept2to1",
                new XAttribute("data", OmniScript.PropBoolean(this.Accept2To1))));
        }

        public override string ToString(int pad, String operation = "")
        {
            if (this.Country1 == null) throw new OmniException("Must specify Country1.");
            String criteria = String.Format("{0}{1}{2} {3}{4} {5} {6}",
                " ".PadRight(pad * FilterNode.PadWidth),
                operation,
                CountryFilterNode.DisplayName,
                (this.Inverted) ? "! " : "",
                this.Country1,
                DirectionString(this.Accept1To2, this.Accept2To1),
                (String.IsNullOrEmpty(Country2)
                    ? "Any"
                    : this.Country2));
            return criteria + base.ToString(pad);
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }

    public class DirectionFilterNode
        : FilterNode
    {
        new public static readonly String ClassName = "DirectionFilterNode";
        new public static readonly String DisplayName = "Direction";

        public WANDirections Direction { get; set; }

        public DirectionFilterNode()
            : base()
        {
            this.Direction = WANDirections.Undefined;
        }

        public DirectionFilterNode(XElement node)
            : base(node)
        {
            this.Load(node);
        }

        public override void Load(XElement node)
        {
            if (node == null) return;
            base.Load(node);

            this.Direction = (WANDirections)FilterPropertyInteger(node, "direction", "data", 0);
        }

        public override void Store(XElement node)
        {
            if (node == null) return;
            base.Store(node);

            node.Add(new XElement("direction", new XAttribute("data", this.Direction.ToString())));
        }

        public override string ToString(int pad, String operation = "")
        {
            String criteria = String.Format("{0}{1}{2} {3}{4}",
                " ".PadRight(pad * FilterNode.PadWidth),
                operation,
                DirectionFilterNode.DisplayName,
                (this.Inverted) ? "! " : "",
                (this.Direction == WANDirections.ToDCE)
                    ? "To DCE"
                    : (this.Direction == WANDirections.ToDTE)
                        ? "To DTE"
                        : "Unknown");
            return criteria + base.ToString(pad);
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }

    public class ErrorFilterNode
        : FilterNode
    {
        new public static readonly String ClassName = "ErrorFilterNode";
        new public static readonly String DisplayName = "Error";

        public bool CrcErrors { get; set; }
        public bool FrameErrors { get; set; }
        public bool OversizeErrors { get; set; }
        public bool RuntErrors { get; set; }

        public ErrorFilterNode()
            : base()
        {
            this.CrcErrors = false;
            this.FrameErrors = false;
            this.OversizeErrors = false;
            this.RuntErrors = false;
        }

        public ErrorFilterNode(XElement node)
            : base(node)
        {
            this.Load(node);
        }

        public override void Load(XElement node)
        {
            if (node == null) return;
            base.Load(node);

            uint flags = FilterPropertyInteger(node, "errorflags", "data", 0);
            this.CrcErrors = (flags & (uint)ErrorFlags.CRC) != 0;
            this.FrameErrors = (flags & (uint)ErrorFlags.Frame) != 0;
            this.OversizeErrors = (flags & (uint)ErrorFlags.Oversize) != 0;
            this.RuntErrors = (flags & (uint)ErrorFlags.Runt) != 0;
        }

        public override void Store(XElement node)
        {
            if (node == null) return;
            base.Store(node);

            uint flags = 0;
            if (this.CrcErrors) flags |= (uint)ErrorFlags.CRC;
            if (this.FrameErrors) flags |= (uint)ErrorFlags.Frame;
            if (this.OversizeErrors) flags |= (uint)ErrorFlags.Oversize;
            if (this.RuntErrors) flags |= (uint)ErrorFlags.Runt;
            node.Add(new XElement("errorflags", new XAttribute("data", flags.ToString())));
        }

        public override string ToString(int pad, String operation = "")
        {
            String criteria = String.Format("{0}{1}{2} {3}crc:{4} frame:{5} oversize:{6} runt:{7}",
                " ".PadRight(pad * FilterNode.PadWidth),
                operation,
                ErrorFilterNode.DisplayName,
                (this.Inverted) ? "! " : "",
                (this.CrcErrors) ? "T" : "F",
                (this.FrameErrors) ? "T" : "F",
                (this.OversizeErrors) ? "T" : "F",
                (this.RuntErrors) ? "T" : "F");
            return criteria + base.ToString(pad);
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }

    public class LengthFilterNode
        : FilterNode
    {
        new public static readonly String ClassName = "LengthFilterNode";
        new public static readonly String DisplayName = "Length";

        public uint Minimum { get; set; }
        public uint Maximum { get; set; }

        public LengthFilterNode()
            : base()
        {
            this.Minimum = 64;
            this.Maximum = 1518;
        }

        public LengthFilterNode(XElement node)
            : base(node)
        {
            this.Load(node);
        }

        public override void Load(XElement node)
        {
            if (node == null) return;
            base.Load(node);

            this.Minimum = FilterPropertyInteger(node, "min", "data", 0);
            this.Maximum = FilterPropertyInteger(node, "max", "data", 0);
        }

        public override void Store(XElement node)
        {
            if (node == null) return;
            base.Store(node);

            node.Add(new XElement("min", new XAttribute("data", this.Minimum.ToString())));
            node.Add(new XElement("max", new XAttribute("data", this.Maximum.ToString())));
        }

        public override string ToString(int pad, String operation = "")
        {
            String criteria = String.Format("{0}{1}{2} {3}min:{4} max:{5}",
                " ".PadRight(pad * FilterNode.PadWidth),
                operation,
                LengthFilterNode.DisplayName,
                (this.Inverted) ? "! " : "",
                this.Minimum,
                this.Maximum);
            return criteria + base.ToString(pad);
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }

    public class PatternFilterNode
        : FilterNode
    {
        new public static readonly String ClassName = "PatternFilterNode";
        new public static readonly String DisplayName = "Pattern";

        public PatternTypes PatternType { get; set; }
        public String Pattern { get; set; }
        public int StartOffset { get; set; }
        public int EndOffset { get; set; }
        public bool CaseSensitive { get; set; }

        public PatternFilterNode()
            : base()
        {
            this.PatternType = PatternTypes.ASCII;
            this.Pattern = "";
            this.StartOffset = 0;
            this.EndOffset = 1517;
            this.CaseSensitive = false;
        }

        public PatternFilterNode(XElement node)
            : base(node)
        {
            this.Load(node);
        }

        public override void Load(XElement node)
        {
            if (node == null) return;
            base.Load(node);

            this.PatternType = (PatternTypes)FilterPropertyInteger(node, "patterntype", "data", 0);
            this.Pattern = FilterPropertyString(node, "patterndata", "data", "");
            this.StartOffset = FilterPropertySigned(node, "startoffset", "data", 0);    // Can be -1.
            this.EndOffset = FilterPropertySigned(node, "endoffset", "data", 0);        // Can be -1.
            this.CaseSensitive = FilterPropertyBool(node, "casesensitive", "data", false); ;
        }

        public override void Store(XElement node)
        {
            if (node == null) return;
            base.Store(node);

            node.Add(new XElement("patterntype", new XAttribute("data", this.PatternType.GetHashCode().ToString())));
            node.Add(new XElement("patterndata", new XAttribute("data", this.Pattern.ToString())));
            node.Add(new XElement("startoffset", new XAttribute("data", this.StartOffset.ToString())));
            node.Add(new XElement("endoffset", new XAttribute("data", this.EndOffset.ToString())));
            node.Add(new XElement("casesensitive", new XAttribute("data", OmniScript.PropBoolean(this.CaseSensitive))));
        }

        public override string ToString(int pad, String operation = "")
        {
            String pattern = this.Pattern;
            switch (this.PatternType)
            {
                case PatternTypes.ASCII:
                    pattern = "ASCII: " + AsciiHexToString(this.Pattern);
                    break;

                case PatternTypes.EBCDIC:
                    pattern = "EBCDIC: " + AsciiHexToEbcdic(this.Pattern);
                    break;

                case PatternTypes.RawData:
                    pattern = "Raw: " + this.Pattern;
                    break;

                case PatternTypes.Regex:
                    pattern = "Regex: " + this.Pattern;
                    break;

                case PatternTypes.Unicode:
                    pattern = "Unicode: " + UnicodeHexToString(this.Pattern);
                    break;
            }
            String criteria = String.Format("{0}{1}{2} {3}{4} begin:{5} end:{6}",
                " ".PadRight(pad * FilterNode.PadWidth),
                operation,
                PatternFilterNode.DisplayName,
                (this.Inverted) ? "! " : "",
                pattern,
                this.StartOffset,
                this.EndOffset);
            return criteria + base.ToString(pad);
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }


    public class PluginFilterNode
        : FilterNode
    {
        new public static readonly String ClassName = "PluginFilterNode";
        new public static readonly String DisplayName = "Plugin";

        public OmniList<OmniId> Ids { get; set; }

        public PluginFilterNode()
            : base()
        {
            this.Ids = new OmniList<OmniId>();
        }

        public PluginFilterNode(XElement node)
            : base(node)
        {
            this.Load(node);
        }

        public override void Load(XElement node)
        {
            if (node == null) return;
            base.Load(node);
            IEnumerable<XElement> pluginIds = node.Elements("pluginid");
            foreach (XElement element in pluginIds)
            {
                XAttribute clsid = element.Attribute("clsid");
                if (clsid != null)
                {
                    OmniId id = new OmniId(clsid.Value.ToString());
                    this.Ids.Add(id);
                }
            }
        }

        public override void Store(XElement node)
        {
            if (node == null) return;
            base.Store(node);
            foreach(OmniId id in this.Ids)
            {
                node.Add(new XElement("pluginid",
                    new XAttribute("clsid", id.ToString())));
            }
        }

        public override string ToString(int pad, String operation = "")
        {
            String criteria = String.Format("{0}{1}{2} {3}{4}",
                " ".PadRight(pad * FilterNode.PadWidth),
                operation,
                PluginFilterNode.DisplayName,
                (this.Inverted) ? "! " : "",
                String.Join(" ", this.Ids));
            return criteria + base.ToString(pad);
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }

    public class PortFilterNode
        : FilterNode
    {
        new public static readonly String ClassName = "PortFilterNode";
        new public static readonly String DisplayName = "Port";

        public OmniPortList Port1 { get; set; }
        public OmniPortList Port2 { get; set; }
        public bool Accept1To2 { get; set; }
        public bool Accept2To1 { get; set; }


        public PortFilterNode()
            : base()
        {
            this.Port1 = null;
            this.Port2 = null;
            this.Accept1To2 = true;
            this.Accept2To1 = true;
        }

        public PortFilterNode(XElement node)
            : base(node)
        {
            this.Load(node);
        }

        public override void Load(XElement node)
        {
            if (node == null) return;
            base.Load(node);

            XElement port1 = node.Element("Sport1");
            if (port1 != null)
            {
                this.Port1 = OmniPortList.Parse(port1);
            }
            XElement port2 = node.Element("Sport2");
            if (port2 != null)
            {
                this.Port2 = OmniPortList.Parse(port2);
            }

            this.Accept1To2 = FilterPropertyBool(node, "accept1to2", "data", this.Accept1To2);
            this.Accept2To1 = FilterPropertyBool(node, "accept2to1", "data", this.Accept2To1);
        }

        public override void Store(XElement node)
        {
            if (node == null) return;
            base.Store(node);

            node.Add(this.Port1.Store("Sport1"));
            if (this.Port2 == null)
            {
                OmniPortList portList = OmniPortList.MakeAny(this.Port1.ElementType());
                node.Add(portList.Store("Sport2"));
            }
            else
            {
                node.Add(this.Port2.Store("Sport2"));
            }
            node.Add(new XElement("accept1to2",
                new XAttribute("data", OmniScript.PropBoolean(this.Accept1To2))));
            node.Add(new XElement("accept2to1",
                new XAttribute("data", OmniScript.PropBoolean(this.Accept2To1))));
        }

        public override string ToString(int pad, String operation = "")
        {
            if (this.Port1 == null) throw new OmniException("Must specify Port1.");
            String criteria = String.Format("{0}{1}{2} {3}{4} {5} {6}",
                " ".PadRight(pad * FilterNode.PadWidth),
                operation,
                PortFilterNode.DisplayName,
                (this.Inverted) ? "! " : "",
                this.Port1.ToString(),
                DirectionString(this.Accept1To2, this.Accept2To1),
                ((this.Port2 == null) || this.Port2.IsEmptyOrNull())
                    ? "Any"
                    : this.Port2.ToString());
            return criteria + base.ToString(pad);
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }

    public class ProtocolFilterNode
        : FilterNode
    {
        new public static readonly String ClassName = "ProtocolFilterNode";
        new public static readonly String DisplayName = "Protocol";

        public uint Protocol { get; set; }
        public bool SliceToHeader { get; set; }
        public String ProtoSpecPath { get; set; }

        public ProtocolFilterNode()
            : base()
        {
            this.Protocol = 0;
            this.SliceToHeader = false;
            this.ProtoSpecPath = null;
        }

        public ProtocolFilterNode(XElement node)
            : base(node)
        {
            if (node == null) return;
            this.Load(node);
        }

        public override void Load(XElement node)
        {
            if (node == null) return;
            base.Load(node);

            byte[] octets = FilterPropertyHex(node, "protocol", "data");
            if (octets != null)
            {
                if (octets.Count() == 4)
                {
                    this.Protocol = BitConverter.ToUInt32(octets, 0);
                }
                else if (octets.Count() == 2)
                {
                    this.Protocol = BitConverter.ToUInt16(octets, 0);
                }
            }
            this.SliceToHeader = FilterPropertyBool(node, "protocol", "slicetoheader", false);
            this.ProtoSpecPath = FilterPropertyString(node, "protocol", "protospecpath", null);
        }

        public void SetProtocol(String protocol)
        {
            OmniScript.NameIndexMap nameMap = OmniScript.GetProtocolShortNameIds();
            this.Protocol = nameMap[protocol];
        }

        public override void Store(XElement node)
        {
            base.Store(node);

            byte[] octets = BitConverter.GetBytes(this.Protocol);
            String protocol = BytesToHexString(octets);
            XElement element = new XElement("protocol",
                new XAttribute("data", protocol),
                new XAttribute("class", "1"),
                new XAttribute("type", "9"),
                new XAttribute("slicetoheader", OmniScript.PropBoolean(this.SliceToHeader)));
            if (this.ProtoSpecPath != null)
            {
                element.Add(new XAttribute("protospecpath", this.ProtoSpecPath));
            }
            node.Add(element);
        }

        public override string ToString(int pad, String operation = "")
        {
            String value = this.Protocol.ToString();
            OmniScript.IndexNameMap idNames = OmniScript.GetProtocolIdNames();
            if (idNames != null && idNames.ContainsKey(this.Protocol))
            {
                value = idNames[this.Protocol];
                if (String.IsNullOrEmpty(value))
                {
                    value = this.Protocol.ToString();
                }
            }
            String criteria = String.Format("{0}{1}{2} {3}{4}",
                " ".PadRight(pad * FilterNode.PadWidth),
                operation,
                ProtocolFilterNode.DisplayName,
                (this.Inverted) ? "! " : "",
                value);
            return criteria + base.ToString(pad);
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }

    public class TimeRangeFilterNode
        : FilterNode
    {
        new public static readonly String ClassName = "TimeRangeFilterNode";
        new public static readonly String DisplayName = "Time Range";

        public PeekTime StartTime { get; set; }
        public PeekTime EndTime { get; set; }

        public TimeRangeFilterNode()
            : base()
        {
            this.StartTime = new PeekTime();
            this.EndTime = new PeekTime();
        }

        public TimeRangeFilterNode(XElement node)
            : base(node)
        {
            if (node == null) return;
            this.Load(node);
        }

        public override void Load(XElement node)
        {
            if (node == null) return;
            base.Load(node);

            this.StartTime = new PeekTime(FilterPropertyLong(node, "start", "data", 0));
            this.EndTime = new PeekTime(FilterPropertyLong(node, "end", "data", 0));
        }

        public void SetEndTime(PeekTime time)
        {
            this.StartTime = time;
        }

        public void SetStartTime(PeekTime time)
        {
            this.EndTime = time;
        }

        public override void Store(XElement node)
        {
            base.Store(node);

            node.Add(new XElement("start", new XAttribute("data", this.StartTime.Time.ToString())));
            node.Add(new XElement("end", new XAttribute("data", this.EndTime.Time.ToString())));
        }

        public override string ToString(int pad, String operation = "")
        {
            String criteria = String.Format("{0}{1}{2}: {3}{4} to {5}",
                " ".PadRight(pad * FilterNode.PadWidth),
                operation,
                TimeRangeFilterNode.DisplayName,
                (this.Inverted) ? "! " : "",
                this.StartTime,
                this.EndTime);
            return criteria + base.ToString(pad);
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }

    public class ValueFilterNode
        : FilterNode
    {
        new public static readonly String ClassName = "ValueFilterNode";
        new public static readonly String DisplayName = "Value";

        public uint Value { get; set; }
        public uint Mask { get; set; }
        public uint Offset { get; set; }
        public uint Length { get; set; }
        public ValueOperators Operator { get; set; }
        public ValueFlags Flags { get; set; }

        public ValueFilterNode()
            : base()
        {
            this.Value = 0;
            this.Mask = 0x0FFFFFFFF;
            this.Offset = 0;
            this.Length = 4;
            this.Operator = ValueOperators.Equal;
            this.Flags = ValueFlags.NetworkByteOrder;
        }

        public ValueFilterNode(XElement node)
            : base(node)
        {
            this.Load(node);
        }

        public override void Load(XElement node)
        {
            if (node == null) return;
            base.Load(node);

            this.Value = FilterPropertyInteger(node, "value", "data", 0);
            this.Mask = FilterPropertyInteger(node, "mask", "data", 0);
            this.Offset = FilterPropertyInteger(node, "offset", "data", 0);
            this.Operator = (ValueOperators)FilterPropertyInteger(node, "op", "data", 0);
            this.Flags = (ValueFlags)FilterPropertyInteger(node, "flags", "data", 0);
        }

        public override void Store(XElement node)
        {
            if (node == null) return;
            base.Store(node);

            node.Add(new XElement("value",
                new XAttribute("data", this.Value.ToString()),
                new XAttribute("type", "2")));
            node.Add(new XElement("mask",
                new XAttribute("data", this.Mask.ToString()),
                new XAttribute("type", "2")));
            node.Add(new XElement("offset", new XAttribute("data", this.Offset.ToString())));
            node.Add(new XElement("op", new XAttribute("data", this.Operator.GetHashCode().ToString())));
            node.Add(new XElement("flags", new XAttribute("data", this.Flags.GetHashCode().ToString())));
        }

        public override string ToString(int pad, String operation = "")
        {
            String criteria = String.Format("{0}{1}{2} {3}{4} off:{5} len:{6}",
                " ".PadRight(pad * FilterNode.PadWidth),
                operation,
                ValueFilterNode.DisplayName,
                (this.Inverted) ? "! " : "",
                this.Value,
                this.Offset,
                this.Length);
            return criteria + base.ToString(pad);
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }

    public class VlanFilterNode
        : FilterNode
    {
        new public static readonly String ClassName = "VlanFilterNode";
        new public static readonly String DisplayName = "Vlan/Mpls";

        public OmniList<String> Ids { get; set; }
        public OmniList<String> Labels { get; set; }

        public VlanFilterNode()
            : base()
        {
            this.Ids = new OmniList<String>();
            this.Labels = new OmniList<String>();
        }

        public VlanFilterNode(XElement node)
            : base(node)
        {
            this.Load(node);
        }

        public override void Load(XElement node)
        {
            if (node == null) return;
            base.Load(node);

            String ids = FilterPropertyString(node, "ids", "data", null);
            if (!String.IsNullOrEmpty(ids))
            {
                this.Ids = new OmniList<String>(ids.Split(' '));
            }
            String labels = FilterPropertyString(node, "labels", "data", null);
            if (!String.IsNullOrEmpty(labels))
            {
                this.Labels = new OmniList<String>(labels.Split(' '));
            }
        }

        public override void Store(XElement node)
        {
            if (node == null) return;
            base.Store(node);

            String ids = String.Join(" ", this.Ids);
            node.Add(new XElement("ids", new XAttribute("data", ids)));
            String labels = String.Join(" ", this.Labels);
            node.Add(new XElement("labels", new XAttribute("data", labels)));
        }

        public override string ToString(int pad, String operation = "")
        {
            String criteria = String.Format("{0}{1}{2} {3}Ids:{4} Labels:{5}",
                " ".PadRight(pad * FilterNode.PadWidth),
                operation,
                VlanFilterNode.DisplayName,
                (this.Inverted) ? "! " : "",
                String.Join(" ", this.Ids),
                String.Join(" ", this.Labels));
            return criteria + base.ToString(pad);
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }

    public class WirelessFilterNode
        : FilterNode
    {
        new public static readonly String ClassName = "WirelessFilterNode";
        new public static readonly String DisplayName = "Wireless";

        public uint ChannelNumber { get; set; }
        public uint ChannelFrequency { get; set; }
        public WirelessBands ChannelBand { get; set; }
        public uint DataRate { get; set; }
        public uint Flags { get; set; }
        public uint SignalMinimum { get; set; }
        public uint SignalMaximum { get; set; }
        public uint NoiseMinimum { get; set; }
        public uint NoiseMaximum { get; set; }
        public uint Encryption { get; set; }
        public uint Decryption { get; set; }
        public OmniAddress Bssid { get; set; }
        public uint MaskBssid { get; set; }

        public WirelessFilterNode()
            : base()
        {
            this.ChannelNumber = 0;
            this.ChannelFrequency = 0;
            this.ChannelBand = WirelessBands.All;
            this.DataRate = 0;
            this.Flags = 0;
            this.SignalMinimum = 0;
            this.SignalMaximum = 0;
            this.NoiseMinimum = 0;
            this.NoiseMaximum = 0;
            this.Encryption = 0;
            this.Decryption = 0;
            this.Bssid = null;
            this.MaskBssid = 0;
        }

        public WirelessFilterNode(XElement node)
            : base(node)
        {
            this.Load(node);
        }

        public override void Load(XElement node)
        {
            if (node == null) return;
            base.Load(node);

            this.ChannelNumber = FilterPropertyInteger(node, "channel", "channelnumber", 0);
            this.ChannelFrequency = FilterPropertyInteger(node, "channel", "channelfrequency", 0);
            this.ChannelBand = (WirelessBands)FilterPropertyInteger(node, "channel", "channelband", 0);
            this.DataRate = FilterPropertyInteger(node, "datarate", "data", 0) / 2;
            this.SignalMinimum = FilterPropertyInteger(node, "signal", "min", 0);
            this.SignalMaximum = FilterPropertyInteger(node, "signal", "max", 0);
            this.NoiseMinimum = FilterPropertyInteger(node, "noise", "min", 0);
            this.NoiseMaximum = FilterPropertyInteger(node, "noise", "max", 0);
            this.Encryption = FilterPropertyInteger(node, "encryption", "data", 0);
            this.Decryption = FilterPropertyInteger(node, "decryption", "data", 0);
            this.Flags = FilterPropertyInteger(node, "flagsn", "data", 0);

            XElement bssid = node.Element("bssid");
            if (bssid != null)
            {
                this.Bssid = OmniAddress.Parse(bssid);
            }
        }

        public override void Store(XElement node)
        {
            if (node == null) return;
            base.Store(node);

            if (this.ChannelNumber != 0)
            {
                node.Add(new XElement("channel",
                    new XAttribute("channelnumber", this.ChannelNumber.ToString()),
                    new XAttribute("channelfrequency", this.ChannelFrequency.ToString()),
                    new XAttribute("channelband", this.ChannelBand.GetHashCode().ToString())));
            }
            if (this.DataRate != 0)
            {
                uint datarate = this.DataRate * 2;
                node.Add(new XElement("datarate", new XAttribute("data", datarate.ToString())));
            }
            if ((this.SignalMinimum > 0) || (this.SignalMaximum > 0))
            {
                node.Add(new XElement("signal",
                    new XAttribute("min", this.SignalMinimum.ToString()),
                    new XAttribute("max", this.SignalMaximum.ToString())));
            }
            if ((this.NoiseMinimum > 0) || (this.NoiseMaximum > 0))
            {
                node.Add(new XElement("noise",
                    new XAttribute("min", this.NoiseMinimum.ToString()),
                    new XAttribute("max", this.NoiseMaximum.ToString())));
            }
            if (this.Encryption > 0)
            {
                node.Add(new XElement("encryption", new XAttribute("data", this.Encryption.ToString())));
            }
            if (this.Decryption > 0)
            {
                node.Add(new XElement("decryptionerror", new XAttribute("data", this.Decryption.ToString())));
            }
            if (this.Bssid != null)
            {
                node.Add(new XElement("bssid", new XAttribute("data", this.Bssid.ToString())));
            }
            if (this.Flags != 0)
            {
                node.Add(new XElement("flagsn", new XAttribute("data", this.Flags.ToString())));
            }
        }

        public override string ToString(int pad, String operation = "")
        {
            String criteria = String.Format("{0}{1}{2} {3}{4}",
                " ".PadRight(pad * FilterNode.PadWidth),
                operation,
                WirelessFilterNode.DisplayName,
                (this.Inverted) ? "! " : "",
                this.ChannelNumber);
            return criteria + base.ToString(pad);
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }
}
