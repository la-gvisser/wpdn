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
    using System.IO;
    using System.Linq;

    public class StatsEntityTable
    {
        private const ushort Magic = 0x6865;
        private const ushort Version2 = 0x0200;
        private const ushort Version3 = 0x0300;
        private const ushort Version4 = 0x0400;
        private const ushort TableCountV2 = 12;
        private const ushort TableCountV3 = 23;
        private const ushort TableCountV4 = 24;

        private const uint MaskV2 = 0x0FFFFFFF;
        private const uint MaskV3 = 0x00FFFFFF;
        private const uint MaskV4 = 0x00FFFFFF;

        private const int ShiftV2 = 28;
        private const int ShiftV3 = 24;
        private const int ShiftV4 = 24;

        private static readonly Dictionary<uint, uint> Masks = new Dictionary<uint, uint>()
        {
            {Version2, MaskV2},
            {Version3, MaskV3},
            {Version4, MaskV4}
        };

        private static readonly Dictionary<uint, int> Shifts = new Dictionary<uint, int>()
        {
            {Version2, ShiftV2},
            {Version3, ShiftV3},
            {Version4, ShiftV4}
        };

        private enum TableIndex
        {
            EthernetProtocol = 0,
            LSAP,
            SNAP,
            MacControl,
            ProtoSpec,
            Ethernet,
            IP,
            AppleTalk,
            DecNet,
            IPv6,
            Wireless,
            IpX,
            IpPort,
            TcpPortPair,
            WanPPPProto,
            WanFrameRelayProto,
            WanX25Proto,
            WanX25EProto,
            WanU200Proto,
            WanIPARSProto,
            WanQ931Proto,
            TokenRing,
            WanDLCI
        }

        private struct ClassSpec
        {
            public MediaSpec.MediaClasses MediaClass;
            public MediaSpec.MediaSpecs MediaSpec;

            public ClassSpec(MediaSpec.MediaClasses mediaClass, MediaSpec.MediaSpecs mediaSpec)
            {
                this.MediaClass = mediaClass;
                this.MediaSpec = mediaSpec;
            }
        }

        private List<ClassSpec> ClassSpecses = new List<ClassSpec>()
        {
            new ClassSpec(MediaSpec.MediaClasses.Protocol, MediaSpec.MediaSpecs.EthernetProtocol),
            new ClassSpec(MediaSpec.MediaClasses.Protocol, MediaSpec.MediaSpecs.Lsap),
            new ClassSpec(MediaSpec.MediaClasses.Protocol, MediaSpec.MediaSpecs.Snap),
            new ClassSpec(MediaSpec.MediaClasses.Protocol, MediaSpec.MediaSpecs.MacControl),
            new ClassSpec(MediaSpec.MediaClasses.Protocol, MediaSpec.MediaSpecs.ProtoSpec),
            new ClassSpec(MediaSpec.MediaClasses.Address, MediaSpec.MediaSpecs.EthernetAddress),
            new ClassSpec(MediaSpec.MediaClasses.Address, MediaSpec.MediaSpecs.IpAddress),
            new ClassSpec(MediaSpec.MediaClasses.Address, MediaSpec.MediaSpecs.AppletalkAddress),
            new ClassSpec(MediaSpec.MediaClasses.Address, MediaSpec.MediaSpecs.DecnetAddress),
            new ClassSpec(MediaSpec.MediaClasses.Address, MediaSpec.MediaSpecs.IpV6Address),
            new ClassSpec(MediaSpec.MediaClasses.Address, MediaSpec.MediaSpecs.WirelessAddress),
            new ClassSpec(MediaSpec.MediaClasses.Address, MediaSpec.MediaSpecs.IpxAddress),
            new ClassSpec(MediaSpec.MediaClasses.Port, MediaSpec.MediaSpecs.IpPort),
            new ClassSpec(MediaSpec.MediaClasses.Port, MediaSpec.MediaSpecs.TcpPortPair),
            new ClassSpec(MediaSpec.MediaClasses.Protocol, MediaSpec.MediaSpecs.WanPppProtocol),
            new ClassSpec(MediaSpec.MediaClasses.Protocol, MediaSpec.MediaSpecs.WanFramerelayProtocol),
            new ClassSpec(MediaSpec.MediaClasses.Protocol, MediaSpec.MediaSpecs.WanX25Protocol),
            new ClassSpec(MediaSpec.MediaClasses.Protocol, MediaSpec.MediaSpecs.WanX25eProtocol),
            new ClassSpec(MediaSpec.MediaClasses.Protocol, MediaSpec.MediaSpecs.WanU200Protocol),
            new ClassSpec(MediaSpec.MediaClasses.Protocol, MediaSpec.MediaSpecs.WanIparsProtocol),
            new ClassSpec(MediaSpec.MediaClasses.Protocol, MediaSpec.MediaSpecs.WanQ931Protocol),
            new ClassSpec(MediaSpec.MediaClasses.Address, MediaSpec.MediaSpecs.TokenringAddress),
            new ClassSpec(MediaSpec.MediaClasses.Address, MediaSpec.MediaSpecs.WanDlciAddress),
            new ClassSpec(MediaSpec.MediaClasses.Protocol, MediaSpec.MediaSpecs.ApplicationId)
        };

        public ushort Version;
        public List<List<MediaSpec>> Tables;

        public StatsEntityTable(ReadStream stream)
        {
            this.Version = 0;
            this.Tables = new List<List<MediaSpec>>();

            this.Load(stream);
        }

        private void Load(ReadStream stream)
        {
            if (stream == null) return;

            ushort magic = stream.ReadUShort();
            if (magic != Magic) return;

            this.Version = stream.ReadUShort();

            uint tableCount = 0;
            switch (this.Version)
            {
                case Version2:
                    tableCount = TableCountV2;
                    break;
                case Version3:
                    tableCount = TableCountV3;
                    break;
                case Version4:
                    tableCount = TableCountV4;
                    break;
                default:
                    break;
            }

            for (int i = 0; i < tableCount; i++)
            {
                uint count = stream.ReadUInt();
                ushort subSize = stream.ReadUShort();
                List<MediaSpec> items = null;
                if ((i == (int)TableIndex.ProtoSpec) && (this.Version > Version2))
                {
                    if (count > 0)
                    {
                        items = new List<MediaSpec>();
                        for (uint s = 0; s < count; s++)
                        {
                            List<int> SubItem = new List<int>();
                            uint ParentCount = stream.ReadUInt();
                            if (ParentCount == 0)
                            {
                                ParentCount = 1;
                            }
                            for (uint p = 0; p < ParentCount; p++)
                            {
                                items.Add(new MediaSpec(ClassSpecses[i].MediaClass,
                                    ClassSpecses[i].MediaSpec, stream.Read(2)));
                            }
                        }
                    }
                }
                else
                {
                    if ((count * subSize) > 0)
                    {
                        items = new List<MediaSpec>();
                        for (uint s = 0; s < count; s++)
                        {
                            items.Add(new MediaSpec(ClassSpecses[i].MediaClass,
                                ClassSpecses[i].MediaSpec, stream.Read(subSize)));
                        }
                    }
                }
                this.Tables.Add(items);
            }
        }

        public MediaSpec GetEntity(uint id)
        {
            List<MediaSpec> table = this.GetTable(id);
            if (table == null) return null;

            int index = (int)this.GetIndex(id);
            return table[index];
        }

        public uint GetIndex(uint id)
        {
            return (id & Masks[this.Version]);
        }

        public uint GetType(uint id)
        {
            return (id >> Shifts[this.Version]);
        }

        public uint[] GetTypeIndex(uint id)
        {
            return new uint[2] { this.GetIndex(id), this.GetType(id) };
        }

        public List<MediaSpec> GetTable(uint id)
        {
            int type = (int)this.GetType(id);
            if (type >= this.Tables.Count()) return null;

            return this.Tables[type];
        }
    }

    public class OmniCallStatistics
    {
        public enum QualityType
        {
            Good,
            Fair,
            Poor,
            Bad
        };

        //         public Dictionary<QualityType, ulong Count>;

        public ulong AllCalls;
        public ulong OpenCalls;
        public ulong Utilization;
        public ulong Qualities;
        public CallStatistics Items;

        public OmniCallStatistics(ReadStream stream, long offset)
        {
            this.Items = new CallStatistics();
        }

        private void Load(ReadStream stream, long offset)
        {
            if (stream == null) return;

            if (offset > 0)
            {
                stream.Seek(offset, SeekOrigin.Begin);
            }
            OmniId clsid = stream.ReadGuid();
            OmniId objid = stream.ReadGuid();
            if (!clsid.Equals(OmniScript.ClassNameIds["CallStats"])) return;
        }

        public CallStatistics Get()
        {
            return this.Items;
        }
    }

    public class OmniNodeStatistics
    {
        private const uint Magic = 0x6D696C73;
        private const ushort Version1 = 1;
        private const ushort Version2 = 2;

        public uint Resets;
        public ulong TimeLimit;
        public NodeStatistics Items;

        public OmniNodeStatistics(StatsEntityTable entities, ReadStream stream, long offset)
        {
            this.Resets = 0;
            this.TimeLimit = 0;
            this.Items = new NodeStatistics();

            this.Load(entities, stream, offset);
        }

        private void Load(StatsEntityTable entities, ReadStream stream, long offset)
        {
            if (stream == null) return;

            if (offset > 0)
            {
                stream.Seek(offset, SeekOrigin.Begin);
            }
            OmniId clsid = stream.ReadGuid();
            OmniId objid = stream.ReadGuid();
            if (!clsid.Equals(OmniScript.ClassNameIds["NodeStats"])) return;

            ushort version = 0;
            uint magic = stream.ReadUInt();
            if (magic == Magic)
            {
                version = stream.ReadUShort();
            }
            else
            {
                stream.Seek(4, SeekOrigin.Current);
                version = Version1;
            }

            switch (version)
            {
                case Version1:
                    this.Resets = stream.ReadUInt();
                    this.TimeLimit = 0;
                    break;
                case Version2:
                    this.Resets = stream.ReadUInt();
                    this.TimeLimit = stream.ReadULong();
                    break;
                default:
                    break;
            }

            uint count = stream.ReadUInt();
            for (uint s = 0; s < count; s++)
            {
                uint id = stream.ReadUInt();
                MediaSpec ms = entities.GetEntity(id);
                NodeStatistic node = new NodeStatistic(ms, stream);
                this.Items.Add(node);
            }
        }

        public NodeStatistics Get()
        {
            return Items;
        }
    }

    public class OmniProtocolStatistics
    {
        private const uint Magic = 0x6D696C73;
        private const ushort Version1 = 1;
        private const ushort Version2 = 2;

        public ushort Version;
        public uint Resets;
        public ulong TimeLimit;
        public ProtocolStatistics Items;

        public OmniProtocolStatistics(StatsEntityTable entities, ReadStream stream, long offset)
        {
            this.Version = 0;
            this.Resets = 0;
            this.TimeLimit = 0;
            this.Items = new ProtocolStatistics();

            this.Load(entities, stream, offset);
        }

        private void Load(StatsEntityTable entities, ReadStream stream, long offset)
        {
            if (stream == null) return;

            if (offset > 0)
            {
                stream.Seek(offset, SeekOrigin.Begin);
            }
            OmniId clsid = stream.ReadGuid();
            OmniId objid = stream.ReadGuid();
            if (!clsid.Equals(OmniScript.ClassNameIds["ProtocolStats"])) return;

            uint magic = stream.ReadUInt();
            if (magic == Magic)
            {
                this.Version = stream.ReadUShort();
            }
            else
            {
                stream.Seek(4, SeekOrigin.Current);
                this.Version = Version1;
            }

            switch (this.Version)
            {
                case Version1:
                    this.Resets = stream.ReadUInt();
                    this.TimeLimit = 0;
                    break;
                case Version2:
                    this.Resets = stream.ReadUInt();
                    this.TimeLimit = stream.ReadULong();
                    break;
                default:
                    break;
            }

            uint count = stream.ReadUInt();
            for (uint s = 0; s < count; s++)
            {
                uint id = stream.ReadUInt();
                MediaSpec ms = entities.GetEntity(id);
                ProtocolStatistic node = new ProtocolStatistic(stream, ms);
                this.Items.Add(node);
            }
        }

        public ProtocolStatistics Get()
        {
            return Items;
        }
    }

    public class SummarySnapshot
    {
        private const ushort Magic = 0x6DD7;
        private const ushort Version1 = 0x0100;
        private const ushort Version2 = 0x0200;

        public OmniId Id;
        public String Name;
        public SummaryStatistics Items;

        public SummarySnapshot(ReadStream stream)
        {
            this.Id = null;
            this.Name = null;
            this.Items = new SummaryStatistics();

            this.Load(stream);
        }

        private void Load(ReadStream stream)
        {
            if (stream == null) return;

            ushort magic = stream.ReadUShort();
            ushort version = 0;
            if (magic == Magic)
            {
                version = stream.ReadUShort();
            }
            else
            {
                stream.Seek(-2, SeekOrigin.Current);
                version = Version1;
            }

            if (version == Version2)
            {
                this.Id = stream.ReadGuid();
                this.Name = stream.ReadUnicode();
                uint count = stream.ReadUInt();
                Dictionary<OmniId, String> names = new Dictionary<OmniId, String>();
                for (uint i = 0; i < count; i++)
                {
                    SummaryStatistic summary = new SummaryStatistic(stream, names);
                    if (String.IsNullOrEmpty(summary.Parent))
                    {
                        names[summary.Id] = summary.Name;
                    }
                    else
                    {
                        if (!this.Items.ContainsKey(summary.Parent))
                        {
                            this.Items.Add(summary.Parent, new SummaryStatisticItem());
                        }
                        this.Items[summary.Parent][summary.Name] = summary;
                    }
                }
            }
        }

        public SummaryStatistics Get()
        {
            return this.Items;
        }
    }

    public class OmniSummaryStatistics
    {
        public OmniId CurrentId;
        public OmniId Id;
        public List<SummarySnapshot> Items;

        public OmniSummaryStatistics(ReadStream stream, long offset)
        {
            this.CurrentId = null;
            this.Id = null;
            this.Items = new List<SummarySnapshot>();

            this.Load(stream, offset);
        }

        public void Load(ReadStream stream, long offset)
        {
            if (stream == null) return;

            if (offset > 0)
            {
                stream.Seek(offset, SeekOrigin.Begin);
            }
            OmniId clsid = stream.ReadGuid();
            OmniId objid = stream.ReadGuid();
            if (!clsid.Equals(OmniScript.ClassNameIds["SummaryStats"])) return;

            this.CurrentId = stream.ReadGuid();
            uint count = stream.ReadUInt();
            this.Id = stream.ReadGuid();
            for (uint i = 0; i < count; i++)
            {
                SummarySnapshot ss = new SummarySnapshot(stream);
                Items.Add(ss);
            }
        }

        public SummaryStatistics Get()
        {
            if (this.Items.Count == 0) return null;

            return Items[0].Get();
        }
    }

    public class StatsStatsTable
    {
        private StatsEntityTable EntityTable;

        public ReadStream DataStream;
        public long NodeOffset;
        public OmniNodeStatistics NodeStats;
        public long ProtocolOffset;
        public OmniProtocolStatistics ProtocolStats;
        public long SummaryOffset;
        public OmniSummaryStatistics SummaryStats;
        public long CallOffset;
        public OmniCallStatistics CallStats;

        public StatsStatsTable(StatsEntityTable entity, ReadStream stream)
        {
            this.EntityTable = entity;
            this.DataStream = stream;
            this.NodeOffset = 0;
            this.NodeStats = null;
            this.ProtocolOffset = 0;
            this.ProtocolStats = null;
            this.SummaryOffset = 0;
            this.SummaryStats = null;
            this.CallOffset = 0;
            this.CallStats = null;

            this.Load();
        }

        private void Load()
        {
            if (this.DataStream == null) return;

            this.NodeOffset = this.DataStream.Find(OmniScript.ClassNameIds["NodeStats"]);
            this.ProtocolOffset = this.DataStream.Find(OmniScript.ClassNameIds["ProtocolStats"]);
            this.SummaryOffset = this.DataStream.Find(OmniScript.ClassNameIds["SummaryStats"]);
            this.CallOffset = this.DataStream.Find(OmniScript.ClassNameIds["CallStats"]);
        }

        public CallStatistics GetCallStatistics()
        {
            if (this.CallStats == null)
            {
                if ((this.DataStream == null) || (this.CallOffset < 0)) return null;

                this.CallStats = new OmniCallStatistics(this.DataStream, this.NodeOffset);
            }

            if (this.CallStats == null) return null;

            return this.CallStats.Get();
        }

        public NodeStatistics GetNodeStatistics()
        {
            if (this.NodeStats == null)
            {
                if ((this.DataStream == null) || (this.NodeOffset < 0)) return null;

                this.NodeStats = new OmniNodeStatistics(this.EntityTable, this.DataStream, this.NodeOffset);
            }

            if (this.NodeStats == null) return null;

            return this.NodeStats.Get();
        }

        public ProtocolStatistics GetProtocolStatistics()
        {
            if (this.ProtocolStats == null)
            {
                if ((this.DataStream == null) || (this.ProtocolOffset < 0)) return null;

                this.ProtocolStats = new OmniProtocolStatistics(this.EntityTable, this.DataStream, this.ProtocolOffset);
            }

            return this.ProtocolStats.Get();
        }

        public SummaryStatistics GetSummaryStatistics()
        {
            if (this.SummaryStats == null)
            {
                if ((this.DataStream == null) || (this.SummaryOffset < 0)) return null;

                this.SummaryStats = new OmniSummaryStatistics(this.DataStream, this.SummaryOffset);
            }

            if (this.SummaryStats == null) return null;

            return this.SummaryStats.Get();
        }
    }

    public class StatsTuplesTable
    {
        private const ushort Magic = 0x6966;
        private const ushort Version1 = 0x0100;
        private const ushort TableCountV1 = 7;
        private const ushort Version2 = 0x0200;
        private const ushort TableCountV2 = 9;

        public ushort Version;
        public List<int> SubTables;

        public StatsTuplesTable(ReadStream stream)
        {
            this.Version = 0;
            this.SubTables = null;

            this.Load(stream);
        }

        private void Load(ReadStream stream)
        {
            if (stream == null) return;

            ushort magic = stream.ReadUShort();
            if (magic == Magic)
            {
                this.Version = stream.ReadUShort();
            }
            else
            {
                stream.Seek(-1, SeekOrigin.Current);
                this.Version = Version1;
            }

            uint tableCount = 0;
            switch (this.Version)
            {
                case Version1:
                    tableCount = TableCountV1;
                    break;
                case Version2:
                    tableCount = TableCountV2;
                    break;
                default:
                    return;
            }

            // Alloc SubTables = List<>(TableCount)
            for (uint i = 0; i < tableCount; i++)
            {
                uint count = stream.ReadUInt();
                for (uint s = 0; s < count; s++)
                {
                    uint src = stream.ReadUInt();
                    uint dst = stream.ReadUInt();
                    uint protocol = stream.ReadUInt();
                    // SubTables[s] = src, dst, protocol.
                }
            }
        }
    }

    public class StatsContext
    {
        private StatsEntityTable EntityTable;
        private StatsTuplesTable TuplesTable;
        private StatsStatsTable StatsTable;

        public ulong RequestTime { get; set; }
        public uint MediaType { get; set; }
        public uint MediaSubType { get; set; }
        public ulong LinkSpeed { get; set; }
        public PeekTime StartTime { get; set; }
        public PeekTime EndTime { get; set; }
        public ulong TotalPackets { get; set; }
        public ulong TotalBytes { get; set; }

        public StatsContext(byte[] data)
        {
            this.EntityTable = null;
            this.TuplesTable = null;
            this.StatsTable = null;

            this.RequestTime = 0;
            this.MediaType = 0;
            this.MediaSubType = 0;
            this.LinkSpeed = 0;
            this.StartTime = new PeekTime();
            this.EndTime = new PeekTime();
            this.TotalPackets = 0;
            this.TotalBytes = 0;

            this.Load(data);
        }

        private void Load(byte[] data)
        {
            if (data == null) return;

            ReadStream stream = new ReadStream(data);
            this.RequestTime = stream.ReadULong();
            this.MediaType = stream.ReadUInt();
            this.MediaSubType = stream.ReadUInt();
            uint ls_32 = stream.ReadUInt();
            if (ls_32 == 0xABCD1234)
            {
                this.LinkSpeed = stream.ReadULong();
            }
            else
            {
                this.LinkSpeed = ls_32;
            }
            this.StartTime = new PeekTime(stream.ReadULong());
            this.EndTime = new PeekTime(stream.ReadULong());
            this.TotalPackets = stream.ReadULong();
            this.TotalBytes = stream.ReadULong();
            this.EntityTable = new StatsEntityTable(stream);
            this.TuplesTable = new StatsTuplesTable(stream);
            this.StatsTable = new StatsStatsTable(this.EntityTable, stream);
        }

        public CallStatistics GetCallStatistics()
        {
            if (this.StatsTable == null) return null;

            return this.StatsTable.GetCallStatistics();
        }

        public NodeStatistics GetNodeStatistics()
        {
            if (this.StatsTable == null) return null;

            return this.StatsTable.GetNodeStatistics();
        }

        public ProtocolStatistics GetProtocolStatistics()
        {
            if (this.StatsTable == null) return null;

            return this.StatsTable.GetProtocolStatistics();
        }

        public SummaryStatistics GetSumaryStatistics()
        {
            if (this.StatsTable == null) return null;

            return this.StatsTable.GetSummaryStatistics();
        }
    }
}
