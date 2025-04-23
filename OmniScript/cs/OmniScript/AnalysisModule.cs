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

    public class AnalysisModule
    {
        public OmniLogger logger;
        public OmniId Id { get; set; }
        public String Name { get; set; }
        public List<OmniId> CategoryList { get; set; }
        public String File { get; set; }
        public String Version { get; set; }
        public String Publisher { get; set; }
        public String Features { get; set; }
        public bool OptionProcessPackets { get; set; }
        public bool OptionPacketSummary { get; set; }
        public bool OptionSummaryStats { get; set; }
        public bool OptionFilterPackets { get; set; }
        public bool Options1 { get; set; }
        public bool Options2 { get; set; }
        public OmniId HandlerId { get; set; }

        public AnalysisModule(OmniLogger logger, XElement node=null)
        {
            this.logger = logger;
            this.CategoryList = new List<OmniId>();
            this.Load(node);
        }

        public void Load(XElement node)
        {
            if ((node == null) || (node.Name.ToString() != "Plugin")) return;
            IEnumerable<XElement> elements = node.Elements();
            foreach (XElement element in elements)
            {
                String name = element.Name.ToString();
                switch (name)
                {
                    case "CLSID":
                        this.Id = Guid.Parse(element.Value);
                        break;

                    case "Categories":
                        LoadCategories(element);
                        break;

                    case "File":
                        this.File = element.Value;
                        break;

                    case "Name":
                        this.Name = element.Value;
                        break;

                    case "Version":
                        this.Version = element.Value;
                        break;

                    case "Publisher":
                        this.Publisher = element.Value;
                        break;

                    case "Features":
                        this.LoadFeatures(element);
                        break;
                }
            }
        }

        public void LoadCategories(XElement node)
        {
            IEnumerable<XElement> elements = node.Elements();
            foreach (XElement element in elements)
            {
                this.CategoryList.Add(Guid.Parse(element.Value));
            }
        }

        public void LoadFeatures(XElement node)
        {
            IEnumerable<XElement> elements = node.Elements();
            foreach (XElement element in elements)
            {
                String name = element.Name.ToString();
                switch (name)
                {
                    case "ProcessPackets":
                        this.OptionProcessPackets = OmniScript.PropBoolean(element.Value);
                        break;

                    case "PacketSummary":
                        this.OptionPacketSummary = OmniScript.PropBoolean(element.Value);
                        break;

                    case "SummaryStats":
                        this.OptionSummaryStats = OmniScript.PropBoolean(element.Value);
                        break;

                    case "Filter":
                        this.OptionFilterPackets = OmniScript.PropBoolean(element.Value);
                        break;

                    case "Options":
                        this.Options1 = OmniScript.PropBoolean(element.Value);
                        break;

                    case "Options2":
                        this.Options2 = OmniScript.PropBoolean(element.Value);
                        break;

                    case "HandlerID":
                        this.HandlerId = Guid.Parse(element.Value);
                        break;
                }
            }
        }

        public void Store(XElement node)
        {
            XElement categories = new XElement("Categories");
            foreach (OmniId id in this.CategoryList)
            {
                XElement idNode = new XElement("Category", id);
                categories.Add(idNode);
            }

            XElement features = new XElement("Features",
                new XElement("ProcessPackets", OmniScript.PropBoolean(this.OptionProcessPackets)),
                new XElement("PacketSummary", OmniScript.PropBoolean(this.OptionPacketSummary)),
                new XElement("SummaryStats", OmniScript.PropBoolean(this.OptionSummaryStats)),
                new XElement("Filter", OmniScript.PropBoolean(this.OptionFilterPackets)),
                new XElement("Options", OmniScript.PropBoolean(this.Options1)),
                new XElement("Options2", OmniScript.PropBoolean(this.Options2)),
                new XElement("HandlerID", this.HandlerId));

            XElement plugin = new XElement("Plugin",
                new XElement("CLSID", this.Id),
                categories,
                new XElement("File", "filename"),
                new XElement("Name", this.Name),
                new XElement("Version", "version"),
                new XElement("Publisher", "publisher"),
                features);
            node.Add(plugin);
        }
    }
}
