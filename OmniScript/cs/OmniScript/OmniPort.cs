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

    public enum PortTypes
    {
        Unknown,
        AppleTalk = 33,
        IP,
        NetWare,
        TCPPair
    }

    public class PortClass
    {
        public uint Class
        {
            get { return 3; }
        }

        public PortClass()
        {
        }

        public override String ToString()
        {
            return this.Class.ToString();
        }
    }

    public class PortRange
    {
        public static bool IsNullOrEmpty(PortRange range)
        {
            return ((range == null) || range.IsZero());
        }

        private ushort Range { get; set; }

        public PortRange(ushort range)
        {
            this.Range = range;
        }

        public bool IsZero()
        {
            return (this.Range == 0);
        }

        public String ToString(ushort port)
        {
            return "-" + (port + this.Range).ToString();
        }
    }

    public abstract class OmniPort
    {
        public static readonly PortClass Class = new PortClass();

        public static OmniPort Parse(XElement node)
        {
            OmniPort result = null;
            if ((node != null) && node.HasAttributes)
            {
                String name = node.Name.ToString();
                bool series = (name[0] == 's') || (name[0] == 'S');

                XAttribute typeAttribute = node.Attribute("type");
                PortTypes type = (typeAttribute != null)
                    ? (PortTypes)Convert.ToInt32(typeAttribute.Value)
                    : PortTypes.Unknown;

                XAttribute classAttribute = node.Attribute("class");
                uint addressClass = (classAttribute != null) ? Convert.ToUInt32(classAttribute.Value) : 3;

                XAttribute maskAttribute = node.Attribute("mask");
                uint portMask = (maskAttribute != null) ? Convert.ToUInt32(maskAttribute.Value) : 0;

                XAttribute dataAttribute = node.Attribute("data");
                String data = (dataAttribute != null) ? dataAttribute.Value.ToString() : "";

                switch (type)
                {
                    case PortTypes.IP:
                        {
                            IPPortList list = IPPortList.Parse(series, portMask, data);
                        }
                        break;

                    default:
                        throw new OmniException("Invalid Port Type");
                }
            }
            return result;
        }

        virtual public bool IsRange { get { return false; } }
        public PortTypes Type { get { return PortTypes.Unknown; } }

        public OmniPort()
        {
        }

//         public OmniPort(String ports)
//             : this()
//         {
//             String[] ranges = ports.Split(' ');
//             foreach (String range in ranges)
//             {
//                 this.Port.Add(PortRange.Parse(range));
//             }
//         }

//         public XElement Store(String name)
//         {
//             return new XElement(name,
//                 new XAttribute("class", OmniPort.Class),
//                 new XAttribute("type", Enum.Format(typeof(PortTypes), this.Type, "D")),
//                 new XAttribute("mask", "3221225472"),
//                 new XAttribute("data", String.Join(" ", this.Port)));
//         }

//         public override string ToString()
//         {
//             if (this.Port == null) return "Any";
//             if (this.Port.Count == 0) return "Any";
//             if ((this.Port.Count == 1) && PortRange.IsNullOrEmpty(this.Port[0])) return "Any";
//             return String.Join(" ", this.Port);
//         }

        public abstract String ToString(String format);
    }

    public class IPPort
        : OmniPort
    {
        new public static readonly PortTypes Type = PortTypes.IP;

        public static String GetNullData()
        {
            return "00";
        }

        public static IPPort Parse(String port)
        {
            String[] ports = port.Split('-');
            if (ports.Length == 1)
            {
                return new IPPort(String.IsNullOrEmpty(ports[0])
                    ? (ushort)0
                    : Convert.ToUInt16(ports[0]));
            }
            if (ports.Length == 2)
            {
                if (String.IsNullOrEmpty(ports[1]))
                {
                    return new IPPort(String.IsNullOrEmpty(ports[0])
                        ? (ushort)0
                        : Convert.ToUInt16(ports[0]));
                }
                return new IPPort(
                    String.IsNullOrEmpty(ports[0]) ? (ushort)0 : Convert.ToUInt16(ports[0]),
                    Convert.ToUInt16(ports[1]));
            }
            return new IPPort();
        }

        public static IPPortList ParseList(String ports)
        {
            IPPortList list = new IPPortList();

            List<String> ranges = OmniAddress.CleanSplit(ports);
            foreach (String item in ranges)
            {
                IPPort port = Parse(item);
                if (port != null)
                {
                    list.ports.Add(port);
                }
            }

            return list;
        }

        public static implicit operator IPPortList(IPPort port)
        {
            IPPortList list = new IPPortList();
            list.Add(port);
            return list;
        }

        static public implicit operator IPPort(int port)
        {
            return new IPPort(port);
        }

        static public implicit operator IPPort(uint port)
        {
            return new IPPort(port);
        }

        static public implicit operator IPPort(short port)
        {
            return new IPPort(port);
        }

        static public implicit operator IPPort(ushort port)
        {
            return new IPPort(port);
        }

        static public implicit operator IPPort(String port)
        {
            return new IPPort(port);
        }

        override public bool IsRange { get { return (this.Range != null); } }
        private ushort Value { get; set; }
        private PortRange Range { get; set; }

        public IPPort()
            : base()
        {
            this.Value = 0;
        }

        public IPPort(ushort value)
            : base()
        {
            this.Value = value;
        }

        public IPPort(long value)
            : base()
        {
            if ((value < 0) || (value > 65535)) throw new OmniException("Invalid IP Port value.");
            this.Value = (ushort)value;
        }

        public IPPort(ulong value)
            : base()
        {
            if (value > 65535) throw new OmniException("Invalid IP Port value.");
            this.Value = (ushort)value;
        }

        public IPPort(ushort minPort, ushort maxPort)
            : base()
        {
            if (minPort == maxPort)
            {
                this.Value = minPort;
                this.Range = new PortRange(0);
            }
            else if (minPort < maxPort)
            {
                this.Value = minPort;
                this.Range = new PortRange((ushort)(maxPort - minPort));
            }
            else
            {
                this.Value = maxPort;
                this.Range = new PortRange((ushort)(minPort - maxPort));
            }
        }

        public IPPort(byte[] bytes)
            : base()
        {
            if (bytes.Length != 2) throw new OmniException("Invalid IP Port data.");
            Array.Reverse(bytes);
            this.Value = BitConverter.ToUInt16(bytes, 0);
        }

        public IPPort(byte[] bytes, uint mask)
            : this(bytes)
        {
            this.Range = new PortRange(this.Value);
        }

        public IPPort(String port)
            : base()
        {
            IPPort _port = IPPort.Parse(port);
            if (_port != null)
            {
                this.Value = _port.Value;
                this.Range = _port.Range;
            }
        }

        public String GetAsData()
        {
            return this.Value.ToString("X2");
        }

        public bool IsNull()
        {
            return (this.Value == 0) && ((this.Range == null) || (this.Range.IsZero()));
        }

        public void StoreTo(XElement node)
        {
            node.Add(new XAttribute("data",
                this.IsNull() ? IPPort.GetNullData() : this.GetAsData()));
        }

        public override String ToString()
        {
            if (this.Value == 0 && this.Range != null && this.Range.IsZero())
            {
                return "";
            }

            String result = this.Value.ToString();
            if (this.Range != null)
            {
                result += this.Range.ToString(this.Value);
            }
            return result;
        }

        public override String ToString(String format)
        {
            return this.ToString();
        }
    }
}
