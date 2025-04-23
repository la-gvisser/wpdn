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

    public class CountryStatistic
    {
        public String CountryCode;
        public String CountryName;
        public PeekTime FirstTimeFrom;
        public PeekTime LastTimeFrom;
        public ulong PacketsFrom;
        public ulong BytesFrom;
        public PeekTime FirstTimeTo;
        public PeekTime LastTimeTo;
        public ulong PacketsTo;
        public ulong BytesTo;

        public CountryStatistic(ReadStream stream, MediaSpec spec)
        {
            this.CountryCode = "";
            this.CountryName = "";
            this.FirstTimeFrom = null;
            this.LastTimeFrom = null;
            this.PacketsFrom = 0;
            this.BytesFrom = 0;
            this.FirstTimeTo = null;
            this.LastTimeTo = null;
            this.PacketsTo = 0;
            this.BytesTo = 0;

            this.Load(stream);
        }

        private void Load(ReadStream stream)
        {
            if (stream == null) return;

            this.CountryCode = stream.ReadString(2);
            stream.Read(6); // Padding.
            this.FirstTimeFrom = new PeekTime(stream.ReadULong());
            this.LastTimeFrom = new PeekTime(stream.ReadULong());
            this.PacketsFrom = stream.ReadULong();
            this.BytesFrom = stream.ReadULong();
            this.FirstTimeTo = new PeekTime(stream.ReadULong());
            this.LastTimeTo = new PeekTime(stream.ReadULong());
            this.PacketsTo = stream.ReadULong();
            this.BytesTo = stream.ReadULong();

            this.CountryName = OmniScript.CoCodeNameMap[this.CountryCode];
        }

        public String Report(char seperator, bool newline)
        {
            uint index = 1;
            String line = String.Format("{1}{0}{2}{0}{3}{0}{4}",
                seperator,
                index,
                this.CountryCode,
                this.BytesFrom,
                this.PacketsFrom);
            if (newline)
            {
                line += Environment.NewLine;
            }
            return line;
        }
    }

    public class CountryStatistics : List<ProtocolStatistic>
    {
    }
}
