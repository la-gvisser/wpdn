// =============================================================================
// <copyright file="Adapter.cs" company="LiveAction, Inc.">
//  Copyright (c) 2018-2021 Savvius, Inc. All rights reserved.
//  Copyright (c) 2015-2019 Savvius, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System;
    using System.Collections.Generic;
    using System.Xml.Linq;

    public class EventLog
    {
        public const String RootName = "properties";

        private OmniEngine Engine { get; set; }
        private OmniApi Connection { get; set; }
        public OmniLogger Logger { get; set; }

        /// <summary>
        /// The optional Context (Capture) Id. Do not modify.
        /// </summary>
        public OmniId ContextId { get; set; }

        /// <summary>
        /// The total number of events that match the query.
        /// </summary>
        public uint Count { get; set; }

        /// <summary>
        /// The time stamp of the first event.
        /// </summary>
        public PeekTime First { get; set; }

        /// <summary>
        /// The time stamp of the last event.
        /// </summary>
        public PeekTime Last { get; set; }

        /// <summary>
        /// The number of informational events.
        /// </summary>
        public uint Informational { get; set; }

        /// <summary>
        /// The number of major events.
        /// </summary>
        public uint Major { get; set; }

        /// <summary>
        /// The number of minor events.
        /// </summary>
        public uint Minor { get; set; }

        /// <summary>
        /// Optional Search String. Do not modify.
        /// </summary>
        public String SearchString { get; set; }

        /// <summary>
        /// The number of severe events.
        /// </summary>
        public uint Severe { get; set; }
        
        /// <summary>
        /// The list of event entries.
        /// </summary>
        public EventLogEntryList EntryList { get; set; }

        /// <summary>
        /// The index of the first item in EntryList from 0 to Count.
        /// </summary>
        public uint FirstEntry { get; set; }

        /// <summary>
        /// The index of the last item in EntryList from 0 to Count.
        /// </summary>
        public uint LastEntry { get; set; }

        public EventLog(OmniEngine engine)
        {
            this.Engine = engine;
            this.Connection = engine.Connection;
            this.Logger = engine.Logger;

            this.ContextId = null;
            this.Count = 0;
            this.First = null;
            this.Last = null;
            this.Informational = 0;
            this.Major = 0;
            this.Minor = 0;
            this.SearchString = null;
            this.Severe = 0;
            this.EntryList = null;
            this.FirstEntry = UInt32.MaxValue;
            this.LastEntry = 0;
        }

        public EventLog(OmniEngine engine, XElement counts, XElement entrys,
                            uint first=1, OmniId id=null, String search=null)
            : this(engine)
        {
            this.ContextId = id;
            this.SearchString = search;
            this.LoadCounts(counts);
            this.EntryList = new EventLogEntryList(this.Engine, entrys);
            if (this.EntryList.Count > 0)
            {
                this.FirstEntry = first;
                this.LastEntry = (uint)(first + this.EntryList.Count);
            }
        }

        public EventLog(OmniEngine engine, XElement counts, XElement entrys,
                            uint first, String search=null)
            : this(engine)
        {
            this.ContextId = null;
            this.SearchString = search;
            this.LoadCounts(counts);
            this.EntryList = new EventLogEntryList(this.Engine, entrys);
            if (this.EntryList.Count > 0)
            {
                this.FirstEntry = first;
                this.LastEntry = (uint)(first + this.EntryList.Count);
            }
        }

        private void LoadCounts(XElement counts)
        {
            if ((counts == null) || (counts.Name.ToString() != "counts")) return;
            IEnumerable<XElement> elements = counts.Elements();
            foreach (XElement element in elements)
            {
                String name = element.Name.ToString();
                switch (name)
                {
                    case "informational":
                        this.Informational = Convert.ToUInt32(element.Value);
                        break;

                    case "minor":
                        this.Minor = Convert.ToUInt32(element.Value);
                        break;

                    case "major":
                        this.Major = Convert.ToUInt32(element.Value);
                        break;

                    case "severe":
                        this.Count = Convert.ToUInt32(element.Value);
                        break;

                    case "total":
                        this.Count = Convert.ToUInt32(element.Value);
                        break;

                    case "first_timestamp":
                        this.First = new PeekTime(Convert.ToUInt64(element.Value));
                        break;

                    case "last_timestamp":
                        this.Last = new PeekTime(Convert.ToUInt64(element.Value));
                        break;
                }
            }
        }

        /// <summary>
        /// Get the next count events. Default is 1.
        /// </summary>
        public uint GetNext(uint count=1)
        {
            if (this.Engine == null)
            {
                throw new OmniException("EventLog has no OmniEngine");
            }

            if (count == 0) return 0;

            EventLog primer = this.Engine.GetEventLog(0, 0, this.ContextId, this.SearchString);
            uint total = primer.Count;
            if (total == 0) return 0;

            int have = ((this.EntryList == null) || (this.EntryList.Count == 0))
                ? 0
                : this.EntryList.Count;
            if (have >= total) return 0;

            EventLog el = this.Engine.GetEventLog(this.LastEntry, count, this.ContextId, this.SearchString);
            uint added = this.Update(el);
            this.LastEntry += added;
            return added;
        }

        /// <summary>
        /// Get count events that are older than the first event in EntryList. Default is 1.
        /// </summary>
        public uint GetPrevious(uint count=1)
        {
            if (this.Engine == null)
            {
                throw new OmniException("EventLog has no OmniEngine");
            }

            if (count == 0) return 0;

            EventLog primer = this.Engine.GetEventLog(0, 0, this.ContextId, this.SearchString);
            uint total = primer.Count;
            if (total == 0) return 0;

            int have = ((this.EntryList == null) || (this.EntryList.Count == 0))
                ? 0
                : this.EntryList.Count;
            if (have >= total) return 0;

            if (this.FirstEntry > total)
            {
                this.FirstEntry = total;
            }

            uint first = (this.FirstEntry > count) ? (this.FirstEntry - count) : 0;
            EventLog el = this.Engine.GetEventLog(first, count, this.ContextId, this.SearchString);
            uint added = this.Update(el);
            this.FirstEntry -= added;
            return added;
        }

        /// <summary>
        /// Get the remaining 'next' events.
        /// </summary>
        public uint Refresh()
        {
            return GetNext(UInt32.MaxValue);
        }

        public EventLog Search(String search, uint first=0, int count=Int32.MaxValue)
        {
            if (this.Engine == null)
            {
                throw new OmniException("EventLog has no OmniEngine");
            }

            uint getFirst = first;
            uint getCount = (uint)((count > 0) ? count : -count);

            if (count < 0)
            {
                EventLog primer = this.Engine.GetEventLog(0, 0, this.ContextId, this.SearchString);
                if ((primer != null) && (primer.Count > getCount))
                {
                    getFirst = primer.Count - getCount;
                }
            }

            return this.Engine.GetEventLog(getFirst, getCount, this.ContextId, search);
        }

        /// <summary>
        /// Update (Merge/Union) with another EventLog.
        /// </summary>
        private uint Update(EventLog el)
        {
            if (el == null) return 0;
            if (el.Engine != this.Engine)
            {
                throw new OmniException("EventLog can't merge EventLog from different OmniEngine");
            }
            this.Count = el.Count;
            this.Last = el.Last;
            return this.EntryList.Merge(el.EntryList);
        }
    }
}
