// =============================================================================
// <copyright file="ConsoleFilterList.cs" company="Savvius, Inc.">
//  Copyright (c) 2015 Savvius, Inc. All rights reserved.
//  Copyright (c) 2013 WildPackets, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Xml.Linq;
    using System.Text;
    using System.Xml;

    /// <summary>
    /// List of ConsoleFilter objects.
    /// </summary>
    public class ConsoleFilterList
        : OmniList<ConsoleFilter>
    {
        public ConsoleFilterList()
        {
        }

        public ConsoleFilterList(String filename, List<NamedId> plugins)
        {
            XDocument doc = XDocument.Load(filename);
            XElement root = doc.Root;
            if (root != null)
            {
                if (root.Name.ToString() == "filters")
                {
                    this.Load(root, plugins);
                }
                else
                {
                    XElement filters = root.Element("filters");
                    if (filters != null)
                    {
                        this.Load(filters, plugins);
                    }
                }
            }
        }

        public ConsoleFilterList(String filename)
            : this(filename, null)
        {
        }


        /// <summary>
        /// Find an ConsoleFilter by its name.
        /// </summary>
        /// <param name="name">The name of the ConsoleFilter to match.</param>
        /// <returns>The ConsoleFilter object was found or null.</returns>
        public ConsoleFilter Find(String name)
        {
            return this.Find(
                delegate(ConsoleFilter filter)
                {
                    return filter.Name == name;
                });
        }

        /// <summary>
        /// Find an ConsoleFilter by its name.
        /// </summary>
        /// <param name="target">The ConsoleFilter to match.</param>
        /// <returns>The ConsoleFilter object was found or null.</returns>
        public ConsoleFilter Find(ConsoleFilter target)
        {
            return this.Find(
                delegate(ConsoleFilter filter)
                {
                    return (filter.Name == target.Name) && (filter.Comment == target.Comment);
                });
        }

        public void Load(XElement node, List<NamedId> plugins)
        {
            if (node.Name.ToString() != "filters") return;
            IEnumerable<XElement> list = node.Elements("filterobj");
            foreach (XElement elem in list)
            {
                ConsoleFilter filter = new ConsoleFilter();
                filter.Load(elem, plugins);
                this.Add(filter);
            }
        }

        public void Store(XElement node)
        {
            XElement filters = new XElement("filters");
            foreach (ConsoleFilter filter in this)
            {
                filter.Store(filters);
            }
            node.Add(filters);
        }
    }
}
