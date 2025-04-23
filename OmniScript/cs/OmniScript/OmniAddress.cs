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
    using System.Globalization;
    using System.Linq;
    using System.Net;
    using System.Net.NetworkInformation;
    using System.Net.Sockets;
    using System.Text;
    using System.Xml.Linq;

    public enum AddressTypes
    {
        Unknown,
        Ethernet = 10,
        TokenRing,
        LAP,
        Wireless,
        AppleTalk = 20,
        IP,
        DECnet,
        Other,
        IPv6,
        IPX
    }

    public class AddressClass
    {
        public uint Class
        {
            get { return 2; }
        }

        public AddressClass()
        {
        }

        public override String ToString()
        {
            return this.Class.ToString();
        }
    }

    public class AddressMask
    {
        static public bool IsNullOrEmpty(AddressMask mask)
        {
            return (mask == null) || (mask.Mask == 0);
        }

        public uint Mask { get; set; }

        public AddressMask(uint mask)
        {
            this.Mask = mask;
        }

        public override String ToString()
        {
            return this.Mask.ToString();
        }

        public String ToString(String format)
        {
            if (format == "F")
            {
                return OmniScript.BitCountToMask((int)this.Mask).ToString();
            }
            return this.Mask.ToString();
        }
    }

    public abstract class OmniAddress
    {
        public static readonly AddressClass Class = new AddressClass();

        public static OmniAddress Parse(XElement node)
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
            
            OmniAddress address = null;

            switch (type)
            {
                case AddressTypes.Ethernet:
                    {
                        EthernetAddressList addresses = EthernetAddressList.Parse(series, addressMask, data);
                        address = (EthernetAddressList.IsNullOrEmpty(addresses)) ? null : addresses.addresses[0];
                    }
                    break;

                case AddressTypes.AppleTalk:
                    {
                        AppleTalkAddressList addresses = AppleTalkAddressList.Parse(series, addressMask, data);
                        address = (AppleTalkAddressList.IsNullOrEmpty(addresses)) ? null : addresses.addresses[0];
                    }
                    break;

                case AddressTypes.IP:
                    {
                        IPv4AddressList addresses = IPv4AddressList.Parse(series, addressMask, data);
                        address = (IPv4AddressList.IsNullOrEmpty(addresses)) ? null : addresses.addresses[0];
                    }
                    break;

                case AddressTypes.IPv6:
                    {
                        IPv6AddressList addresses = IPv6AddressList.Parse(series, addressMask, data);
                        address = (IPv6AddressList.IsNullOrEmpty(addresses)) ? null : addresses.addresses[0];
                    }
                    break;

                case AddressTypes.Wireless:
                    {
                        WirelessAddressList addresses = WirelessAddressList.Parse(series, addressMask, data);
                        address = (WirelessAddressList.IsNullOrEmpty(addresses)) ? null : addresses.addresses[0];
                    }
                    break;

                default:
//                    addresses = OtherAddressList.Parse(type, addressMask, data);
                    break;
            }

            return address;
        }

        public static List<String> CleanSplit(String value)
        {
            List<String> result = new List<String>();

            String clean = value.Replace(@"\t", " ");
            clean = clean.Replace(@"\r", " ");
            clean = clean.Replace(@"\n", " ");
            clean = clean.Replace(",", " ");
            foreach (String item in clean.Split(' '))
            {
                if (item.Length > 0)
                {
                    result.Add(item);
                }
            }
            return result;
        }

        public bool Series;
        public AddressTypes Type { get { return AddressTypes.Unknown; } }

        public OmniAddress(bool series)
        {
            this.Series = series;
        }

        public abstract String ToString(String format);
    }

    public class AppleTalkAddress
        : OmniAddress
    {
        new public static readonly AddressTypes Type = AddressTypes.AppleTalk;

        public static String GetNullData()
        {
            return "000000";
        }

        public static AppleTalkAddress Parse(String address)
        {
            String[] octets = address.Split('.');
            if (octets.Length != 2) return null;
            AddressMask mask = null;

            byte[] bytes = new byte[4];
            for (int i = 0; i < 2; ++i)
            {
                if ((octets[i] == "*") && (mask == null))
                {
                    mask = new AddressMask((uint)i * 8);
                    break;
                }
                else
                {
                    bytes[1 - i] = byte.Parse(octets[i]);
                }
            }

            uint value = BitConverter.ToUInt32(bytes, 0);
            return new AppleTalkAddress(value, mask);
        }

        public static AppleTalkAddressList ParseList(String addresses)
        {
            AppleTalkAddressList list = new AppleTalkAddressList();

            List<String> ranges = OmniAddress.CleanSplit(addresses);
            foreach (String item in ranges)
            {
                AppleTalkAddress address = Parse(item);
                if (address != null)
                {
                    list.addresses.Add(address);
                }
            }

            return list;
        }

        public static implicit operator AppleTalkAddressList(AppleTalkAddress address)
        {
            AppleTalkAddressList list = new AppleTalkAddressList();
            list.Add(address);
            return list;
        }

        public uint Value { get; set; }
        public AddressMask Mask { get; set; }

        public AppleTalkAddress()
            : base(false)
        {
            this.Value = 0;
            this.Mask = null;
        }

        public AppleTalkAddress(uint value, AddressMask mask)
            : base(false)
        {
            this.Value = value;
            this.Mask = mask;
        }

        public AppleTalkAddress(byte[] bytes)
            : base(false)
        {
            if (bytes.Length != 3) throw new OmniException("Invalid AppleTalk Address data.");

            byte[] _bytes = new byte[4];
            bytes.CopyTo(_bytes, 1);
            Array.Reverse(_bytes);

            this.Value = BitConverter.ToUInt32(_bytes, 0);
        }

        public AppleTalkAddress(byte[] bytes, AddressMask mask)
            : this(bytes)
        {
            this.Mask = mask;
        }

        public AppleTalkAddress(String address)
            : base(false)
        {
            AppleTalkAddress _address = AppleTalkAddress.Parse(address);
            if (_address != null)
            {
                this.Value = _address.Value;
                this.Mask = _address.Mask;
            }
        }

        public String GetAsData()
        {
            return this.Value.ToString("X6");
        }

        public bool IsNull()
        {
            return (this.Value == 0) && ((this.Mask == null) || (this.Mask.Mask == 0));
        }

        public void StoreTo(XElement node)
        {
            if (this.Mask != null)
            {
                node.Add(new XAttribute("mask", OmniScript.BitCountToMask((int)this.Mask.Mask / 8).ToString()));
            }
            node.Add(new XAttribute("data",
                this.IsNull() ? EthernetAddress.GetNullData() : this.GetAsData()));
        }

        public override String ToString()
        {
            byte[] bytes = BitConverter.GetBytes(this.Value);
            Array.Reverse(bytes);
            uint mask = (AddressMask.IsNullOrEmpty(this.Mask)) ? 0 : this.Mask.Mask / 8;
            mask = (mask == 0) ? 2 : mask;
            String[] octets = new String[2];
            for (uint i = 0; i < 2; ++i)
            {
                octets[i] = (i >= mask) ? "*" : bytes[i + 2].ToString();
            }

            return String.Join(".", octets);
        }

        public override String ToString(String format)
        {
            return this.ToString();
        }
    }

    public class EthernetAddress
        : OmniAddress
    {
        new public static readonly AddressTypes Type = AddressTypes.Ethernet;

        public static String GetNullData()
        {
            return "000000000000";
        }

        public static EthernetAddress Parse(String address)
        {
            String[] octets = address.Split(':');
            if (octets.Length != 6) return null;
            AddressMask mask = null;

            byte[] bytes = new byte[8];
            for (int i = 0; i < 6; ++i)
            {
                if ((octets[i] == "*") && (mask == null))
                {
                    mask = new AddressMask((uint)i * 8);
                    break;
                }
                else
                {
                    bytes[5 - i] = byte.Parse(octets[i], NumberStyles.AllowHexSpecifier);
                }
            }

            ulong value = BitConverter.ToUInt64(bytes, 0);
            return new EthernetAddress(value, mask);
        }

        public static EthernetAddressList ParseList(String addresses)
        {
            EthernetAddressList list = new EthernetAddressList();

            List<String> ranges = OmniAddress.CleanSplit(addresses);
            foreach (String item in ranges)
            {
                EthernetAddress address = Parse(item);
                if (address != null)
                {
                    list.addresses.Add(address);
                }
            }

            return list;
        }

        public static implicit operator EthernetAddressList(EthernetAddress address)
        {
            EthernetAddressList list = new EthernetAddressList();
            list.Add(address);
            return list;
        }

        private ulong Value { get; set; }
        private AddressMask Mask { get; set; }

        public EthernetAddress()
            : base(false)
        {
            this.Value = 0;
            this.Mask = null;
        }

        public EthernetAddress(ulong value, AddressMask mask)
            : base(false)
        {
            this.Value = value;
            this.Mask = mask;
        }

        public EthernetAddress(byte[] bytes)
            : base(false)
        {
            if (bytes.Length != 6) throw new OmniException("Invalid Ethernet Address data.");
                
            byte[] _bytes = new byte[8];
            bytes.CopyTo(_bytes, 2);
            Array.Reverse(_bytes);

            this.Value = BitConverter.ToUInt64(_bytes, 0);
        }

        public EthernetAddress(byte[] bytes, AddressMask mask)
            : this(bytes)
        {
            this.Mask = mask;
        }

        public EthernetAddress(PhysicalAddress address)
            : this(address.GetAddressBytes(), null)
        {
        }

        public EthernetAddress(String address)
            : base(false)
        {
            EthernetAddress _address = EthernetAddress.Parse(address);
            if (_address != null)
            {
                this.Value = _address.Value;
                this.Mask = _address.Mask;
            }
        }

        public String GetAsData()
        {
            return this.Value.ToString("X12");
        }

        public bool IsNull()
        {
            return (this.Value == 0) && ((this.Mask == null) || (this.Mask.Mask == 0));
        }

        public void StoreTo(XElement node)
        {
            if (this.Mask != null)
            {
                node.Add(new XAttribute("mask", OmniScript.BitCountToMask((int)this.Mask.Mask / 8).ToString()));
            }
            node.Add(new XAttribute("data",
                this.IsNull() ? EthernetAddress.GetNullData() : this.GetAsData()));
        }

        public override String ToString()
        {
            byte[] bytes = BitConverter.GetBytes(this.Value);
            Array.Reverse(bytes);
            uint mask = (AddressMask.IsNullOrEmpty(this.Mask)) ? 0 : this.Mask.Mask / 8;
            mask = (mask == 0) ? 6 : mask;
            String[] octets = new String[6];
            for (uint i = 0; i < 6; ++i)
            {
                octets[i] = (i >= mask) ? "*" : bytes[2 + i].ToString("X2");
            }

            return String.Join(":", octets);
        }

        public override String ToString(String format)
        {
            return this.ToString();
        }
    }


    public class IPv4Address
        : OmniAddress
    {
        new public static readonly AddressTypes Type = AddressTypes.IP;

        public static String GetNullData()
        {
            return "00000000";
        }

        public static IPv4Address Parse(String address)
        {
            String[] parts = address.Split('/');
            String[] octets = parts[0].Split('.');
            if (octets.Length != 4) return null;
            AddressMask mask = null;

            byte[] bytes = new byte[4];
            for (int i = 0; i < 4; ++i)
            {
                if ((octets[i] == "*") && (mask == null))
                {
                    mask = new AddressMask((uint)(i * 8));
                    break;
                }
                else
                {
                    bytes[3 - i] = byte.Parse(octets[i]);
                }
            }

            UInt32 value = BitConverter.ToUInt32(bytes, 0);
            // CIDR mask value overrides the wildcard value.
            if (parts.Length > 1)
            {
                mask = new AddressMask(Convert.ToUInt32(parts[1]));
            }

            return new IPv4Address(value, mask);
        }

        public static IPv4AddressList ParseList(String addresses)
        {
            IPv4AddressList list = new IPv4AddressList();

            List<String> ranges = OmniAddress.CleanSplit(addresses);
            foreach (String item in ranges)
            {
                IPv4Address address = Parse(item);
                if (address != null)
                {
                    list.Add(address);
                }
            }

            return list;
        }

        public static implicit operator IPv4AddressList(IPv4Address address)
        {
            IPv4AddressList list = new IPv4AddressList();
            list.Add(address);
            return list;
        }

        private uint Value { get; set; }        // IPAddress
        private AddressMask Mask { get; set; }  // NetMask

        public IPv4Address()
            : base(false)
        {
            this.Value = 0;
            this.Mask = null;
        }

        public IPv4Address(uint value, AddressMask mask)
            : base(false)
        {
            this.Value = value;
            this.Mask = mask;
        }

        public IPv4Address(byte[] bytes)
            : base(false)
        {
            if (bytes.Length != 4) throw new OmniException("Invalid IPv4 Address data.");

            Array.Reverse(bytes);
            UInt32 value = BitConverter.ToUInt32(bytes, 0);
            this.Value = value;
        }

        public IPv4Address(byte[] bytes, AddressMask mask)
            : this(bytes)
        {
            this.Mask = mask;
        }

        public IPv4Address(IPAddress address)
            : this(address.GetAddressBytes(), null)
        {
        }

        public IPv4Address(String address)
            : this()
        {
            IPv4Address _address = IPv4Address.Parse(address);
            if (_address  != null)
            {
                this.Value = _address.Value;
                this.Mask = _address.Mask;
            }
        }

        public String GetAsData()
        {
            return this.Value.ToString("X8");
        }

        public bool IsNull()
        {
            return (this.Value == 0) && AddressMask.IsNullOrEmpty(this.Mask);
        }

        public void StoreTo(XElement node)
        {
            if (this.Mask != null)
            {
                node.Add(new XAttribute("mask", OmniScript.BitCountToMask((int)this.Mask.Mask).ToString()));
            }
            node.Add(new XAttribute("data",
                this.IsNull() ? IPv4Address.GetNullData() : this.GetAsData()));
        }

        public override String ToString()
        {
            String mask = (!AddressMask.IsNullOrEmpty(this.Mask)) ? String.Format("/{0}", this.Mask.Mask) : "";
            String str = String.Format("{0}.{1}.{2}.{3}{4}",
                (this.Value >> 24), ((this.Value & 0x00FF0000) >> 16),
                ((this.Value & 0x0000FF00) >> 8), (this.Value & 0x000000FF),
                mask);
            return str;
        }

        public override String ToString(String format)
        {
            return this.ToString();
        }
    }


    public class IPv6Address
        : OmniAddress
    {
        new public static readonly AddressTypes Type = AddressTypes.IPv6;

        public static String GetNullData()
        {
            return "00000000000000000000000000000000";
        }

        public static IPv6Address Parse(String address)
        {
            String[] hextets = address.Split(':');
            if (hextets.Length != 8) return null;
            AddressMask mask = null;

            byte[] bytes = new byte[16];
            for (int i = 0; i < 8; ++i)
            {
                if ((hextets[i] == "*") && (mask == null))
                {
                    mask = new AddressMask((uint)i * 2);
                    break;
                }
                else
                {
                    ushort value = ushort.Parse(hextets[i], NumberStyles.AllowHexSpecifier);
                    bytes[i * 2] = (byte)(value >> 8);
                    bytes[(i * 2) + 1] = (byte)(value & 0x00FF);
                }
            }

            return new IPv6Address(bytes, mask);
        }

        public static IPv6AddressList ParseList(String addresses)
        {
            IPv6AddressList list = new IPv6AddressList();

            List<String> ranges = OmniAddress.CleanSplit(addresses);
            foreach (String item in ranges)
            {
                IPv6Address address = Parse(item);
                if (address != null)
                list.addresses.Add(address);
            }

            return list;
        }

        public static implicit operator IPv6AddressList(IPv6Address address)
        {
            IPv6AddressList list = new IPv6AddressList();
            list.Add(address);
            return list;
        }

        public static String CompressHextets(String[] hextets)
        {
            int bestDoubleColonStart = -1;
            int bestDoubleColonLength = 0;
            int bestDoubleColonEnd = hextets.Count() + 1;
            int doubleColonStart = -1;
            int doubleColonLength = 0;
            int index = 0;
            foreach (String hex in hextets)
            {
                if (hex == "0000")
                {
                    doubleColonLength += 1;
                    if (doubleColonStart == -1)
                    {
                        doubleColonStart = index;
                    }
                    if (doubleColonLength > bestDoubleColonLength)
                    {
                        bestDoubleColonStart = doubleColonStart;
                        bestDoubleColonLength = doubleColonLength;
                    }
                }
                else
                {
                    doubleColonStart = -1;
                    doubleColonLength = 0;
                }
                index++;
            }

            StringBuilder builder = new StringBuilder();
            if (bestDoubleColonLength == 0)
            {
                foreach (String hex in hextets)
                {
                    builder.Append(hex + ":");
                }
                builder.Remove((builder.Length - 1), 1);
            }
            else
            {
                index = 0;
                bestDoubleColonEnd = bestDoubleColonStart + bestDoubleColonLength;
                foreach (String hex in hextets)
                {
                    if (index < bestDoubleColonStart)
                    {
                        builder.Append(hex + ":");
                    }
                    else if (index >= (bestDoubleColonEnd))
                    {
                        builder.Append(":" + hex);
                    }
                    index++;
                }
            }
            return builder.ToString();
        }

        private byte[] Value { get; set; }
        private AddressMask Mask { get; set; }

        public IPv6Address()
            : base(false)
        {
            this.Value = new byte[16];
            this.Mask = null;
        }

        public IPv6Address(byte[] value)
            : base(false)
        {
            this.Value = value;
        }

        public IPv6Address(byte[] value, AddressMask mask)
            : this(value)
        {
            this.Mask = mask;
        }

        public IPv6Address(IPAddress address)
            : base(false)
        {
            if (address.AddressFamily != AddressFamily.InterNetworkV6) throw new ArgumentException();
            this.Value = address.GetAddressBytes();
        }

        public IPv6Address(String address)
            : base(false)
        {
            IPv6Address _address = IPv6Address.Parse(address);
            if (_address != null)
            {
                this.Value = _address.Value;
                this.Mask = _address.Mask;
            }
        }

        public String GetAsData()
        {
            return string.Concat(this.Value.Select(b => b.ToString("X2")));
        }

        public bool IsNull()
        {
            return (this.Value == null || (this.Value.Sum(item => item) == 0)) && (this.Mask.Mask == 0);
        }

        public void StoreTo(XElement node)
        {
            if (this.Mask != null)
            {
                node.Add(new XAttribute("mask", OmniScript.BitCountToMask((int)this.Mask.Mask).ToString()));
            }
            node.Add(new XAttribute("data",
                this.IsNull() ? IPv6Address.GetNullData() : this.GetAsData()));
        }

        public override String ToString()
        {
            byte[] bytes = this.Value;
            uint mask = (AddressMask.IsNullOrEmpty(this.Mask)) ? 0 : this.Mask.Mask / 2;
            mask = (mask == 0) ? 8 : mask;
            
            String[] hextets = new String[8];
            for (uint i = 0; i < 8; ++i)
            {
                if (i >= mask)
                {
                    hextets[i] = "*";
                }
                else
                {
                    ushort hextet = (ushort)((bytes[i * 2] << 8) | bytes[(i * 2) + 1]);
                    hextets[i] = hextet.ToString("X4");
                }
            }

            return String.Join(":", hextets);
//             String str = String.Format("{0}:{1}:{2}:{3}:{4}:{5}:{6}:{7}",
//                 hextets[0], hextets[1], hextets[2], hextets[3], hextets[4], hextets[5], hextets[6], hextets[7]);
//             return str;
        }

        public override String ToString(String format)
        {
            return this.ToString();
        }
    }

    public class OtherAddress
        : OmniAddress
    {
        public static OtherAddress Parse(AddressTypes addressType, AddressMask addressMask, String value)
        {
            OtherAddress address = new OtherAddress(addressType, addressMask);
            address.Address = value;
            return address;
        }

        public String Address { get; set; }
        public AddressMask Mask { get; set; }
        public AddressTypes _type = AddressTypes.Unknown;
        new public AddressTypes Type { get { return this._type; } }

        public OtherAddress(AddressTypes type, AddressMask addressMask)
            : base(false)
        {
            this.Address = "";
            this.Mask = addressMask;
            this._type = type;
        }

        public override String ToString(String format)
        {
            return this.Address;
        }

        public override String ToString()
        {
            return this.ToString(null);
        }
    }

    public class WirelessAddress
        : OmniAddress
    {
        new public static readonly AddressTypes Type = AddressTypes.Wireless;

        public static String GetNullData()
        {
            return "000000000000";
        }

        public static WirelessAddress Parse(String address)
        {
            String[] octets = address.Split(':');
            if (octets.Length != 6) return null;
            AddressMask mask = null;

            byte[] bytes = new byte[8];
            for (int i = 0; i < 6; ++i)
            {
                if ((octets[i] == "*") && (mask == null))
                {
                    mask = new AddressMask((uint)i * 8);
                    break;
                }
                else
                {
                    bytes[5 - i] = byte.Parse(octets[i], NumberStyles.AllowHexSpecifier);
                }
            }

            ulong value = BitConverter.ToUInt64(bytes, 0);
            return new WirelessAddress(value, mask);
        }

        public static WirelessAddressList ParseList(String addresses)
        {
            WirelessAddressList list = new WirelessAddressList();

            List<String> ranges = OmniAddress.CleanSplit(addresses);
            foreach (String item in ranges)
            {
                WirelessAddress address = Parse(item);
                if (address != null)
                {
                    list.addresses.Add(address);
                }
            }

            return list;
        }

        public static implicit operator WirelessAddressList(WirelessAddress address)
        {
            WirelessAddressList list = new WirelessAddressList();
            list.Add(address);
            return list;
        }

        private ulong Value { get; set; }
        private AddressMask Mask { get; set; }

        public WirelessAddress()
            : base(false)
        {
            this.Value = 0;
            this.Mask = null;
        }

        public WirelessAddress(ulong value, AddressMask mask)
            : base(false)
        {
            this.Value = value;
            this.Mask = mask;
        }

        public WirelessAddress(byte[] bytes)
            : base(false)
        {
            if (bytes.Length != 6) throw new OmniException("Invalid Wireless Address data.");

            byte[] _bytes = new byte[8];
            bytes.CopyTo(_bytes, 2);
            Array.Reverse(_bytes);

            this.Value = BitConverter.ToUInt64(_bytes, 0);
        }

        public WirelessAddress(byte[] bytes, AddressMask mask)
            : this(bytes)
        {
            this.Mask = mask;
        }

        public WirelessAddress(PhysicalAddress address)
            : this(address.GetAddressBytes(), null)
        {
        }

        public WirelessAddress(String address)
            : base(false)
        {
            WirelessAddress _address = WirelessAddress.Parse(address);
            if (_address != null)
            {
                this.Value = _address.Value;
                this.Mask = _address.Mask;
            }
        }

        public String GetAsData()
        {
            return this.Value.ToString("X12");
        }

        public bool IsNull()
        {
            return (this.Value == 0) && ((this.Mask == null) || (this.Mask.Mask == 0));
        }

        public void StoreTo(XElement node)
        {
            if (this.Mask != null)
            {
                node.Add(new XAttribute("mask", OmniScript.BitCountToMask((int)this.Mask.Mask / 8).ToString()));
            }
            node.Add(new XAttribute("data",
                this.IsNull() ? EthernetAddress.GetNullData() : this.GetAsData()));
        }

        public override String ToString()
        {
            byte[] bytes = BitConverter.GetBytes(this.Value);
            Array.Reverse(bytes);
            uint mask = (AddressMask.IsNullOrEmpty(this.Mask)) ? 0 : this.Mask.Mask / 8;
            mask = (mask == 0) ? 6 : mask;
            String[] octets = new String[6];
            for (uint i = 0; i < 6; ++i)
            {
                octets[i] = (i >= mask) ? "*" : bytes[2 + i].ToString("X2");
            }

            return String.Join(":", octets);
//             String str = String.Format("{0}:{1}:{2}:{3}:{4}:{5}",
//                 octets[0], octets[1], octets[2], octets[3], octets[4], octets[5]);
//             return str;
        }

        public override String ToString(String format)
        {
            return this.ToString();
        }
    }
}
