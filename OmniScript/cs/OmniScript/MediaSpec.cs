// =============================================================================
// <copyright file="Adapter.cs" company="LiveAction, Inc.">
//  Copyright (c) 2018-2021 Savvius, Inc. All rights reserved.
//  Copyright (c) 2015 Savvius, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System;
    using System.Runtime.InteropServices;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Text;

    public class MediaSpec
    {
        public enum MediaSpecs
        {
            Null = 0,
            EthernetProtocol,
            Lsap,
            Snap,
            Lap,
            Ddp,
            MacControl,
            ApplicationId = 8,
            ProtoSpec,
            EthernetAddress,
            TokenringAddress,
            LapAddress,
            WirelessAddress,
            AppletalkAddress = 20,
            IpAddress,
            DecnetAddress,
            OtherAddress,
            IpV6Address,
            IpxAddress,
            Error = 32,
            AtPort,
            IpPort,
            NetwarePort,
            TcpPortPair,
            WanPppProtocol,
            WanFramerelayProtocol,
            WanX25Protocol,
            WanX25eProtocol,
            WanIparsProtocol,
            WanU200Protocol,
            WanDlciAddress,
            WanQ931Protocol,
        }

        public enum MediaClasses
        {
            None = 0,
            Protocol,
            Address,
            Port
        }

        public enum WirelessMediaDomains
        {
            None = 0,
            FCC,
            MKK,
            ETSU
        }

        public enum MediaTypes
        {
            k802_3 = 0,
            k802_5,
            Fddi,
            Wan,
            Localtalk,
            Dix,
            ArcnetRaw,
            Arcnet878_2,
            ATM,
            WirelessWan,
            IRDA,
            BPC,
            CoWan,
            k1394,
            Max
        }

        public enum MediaSubTypes
        {
            Native = 0,
            k802_11_B,
            k802_11_A,
            k802_11_General,
            Wan_Ppp,
            Wan_FrameRelay,
            Wan_X25,
            Wan_X25E,
            Wan_Ipars,
            Wan_U200,
            Wan_Q931,
            MAX
        }

        private String RenderOther()
        {
            return String.Format("{0:X}", this.Value);
        }

        private String RenderIPv4()
        {
            IPv4Address ipv4 = new IPv4Address(this.Value);
            return ipv4.ToString(".");
        }

        private String RenderIPv6()
        {
            String[] hextets = new String[8];
            for (int i = 0; i < 8; i++)
            {
                hextets[i] = String.Format("{0:X2}{1:X2}", this.Value[i * 2], this.Value[(i * 2) + 1]);
            }
            return IPv6Address.CompressHextets(hextets);
        }

        private String RenderEthernet()
        {
            EthernetAddress eth = new EthernetAddress(this.Value);
            return eth.ToString(":");
        }

        private String RenderPortPair()
        {
            return String.Format("{0}-{1}",
                BitConverter.ToUInt16(this.Value, 0),
                BitConverter.ToUInt16(this.Value, 2));
        }

        private String RenderProtoSpec()
        {
            uint index = BitConverter.ToUInt16(this.Value, 0);
            OmniScript.IndexNameMap shortNames = OmniScript.GetProtocolIdShortNames();
            if (shortNames.ContainsKey(index))
            {
                return shortNames[index];
            }
            return "";
        }

        private String RenderApplication()
        {
            return String.Format("{0}{1}{2}{3}{4}{5}{6}{7}",
                this.Value[0], this.Value[1], this.Value[2], this.Value[3],
                this.Value[4], this.Value[5], this.Value[6], this.Value[7]);
        }

        public MediaClasses MediaClass;
        public MediaSpecs MediaSpecification;
        public MediaTypes MediaType;
        public byte[] Value;
        public String PreFormatted;

        public MediaSpec(MediaClasses mediaClass, MediaTypes mediaType, byte[] value)
        {
            this.MediaClass = mediaClass;
            this.MediaType = mediaType;
            this.Value = value;
            this.PreFormatted = null;
        }

        public MediaSpec(MediaClasses mediaClass, MediaSpecs mediaSpec, byte[] value)
        {
            this.MediaClass = mediaClass;
            this.MediaSpecification = mediaSpec;
            this.Value = value;
        }

        public String Report()
        {
            if (this.PreFormatted == null)
            {
                switch (this.MediaSpecification)
                {
                    case MediaSpecs.Null:
                        this.PreFormatted = "";
                        break;
                    
                    case MediaSpecs.ApplicationId:
                        this.PreFormatted = RenderApplication();
                        break;
                    
                    case MediaSpecs.ProtoSpec:
                        this.PreFormatted = RenderProtoSpec();
                        break;
                    
                    case MediaSpecs.EthernetAddress:
                    case MediaSpecs.TokenringAddress:
                    case MediaSpecs.LapAddress:
                    case MediaSpecs.WirelessAddress:
                        this.PreFormatted = RenderEthernet();
                        break;

                    case MediaSpecs.IpAddress:
                        this.PreFormatted = RenderIPv4();
                        break;

                    case MediaSpecs.IpV6Address:
                        this.PreFormatted = RenderIPv6();
                        break;

                    case MediaSpecs.TcpPortPair:
                        this.PreFormatted = RenderPortPair();
                        break;

//                     case MediaSpecs.EthernetProtocol:
//                     case MediaSpecs.Lsap:
//                     case MediaSpecs.Snap:
//                     case MediaSpecs.Lap:
//                     case MediaSpecs.Ddp:
//                     case MediaSpecs.MacControl:
//                     case MediaSpecs.AppletalkAddress:
//                     case MediaSpecs.DecnetAddress:
//                     case MediaSpecs.OtherAddress:
//                     case MediaSpecs.IpxAddress:
//                     case MediaSpecs.Error:
//                     case MediaSpecs.AtPort:
//                     case MediaSpecs.IpPort:
//                     case MediaSpecs.NetwarePort:
//                     case MediaSpecs.WanPppProtocol:
//                     case MediaSpecs.WanFramerelayProtocol:
//                     case MediaSpecs.WanX25Protocol:
//                     case MediaSpecs.WanX25eProtocol:
//                     case MediaSpecs.WanIparsProtocol:
//                     case MediaSpecs.WanU200Protocol:
//                     case MediaSpecs.WanDlciAddress:
//                     case MediaSpecs.WanQ931Protocol:
                    default:
                        this.PreFormatted = RenderOther();
                        break;
                }
            }
            return this.PreFormatted;
        }
    }
}
