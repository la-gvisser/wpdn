namespace EmailCaptureSettings
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;

    using Savvius.Omni.OmniScript;

    class Program
    {
        public OmniScript omni;
        public OmniEngine engine;

        public String ValidFileName(String fileName)
        {
            foreach (char c in System.IO.Path.GetInvalidFileNameChars())
            {
                fileName = fileName.Replace(c, '_');
            }
            return fileName;
        }

        public void TestEmailCapture(String captureName)
        {
            Capture capture = engine.FindCapture(captureName);
            if (capture != null)
            {
                CaptureTemplate template = capture.GetCaptureTemplate();
                String filename = @"C:\Temp\Backup\" + ValidFileName(template.Name + ".xml");
                template.Store(filename, engine);

                PluginSettings settings = template.Plugins.PluginList.Find(EmailCaptureOE.ClassId);
                if (settings != null)
                {
                    // Create a new EmailCaptureOE from the existing settings.
                    EmailCaptureOE email = new EmailCaptureOE(settings);

                    // Replace the existing settings with the new object.
                    template.Plugins.PluginList.Replace(email);

                    // Save the current settings to file.
                    String filename2 = @"C:\Temp\Backup\" + ValidFileName(template.Name + "-2.xml");
                    template.Store(filename2, engine);

                    // Add a new case to the settings.
                    EmailCaptureOE.Case kase2 = new EmailCaptureOE.Case();
                    kase2.Enabled = true;
                    kase2.FullCapture = true;
                    kase2.Name = "Case 1001001";
                    kase2.Addresses.Add(new EmailCaptureOE.EmailAddress("alpha@sample.com"));
                    kase2.Addresses.Add(new EmailCaptureOE.EmailAddress("beta@sample.com"));
                    kase2.Addresses.Add(new EmailCaptureOE.EmailAddress("gamma@sample.com"));
                    kase2.Addresses.Add(new EmailCaptureOE.EmailAddress("delta@sample.com"));
                    kase2.FileNamePrefix = "73_";
                    kase2.OutputDirectory = @"C:\EmailCaptureOE\Case-2";
                    kase2.AppendEmail = false;
                    kase2.SMTP = true;
                    kase2.POP3 = true;
                    kase2.DisplayPackets = true;
                    kase2.WritePackets = false;
                    kase2.DisplayStatistics = false;
                    kase2.LogMessages = true;
                    kase2.StartTime = new PeekTime();
                    ulong duration = 7 * 24 * 60 * 60 * 1000000000L;
                    kase2.StopTime = new PeekTime(kase2.StartTime + duration);
                    kase2.FullEmail = true;
                    kase2.FullPacket = true;

                    email.Cases.Add(kase2);

                    // Save the new settings to file.
                    String filename3 = @"C:\Temp\Backup\" + ValidFileName(template.Name + "-3.xml");
                    template.Store(filename3, engine);

                    // Rename the template and create a new capture.
                    template.Name += "-2";
                    Capture existing = engine.FindCapture(template.Name);
                    while (existing != null)
                    {
                        engine.DeleteCapture(existing);
                        existing = engine.FindCapture(template.Name);
                    }
                    engine.CreateCapture(template);
                }
            }
        }

        static void Main(string[] args)
        {
            String capturename = null;

            IEnumerator e = args.GetEnumerator();
            String v;
            while (e.MoveNext())
            {
                capturename = (String)e.Current;
            }

            // localhost, default port.
            OmniScript.EngineAddress address = new OmniScript.EngineAddress();
            // current user.
            OmniScript.EngineCredentials credentials = new OmniScript.EngineCredentials();

            Program prog = new Program();

            // Create an OmniScript object.
            prog.omni = new OmniScript();

            // Create an OmniEngine object.
            prog.engine = prog.omni.CreateEngine(address);

            // Connect to the engine.
            prog.engine.Connect(credentials);

            // Test the EmailCaptureOE plugin class.
            prog.TestEmailCapture(capturename);

            // Disconnect from the engine.
            prog.engine.Disconnect();
        }
    }
}
