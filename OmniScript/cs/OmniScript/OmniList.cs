// =============================================================================
// <copyright file="Adapter.cs" company="LiveAction, Inc.">
//  Copyright (c) 2018-2021 Savvius, Inc. All rights reserved.
//  Copyright (c) 2015-2018 Savvius, Inc. All rights reserved.
//  Copyright (c) 2013-2015 WildPackets, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System.Collections.Generic;

    /// <summary>
    /// An extension of the List template with additional utility methods.
    /// </summary>
    /// <typeparam name="T">The class of object that the list manages.</typeparam>
    public class OmniList<T>
        : List<T>
    {
        public OmniList()
        {
        }

        public OmniList(IEnumerable<T> collection)
            : base(collection)
        {
        }

        /// <summary>
        /// Is this list null or empty.
        /// </summary>
        /// <param name="list">The list to examine.</param>
        /// <returns>Returns true if the list is either null or empty, false otherwise.</returns>
        public static bool IsNullOrEmpty(List<T> list)
        {
            return (list == null) || (list.Count == 0);
        }
    }
}
