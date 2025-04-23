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
    /// List of PluginSettings objects.
    /// </summary>
    public class PluginSettingsList
        : OmniList<PluginSettings>
    {
        public const String PluginRootName = "properties";
        public const String ConfigRootName = "plugins";

        public PluginSettingsList()
        {
        }

        /// <summary>
        /// Add a plugin to the list.
        /// </summary>
        public void Add(AnalysisModule plugin)
        {
            this.Add(new PluginSettings(plugin));
        }

        /// <summary>
        /// Add a list of plugins to the list.
        /// </summary>
        public void Add(AnalysisModuleList plugins)
        {
            foreach (AnalysisModule plugin in plugins)
            {
                this.Add(new PluginSettings(plugin));
            }
        }

        /// <summary>
        /// Find an PluginSettings by its name.
        /// </summary>
        /// <param name="name">The name of the PluginSettings to match.</param>
        /// <returns>The PluginSettings object was found or null.</returns>
        public PluginSettings Find(String name)
        {
            return this.Find(
                delegate(PluginSettings plugin)
                {
                    return plugin.Name == name;
                });
        }

        /// <summary>
        /// Find an PluginSettings by its id.
        /// </summary>
        /// <param name="id">The OmniId of the PluginSettings to match.</param>
        /// <returns>The PluginSettings object was found or null.</returns>
        public PluginSettings Find(OmniId id)
        {
            return this.Find(
                delegate(PluginSettings settings)
                {
                    return id.Id == settings.Id.Id;
                });
        }

        public void Load(XElement node, XElement config)
        {
            if ((node == null) || (node.Name.ToString() != PluginRootName)) return;
            IEnumerable<XElement> list = node.Elements();
            foreach (XElement elem in list)
            {
                OmniId id = Guid.Parse(elem.Value);
                this.Add(new PluginSettings(id));
            }

            if ((config != null) && (config.Name.ToString() == ConfigRootName))
            {
                IEnumerable<XElement> plugins = config.Elements("plugin");
                foreach (XElement plugin in plugins)
                {
                    XAttribute clsidAttribute = plugin.Attribute("clsid");
                    if (clsidAttribute != null)
                    {
                        OmniId id = Guid.Parse(clsidAttribute.Value);
                        PluginSettings settings = this.Find(id);
                        if (!OmniId.IsNullOrEmpty(id) && (settings != null))
                        {
                            settings.Load(plugin);
                        }
                    }
                }
            }

            // Sort the list.
            this.Sort((x, y) =>
                x.Name.CompareTo(y.Name));
        }

        public void Replace(PluginSettings settings)
        {
            int index = this.FindIndex(x => x.Id == settings.Id);
            if (index > 0)
            {
                this[index] = settings;
            }
        }

        public void Set(AnalysisModuleList plugins)
        {
            this.Clear();
            this.Add(plugins);
        }

        public void Store(XElement node, XElement config)
        {
            XElement plugins = new XElement(PluginRootName);
            XElement configs = new XElement(ConfigRootName);

            foreach (PluginSettings settings in this)
            {
                XElement elem = new XElement("prop",
                    new XAttribute("type", "8"),
                    settings.Id);
                plugins.Add(elem);

                XElement configElem = settings.Store();
                if (configElem != null)
                {
                    configs.Add(configElem);
                }
            }

            node.Add(plugins);
            config.Add(configs);
        }
    }
}
