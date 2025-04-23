// =============================================================================
// <copyright file="CsTest.cs" company="LiveAction, Inc.">
//  Copyright (c) 2018-2021 LiveAction, Inc. All rights reserved.
//  Copyright (c) 2015-2018 Savvius, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace CsTest
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Threading;
    using System.Xml.Linq;
    using Savvius.Omni.OmniScript;
    using OmniPrinters;

    public class Options
    {
        public bool DisplayHelp;
        public String LogFileName;
        public OmniScript.EngineAddress Address { get; set; }
        public OmniScript.EngineCredentials Credentials { get; set; }

        public Options()
        {
            this.DisplayHelp = false;
            this.LogFileName = null;
            this.Address = new OmniScript.EngineAddress();
            this.Credentials = new OmniScript.EngineCredentials();
        }
    }

    public class Program
    {
        List<String> Messages;

        public Program()
        {
            this.Messages = new List<String>();
        }

        public String ValidFileName(String fileName)
        {
            foreach (char c in System.IO.Path.GetInvalidFileNameChars())
            {
                fileName = fileName.Replace(c, '_');
            }
            return fileName;
        }

        public Adapter GetDefaultAdapter()
        {
            Adapter adpt = null;
            if (engine == null) return null;
            AdapterList adapters = engine.GetAdapterList();
            if (adapters.Count == 0) return null;
            if (adapters.Count == 1) return adapters[0];
            if (engine.Platform.OS == HostPlatform.OperatingSystem.Windows)
            {
                adpt = adapters.FindByName("Local Area Connection");
            }
            else if (engine.Platform.OS == HostPlatform.OperatingSystem.Linux)
            {
                adpt = adapters.FindByName("eth0");
            }
            if (adpt == null)
            {
                foreach (Adapter adapter in adapters)
                {
                    if (adapter.Name.EndsWith("0"))
                    {
                        adpt = adapter;
                        break;
                    }
                }
            }
            return adpt;
        }

        public ulong DoBackup()
        {
            String testName = "Backup Test: ";
            ulong failures = 0;

            String pathFileAdapter = Path.Combine(this.pathFiles, "Dual-Layer-XML.xml");
            CaptureTemplate ct = new CaptureTemplate(pathFileAdapter);
//             XElement doc = new XElement("plugin");
//             foreach (PluginSettings settings in ct.Plugins.PluginList)
//             {
//                 String options = settings.Settings;
//                 string xml = "<OmniScript>" + options + "</OmniScript>";
//                 XElement elem = XElement.Parse(xml);
//                 //doc.Add(elem.Elements());
//                 foreach (XElement e in elem.Elements())
//                 {
//                     doc.Add(new XElement(e));
//                 }
//             }
            String pathResult = Path.Combine(this.pathTemp, "Dual-Layer-XML_exp.xml");
            ct.Store(pathResult, this.engine);

            try
            {
                Capture garyCapture1 = engine.FindCapture("Gary - Capture 1");
                if (garyCapture1 != null)
                {
                    CaptureTemplate template = garyCapture1.GetCaptureTemplate();
                    String filename = Path.Combine(this.pathTemp, ValidFileName(Path.ChangeExtension(template.Name, ".xml")));
                    template.Store(filename, engine);
                }
            }
            catch (OmniException e)
            {
                this.Messages.Add(testName + "First test failed: " + e.Message);
                failures++;
            }

            try
            {
                XElement noAttrib = new XElement("Test");
                XAttribute noValue = noAttrib.Attribute("Value");
                String value = (noValue != null) ? noValue.Value.ToString() : null;

                //CaptureTemplate restart1 = new CaptureTemplate(@"Restart-1.xml");
//                 if (restart1 != null)
//                 {
//                     OmniPrinters.PrintCaptureTemplate(restart1);
//                 }
            }
            catch (OmniException e)
            {
                this.Messages.Add(testName + "Second test failed: " + e.Message);
                failures++;
            }

            try
            {
                FilterList fl = new FilterList(null);
                fl.Load(Path.Combine(this.pathFiles, "filter-1.flt"), null);
            }
            catch (OmniException e)
            {
                this.Messages.Add(testName + "Third test failed: " + e.Message);
                failures++;
            }


            //if (false) {
            //    CaptureList captures = engine.GetCaptureList();
            //    foreach (Capture capture in captures)
            //    {
            //        CaptureTemplate template = capture.GetCaptureTemplate();
            //        String filename = @"C:\Temp\Backup\" + ValidFileName(template.Name + ".xml");
            //        template.Store(filename, engine);
            //        String xml = template.ToString(engine);
            //    }

            //    FilterList filters = engine.GetFilterList();
            //    foreach (Filter filter in filters)
            //    {
            //        String filename = @"C:\Temp\Backup\" + ValidFileName(filter.Name + ".xml");
            //        String xml = filter.ToString("X");
            //        using (StreamWriter outfile = new StreamWriter(filename))
            //        {
            //            outfile.Write(xml);
            //        }
            //    }
            //}

            return failures;
        }

        public ulong DoUnitTests()
        {
            this.log.WriteLine(Environment.NewLine + "Unit Tests");
            String testName = "Unit Tests: ";
            ulong failures = 0;

            {
                AppleTalkAddress empty = new AppleTalkAddress();
                if (empty.ToString() != "0.0") ++failures;
                this.log.WriteLine("E: 0.0");
                this.log.WriteLine("   " + empty.ToString());
                if (empty.ToString() != "0.0")
                {
                    this.Messages.Add(testName + "AppleTalkAddress formatting failure: ");
                    ++failures;
                }

                AppleTalkAddress zero = new AppleTalkAddress("0.0");
                this.log.WriteLine("E: 0.0");
                this.log.WriteLine("   " + zero.ToString());
                if (zero.ToString() != "0.0")
                {
                    this.Messages.Add(testName + "AppleTalkAddress formatting failure: ");
                    ++failures;
                }

                AppleTalkAddress numeric = new AppleTalkAddress("12.34");
                this.log.WriteLine("E: 12.34");
                this.log.WriteLine("   " + numeric.ToString());
                if (numeric.ToString() != "12.34")
                {
                    this.Messages.Add(testName + "AppleTalkAddress formatting failure: ");
                    ++failures;
                }

                AppleTalkAddress masked = new AppleTalkAddress("246.*");
                this.log.WriteLine("E: 246.*");
                this.log.WriteLine("   " + masked.ToString());
                if (masked.ToString() != "246.*")
                {
                    this.Messages.Add(testName + "AppleTalkAddress formatting failure: ");
                    ++failures;
                }

                AppleTalkAddressList list = new AppleTalkAddressList("1.2 3.4 200.*");
                this.log.WriteLine("E: 1.2 3.4 200.*");
                this.log.WriteLine("   " + list.ToString());
                if (list.ToString() != "1.2 3.4 200.*")
                {
                    this.Messages.Add(testName + "AppleTalkAddress formatting failure: ");
                    ++failures;
                }

                FilterList fl = new FilterList(null);
                fl.Load(Path.Combine(this.pathFiles, "filter-apt.xml"), null);
                OmniPrinters.PrintFilterList(fl, 0);
                XElement node = new XElement("doc");
                fl.Store(node);
                node.Save(Path.Combine(this.pathTemp, "filter-apt_out.xml"));
            }

            {
                EthernetAddress empty = new EthernetAddress();
                this.log.WriteLine("E: 00:00:00:00:00:00");
                this.log.WriteLine("   " + empty.ToString());
                if (empty.ToString() != "00:00:00:00:00:00")
                {
                    this.Messages.Add(testName + "EthernetAddress formatting failure: ");
                    ++failures;
                }

                EthernetAddress zero = new EthernetAddress("00:00:00:00:00:00");
                this.log.WriteLine("E: 00:00:00:00:00:00");
                this.log.WriteLine("   " + zero.ToString());
                if (zero.ToString() != "00:00:00:00:00:00")
                {
                    ++failures;
                }
                
                EthernetAddress numeric = new EthernetAddress("12:34:56:78:9A:BC");
                this.log.WriteLine("E: 12:34:56:78:9A:BC");
                this.log.WriteLine("   " + numeric.ToString());
                if (numeric.ToString() != "12:34:56:78:9A:BC")
                {
                    this.Messages.Add(testName + "EthernetAddress formatting failure: ");
                    ++failures;
                }
                
                EthernetAddress masked = new EthernetAddress("EF:CD:AB:89:*:*");
                this.log.WriteLine("E: EF:CD:AB:89:*:*");
                this.log.WriteLine("   " + masked.ToString());
                if (masked.ToString() != "EF:CD:AB:89:*:*")
                {
                    this.Messages.Add(testName + "EthernetAddress formatting failure: ");
                    ++failures;
                }
                
                EthernetAddressList list = new EthernetAddressList("01:02:03:04:05:06 11:22:33:44:*:*");
                this.log.WriteLine("E: 01:02:03:04:05:06 11:22:33:44:*:*");
                this.log.WriteLine("   " + list.ToString());
                if (list.ToString() != "01:02:03:04:05:06 11:22:33:44:*:*")
                {
                    this.Messages.Add(testName + "EthernetAddress formatting failure: ");
                    ++failures;
                }

                FilterList fl = new FilterList(null);
                fl.Load(Path.Combine(this.pathFiles, "filter-eth.xml"), null);
                OmniPrinters.PrintFilterList(fl, 0);
                XElement node = new XElement("doc");
                fl.Store(node);
                node.Save(Path.Combine(this.pathTemp, "filter-eth_out.xml"));
            }

            {
                IPv4Address empty = new IPv4Address();
                this.log.WriteLine("E: 0.0.0.0");
                this.log.WriteLine("   " + empty.ToString());
                if (empty.ToString() != "0.0.0.0")
                {
                    this.Messages.Add(testName + "IPv4Address formatting failure: ");
                    ++failures;
                }

                IPv4Address zero = new IPv4Address("0.0.0.0");
                this.log.WriteLine("E: 0.0.0.0");
                this.log.WriteLine("   " + zero.ToString());
                if (zero.ToString() != "0.0.0.0")
                {
                    this.Messages.Add(testName + "IPv4Address formatting failure: ");
                    ++failures;
                }
                
                IPv4Address numeric = new IPv4Address("123.45.67.89");
                this.log.WriteLine("E: 123.45.67.89");
                this.log.WriteLine("   " + numeric.ToString());
                if (numeric.ToString() != "123.45.67.89")
                {
                    this.Messages.Add(testName + "IPv4Address formatting failure: ");
                    ++failures;
                }
                
                IPv4Address masked = new IPv4Address("250.240.230.*");
                this.log.WriteLine("E: 250.240.230.0/24");
                this.log.WriteLine("   " + masked.ToString());
                if (masked.ToString() != "250.240.230.0/24")
                {
                    this.Messages.Add(testName + "IPv4Address formatting failure: ");
                    ++failures;
                }
                
                IPv4Address cidr = new IPv4Address("251.241.0.0/16");
                this.log.WriteLine("E: 251.241.0.0/16");
                this.log.WriteLine("   " + cidr.ToString());
                if (cidr.ToString() != "251.241.0.0/16")
                {
                    this.Messages.Add(testName + "IPv4Address formatting failure: ");
                    ++failures;
                }
                
                IPv4AddressList list = new IPv4AddressList("1.2.3.4 2.4.6.0/24");
                this.log.WriteLine("E: 1.2.3.4 2.4.6.0/24");
                this.log.WriteLine("   " + list.ToString());
                if (list.ToString() != "1.2.3.4 2.4.6.0/24")
                {
                    this.Messages.Add(testName + "IPv4Address formatting failure: ");
                    ++failures;
                }

                FilterList fl = new FilterList(null);
                fl.Load(Path.Combine(this.pathFiles, "filter-ip4.xml"), null);
                OmniPrinters.PrintFilterList(fl, 0);
                XElement node = new XElement("doc");
                fl.Store(node);
                node.Save(Path.Combine(this.pathTemp, "filter-ip4_out.xml"));
            }

            {
                IPv6Address empty = new IPv6Address();
                this.log.WriteLine("E: 0000:0000:0000:0000:0000:0000:0000:0000");
                this.log.WriteLine("   " + empty.ToString());
                if (empty.ToString() != "0000:0000:0000:0000:0000:0000:0000:0000")
                {
                    this.Messages.Add(testName + "IPv6Address formatting failure: ");
                    ++failures;
                }

                IPv6Address zero = new IPv6Address("0:0:0:0:0:0:0:0");
                this.log.WriteLine("E: 0000:0000:0000:0000:0000:0000:0000:0000");
                this.log.WriteLine("   " + zero.ToString());
                if (zero.ToString() != "0000:0000:0000:0000:0000:0000:0000:0000")
                {
                    this.Messages.Add(testName + "IPv6Address formatting failure: ");
                    ++failures;
                }
                
                IPv6Address numeric = new IPv6Address("0123:4567:89AB:CDEF:1122:4466:88AA:CCEE");
                this.log.WriteLine("E: 0123:4567:89AB:CDEF:1122:4466:88AA:CCEE");
                this.log.WriteLine("   " + numeric.ToString());
                if (numeric.ToString() != "0123:4567:89AB:CDEF:1122:4466:88AA:CCEE")
                {
                    this.Messages.Add(testName + "IPv6Address formatting failure: ");
                    ++failures;
                }
                
                IPv6Address masked = new IPv6Address("1234:5678:9ABC:DEF0:1122:3344:*:*");
                this.log.WriteLine("E: 1234:5678:9ABC:DEF0:1122:3344:*:*");
                this.log.WriteLine("   " + masked.ToString());
                if (masked.ToString() != "1234:5678:9ABC:DEF0:1122:3344:*:*")
                {
                    this.Messages.Add(testName + "IPv6Address formatting failure: ");
                    ++failures;
                }
                
                IPv6AddressList list = new IPv6AddressList("01:02:03:04:05:06:07:08 11:22:33:44:55:66:*:*");
                this.log.WriteLine("E: 0001:0002:0003:0004:0005:0006:0007:0008 0011:0022:0033:0044:0055:0066:*:*");
                this.log.WriteLine("   " + list.ToString());
                if (list.ToString() != "0001:0002:0003:0004:0005:0006:0007:0008 0011:0022:0033:0044:0055:0066:*:*")
                {
                    this.Messages.Add(testName + "IPv6Address formatting failure: ");
                    ++failures;
                }

                FilterList fl = new FilterList(null);
                fl.Load(Path.Combine(this.pathFiles, "filter-ip6.xml"), null);
                OmniPrinters.PrintFilterList(fl, 0);
                XElement node = new XElement("doc");
                fl.Store(node);
                node.Save(Path.Combine(this.pathTemp, "filter-ip6_out.xml"));
            }

            {
                WirelessAddress empty = new WirelessAddress();
                this.log.WriteLine("E: 00:00:00:00:00:00");
                this.log.WriteLine("   " + empty.ToString());
                if (empty.ToString() != "00:00:00:00:00:00")
                {
                    this.Messages.Add(testName + "WirelessAddress formatting failure: ");
                    ++failures;
                }

                WirelessAddress zero = new WirelessAddress("00:00:00:00:00:00");
                this.log.WriteLine("E: 00:00:00:00:00:00");
                this.log.WriteLine("   " + zero.ToString());
                if (zero.ToString() != "00:00:00:00:00:00")
                {
                    this.Messages.Add(testName + "WirelessAddress formatting failure: ");
                    ++failures;
                }
                
                WirelessAddress numeric = new WirelessAddress("12:34:56:78:9A:BC");
                this.log.WriteLine("E: 12:34:56:78:9A:BC");
                this.log.WriteLine("   " + numeric.ToString());
                if (numeric.ToString() != "12:34:56:78:9A:BC")
                {
                    this.Messages.Add(testName + "WirelessAddress formatting failure: ");
                    ++failures;
                }
                
                WirelessAddress masked = new WirelessAddress("FE:DC:BA:*:*:*");
                this.log.WriteLine("E: FE:DC:BA:*:*:*");
                this.log.WriteLine("   " + masked.ToString());
                if (masked.ToString() != "FE:DC:BA:*:*:*")
                {
                    this.Messages.Add(testName + "WirelessAddress formatting failure: ");
                    ++failures;
                }
                
                WirelessAddressList list = new WirelessAddressList("01:02:03:04:05:06 11:22:33:44:*:*");
                this.log.WriteLine("E: 01:02:03:04:05:06 11:22:33:44:*:*");
                this.log.WriteLine("   " + list.ToString());
                if (list.ToString() != "01:02:03:04:05:06 11:22:33:44:*:*")
                {
                    this.Messages.Add(testName + "WirelessAddress formatting failure: ");
                    ++failures;
                }

                FilterList fl = new FilterList(null);
                fl.Load(Path.Combine(this.pathFiles, "filter-wl.xml"), null);
                OmniPrinters.PrintFilterList(fl, 0);
                XElement node = new XElement("doc");
                fl.Store(node);
                node.Save(Path.Combine(this.pathTemp, "filter-wl_out.xml"));
            }

            {
                OmniPort empty = new IPPort();
                this.log.WriteLine("E: 0");
                this.log.WriteLine("   " + empty.ToString());
                if (empty.ToString() != "0")
                {
                    this.Messages.Add(testName + "OmniPort formatting failure: ");
                    ++failures;
                }

                OmniPort zero = new IPPort();
                this.log.WriteLine("E: 0");
                this.log.WriteLine("   " + zero.ToString());
                if (zero.ToString() != "0")
                {
                    this.Messages.Add(testName + "OmniPort formatting failure: ");
                    ++failures;
                }

                IPPort numeric = new IPPort("12");
                this.log.WriteLine("E: 12");
                this.log.WriteLine("   " + numeric.ToString());
                if (numeric.ToString() != "12")
                {
                    this.Messages.Add(testName + "IPPort formatting failure: ");
                    ++failures;
                }

                IPPort range = new IPPort("1234-5678");
                this.log.WriteLine("E: 1234-5678");
                this.log.WriteLine("   " + range.ToString());
                if (range.ToString() != "1234-5678")
                {
                    this.Messages.Add(testName + "IPPort formatting failure: ");
                    ++failures;
                }

                IPPortList list = new IPPortList("80 8080 60000-65535");
                this.log.WriteLine("E: 80 8080 60000-65535");
                this.log.WriteLine("   " + list.ToString());
                if (list.ToString() != "80 8080 60000-65535")
                {
                    this.Messages.Add(testName + "IPPortList formatting failure: ");
                    ++failures;
                }

                FilterList fl = new FilterList(null);
                fl.Load(Path.Combine(this.pathFiles, "filter-port.xml"), null);
                OmniPrinters.PrintFilterList(fl, 0);
                XElement node = new XElement("doc");
                fl.Store(node);
                node.Save(Path.Combine(this.pathTemp, "filter-port_out.xml"));
            }

            {
                FilterList fl = new FilterList(null);
                fl.Load(Path.Combine(this.pathFiles, "filter-app.xml"), null);
                OmniPrinters.PrintFilterList(fl, 0);
                XElement node = new XElement("doc");
                fl.Store(node);
                node.Save(this.pathTemp + "filter-app_out.xml");
            }

            {
                PeekTime pt1 = new PeekTime();
                Thread.Sleep(2000);
                PeekTime pt2 = new PeekTime();
                ulong df = pt2 - pt1;
                if (df > (10L * 1000000000L))
                {
                    this.log.WriteLine("*** Check PeekTime subtraction.");
                }
            }

            /*
            {
                OmniScript.IndexNameMap idNames = OmniScript.GetProtocolIdNames();
                String filename = @"C:\Temp\Backup\idNames.txt";
                using (StreamWriter outfile = new StreamWriter(filename))
                {
                    var sorted = from entry in idNames orderby entry.Value ascending select entry;
                    foreach (KeyValuePair<uint, String> item in sorted)
                    {
                        outfile.WriteLine(item.Key.ToString("D5") + " : " + item.Value.ToString());
                    }
                }
            }
            */
            return failures;
        }

        public ulong DoGets()
        {
            this.log.WriteLine(Environment.NewLine + "Test Gets");
            String testName = "Gets Tests: ";
            ulong failures = 0;
            try
            {
                EngineStatus status = this.engine.GetStatus();
                OmniPrinters.PrintEngineStatus(status);
                this.log.WriteLine("Refresh Engine Status.");
                status.Refresh();
                OmniPrinters.PrintEngineStatus(status);

                AdapterList adapterList = this.engine.GetAdapterList();
                OmniPrinters.PrintAdapterList(adapterList);
                if (adapterList.Count > 0)
                {
                    Adapter adapter = engine.FindAdapterByName(adapterList[0].Name);
                    if (adapter == null)
                    {
                        this.log.WriteLine("*** Failed to find adapter by name.");
                    }
                    adapter = engine.FindAdapterById(adapterList[0].Id);
                    if (adapter == null)
                    {
                        this.log.WriteLine("*** Failed to find adapter by Id.");
                    }
                }

                AlarmList alarmList = engine.GetAlarmList();
                OmniPrinters.PrintAlarmList(alarmList);

                AnalysisModuleList pluginList = engine.GetAnalysisModuleList();
                OmniPrinters.PrintAnalysisModuleList(pluginList);

                CaptureList captureList = this.engine.GetCaptureList();
                OmniPrinters.PrintCaptureList(captureList);

                EventLog eventLog = this.engine.GetEventLog(0, 0);
                if (eventLog.Count > 20)
                {
                    eventLog = this.engine.GetEventLog(10, 5);
                    if (eventLog.EntryList.Count != 5)
                    {
                        this.log.WriteLine("*** Failed to get 5 Entry Log entries.");
                    }
                    OmniPrinters.PrintEventLog(eventLog);
                    eventLog.GetPrevious(5);
                    if (eventLog.EntryList.Count != 10)
                    {
                        this.log.WriteLine("*** Failed to get previous 5 Entry Log entries.");
                    }
                    OmniPrinters.PrintEventLog(eventLog);
                    eventLog.GetNext(5);
                    if (eventLog.EntryList.Count != 15)
                    {
                        this.log.WriteLine("*** Failed to get next 5 Entry Log entries.");
                    }
                    OmniPrinters.PrintEventLog(eventLog);
                }
                eventLog.Refresh();
                OmniPrinters.PrintEventLog(eventLog);

                if (captureList.Count > 0)
                {
                    Capture capture = engine.FindCapture(captureList[0].Name);
                    if (capture == null)
                    {
                        this.log.WriteLine("*** Failed to find capture by Name.");
                    }
                    capture = engine.FindCapture(captureList[0].Id);
                    if (capture == null)
                    {
                        this.log.WriteLine("*** Failed to find capture by Id.");
                    }
                }
                CaptureTemplateList templateList = new CaptureTemplateList(null);
                foreach (Capture capture in captureList)
                {
                    CaptureTemplate ct = capture.GetCaptureTemplate();
                    templateList.Add(ct);
                }
                templateList.SortByName();
                OmniPrinters.PrintCaptureTemplateList(templateList);

                FilterList filterList = this.engine.GetFilterList();
                OmniPrinters.PrintFilterList(filterList);
                if (filterList.Count > 0)
                {
                    Filter filter = engine.FindFilter(filterList[0].Name);
                    if (filter == null)
                    {
                        this.log.WriteLine("*** Failed to find filter by Name.");
                    }
                    filter = engine.FindFilter(filterList[0].Id);
                    if (filter == null)
                    {
                        this.log.WriteLine("*** Failed to find filter by Id.");
                    }
                }

                ForensicFileList forensicFileList = engine.GetForensicFileList();
                OmniPrinters.PrintForensicFileList(forensicFileList);

                ForensicSearchList forensicSearchList = engine.GetForensicSearchList();
                OmniPrinters.PrintForensicSearchList(forensicSearchList);

                GraphTemplateList graphTemplateList = engine.GetGraphTemplateList();
                OmniPrinters.PrintGraphTemplateList(graphTemplateList);
            }
            catch (OmniException e)
            {
                this.Messages.Add(testName + "failure: " + e.Message);
                failures++;
            }
            return failures;
        }

        public ulong DoFilters(String filterName)
        {
            this.log.WriteLine(Environment.NewLine + "Test Filters");
            String testName = "Filters Tests: ";
            ulong failures = 0;

            List<String> filenames = new List<string>() {"filters.flt", "filters.xml"};
            foreach (var filename in filenames)
            {
                try
                {
                    FilterList fl = this.engine.GetFilterList();
                    if (fl.Count > 0 )
                    {
                        // Console.WriteLine("Delete Filter");
                        // Console.ReadLine();
                        String name = fl[0].Name;
                        this.engine.DeleteFilter(fl[0]);
                    }

                    this.engine.DeleteFilter(this.engine.GetFilterList());

                    String filepath = Path.Combine(this.pathFiles, filename);
                    FilterList filterList = OmniScript.ReadFilterFile(filepath);
                    OmniPrinters.PrintFilterList(filterList);
                    if (true)
                    {
                        foreach (var filter in filterList)
                        {
                            XElement items = new XElement("mutable-items",
                                new XAttribute("clsid", OmniScript.ClassNameIds["Filter"].ToString()));
                            XElement item = new XElement("item", new XAttribute("type", "1"));
                            item.Add(filter.Store());
                            items.Add(item);
                            this.log.WriteLine(items.ToString());
                        }
                    }
                    try
                    {
                        engine.AddFilter(filterList);
                    }
                    catch (OmniException e2)
                    {
                        this.Messages.Add(testName + "Failure: " + e2.Message);
                        this.Messages.Add(testName + "failed to Add Filter List from file: " + filename);
                        failures++;
                        foreach (var filter in filterList)
                        {
                            try
                            {
                                this.engine.AddFilter(filter);
                            }
                            catch (OmniException e3)
                            {
                                this.Messages.Add(testName + "Failure: " + e3.Message);
                                this.Messages.Add(testName + "failed to Add Filter List from file: " + filter.Name);

                                if (true)
                                {
                                    XElement items = new XElement("mutable-items",
                                        new XAttribute("clsid", OmniScript.ClassNameIds["Filter"].ToString()));
                                    XElement item = new XElement("item", new XAttribute("type", "1"));
                                    item.Add(filter.Store());
                                    items.Add(item);
                                    this.log.WriteLine(items.ToString());
                                }
                            }
                        }
                    }
                }
                catch (OmniException e1)
                {
                    this.Messages.Add(testName + "failure: " + e1.Message);
                    failures++;
                }
            }

            try
            {
                FilterList fl = this.engine.GetFilterList();
                Filter fli = this.engine.FindFilter(filterName);
                if (fli != null)
                {
                    this.engine.DeleteFilter(fli);
                }

                String simpleName = "C#-Simple";
                String addressName = "C#-Address";
                String wildcardName = "C#-Wildcard";
                String vlanmplsName = "C#-VLAN-MPLS";
                String complexName = "C#-Complex";
                String[] filterNames = new String[] {simpleName, addressName, wildcardName, vlanmplsName, complexName};

                // String emailCapture = "{A090DCAD-ACA7-4734-9F1C-FA7DD153F501}";

                foreach (String name in filterNames)
                {
                    Filter filter = this.engine.FindFilter(name);
                    while (filter != null)
                    {
                        this.engine.DeleteFilter(filter);
                        filter = this.engine.FindFilter(name);
                    }
                }

                Filter simple = new Filter(this.engine.Logger, simpleName);
                {
                    simple.Comment = "C# created simple filter.";
                    AddressFilterNode addrNode = new AddressFilterNode();
                    addrNode.Address1 = new IPv4Address("1.2.3.4");
                    addrNode.Address2 = new IPv4Address("2.4.6.8");
                    addrNode.Accept1To2 = true;
                    addrNode.Accept2To1 = true;
                    simple.Criteria = addrNode;
                    this.engine.AddFilter(simple);
                    OmniPrinters.PrintFilter(simple);
                }

                Filter address = new Filter(this.engine.Logger, addressName);
                {
                    address.Comment = "C# created address filter.";
                    AddressFilterNode addrNode = new AddressFilterNode();
                    addrNode.Address1 = new EthernetAddress("00:50:56:97:3a:6c");
                    addrNode.Address2 = new EthernetAddress("00:50:56:97:4b:7d");
                    addrNode.Accept1To2 = true;
                    addrNode.Accept2To1 = true;
                    address.Criteria = addrNode;
                    engine.AddFilter(address);
                    OmniPrinters.PrintFilter(address);
                }

                Filter wildcard = new Filter(this.engine.Logger, wildcardName);
                {
                    wildcard.Comment = "C# created wildcard filter.";
                    AddressFilterNode addrNode = new AddressFilterNode();
                    addrNode.Address1 = new EthernetAddress("00:14:2F:*:*:*");
                    addrNode.Accept1To2 = true;
                    addrNode.Accept2To1 = true;
                    wildcard.Criteria = addrNode;
                    engine.AddFilter(wildcard);
                    OmniPrinters.PrintFilter(wildcard);
                }

                Filter vlanmpls = new Filter(this.engine.Logger, vlanmplsName);
                {
                    vlanmpls.Comment = "C# created vlan-mpls filter.";
                    VlanFilterNode vlanmplsNode = new VlanFilterNode();
                    vlanmplsNode.Ids.Add("80");
                    vlanmplsNode.Ids.Add("100-110");
                    vlanmplsNode.Labels.Add("200-220");
                    vlanmplsNode.Labels.Add("1024");
                    vlanmpls.Criteria = vlanmplsNode;
                    engine.AddFilter(vlanmpls);
                    OmniPrinters.PrintFilter(vlanmpls);
                }

                Filter complex = new Filter(this.engine.Logger, complexName);
                {
                    complex.Comment = "C# created complex filter.";
                    AddressFilterNode addrNode = new AddressFilterNode();
                    addrNode.Address1 = new IPv4Address("1.2.3.4");
                    addrNode.Accept1To2 = true;
                    addrNode.Accept2To1 = true;

                    ProtocolFilterNode protocolNode = new ProtocolFilterNode();
                    protocolNode.SetProtocol("HTTP");

                    PortFilterNode portNode = new PortFilterNode();
                    portNode.Port1 = new IPPort(80);

                    addrNode.AndNode = protocolNode;
                    addrNode.OrNode = portNode;

                    complex.Criteria = addrNode;
                    engine.AddFilter(complex);
                    OmniPrinters.PrintFilter(complex);
                }
            }
            catch (OmniException e)
            {
                this.Messages.Add(testName + "Failure: " + e.Message);
                failures++;
            }
            return failures;
        }

        public ulong DoFileOps(String fileName)
        {
            this.log.WriteLine(Environment.NewLine + "Test File Ops");
            String testName = "File Ops Tests: ";
            ulong failures = 0;
            try
            {
                FileInformationList list = engine.GetFileList();
                OmniPrinters.PrintFileInformationList(list);
                foreach (FileInformation info in list)
                {
                    if (info.Name == fileName)
                    {
                        engine.DeleteFile(info);
                    }
                }
                engine.SendFile(Path.Combine(this.pathFiles, fileName));
            }
            catch (OmniException e)
            {
                this.Messages.Add(testName + "Failure: " + e.Message);
                failures++;
            }
            catch (SystemException e)
            {
                this.Messages.Add(testName + "Failure: " + e.Message);
                failures++;
            }

            try
            {
                String localName = String.Format("{0}{1}{2}",
                    Path.GetFileNameWithoutExtension(fileName),
                    ".0",
                    Path.GetExtension(fileName));
                if (File.Exists(localName))
                {
                    File.Delete(localName);
                }
                engine.GetFile(fileName, localName);
            }
            catch (OmniException e)
            {
                this.Messages.Add(testName + "Failure: " + e.Message);
                failures++;
            }
            catch (SystemException e)
            {
                this.Messages.Add(testName + "Failure: " + e.Message);
                failures++;
            }
            return failures;
        }

        public ulong DoDeleteAll()
        {
            this.log.WriteLine(Environment.NewLine + "Test Delete All");
            String testName = "Delete All Tests: ";
            ulong failures = 0;
            try
            {
                CaptureList captures = engine.GetCaptureList();
                engine.DeleteCapture(captures);
                ForensicSearchList searches = engine.GetForensicSearchList();
                // engine.DeleteForensicSearch(searches)
                ForensicFileList files = engine.GetForensicFileList();
                // engine.DeleteForensicFile(files);

                CaptureList capturesTemp = engine.GetCaptureList();
                if (capturesTemp.Count() > 0)
                {
                    this.log.WriteLine("*** Failed to delete all captures.");
                }
            }
            catch (OmniException e)
            {
                this.Messages.Add(testName + "Failure: " + e.Message);
                failures++;
            }
            return failures;
        }

        public ulong DoCaptureCreate(String nameCapture)
        {
            this.log.WriteLine(Environment.NewLine + "Test Capture Create");
            String testName = "Capture Create Tests: ";
            ulong failures = 0;
            try
            {
                Capture capture = engine.FindCapture(nameCapture);
                while (capture != null)
                {
                    engine.DeleteCapture(capture);
                    capture = engine.FindCapture(nameCapture);
                }

                Capture newCapture = null;
                {
                    Adapter adapter = GetDefaultAdapter();
                    CaptureTemplate template = new CaptureTemplate();
                    if (adapter != null)
                    {
                        template.SetAdapter(adapter);
                    }
                    template.General.Name = nameCapture;
                    template.General.BufferSize = 10 * OmniScript.bytesInAMegabyte;
                    template.General.Comment = "C# generated capture.";
                    template.General.Directory = Path.Combine(this.pathTemp, "Capture Files");
                    template.General.FilePattern = nameCapture + '-';
                    template.General.FileSize = 128 * OmniScript.bytesInAMegabyte;
                    template.General.KeepLastFilesCount = 5;
                    template.General.MaxFileAge = OmniScript.secondsInAHour;
                    template.General.MaxTotalFileSize = OmniScript.bytesInAGigabyte;
                    template.General.SliceLength = 256;
                    template.General.TapTimestamps = 0;
                    template.General.OptionCaptureToDisk = true;
                    template.General.OptionContinuousCapture = true;
                    template.General.OptionDeduplicate = true;
                    template.General.OptionFileAge = true;
                    template.General.OptionKeepLastFiles = true;
                    template.General.OptionPriorityCtd = true;
                    template.General.OptionSaveAsTemplate = true;
                    template.General.OptionSlicing = true;
                    template.General.OptionStartCapture = false;
                    template.General.OptionTimelineAppStats = true;
                    template.General.OptionTimelineStats = true;
                    template.General.OptionTimelineTopStats = true;
                    template.General.OptionTimelineVoipStats = true;
                    template.General.OptionTotalFileSize = true;
                    template.SetAll(false);
                    template.Filter.Mode = CaptureTemplate.FilterSettings.Modes.AcceptMatchingAny;
                    template.Filter.Filters.Add("HTTP");

                    string xml = template.ToString(engine);
                    this.log.WriteLine(Environment.NewLine + Environment.NewLine + xml + Environment.NewLine + Environment.NewLine);

                    newCapture = engine.CreateCapture(template);
                    OmniPrinters.PrintCapture(newCapture);
                }
                if (newCapture != null)
                {
                    newCapture.Start();
                    // while (newCapture.PacketsFiltered < 1000)
                    while ((newCapture.PacketsFiltered < 10) && (newCapture.PacketsReceived < 10))
                    {
                        newCapture.Refresh();
                    }
                    newCapture.Stop();
                    OmniPrinters.PrintCapture(newCapture);

                    {
                        CaptureTemplate updateTemplate = newCapture.GetCaptureTemplate();
                        OmniPrinters.PrintCaptureTemplate(updateTemplate);
                        updateTemplate.SetAll();
                        updateTemplate.Plugins.Set(engine.GetAnalysisModuleList());
                        updateTemplate.Filter.Filters.Add("FTP");
                        newCapture.Modify(updateTemplate);
                        OmniPrinters.PrintCapture(newCapture);
                    }
                }
                else
                {
                    this.Messages.Add(testName + "Failure");
                    ++failures;
                }
            }
            catch (OmniException e)
            {
                this.Messages.Add(testName + "Failure: " + e.Message);
                failures++;
            }
            return failures;
        }

        public ulong DoFileAdapter(String nameCapture, String nameFile, String[] saveAllNames)
        {
            this.log.WriteLine(Environment.NewLine + "Test File Adapter");
            String testName = "File Adapter Tests: ";
            ulong failures = 0;
            try
            {
                EngineStatus status = this.engine.GetStatus();

                FileInformationList infoList = this.engine.GetFileList();
                FileInformation info = infoList.FindByName(nameFile);
                if (info == null)
                {
                    String pathname = Path.Combine(this.pathFiles, nameFile);
                    engine.SendFile(pathname);

                    infoList = this.engine.GetFileList();
                    info = infoList.FindByName(nameFile);
                    if (info == null)
                    {
                        this.Messages.Add(testName + "Failure to send file to the engine.");
                        ++failures;
                        return failures;
                    }
                }

                CaptureList cl = this.engine.GetCaptureList();
                List<Capture> cli = cl.FindAll(c => (c.Name == nameCapture));
                this.engine.DeleteCapture(cli);

                Capture capture = this.engine.FindCapture(nameCapture);
                while (capture != null)
                {
                    engine.DeleteCapture(capture);
                    capture = engine.FindCapture(nameCapture);
                }

                FileAdapter fileAdapter = new FileAdapter(status.DataFolder + nameFile, this.engine);
                fileAdapter.Limit = 2;
                fileAdapter.Speed = 0.0;

                CaptureTemplate template = new CaptureTemplate();
                template.SetAdapter(fileAdapter);
                template.General.Name = nameCapture;
                template.General.OptionContinuousCapture = true;
                template.General.OptionCaptureToDisk = true;
                template.General.OptionStartCapture = false;
                template.Analysis.NodeLimit.Enabled = true;
                template.Analysis.ProtocolLimit.Enabled = true;
                template.Analysis.OptionAlarms = true;
                template.Analysis.OptionAnalysisModules = true;
                template.Analysis.OptionError = true;
                template.Analysis.OptionNetwork = true;
                template.Analysis.OptionSize = true;
                template.Analysis.OptionSummary = true;
                template.Analysis.OptionTraffic = true;
//                 template.StopTrigger.Enabled = true;
//                 template.StopTrigger.Time.Enabled = true;
//                 template.StopTrigger.Time.OptionElapsedTime = true;
//                 template.StopTrigger.Time.Time = new PeekTime((54 + 5) * 1000000000L);
//                 template.StopTrigger.Captured.Enabled = true;
//                 template.StopTrigger.Captured.Bytes = (970043 + 64);

                String logpathname = Path.Combine(this.pathTemp, "log_capture_template.xml");
                template.Store(logpathname, this.engine);

                Capture newCapture = engine.CreateCapture(template);
                if (newCapture == null) throw new OmniException("Failed to create capture with file adapter.");
                OmniPrinters.PrintCapture(newCapture);
                newCapture.Start();
                newCapture.Refresh();
                while (newCapture.IsCapturing() && (newCapture.PacketCount < 6000))
                {
                    newCapture.Refresh();
                }
                newCapture.Stop();
                newCapture.Refresh();
                OmniPrinters.PrintCapture(newCapture);

                FileInformationList fil = engine.GetFileList();
                foreach (String name in saveAllNames)
                {
                    if (fil.Any(fi => (fi.Name == name)))
                    {
                        engine.DeleteFile(name);
                    }
                }

                engine.SaveAllPackets(newCapture, saveAllNames[0]);
                newCapture.SaveAllPackets(saveAllNames[1]);

            }
            catch (OmniException e)
            {
                this.Messages.Add(testName + "Failure: " + e.Message);
                failures++;
            }
            return failures;
        }

        public ulong DoStats()
        {
            this.log.WriteLine(Environment.NewLine + "Test Stats");
            String testName = "Stats Tests: ";
            ulong failures = 0;
            try
            {
                CaptureList captures = engine.GetCaptureList();
                if (captures.Count == 0){
                    return failures;
                }

                Capture capture = captures[0];
                SummaryStatistics summarys = capture.GetSummaryStats();

                if (summarys != null)
                {
                    int success = 0;

                    SummaryStatistic packetsItem = summarys["Network"]["Total Packets"];
                    ulong totalPackets = 0;
                    if (packetsItem.TryULong(ref totalPackets))
                    {
                        success++;
                    }
                    SummaryStatistic bytesItem = summarys["Network"]["Total Bytes"];
                    ulong totalBytes = 0;
                    if (bytesItem.TryULong(ref totalBytes))
                    {
                        success++;
                    }

                    try
                    {
                        ulong packetsTotal = summarys["Network"]["Total Packets"].ToULong();
                        ulong bytesTotal = summarys["Network"]["Total Bytes"].ToULong();
                    }
                    catch (OmniException e)
                    {
                        this.Messages.Add(testName + "Failure to get statistic: " + e.Message);
                        failures++;
                    }
                }
            }
            catch (OmniException e)
            {
                this.Messages.Add(testName + "Failure: " + e.Message);
                failures++;
            }
            return failures;
        }

        public ulong DoForensicTemplate()
        {
            this.log.WriteLine(Environment.NewLine + "Test Forensic Template");
            String testName = "Forensic Tests: ";
            ulong failures = 0;
            try
            {
            }
            catch (OmniException e)
            {
                this.Messages.Add(testName + "Failure: " + e.Message);
                failures++;
            }
            return failures;
        }

        public ulong DoThreadCountTest()
        {
            Console.WriteLine(Environment.NewLine);
            Console.Write("Begin");
            // Console.ReadLine();

            String testName = "Thread Count Tests: ";
            ulong failures = 0;

            if (true)
            {
                OmniScript omni = null;

                OmniScript.EngineAddress address1 = new OmniScript.EngineAddress();
                OmniScript.EngineCredentials credentials1 = new OmniScript.EngineCredentials();
                OmniEngine engine1 = null;

                OmniScript.EngineAddress address2 = new OmniScript.EngineAddress("10.4.2.205");
                OmniScript.EngineCredentials credentials2 = new OmniScript.EngineCredentials("ThirdParty", "", "gary", "savvius");
                OmniEngine engine2 = null;

                try
                {
                    // Create an OmniScript object.
                    omni = new OmniScript();
                    Console.Write(Environment.NewLine);
                    Console.Write("Create OmniScript");
                    // Console.ReadLine();

                    engine1 = omni.CreateEngine(address1);
                    Console.Write(Environment.NewLine);
                    Console.Write("Create Engine 1");
                    // Console.ReadLine();

                    engine2 = omni.CreateEngine(address2);
                    Console.Write(Environment.NewLine);
                    Console.Write("Create Engine 2");
                    // Console.ReadLine();

                    engine1.Connect(credentials1);
                    Console.Write(Environment.NewLine);
                    Console.Write("Connect 1");
                    // Console.ReadLine();

                    engine2.Connect(credentials2);
                    Console.Write(Environment.NewLine);
                    Console.Write("Connect 2");
                    // Console.ReadLine();

                }
                catch (OmniException e)
                {
                    this.Messages.Add(testName + "Failure: " + e.Message);
                    failures++;
                }

                if (true)
                {
                    if (engine1 != null)
                    {
                        engine1.Disconnect();
                    }
                    Console.Write(Environment.NewLine);
                    Console.Write("Disconnect 1");
                    // Console.ReadLine();

                    if (engine2 != null)
                    {
                        engine2.Disconnect();
                    }
                    Console.Write(Environment.NewLine);
                    Console.Write("Disconnect 2");
                    // Console.ReadLine();

                    engine1 = null;
                    engine2 = null;
                    Console.Write(Environment.NewLine);
                    Console.Write("Clear Engines");
                    // Console.ReadLine();

                    omni = null;
                    Console.Write(Environment.NewLine);
                    // Console.ReadLine();
                }
            }

            Console.Write(Environment.NewLine);
            Console.Write("Done");
            Console.ReadLine();
            return failures;
        }

        public ulong DoPluginSettings()
        {
            this.log.WriteLine(Environment.NewLine + "Test Plugin Settings");
            // Console.ReadLine();

            String testName = "Plugin Settings Tests: ";
            ulong failures = 0;
            try
            {
                Capture capture = this.engine.FindCapture("Capture with Plugin Settings");
                if (capture != null)
                {
                    CaptureTemplate template = capture.GetCaptureTemplate();
                    OmniPrinters.PrintCaptureTemplate(template);
                    for (int i = 0; i < template.Plugins.PluginList.Count; i++)
                    {
                        if (template.Plugins.PluginList[i].Name.Substring(0, 10) == "CefPrefsOE")
                        {
                            // Someone decided that their XML settings didn't need to be wrapped in a single tag...
                            String settings = "<plugin>" + template.Plugins.PluginList[i].Settings + "</plugin>";
                            XDocument doc = XDocument.Parse(settings);
                            XElement root = doc.Root;   // root is the 'plugin' element.
                            // Iterate through all the 'Case' elements.
                            IEnumerable<XElement> cases = root.Elements("Case");
                            foreach (XElement e in cases)
                            {
                                // Double the value: abc -> abc abc
                                e.Value = e.Value + " " + e.Value;
                            }

                            // Reconstruct the settings.
                            String newSettings = "";
                            foreach (XElement e in cases)
                            {
                                newSettings += e.ToString();
                            }
                            XElement notes = root.Element("Notes");
                            if (notes != null)
                            {
                                newSettings += root.Element("Notes").ToString();
                            }

                            template.Plugins.PluginList[i].Settings = newSettings;
                        }
                    }
                    capture.Modify(template);
                }
            }
            catch (OmniException e)
            {
                this.Messages.Add(testName + "Failure: " + e.Message);
                failures++;
            }
            return failures;
        }

        public ulong DoPluginMessage()
        {
            this.log.WriteLine(Environment.NewLine + "Test Plugin Message");
            // Console.ReadLine();

            String testName = "Plugin Message Tests: ";
            ulong failures = 0;
            try
            {
                OmniId pluginId = new OmniId("{E63C9D25-B13C-47F8-91CB-BDD25E191FDA}");
                OmniId contextId = new OmniId();
                String text = "<Tab><Message>Hello</Message></Tab>";
                String result = this.engine.SendPluginMessage(pluginId, contextId,text);

                XDocument doc = XDocument.Parse(result);
                XElement root = doc.Root;
                XElement msg = root.Element("Message");
                String message = msg.Value;

                if (message != "olleH") failures++;
            }
            catch (OmniException e)
            {
                this.Messages.Add(testName + "Failure: " + e.Message);
                failures++;
            }
            return failures;
        }


        static void Help(OmniLogger log)
        {
            //              1         2         3         4         5         6         7         8
            //     12345678901234567890123456789012345678901234567890123456789012345678901234567890
            log.WriteLine("QuickTest");
            log.WriteLine("  [-h <engine IP>]   : IP address of engine. Default is localhost.");
            log.WriteLine("  [-p <engine Port>] : Port number of the engine. Default is 6367.");
            log.WriteLine("  [-a <auth>]        : Authentication type: 'Default' or 'Third Party'. Default is 'Default'.");
            log.WriteLine("  [-d <domain]       : Domain of the credentials.");
            log.WriteLine("  [-u <username>     : User account name.");
            log.WriteLine("  [-w <password>]    : User account password.");
            log.WriteLine("  [-l <log file>]    : Log filename.");
            log.WriteLine("  [-?]               : Display this message and quit.");
        }

        static Options ParseCommandLine(String[] args)
        {
            Options options = new Options();
            IEnumerator e = args.GetEnumerator();
            String v;
            while (e.MoveNext())
            {
                v = (String)e.Current;
                switch (v)
                {
                    case "-h":
                        if (!e.MoveNext()) break;
                        options.Address.Host = (String)e.Current;
                        break;

                    case "-p":
                        if (!e.MoveNext()) break;
                        options.Address.Port = Convert.ToUInt32((String)e.Current);
                        break;

                    case "-a":
                        if (!e.MoveNext()) break;
                        options.Credentials.Auth = (String)e.Current;
                        break;

                    case "-d":
                        if (!e.MoveNext()) break;
                        options.Credentials.Domain = (String)e.Current;
                        break;

                    case "-u":
                        if (!e.MoveNext()) break;
                        options.Credentials.Account = (String)e.Current;
                        break;

                    case "-w":
                        if (!e.MoveNext()) break;
                        options.Credentials.Password = (String)e.Current;
                        break;

                    case "-l":
                        if (!e.MoveNext()) break;
                        options.LogFileName = (String)e.Current;
                        break;

                    case "-?":
                        options.DisplayHelp = true;
                        break;
                }
            }
            return options;
        }

        public OmniLogger log;
        public OmniScript omni;
        public OmniEngine engine;
        public String pathFiles;
        public String pathTemp;

        static void Main(string[] args)
        {
            bool testLegacyConnect = false;

            Console.Clear();

            Program prog = new Program();
            prog.log = new OmniLogger(OmniLogger.Verboseness.Info);
            OmniPrinters.log = prog.log;

            bool test = true;

            Console.WriteLine("Begin CsTest:");
            // Console.ReadLine();

            if (args.Count() == 0)
            {
#if _WINDOWS
                String host = "";
                String auth = "";
                String user = "";
                string pwd = "";
#else
                String host = "192.168.7.200";
                String auth = "ThirdParty";
                String user = "";
                string pwd = "";
#endif

                String[] ayDefaults = new String[] {
                    "-h", host,
                    "-a", auth,
                    "-u", user,
                    "-w", pwd
                };
                args = ayDefaults;
            }
            Options options = ParseCommandLine(args);

            prog.log.LogToFile(options.LogFileName);

            String strCurrentDirectory = Directory.GetCurrentDirectory();
            DirectoryInfo infoFiles = new DirectoryInfo(strCurrentDirectory + @"../../../../files/");
            String pathFiles = infoFiles.FullName;
            DirectoryInfo infoTemp;
            if (infoFiles.Exists)
            {
                Directory.SetCurrentDirectory(infoFiles.FullName);
                infoTemp = new DirectoryInfo(strCurrentDirectory + @"/temp/");
            }
            else
            {
                infoFiles = new DirectoryInfo(strCurrentDirectory + @"/../../files/");
                if (infoFiles.Exists)
                {
                    Directory.SetCurrentDirectory(infoFiles.FullName);
                    infoTemp = new DirectoryInfo(strCurrentDirectory + @"/../../temp/");
                }
                else
                {
                    prog.log.WriteLine("Failed to open Files directory.");
                    return;
                }
            }
            if (!infoTemp.Exists)
            {
                infoTemp.Create();
            }

            if (options.DisplayHelp)
            {
                Help(prog.log);
                return;
            }

            String nameCapture = "C# Test Script";
            String filterName = "C# Simple";
            //String ForensicName = "C# Forensic";
            //String fileCapture = "fortworth101.pkt";
            String testFileName = "OmniScript.egg";
            //int CaptureFilePacketCount = 3190;
            //String ResultFile = "results.pkt";

            Console.WriteLine("Create OmniScript");
            // Console.ReadLine();

            // Create an OmniScript object.
            OmniScript omni = new OmniScript();

            if ( test && testLegacyConnect )
            {
                // Test Legacy Connect command.
                OmniEngine eng = omni.Connect(options.Address, options.Credentials);
                if (eng != null)
                {
                    if (eng.IsConnected())
                    {
                        eng.Disconnect();
                        if (eng.IsConnected())
                        {
                            prog.log.WriteLine("*** Failed to disconnect from the Legacy connection.");
                            return;
                        }
                    }
                    else
                    {
                        prog.log.WriteLine("*** Legacy Connect failed.");
                        return;
                    }
                }
                else
                {
                    prog.log.WriteLine("*** Failed legacy connect command. ***");
                }
            }

            Console.Write(Environment.NewLine);
            Console.Write("Create Engine");
            // Console.ReadLine();

            // Create an OmniEngine object.
            // host address and port.
            OmniEngine engine = omni.CreateEngine(options.Address);
            if (engine == null)
            {
                prog.log.WriteLine("*** Failed to create a new engine object. ***");
            }
            engine.Logger = prog.log;

            Console.WriteLine("Connect");
            // Console.ReadLine();

            // Connect to the engine.
            if (!engine.Connect(options.Credentials))
            {
                prog.log.WriteLine("*** Connection failed.");
                return;
            }
            if (!engine.IsConnected())
            {
                prog.log.WriteLine("*** Failed to connect.");
                return;
            }

            Console.WriteLine("Connected");
            // Console.ReadLine();

            prog.omni = omni;
            prog.engine = engine;
            prog.pathFiles = infoFiles.FullName;
            prog.pathTemp = infoTemp.FullName;

            engine = null;
            omni = null;

            ulong failures = 0;
            
            ulong failuresBackup = 0;
            ulong failuresUnitTests = 0;
            ulong failuresGets = 0;
            ulong failuresFilters = 0;
            ulong failuresFileOps = 0;
            ulong failuresDeleteAll = 0;
            ulong failuresCaptureCreate = 0;
            ulong failuresFileAdapter = 0;
            ulong failuresStats = 0;
            ulong failuresForensicTemplate = 0;
            ulong failureThreadCount = 0;
            ulong failurePluginSettings = 0;
            ulong failurePluginMessage = 0;

            // Which tests to run.
            bool testBackup = true;
            bool testUnitTests = true;
            bool testGets = true;
            bool testFilters = true;
            bool testFileOps = true;
            bool testDeleteAll = true;
            bool testCaptureCreate = true;
            bool testFileAdapter = true;
            bool testStats = true;
            bool testForensicTemplate = true;

            // Development Tests
            bool testThreadCount = false;
            bool testPluginSettings = false;
            bool testPluginMessage = false;

            if (test)
            {
                try
                {
                    if (testBackup)
                    {
                        failuresBackup = prog.DoBackup();
                        failures += failuresBackup;
                    }

                    if (testUnitTests)
                    {
                        failuresUnitTests = prog.DoUnitTests();
                        failures += failuresUnitTests;
                    }

                    if (testGets)
                    {
                        failuresGets = prog.DoGets();
                        failures += failuresGets;
                    }

                    if (testFilters)
                    {
                        failuresFilters = prog.DoFilters(filterName);
                        failures += failuresFilters;
                    }

                    if (testFileOps)
                    {
                        String fullPath = Path.Combine(prog.pathFiles, testFileName);
                        if (!File.Exists(fullPath))
                        {
                            DirectoryInfo infoDist = new DirectoryInfo(strCurrentDirectory + @"../../../py/OmniScript/dist");
                            String fullSourcePath = Path.Combine(infoDist.FullName, "OmniScript-0.6.0-py2.7.egg");
                            File.Copy(fullSourcePath, fullPath);
                        }
                        failuresFileOps = prog.DoFileOps(testFileName);
                        failures += failuresFileOps;
                    }

                    if (testDeleteAll)
                    {
                        failuresDeleteAll = prog.DoDeleteAll();
                        failures += failuresDeleteAll;
                    }

                    if (testCaptureCreate)
                    {
                        failuresCaptureCreate = prog.DoCaptureCreate(nameCapture);
                        failures += failuresCaptureCreate;
                    }

                    if (testFileAdapter)
                    {
                        String[] saveAllNames = new String[] { "C#-TestScript_Engine.pkt", "C#-TestScript_Capture.pkt" };
                        failuresFileAdapter = prog.DoFileAdapter(nameCapture, "fortworth101.pkt", saveAllNames);
                        failures += failuresFileAdapter;
                    }

                    if (testStats)
                    {
                        failuresStats = prog.DoStats();
                        failures += failuresStats;
                    }

                    if (testForensicTemplate)
                    {
                        failuresForensicTemplate = prog.DoForensicTemplate();
                        failures += failuresForensicTemplate;
                    }

                    if (testThreadCount)
                    {
                        failureThreadCount = prog.DoThreadCountTest();
                        failures += failureThreadCount;
                    }

                    if (testPluginSettings)
                    {
                        failurePluginSettings = prog.DoPluginSettings();
                        failures += failurePluginSettings;
                    }

                    if (testPluginMessage)
                    {
                        failurePluginMessage = prog.DoPluginMessage();
                        failures += failurePluginMessage;
                    }

                }
                catch (OmniException e)
                {
                    prog.log.WriteLine(e.Message);
                }

                if (failures == 0)
                {
                    prog.log.WriteLine(Environment.NewLine + "Success");
                }
                else
                {
                    prog.log.WriteLine(Environment.NewLine);
                    prog.log.WriteLine(String.Format("*** FAILED *** Failures: {0}", failures));
                    prog.log.WriteLine(String.Format("  Backup               : {0}", failuresBackup));
                    prog.log.WriteLine(String.Format("  UnitTests            : {0}", failuresUnitTests));
                    prog.log.WriteLine(String.Format("  Gets                 : {0}", failuresGets));
                    prog.log.WriteLine(String.Format("  Filters              : {0}", failuresFilters));
                    prog.log.WriteLine(String.Format("  FileOps              : {0}", failuresFileOps));
                    prog.log.WriteLine(String.Format("  DeleteAll            : {0}", failuresDeleteAll));
                    prog.log.WriteLine(String.Format("  CaptureCreate        : {0}", failuresCaptureCreate));
                    prog.log.WriteLine(String.Format("  FileAdapter          : {0}", failuresFileAdapter));
                    prog.log.WriteLine(String.Format("  Stats                : {0}", failuresStats));
                    prog.log.WriteLine(String.Format("  ForensicTemplate     : {0}", failuresForensicTemplate));
                    prog.log.WriteLine(String.Format("  ThreadCount          : {0}", failureThreadCount));
                    prog.log.WriteLine(String.Format("  PluginSettings       : {0}", failurePluginSettings));
                    prog.log.WriteLine(String.Format("  PluginMessage        : {0}", failurePluginMessage));
                    // throw new OmniException(String.Format("*** FAILED *** Failures: {0}", failures));
                }
                foreach (string msg in prog.Messages)
                {
                    prog.log.WriteLine(msg);
                }
            }
            prog.log.WriteLine(Environment.NewLine);
            prog.log.WriteLine("Testing Done");

            Console.WriteLine("Disconnect");
            // Console.ReadLine();

            prog.engine.Disconnect();

            Console.WriteLine("Set engine, omni and program to null.");
            // Console.ReadLine();

            prog.engine = null;
            prog.omni = null;
            prog = null;

            Console.WriteLine(Environment.NewLine);
            Console.WriteLine("Done");
            // Console.ReadLine();
        }
    }
}
