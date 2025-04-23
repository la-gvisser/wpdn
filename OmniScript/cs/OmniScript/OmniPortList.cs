// =============================================================================
// <copyright file="Adapter.cs" company="LiveAction, Inc.">
//  Copyright (c) 2018-2021 Savvius, Inc. All rights reserved.
//  Copyright (c) 2015-2018 Savvius, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Xml.Linq;

    abstract public class OmniPortList
    {
        static public bool IsNullOrEmpty(OmniPortList array)
        {
            return (array == null) || array.IsEmpty();
        }

        static public OmniPortList MakeAny(PortTypes type)
        {
            OmniPortList list = null;
            switch (type)
            {
                case PortTypes.IP:
                    list = new IPPortList();
                    list.Add(new IPPort(0, 0));
                    break;

                default:
                    throw new OmniException("Unsupported Port type.");
            }
            return list;
        }

        static public OmniPortList Parse(XElement node)
        {
            String name = node.Name.ToString();
            bool series = (name[0] == 's') || (name[0] == 'S');

            XAttribute classAttribute = node.Attribute("class");
            AddressClass addressClass = (classAttribute != null)
                ? new AddressClass()
                : null;

            XAttribute typeAttribute = node.Attribute("type");
            PortTypes type = (typeAttribute != null)
                ? (PortTypes)Convert.ToUInt32(typeAttribute.Value)
                : PortTypes.Unknown;

            XAttribute dataAttribute = node.Attribute("data");
            String data = (dataAttribute != null)
                ? dataAttribute.Value.ToString()
                : "";

            XAttribute maskAttribute = node.Attribute("mask");
            uint portMask = (maskAttribute != null)
                ? Convert.ToUInt32(maskAttribute.Value)
                : 0;

            OmniPortList addresses = null;

            switch (type)
            {
                case PortTypes.IP:
                    addresses = IPPortList.Parse(series, portMask, data);
                    break;

                default:
                    throw new OmniException("Unsupported Port type.");
            }

            return addresses;
        }

        abstract public int Count { get; }
        abstract public PortTypes ElementType();

        abstract public void Add(OmniPort item);

        abstract public bool IsEmpty();

        abstract public bool IsEmptyOrNull();

        abstract public XElement Store(String name);

        abstract public String ToString(String format);
    }

    public class IPPortList
        : OmniPortList
    {
        static public IPPortList Parse(bool series, uint portMask, String data)
        {
            IPPortList portList = new IPPortList();
            if (series)
            {
                portList.ports.AddRange(IPPort.ParseList(data).ports.ToList());
            }
            else
            {
                byte[] bytes = FilterNode.HexStringToBytes(data);
                IPPort port;
                if (portMask == 0)
                {
                    port = new IPPort(bytes);
                }
                else
                {
                    uint mask = OmniScript.HighBitsSet(portMask) * 8;
                    port = new IPPort(bytes, mask);
                }
                portList.ports.Add(port);
            }
            return portList;
        }

        public List<IPPort> ports { get; set; }
        override public int Count { get { return this.ports.Count; } }

        public IPPortList()
            : base()
        {
            this.ports = new List<IPPort>();
        }

        public IPPortList(int port)
            : base()
        {
            this.ports = new List<IPPort>();
            this.ports.Add(new IPPort(port));
        }

        public IPPortList(byte[] bytes)
            : this()
        {
            if (bytes != null)
            {
                IPPort port = new IPPort(bytes);
                this.ports.Add(port);
            }
        }

        public IPPortList(String address)
            : this()
        {
            if (!String.IsNullOrEmpty(address))
            {
                this.ports.AddRange(IPPort.ParseList(address).ports.ToList());
            }
        }

        override public void Add(OmniPort item)
        {
            IPPort port = (IPPort)item;
            this.ports.Add(port);
        }

        override public PortTypes ElementType()
        {
            return PortTypes.IP;
        }

        override public bool IsEmpty()
        {
            return (this.ports.Count == 0);
        }

        override public bool IsEmptyOrNull()
        {
            return this.IsEmpty() || ((this.ports.Count == 1) && this.ports[0].IsNull());
        }

        override public String ToString()
        {
            return String.Join(" ", this.ports);
        }

        override public String ToString(String format)
        {
            return this.ToString();
        }

        override public XElement Store(String name)
        {
            // Sport is always used.
            bool isSeries = (Char.ToLower(name[0]) == 's');
            XElement result = new XElement(name);
            result.Add(new XAttribute("class", OmniPort.Class));
            result.Add(new XAttribute("type", Enum.Format(typeof(PortTypes), PortTypes.IP, "D")));
            result.Add(new XAttribute("mask", "36221225472"));  // 0xC0000000
            if (this.IsEmpty())
            {
                result.Add(new XAttribute("data", (isSeries) ? "" : IPPort.GetNullData()));
            }
            else
            {
                if (isSeries)
                {
                    result.Add(new XAttribute("data", String.Join(" ", this.ports)));
                }
                else
                {
                    this.ports[0].StoreTo(result);
                }
            }
            return result;
        }
    }
}
