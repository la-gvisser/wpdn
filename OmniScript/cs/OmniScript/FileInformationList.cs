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
    using System.IO;
    using System.Xml.Linq;
    using System.Text;
    using System.Xml;

    public class FileInformationList
        : OmniList<FileInformation>
    {
        public const String RootName = "files";

        public OmniLogger Logger;

        public FileInformationList(OmniLogger logger, XElement files)
        {
            this.Logger = logger;
            this.Load(files);
        }

        /// <summary>
        /// Find an FileInformation by its file name.
        /// </summary>
        /// <param name="value">The name of the FileInformation to match.</param>
        /// <returns>The FileInformation object found or null.</returns>
        public FileInformation FindByName(String name)
        {
            return this.Find(
                delegate(FileInformation info)
                {
                    return info.Name == name;
                });
        }

        public void Load(XElement node)
        {
            if (node == null) return;
            if (node.Name.ToString() != "files") return;
            IEnumerable<XElement> list = node.Elements("file");
            foreach (XElement elem in list)
            {
                FileInformation info = new FileInformation(this.Logger, elem);
                this.Add(info);
            }
            // Sort the list.
            this.Sort((x, y) =>
                x.Name.CompareTo(y.Name));
        }

        public void Store(XElement node)
        {
            XElement files = new XElement("files");
            foreach (FileInformation info in this)
            {
                info.Store(files);
            }
            node.Add(files);
        }
    }
}
