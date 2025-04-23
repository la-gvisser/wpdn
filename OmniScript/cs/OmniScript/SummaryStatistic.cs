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

    public class SummaryStatistic
    {
        public const int PacketIndex = 0;
        public const int ByteIndex = 1;
        public const int ValueIndex = 2;

        public enum StatValueKind
        {
            Zero,
            Date,
            Time,
            Duration,
            Packets,
            Bytes,
            Pair,
            Int,
            Double
        }

        public class StatValue
        {
            public StatValueKind Kind;
            public Object Value;

            public StatValue(StatValueKind kind, byte[] value)
            {
                this.Kind = kind;
                this.Value = null;

                switch (kind)
                {
                    case StatValueKind.Zero:
                        this.Value = 0;
                        break;

                    case StatValueKind.Date:
                        this.Value = new PeekTime(BitConverter.ToUInt64(value, 0));
                        break;

                    case StatValueKind.Time:
                        this.Value = new PeekTime(BitConverter.ToUInt64(value, 0));
                        break;

                    case StatValueKind.Duration:
                        this.Value = new PeekTime(BitConverter.ToUInt64(value, 0));
                        break;

                    case StatValueKind.Packets:
                        this.Value = BitConverter.ToUInt64(value, 0);
                        break;

                    case StatValueKind.Bytes:
                        this.Value = BitConverter.ToUInt64(value, 0);
                        break;

                    case StatValueKind.Pair:
                        ulong packets = BitConverter.ToUInt64(value, 0);
                        ulong bytes = BitConverter.ToUInt64(value, 8);
                        this.Value = new ulong[2] { packets, bytes };
                        break;

                    case StatValueKind.Int:
                        this.Value = BitConverter.ToUInt32(value, 0);
                        break;

                    case StatValueKind.Double:
                        this.Value = BitConverter.ToDouble(value, 0);
                        break;

                    default:
                        break;
                }
            }

            public override String ToString()
            {
                switch (this.Kind)
                {
                    case StatValueKind.Zero:
                        return "0";

                    case StatValueKind.Date:
                        return (this.Value as PeekTime).ToString();

                    case StatValueKind.Time:
                        return (this.Value as PeekTime).ToString();

                    case StatValueKind.Duration:
                        return this.Value.ToString();

                    case StatValueKind.Packets:
                        return this.Value.ToString();

                    case StatValueKind.Bytes:
                        return this.Value.ToString();

                    case StatValueKind.Pair:
                        ulong[] uls = this.Value as ulong[];
                        return uls[PacketIndex].ToString() + "," + uls[ByteIndex].ToString();

                    case StatValueKind.Int:
                        return this.Value.ToString();

                    case StatValueKind.Double:
                        return this.Value.ToString();

                    default:
                        return null;
                }
            }
        }

        private String NameLookup(OmniId id, Dictionary<OmniId, String> names)
        {
            if ((id == null) || (id.Id == null)) return null;

            if (names.ContainsKey(id))
            {
                return names[id];
            }

            if (OmniScript.StatIdNames.ContainsKey(id))
            {
                return OmniScript.StatIdNames[id];
            }

            if (OmniScript.ClassIdNames.ContainsKey(id))
            {
                return OmniScript.ClassIdNames[id];
            }

            return null;
        }

        public OmniId Id;
        public String Name;
        public uint Flags;
        public String Parent;
        public OmniId ParentId;
        public StatValue Value;

        public SummaryStatistic(ReadStream stream, Dictionary<OmniId, String> names)
        {
            this.Id = null;
            this.Name = null;
            this.Flags = 0;
            this.Parent = null;
            this.ParentId = null;
            this.Value = null;

            this.Load(stream, names);
        }

        private void Load(ReadStream stream, Dictionary<OmniId, String> names)
        {
            if (stream == null) return;

            this.Id = stream.ReadGuid();
            this.ParentId = stream.ReadGuid();
            this.Name = stream.ReadUnicode();
            StatValueKind kind = (StatValueKind)stream.ReadUInt();
            this.Flags = stream.ReadUInt();
            byte[] value = stream.Read(16);

            this.Parent = NameLookup(this.ParentId, names);
            this.Value = new StatValue(kind, value);

            if (this.Name.Length == 0)
            {
                this.Name = NameLookup(this.Id, names);
            }
        }

        public String[] PacketBytesValue()
        {
            String[] pbv = new String[3];
            if (this.Value.Kind == StatValueKind.Packets)
            {
                pbv[PacketIndex] = this.ToString();
            }
            else if (this.Value.Kind == StatValueKind.Bytes)
            {
                pbv[ByteIndex] = this.ToString();
            }
            else if (this.Value.Kind == StatValueKind.Pair)
            {
                ulong[] uls = this.Value.Value as ulong[];
                pbv[PacketIndex] = uls[PacketIndex].ToString();
                pbv[ByteIndex] = uls[ByteIndex].ToString();
            }
            else
            {
                pbv[ValueIndex] = this.ToString();
            }
            return pbv;
        }

        public String Report(char seperator, bool newline)
        {
            String[] pbv = this.PacketBytesValue();
            String line = String.Format("{0}{1}{2}{3}{4}{5}{6}",
                this.Parent, seperator,
                this.Name, seperator,
                pbv[PacketIndex], seperator,
                pbv[ByteIndex], seperator,
                pbv[ValueIndex]);
            if (newline)
            {
                line += Environment.NewLine;
            }
            return line;
        }

        public double ToDouble()
        {
            if ((this.Value == null) ||
                (this.Value.Kind != StatValueKind.Double))
            {
                throw new InvalidDataException();
            }

            return (double)this.Value.Value;
        }

        public long ToLong()
        {
            if ((this.Value == null) ||
                (this.Value.Kind != StatValueKind.Int))
            {
                throw new InvalidDataException();
            }

            return (long)this.Value.Value;
        }

        public PeekTime ToTime()
        {
            if ((this.Value == null) ||
                !((this.Value.Kind == StatValueKind.Date) ||
                  (this.Value.Kind == StatValueKind.Time)))
            {
                throw new InvalidDataException();
            }

            return this.Value.Value as PeekTime;
        }

        public ulong ToULong()
        {
            if ((this.Value == null) ||
                !((this.Value.Kind == StatValueKind.Zero) ||
                  (this.Value.Kind == StatValueKind.Duration) ||
                  (this.Value.Kind == StatValueKind.Packets) ||
                  (this.Value.Kind == StatValueKind.Bytes) ||
                  (this.Value.Kind == StatValueKind.Int)))
            {
                throw new InvalidDataException();
            }

            return (ulong)this.Value.Value;
        }

        public ulong[] ToULongPair()
        {
            if ((this.Value == null) ||
                (this.Value.Kind != StatValueKind.Pair))
            {
                throw new InvalidDataException();
            }
            ulong[] uls = this.Value.Value as ulong[];
            ulong[] value = new ulong[2];
            value[0] = uls[0];
            value[1] = uls[1];

            return value;
        }

        public bool TryDouble(ref double value)
        {
            if (this.Value == null) return false;

            if (this.Value.Kind == StatValueKind.Double)
            {
                value = (double)this.Value.Value;
                return true;
            }
            return false;
        }

        public bool TryLong(ref long value)
        {
            if (this.Value == null) return false;

            if (this.Value.Kind == StatValueKind.Int)
            {
                value = (long)this.Value.Value;
                return true;
            }
            return false;
        }

        public bool TryTime(ref PeekTime value)
        {
            if (this.Value == null) return false;

            if ((this.Value.Kind == StatValueKind.Date) ||
                (this.Value.Kind == StatValueKind.Time))
            {
                value = (PeekTime)this.Value.Value;
                return true;
            }
            return false;
        }

        public bool TryULong(ref ulong value)
        {
            if (this.Value == null) return false;

            if ((this.Value.Kind == StatValueKind.Zero) ||
                (this.Value.Kind == StatValueKind.Duration) ||
                (this.Value.Kind == StatValueKind.Packets) ||
                (this.Value.Kind == StatValueKind.Bytes) ||
                (this.Value.Kind == StatValueKind.Int))
            {
                value = (ulong)this.Value.Value;
                return true;
            }
            return false;
        }

        public bool TryULongPair(ref ulong[] value)
        {
            if (this.Value == null) return false;
            if (this.Value.Kind == StatValueKind.Pair)
            {
                ulong[] uls = this.Value.Value as ulong[];
                value = new ulong[2];
                value[0] = uls[0];
                value[1] = uls[1];
                return true;
            }
            return false;
        }
    }

    public class SummaryStatisticItem : Dictionary<String, SummaryStatistic>
    {
    }

    public class SummaryStatistics : Dictionary<String, SummaryStatisticItem>
    {
    }
}
