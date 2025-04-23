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

    public enum AdapterTypes
    {
        Unknown,
        NIC,
        File,
        Plugin
    }
    public class Adapter
    {
        public static readonly AdapterTypes Type = AdapterTypes.Unknown;

        public const String RootName = "adapterinfoobj";

        private OmniEngine engine;
        public OmniLogger logger;

        /// <summary>
        /// Gets or sets the adapter type.
        /// </summary>
        public AdapterTypes AdapterType { get; set; }

        /// <summary>
        /// Gets or sets the network address.
        /// </summary>
        public String Address { get; set; }

        /// <summary>
        /// Gets or sets the description/name.
        /// </summary>
        public String Description { get; set; }

        /// <summary>
        /// Gets or sets the identifier.
        /// </summary>
        public String Id { get; set; }

        /// <summary>
        /// Gets or sets the Link Speed.
        /// </summary>
        public ulong LinkSpeed { get; set; }

        /// <summary>
        /// Gets or sets the Media Type.
        /// </summary>
        public uint MediaType { get; set; }

        /// <summary>
        /// Gets or sets the Media Sub-Type.
        /// </summary>
        public uint MediaSubType { get; set; }

         /// <summary>
        /// Gets or sets the name.
        /// </summary>
        public String Name { get; set; }

        public Adapter(AdapterTypes type=AdapterTypes.Unknown, OmniEngine engine=null)
        {
            this.engine = engine;
            this.logger = engine.Logger;
            this.AdapterType = type;
        }

        public override String ToString()
        {
            return this.Name;
        }
    }
}
