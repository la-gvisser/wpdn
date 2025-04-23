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

    public class EventLogEntry
    {
        public const String RootName = "message";

        private OmniEngine Engine { get; set; }
        private OmniApi Connection { get; set; }
        public OmniLogger Logger { get; set; }

        /// <summary>
        /// The Context (Capture) Id.
        /// </summary>
        public OmniId ContextId { get; set; }

        /// <summary>
        /// The index of the event into the entire Event Log.
        /// </summary>
        public uint Index { get; set; }

        /// <summary>
        /// The message.
        /// </summary>
        public String Message { get; set; }

        /// <summary>
        /// The severity.
        /// </summary>
        public uint Severity { get; set; }

        /// <summary>
        /// The Source Id.
        /// </summary>
        public OmniId SourceId { get; set; }

        /// <summary>
        /// The Source Key.
        /// </summary>
        public uint SourceKey { get; set; }

        /// <summary>
        /// The time stamp of when the event was added to the Event Log.
        /// </summary>
        public PeekTime Timestamp { get; set; }

        public EventLogEntry(OmniEngine engine, XElement node)
        {
            this.Engine = engine;
            this.Connection = engine.Connection;
            this.Logger = engine.Logger;

            this.Index = 0;
            this.ContextId = null;
            this.Message = null;
            this.Severity = 0;
            this.SourceId = null;
            this.SourceKey = 0;
            this.Timestamp = null;

            Load(node);
        }

        public bool Equals(EventLogEntry entry)
        {
            if (entry is null) return false;
            return this.Index == entry.Index;
        }
        public override bool Equals(object obj) => Equals(obj as EventLogEntry);
        public override int GetHashCode() { return (int)Index; }

        private void Load(XElement node)
        {
            if ((node == null) || (node.Name.ToString() != EventLogEntry.RootName)) return;
            IEnumerable<XElement> elements = node.Elements();
            foreach (XElement element in elements)
            {
                String name = element.Name.ToString();
                switch (name)
                {
                    case "contextid":
                        this.ContextId = new OmniId(element.Value);
                        break;

                    case "messageid":
                        this.Index = Convert.ToUInt32(element.Value);
                        break;

                    case "peektimestamp":
                        this.Timestamp = new PeekTime(Convert.ToUInt64(element.Value));
                        break;

                    case "severity":
                        this.Severity = Convert.ToUInt32(element.Value);
                        break;

                    case "shortmessage":
                        this.Message = element.Value;
                        break;

                    case "sourceid":
                        this.SourceId = new OmniId(element.Value);
                        break;

                    case "sourcekey":
                        this.SourceKey = Convert.ToUInt32(element.Value);
                        break;
                }
            }
        }
    }
}
