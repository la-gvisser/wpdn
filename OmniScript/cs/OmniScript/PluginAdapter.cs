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

    public class PluginAdapter
        : Adapter
    {
        new public static readonly AdapterTypes Type = AdapterTypes.Plugin;

        new public const String RootName = "pluginadapterinfo";

        /// <summary>
        /// Gets or sets the plug-in's name.
        /// </summary>
        public String Plugin { get; set; }

        public PluginAdapter(OmniEngine engine, XElement adapterinfo)
            : base(PluginAdapter.Type, engine)
        {
            this.Load(adapterinfo);
        }

        public void Load(XElement adapterinfo)
        {
            if ((adapterinfo == null) || (adapterinfo.Name != RootName)) return;

            IEnumerable<XElement> elements = adapterinfo.Elements();
            foreach (XElement element in elements)
            {
                switch (element.Name.ToString())
                {
                    // Adapter Properties
                    case "address":
                        this.Address = element.Value;
                        break;

                    case "description":
                        this.Description = element.Value;
                        break;

                    case "identifier":
                        this.Id = element.Value;
                        break;

                    case "linkspeed":
                        this.LinkSpeed = Convert.ToUInt64(element.Value);
                        break;

                    case "mediatype":
                        this.MediaType = Convert.ToUInt32(element.Value);
                        break;

                    case "mediasubtype":
                        this.MediaSubType = Convert.ToUInt32(element.Value);
                        break;

                    case "title":
                        this.Name = element.Value;
                        break;

                    // Plugin Adapter Properties
                    case "plugin":
                        this.Plugin = element.Value;
                        break;
                }
            }
        }
    }
}
