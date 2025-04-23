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
    using System.Linq;
    using System.Xml.Linq;

    public class PluginSettings
    {
        public static bool IsNullOrEmpty(PluginSettings settings)
        {
            if (settings == null) return true;
            return String.IsNullOrEmpty(settings.Settings);
        }

        public OmniId Id { get; set; }
        public String Name { get; set; }
        public String Settings { get; set; }

        public PluginSettings(OmniId id)
        {
            this.Id = id;
            this.Name = "";
            this.Settings = null;
        }

        public PluginSettings(PluginSettings settings)
            : this(OmniId.Empty.Id)
        {
            if (settings != null)
            {
                this.Id = settings.Id;
                this.Name = settings.Name;
                this.Settings = settings.Settings;
            }
        }
        public PluginSettings(AnalysisModule module)
            : this(OmniId.Empty.Id)
        {
            if (module != null)
            {
                this.Id = module.Id;
                this.Name = module.Name;
            }
        }

        public virtual void Load(XElement obj)
        {
            if (obj == null) return;

            XAttribute clsidAttribute = obj.Attribute("clsid");
            if (clsidAttribute == null) return;

            OmniId id = Guid.Parse(clsidAttribute.Value);
            if (this.Id.Equals(id))
            {
                XAttribute name = obj.Attribute("name");
                if (name != null)
                {
                    this.Name = name.Value.ToString();
                    this.Settings = String.Join("", obj.Elements().Select(e => e.ToString(SaveOptions.DisableFormatting)));
                }
            }
        }

        public virtual XElement Store()
        {
            if (!String.IsNullOrEmpty(this.Settings))
            {
                XElement plugin = new XElement("plugin",
                    new XAttribute("clsid", this.Id),
                    new XAttribute("name",
                        (String.IsNullOrEmpty(this.Name)) ? "" : this.Name));
                if (!String.IsNullOrEmpty(this.Settings))
                {
                    String xml = "<Options>" + this.Settings + "</Options>";
                    XElement options = XElement.Parse(xml);
                    IEnumerable<XElement> elements = options.Elements();
                    if (elements.Count() > 0)
                    {
                        plugin.Add(elements);
                    }
                }
                return plugin;
            }
            return null;
        }

        public virtual String ToString(String format)
        {
            if (!String.IsNullOrEmpty(this.Settings))
            {
                return this.Settings;
            }
            return "";
        }
    }
}
