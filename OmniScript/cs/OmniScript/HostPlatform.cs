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

    public class HostPlatform
    {
        private const uint CaptureMagic = 0xABCD;
        private const uint VigilMagic = 0x5653;
        private const uint InSightMagic = 0x4953;

        public static readonly String DefaultAuth = "Default";
        public static readonly String WindowsAuth = "Default";
        public static readonly String LinuxAuth = "ThirdPartyAuth";
        public static readonly String MacAuth = "ThirdPartyAuth";

        public enum EngineKind { Unknown, Capture, Vigil, InSight };
        public enum OperatingSystem { Unknown, Windows, Linux, Mac };

        private readonly Dictionary<uint, EngineKind> KindMap = new Dictionary<uint, EngineKind>()
        {
            { CaptureMagic, EngineKind.Capture },
            { VigilMagic, EngineKind.Vigil },
            { InSightMagic, EngineKind.InSight }
        };

        public EngineKind Kind { get; set; }
        public uint Magic { get; set; }
        public uint Major { get; set; }
        public uint Minor { get; set; }
        public OperatingSystem OS { get; set; }

        public HostPlatform(uint magic, uint version, byte os)
        {
            this.Magic = magic;
            this.Kind = (KindMap.ContainsKey(magic)) ? KindMap[magic] : EngineKind.Unknown;
            this.Major = version & 0x00FF;   // 0x0501 == 1.5
            this.Minor = (version & 0x0FF00) >> 8;
            
            switch (os)
            {
                case 3:
                    this.OS = OperatingSystem.Windows;
                    break;

                case 70:
                    this.OS = OperatingSystem.Mac;
                    break;

                case 100:
                    this.OS = OperatingSystem.Linux;
                    break;

                default:
                    this.OS = OperatingSystem.Unknown;
                    break;
            }
        }

        static public String GetAuth(HostPlatform platform, String auth)
        {
            if (String.IsNullOrEmpty(auth))
            {
                if (platform != null) {
                    switch (platform.OS)
                    {
                        case OperatingSystem.Windows:
                            return HostPlatform.WindowsAuth;

                        case OperatingSystem.Linux:
                            return HostPlatform.LinuxAuth;

                        case OperatingSystem.Mac:
                            return HostPlatform.MacAuth;

                        default:
                            return HostPlatform.DefaultAuth;
                    }
                }
                return HostPlatform.DefaultAuth;
            }

            String lowauth = auth.ToLower().Replace(' ', '-');
            if (lowauth == "default")
            {
                return HostPlatform.WindowsAuth;
            }
            else if ((lowauth == "thirdparty") || (lowauth == "third-party"))
            {
                return HostPlatform.LinuxAuth;
            }

            return HostPlatform.DefaultAuth;
        }

        public ulong GetProtocol()
        {
            return (this.Magic << 16) | ((this.Minor << 8) & 0x0FF) | (this.Major & 0x0FF);
        }
    }
}
