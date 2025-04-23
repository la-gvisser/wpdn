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

    /// <summary>
    /// List of ForensicFile objects.
    /// </summary>
    public class ForensicFileList
        : OmniList<ForensicFile>
    {
        public OmniLogger logger;

        public ForensicFileList(OmniLogger logger = null, OmniDataTable table=null)
        {
            this.logger = logger;
            this.Load(table);
        }

        /// <summary>
        /// Find an ForensicFile by its name.
        /// </summary>
        /// <param name="name">The name of the ForensicFile to match.</param>
        /// <returns>The ForensicFile object was found or null.</returns>
        public ForensicFile Find(String name)
        {
            return this.Find(
                delegate(ForensicFile forensicFile)
                {
                    return forensicFile.Name == name;
                });
        }

        public void Load(OmniDataTable table)
        {
            if (table == null) return;

            foreach (List<Object> row in table.Rows)
            {
                this.Add(new ForensicFile(table.Labels, row));
            }
            // Sort the list.
            this.Sort((x, y) =>
                x.Name.CompareTo(y.Name));
        }
    }
}

