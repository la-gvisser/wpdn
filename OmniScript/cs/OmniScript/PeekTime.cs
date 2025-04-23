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

    public class PeekTime
    {
        // Convert from Ansi time (seconds) to Peek Time (nanoseconds).
        static public ulong ansiTimeMultiplier = 1000000000;
        // The adjustment in seconds (Ansi time) from 1/1/1601 to 1/1/1970.
        static public ulong ansiTimeAdjustment = 11644473600;
        // Convert from DateTime (100 nanoseconds) to Peek Time (nanoseconds).
        static public ulong datetimeMultiplier = 100;
        // The adjustment in 100 nanosecond units from 1/1/1 to 1/1/1601.
        static public long datetimeAdjustment = new DateTime(1601, 1, 1, 0, 0, 0).Ticks;

        public static implicit operator ulong(PeekTime time)
        {
            return time.Time;
        }

        public static bool IsNullOrEmpty(PeekTime time)
        {
            if (time == null) return true;
            if (time.Time == 0) return true;
            return false;
        }

        // Number of nanoseconds since 1/1/1601.
        public ulong Time { get; set; }

        public PeekTime(ulong nanoseconds)
        {
            this.Time = nanoseconds;
        }

        public PeekTime()
            : this((ulong)(DateTime.UtcNow.Ticks - PeekTime.datetimeAdjustment) * PeekTime.datetimeMultiplier)
        {
        }

        public PeekTime(DateTime datetime)
            : this((ulong)(datetime.Ticks - PeekTime.datetimeAdjustment) * PeekTime.datetimeMultiplier)
        {
        }

        public PeekTime(String value)
            : this(Convert.ToUInt64(value))
        {
        }

        public ulong ToAnsiTime()
        {
            ulong ansi = this.Time / PeekTime.ansiTimeMultiplier;
            if (ansi > PeekTime.ansiTimeAdjustment)
            {
                ansi -= PeekTime.ansiTimeAdjustment;
            }
            return ansi;
        }

        // System.DateTime is in 100-nanosecond units.
        public DateTime ToDateTime()
        {
            DateTime dt = new DateTime(PeekTime.datetimeAdjustment);    // 1/1/1601
            long thistime = (long)(this.Time / PeekTime.datetimeMultiplier);
            return dt.AddTicks(thistime);
        }

        public TimeSpan ToDuration()
        {
            return new TimeSpan((System.Int64)(this.Time / datetimeMultiplier));
        }

        public String ToString(String format)
        {
            if (format != null)
            {
                return this.ToDateTime().ToString(format);
            }
            return Time.ToString();
        }

        public override String ToString()
        {
            return Time.ToString();
        }
    }
}
