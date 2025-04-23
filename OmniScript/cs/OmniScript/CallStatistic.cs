// =============================================================================
// <copyright file="Adapter.cs" company="LiveAction, Inc.">
//  Copyright (c) 2018-2021 Savvius, Inc. All rights reserved.
//  Copyright (c) 2015-2018 Savvius, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System.Collections.Generic;

    public class CallStatistic
    {
        public CallStatistic(ReadStream stream)
        {
            this.Load(stream);
        }

        private void Load(ReadStream stream)
        {
            if (stream == null) return;
        }
    }

    public class CallStatistics : List<CallStatistic>
    {
    }
}
