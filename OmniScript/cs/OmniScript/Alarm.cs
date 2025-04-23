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
    using System.Xml.Linq;

    public class Alarm
    {
        public const String RootName = "alarmnode";

        private OmniEngine Engine { get; set; }
        public OmniLogger Logger { get; set; }

        /// <summary>
        /// Gets or sets the time and date the alarm was created.
        /// </summary>
        public PeekTime Created { get; set; }

        /// <summary>
        /// Gets or sets the Id of the alarm.
        /// </summary>
        public OmniId Id { get; set; }

        /// <summary>
        /// Gets or sets the time and date the alarm was last modified.
        /// </summary>
        public PeekTime Modified{ get; set; }

        /// <summary>
        /// Gets or sets the name of the alarm.
        /// </summary>
        public String Name { get; set; }

        /// <summary>
        /// Gets or sets the Track Type of the alarm.
        /// </summary>
        public uint TrackType { get; set; }

        public Alarm(OmniEngine engine, XElement node=null)
        {
            this.Engine = engine;
            this.Logger = engine.Logger;
            this.Load(node);
        }

        public void Load(XElement node)
        {
            if (node == null) return;
            XElement alarm = node.Element(RootName);
            if (alarm == null) return;

            IEnumerable<XAttribute> attrib = alarm.Attributes();
            foreach (XAttribute a in attrib)
            {
                switch (a.Name.ToString())
                {
                    case "id":
                        this.Id = new OmniId(a.Value);
                        break;

                    case "created":
                        this.Created = new PeekTime(a.Value);
                        break;

                    case "modified":
                        this.Modified = new PeekTime(a.Value);
                        break;

                    case "name":
                        this.Name = a.Value;
                        break;

                    case "tracktype":
                        this.TrackType = Convert.ToUInt32(a.Value);
                        break;
                }
            }
        }
    }
}
