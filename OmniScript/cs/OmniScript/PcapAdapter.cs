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

    public class PcapAdapter
        : Adapter
    {
        new public static readonly AdapterTypes Type = AdapterTypes.NIC;

        new public const String RootName = "PcapAdapterInfo";

        /// <summary>
        /// Gets or sets the device name.
        /// </summary>
        public String DeviceName { get; set; }

        /// <summary>
        /// Gets or sets the Adapter Features.
        /// </summary>
        public uint Features { get; set; }

        /// <summary>
        /// Is the adapter hidden.
        /// </summary>
        public bool Hidden { get; set; }

        /// <summary>
        /// Gets or sets the Interface Features.
        /// </summary>
        public uint InterfaceFeatures { get; set; }

        /// <summary>
        /// Does the adapter support the OmniPeek Wireless API.
        /// </summary>
        public bool OmniPeekAPI { get; set; }

        /// <summary>
        /// Is the adapter valid.
        /// </summary>
        public bool Valid { get; set; }

        /// <summary>
        /// Is the adapter valid for advanced.
        /// </summary>
        public bool ValidAdvanced { get; set; }

        public PcapAdapter(OmniEngine engine, XElement adapterinfo)
            : base(PcapAdapter.Type, engine)
        {
            this.AdapterType = AdapterTypes.NIC;
            this.Hidden = false;
            this.InterfaceFeatures = 0;
            this.OmniPeekAPI = false;
            this.Valid = true;
            this.ValidAdvanced = true;

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

                    case "Description":
                        this.Description = element.Value;
                        this.Name = this.Description;
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

                    // PCAP Adapter Properties
                    
                    case "Flags":
                        this.Features = Convert.ToUInt32(element.Value);
                        break;
                }
            }
        }
    }
}
