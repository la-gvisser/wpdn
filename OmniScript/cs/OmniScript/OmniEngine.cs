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
    using System.Collections.Generic;
    using System.IO;
    using System.Xml.Linq;
    using System.Text;

    /// <summary>
    /// OmniEngine object.
    /// </summary>
    /// <devnotes>Response of OmniScript Connect</devnotes>
    public class OmniEngine
    {
        private String Host;
        private uint Port;
        private uint Timeout;
        public HostPlatform Platform;
        public OmniApi Connection;
        public OmniLogger Logger;
        protected AnalysisModuleList Plugins;

        // private FilterList; // may want to cache the filter list.

        public OmniEngine(OmniLogger logger, String host, uint port, HostPlatform platform=null)
        {
            ulong protocol = (platform != null) ? platform.GetProtocol() : 0;
            this.Connection = new OmniApi(this, protocol);
            this.Host = !String.IsNullOrEmpty(host) ? host : OmniScript.DefaultHost;
            this.Port = (port > 0) ? port : OmniScript.DefaultPort;
            this.Timeout = 120 * 1000; // 2 minutes (120,000 milliseconds)
            this.Platform = platform;
            this.Logger = logger;
            this.Plugins = null;
        }

        public OmniEngine(OmniLogger logger=null)
            : this(logger, OmniScript.DefaultHost, OmniScript.DefaultPort)
        {
        }

        /// <summary>
        /// Add a filter the OmniEngine.
        /// </summary>
        public void AddFilter(FilterList filterList)
        {
            XElement items = new XElement("mutable-items",
                    new XAttribute("clsid", OmniScript.ClassNameIds["Filter"].ToString()));
            foreach (Filter filter in filterList)
            {
                XElement item = new XElement("item",
                    new XAttribute("type", "1"));
                item.Add(filter.Store());
                items.Add(item);
            }

            XDocument mi = new XDocument(new XElement("request", items));
            String request = mi.ToString(SaveOptions.DisableFormatting);
            String response = this.Connection.XmlCommand(OmniApi.OMNI_MODIFY_FILTERS, request);
            OmniApi.ParseResponse(response);
        }

        /// <summary>
        /// Add a filter the OmniEngine.
        /// </summary>
        public void AddFilter(Filter filter)
        {
            if (filter != null)
            {
                FilterList fl = new FilterList(null) { filter };
                this.AddFilter(fl);
            }
        }

        /// <summary>
        /// Save packets from a capture to file in the requested format.
        /// </summary>
//         public void ConvertCaptureFile(String source, String destination, String format)
//         {
//             this.Connection.ConvertPktFile(source, destination, format);
//         }

        /// <summary>
        /// Open the connection to the OmniEngine.
        /// </summary>
        public bool Connect(String auth, String domain, String account, String password)
        {
            String _auth = HostPlatform.GetAuth(this.Platform, auth);
            String _domain = (!String.IsNullOrEmpty(domain)) ? domain : "";
            String _account = (!String.IsNullOrEmpty(account)) ? account : "";
            String _password = (!String.IsNullOrEmpty(password)) ? password : "";

            // Reducing ", " to "," causes the tokenizer to collapse ",," to ",".
            String credentials = String.Join(", ", this.Host, this.Port.ToString(),
                _auth, _domain, _account, _password, "30");
            String response = this.Connection.CommandString(OmniApi.OMNI_CONNECT, credentials);
            int result = OmniApi.GetResponseCode(response);
            return (result == 0);   // Connected: 0x00000000, anything else is not connected.
        }

        /// <summary>
        /// Open the connection to the OmniEngine.
        /// </summary>
        public bool Connect(OmniScript.EngineCredentials credentials)
        {
            return this.Connect(credentials.Auth, credentials.Domain, credentials.Account, credentials.Password);
        }

        /// <summary>
        /// Open the connection to the OmniEngine.
        /// </summary>
        public bool Connect(String account, String password)
        {
            return this.Connect(null, null, account, password);
        }

        /// <summary>
        /// Open the connection to the OmniEngine.
        /// </summary>
        public bool Connect(String domain, String account, String password)
        {
            return this.Connect(null, domain, account, password);
        }

        /// <summary>
        /// Create an Capture from an OmniTemplate.
        /// </summary>
        public Capture CreateCapture(CaptureTemplate template)
        {
            try
            {
                // Ensure that a new Id is generated.
                //OmniId templateId = template.Id;
                //template.Id = null;
                String ct = template.ToString(this);
                //template.Id = templateId;

                String request = "<request>" + ct + "</request>";
                String xml = this.Connection.XmlCommand(
                    OmniApi.OMNI_CREATE_CAPTURE, request, this.Timeout);

                XElement prop = OmniApi.ParseResponse(xml, "prop");
                if ((prop != null) && !String.IsNullOrEmpty(prop.Value))
                {
                    Guid id = Guid.Parse(prop.Value);
                    return this.FindCapture(id);
                }
                return null;
            }
            catch (System.Exception)
            {
                return null;            	
            }
        }

        /// <summary>
        /// Delete a list of captures from the OmniEngine by Id.
        /// </summary>
        public void DeleteCapture(List<String> ids)
        {
            const String RootName = "captures";
            const String ItemName = "capture";

            if ((ids == null) || (ids.Count == 0)) return;

            XElement captures = new XElement(RootName);
            foreach (String id in ids)
            {
                if (!String.IsNullOrEmpty(id))
                {
                    captures.Add(new XElement(ItemName, new XAttribute("id", id)));
                }
            }
            if (captures.Element(ItemName) == null) return;

            XDocument doc = new XDocument(new XElement("request", captures));
            String request = doc.ToString(SaveOptions.DisableFormatting);
            String response = this.Connection.XmlCommand(
                OmniApi.OMNI_DELETE_CAPTURES, request, this.Timeout);
            OmniApi.ParseResponse(response);
        }

        /// <summary>
        /// Delete capture from the OmniEngine by Id.
        /// </summary>
        public void DeleteCapture(String id)
        {
            if (!String.IsNullOrEmpty(id))
            {
                this.DeleteCapture(new List<string>() { id });
            }
        }

        /// <summary>
        /// Delete an Capture from the OmniEngine.
        /// </summary>
        public void DeleteCapture(Capture capture)
        {
            if ((capture != null) && !OmniId.IsNullOrEmpty(capture.Id))
            {
                DeleteCapture(new List<string>() { capture.Id.ToString() });
            }
        }

        /// <summary>
        /// Delete a List of Capture from the OmniEngine.
        /// </summary>
        public void DeleteCapture(IEnumerable<Capture> captures)
        {
            List<String> ids = new List<String>();
            foreach (Capture capture in captures)
            {
                if ((capture != null) && !OmniId.IsNullOrEmpty(capture.Id))
                {
                    ids.Add(capture.Id.ToString());
                }
            }
            DeleteCapture(ids);
        }

        /// <summary>
        /// Delete a list of files from the OmniEngine's file system.
        /// </summary>
        public void DeleteFile(IEnumerable<String> names)
        {
            if (names == null) return;

            const String RootName = "files";
            const String ItemName = "file";

            XElement files = new XElement(RootName);
            foreach (String name in names)
            {
                if (!String.IsNullOrEmpty(name))
                {
                    files.Add(new XElement(ItemName, new XAttribute("name", name)));
                }
            }
            if (files.Element(ItemName) == null) return;

            XDocument doc = new XDocument(new XElement("request", files));
            String request = doc.ToString(SaveOptions.DisableFormatting);
            String response = this.Connection.XmlCommand(
                OmniApi.OMNI_DELETE_FILES, request, this.Timeout);
            OmniApi.ParseResponse(response);
        }

        /// <summary>
        /// Delete a file from the OmniEngine's file system.
        /// </summary>
        public void DeleteFile(String name)
        {
            if (!String.IsNullOrEmpty(name))
            {
                this.DeleteFile(new List<String>() { name });
            }
        }

        /// <summary>
        /// Delete a file from the OmniEngine's file system.
        /// </summary>
        public void DeleteFile(FileInformation info)
        {
            this.DeleteFile(new List<String>() { info.Name });
        }

        /// <summary>
        /// Delete a list of files from the OmniEngine's file system.
        /// </summary>
        public void DeleteFile(IEnumerable<FileInformation> infos)
        {
            if (infos == null) return;

            List<String> list = new List<String>();
            foreach (FileInformation info in infos)
            {
                if ((info != null) && !String.IsNullOrEmpty(info.Name))
                {
                    list.Add(info.Name);
                }
            }
            this.DeleteFile(list);
        }

        /// <summary>
        /// Delete filter from the OmniEngine by Id.
        /// </summary>
        public void DeleteFilter(IEnumerable<String> ids)
        {
            if (ids == null) return;
            
            XElement items = new XElement("mutable-items",
                new XAttribute("clsid", OmniScript.ClassNameIds["Filter"].ToString()));
            foreach (String id in ids)
            {
                items.Add( new XElement("item",
                           new XAttribute("type", "3"),
                           new XElement("filter",
                           new XAttribute("id", id))));
            }
            XDocument doc = new XDocument(new XElement("request", items));
            String request = doc.ToString(SaveOptions.DisableFormatting);
            String response = this.Connection.XmlCommand(
                OmniApi.OMNI_MODIFY_FILTERS, request, this.Timeout);
            OmniApi.ParseResponse(response);
        }

        /// <summary>
        /// Delete filter from the OmniEngine by Id.
        /// </summary>
        public void DeleteFilter(String id)
        {
            this.DeleteFilter(new List<String>() { id });
        }

        /// <summary>
        /// Delete an Filter from the OmniEngine.
        /// </summary>
        public void DeleteFilter(Filter filter)
        {
            if ((filter != null) && !OmniId.IsNullOrEmpty(filter.Id))
            {
                this.DeleteFilter(filter.Id.ToString());
            }
        }

        /// <summary>
        /// Delete a list of Filters from the OmniEngine.
        /// </summary>
        public void DeleteFilter(FilterList filters)
        {
            List<String> ids = new List<String>();
            foreach (var filter in filters)
            {
                ids.Add(filter.Id.ToString());
            }
            this.DeleteFilter(ids);
        }

        /// <summary>
        /// Disconnect from the engine.
        /// </summary>
        public void Disconnect()
        {
            this.Connection.Command(OmniApi.OMNI_DISCONNECT, null);
        }

        /// <summary>
        /// Find an adapter by identifier.
        /// </summary>
        public Adapter FindAdapterById(String id)
        {
            Adapter adapter = null;
            AdapterList list = this.GetAdapterList();
            if (list != null)
            {
                adapter = list.FindById(id);
            }
            return adapter;
        }

        /// <summary>
        /// Find an adapter by description/name.
        /// </summary>
        public Adapter FindAdapterByName(String name)
        {
            Adapter adapter = null;
            AdapterList list = this.GetAdapterList();
            if (list != null)
            {
                adapter = list.FindByName(name);
            }
            return adapter;
        }

        /// <summary>
        /// Find a capture by id.
        /// </summary>
        public Capture FindCapture(Guid id)
        {
            Capture capture = null;
            CaptureList list = this.GetCaptureList();
            if (list != null)
            {
                capture = list.Find(id);
            }
            return capture;
        }

        /// <summary>
        /// Find a capture by name.
        /// </summary>
        public Capture FindCapture(String name)
        {
            Capture capture = null;
            CaptureList list = this.GetCaptureList();
            if (list != null)
            {
                capture = list.Find(name);
            }
            return capture;
        }

        /// <summary>
        /// Find a filter by name.
        /// </summary>
        public Filter FindFilter(String name)
        {
            Filter filter = null;
            FilterList list = this.GetFilterList();
            if (list != null)
            {
                filter = list.Find(name);
            }
            return filter;
        }

        /// <summary>
        /// Find a capture by id.
        /// </summary>
        public Filter FindFilter(OmniId id)
        {
            Filter filter = null;
            FilterList list = this.GetFilterList();
            if (list != null)
            {
                filter = list.Find(id);
            }
            return filter;
        }

        /// <summary>
        /// Get the OmniEngine's list of adapters.
        /// </summary>
        public AdapterList GetAdapterList()
        {
            String response = this.Connection.XmlCommand(
                OmniApi.OMNI_GET_ADAPTER_LIST, null, this.Timeout);
            XElement adapterinfolist = OmniApi.ParseResponse(response, AdapterList.RootName);
            return new AdapterList(this, adapterinfolist);
        }

        /// <summary>
        /// Get the OmniEngine's list of alarms.
        /// </summary>
        public AlarmList GetAlarmList()
        {
            String response = this.Connection.XmlCommand(
                OmniApi.OMNI_GET_ALARMS, null, this.Timeout);
            XElement alarmlist = OmniApi.ParseResponse(response, AlarmList.RootName);
            return new AlarmList(this, alarmlist);
        }

        /// <summary>
        /// Get the OmniEngine's list of analysis modules (plugins).
        /// </summary>
        public AnalysisModuleList GetAnalysisModuleList(bool refresh=false)
        {
            if (this.Plugins == null || refresh)
            {
                String response = this.Connection.XmlCommand(
                    OmniApi.OMNI_GET_INSTALLED_PLUGINS, null, this.Timeout);
                XElement plugins = OmniApi.ParseResponse(response, AnalysisModuleList.RootName);
                this.Plugins = new AnalysisModuleList(this.Logger, plugins);
            }
            return this.Plugins;
        }

        /// <summary>
        /// Get the OmniEngine's list of captures.
        /// </summary>
        public CaptureList GetCaptureList()
        {
            String response = this.Connection.XmlCommand(
                OmniApi.OMNI_GET_CAPTURE_LIST, null, this.Timeout);
            XElement captures = OmniApi.ParseResponse(response, CaptureList.RootName);
            return new CaptureList(this, captures);
        }

        /// <summary>
        /// Get the Capture Template of a capture.
        /// </summary>
        public CaptureTemplate GetCaptureTemplate(OmniId id)
        {
            XElement prop = new XElement("prop",
                new XAttribute("name", "id"),
                new XAttribute("type", "8"),
                id.ToString());
            XDocument doc = new XDocument(new XElement("request", prop));
            String request = doc.ToString(SaveOptions.DisableFormatting);
            String response = this.Connection.XmlCommand(
                OmniApi.OMNI_GET_CAPTURE_OPTIONS, request, this.Timeout);
            XElement properties = OmniApi.ParseResponse(response, CaptureTemplate.RootName);
            return new CaptureTemplate(properties, this);
        }

        /// <summary>
        /// Get the OmniEngine's Event Log.
        /// first : index of the first item to retrieve, starting with 1.
        /// count : number of items to retrieve. If -1 then get all.
        /// </summary>
        public EventLog GetEventLog(uint first, uint count, OmniId context = null, String search=null)
        {
            XElement properties = new XElement("properties",
                OmniScript.Prop("Informational", 11, "-1"),
                OmniScript.Prop("Limit", 19, count.ToString()),
                OmniScript.Prop("Major", 11, "-1"),
                OmniScript.Prop("Messages", 11, "-1"),
                OmniScript.Prop("Minor", 11, "-1"),
                OmniScript.Prop("Offset", 19, first.ToString()),
                OmniScript.Prop("Severe", 11, "-1"));
            XDocument doc = new XDocument(new XElement("request", properties));
            String request = doc.ToString(SaveOptions.DisableFormatting);
            String response = this.Connection.XmlCommand(
                OmniApi.OMNI_GET_LOG_MSGS, request, this.Timeout);
            XElement counts = OmniApi.ParseResponse(response, "counts");
            XElement entrys = OmniApi.ParseResponse(response, "messages");
            return new EventLog(this, counts, entrys);
        }

        /// <summary>
        /// Copy a file from OmniEngine's files system to a local file.
        /// If the source is not fully qualified, then it is relative to the engine's
        /// data folder.
        /// </summary>
        public void GetFile(String source, String destination = null)
        {
            String name = (String.IsNullOrEmpty(destination)) ? source : destination;
            long chunkSize = 64 * 1024;
            long start = 0;
            long end = chunkSize;
            int result = 0;
            using (BinaryWriter stream = new BinaryWriter(File.OpenWrite(destination)))
            {
                while (result == 0)
                {
                    XElement fi = new XElement("get-file",
                        new XElement("file",
                            new XAttribute("name", source),
                            new XAttribute("start-offset", start.ToString()),
                            new XAttribute("end-offset", end.ToString())));
                    String request = fi.ToString(SaveOptions.DisableFormatting);
                    byte[] response = this.Connection.XmlCommandBytes(OmniApi.OMNI_GET_FILE, request, this.Timeout);
                    if (response.Length < 8)
                    {
                        break;
                    }
                    ReadStream data = new ReadStream(response);
                    result = data.ReadInt();
                    int chunk = data.ReadInt();
                    if (result == 0)
                    {
                        stream.Write(response, 8, chunk);
                        start += chunk;
                        end += chunk;
                        if ((chunk < chunkSize) || (chunk == 0)) break;
                    }
                }
            }
        }

        /// <summary>
        /// Get a list of Filter Information objects.
        /// </summary>
        public FileInformationList GetFileList(String path="", String mask="*", bool recursive=true, bool folders=false, bool fromData=true)
        {
            String pathType = (fromData) ? "relative-path" : "absolute-path";
            XElement getFileList = new XElement("request");
            getFileList.Add(new XElement("file-type",
                new XAttribute("mask", mask),
                new XAttribute(pathType, path),
                new XAttribute("recursive", (recursive) ? "1" : "0"),
                new XAttribute("include-dirs", (folders) ? "1" : "0")));
            String xml = getFileList.ToString(SaveOptions.DisableFormatting);
            String response = this.Connection.XmlCommand(
                OmniApi.OMNI_GET_FILE_LIST, xml, this.Timeout);
            XElement files = OmniApi.ParseResponse(response, FileInformationList.RootName);
            return new FileInformationList(this.Logger, files);
        }

        /// <summary>
        /// Get the OmniEngine's list of filters.
        /// </summary>
        public FilterList GetFilterList()
        {
            String response = this.Connection.XmlCommand(
                OmniApi.OMNI_GET_FILTERS, null, this.Timeout);
            XElement filters = OmniApi.ParseResponse(response, FilterList.RootName);
            return new FilterList(this.Logger, filters);
        }

        /// <summary>
        /// Get the OmniEngine's list of Forensic Files.
        /// </summary>
        public ForensicFileList GetForensicFileList()
        {
            OmniDataTable dataTable = new OmniDataTable();
            byte[] response = this.Connection.XmlCommandBytes(
                OmniApi.OMNI_GET_PACKETFILE_TABLE, null, this.Timeout);
            dataTable.Load(response);
            return new ForensicFileList(this.Logger, dataTable);
        }

        /// <summary>
        /// Get the OmniEngine's list of Forensic Searches.
        /// </summary>
        public ForensicSearchList GetForensicSearchList()
        {
            String response = this.Connection.XmlCommand(
                OmniApi.OMNI_GET_FILE_VIEW_LIST, null, this.Timeout);
            XElement views = OmniApi.ParseResponse(response, ForensicSearchList.RootName);
            return new ForensicSearchList(this.Logger, views);
        }

        /// <summary>
        /// Get the OmniEngine's list of Graph Templates.
        /// </summary>
        public GraphTemplateList GetGraphTemplateList()
        {
            String response = this.Connection.XmlCommand(
                OmniApi.OMNI_GET_GRAPH_TEMPLATES, null, this.Timeout);
            XElement views = OmniApi.ParseResponse(response, GraphTemplateList.RootName);
            return new GraphTemplateList(this.Logger, views);
        }

        /// <summary>
        /// Get the OmniEngine's status.
        /// </summary>
        public EngineStatus GetStatus()
        {
            String response = this.Connection.XmlCommand(
                OmniApi.OMNI_GET_STATUS, null, this.Timeout);
            XElement status = OmniApi.ParseResponse(response, EngineStatus.RootName);
            return new EngineStatus(this, status);
        }

        /// <summary>
        /// Get the OmniEngine's version.
        /// </summary>
        public String GetVersion()
        {
            String version = this.Connection.XmlCommand(
                OmniApi.OMNI_GET_VERSION, null, this.Timeout);
            return version;
        }

        /// <summary>
        /// Is there an open connection to the OmniEngine.
        /// </summary>
        public bool IsConnected()
        {
            String response = this.Connection.CommandString(OmniApi.OMNI_IS_CONNECTED);
            if (response == null)
            {
                return false;
            }
            int result = OmniApi.GetResponseCode(response); // HE_S_FALSE = 0x00000001
            return (result == 0);
        }

        /// <summary>
        /// Modify an existing Capture with a CaptureTemplate.
        /// </summary>
        public Capture ModifyCapture(CaptureTemplate template, Capture capture = null)
        {
            return ModifyCapture(template, (capture != null) ? capture.Id : template.Id);
        }

        public Capture ModifyCapture(CaptureTemplate template, OmniId captureId)
        {
            try
            {
                CaptureTemplate _template = template;
                _template.Id = captureId;

                // Ensure the template has a non-null Id.
                if (OmniId.IsNullOrEmpty(_template.Id))
                {
                    return null;
                }

                String ct = _template.ToString(this, true);
                String request = "<request>" + ct + "</request>";
                String xml = this.Connection.XmlCommand(
                    OmniApi.OMNI_SET_CAPTURE_OPTIONS, request, this.Timeout);
                OmniApi.ParseResponse(xml);
                return this.FindCapture(template.Id);
            }
            catch (System.Exception)
            {
                return null;
            }
        }

        /// <summary>
        /// Reset captures by a list of Ids as string.
        /// </summary>
        public void ResetCapture(List<String> ids)
        {
            if ((ids == null) || (ids.Count == 0)) return;

            XElement captures = OmniEngine.XmlCaptureList(ids);
            if (captures == null) return;

            XDocument doc = new XDocument(new XElement("request", captures));
            String request = doc.ToString(SaveOptions.DisableFormatting);
            String xml = this.Connection.XmlCommand(
                OmniApi.OMNI_STOP_CAPTURES, request, this.Timeout);
            OmniApi.ParseResponse(xml);
        }

        /// <summary>
        /// Reset a capture by Id.
        /// </summary>
        public void ResetCapture(String id)
        {
            String request = "<request><captures><capture id=" + id + " /></captures></request>";
            String response = this.Connection.XmlCommand(
                OmniApi.OMNI_CLEAR_CAPTURES, request, this.Timeout);
            OmniApi.ParseResponse(response);
        }

        /// <summary>
        /// Reset a capture by Id.
        /// </summary>
        public void ResetCapture(OmniId id)
        {
            ResetCapture(id.ToString());
        }

        /// <summary>
        /// Reset a list of Capture from the OmniEngine.
        /// </summary>
        public void ResetCapture(IEnumerable<Capture> captures)
        {
            List<String> ids = new List<String>();
            foreach (Capture capture in captures)
            {
                if ((capture != null) && !OmniId.IsNullOrEmpty(capture.Id))
                {
                    ids.Add(capture.Id.ToString());
                }
            }
            ResetCapture(ids);
        }

        /// <summary>
        /// Save Packets to file.
        /// </summary>
        public void SaveAllPackets(OmniId captureId, String filename)
        {
            XDocument doc = new XDocument(
                new XElement("request",
                    new XElement("attributes",
                        new XAttribute("capture-id", captureId.ToString()),
                        new XAttribute("filename", filename))));
            String request = doc.ToString(SaveOptions.DisableFormatting);
            String xml = this.Connection.XmlCommand(
                OmniApi.OMNI_SAVE_CAPTURE_FILE, request, this.Timeout);
            OmniApi.ParseResponse(xml);
        }

        /// <summary>
        /// Save Packets to file.
        /// </summary>
        public void SaveAllPackets(Capture capture, String filename)
        {
            this.SaveAllPackets(capture.Id, filename);
        }

        /// <summary>
        /// Copy a local file to OmniEngine's file system.
        /// </summary>
        public void SendFile(String source, String destination=null)
        {
            if (!File.Exists(source)) throw new OmniException("File does not exist.");
            FileInfo info = new FileInfo(source);

            String name = (String.IsNullOrEmpty(destination)) ? Path.GetFileName(source) : destination;
            int streamFormat = 1;
            int operation = OmniApi.WRITE_OP_CREATE;
            uint fileId = 0;
            {
                byte[] asciiBytes = Encoding.Convert(Encoding.Unicode, Encoding.ASCII, Encoding.Unicode.GetBytes(name));

                int requestLength = (3 * sizeof(int)) + asciiBytes.Length + sizeof(long); // int, int, int, string, long
                WriteStream request = new WriteStream(requestLength);
                request.Write(streamFormat);
                request.Write(operation);
                request.Write(asciiBytes.Length);
                request.Write(asciiBytes);
                request.Write(info.Length);
                byte[] response = this.Connection.Command(OmniApi.OMNI_SET_FILE, request, this.Timeout);
                if (response.Length == 8)
                {
                    ReadStream setResponse = new ReadStream(response);
                    uint xformat = setResponse.ReadUInt();
                    fileId = setResponse.ReadUInt();
                }
                else
                {
                    //OmniApi.ParseResponse(System.Text.Encoding.Unicode.GetString(response));
                    OmniApi.ParseResponse(System.Text.Encoding.ASCII.GetString(response));
                }
            }

            using (BinaryReader stream = new BinaryReader(File.Open(source, FileMode.Open)))
            {
                operation = OmniApi.WRITE_OP_WRITE;
                long chunkSize = 64 * 1024;
                while (stream.BaseStream.Position < info.Length)
                {
                    long position = stream.BaseStream.Position;
                    int chunk = (int)Math.Min(chunkSize, (info.Length - position));
                    byte[] data = stream.ReadBytes(chunk);
                    int requestLength = (3 * sizeof(int)) + (2 * sizeof(long)) + data.Length; // int, int, int, long, long, data.length
                    WriteStream request = new WriteStream(requestLength);
                    request.Write(streamFormat);
                    request.Write(operation);
                    request.Write(fileId);
                    request.Write(position);
                    request.Write(chunkSize);
                    request.Write(data);
                    byte[] response = this.Connection.Command(OmniApi.OMNI_SET_FILE, request, this.Timeout);
                    //OmniApi.ParseResponse(System.Text.Encoding.Unicode.GetString(response));
                    OmniApi.ParseResponse(System.Text.Encoding.ASCII.GetString(response));
                }
            }

            {
                operation = OmniApi.WRITE_OP_CLOSE;
                int requestLength = (3 * sizeof(int)); // int, int, int
                WriteStream request = new WriteStream(requestLength);
                request.Write(streamFormat);
                request.Write(operation);
                request.Write(fileId);
                byte[] response = this.Connection.Command(OmniApi.OMNI_SET_FILE, request, this.Timeout);
                //OmniApi.ParseResponse(System.Text.Encoding.Unicode.GetString(response));
                OmniApi.ParseResponse(System.Text.Encoding.ASCII.GetString(response));
            }
        }

        /// <summary>
        /// Send a message to a plugin.
        /// </summary>
        public String SendPluginMessage(OmniId plugin, OmniId capture, String text="", byte[] binary=null)
        {
            int pluginHeaderLength = 4 + 16 + 16 + 8;
            long textLength = 0;
            if (!String.IsNullOrEmpty(text))
            {
                textLength = text.Length * 2;
            }
            long binaryLength = 0;
            if (binary != null)
            {
                binaryLength = binary.Length;
            }
            long payloadLength = 8 + 8 + textLength + binaryLength;
            int requestLength = pluginHeaderLength + (int)payloadLength;
            WriteStream request = new WriteStream(requestLength);
            // Plugin Message Header
            request.Write(pluginHeaderLength);
            request.Write(capture.GetBytes());
            request.Write(plugin.GetBytes());
            request.Write(payloadLength);
            // Payload
            request.Write(textLength);
            request.Write(binaryLength);
            if (textLength > 0)
            {
                request.WriteUnicode(text);
            }
            if (binaryLength > 0)
            {
                request.Write(binary);
            }
            byte[] response = this.Connection.Command(OmniApi.OMNI_PLUGIN_MESSAGE, request, this.Timeout);
            ReadStream stream = new ReadStream(response);
            ulong headerCount = stream.ReadUInt();
            OmniId idContext = stream.ReadGuid();
            OmniId idPlugin = stream.ReadGuid();
            ulong outerCount = stream.ReadULong();
            ulong textCount = stream.ReadULong();
            ulong binaryCount = stream.ReadULong();
            String xml = stream.ReadString((int)textCount);
            return xml;
        }

        /// <summary>
        /// Start capturing by Id.
        /// </summary>
        public void StartCapture(List<String> ids)
        {
            if ((ids == null) || (ids.Count == 0)) return;

            XElement captures = OmniEngine.XmlCaptureList(ids);
            if (captures == null) return;

            XDocument doc = new XDocument(new XElement("request", captures));
            String request = doc.ToString(SaveOptions.DisableFormatting);
            String xml = this.Connection.XmlCommand(
                OmniApi.OMNI_START_CAPTURES, request, this.Timeout);
            OmniApi.ParseResponse(xml);
        }

        /// <summary>
        /// Start capturing by Id.
        /// </summary>
        public void StartCapture(String id)
        {
            this.StartCapture(new List<String>() { id });
        }

        public void StartCapture(OmniId id)
        {
            this.StartCapture(new List<String>() { id.ToString() });
        }

        /// <summary>
        /// Start a list of Capture from the OmniEngine.
        /// </summary>
        public void StartCapture(IEnumerable<Capture> captures)
        {
            List<String> ids = new List<String>();
            foreach (Capture capture in captures)
            {
                if ((capture != null) && !OmniId.IsNullOrEmpty(capture.Id))
                {
                    ids.Add(capture.Id.ToString());
                }
            }
            StartCapture(ids);
        }

        /// <summary>
        /// Stop capturing by a list of Ids as string.
        /// </summary>
        public void StopCapture(List<String> ids)
        {
            if ((ids == null) || (ids.Count == 0)) return;

            XElement captures = OmniEngine.XmlCaptureList(ids);
            if (captures == null) return;

            XDocument doc = new XDocument(new XElement("request", captures));
            String request = doc.ToString(SaveOptions.DisableFormatting);
            String xml = this.Connection.XmlCommand(
                OmniApi.OMNI_STOP_CAPTURES, request, this.Timeout);
            OmniApi.ParseResponse(xml);
        }

        /// <summary>
        /// Stop capturing by Id.
        /// </summary>
        public void StopCapture(String id)
        {
            this.StopCapture(new List<String>() { id });
        }

        public void StopCapture(OmniId id)
        {
            this.StopCapture(new List<String>() { id.ToString() });
        }

        /// <summary>
        /// Stop a list of Capture from the OmniEngine.
        /// </summary>
        public void StopCapture(IEnumerable<Capture> captures)
        {
            List<String> ids = new List<String>();
            foreach (Capture capture in captures)
            {
                if ((capture != null) && !OmniId.IsNullOrEmpty(capture.Id))
                {
                    ids.Add(capture.Id.ToString());
                }
            }
            StopCapture(ids);
        }

        private static XElement XmlCaptureList(List<String> ids)
        {
            const String itemName = "capture";

            XElement captures = new XElement("captures");
            foreach (String id in ids)
            {
                if (!String.IsNullOrEmpty(id))
                {
                    XElement capture = new XElement(itemName,
                        new XAttribute("id", id));
                    captures.Add(capture);
                }
            }
            return (captures.Element(itemName) != null) ? captures : null;
        }
    }
}