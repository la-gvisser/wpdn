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
    using System.Net;
    using System.Net.NetworkInformation;
    using System.Xml.Linq;

    abstract public class OmniAddressList
    {
        static public bool IsNullOrEmpty(OmniAddressList array)
        {
            return (array == null) || array.IsEmpty();
        }

        static public OmniAddressList MakeAny(AddressTypes type)
        {
            OmniAddressList list = null;
            AddressMask mask = new AddressMask(0);
            switch (type)
            {
                case AddressTypes.AppleTalk:
                    list = new AppleTalkAddressList();
                    list.Add(new AppleTalkAddress(0, mask));
                    break;

                case AddressTypes.Ethernet:
                    list = new EthernetAddressList();
                    list.Add(new EthernetAddress(0, mask));
                    break;

                case AddressTypes.IP:
                    list = new IPv4AddressList();
                    list.Add(new IPv4Address(0, mask));
                    break;

                case AddressTypes.IPv6:
                    list = new IPv6AddressList();
                    byte[] bytes = new byte[16];
                    list.Add(new IPv6Address(bytes, mask));
                    break;

                case AddressTypes.Wireless:
                    list = new WirelessAddressList();
                    list.Add(new WirelessAddress(0, mask));
                    break;

                default:
                    throw new OmniException("Unsupported Address type.");
                //                     address = new OtherAddress(false, null);
                //                     break;
            }
            return list;
        }

        static public OmniAddressList Parse(XElement node)
        {
            String name = node.Name.ToString();
            bool series = (name[0] == 's') || (name[0] == 'S');

            XAttribute classAttribute = node.Attribute("class");
            AddressClass addressClass = (classAttribute != null)
                ? new AddressClass()
                : null;

            XAttribute typeAttribute = node.Attribute("type");
            AddressTypes type = (typeAttribute != null)
                ? (AddressTypes)Convert.ToUInt32(typeAttribute.Value)
                : AddressTypes.Other;

            XAttribute dataAttribute = node.Attribute("data");
            String data = (dataAttribute != null)
                ? dataAttribute.Value.ToString()
                : "";

            XAttribute maskAttribute = node.Attribute("mask");
            AddressMask addressMask = (maskAttribute != null)
                ? new AddressMask(Convert.ToUInt32(maskAttribute.Value))
                : null;

            OmniAddressList addresses = null;

            switch (type)
            {
                case AddressTypes.Ethernet:
                    addresses = EthernetAddressList.Parse(series, addressMask, data);
                    break;

                case AddressTypes.AppleTalk:
                    addresses = AppleTalkAddressList.Parse(series, addressMask, data);
                    break;

                case AddressTypes.IP:
                    addresses = IPv4AddressList.Parse(series, addressMask, data);
                    break;

                case AddressTypes.IPv6:
                    addresses = IPv6AddressList.Parse(series, addressMask, data);
                    break;

                case AddressTypes.Wireless:
                    addresses = WirelessAddressList.Parse(series, addressMask, data);
                    break;

                default:
                    throw new OmniException("Unsupported Address type.");

//                 default:
//                     addresses = OtherAddressList.Parse(type, addressMask, data);
//                     break;
            }

            return addresses;
        }

        abstract public int Count { get; }
        abstract public AddressTypes ElementType();

        abstract public void Add(OmniAddress item);

        abstract public bool IsEmpty();

        abstract public bool IsEmptyOrNull();

        abstract public XElement Store(String name);

        abstract public String ToString(String format);
    }

    public class AppleTalkAddressList
        : OmniAddressList
    {
        static public AppleTalkAddressList Parse(bool series, AddressMask addressMask, String data)
        {
            AppleTalkAddressList addresses = new AppleTalkAddressList();
            if (series)
            {
                addresses.addresses.AddRange(AppleTalkAddress.ParseList(data).addresses.ToList());
            }
            else
            {
                byte[] bytes = FilterNode.HexStringToBytes(data);
                AppleTalkAddress address;
                if (addressMask == null)
                {
                    address = new AppleTalkAddress(bytes);
                }
                else
                {
                    AddressMask mask = new AddressMask(OmniScript.HighBitsSet(addressMask.Mask) * 8);
                    address = new AppleTalkAddress(bytes, mask);
                }
                addresses.addresses.Add(address);
            }
            return addresses;
        }

        public List<AppleTalkAddress> addresses { get; set; }
        override public int Count { get { return this.addresses.Count; } }

        public AppleTalkAddressList()
            : base()
        {
            this.addresses = new List<AppleTalkAddress>();
        }

        public AppleTalkAddressList(byte[] bytes)
            : this()
        {
            if (bytes != null)
            {
                AppleTalkAddress address = new AppleTalkAddress(bytes);
                this.addresses.Add(address);
            }
        }

        public AppleTalkAddressList(String address)
            : this()
        {
            if (!String.IsNullOrEmpty(address))
            {
                this.addresses.AddRange(AppleTalkAddress.ParseList(address).addresses.ToList());
            }
        }

        override public void Add(OmniAddress item)
        {
            AppleTalkAddress address = (AppleTalkAddress)item;
            this.addresses.Add(address);
        }

        override public AddressTypes ElementType()
        {
            return AddressTypes.AppleTalk;
        }

        override public bool IsEmpty()
        {
            return (this.addresses.Count == 0);
        }

        override public bool IsEmptyOrNull()
        {
            return this.IsEmpty() || ((this.addresses.Count == 1) && this.addresses[0].IsNull());
        }

        override public String ToString()
        {
            return String.Join(" ", this.addresses);
        }

        override public String ToString(String format)
        {
            return this.ToString();
        }

        override public XElement Store(String name)
        {
            bool isSeries = (Char.ToLower(name[0]) == 's');
            XElement result = new XElement(name);
            result.Add(new XAttribute("class", OmniAddress.Class));
            result.Add(new XAttribute("type", Enum.Format(typeof(AddressTypes), AddressTypes.AppleTalk, "D")));
            if (this.IsEmpty())
            {
                result.Add(new XAttribute("data", (isSeries) ? "" : AppleTalkAddress.GetNullData()));
            }
            else
            {
                if (isSeries)
                {
                    result.Add(new XAttribute("data", String.Join(" ", this.addresses)));
                }
                else
                {
                    this.addresses[0].StoreTo(result);
                }
            }
            return result;
        }
    }


    public class EthernetAddressList
        : OmniAddressList
    {
        static public EthernetAddressList Parse(bool series, AddressMask addressMask, String data)
        {
            EthernetAddressList addresses = new EthernetAddressList();
            if (series)
            {
                addresses.addresses.AddRange(EthernetAddress.ParseList(data).addresses.ToList());
            }
            else
            {
                byte[] bytes = FilterNode.HexStringToBytes(data);
                EthernetAddress address;
                if (addressMask == null)
                {
                    address = new EthernetAddress(bytes);
                }
                else
                {
                    AddressMask mask = new AddressMask(OmniScript.HighBitsSet(addressMask.Mask) * 8);
                    address = new EthernetAddress(bytes, mask);
                }
                addresses.addresses.Add(address);
            }
            return addresses;
        }

        public List<EthernetAddress> addresses { get; set; }
        override public int Count { get { return this.addresses.Count; } }

        public EthernetAddressList()
            : base()
        {
            this.addresses = new List<EthernetAddress>();
        }

        public EthernetAddressList(PhysicalAddress physical)
            : this()
        {
            EthernetAddress address = new EthernetAddress(physical);
            this.addresses.Add(address);
        }

        public EthernetAddressList(byte[] bytes)
            : this()
        {
            if (bytes != null)
            {
                EthernetAddress address = new EthernetAddress(bytes);
                this.addresses.Add(address);
            }
        }

        public EthernetAddressList(String address)
            : this()
        {
            if (!String.IsNullOrEmpty(address))
            {
                this.addresses.AddRange(EthernetAddress.ParseList(address).addresses.ToList());
            }
        }

        override public void Add(OmniAddress item)
        {
            EthernetAddress address = (EthernetAddress)item;
            this.addresses.Add(address);
        }

        override public AddressTypes ElementType()
        {
            return AddressTypes.Ethernet;
        }

        override public bool IsEmpty()
        {
            return (this.addresses.Count == 0);
        }

        override public bool IsEmptyOrNull()
        {
            return this.IsEmpty() || ((this.addresses.Count == 1) && this.addresses[0].IsNull());
        }

        override public String ToString()
        {
            return String.Join(" ", this.addresses);
        }

        override public String ToString(String format)
        {
            return this.ToString();
        }

        override public XElement Store(String name)
        {
            bool isSeries = (Char.ToLower(name[0]) == 's');
            XElement result = new XElement(name);
            result.Add(new XAttribute("class", OmniAddress.Class));
            result.Add(new XAttribute("type", Enum.Format(typeof(AddressTypes), AddressTypes.Ethernet, "D")));
            if (this.IsEmpty())
            {
                result.Add(new XAttribute("data", (isSeries) ? "" : EthernetAddress.GetNullData()));
            }
            else
            {
                if (isSeries)
                {
                    result.Add(new XAttribute("data", String.Join(" ", this.addresses)));
                }
                else
                {
                    this.addresses[0].StoreTo(result);
                }
            }
            return result;
        }
    }


    public class IPv4AddressList
        : OmniAddressList
    {
        static public IPv4AddressList Parse(bool series, AddressMask addressMask, String data)
        {
            IPv4AddressList addresses = new IPv4AddressList();
            if (series)
            {
                addresses.addresses.AddRange(IPv4Address.ParseList(data).addresses.ToList());
            }
            else
            {
                byte[] bytes = FilterNode.HexStringToBytes(data);
                IPv4Address address = null;
                if (addressMask == null)
                {
                    address = new IPv4Address(bytes);
                }
                else
                {
                    AddressMask mask = new AddressMask(OmniScript.HighBitsSet(addressMask.Mask));
                    address = new IPv4Address(bytes, mask);
                }
                addresses.Add(address);
            }
            return addresses;
        }

        public List<IPv4Address> addresses { get; set; }
        override public int Count { get { return this.addresses.Count; } }

        public IPv4AddressList()
            : base()
        {
            this.addresses = new List<IPv4Address>();
        }

        public IPv4AddressList(IPAddress ip)
            : this()
        {
            if (ip != null)
            {
                IPv4Address address = new IPv4Address(ip);
                this.addresses.Add(address);
            }
        }

        public IPv4AddressList(byte[] bytes)
            : this()
        {
            if (bytes != null)
            {
                IPv4Address address = new IPv4Address(bytes);
                this.addresses.Add(address);
            }
        }

        public IPv4AddressList(String address)
            : this()
        {
            if (!String.IsNullOrEmpty(address))
            {
                this.addresses.AddRange(IPv4Address.ParseList(address).addresses.ToList());
            }
        }

        override public void Add(OmniAddress item)
        {
            IPv4Address address = (IPv4Address)item;
            this.addresses.Add(address);
        }

        override public AddressTypes ElementType()
        {
            return AddressTypes.IP;
        }

        override public bool IsEmpty()
        {
            return (this.addresses.Count == 0);
        }

        override public bool IsEmptyOrNull()
        {
            return this.IsEmpty() || ((this.addresses.Count == 1) && this.addresses[0].IsNull());
        }

        override public XElement Store(String name)
        {
            bool isSeries = (Char.ToLower(name[0]) == 's');
            XElement result = new XElement(name);
            result.Add(new XAttribute("class", OmniAddress.Class));
            result.Add(new XAttribute("type", Enum.Format(typeof(AddressTypes), AddressTypes.IP, "D")));
            if (this.IsEmpty())
            {
                result.Add(new XAttribute("data", (isSeries) ? "" : IPv4Address.GetNullData()));
            }
            else
            {
                if (isSeries)
                {
                    result.Add(new XAttribute("data", String.Join(" ", this.addresses)));
                }
                else
                {
                    this.addresses[0].StoreTo(result);
                }
            }
            return result;
        }

        override public String ToString()
        {
            return String.Join(" ", this.addresses);
        }

        override public String ToString(String format)
        {
            return this.ToString();
        }
    }

    public class IPv6AddressList
        : OmniAddressList
    {
        static public IPv6AddressList Parse(bool series, AddressMask addressMask, String data)
        {
            IPv6AddressList addresses = new IPv6AddressList();
            if (series)
            {
                addresses.addresses.AddRange(IPv6Address.ParseList(data).addresses.ToList());
            }
            else
            {
                byte[] bytes = FilterNode.HexStringToBytes(data);
                IPv6Address address = null;
                if (addressMask == null)
                {
                    address = new IPv6Address(bytes);
                }
                else
                {
                    AddressMask mask = new AddressMask(OmniScript.HighBitsSet(addressMask.Mask));
                    address = new IPv6Address(bytes, mask);
                }
                addresses.Add(address);
            }
            return addresses;
        }

        public List<IPv6Address> addresses { get; set; }
        override public int Count { get { return this.addresses.Count; } }

        public IPv6AddressList()
            : base()
        {
            this.addresses = new List<IPv6Address>();
        }

        public IPv6AddressList(IPAddress ip)
            : this()
        {
            if (ip != null)
            {
                IPv6Address address = new IPv6Address(ip);
                this.addresses.Add(address);
            }
        }

        public IPv6AddressList(byte[] bytes)
            : this()
        {
            if (bytes != null)
            {
                IPv6Address address = new IPv6Address(bytes);
                this.addresses.Add(address);
            }
        }

        public IPv6AddressList(String address)
            : this()
        {
            if (!String.IsNullOrEmpty(address))
            {
                this.addresses.AddRange(IPv6Address.ParseList(address).addresses.ToList());
            }
        }

        override public void Add(OmniAddress item)
        {
            IPv6Address address = (IPv6Address)item;
            this.addresses.Add(address);
        }

        override public AddressTypes ElementType()
        {
            return AddressTypes.IPv6;
        }

        override public bool IsEmpty()
        {
            return (this.addresses.Count == 0);
        }

        override public bool IsEmptyOrNull()
        {
            return this.IsEmpty() || ((this.addresses.Count == 1) && this.addresses[0].IsNull());
        }

        override public String ToString()
        {
            return String.Join(" ", this.addresses);
        }

        override public String ToString(String format)
        {
            return this.ToString();
        }

        override public XElement Store(String name)
        {
            bool isSeries = (Char.ToLower(name[0]) == 's');
            XElement result = new XElement(name);
            result.Add(new XAttribute("class", OmniAddress.Class));
            result.Add(new XAttribute("type", Enum.Format(typeof(AddressTypes), AddressTypes.IPv6, "D")));
            if (this.IsEmpty())
            {
                result.Add(new XAttribute("data", (isSeries) ? "" : IPv6Address.GetNullData()));
            }
            else
            {
                if (isSeries)
                {
                    result.Add(new XAttribute("data", String.Join(" ", this.addresses)));
                }
                else
                {
                    this.addresses[0].StoreTo(result);
                }
            }
            return result;
        }
    }


    public class WirelessAddressList
        : OmniAddressList
    {
        static public WirelessAddressList Parse(bool series, AddressMask addressMask, String data)
        {
            WirelessAddressList addresses = new WirelessAddressList();
            if (series)
            {
                addresses.addresses.AddRange(WirelessAddress.ParseList(data).addresses.ToList());
            }
            else
            {
                byte[] bytes = FilterNode.HexStringToBytes(data);
                WirelessAddress address;
                if (addressMask == null)
                {
                    address = new WirelessAddress(bytes);
                }
                else
                {
                    AddressMask mask = new AddressMask(OmniScript.HighBitsSet(addressMask.Mask) * 8);
                    address = new WirelessAddress(bytes, mask);
                }
                addresses.addresses.Add(address);
            }
            return addresses;
        }

        public List<WirelessAddress> addresses { get; set; }
        override public int Count { get { return this.addresses.Count; } }

        public WirelessAddressList()
            : base()
        {
            this.addresses = new List<WirelessAddress>();
        }

        public WirelessAddressList(PhysicalAddress physical)
            : this()
        {
            WirelessAddress address = new WirelessAddress(physical);
            this.addresses.Add(address);
        }

        public WirelessAddressList(byte[] bytes)
            : this()
        {
            if (bytes != null)
            {
                WirelessAddress address = new WirelessAddress(bytes);
                this.addresses.Add(address);
            }
        }

        public WirelessAddressList(String address)
            : this()
        {
            if (!String.IsNullOrEmpty(address))
            {
                this.addresses.AddRange(WirelessAddress.ParseList(address).addresses.ToList());
            }
        }

        override public void Add(OmniAddress item)
        {
            WirelessAddress address = (WirelessAddress)item;
            this.addresses.Add(address);
        }

        override public AddressTypes ElementType()
        {
            return AddressTypes.Wireless;
        }

        override public bool IsEmpty()
        {
            return (this.addresses.Count == 0);
        }

        override public bool IsEmptyOrNull()
        {
            return this.IsEmpty() || ((this.addresses.Count == 1) && this.addresses[0].IsNull());
        }

        override public String ToString()
        {
            return String.Join(" ", this.addresses);
        }

        override public String ToString(String format)
        {
            return this.ToString();
        }

        override public XElement Store(String name)
        {
            bool isSeries = (Char.ToLower(name[0]) == 's');
            XElement result = new XElement(name);
            result.Add(new XAttribute("class", OmniAddress.Class));
            result.Add(new XAttribute("type", Enum.Format(typeof(AddressTypes), AddressTypes.Wireless, "D")));
            if (this.IsEmpty())
            {
                result.Add(new XAttribute("data", (isSeries) ? "" : WirelessAddress.GetNullData()));
            }
            else
            {
                if (isSeries)
                {
                    result.Add(new XAttribute("data", String.Join(" ", this.addresses)));
                }
                else
                {
                    this.addresses[0].StoreTo(result);
                }
            }
            return result;
        }
    }
}
