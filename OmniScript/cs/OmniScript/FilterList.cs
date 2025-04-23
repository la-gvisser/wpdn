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
    /// List of Filter objects.
    /// </summary>
    public class FilterList
        : OmniList<Filter>
    {
        public const String RootName = "filters";

        public OmniLogger logger;
        public PeekTime Modified { get; set; }

        public FilterList(OmniLogger logger, XElement filters)
        {
            this.logger = logger;
            this.Modified = null;
            this.Load(filters);
        }

        public FilterList(OmniLogger logger)
            : this(logger, null)
        {
        }

        /// <summary>
        /// Find an Filter by its name.
        /// </summary>
        /// <param name="name">The name of the Filter to match.</param>
        /// <returns>The Filter object was found or null.</returns>
        public Filter Find(String name)
        {
            return this.Find(
                delegate(Filter filter)
                {
                    return filter.Name == name;
                });
        }

        /// <summary>
        /// Find an Filter by its id.
        /// </summary>
        /// <param name="id">The OmniId of the Filter to match.</param>
        /// <returns>The Filter object was found or null.</returns>
        public Filter Find(OmniId id)
        {
            return this.Find(
                delegate(Filter filter)
                {
                    return id.Id == filter.Id.Id;
                });
        }

        public void Load(XElement node, List<NamedId> plugins)
        {
            if (node == null) return;
            if (node.Name.ToString() != "filters") return;
            this.Modified = null;
            XAttribute modtime = node.Attribute("modtime");
            if (modtime != null)
            {
                this.Modified = new PeekTime(Convert.ToUInt64(modtime.Value));
            }

            foreach (XElement elem in node.Elements("filterobj"))
            {
                Filter filter = new Filter(logger);
                filter.Load(elem, plugins);
                this.Add(filter);
            }
            // Sort the list.
            this.Sort((x, y) =>
                x.Name.CompareTo(y.Name));
        }

        public void Load(XElement node)
        {
            Load(node, null);
        }

        public void Load(String filename, List<NamedId> plugins)
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

        public void Store(XElement node)
        {
            XElement filters = new XElement("filters");
            if (!PeekTime.IsNullOrEmpty(this.Modified))
            {
                filters.Add(new XAttribute("modtime", this.Modified));
            }

            foreach (Filter filter in this)
            {
                XElement filterobj = new XElement("filterobj",
                    new XAttribute("clsid", OmniScript.ClassNameIds["Filter"].ToString()));
                filter.Store(filterobj);
                filters.Add(filterobj);
            }
            node.Add(filters);
        }
    }
}
