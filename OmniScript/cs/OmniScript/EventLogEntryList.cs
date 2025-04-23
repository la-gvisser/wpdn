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

    /// <summary>
    /// List of EventLogEntrys.
    /// </summary>
    public class EventLogEntryList
        : OmniList<EventLogEntry>
    {
        public const String RootName = "messages";

        public OmniEngine Engine { get; set; }
        public OmniLogger Logger { get; set; }

        public EventLogEntryList(OmniEngine engine, XElement node)
        {
            this.Engine = engine;
            this.Logger = this.Engine.Logger;
            this.Load(node);
        }

        public void Load(XElement node)
        {
            if (node == null) return;
            IEnumerable<XElement> element = node.Elements("message");
            foreach (XElement obj in element)
            {
                EventLogEntry entry = new EventLogEntry(this.Engine, obj);
                if (entry != null)
                {
                    this.Add(entry);
                }
            }
            // Sort the list.
            this.Sort((x, y) =>
                x.Index.CompareTo(y.Index));
        }

        public uint Merge(EventLogEntryList other)
        {
            if ((other == null) || (other.Count == 0)) return 0;
            if ((this.Count == 0) || (this[this.Count - 1].Index < other[0].Index))
            {
                this.AddRange(other);
                return (uint)other.Count;
            }

            if (other[other.Count - 1].Index < this[0].Index)
            {
                this.InsertRange(0, other);
                return (uint)other.Count;
            }

            int oldCount = this.Count;
            int this_i = 0;
            int other_i = 0;

            while (this[this_i].Index < other[other_i].Index) this_i++;

            while ((this_i < this.Count) && (other_i < other.Count))
            {
                if (other[other_i].Index == this[this_i].Index)
                {
                    other_i++;
                }
                else if (other[other_i].Index < this[this_i].Index)
                {
                    this.Insert(this_i, other[other_i++]);
                }
                else
                {
                    this_i++;
                }
            }

            if (other_i >= other.Count) return (uint)(this.Count - oldCount);

            if (this_i >= this.Count)
            {
                this.AddRange(other.GetRange(other_i, (other.Count - other_i)));
            }

            return (uint)(this.Count - oldCount);
        }
    }
}
