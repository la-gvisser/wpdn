// =============================================================================
// <copyright file="Program.cs" company="WildPackets, Inc.">
//  Copyright (c) 2013 WildPackets, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace OEMigrate
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.IO;
    using System.Linq;
    using System.Text;
    using System.Xml.Linq;
    using Savvius.Omni.OmniScript;

    /// <summary>
    /// Program
    /// </summary>
    class Program
    {
        public class Options
        {
            public String LogPath { get; set; }
            public bool ContinueOnError { get; set; }
            public OmniScript.EngineAddress Address { get; set; }
            public OmniScript.EngineCredentials Credentials { get; set; }

            public Options()
            {
                this.LogPath = @"C:\oemigrate";
                this.ContinueOnError = false;
                this.Address = new OmniScript.EngineAddress();
                this.Credentials = new OmniScript.EngineCredentials();
            }
        }

        static void CopyFile(String source, String destination)
        {
            System.IO.File.Copy(source, destination, true);
            File.SetAttributes(destination, File.GetAttributes(destination) & ~FileAttributes.ReadOnly);
        }

        static PeekPluginList GetPeekPlugins(String path)
        {
            // To use GetPeekPlugins the executable must be edited to export
            // OmniPeek::GetAppResourcePathCallback. Otherwise the app will crash.
            //   ILDasm.exe oemigrate.exe /out:oemigrate.il
            //   Edit oemigrate.il, the changes are in oemigrate.il in this project.
            //   ILasm.exe oemigrate.il /out:oemigrate.exe

            String currentPath = Directory.GetCurrentDirectory();
            Directory.SetCurrentDirectory(path);

            PeekPluginList pluginList = new PeekPluginList();
            String[] files = Directory.GetFiles(path, "*.dll");
            foreach (String file in files)
            {
                PeekPlugin plugin = new PeekPlugin(file);
                if (plugin.Load())
                {
                    pluginList.Add(plugin);
                }
            }

            Directory.SetCurrentDirectory(currentPath);

            return pluginList;
        }

        static Transform.GuidMap GetPluginIdMap(Transform.GuidMap baseMap, NamedIdList peekPlugins, AnalysisModuleList enginePlugins, NamedIdList missing)
        {
            Transform.GuidMap map = baseMap;
            foreach (NamedId peek in peekPlugins)
            {
                // Does the OmniPeek Plugin already have an entry in the map?
                NamedId existing = map.FindById(peek.Id);
                if (existing != null)
                {
                    continue;
                }

                // Is there an OmniEngine Plugin with the same Id?
                AnalysisModule omni = enginePlugins.Find(peek.Id);
                if (omni != null)
                {
                    NamedId omniIdName = new NamedId(omni.Name, omni.Id);
                    map.AddUnique(peek, omniIdName);
                    continue;
                }

                // Try constructing the name: CMI Capture becomes CMICaptureOE.
                String peekName = peek.Name.Replace(" ", "");
                omni = enginePlugins.Find(peekName + "OE", false);
                if (omni != null)
                {
                    NamedId omniIdName = new NamedId(omni.Name, omni.Id);
                    map.AddUnique(peek, omniIdName);
                    continue;
                }

                // Note the missing OmniPeek Plugin.
                NamedId existingId = map.FindById(peek.Id);
                if (existingId == null)
                {
                    missing.Add(peek);
                }
            }
            return map;
        }

        static NamedIdList GetDefaultIds()
        {
            NamedIdList defaultIds = new NamedIdList();
            if (defaultIds.Count == 0)
            {
                defaultIds.Add(new NamedId(Guid.Parse("{45A40522-3991-4295-808B-1C98ABBEDFE0}"), "Aruba Remote Adapter"));
                defaultIds.Add(new NamedId(Guid.Parse("{16C182A2-390C-4FA6-ACDD-5ABEDD72B16B}"), "Cisco Remote Adapter"));
                defaultIds.Add(new NamedId(Guid.Parse("{EF3D3547-AECB-4CCD-A8BE-82E7BBF12AB2}"), "Compass Adapter"));
                defaultIds.Add(new NamedId(Guid.Parse("{20A0696A-DCAD-480C-9C89-E1647A0580C8}"), "Aggregator/Roaming"));
                defaultIds.Add(new NamedId(Guid.Parse("{DD270CE0-21D8-4001-9F9C-FF9F690F2D8D}"), "AppleTalk Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{BC7CC85A-7404-43EE-9033-FDEE2E2373EC}"), "Checksums Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{0A8461F5-E8AB-4C22-9296-290BEB9FC4B1}"), "Duplicate Address"));
                defaultIds.Add(new NamedId(Guid.Parse("{A090DCAD-ACA7-4734-9F1C-FA7DD153F501}"), "Email Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{B77F78E1-ABC6-4ACF-9664-3C8A9671695A}"), "FTP Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{8BE46BFF-6FE0-4A1D-BEFE-7B82DAFE1188}"), "ICMP Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{EAF651D4-6488-4D8A-A93E-758213F32AFD}"), "IP Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{18C1A482-05DB-4258-AB2E-26C626DD3B28}"), "Modbus Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{C502677E-866C-42A9-95F7-DC874A372BA2}"), "NCP Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{7257C019-1E22-4D9E-8A88-AB9E815A2F8A}"), "NCP Analysis (2)"));
                defaultIds.Add(new NamedId(Guid.Parse("{F28306F0-8BBC-4C4E-9AB8-568016F9EFD3}"), "NetWare Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{55E8035F-015D-49AF-9F5E-61713E26FE67}"), "Newsgroup Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{BEB79508-5145-418A-BA3B-CB4BC7CF768F}"), "PPP Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{59A7A57D-2F10-493C-B3F2-DE0C55F6F048}"), "Radius Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{959F52CC-8771-455B-B570-3E0C738E36F1}"), "SCTP Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{A613412F-5E0D-46EE-9020-95DD2FBA9340}"), "SCTP Analysis (2)"));
                defaultIds.Add(new NamedId(Guid.Parse("{CA172D2A-9217-4704-ADE1-606DC06B1929}"), "SMB Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{9B6FD118-B8E4-42E9-8704-7610529FF322}"), "SQL Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{D2203288-26C5-405E-8267-A1EC5C97A70D}"), "SUM Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{59D7C691-1392-4AEB-8E3E-4A0BA772A213}"), "Telnet Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{FB589B62-D187-41EC-8D15-2FAC50AC487E}"), "MPLS/VLAN Analysis Module"));
                defaultIds.Add(new NamedId(Guid.Parse("{94A6242E-8FE0-440A-8AAB-9DA77AC7F7BD}"), "VoIP Analysis Module"));
                defaultIds.Add(new NamedId(Guid.Parse("{029E7440-7028-479A-9519-5F7A30D211CF}"), "WAN Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{E2E91781-238D-4DB6-85E2-142A1EB01A7A}"), "Web Analysis"));
                defaultIds.Add(new NamedId(Guid.Parse("{DFF9DB51-75D3-405E-862B-622EDDCD04CC}"), "802.11 Analysis"));
            }
            return defaultIds;
        }

        static AnalysisModuleList GetCustomPluginList(AnalysisModuleList plugins)
        {
            NamedIdList defaultIds = GetDefaultIds();
            AnalysisModuleList list = new AnalysisModuleList(plugins.logger);
            foreach (AnalysisModule plugin in plugins)
            {
                NamedId resultId = defaultIds.Find(plugin.Id);
                if (NamedId.IsNullOrEmpty(resultId))
                {
                    list.Add(plugin);
                }
            }
            return list;
        }

        public static String MakeValidFileName(string name)
        {
            String invalidChars = System.Text.RegularExpressions.Regex.Escape(new String(System.IO.Path.GetInvalidFileNameChars()));
            String invalidReStr = String.Format(@"([{0}]*\.+$)|([{0}]+)", invalidChars);
            return System.Text.RegularExpressions.Regex.Replace(name, invalidReStr, "_");
        }

        static void LogToFile(String filename, String content)
        {
            using (StreamWriter outfile = new StreamWriter(filename, false))
            {
                outfile.Write(content);
            }
        }

        static void DisplayHelpText()
        {
            Console.WriteLine("OEMigrate [options]");
            Console.WriteLine("");
            Console.WriteLine("Options:");
            Console.WriteLine("  -l log_path");
            Console.WriteLine("       Path of the output directory [Default: C:\\oemigrate]");
            Console.WriteLine("  -c");
            Console.WriteLine("       Continue on error [Default: false]");
            Console.WriteLine("");
            Console.WriteLine("  Engine Credentials:");
            Console.WriteLine("  -e engine");
            Console.WriteLine("       IP Address of OmniEngine [Default: 'localhost']");
            Console.WriteLine("  -p port");
            Console.WriteLine("       Port number of OmniEngine [Default: 6367]");
            Console.WriteLine("  -t auth");
            Console.WriteLine("       Authentication method: 'Default' or 'Third Party'");
            Console.WriteLine("       [Default: 'Default']");
            Console.WriteLine("  -d domain");
            Console.WriteLine("       Domain of the user account. [Default: local domain]");
            Console.WriteLine("  -u user");
            Console.WriteLine("       User [Default: local account]");
            Console.WriteLine("  -w password");
            Console.WriteLine("       Password [Default: local account password]");
        }

        static bool ParseCommandLine(String[] args, Options options)
        {
            char[] trimChars = { ' ', '/', '\\' };
            int count = args.Count();
            for (int i = 0; i < count; i++)
            {
                if (args[i][0] == '-')
                {
                    char cmd = args[i][1];

                    // Commands with no arguments.
                    switch (cmd)
                    {
                        case 'c':   // Continue on error
                            options.ContinueOnError = true;
                            continue;

                        case '?':   // Display Help text
                            DisplayHelpText();
                            return false;
                    }

                    // Commands with one argument
                    i++;
                    if (i >= count)
                    {
                        //fail
                        Console.WriteLine("OEMigrate: Missing argument.");
                        DisplayHelpText();
                        return false;
                    }

                    switch (cmd)
                    {
                        case 'e':   // engine ip address
                            options.Address.Host = args[i];
                            break;

                        case 'p':   // Port
                            options.Address.Port = Convert.ToUInt16(args[i]);
                            break;

                        case 't':   // auTh : Default or Third Party
                            options.Credentials.Auth = args[i];
                            break;

                        case 'd':   // Domain
                            options.Credentials.Domain = args[i];
                            break;

                        case 'u':   // User account
                            options.Credentials.Account = args[i];
                            break;

                        case 'w':   // passWord
                            options.Credentials.Password = args[i];
                            break;

                        case 'l':   // Log Path
                            options.LogPath = args[i].TrimEnd(trimChars);
                            break;
                    }
                }
            }

            return true;
        }

        static NamedIdList ReadPeekPluginReport(String peekPath, String localPath, String logPath, OmniLogger logger)
        {
            String appPath = localPath + @"\bin\PeekPluginReport.exe";
            String reportPath = logPath + @"\PluginReport.txt";
            logger.Log("  Creating Plugin Report:");
            logger.Log("    Application:        " + appPath);
            logger.Log("    OmniPeek Directory: " + peekPath);
            logger.Log("    Report:             " + reportPath);

            Process peekPluginReport = new Process();
            peekPluginReport.StartInfo.FileName = appPath;
            peekPluginReport.StartInfo.Arguments = "-p \"" + peekPath + "\" -o \"" + reportPath + "\"";
            peekPluginReport.Start();
            peekPluginReport.WaitForExit();

            NamedIdList pluginList = NamedIdList.Read(reportPath);

            NamedIdList defaultIds = GetDefaultIds();
            NamedIdList list = new NamedIdList();
            foreach (NamedId plugin in pluginList)
            {
                NamedId resultId = defaultIds.Find(plugin.Id);
                if (NamedId.IsNullOrEmpty(resultId))
                {
                    list.Add(plugin);
                }
            }
            return list;
        }

        static void Main(String[] args)
        {
            Options options = new Options();
            if (!ParseCommandLine(args, options))
            {
                return;
            }

            String logPath = options.LogPath;

            DirectoryInfo logDI = Directory.CreateDirectory(logPath);
            if (!logDI.Exists)
            {
                Console.WriteLine("*** Invalid logging path (-l): " + logPath);
                return;
            }
            logPath = logDI.FullName;   // Get existing directory capitalization.

            Directory.CreateDirectory(logPath + @"\Engine");
            Directory.CreateDirectory(logPath + @"\Engine\Captures");
            Directory.CreateDirectory(logPath + @"\Engine\Filters");
            Directory.CreateDirectory(logPath + @"\Migrate");
            Directory.CreateDirectory(logPath + @"\Migrate\Captures");
            Directory.CreateDirectory(logPath + @"\Migrate\Filters");
            Directory.CreateDirectory(logPath + @"\Peek");
            Directory.CreateDirectory(logPath + @"\Peek\Captures");
            Directory.CreateDirectory(logPath + @"\Peek\Filters");

            OmniLogger logger = new OmniLogger();
            logger.LogToFile(logPath + @"\oemigrate.txt");

            // ================================================================
            logger.Log("oeMigrate");
            logger.Log();

            String currentPath = Directory.GetCurrentDirectory();
            logger.Log("Current Directory:            " + currentPath);

            String progFilesX86 = OmniScript.GetProgramFilesX86();

            String scriptPath = progFilesX86 + @"\Savvius\OmniScript";
            logger.Log("OmniScript Program Directory: " + scriptPath);

            String omnipeekPath = progFilesX86 + @"\Savvius\OmniPeek";
            logger.Log("OmniPeek Program Directory:   " + omnipeekPath);

            String appData = System.Environment.GetEnvironmentVariable("APPDATA");
            String peekDataPath = appData + @"\Savvius\OmniPeek";
            logger.Log("OmniPeek Data Directory:      " + peekDataPath);
            logger.Log("Logging Directory:            " + logPath);
            logger.Log();

            // Get the list of Omni's (default) plugin ids.
            NamedIdList defaultIds = GetDefaultIds();
            defaultIds.Save(logPath + @"\WPPluginIds.txt");
            logger.Log("OmniPeek & OmniEngine Plugins Ids saved to: " + logPath + @"\WPPluginIds.txt");

            // ================================================================
            logger.Log();
            logger.Log("OmniPeek:");
            // ================================================================

            // Get OmniPeek's list of plugins.
            NamedIdList peekPlugins = ReadPeekPluginReport(omnipeekPath, scriptPath, logPath, logger);
            logger.Log("  Plugins:         " + peekPlugins.Count.ToString());
            peekPlugins.WriteToLog(logger, "    ");

            // Read the current user's OmniPeek Filter list.
            ConsoleFilterList userFilters = null;
            String userFiltersPath = peekDataPath + @"\Filters.flt";
            if (File.Exists(userFiltersPath))
            {
                userFilters = new ConsoleFilterList(userFiltersPath, peekPlugins);
                logger.Log("  User Filters:    " + userFilters.Count.ToString().PadRight(4) + " " + userFiltersPath);

                // Copy the user's filter file to the log folder.
                CopyFile(userFiltersPath, logPath + @"\Peek\" + "userfilters.xml");
                logger.Log("  Saved User Filters file to: " + logPath + @"\Peek\" + "userfilters.xml");
            }
            else
            {
                logger.Log("  User Filters file does not exist: " + userFiltersPath);
            }

            if (userFilters != null)
            {
                foreach (ConsoleFilter filter in userFilters)
                {
                    // Print the each filter to file.
                    LogToFile(logPath + @"\Peek\Filters\" + MakeValidFileName(filter.Name) + ".txt", filter.ToString());
                    // Write each filter to file as XML.
                    LogToFile(logPath + @"\Peek\Filters\" + MakeValidFileName(filter.Name) + ".xml", filter.ToString("X"));
                }
                logger.Log("  Saved User Filters to: " + logPath + @"\Peek\Filters");
            }

            // Read OmniPeek's Default Filter list.
            ConsoleFilterList defaultFilters = null;
            String defaultFiltersPath = omnipeekPath + @"\1033\Filters\default.flt";
            if (File.Exists(defaultFiltersPath))
            {
                defaultFilters = new ConsoleFilterList(defaultFiltersPath);
                logger.Log("  Default Filters: " + defaultFilters.Count.ToString().PadRight(4) + " " + defaultFiltersPath);

                // Copy the default filters file to the log folder.
                CopyFile(defaultFiltersPath, logPath + @"\Peek\" + "defaultfilters.xml");
                logger.Log("  Saved User Filters file to: " + logPath + @"\Peek\" + "defaultfilters.xml");
            }
            else
            {
                logger.Log("  Default Filters file does not exist: " + defaultFiltersPath);
            }

            // Build a list of Console Adapters. OmniPeek Capture Templates do not always include
            // all of the adapter attributes (the AdapterName for instance).
            Transform.ConsoleAdapterList peekAdapterList = new Transform.ConsoleAdapterList();

            // Read the CaptureSettings list of OmniPeek capture templates.
            ConsoleTemplateList peekCaptures = null;
            String peekCapturesPath = peekDataPath + @"\CaptureSettings.xml";
            if (File.Exists(peekCapturesPath))
            {
                peekCaptures = new ConsoleTemplateList(peekCapturesPath);
                logger.Log("  Captures:        " + peekCaptures.Count.ToString().PadRight(4) + " " + peekCapturesPath);
                foreach (ConsoleTemplate capture in peekCaptures)
                {
                    logger.Log("    " + capture.CaptureName);
                    capture.Store(logPath + @"\Peek\Captures\" + capture.CaptureName + ".xml");

                    Transform.ConsoleAdapter adapter = new Transform.ConsoleAdapter(capture.AdapterSettings.Id, capture.AdapterSettings.Type, capture.AdapterSettings.Name);
                    peekAdapterList.AddUnique(adapter);
                }

                // Copy the Capture Settings file to the log folder.
                CopyFile(peekCapturesPath, logPath + @"\Peek\" + "CaptureSettings.xml");
                logger.Log("  Saved Capture Settings file to: " + logPath + @"\Peek\" + "CaptureSettings.xml");
            }
            else
            {
                logger.Log("  Capture Settings file does not exist: " + peekCapturesPath);
            }

            // ================================================================
            logger.Log();
            logger.Log("OmniEngine:");
            // ================================================================

            // Connect to the OmniEngine.
            OmniScript omni = new OmniScript(logger);

            // Create an engine object.
            OmniEngine engine = omni.CreateEngine(options.Address);
            if (engine == null)
            {
                logger.Log("*** Failed to create an engine object. ***");
                return;
            }

            // Connect( host, port, auth, domain, user, password );
            //   See OmniPeek's Connect dialog in the Engines view.
            //   auth : "Default" or "Third Party".
            if (!engine.Connect(options.Credentials))
            {
                logger.Log("*** Failed to issue connect to the engine.");
                return;
            }

            if (!engine.IsConnected())
            {
                logger.Log("*** Failed to connect to the engine.");
                return;
            }
            logger.Log("  Connected to OmniEngine: " + options.Address.Host);

            AnalysisModuleList enginePlugins = engine.GetAnalysisModuleList();
            logger.Log("  Plugins: " + enginePlugins.Count.ToString());

            FilterList engineFilters = engine.GetFilterList();
            logger.Log("  Filters: " + engineFilters.Count.ToString());

            // Save the xml of the engine filters that is returned by the engine.
            // //String engineFiltersXml = engine.GetFilterListXml();
            // //LogToFile(logPath + @"\Engine\" + "rawfilterlist.xml", engineFiltersXml);
            // //logger.Log("    Saved Raw Filter list to: " + logPath + @"\Engine\" + "rawfilterlist.xml");
            // //LogToFile(logPath + @"\Engine\" + "filterlist.xml", engineFilters.XmlText);
            // //logger.Log("    Saved Filters to: " + logPath + @"\Engine\" + "filterlist.xml");

            foreach (Filter filter in engineFilters)
            {
                LogToFile(logPath + @"\Engine\Filters\" + MakeValidFileName(filter.Name) + ".xml", filter.ToString("X"));
            }
            logger.Log("    Saved individual Filters to: " + logPath + @"\Engine\Filters");

            // ================================================================
            logger.Log();
            logger.Log("Migration:");
            // ================================================================

            AnalysisModuleList customPlugins = GetCustomPluginList(enginePlugins);
            logger.Log("  Custom Plugins: " + customPlugins.Count.ToString());
            customPlugins.GetNamedIdList().WriteToLog(logger, "    ");

            logger.Log();

            // Load the existing OmniPeek plugin Guid to OmniEngine plugin Guid map.
            String existingIdMapPath = scriptPath + @"\PluginIdMap.txt";
            Transform.GuidMap existingIdMap = Transform.GuidMap.Read(existingIdMapPath);
            logger.Log("  Existing Plugin Id Map: " + existingIdMap.Count + " " + existingIdMapPath);
            String savedIdMapPath = logPath + @"\ExistingIdMap.txt";
            existingIdMap.Save(savedIdMapPath);
            logger.Log("  Existing Plugin Id Map saved to: " + savedIdMapPath);

            // Build the OmniPeek plugin to OmniEngine plugin map.
            String pluginIdMapPath = logPath + @"\PluginIdMap.txt";
            NamedIdList missingIds = new NamedIdList();
            Transform.GuidMap pluginIdMap = GetPluginIdMap(existingIdMap, peekPlugins, customPlugins, missingIds);
            logger.Log("  Plugin Id Map: " + pluginIdMap.Count.ToString().PadRight(4) + pluginIdMapPath);
            pluginIdMap.WriteToLog(logger, "    ");
            pluginIdMap.Save(pluginIdMapPath);

            if (!NamedIdList.IsNullOrEmpty(missingIds))
            {
                logger.Log();
                logger.Log("*** OmniPeek Plugins without OmniEngine counterpart:");
                missingIds.WriteToLog(logger, "  ");
                logger.Log("*** Plugin mapping saved to: " + pluginIdMapPath);
                logger.Log("***      Update and copy to: " + existingIdMapPath);

                if (!options.ContinueOnError)
                {
                    return;
                }
            }

            logger.Log();

            // Create a list of OmniEngine Filters from the user's filter list that are not
            // in OmniPeek's default filter list.
            FilterList customFilters = new FilterList(logger);
            if (!ConsoleFilterList.IsNullOrEmpty(userFilters))
            {
                foreach (ConsoleFilter filter in userFilters)
                {
                    ConsoleFilter found = defaultFilters.Find(filter);
                    if (found == null)
                    {
                        List<String> errors = new List<String>();
                        Filter unique = Transform.Filter(filter, defaultIds, pluginIdMap, errors);
                        if (unique != null && (errors.Count == 0))
                        {
                            customFilters.Add(unique);
                        }
                        else
                        {
                            logger.Log(String.Format("*** Failed to convert OmniPeek Filter to OmniEngine Filter: {0}", filter.Name));
                            foreach (String error in errors)
                            {
                                logger.Log(error);
                            }

                            if (!options.ContinueOnError)
                            {
                                return;
                            }
                        }
                    }
                }
            }
            logger.Log("  Custom Filters: " + customFilters.Count.ToString());
            foreach (Filter filter in customFilters)
            {
                logger.Log("    " + filter.Name);
                LogToFile(logPath + @"\Engine\" + MakeValidFileName(filter.Name) + ".txt", filter.ToString());
                LogToFile(logPath + @"\Engine\" + MakeValidFileName(filter.Name) + ".xml", filter.ToString("X"));
            }

            // Add the unique filters to the engine.
            if (customFilters.Count > 0)
            {
                // engine.AddFilters(customFilters);
                // Add each filter one at a time.
                foreach (Filter filter in customFilters)
                {
                    try
                    {
                        FilterList list = new FilterList(logger);
                        list.Add(filter);
                        engine.AddFilter(list);
                    }
                    catch (System.Exception)
                    {
                        logger.Log("*** Failed to add filter: " + filter.Name);
                        if (!options.ContinueOnError)
                        {
                            return;
                        }
                    }
                }
            }

            FilterList filters = engine.GetFilterList();
            logger.Log("  Updated engine filter count: " + filters.Count.ToString());

            if (!ConsoleTemplateList.IsNullOrEmpty(peekCaptures))
            {
                logger.Log();
                int longestName = peekCaptures.GetNamedIdList().LongestName();
                foreach (ConsoleTemplate console in peekCaptures)
                {
                    List<String> errors = new List<String>();
                    CaptureTemplate template = Transform.Template(console, filters, defaultIds, pluginIdMap, peekAdapterList, errors);
                    if ((template != null) && (errors.Count == 0))
                    {
                        LogToFile(logPath + @"\Migrate\Captures\" + MakeValidFileName(template.Name) + ".xml", template.ToString(engine));
                        logger.Log("    Capture Template saved in: " + logPath + @"\Migrate\Captures\" + MakeValidFileName(template.Name) + ".xml");

                        Capture capture = engine.CreateCapture(template);
                        if (capture != null)
                        {
                            logger.Log("  Created OmniEngine Capture: " + capture.Name.PadRight(longestName) + " : " + capture.Id);
                        }
                        else
                        {
                            logger.Log("*** Failed to create OmniEngine Capture: " + template.Name);
                            if (!options.ContinueOnError)
                            {
                                return;
                            }
                        }
                    }
                    else
                    {
                        logger.Log(String.Format("***Failed to migrate Capture: {0}", console.CaptureName));
                        foreach (String error in errors)
                        {
                            logger.Log(error);
                        }

                        if (!options.ContinueOnError)
                        {
                            return;
                        }
                    }
                }
            }
        }
    };
}
