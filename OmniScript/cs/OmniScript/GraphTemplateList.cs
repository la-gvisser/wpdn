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
    /// List of GraphTemplate objects.
    /// </summary>
    public class GraphTemplateList
        : OmniList<GraphTemplate>
    {
        public const String RootName = "graph-collection";

        public OmniLogger Logger;

        public GraphTemplateList(OmniLogger logger, XElement node)
        {
            this.Logger = logger;
            this.Load(node);
        }

        public void Load(XElement node)
        {
            if ((node == null) || (node.Name.ToString() != RootName)) return;
            IEnumerable<XElement> element = node.Elements(GraphTemplate.ClassName);
            foreach (XElement obj in element)
            {
                GraphTemplate graph = new GraphTemplate(this.Logger, obj);
                if (graph != null)
                {
                    this.Add(graph);
                }
            }
        }
    }
}
