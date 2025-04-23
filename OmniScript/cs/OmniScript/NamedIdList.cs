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
    using System.IO;

    public class NamedIdList
        : OmniList<NamedId>
    {
        public static NamedIdList Read(String filename)
        {
            NamedIdList list = new NamedIdList();
            StreamReader reader = null;
            try
            {
                reader = new StreamReader(filename);
                do
                {
                    String line = reader.ReadLine();
                    if (!String.IsNullOrEmpty(line))
                    {
                        String[] parts = line.Split(':');
                        NamedId namedId = new NamedId(parts[0].Trim(), Guid.Parse(parts[1]));
                        list.Add(namedId);
                    }
                }
                while (reader.Peek() != -1);
            }
            catch
            {
                ;
            }
            finally
            {
                if (reader != null)
                {
                    reader.Close();
                }
            }
            return list;
        }

        /// <summary>
        /// Find a NamedId by its name.
        /// </summary>
        /// <param name="name">The name of the NamedId to match.</param>
        /// <returns>The NamedId object was found or null.</returns>
        public NamedId Find(String name)
        {
            return this.Find(
                delegate(NamedId namedId)
                {
                    return namedId.Name == name;
                });
        }

        /// <summary>
        /// Find a NamedId by its id.
        /// </summary>
        /// <param name="name">The id of the NamedId to match.</param>
        /// <returns>The NamedId object was found or null.</returns>
        public NamedId Find(OmniId id)
        {
            return this.Find(
                delegate(NamedId namedId)
                {
                    return namedId.Id.Id == id.Id;
                });
        }

        /// <summary>
        /// Find a NamedId by its GUID.
        /// </summary>
        /// <param name="name">The GUID of the NamedId to match.</param>
        /// <returns>The NamedId object was found or null.</returns>
        public NamedId Find(Guid id)
        {
            return this.Find(
                delegate(NamedId namedId)
                {
                    return namedId.Id.Id == id;
                });
        }

        public int LongestName(int minLength = 10)
        {
            int longest = minLength;
            foreach (NamedId item in this)
            {
                if (item.Name.Length > longest)
                {
                    longest = item.Name.Length;
                }
            }
            return longest;
        }

        public void Save(String filename)
        {
            using (StreamWriter outfile = new StreamWriter(filename))
            {
                foreach (NamedId namedId in this)
                {
                    outfile.Write(namedId.ToString("F"));
                    outfile.Write(Environment.NewLine);
                }
            }
        }

        public void WriteToLog(OmniLogger log, String pad = "")
        {
            String format = String.Format("P{0}", this.LongestName());
            foreach (NamedId item in this)
            {
                log.Log(pad + item.ToString(format));
            }
        }
    }
}
