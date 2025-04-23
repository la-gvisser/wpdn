﻿// =============================================================================
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
    using System.IO;
    using System.Net.Sockets;
    using System.Xml.Linq;

    public class OmniScript
    {
        public static uint BitCountToMask(int value)
        {
            return ~((UInt32)0xFFFFFFFF >> value);
        }

        public static uint HighBitsSet(UInt32 value)
        {
            uint count = 0;
            UInt32 mask = 0x80000000;
            while ((value & mask) != 0)
            {
                count++;
                mask >>= 1;
            }
            return count;
        }

        public class IndexNameMap : Dictionary<uint, String>
        {
            public IndexNameMap()
            {
            }

            public IndexNameMap(String filename, IndexNameMap shortMap)
            {
                this.Load(filename, shortMap);
            }

            private void LoadId(XElement node, IndexNameMap shortMap)
            {
                if (node == null) return;

                XElement id = node.Element("PSpecID");
                if (id == null) return;

                uint index = Convert.ToUInt32(id.Value);
                if (!this.ContainsKey(index))
                {
                    XAttribute attrName = node.Attribute("Name");
                    String name = (attrName != null) ? attrName.Value.ToString() : "";
                    this.Add(index, name);

                    XElement attrSName = node.Element("SName");
                    shortMap.Add(index, (attrSName != null)
                        ? attrSName.Value.ToString()
                        : name);
                }

                foreach (XElement pspec in node.Elements("PSpec"))
                {
                    LoadId(pspec, shortMap);
                    LoadSwitch(pspec, shortMap);
                }
            }

            private void LoadSwitch(XElement node, IndexNameMap shortMap)
            {
                if (node == null) return;

                XElement switch_elem = node.Element("Switch");
                if (switch_elem == null) return;

                foreach (XElement pspec in switch_elem.Elements("PSpec"))
                {
                    LoadId(pspec, shortMap);
                }
            }

            public void Load(String filename, IndexNameMap shortMap)
            {
                XDocument doc = XDocument.Load(filename);
                XElement root = doc.Root;
                foreach (XElement spec in root.Elements("PSpec"))
                {
                    XAttribute typeAttribute = spec.Attribute("Type");
                    if (typeAttribute != null)
                    {
                        foreach (XElement pspec in spec.Elements("PSpec"))
                        {
                            LoadId(pspec, shortMap);
                            LoadSwitch(pspec, shortMap);
                        }
                        LoadSwitch(spec, shortMap);
                    }
                    else
                    {
                        LoadId(spec, shortMap);
                    }
                }
            }
        }

        public class IdNameMap : Dictionary<OmniId, String>
        {
            public IdNameMap()
            {
            }

            public IdNameMap(String filename)
                : this()
            {
                this.Load(filename);
            }

            public void Load(String filename)
            {
                if (String.IsNullOrEmpty(filename)) return;

                StreamReader reader = new StreamReader(filename);
                while (reader.Peek() >= 0)
                {
                    String line = reader.ReadLine();
                    //           1         2         3         4
                    // 01234567890123456789012345678901234567890123456789
                    // {C533BF09-F2F4-418D-A626-A5B5F81D725F} Debug
                    this.Add(new OmniId(line.Substring(0, 38)), line.Substring(39));
                }
                reader.Close();
            }
        }

        public class NameIdMap : Dictionary<String, OmniId>
        {
            public NameIdMap()
            {
            }

            public void Load(IdNameMap map)
            {
                foreach (KeyValuePair<OmniId, String> item in map)
                {
                    if (!this.ContainsKey(item.Value))
                    {
                        this.Add(item.Value, item.Key);
                    }
                }
            }
        }

        public class NameIndexMap : Dictionary<String, uint>
        {
            public NameIndexMap()
            {
            }

            public void Load(IndexNameMap map)
            {
                foreach (KeyValuePair<uint, String> item in map)
                {
                    if (!this.ContainsKey(item.Value))
                    {
                        this.Add(item.Value, item.Key);
                    }
                }
            }
        }


        public class CodeNameMap : Dictionary<String, String>
        {
            public CodeNameMap()
            {
            }

            public CodeNameMap(String filename)
            {
                this.Load(filename);
            }

            public void Load(String filename)
            {
                if (String.IsNullOrEmpty(filename)) return;

                StreamReader reader = new StreamReader(filename);
                while (reader.Peek() >= 0)
                {
                    String line = reader.ReadLine();
                    //           1         2         3         4
                    // 01234567890123456789012345678901234567890123456789
                    // Code Name
                    String code = line.Substring(0, 2);
                    String country = line.Substring(3);
                    this.Add(code, country);
                }
                reader.Close();
            }
        }


        private const int INIT_FLAGS_COINIT = 0x0001;
        private const int INIT_FLAGS_PROTOSPEC = 0x0002;
        private const int INIT_FLAGS_EXPERT = 0x0004;

        public const String propBagTrue = "-1";
        public const String propBagFalse = "0";

        public const uint secondsInAMinute = 60;
        public const uint minutesInAHour = 60;
        public const uint secondsInAHour = secondsInAMinute * minutesInAHour;
        public const uint hoursInADay = 24;
        public const uint secondsInADay = secondsInAHour * hoursInADay;

        public const ulong bytesInAKilobyte = 1024;
        public const ulong bytesInAMegabyte = 1024 * 1024;
        public const ulong bytesInAGigabyte = 1024 * 1024 * 1024;

        public const UInt32 OMNI_CONNECT = 1;
        public const UInt32 OMNI_DISCONNECT = 2;
        public const UInt32 OMNI_IS_CONNECTED = 3;
        public const UInt32 OMNI_GET_STATS_CONTEXT = 4;

        public const UInt32 OMNI_GET_VERSION = 100;

        public static readonly IReadOnlyList<uint> IntervalMultiplier = new List<uint>
        {
            0,
            1,
            OmniScript.secondsInAMinute,
            OmniScript.secondsInAHour,
            OmniScript.secondsInADay
        };

        public enum IntervalUnits
        {
            Invalid = 0,
            Seconds,
            Minutes,
            Hours,
            Days
        }

        public enum NodeColumnIndexes
        {
            BytesSent,
            BytesReceived,
            PacketsSent,
            PacketsReceived,
            BroadcastPackets,
            BroadcastBytes,
            MulticastPackets,
            MulticastBytes,
            MinSizeSent,
            MaxSizeSent,
            MinSizeReceived,
            MaxSizeReceived,
            FirstTimeSent,
            LastTimeSent,
            FirstTimeReceived,
            LastTimeReceived
        }

        public static Dictionary<String, IntervalUnits> InvervalUnitsMap = new Dictionary<String, IntervalUnits>()
        {
            {"seconds", IntervalUnits.Seconds},
            {"minutes", IntervalUnits.Minutes},
            {"hours", IntervalUnits.Hours},
            {"days", IntervalUnits.Days}
        };

        public static String ConfigPath;
        public static readonly IdNameMap ClassIdNames;
        public static readonly NameIdMap ClassNameIds;
        public static readonly IdNameMap StatIdNames;
        public static readonly CodeNameMap CoCodeNameMap;

        private static readonly IndexNameMap ProtocolIdNames;
        private static readonly IndexNameMap ProtocolIdShortNames;
        private static readonly NameIndexMap ProtocolShortNameIds;

        public const String DefaultHost = "localhost";
        public const UInt32 DefaultPort = 6367;
        public const String AuthDefault = "Default";
        public const String AuthThirdParty = "ThirdParty";
        public const String DefaultAuth = AuthDefault;

        public class EngineAddress
        {
            public String Host { get; set; }
            public UInt32 Port { get; set; }

            public EngineAddress(String host = OmniScript.DefaultHost, UInt32 port = OmniScript.DefaultPort)
            {
                this.Host = (!String.IsNullOrEmpty(host)) ? host : DefaultHost;
                this.Port = (port > 0) ? port : DefaultPort;
            }
        }

        public class EngineCredentials
        {
            public String Auth { get; set; }
            public String Domain { get; set; }
            public String Account { get; set; }
            public String Password { get; set; }

            public EngineCredentials(String auth, String domain, String account, String password)
            {
                this.Auth = auth;
                this.Domain = domain;
                this.Account = account;
                this.Password = password;
            }

            public EngineCredentials()
                : this(null, null, null, null)
            {
            }
        }

        public static String GetProgramFilesX86()
        {
            String progFiles = System.Environment.GetEnvironmentVariable("ProgramFiles");
            String progFilesX86 = System.Environment.GetEnvironmentVariable("ProgramFiles(x86)");
            if (progFilesX86 == null)
            {
                return progFiles;
            }
            if (progFiles == progFilesX86)
            {
                progFiles = System.Environment.GetEnvironmentVariable("ProgramW6432");
            }

            return progFilesX86;
        }

        public static String GetProgramFiles()
        {
            String progFiles = System.Environment.GetEnvironmentVariable("ProgramFiles");
            String progFilesX86 = System.Environment.GetEnvironmentVariable("ProgramFiles(x86)");
            String progFilesW6432 = System.Environment.GetEnvironmentVariable("ProgramW6432");
            if (progFilesX86 == null)
            {
                return progFiles;
            }
            return progFilesW6432;
        }

        public static XElement Prop(String name, uint type, String value)
        {
            return new XElement("prop",
                new XAttribute("name", name),
                new XAttribute("type", type.ToString()),
                value);
        }

        public static XElement Prop(String name, bool value)
        {
            return OmniScript.Prop(name, 11, OmniScript.PropBoolean(value));
        }

        public static String PropBoolean(bool value)
        {
            if (value)
            {
                return "1";
            }
            return "0";
        }

        public static bool PropBoolean(String value)
        {
            return (((String.IsNullOrEmpty(value)) ? 0 : Convert.ToInt32(value)) != 0);
        }

        public static bool PropIsEnabled(XElement node)
        {
            if (node != null)
            {
                XAttribute enabled = node.Attribute("enabled");
                if (enabled != null)
                {
                    return OmniScript.PropBoolean(enabled.Value);
                }
            }
            return false;
        }

        private HostPlatform GetPlatform(String host, UInt32 port)
        {
            String _host = !String.IsNullOrEmpty(host) ? host : OmniScript.DefaultHost;
            Int32 _port = (Int32)((port > 0) ? port : OmniScript.DefaultPort);

            HostPlatform platform = null;
            byte[] data = new byte[256];
            Socket sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            if (sock != null)
            {
                try
                {
                    sock.Connect(_host, _port);
                    sock.ReceiveTimeout = 30000;    // Set the Receive timeout to 30 seconds.
                    // short:protocol, short:magic, uchar, uchar, short, uint
                    int len = sock.Receive(data);
                    if (len > 11)
                    {
                        ReadStream stream = new ReadStream(data);
                        ushort magic = stream.ReadUShort();
                        ushort version = stream.ReadUShort();
                        byte os = stream.ReadByte();
                        byte b1 = stream.ReadByte();
                        ushort s1 = stream.ReadUShort();
                        uint i1 = stream.ReadUInt();

                        platform = new HostPlatform(magic, version, os);
                    }
                }
                catch (System.Net.Sockets.SocketException)
                {
                    // Timeout...
                }

                if (sock != null)
                {
                    if ( sock.Connected)
                    {
                        sock.Disconnect(false);
                    }
                    ((IDisposable)sock).Dispose();
                }
            }
            sock = null;
            data = null;

            return platform;
        }

        public OmniLogger logger;

        static OmniScript()
        {
            DirectoryInfo infoConfig = new DirectoryInfo(AppDomain.CurrentDomain.BaseDirectory);
            if (infoConfig.Exists)
            {
                ConfigPath = infoConfig.FullName;
            }
            else
            {
                infoConfig = new DirectoryInfo(Path.Combine(GetProgramFiles(), "Savvius", "OmniScript"));
                if (infoConfig.Exists)
                {
                    ConfigPath = infoConfig.FullName;
                }
                else
                {
                    throw new OmniException("Incomplete OmniScript install: configuration path is missing.");
                }
            }
            ClassIdNames = new IdNameMap();
            ClassNameIds = new NameIdMap();
            StatIdNames = new IdNameMap();
            CoCodeNameMap = new CodeNameMap();
            ProtocolIdNames = new IndexNameMap();
            ProtocolIdShortNames = new IndexNameMap();
            ProtocolShortNameIds = new NameIndexMap();
        }

        /// <summary>
        /// Initializes a new instance of the OmniScript class.
        /// </summary>
        public OmniScript(OmniLogger logger, String path)
        {
            if (!String.IsNullOrEmpty(path))
            {
                OmniScript.ConfigPath = path;
            }

            if (OmniScript.ClassIdNames.Count == 0)
            {
                OmniScript.ClassIdNames.Load(Path.Combine(OmniScript.ConfigPath, "_class_ids.txt"));
            }
            if (OmniScript.ClassNameIds.Count == 0)
            {
                OmniScript.ClassNameIds.Load(OmniScript.ClassIdNames);
            }
            if (OmniScript.StatIdNames.Count == 0)
            {
                OmniScript.StatIdNames.Load(Path.Combine(OmniScript.ConfigPath, "_stat_ids.txt"));
            }
            if (OmniScript.CoCodeNameMap.Count == 0)
            {
                OmniScript.CoCodeNameMap.Add("Unknown", "Unknown");
                OmniScript.CoCodeNameMap.Add("Private", "Private Network");
                OmniScript.CoCodeNameMap.Add("Multicast", "Multicast");
                OmniScript.CoCodeNameMap.Load(Path.Combine(OmniScript.ConfigPath, "_co_codes.txt"));
            }

            String initPath = Directory.GetCurrentDirectory();
            Directory.SetCurrentDirectory(OmniScript.ConfigPath);
            OmniApi.Initialize();
            Directory.SetCurrentDirectory(initPath);
            this.logger = logger;
        }

        /// <summary>
        /// Initializes a new instance of the OmniScript class.
        /// </summary>
        public OmniScript()
            : this(new OmniLogger(), null)
        {
        }

        /// <summary>
        /// Initializes a new instance of the OmniScript class.
        /// </summary>
        public OmniScript(OmniLogger logger)
            : this(logger, null)
        {
        }

        /// <summary>
        /// Initializes a new instance of the OmniScript class.
        /// </summary>
        public OmniScript(String path)
            : this(new OmniLogger(), path)
        {
        }

        /// <summary>
        /// Connect to an OmniEngine.
        /// </summary>
        public OmniEngine Connect(EngineAddress address, EngineCredentials credentials)
        {
            return this.Connect(address.Host, address.Port, credentials.Auth,
                credentials.Domain, credentials.Account, credentials.Password);
        }

        /// <summary>
        /// Connect to an OmniEngine.
        /// </summary>
        public OmniEngine Connect(String host, uint port, String auth, String domain, String account, String password)
        {
            try
            {
                OmniEngine engine = CreateEngine(host, port);
                engine.Connect(auth, domain, account, password);
                return engine;
            }
            catch
            {
                return null;
            }
        }

        /// <summary>
        /// Create a new OmniEngine object.
        /// </summary>
        public OmniEngine CreateEngine(String host, uint port)
        {
            HostPlatform platform = this.GetPlatform(host, port);
            return new OmniEngine(this.logger, host, port, platform);
        }

        /// <summary>
        /// Create a new OmniEngine object.
        /// </summary>
        public OmniEngine CreateEngine(EngineAddress address)
        {
            return this.CreateEngine(address.Host, address.Port);
        }

        /// <summary>
        /// Convert IPv4 address to String of hex digits.
        /// </summary>
        public static String IpAddressToHex(String ipAddress)
        {
            String[] fields = ipAddress.Split('.');
            uint[] values = Array.ConvertAll<String, uint>(fields, delegate(String str) { return uint.Parse(str); });
            if (values.Length != 4) return null;

            return String.Format("{0}{1}{2}{3}", values[0].ToString("X2"), values[1].ToString("X2"), values[2].ToString("X2"), values[3].ToString("X2"));
        }

        public static IndexNameMap GetProtocolIdNames()
        {
            if (OmniScript.ProtocolIdNames.Count == 0)
            {
                OmniScript.ProtocolIdNames.Load(
                    Path.Combine(OmniScript.ConfigPath, "pspecs.xml"),
                    OmniScript.ProtocolIdShortNames);
            }
            return OmniScript.ProtocolIdNames;
        }

        public static IndexNameMap GetProtocolIdShortNames()
        {
            OmniScript.GetProtocolIdNames();
            return OmniScript.ProtocolIdShortNames;
        }

        public static NameIndexMap GetProtocolShortNameIds()
        {
            if (OmniScript.ProtocolShortNameIds.Count == 0)
            {
                OmniScript.ProtocolShortNameIds.Load(OmniScript.GetProtocolIdShortNames());
            }
            return OmniScript.ProtocolShortNameIds;
        }

        public static String ParseBooleanResponse(String xml, String tag)
        {
            XDocument doc = XDocument.Parse(xml);
            XElement root = doc.Root;
            XElement msg = root.Element("msg");
            if (msg == null) throw new OmniException("Invalid OmniEngine command response");

            XAttribute hrAttribute = msg.Attribute("hr");
            if (hrAttribute == null) return "0x80004005";
            
            int hr = 0;
            String hrString = hrAttribute.Value;
            if (hrString.Length > 3)
            {
                hr = (hrString.Substring(0, 2) == "0x")
                    ? int.Parse(hrString.Substring(2), NumberStyles.AllowHexSpecifier)
                    : Convert.ToInt32(hrString);
            }
            else
            {
                hr = Convert.ToInt32(hrString);
            }
            if (hr == 0) throw new OmniException("OmniEngine command failed.");
            if (tag == null) return "";

            XElement result = msg.Element(tag);
            return result.ToString();
        }

        public static FilterList ReadFilterFile(String filename)
        {
            FilterList filterList = new FilterList(null);

            XDocument doc = XDocument.Load(filename);
            XElement root = doc.Root;
            if (root != null)
            {
                filterList.Load(root);
            }

            return filterList;
        }
    }
}
