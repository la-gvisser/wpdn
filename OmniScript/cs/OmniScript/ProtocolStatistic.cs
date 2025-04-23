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

    public class ProtocolStatistic
    {
        public String Name;
        public PeekTime FirstTime;
        public PeekTime LastTime;
        public ulong Packets;
        public ulong Bytes;

        public ProtocolStatistic(ReadStream stream, MediaSpec spec)
        {
            this.Name = spec.Report();
            this.FirstTime = null;
            this.LastTime = null;
            this.Packets = 0;
            this.Bytes = 0;

            this.Load(stream);
        }

        private void Load(ReadStream stream)
        {
            if (stream == null) return;

            this.FirstTime = new PeekTime(stream.ReadULong());
            this.LastTime = new PeekTime(stream.ReadULong());
            this.Packets = stream.ReadULong();
            this.Bytes = stream.ReadULong();
        }

        public String Report(char seperator, bool newline)
        {
            uint index = 1;
            String line = String.Format("{1}{0}{2}{0}{3}{0}{4}",
                seperator,
                index,
                this.Name,
                this.Bytes,
                this.Packets);
            if (newline)
            {
                line += Environment.NewLine;
            }
            return line;
        }
    }

    public class ProtocolStatistics : List<ProtocolStatistic>
    {
    }
}
