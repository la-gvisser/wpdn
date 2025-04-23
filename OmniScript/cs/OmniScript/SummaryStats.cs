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

    public class SummaryStats
        : Dictionary<String, Dictionary<String, String>>
    {
        public Dictionary<String, String> GetKey(String key)
        {
            if (!this.ContainsKey(key))
            {
                this.Add(key, new Dictionary<String, String>());
            }
            return this[key];
        }
    }
}
