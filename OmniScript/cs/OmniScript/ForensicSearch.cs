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
    using System.Xml.Linq;

    public class ForensicSearch
    {
        public const String RootName = "FileView";

        //private OmniEngine Engine;
        //private Object Context;
        public OmniLogger Logger;

        public String Adapter;
        public String CaptureName;
        public OmniId SessionId;
        public String Creator;
        public String CreatorSid;
        public UInt64 Duration;
        public UInt64 FirstPacket;
        public OmniId Id;
        public UInt64 LinkSpeed;
        public String LoadProgress;
        public String LoadPercent;
        public Object MediaInfo;
        public uint MediaType;
        public uint MediaSubType;
        public String ModifiedBy;
        public String ModificationType;
        public String Name;
        public uint OpenResult;
        public UInt64 PacketCount;
        public uint PercentProgress;
        public uint ProcessPercentProgress;
        public String ProcessProgress;
        public String Progress;
        public PeekTime StartTime;
        public uint Status;
        public PeekTime StopTime;
        public bool OptionExpert;
        public bool OptionGraphs;
        public bool OptionLog;
        public bool OptionPacketBuffer;
        public bool OptionVoice;
        public bool OptionWeb;

        // XML Tags
        const String tagId = "ID";
        const String tagName = "Name";
        const String tagAdapter = "Adapter";
        const String tagCaptureName = "CaptureName";
        const String tagCaptureSessionId = "CaptureSessionID";
        const String tagCreator = "Creator";
        const String tagCreatorSID = "Creator-SID";
        const String tagDuration = "Duration";
        const String tagStatus = "File View Status";
        const String tagFirstPacket = "First Packet";
        const String tagLinkSpeed = "Link Speed";
        const String tagLoadPercent = "Load Percent Progress";
        const String tagLoadProgress = "Load Progress";
        const String tagMediaType = "Media Type";
        const String tagMediaSubType = "Media Subtype";
        const String tagModifiedBy = "Mod By";
        const String tagModificationType = "Mod Type";
        const String tagOpenResult = "Open Result";
        const String tagPacketCount = "Packet Count";
        const String tagPercentProgress = "Percent Progress";
        const String tagProcessPercentProgress = "Process Percent Progress";
        const String tagProcessProgress = "Process Progress";
        const String tagProgress = "Progress";
        const String tagStartTime = "Start Time";
        const String tagStopTime = "Stop Time";
        const String tagMediaInfo = "MediaInfo";
        const String tagOptionExpert = "Expert Enabled";
        const String tagOptionGraphs = "Graphs-Enabled";
        const String tagOptionLog = "Log Enabled";
        const String tagOptionPacketBuffer = "Packet Buffer Enabled";
        const String tagOptionVoice = "Voice Enabled";
        const String tagOptionWeb = "Web Enabled";

        public ForensicSearch(OmniLogger logger, XElement node)
        {
            this.Logger = logger;
            this.Load(node);
        }

        public void Load(XElement node)
        {
            if (node == null) return;

            uint type = 0;
            IEnumerable<XElement> props = node.Elements("prop");
            foreach (XElement prop in props)
            {
                String name = prop.Attribute("name").Value;
                switch (name)
                {
                    case tagId:
                        {
                            XAttribute tipe = prop.Attribute("type");
                            type = (tipe != null) ? Convert.ToUInt32(tipe.Value) : 0;
                            if (type == 8)
                            {
                                this.Id = Guid.Parse(prop.Value);
                            }
                        }
                        break;

                    case tagName:
                        this.Name = prop.Value;
                        break;

                    case tagAdapter:
                        this.Adapter = prop.Value;
                        break;

                    case tagCaptureName:
                        this.CaptureName = prop.Value;
                        break;

                    case tagCaptureSessionId:
                        {
                            XAttribute tipe = prop.Attribute("type");
                            type = (tipe != null) ? Convert.ToUInt32(tipe.Value) : 0;
                            if (type == 8)
                            {
                                this.SessionId = new OmniId(prop.Value);
                            }
                        }
                        break;

                    case tagCreator:
                        this.Creator = prop.Value;
                        break;

                    case tagCreatorSID:
                        this.CreatorSid = prop.Value;
                        break;

                    case tagDuration:
                        this.Duration = Convert.ToUInt64(prop.Value);
                        break;

                    case tagStatus:
                        this.Status = Convert.ToUInt32(prop.Value);
                        break;

                    case tagFirstPacket:
                        this.FirstPacket = Convert.ToUInt64(prop.Value);
                        break;

                    case tagLinkSpeed:
                        this.LinkSpeed = Convert.ToUInt64(prop.Value);
                        break;

                    case tagLoadPercent:
                        this.LoadPercent = prop.Value;
                        break;

                    case tagLoadProgress:
                        this.LoadProgress = prop.Value;
                        break;

                    case tagMediaType:
                        this.MediaType = Convert.ToUInt32(prop.Value);
                        break;

                    case tagMediaSubType:
                        this.MediaSubType = Convert.ToUInt32(prop.Value);
                        break;

                    case tagModifiedBy:
                        this.ModifiedBy = prop.Value;
                        break;

                    case tagModificationType:
                        this.ModificationType = prop.Value;
                        break;

                    case tagOpenResult:
                        this.OpenResult = Convert.ToUInt32(prop.Value);
                        break;

                    case tagPacketCount:
                        this.PacketCount = Convert.ToUInt64(prop.Value);
                        break;

                    case tagPercentProgress:
                        this.PercentProgress = Convert.ToUInt32(prop.Value);
                        break;

                    case tagProcessPercentProgress:
                        this.ProcessPercentProgress = Convert.ToUInt32(prop.Value);
                        break;

                    case tagProcessProgress:
                        this.ProcessProgress = prop.Value;
                        break;

                    case tagProgress:
                        this.Progress = prop.Value;
                        break;

                    case tagStartTime:
                        this.StartTime = new PeekTime(prop.Value);
                        break;

                    case tagStopTime:
                        this.StopTime = new PeekTime(prop.Value);
                        break;

                    case tagMediaInfo:
                        break;

                    case tagOptionExpert:
                        this.OptionExpert = OmniScript.PropBoolean(prop.Value);
                        break;

                    case tagOptionGraphs:
                        this.OptionGraphs = OmniScript.PropBoolean(prop.Value);
                        break;

                    case tagOptionLog:
                        this.OptionLog = OmniScript.PropBoolean(prop.Value);
                        break;

                    case tagOptionPacketBuffer:
                        this.OptionPacketBuffer = OmniScript.PropBoolean(prop.Value);
                        break;

                    case tagOptionVoice:
                        this.OptionVoice = OmniScript.PropBoolean(prop.Value);
                        break;

                    case tagOptionWeb:
                        this.OptionWeb = OmniScript.PropBoolean(prop.Value);
                        break;
                }
            }
        }
    }
}
