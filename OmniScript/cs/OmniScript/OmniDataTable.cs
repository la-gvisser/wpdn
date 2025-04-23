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

    public class OmniDataTable
    {
        public List<String> Labels { get; set; }
        public List<ushort> Types { get; set; }
        public List<List<Object>> Rows { get; set; }

        public OmniDataTable()
        {
            this.Labels = new List<string>();
            this.Types = new List<ushort>();
            this.Rows = new List<List<Object>>();
        }

        public void Load(byte[] data)
        {
            if (data.Length < 8) return;
            ReadStream stream = new ReadStream(data);

            uint majorVersion = stream.ReadUInt();
            uint minorVersion = stream.ReadUInt();
            if (majorVersion != 3) throw new OmniException("Unsupported Data Table version.");

            uint columnCount = stream.ReadUInt();
            uint rowCount = stream.ReadUInt();
            for (uint c = 0; c < columnCount; c++)
            {
                this.Types.Add(stream.ReadUShort());
                this.Labels.Add(stream.ReadUnicode());
            }

            for (uint r = 0; r < rowCount; r++)
            {
                List<Object> row = new List<Object>();
                for (uint c = 0; c < columnCount; c++)
                {
                    ushort type = stream.ReadUShort();
                    switch (type)
                    {
                        case 3:
                            row.Add(stream.ReadUInt());
                            break;

                        case 8:
                            row.Add(stream.ReadString());
                            break;

                        case 19:
                            row.Add(stream.ReadInt());
                            break;

                        case 21:
                            row.Add(stream.ReadULong());
                            break;

                        case 22:
                            row.Add(stream.ReadUInt());
                            break;

                        case 23:
                            row.Add(stream.ReadUInt());
                            break;

                        case 0:
                            row.Add(null);
                            break;

                        default:
                            // Raise Invalid Data Type.
                            break;
                    }
                }
                this.Rows.Add(row);
            }
        }
    }
}
