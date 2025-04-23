// =============================================================================
// <copyright file="ConsoleTemplateList.cs" company="Savvius, Inc.">
//  Copyright (c) 2015 Savvius, Inc. All rights reserved.
//  Copyright (c) 2013 WildPackets, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Xml.Linq;

    /// <summary>
    /// A list of Console Templates.
    /// </summary>
    public class ConsoleTemplateList
        : OmniList<ConsoleTemplate>
    {
        /// <summary>
        /// Load a list of Console Templates.
        /// </summary>
        /// <param name="filename">The name of the file to parse.</param>
        public ConsoleTemplateList(String filename)
        {
            Load(filename);
        }

        public NamedIdList GetNamedIdList()
        {
            NamedIdList list = new NamedIdList();
            foreach (ConsoleTemplate item in this)
            {
                list.Add(new NamedId(item.CaptureName, Guid.Empty));
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
                ConsoleTemplate template = new ConsoleTemplate(capture);
                this.Add(template);
            }
        }

        public void Store(String filename)
        {
            XElement root = new XElement("CaptureSettings", new XAttribute("Version", "1.0"));
            foreach (ConsoleTemplate template in this)
            {
                root.Add(template.Store());
            }
            XDocument doc = new XDocument(root);
            doc.Save(filename);
        }
    }
}
