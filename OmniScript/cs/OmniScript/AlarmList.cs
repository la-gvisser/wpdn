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

    /// <summary>
    /// List of Alarms.
    /// </summary>
    /// <devnotes>Response of GetAlarms</devnotes>
    public class AlarmList
        : OmniList<Alarm>
    {
        public const String RootName = "Alarms";

        public OmniEngine Engine { get; set; }
        public OmniLogger Logger { get; set; }

        public AlarmList(OmniEngine engine, XElement node)
        {
            this.Engine = engine;
            this.Logger = this.Engine.Logger;
            this.Load(node);
        }

        /// <summary>
        /// Find an Alarm by its Name.
        /// </summary>
        /// <param name="value">The name of the Alarm to match.</param>
        /// <returns>The Alarm object was found or null.</returns>
        public Alarm Find(String name)
        {
            return this.Find(
                delegate(Alarm alarm)
                {
                    return alarm.Name == name;
                });
        }

        /// <summary>
        /// Find an Alarm by its Id.
        /// </summary>
        /// <param name="value">The Id of the Alarm to match.</param>
        /// <returns>The Alarm object was found or null.</returns>
        public Alarm Find(OmniId value)
        {
            return this.Find(
                delegate(Alarm alarm)
                {
                    return alarm.Id == value;
                });
        }

        public void Load(XElement node)
        {
            if (node == null) return;
            IEnumerable<XElement> element = node.Elements("Alarmobj");
            foreach (XElement obj in element)
            {
                Alarm alarm = new Alarm(this.Engine, obj);
                if (alarm != null)
                {
                    this.Add(alarm);
                }
            }
        }
    }
}
