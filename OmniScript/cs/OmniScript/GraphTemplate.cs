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

    public class GraphTemplate
    {
        public const String ClassName = "graph-template";
        public const String RootName = "simplegraph";

        public OmniLogger Logger { get; set; }

        /// <summary>
        /// Gets or sets the description of the graph.
        /// </summary>
        public String Description { get; set; }

        /// <summary>
        /// Gets or sets the Id of the graph.
        /// </summary>
        public OmniId GraphId { get; set; }

        /// <summary>
        /// Gets or sets the Id of the graph.
        /// </summary>
        public OmniId Id { get; set; }

        /// <summary>
        /// Gets or sets the name of the graph.
        /// </summary>
        public String Name { get; set; }

        /// <summary>
        /// Gets or sets the start of the graph.
        /// </summary>
        public uint Start { get; set; }

        /// <summary>
        /// Gets or sets the sample interval of the graph.
        /// </summary>
        public uint SampleInterval { get; set; }

        /// <summary>
        /// Gets or sets the sample count of the graph.
        /// </summary>
        public uint SampleCount { get; set; }

        public GraphTemplate(OmniLogger logger, XElement node=null)
        {
            this.Logger = logger;
            this.Load(node);
        }

        public void Load(XElement node)
        {
            if ((node == null) || (node.Name != ClassName)) return;
            XElement graph = node.Element(RootName);
            if (graph == null) return;
            XElement details = graph.Element("details");
            if (details == null) return;
            IEnumerable<XAttribute> attribs = details.Attributes();
            foreach (XAttribute attrib in attribs)
            {
                String name = attrib.Name.ToString();
                switch (name)
                {
                    case "desc":
                        this.Description = attrib.Value;
                        break;

                    case "id":
                        this.GraphId = new OmniId(attrib.Value);
                        break;

                    case "sample-count":
                        this.SampleCount = Convert.ToUInt32(attrib.Value);
                        break;

                    case "sample-int":
                        this.SampleInterval = Convert.ToUInt32(attrib.Value);
                        break;

                    case "templateid":
                        this.Id = new OmniId(attrib.Value);
                        break;

                    case "title":
                        this.Name = attrib.Value;
                        break;

                    case "start":
                        this.Start = Convert.ToUInt32(attrib.Value);
                        break;
                }
            }
        }
    }
}
