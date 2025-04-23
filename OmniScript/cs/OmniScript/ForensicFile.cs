// =============================================================================
// <copyright file="Adapter.cs" company="LiveAction, Inc.">
//  Copyright (c) 2018-2021 Savvius, Inc. All rights reserved.
//  Copyright (c) 2015 Savvius, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;

    public class ForensicFile
    {
        public String AdapterAddress { get; set; }
        public String AdapterName { get; set; }
        public OmniId CaptureId { get; set; }
        public String CaptureName { get; set; }
        public UInt64 DroppedPacketCount { get; set; }
        public PeekTime EndTime { get; set; }
        public uint FileIndex { get; set; }
        public UInt64 LinkSpeed { get; set; }
        public uint MediaType { get; set; }
        public uint MediaSubType { get; set; }
        public String Name { get; set; }
        public UInt64 PacketCount { get; set; }
        public String Path { get; set; }
        public OmniId SessionId { get; set; }
        public UInt64 Size { get; set; }
        public PeekTime StartTime { get; set; }
        public uint Status { get; set; }
        public int TimeZoneBias { get; set; }

        public ForensicFile(List<String> labels, List<Object> values)
        {
            this.Load(labels, values);
        }

        public void Load(List<String> labels, List<Object> values)
        {
            if ((labels == null) || (values == null)) return;

            for (int i = 0; i < values.Count(); i++)
            {
                switch (labels[i])
                {
                    case "AdapterAddr":
                        LoadAdapterAddress(values[i]);
                        break;

                    case "AdapterName":
                        LoadAdapterName(values[i]);
                        break;

                    case "CaptureName":
                        LoadCaptureName(values[i]);
                        break;

                    case "CaptureID":
                        LoadCaptureId(values[i]);
                        break;

                    case "DroppedCount":
                        LoadDroppedPacketCount(values[i]);
                        break;

                    case "DroppedPacketCount":
                        LoadDroppedPacketCount(values[i]);
                        break;

                    case "FileIndex":
                        LoadFileIndex(values[i]);
                        break;

                    case "FileName":
                        LoadName(values[i]);
                        break;

                    case "FileSize":
                        LoadSize(values[i]);
                        break;

                    case "LinkSpeed":
                        LoadLinkSpeed(values[i]);
                        break;

                    case "MediaSubType":
                        LoadMediaSubType(values[i]);
                        break;

                    case "MediaType":
                        LoadMediaType(values[i]);
                        break;

                    case "PacketCount":
                        LoadPacketCount(values[i]);
                        break;

                    case "PartialPath":
                        LoadPath(values[i]);
                        break;

                    case "Status":
                        LoadStatus(values[i]);
                        break;

                    case "SessionEndTime":
                        LoadEndTime(values[i]);
                        break;

                    case "SessionID":
                        LoadSessionId(values[i]);
                        break;

                    case "SessionStartTime":
                        LoadStartTime(values[i]);
                        break;

                    case "TimeZoneBias":
                        LoadTimeZoneBias(values[i]);
                        break;
                }
            }
        }

        public void LoadAdapterAddress(Object obj)
        {
            this.AdapterAddress = obj as String;
        }

        public void LoadAdapterName(Object obj)
        { 
            this.AdapterName = obj as String;
        }
        
        public void LoadCaptureId(Object obj)
        {
            this.CaptureId = obj as OmniId;
        }
        
        public void LoadCaptureName(Object obj)
        {
            this.CaptureName = obj as String;
        }
        
        public void LoadDroppedPacketCount(Object obj)
        {
            this.DroppedPacketCount = Convert.ToUInt64(obj);
        }
        
        public void LoadEndTime(Object obj)
        {
            this.EndTime = obj as PeekTime;
        }
        
        public void LoadFileIndex(Object obj)
        {
            this.FileIndex = Convert.ToUInt32(obj);
        }
        
        public void LoadLinkSpeed(Object obj)
        {
            this.LinkSpeed = Convert.ToUInt64(obj);
        }
        
        public void LoadMediaType(Object obj)
        {
            this.MediaType = Convert.ToUInt32(obj);
        }
        
        public void LoadMediaSubType(Object obj)
        {
            this.MediaSubType = Convert.ToUInt32(obj);
        }
        
        public void LoadName(Object obj)
        {
            this.Name = obj as String;
        }
        
        public void LoadPacketCount(Object obj)
        {
            this.PacketCount = Convert.ToUInt64(obj);
        }
        
        public void LoadPath(Object obj)
        {
            this.Path = obj as String;
        }
        
        public void LoadSessionId(Object obj)
        {
            this.SessionId = obj as OmniId;
        }
        
        public void LoadSize(Object obj)
        {
            this.Size = Convert.ToUInt64(obj);
        }
        
        public void LoadStartTime(Object obj)
        {
            this.StartTime = obj as PeekTime;
        }
        
        public void LoadStatus(Object obj)
        {
            this.Status = Convert.ToUInt32(obj);
        }
        
        public void LoadTimeZoneBias(Object obj)
        {
            this.TimeZoneBias = Convert.ToInt32(obj);
        }
    
    }
}
