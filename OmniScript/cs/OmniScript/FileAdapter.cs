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
    using System.Linq;
    using System.Xml.Linq;
    using System.Text;

    public class FileAdapter
        : Adapter
    {
        new public static readonly AdapterTypes Type = AdapterTypes.File;

        new public const String RootName = "fileadapterinfo";
        public const String AltRootName = "replayadapterinfo";

        /// <summary>
        /// Gets or sets the replay limit.
        /// </summary>
        public uint Limit { get; set; }

        /// <summary>
        /// Gets or sets the replay mode.
        /// </summary>
        public uint Mode { get; set; }

        /// <summary>
        /// Gets or sets the replay speed.
        /// </summary>
        public double Speed { get; set; }

        public FileAdapter(OmniEngine engine, XElement adapterinfo)
            : base(FileAdapter.Type, engine)
        {
            this.AdapterType = AdapterTypes.File;

            this.Load(adapterinfo);
        }

        public FileAdapter(String filename, OmniEngine engine=null)
            : this(engine, null)
        {
            this.Name = filename;
        }

        public void Load(XElement adapterinfo)
        {
            if ((adapterinfo == null) || !((adapterinfo.Name == RootName) || (adapterinfo.Name == AltRootName))) return;

            IEnumerable<XElement> elements = adapterinfo.Elements();
            foreach (XElement element in elements)
            {
                switch (element.Name.ToString())
                {
                    // Adapter Properties
                    case "description":
                        this.Description = element.Value;
                        break;

                    case "filename":
                        this.Name = element.Value;
                        break;

                    case "filepath":
                        this.Name = element.Value;
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

                    // File Adapter Properties
                    case "limit":
                        this.Limit = Convert.ToUInt32(element.Value);
                        break;

                    case "mode":
                        this.Mode = Convert.ToUInt32(element.Value);
                        break;

                    case "speed":
                        this.Speed = Convert.ToDouble(element.Value);
                        break;
                }
            }
        }
    }
}
