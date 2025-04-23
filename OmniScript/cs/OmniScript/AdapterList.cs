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
    /// List of Adapters.
    /// </summary>
    /// <devnotes>Response of GetAdapters</devnotes>
    public class AdapterList
        : OmniList<Adapter>
    {
        public static Adapter AdapterFactory(XElement element, OmniEngine engine=null)
        {
            switch (element.Name.ToString())
            {
                case NDISAdapter.RootName:
                    return new NDISAdapter(engine, element);

                case PcapAdapter.RootName:
                    return new PcapAdapter(engine, element);

                case DPDKAdapter.RootName:
                    return new DPDKAdapter(engine, element);

                case PluginAdapter.RootName:
                    return new PluginAdapter(engine, element);

                case FileAdapter.RootName:
                    return new FileAdapter(engine, element);

                case FileAdapter.AltRootName:
                    return new FileAdapter(engine, element);
            }
            return null;
        }
        
        public const String RootName = "adapterinfolist";

        public enum FindAttribute { Name, Id };

        public OmniEngine Engine { get; set; }

        public AdapterList()
        {
        }

        public AdapterList(OmniEngine engine, XElement node)
        {
            this.Engine = engine;
            this.Load(node);
        }

        /// <summary>
        /// Find an Adapter by its Id.
        /// </summary>
        /// <param name="id">The id of the Adapter to match.</param>
        /// <returns>The Adapter object was found or null.</returns>
        public Adapter FindById(String id)
        {
            return this.Find(
                delegate(Adapter adpt)
                {
                    return adpt.Id == id;
                });
        }

        /// <summary>
        /// Find an Adapter by its name.
        /// </summary>
        /// <param name="value">The name of the Adapter to match.</param>
        /// <returns>The Adapter object was found or null.</returns>
        public Adapter FindByName(String name)
        {
            return this.Find(
                delegate(Adapter adpt)
                {
                    return adpt.Name == name;
                });
        }

        public void Load(XElement node)
        {
            if (node == null) return;

            IEnumerable<XElement> adapterinfos = node.Elements(Adapter.RootName);
            foreach (XElement adapterinfo in adapterinfos)
            {
                if (adapterinfo == null) continue;

                IEnumerable<XElement> elements = adapterinfo.Elements();
                // There is only one element, but First and FirstOrDefault don't compile.
                foreach (XElement element in elements)
                {
                    Adapter adapter = AdapterFactory(element, this.Engine);
                    if (adapter != null)
                    {
                        this.Add(adapter);
                        break;
                    }
                }
            }
            // Sort the list.
            this.Sort((x, y) =>
                x.Name.CompareTo(y.Name));
        }
    }
}
