﻿// =============================================================================
// <copyright file="Adapter.cs" company="LiveAction, Inc.">
//  Copyright (c) 2018-2021 Savvius, Inc. All rights reserved.
//  Copyright (c) 2015-2018 Savvius, Inc. All rights reserved.
//  Copyright (c) 2013-2015 WildPackets, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System;
    using System.Runtime.InteropServices;
    using System.Globalization;
    using System.Text;
    using System.Xml.Linq;

    public class OmniApi
    {
        public const UInt32 OMNI_CONNECT = 1;
        public const UInt32 OMNI_DISCONNECT = 2;
        public const UInt32 OMNI_IS_CONNECTED = 3;
        public const UInt32 OMNI_GET_STATS_CONTEXT = 4;

        public const UInt32 OMNI_GET_VERSION = 100;
        public const UInt32 OMNI_GET_STATUS = 110;
        public const UInt32 OMNI_GET_CAPABILITIES = 111;
        public const UInt32 OMNI_GET_LOG_MSGS = 112;
        public const UInt32 OMNI_GET_ADAPTER_LIST = 113;
        public const UInt32 OMNI_GET_CAPTURE_LIST = 114;
        public const UInt32 OMNI_GET_CAPTURE_PROPS = 115;
        public const UInt32 OMNI_CREATE_CAPTURE = 116;
        public const UInt32 OMNI_DELETE_CAPTURES = 117;
        public const UInt32 OMNI_GET_CAPTURE_OPTIONS = 118;
        public const UInt32 OMNI_SET_CAPTURE_OPTIONS = 119;
        public const UInt32 OMNI_START_CAPTURES = 120;
        public const UInt32 OMNI_STOP_CAPTURES = 121;
        public const UInt32 OMNI_CLEAR_CAPTURES = 122;
        public const UInt32 OMNI_GET_STATS = 123;
        public const UInt32 OMNI_TAKE_SNAPSHOT = 124;
        public const UInt32 OMNI_GET_FILTERS = 125;
        public const UInt32 OMNI_SET_FILTERS = 126;
        public const UInt32 OMNI_GET_FILTER_CONFIG = 127;
        public const UInt32 OMNI_SET_FILTER_CONFIG = 128;
        public const UInt32 OMNI_GET_NOTIFICATIONS = 129;
        public const UInt32 OMNI_SET_NOTIFICATIONS = 130;
        public const UInt32 OMNI_GET_PACKETS = 131;
        public const UInt32 OMNI_SELECT_RELATED = 132;
        public const UInt32 OMNI_CLEAR_LOG = 133;
        public const UInt32 OMNI_GET_FILE_LIST = 137;
        public const UInt32 OMNI_GET_FILE = 138;
        public const UInt32 OMNI_DELETE_FILES = 139;
        public const UInt32 OMNI_SAVE_CAPTURE_FILE = 140;
        public const UInt32 OMNI_GET_ALARMS = 141;
        public const UInt32 OMNI_GET_GRAPH_TEMPLATES = 160;
        public const UInt32 OMNI_MODIFY_FILTERS = 173;
        public const UInt32 OMNI_EXPERT_EXECUTE_QUERY = 178;
        public const UInt32 OMNI_PLUGIN_MESSAGE = 179;
        public const UInt32 OMNI_GET_PACKETFILE_TABLE = 184;
        public const UInt32 OMNI_CREATE_FILE_VIEW = 185;
        public const UInt32 OMNI_DELETE_FILE_VIEW = 186;
        public const UInt32 OMNI_SYNC_DATABASE = 199;
        public const UInt32 OMNI_DELETE_ALL_CAPTURE_SESSIONS = 203;
        public const UInt32 OMNI_GET_FILE_VIEW_LIST = 210;
        public const UInt32 OMNI_SET_FILE = 213;
        public const UInt32 OMNI_GET_INSTALLED_PLUGINS = 523;
        public const UInt32 OMNI_GET_DIAGNOSTICS_INFO = 720;

        public const int WRITE_OP_CREATE = 0;
        public const int WRITE_OP_WRITE = 1;
        public const int WRITE_OP_CLOSE = 2;

#if _WINDOWS        
        [DllImport("./omniapi.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern void Initialize(UInt32 flags);

        [DllImport("./omniapi.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern UInt64 NewOmniConnection(UInt64 optional);

        [DllImport("./omniapi.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern IntPtr IssueCommand(UInt64 connection, UInt32 command, UInt32 timeout, byte[] request, UInt32 length, out Int32 count);

        [DllImport("./omniapi.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern IntPtr IssueXmlCommand(UInt64 connection, UInt32 command, UInt32 timeout, byte[] request, out Int32 count);

        [DllImport("./omniapi.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern void ReleaseResponse(UInt64 connection, IntPtr response);
#else
        [DllImport("./libshimomniapi.so", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Initialize(UInt32 flags);

        [DllImport("./libshimomniapi.so", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 NewOmniConnection(UInt64 optional);

        [DllImport("./libshimomniapi.so", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr IssueCommand(UInt64 connection, UInt32 command, UInt32 timeout, byte[] request, UInt32 length, out Int32 count);

        [DllImport("./libshimomniapi.so", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr IssueXmlCommand(UInt64 connection, UInt32 command, UInt32 timeout, byte[] request, out Int32 count);

        [DllImport("./libshimomniapi.so", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ReleaseResponse(UInt64 connection, IntPtr response);
#endif
        private UInt64 connection;
        private OmniEngine engine;
        private uint timeout;

        public static void Initialize()
        {
            OmniApi.Initialize(0);
        }

        public OmniApi(OmniEngine engine, UInt64 optional=0, uint timeout=10000)
        {
            this.connection = NewOmniConnection(optional);
            this.engine = engine;
            this.timeout = 20000; // 20 seconds.
        }

        public byte[] Command(UInt32 command, byte[] request=null, uint timeout=0)
        {
            UInt32 _length = (request != null) ? (UInt32)request.Length : 0;
            UInt32 _timeout = (timeout > 0) ? timeout : this.timeout;
            Int32 count = 0;
            IntPtr response = IssueCommand(this.connection, command, _timeout, request, _length, out count);
            if ((count == 0) || (response == IntPtr.Zero))
            {
                return null;
            }
            byte[] result = new byte[count];
            Marshal.Copy(response, result, 0, count);
            ReleaseResponse(this.connection, response);
            return result;
        }

        public String CommandString(UInt32 command, String request=null, uint timeout=0)
        {
            Encoding ascii = Encoding.ASCII;
            Encoding unicode = Encoding.Unicode;    // Windows 16-bit Unicode
            Encoding utf32 = Encoding.UTF32;        // Linux 32-bit Unicode

            byte[] bytesRequest = (request != null) ? unicode.GetBytes(request) : null;
            if (engine.Platform.OS == HostPlatform.OperatingSystem.Linux)
            {
            byte[] bytesUnicode = (bytesRequest != null) ? Encoding.Convert(unicode, utf32, bytesRequest) : null;
                if (bytesUnicode != null )
                {
                    bytesRequest = bytesUnicode;
                }
            }
            byte[] bytesResponse = this.Command(command, bytesRequest, timeout);
            if ((bytesResponse == null) || (bytesResponse.Length == 0))
            {
                return "<peek><msg hr=\"0x80004005\"/></peek>"; // HE_E_FAIL
            }
            return ascii.GetString( bytesResponse );
        }

        public OmniEngine GetEngine()
        {
            return this.engine;
        }

        public static int GetResponseCode(String xml)
        {
            XDocument doc = XDocument.Parse(xml);
            XElement root = doc.Root;
            XElement msg = root.Element("msg");
            if (msg == null) throw new OmniException("Invalid OmniEngine command response");

            XAttribute hrAttribute = msg.Attribute("hr");
            if (hrAttribute == null) throw new OmniException("OmniEngine command failed: 0x80004005.", 0x80004005);

            int hr = 0;
            String hrString = hrAttribute.Value;
            if (hrString.Length > 3)
            {
                hr = (hrString.Substring(0, 2) == "0x")
                    ? int.Parse(hrString.Substring(2), NumberStyles.AllowHexSpecifier)
                    : Convert.ToInt32(hrString);
            }
            else
            {
                hr = Convert.ToInt32(hrString);
            }
            return hr;
        }

        public static XElement ParseResponse(String xml, String tag = null)
        {
            XDocument doc = XDocument.Parse(xml);
            XElement root = doc.Root;
            XElement msg = root.Element("msg");
            if (msg == null) throw new OmniException("Invalid OmniEngine command response");

            XAttribute hrAttribute = msg.Attribute("hr");
            if (hrAttribute == null) throw new OmniException("OmniEngine command failed: 0x80004005.", 0x80004005);

            int hr = 0;
            String hrString = hrAttribute.Value;
            if (hrString.Length > 3)
            {
                hr = (hrString.Substring(0, 2) == "0x")
                    ? int.Parse(hrString.Substring(2), NumberStyles.AllowHexSpecifier)
                    : Convert.ToInt32(hrString);
            }
            else
            {
                hr = Convert.ToInt32(hrString);
            }

            if (hr < 0)
            {
                throw new OmniException(String.Format("OmniEngine command failed: 0x{0:X8}", hr), (uint)hr);
            }
            return (tag != null) ? msg.Element(tag) : null;
        }

        public String XmlCommand(UInt32 command, String request, UInt32 timeout=0)
        {
            Encoding ascii = Encoding.ASCII;

            byte[] bytesResponse = XmlCommandBytes(command, request, timeout);
            if ((bytesResponse == null) || (bytesResponse.Length == 0))
            {
                return "<peek><msg hr=\"0x80004005\"/></peek>";
            }
            return ascii.GetString(bytesResponse);
        }

        public byte[] XmlCommandBytes(UInt32 command, String request, UInt32 timeout=0)
        {
            Encoding unicode = Encoding.Unicode;    // Windows 16-bit Unicode
            Encoding utf32 = Encoding.UTF32;        // Linux 32-bit Unicode

            byte[] bytesRequest = (request != null) ? unicode.GetBytes(request) : null;
            if (engine.Platform.OS == HostPlatform.OperatingSystem.Linux)
            {
            byte[] bytesUnicode = (bytesRequest != null) ? Encoding.Convert(unicode, utf32, bytesRequest) : null;
                if (bytesUnicode != null )
                {
                    bytesRequest = bytesUnicode;
                }
            }

            Int32 count = 0;
            IntPtr response = IssueXmlCommand(this.connection, command, timeout, bytesRequest, out count);
            if ((count == 0) || (response == IntPtr.Zero))
            {
                return null;
            }
            // Strip off the trailing C string null.
            byte[] bytesResponse = new byte[count];
            Marshal.Copy(response, bytesResponse, 0, count);
            ReleaseResponse(this.connection, response);
            return bytesResponse;
        }
    }
}
