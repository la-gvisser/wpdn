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

    public class NDISAdapter
        : Adapter
    {
        new public static readonly AdapterTypes Type = AdapterTypes.NIC;

        new public const String RootName = "ndisadapterinfo";

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

        public NDISAdapter(OmniEngine engine, XElement adapterinfo)
            : base(NDISAdapter.Type, engine)
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
                    case "address":
                        this.Address = element.Value;
                        break;

                    case "name":
                        this.Description = element.Value;
                        break;

                    case "enumerator":
                        this.Id = element.Value;
                        break;

                    case "linkspeed":
                        this.LinkSpeed = Convert.ToUInt64(element.Value);
                        break;

                    case "ndismediatype":
                        this.MediaType = Convert.ToUInt32(element.Value);
                        break;

                    case "ndisphysmedium":
                        this.MediaSubType = Convert.ToUInt32(element.Value);
                        break;

                    case "description":
                        this.Name = element.Value;
                        break;

                    // NDIS Adapter Properties
                    case "adapterfeatures":
                        this.Features = Convert.ToUInt32(element.Value);
                        break;

                    case "interfacefeatures":
                        this.InterfaceFeatures = Convert.ToUInt32(element.Value);
                        break;

                    case "hidden":
                        this.Hidden = OmniScript.PropBoolean(element.Value);
                        break;

                    case "title":
                        this.DeviceName = element.Value;
                        break;

                    case "valid":
                        this.Valid = OmniScript.PropBoolean(element.Value);
                        break;

                    case "validadvanced":
                        this.ValidAdvanced = OmniScript.PropBoolean(element.Value);
                        break;
                }
            }
        }
    }
}
