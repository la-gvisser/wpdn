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
    using System.Linq;
    using System.Text;

    public class NamedId
    {
        static public bool IsNullOrEmpty(NamedId item)
        {
            return (item == null) || (item.Id.Id == Guid.Empty);
        }

        public OmniId Id { get; set; }
        public String Name { get; set; }

        public NamedId(Guid id, String name)
        {
            this.Id = id;
            this.Name = name;
        }

        public NamedId(String name, Guid id)
            : this(id, name)
        {
        }

        public String ToString(String format)
        {
            if (format != null)
            {
                if ((format == "F") || (format == "f"))
                {
                    return this.Name + " : " + this.Id.ToString();
                }

                if ((format[0] == 'P') || (format[0] == 'p'))
                {
                    int pad = 0;
                    String padding = format.Substring(1);
                    if (padding.Length > 0)
                    {
                        pad = Convert.ToInt32(format.Substring(1));
                    }
                    return this.Name.PadRight(pad) + " : " + this.Id.ToString();
                }
            }
            return this.Id.ToString();
        }

        public override String ToString()
        {
            return this.ToString(null);
        }
    }
}
