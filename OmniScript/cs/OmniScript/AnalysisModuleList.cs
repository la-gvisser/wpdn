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

    public class AnalysisModuleList
        : OmniList<AnalysisModule>
    {
        public const String RootName = "Plugins";

        public OmniLogger logger;

        public AnalysisModuleList(OmniLogger logger, XElement plugins)
        {
            this.logger = logger;
            this.Load(plugins);
        }

        public AnalysisModuleList(OmniLogger logger)
            : this(logger, null)
        {
        }

        /// <summary>
        /// Find an OmniPlugin by its name.
        /// </summary>
        /// <param name="name">The name of the OmniPlugin to match.</param>
        /// <param name="caseSensitive">Perform a case sensitive match.</param>
        /// <returns>The OmniPlugin object was found or null.</returns>
        public AnalysisModule Find(String name, bool caseSensitive)
        {
            if (caseSensitive)
            {
                return this.Find(
                    delegate(AnalysisModule plugin)
                    {
                        return plugin.Name == name;
                    });
            }
            return this.Find(
                delegate(AnalysisModule plugin)
                {
                    return plugin.Name.ToLower() == name.ToLower();
                });
        }

        /// <summary>
        /// Find an OmniPlugin by its name. (Case sensitive)
        /// </summary>
        /// <param name="name">The name of the OmniPlugin to match.</param>
        /// <returns>The OmniPlugin object was found or null.</returns>
        public AnalysisModule Find(String name)
        {
            return this.Find(name, true);
        }

        /// <summary>
        /// Find an OmniPlugin by its id.
        /// </summary>
        /// <param name="id">The id of the OmniPlugin to match.</param>
        /// <returns>The OmniPlugin object was found or null.</returns>
        public AnalysisModule Find(Guid id)
        {
            return this.Find(
                delegate(AnalysisModule plugin)
                {
                    return plugin.Id == id;
                });
        }

        public NamedIdList GetNamedIdList()
        {
            NamedIdList list = new NamedIdList();
            foreach (AnalysisModule item in this)
            {
                list.Add(new NamedId(item.Name, item.Id));
            }
            return list;
        }

        public void Load(XElement node)
        {
            if ((node == null) || (node.Name.ToString() != "Plugins")) return;
            IEnumerable<XElement> list = node.Elements("Plugin");
            foreach (XElement elem in list)
            {
                this.Add(new AnalysisModule(logger, elem));
            }
            // Sort the list.
            this.Sort((x, y) =>
                x.Name.CompareTo(y.Name));
        }

        public void Store(XElement node)
        {
            XElement plugins = new XElement("plugins");
            foreach (AnalysisModule plugin in this)
            {
                plugin.Store(plugins);
            }
            node.Add(plugins);
        }
    }
}
