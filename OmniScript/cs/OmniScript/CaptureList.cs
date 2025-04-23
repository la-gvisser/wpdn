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
    using System.Collections.Generic;
    using System.Xml.Linq;

    /// <summary>
    /// List of Capture objects.
    /// </summary>
    public class CaptureList
        : OmniList<Capture>
    {
        public const String RootName = "captures";

        public OmniLogger Logger;

        public CaptureList(OmniLogger logger)
        {
            this.Logger = logger;
        }

        public CaptureList(OmniEngine engine, XElement captures)
        {
            this.Logger = engine.Logger;

            IEnumerable<XElement> list = captures.Elements("capture");
            foreach (XElement elem in list)
            {
                this.Add(new Capture(engine, elem));
            }
            // Sort the list.
            this.Sort((x, y) =>
                x.Name.CompareTo(y.Name));
        }

        /// <summary>
        /// Find an Capture by its id.
        /// </summary>
        /// <param name="id">The id of the Capture to match.</param>
        /// <returns>The Capture object was found or null.</returns>
        public Capture Find(Guid id)
        {
            return this.Find(
                delegate(Capture capt)
                {
                    return capt.Id == id;
                });
        }

        /// <summary>
        /// Find a Capture by its name.
        /// </summary>
        /// <param name="name">The name of the Capture to match.</param>
        /// <returns>The Capture object was found or null.</returns>
        public Capture Find(String name)
        {
            return this.Find(
                delegate(Capture capt)
                {
                    return capt.Name == name;
                });
        }
    }
}
