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

    public class NodeStatistic
    {
        private MediaSpec Node;

        public PeekTime FirstTimeSent;
        public PeekTime LastTimeSent;
        public PeekTime FirstTimeReceived;
        public PeekTime LastTimeReceived;
        public ulong PacketsSent;
        public ulong BytesSent;
        public ulong PacketsReceived;
        public ulong BytesReceived;
        public ulong BroadcastPackets;
        public ulong BroadcastBytes;
        public ulong MulticastPackets;
        public ulong MulticastBytes;
        public ushort MinPacketSizeSent;
        public ushort MaxPacketSizeSent;
        public ushort MinPacketSizeReceived;
        public ushort MaxPacketSizeReceived;

        public NodeStatistic(MediaSpec node, ReadStream stream)
        {
            this.Node = node;

            this.FirstTimeSent = null;
            this.LastTimeSent = null;
            this.FirstTimeReceived = null;
            this.LastTimeReceived = null;
            this.PacketsSent = 0;
            this.BytesSent = 0;
            this.PacketsReceived = 0;
            this.BytesReceived = 0;
            this.BroadcastPackets = 0;
            this.BroadcastBytes = 0;
            this.MulticastPackets = 0;
            this.MulticastBytes = 0;
            this.MinPacketSizeSent = 0;
            this.MaxPacketSizeSent = 0;
            this.MinPacketSizeReceived = 0;
            this.MaxPacketSizeReceived = 0;

            this.Load(stream);
        }

        private void Load(ReadStream stream)
        {
            if (stream == null) return;

            this.FirstTimeSent = new PeekTime(stream.ReadULong());
            this.LastTimeSent = new PeekTime(stream.ReadULong());
            this.FirstTimeReceived = new PeekTime(stream.ReadULong());
            this.LastTimeReceived = new PeekTime(stream.ReadULong());
            this.PacketsSent = stream.ReadULong();
            this.BytesSent = stream.ReadULong();
            this.PacketsReceived = stream.ReadULong();
            this.BytesReceived = stream.ReadULong();
            this.BroadcastPackets = stream.ReadULong();
            this.BroadcastBytes = stream.ReadULong();
            this.MulticastPackets = stream.ReadULong();
            this.MulticastBytes = stream.ReadULong();
            this.MinPacketSizeSent = stream.ReadUShort();
            this.MaxPacketSizeSent = stream.ReadUShort();
            this.MinPacketSizeReceived = stream.ReadUShort();
            this.MaxPacketSizeReceived = stream.ReadUShort();
        }

        public String Report(OmniScript.NodeColumnIndexes[] columns, char seperator, bool newline)
        {
            String line = this.Node.Report();
            line += seperator.ToString();
            for (int i = 0; i < columns.Length; i++)
            {
                OmniScript.NodeColumnIndexes index = columns[i];
                switch (index)
                {
                    case OmniScript.NodeColumnIndexes.BytesSent:
                        line += this.BytesSent.ToString();
                        break;
                    case OmniScript.NodeColumnIndexes.BytesReceived:
                        line += this.BytesReceived.ToString();
                        break;
                    case OmniScript.NodeColumnIndexes.PacketsSent:
                        line += this.PacketsSent.ToString();
                        break;
                    case OmniScript.NodeColumnIndexes.PacketsReceived:
                        line += this.PacketsReceived.ToString();
                        break;
                    case OmniScript.NodeColumnIndexes.BroadcastPackets:
                        line += this.BroadcastPackets.ToString();
                        break;
                    case OmniScript.NodeColumnIndexes.BroadcastBytes:
                        line += this.BroadcastBytes.ToString();
                        break;
                    case OmniScript.NodeColumnIndexes.MulticastPackets:
                        line += this.MulticastPackets.ToString();
                        break;
                    case OmniScript.NodeColumnIndexes.MulticastBytes:
                        line += this.MulticastBytes.ToString();
                        break;
                    case OmniScript.NodeColumnIndexes.MinSizeSent:
                        line += this.MinPacketSizeSent.ToString();
                        break;
                    case OmniScript.NodeColumnIndexes.MaxSizeSent:
                        line += this.MaxPacketSizeSent.ToString();
                        break;
                    case OmniScript.NodeColumnIndexes.MinSizeReceived:
                        line += this.MinPacketSizeReceived.ToString();
                        break;
                    case OmniScript.NodeColumnIndexes.MaxSizeReceived:
                        line += this.MaxPacketSizeReceived.ToString();
                        break;
                    case OmniScript.NodeColumnIndexes.FirstTimeSent:
                        line += this.FirstTimeSent.ToString();
                        break;
                    case OmniScript.NodeColumnIndexes.LastTimeSent:
                        line += this.LastTimeSent.ToString();
                        break;
                    case OmniScript.NodeColumnIndexes.FirstTimeReceived:
                        line += this.FirstTimeReceived.ToString();
                        break;
                    case OmniScript.NodeColumnIndexes.LastTimeReceived:
                        line += this.LastTimeReceived.ToString();
                        break;
                }
                line += seperator.ToString();
            }
            if (newline)
            {
                line += Environment.NewLine;
            }
            return line;
        }
    }

    public class NodeStatistics : List<NodeStatistic>
    {
    }
}
