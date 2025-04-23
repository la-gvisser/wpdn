// =============================================================================
// <copyright file="Adapter.cs" company="LiveAction, Inc.">
//  Copyright (c) 2018-2021 Savvius, Inc. All rights reserved.
//  Copyright (c) 2015-2018 Savvius, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System;
    using System.Runtime.InteropServices;
    using System.Collections.Generic;
    using System.IO;
    using System.Net;
    using System.Net.NetworkInformation;
    using System.Xml.Linq;
    using System.Text;

    public class FileInformation
    {
        public static readonly uint FlagsFolder = 0x00000010;

        public const int FILE_FLAGS_READONLY = 0x00000001;
        public const int FILE_FLAGS_HIDDEN = 0x00000002;
        public const int FILE_FLAGS_SYSTEM = 0x00000004;
        public const int FILE_FLAGS_FOLDER = 0x00000010;
        public const int FILE_FLAGS_ARCHIVE = 0x00000020;
        public const int FILE_FLAGS_DEVICE = 0x00000040;
        public const int FILE_FLAGS_NORMAL = 0x00000080;
        public const int FILE_FLAGS_TEMPORARY = 0x00000100;
        public const int FILE_FLAGS_SPARSE_FILE = 0x00000200;
        public const int FILE_FLAGS_REPARSE_POINT = 0x00000400;
        public const int FILE_FLAGS_COMPRESSED = 0x00000800;
        public const int FILE_FLAGS_OFFLINE = 0x00001000;
        public const int FILE_FLAGS_NOT_CONTENT_INDEXED = 0x00002000;
        public const int FILE_FLAGS_ENCRYPTED = 0x00004000;
        public const int FILE_FLAGS_VIRTUAL = 0x00010000;

        public OmniLogger Logger;

        public uint Flags;
        public PeekTime Modified;
        public String Path;
        public String Name;
        public UInt64 Size;

        // XML Tags
        const String tagRootName = "file";
        const String tagFlags = "attr";
        const String tagModified = "mod";
        const String tagName = "name";
        const String tagSize = "size";

        public FileInformation(OmniLogger logger, XElement node=null)
        {
            this.Logger = logger;
            this.Load(node);
        }

        public void Load(XElement node)
        {
            if (node == null) return;
            XElement info = null;
            if (node.Name.ToString() == tagRootName)
            {
                info = node;
            }
            if (info == null) return;

            IEnumerable<XAttribute> attribs = info.Attributes();
            foreach (XAttribute attrib in attribs)
            {
                String name = attrib.Name.ToString();
                switch (name)
                {
                    case tagFlags:
                        this.Flags = Convert.ToUInt32(attrib.Value);
                        break;

                    case tagModified:
                        this.Modified = new PeekTime(attrib.Value);
                        break;

                    case tagName:
                        this.Name = attrib.Value;
                        break;

                    case tagSize:
                        this.Flags = Convert.ToUInt32(attrib.Value);
                        break;
                }
            }
        }

        public XElement Store()
        {
            XElement file = new XElement(tagRootName,
                new XAttribute(tagName, this.Name),
                new XAttribute(tagFlags, this.Flags.ToString()),
                new XAttribute(tagSize, this.Size.ToString()));
            if (this.Modified != null)
            {
                file.Add(new XAttribute(tagModified, this.Modified.ToString()));
            }

            return file;
        }

        public void Store(XElement node)
        {
            node.Add(this.Store());
        }

        public bool IsFolder()
        {
            return ((this.Flags & FlagsFolder) != 0);
        }
    }
}
