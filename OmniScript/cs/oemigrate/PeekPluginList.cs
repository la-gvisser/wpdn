// =============================================================================
// <copyright file="PeekPluginList.cs" company="WildPackets, Inc.">
//  Copyright (c) 2013 WildPackets, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace OEMigrate
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using Savvius.Omni.OmniScript;

    public class PeekPluginList
        : List<PeekPlugin>
    {
        public NamedIdList GetNamedIdList()
        {
            NamedIdList list = new NamedIdList();
            foreach (PeekPlugin item in this)
            {
                list.Add(new NamedId(item.Name, item.Id));
            }
            return list;
        }
    }
}
