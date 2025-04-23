// =============================================================================
// <copyright file="QuickTest.cs" company="Savvius, Inc.">
//  Copyright (c) 2015 Savvius, Inc. All rights reserved.
//  Copyright (c) 2013 WildPackets, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace QuickTest
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Text;
    using Savvius.Omni.OmniScript;

    public class Options
    {
        public bool DisplayHelp;
        public bool Vigil;
        public String LogFileName;
        public OmniScript.EngineAddress Address { get; set; }
        public OmniScript.EngineCredentials Credentials { get; set; }
        public bool Filters;

        public Options()
        {
            this.DisplayHelp = false;
            this.LogFileName = null;
            this.Address = new OmniScript.EngineAddress();
            this.Credentials = new OmniScript.EngineCredentials();
            this.Filters = false;
        }
    }

    /// <summary>
    /// Program
    /// </summary>
    class Program
    {
        public class ProgramLog
        {
            private StreamWriter Log;

            public ProgramLog(String filename)
            {
                this.Log = null;
                if (filename != null)
                {
                    this.Log = new StreamWriter(filename);
                }
            }

            ~ProgramLog()
            {
            }

            public void Flush()
            {
                if (this.Log != null)
                {
                    this.Log.Flush();
                }
            }

            public void WriteLine(String line)
            {
                Console.WriteLine(line);
                if (this.Log != null)
                {
                    this.Log.WriteLine(line);
                }
            }
        }

        static void Help()
        {
            //              1         2         3         4         5         6         7         8
            //     12345678901234567890123456789012345678901234567890123456789012345678901234567890
            Console.WriteLine("QuickTest");
            Console.WriteLine("  [-e <engine IP>]   : IP address of engine. Default is localhost.");
            Console.WriteLine("  [-p <engine Port>] : Port number of the engine. Default is 6367.");
            Console.WriteLine("  [-t <auth>]        : Authentication type: 'Default' or 'Third Party'. Default is 'Default'.");
            Console.WriteLine("  [-d <domain]       : Domain of the credentials.");
            Console.WriteLine("  [-u <username>     : User account name.");
            Console.WriteLine("  [-w <password>]    : User account password.");
            Console.WriteLine("  [-f]               : Display the Filters.");
            Console.WriteLine("  [-l <log file>]    : Log filename.");
            Console.WriteLine("  [-?]               : Display this message and quit.");
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
                    case "-e":
                        if (!e.MoveNext()) break;
                        options.Address.Host = (String)e.Current;
                        break;

                    case "-p":
                        if (!e.MoveNext()) break;
                        options.Address.Port = Convert.ToUInt32((String)e.Current);
                        break;

                    case "-t":
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

                    case "-f":
                        options.Filters = true;;
                        break;

                    case "-l":
                        if (!e.MoveNext()) break;
                        options.LogFileName = (String)e.Current;
                        break;

                    case "-v":
                        options.Vigil = true;
                        break;

                    case "-?":
                        options.DisplayHelp = true;
                        break;
                }
            }
            return options;
        }

        static void Main(String[] args)
        {
            Options options = Program.ParseCommandLine(args);
            ProgramLog Log = new ProgramLog(options.LogFileName);
            {
                Log.WriteLine("OmniScript QuickTest:");

                if (options.DisplayHelp)
                {
                    Help();
                    return;
                }

                if (options.Filters)
                {
                    Log.WriteLine("Pretty Print Filters enabled.");
                }

                // Create an OmniScript object.
                OmniScript omni = new OmniScript();

                // Create an OmniEngine object.
                // host address and port.

                OmniEngine engine = omni.CreateEngine(options.Address);
                if (engine == null)
                {
                    Log.WriteLine("*** Failed to create a new engine object. ***");
                    return;
                }

                // Connect( EngineCredentials ) or
                // Connect( auth, domain, user, password );
                //   See OmniPeek's Connect dialog in the Engines view.
                //   auth : "Default" or "Third Party".
                try
                {
                    engine.Connect(options.Credentials);
                    Log.WriteLine("  Connected to OmniEngine: " + options.Address.Host);

                    bool bConnected = engine.IsConnected();
                    if (bConnected)
                    {
                        Log.WriteLine("The connection is open.");
                    }
                    else
                    {
                        Log.WriteLine("*** Failed to open the connection.");
                        return;
                    }
                }
                catch (OmniException e)
                {
                    Log.WriteLine(e.Message);
                    return;
                }

                string strVersion = engine.GetVersion();
                Log.WriteLine("Version: " + strVersion);

                AdapterList adapters = engine.GetAdapterList();
                Log.WriteLine("Adapters: " + adapters.Count.ToString());
                foreach (Adapter adapter in adapters)
                {
                    Log.WriteLine("  " + adapter.Name);
                }

                FilterList filters = engine.GetFilterList();
                Log.WriteLine("Filters: " + filters.Count.ToString());
                foreach (Filter filter in filters)
                {
                    if (options.Filters)
                    {
                        Log.WriteLine(filter.ToString(0));
                        Log.WriteLine("");
                    }
                    else
                    {
                        Log.WriteLine("  " + filter.Name);
                    }
                }

                AnalysisModuleList plugins = engine.GetAnalysisModuleList();
                Log.WriteLine("Plugins: " + plugins.Count.ToString());
                foreach (AnalysisModule plugin in plugins)
                {
                    Log.WriteLine("  " + plugin.Name);
                }

                CaptureList captures = engine.GetCaptureList();
                Log.WriteLine("Captures: " + captures.Count.ToString());
                foreach (Capture capture in captures)
                {
                    Log.WriteLine("  " + capture.Name);
                    if (capture.Comment != null)
                    {
                        Log.WriteLine("    Comment: " + capture.Comment);
                    }
                    if (capture.Adapter != null)
                    {
                        Log.WriteLine("    Adapter: " + capture.Adapter.Name);
                    }
                }

                Log.WriteLine("OmniScript QuickTest: Complete.");
                Log.Flush();
            }
        }
    }
}
