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
    /// List of ForensicSearch objects.
    /// </summary>
    public class ForensicSearchList
        : OmniList<ForensicSearch>
    {
        public const String RootName = "FileViews";

        public OmniLogger Logger;

        public ForensicSearchList(OmniLogger logger, XElement node)
        {
            this.Logger = logger;
            this.Load(node);
        }

        public void Load(XElement node)
        {
            if ((node == null) || (node.Name.ToString() != RootName)) return;

            IEnumerable<XElement> views = node.Elements(ForensicSearch.RootName);
            foreach (XElement view in views)
            {
                this.Add(new ForensicSearch(this.Logger, view));
            }
            // Sort the list.
            this.Sort((x, y) =>
                x.Name.CompareTo(y.Name));
        }
    }
}
