// =============================================================================
// <copyright file="Adapter.cs" company="LiveAction, Inc.">
//  Copyright (c) 2021 LiveAction, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System;
    using System.Collections.Generic;
    using System.Xml.Linq;

    public class DPDKAdapter
        : Adapter
    {
        new public static readonly AdapterTypes Type = AdapterTypes.NIC;

        new public const String RootName = "DPDKAdapterInfo";

        /// <summary>
        /// The adapter Flags.
        /// </summary>
        public uint Flags { get; set; }

        /// <summary>
        /// Is the adapter valid.
        /// </summary>
        public bool Valid { get; set; }

        public DPDKAdapter(OmniEngine engine, XElement adapterinfo)
            : base(DPDKAdapter.Type, engine)
        {
            this.Load(adapterinfo);
        }

        public void Load(XElement adapterinfo)
        {
            if ((adapterinfo == null) || (adapterinfo.Name != RootName)) return;

            IEnumerable<XElement> elements = adapterinfo.Elements();
            foreach (XElement element in elements)
            {
                switch (element.Name.ToString())
                {
                    // Adapter Properties
                    case "Address":
                        this.Address = element.Value;
                        break;

                    case "ID":
                        this.Id = element.Value;
                        break;

                    case "LinkSpeed":
                        this.LinkSpeed = Convert.ToUInt64(element.Value);
                        break;

                    case "MediaType":
                        this.MediaType = Convert.ToUInt32(element.Value);
                        break;

                    case "MediaSubType":
                        this.MediaSubType = Convert.ToUInt32(element.Value);
                        break;

                    case "Description":
                        this.Name = element.Value;
                        break;

                    // DPDK Adapter Properties
                    case "Flags":
                        this.Flags = Convert.ToUInt32(element.Value);
                        break;                }
            }

            this.Valid = true;
        }
    }
}
