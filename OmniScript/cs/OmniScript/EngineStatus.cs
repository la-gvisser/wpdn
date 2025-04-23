// =============================================================================
// <copyright file="Adapter.cs" company="LiveAction, Inc.">
//  Copyright (c) 2018-2021 Savvius, Inc. All rights reserved.
//  Copyright (c) 2015-2018 Savvius, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System;
    using System.Runtime.InteropServices;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Xml.Linq;
    using System.Text;
    using System.Globalization;

    public class EngineStatus
    {
        public const String RootName = "properties";

        private OmniEngine Engine { get; set; }
        private OmniApi Connection { get; set; }
        public OmniLogger Logger { get; set; }

        /// <summary>
        /// Gets or sets the number of adapters.
        /// </summary>
        public uint AdapterCount { get; set; }

        /// <summary>
        /// Gets or sets the number of alarms.
        /// </summary>
        public uint AlarmCount { get; set; }

        /// <summary>
        /// Gets or sets the number of captures.
        /// </summary>
        public uint CaptureCount { get; set; }

        /// <summary>
        /// Gets or sets the number of CPUs on the host system.
        /// </summary>
        public uint CPUCount { get; set; }

        /// <summary>
        /// Gets or sets the type of CPU on the host system.
        /// </summary>
        public String CPUType { get; set; }

        /// <summary>
        /// Gets or sets the path of the Data Folder.
        /// </summary>
        public String DataFolder { get; set; }

        /// <summary>
        /// Gets or sets the type of Engine.
        /// </summary>
        public String EngineType { get; set; }

        /// <summary>
        /// Gets or sets the number of files.
        /// </summary>
        public uint FileCount { get; set; }

        /// <summary>
        /// Gets or sets the File Version of the Engine.
        /// </summary>
        public String FileVersion { get; set; }

        /// <summary>
        /// Gets or sets the number of filters.
        /// </summary>
        public uint FilterCount { get; set; }

        /// <summary>
        /// Gets or sets the last time the filters where modified.
        /// </summary>
        public PeekTime FilterModificationTime { get; set; }

        /// <summary>
        /// Gets or sets the number of Forensic Searches.
        /// </summary>
        public uint ForensicSearchCount { get; set; }

        /// <summary>
        /// Gets or sets the number of graphs.
        /// </summary>
        public uint GraphCount { get; set; }

        /// <summary>
        /// Gets or sets the name of the host system.
        /// </summary>
        public String Host { get; set; }

        /// <summary>
        /// Gets or sets the total number of log entries.
        /// </summary>
        public uint LogTotalCount { get; set; }

        /// <summary>
        /// Gets or sets the amount of physical memory available.
        /// </summary>
        public ulong MemoryAvailablePhysical { get; set; }

        /// <summary>
        /// Gets or sets the amount of total amount of physical memory.
        /// </summary>
        public ulong MemoryTotalPhysical { get; set; }

        /// <summary>
        /// Gets or sets the name of the Engine.
        /// </summary>
        public String Name { get; set; }

        /// <summary>
        /// Gets or sets the total number name table entries.
        /// </summary>
        public uint NameTableCount { get; set; }

        /// <summary>
        /// Gets or sets the number of notifications.
        /// </summary>
        public uint NotificationCount { get; set; }

        /// <summary>
        /// Gets or sets the name of the host system's operating system.
        /// </summary>
        public String OS { get; set; }

        /// <summary>
        /// Gets or sets the name of the host system's platform.
        /// </summary>
        public String Platform { get; set; }

        /// <summary>
        /// Gets or sets the communication port.
        /// </summary>
        public uint Port { get; set; }

        /// <summary>
        /// Gets or sets the Product version of the Engine.
        /// </summary>
        public String ProductVersion { get; set; }

        /// <summary>
        /// Gets or sets the storage space available.
        /// </summary>
        public ulong StorageAvailable { get; set; }

        /// <summary>
        /// Gets or sets the total amount of storage space.
        /// </summary>
        public ulong StorageTotal { get; set; }

        /// <summary>
        /// Gets or sets the amount of storage space used.
        /// </summary>
        public ulong StorageUsed { get; set; }

        /// <summary>
        /// Gets or sets the host system's time of day.
        /// </summary>
        public PeekTime Time { get; set; }

        /// <summary>
        /// Gets or sets the host system's timezone bias.
        /// </summary>
        public int TimeZoneBias { get; set; }

        /// <summary>
        /// Gets or sets the number of nanoseconds the Engine has been up.
        /// </summary>
        public PeekTime UpTime { get; set; }

        public EngineStatus(OmniEngine engine)
        {
            this.Engine = engine;
            this.Connection = engine.Connection;
            this.Logger = engine.Logger;

            this.AdapterCount = 0;
            this.AlarmCount = 0;
            this.CaptureCount = 0;
            this.CPUCount = 0;
            this.CPUType = "";
            this.DataFolder = "";
            this.EngineType = "";
            this.FileCount = 0;
            this.FileVersion = "";
            this.FilterCount = 0;
            this.FilterModificationTime = new PeekTime();
            this.ForensicSearchCount = 0;
            this.GraphCount = 0;
            this.Host = "";
            this.LogTotalCount = 0;
            this.MemoryAvailablePhysical = 0;
            this.MemoryTotalPhysical = 0;
            this.Name = "";
            this.NameTableCount = 0;
            this.NotificationCount = 0;
            this.OS = "";
            this.Platform = "";
            this.Port = 0;
            this.ProductVersion = "";
            this.StorageAvailable = 0;
            this.StorageTotal = 0;
            this.StorageUsed = 0;
            this.Time = new PeekTime();
            this.TimeZoneBias = 0;
            this.UpTime = new PeekTime();
        }

        public EngineStatus(OmniEngine engine, XElement status)
            : this(engine)
        {
            this.Load(status);
        }

        private void Load(XElement status)
        {
            if (status == null) return;

            IEnumerable<XElement> props = status.Elements("prop");
            foreach (XElement prop in props)
            {
                switch (prop.Attribute("name").Value)
                {
                    case "AdapterCount":
                        this.AdapterCount = Convert.ToUInt32(prop.Value);
                        break;

                    case "AlarmCount":
                        this.AlarmCount = Convert.ToUInt32(prop.Value);
                        break;

                    case "Captures":
                        this.CaptureCount = Convert.ToUInt32(prop.Value);
                        break;

                    case "CaptureSessionCount":
                        this.ForensicSearchCount = Convert.ToUInt32(prop.Value);
                        break;

                    case "CpuCount":
                        this.CPUCount = Convert.ToUInt32(prop.Value);
                        break;

                    case "CpuType":
                        this.CPUType = prop.Value;
                        break;

                    case "DataFolder":
                        this.DataFolder = prop.Value;
                        break;

                    case "engine-type":
                        this.EngineType = prop.Value;
                        break;

                    case "FileCount":
                        this.FileCount = Convert.ToUInt32(prop.Value);
                        break;

                    case "FileVersion":
                        this.FileVersion = prop.Value;
                        break;

                    case "FilterCount":
                        this.FilterCount = Convert.ToUInt32(prop.Value);
                        break;

                    case "FiltersModTime":
                        this.FilterModificationTime = new PeekTime(prop.Value);
                        break;

                    case "ForensicSearches":
                        this.ForensicSearchCount = Convert.ToUInt32(prop.Value);
                        break;

                    case "GraphCount":
                        this.GraphCount = Convert.ToUInt32(prop.Value);
                        break;

                    case "LogTotalCount":
                        this.LogTotalCount = Convert.ToUInt32(prop.Value);
                        break;

                    case "MemAvailPhys":
                        this.MemoryAvailablePhysical = Convert.ToUInt64(prop.Value);
                        break;

                    case "MemTotalPhys":
                        this.MemoryTotalPhysical = Convert.ToUInt64(prop.Value);
                        break;

                    case "Name":
                        this.Name = prop.Value;
                        break;

                    case "NameCount":
                        this.NameTableCount = Convert.ToUInt32(prop.Value);
                        break;

                    case "NotificationCount":
                        this.NotificationCount = Convert.ToUInt32(prop.Value);
                        break;

                    case "OperatingSystem":
                        this.OS = prop.Value;
                        break;

                    case "OS":
                        this.OS = prop.Value;
                        break;

                    case "Platform":
                        this.Platform = prop.Value;
                        break;

                    case "ProductVersion":
                        this.ProductVersion = prop.Value;
                        break;

                    case "StorageTotal":
                        this.StorageTotal = Convert.ToUInt64(prop.Value);
                        break;

                    case "StorageUsed":
                        this.StorageUsed = Convert.ToUInt64(prop.Value);
                        break;

                    case "Time":
                        this.Time = new PeekTime(prop.Value);
                        break;

                    case "TimeZoneBias":
                        this.TimeZoneBias = Convert.ToInt32(prop.Value);
                        break;

                    case "Uptime":
                        this.UpTime = new PeekTime(prop.Value);
                        break;
                }
            }

            this.CPUType = String.Join(" ", this.CPUType.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries));
            this.StorageAvailable = this.StorageTotal = this.StorageUsed;
        }

        public void Refresh()
        {
            String response = this.Connection.XmlCommand( OmniApi.OMNI_GET_STATUS, null);
            XElement props = OmniApi.ParseResponse(response, EngineStatus.RootName);
            if (props != null)
            {
                this.Load(props);
            }
        }
    }
}
