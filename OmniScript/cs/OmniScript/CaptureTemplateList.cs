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
    /// A list of Capture Templates.
    /// </summary>
    public class CaptureTemplateList
        : OmniList<CaptureTemplate>
    {
        /// <summary>
        /// Load a list of Capture Templates.
        /// </summary>
        /// <param name="filename">The name of the file to parse.</param>
        public CaptureTemplateList(String filename = null)
        {
            if (!String.IsNullOrEmpty(filename))
            {
                Load(filename);
            }
        }

        public NamedIdList GetNamedIdList()
        {
            NamedIdList list = new NamedIdList();
            foreach (CaptureTemplate item in this)
            {
                list.Add(new NamedId(item.Name, item.Id));
            }
            return list;
        }

        public void Load(String filename)
        {
            XDocument doc = XDocument.Load(filename);
            XElement root = doc.Root;
            IEnumerable<XElement> captures = root.Elements("CaptureWindow");
            foreach (XElement capture in captures)
            {
                CaptureTemplate template = new CaptureTemplate(capture);
                this.Add(template);
            }
            // Sort the list.
            this.SortByName();
        }

        public void SortByName()
        {
            this.Sort((x, y) =>
                x.General.Name.CompareTo(y.General.Name));
        }

        public void Store(String filename, OmniEngine engine)
        {
            XElement root = new XElement("CaptureSettings", new XAttribute("Version", "1.0"));
            foreach (CaptureTemplate template in this)
            {
                XElement capture = new XElement("CaptureWindow");
                template.Store(capture, engine);
                root.Add(capture);
            }
            XDocument doc = new XDocument(root);
            doc.Save(filename);
        }
    }
}
