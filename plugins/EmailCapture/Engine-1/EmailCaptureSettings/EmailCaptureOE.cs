namespace EmailCaptureSettings
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using System.Xml.Linq;

    using Savvius.Omni.OmniScript;

    public class EmailCaptureOE
        : PluginSettings
    {
        public static String PropBooleanString(bool value)
        {
            return (value) ? "True" : "False";
        }

        public static XAttribute PropBoolean(bool value)
        {
            return new XAttribute("Enabled", PropBooleanString(value));
        }

        public static bool PropBoolean(String value)
        {
            return (String.IsNullOrEmpty(value))
                ? false
                : (value[0] == 'T' || value[0] == 't');
        }

        public static bool IsPropEnabled(XElement node)
        {
            if (node != null)
            {
                XAttribute enabled = node.Attribute("Enabled");
                if (enabled != null)
                {
                    return PropBoolean(enabled.Value);
                }
            }
            return false;
        }

        public static PeekTime ParseTimeStamp(String dateTime)
        {
            CultureInfo culture = CultureInfo.CreateSpecificCulture("en-US");
            DateTimeFormatInfo dtfi = culture.DateTimeFormat;
            dtfi.DateSeparator = "-";
            dtfi.TimeSeparator = ".";

            String cleanTime = dateTime.Replace('T', ' ');
            return new PeekTime(DateTime.Parse(cleanTime, dtfi).ToUniversalTime());
        }

        public static String FormatTimeStamp(PeekTime peekTime)
        {
            return peekTime.ToAnsiTime().ToString();
        }

        public class EmailAddress
        {
            public bool Enabled { get; set; }
            public String Address { get; set; }

            public EmailAddress(String address, bool enabled=true)
            {
                this.Enabled = enabled;
                this.Address = address;
            }

            public EmailAddress(XElement node=null)
            {
                this.Load(node);
            }

            public void Load(XElement node)
            {
                if (node == null) return;
                this.Enabled = IsPropEnabled(node);
                this.Address = node.Value;
            }

            public XElement Store()
            {
                return new XElement("Address",
                    this.Address,
                    new XAttribute("Enabled", PropBooleanString(this.Enabled)));
            }
        }

        public class AddressList
            : List<EmailAddress>
        {
            public bool CaptureAll { get; set; }

            public AddressList(XElement node=null)
            {
                this.CaptureAll = false;
                this.Load(node);
            }

            public void Add(String address)
            {
                this.Add(new EmailAddress(address, true));
            }

            public void Load(XElement node)
            {
                if (node == null) return;
                this.CaptureAll = PropBoolean(node.Attribute("CaptureAll").Value);

                IEnumerable<XElement> addresses = node.Elements("Address");
                foreach (XElement address in addresses)
                {
                    this.Add(new EmailAddress(address));
                }

            }

            public XElement Store()
            {
                XElement addresses = new XElement("AddressList",
                    new XAttribute("CaptureAll", PropBooleanString(this.CaptureAll)));
                this.ForEach(delegate(EmailAddress address)
                {
                    addresses.Add(address.Store());
                });
                return addresses;
            }
        }

        public class Case
        {
            public bool Enabled { get; set; }
            public bool FullCapture { get; set; }
            public String Name { get; set; }
            public AddressList Addresses { get; set; }
            public String FileNamePrefix { get; set; }
            public bool AppendEmail { get; set; }
            public String OutputDirectory { get; set; }
            public bool SMTP { get; set; }
            public bool POP3 { get; set; }
            public bool DisplayPackets { get; set; }
            public bool WritePackets { get; set; }
            public bool DisplayStatistics { get; set; }
            public bool LogMessages { get; set; }
            public PeekTime StartTime { get; set; }
            public PeekTime StopTime { get; set; }

            public bool PenEmail { get; set; }
            public bool PenText { get; set; }
            public bool PenPerAddress { get; set; }
            public bool PenSaveAgeEnabled { get; set; }
            public int PenSaveAge { get; set; }
            public bool PenSaveSizeEnabled { get; set; }
            public int PenSaveSize { get; set; }

            public bool FullEmail { get; set; }
            public bool FullPacket { get; set; }

            public Case(XElement node=null)
            {
                if (node == null)
                {
                    this.Name = "";
                    this.FileNamePrefix = "";
                    this.OutputDirectory = "";
                    this.Addresses = new AddressList();
                    this.StartTime = new PeekTime();
                    this.StopTime = new PeekTime();
                }
                else
                {
                    this.Load(node);
                }
            }

            public void Load(XElement node)
            {
                if (node == null) return;

                this.Enabled = IsPropEnabled(node);
                this.FullCapture = (node.Attribute("CaptureMode").Value.ToLower() == "full");
                this.Name = node.Element("Name").Value;
                this.Addresses = new AddressList(node.Element("AddressList"));
                XElement prefix = node.Element("FileNamePrefix");
                if (prefix != null)
                {
                    this.FileNamePrefix = prefix.Value;
                    this.AppendEmail = (Convert.ToInt32(prefix.Attribute("AppendEmail").Value) != 0);
                }
                this.OutputDirectory = node.Element("OutputDirectory").Value;
                this.SMTP = IsPropEnabled(node.Element("Protocols").Element("SMTP"));
                this.POP3 = IsPropEnabled(node.Element("Protocols").Element("POP3"));
                this.DisplayPackets = IsPropEnabled(node.Element("DisplayPackets"));
                this.WritePackets = IsPropEnabled(node.Element("WritePackets"));
                this.DisplayStatistics = IsPropEnabled(node.Element("DisplayStats"));
                this.LogMessages = IsPropEnabled(node.Element("LogMessages"));
                this.StartTime = ParseTimeStamp(node.Element("StartTime").Value);
                this.StopTime = ParseTimeStamp(node.Element("StopTime").Value);

                XElement pen = node.Element("Pen");
                if (pen != null)
                {
                    this.PenEmail = IsPropEnabled(pen.Element("Email"));
                    this.PenText = IsPropEnabled(pen.Element("Text"));
                    this.PenPerAddress = IsPropEnabled(pen.Element("PerAddress"));
                    XElement saveAge = pen.Element("SaveAge");
                    if (saveAge != null)
                    {
                        this.PenSaveAgeEnabled = IsPropEnabled(saveAge);
                        this.PenSaveAge = Convert.ToInt32(saveAge.Attribute("Value").Value);
                    }
                    XElement saveSize = pen.Element("SaveSize");
                    if (saveSize != null)
                    {
                        this.PenSaveSizeEnabled = IsPropEnabled(saveSize);
                        this.PenSaveSize = Convert.ToInt32(saveSize.Attribute("Value").Value);
                    }
                }

                XElement full = node.Element("Full");
                if (full != null)
                {
                    this.FullEmail = IsPropEnabled(full.Element("Email"));
                    this.FullPacket = IsPropEnabled(full.Element("Packet"));
                }
            }

            public XElement Store()
            {
                XElement protocols = new XElement("Protocols",
                    new XElement("SMTP", PropBoolean(this.SMTP)),
                    new XElement("POP3", PropBoolean(this.POP3)));
                XElement pen = new XElement("Pen",
                    new XElement("Email", PropBoolean(this.PenEmail)),
                    new XElement("Text", PropBoolean(this.PenText)),
                    new XElement("PerAddress", PropBoolean(this.PenPerAddress)),
                    new XElement("SaveAge",
                        PropBoolean(this.PenSaveAgeEnabled),
                        new XAttribute("Value", this.PenSaveAge)),
                    new XElement("SaveSize",
                        PropBoolean(this.PenSaveSizeEnabled),
                        new XAttribute("Value", this.PenSaveSize)));
                XElement full = new XElement("Full",
                    new XElement("Email", PropBoolean(this.FullEmail)),
                    new XElement("Packet", PropBoolean(this.FullPacket)));
                XElement kase = new XElement("Case",
                    PropBoolean(this.Enabled),
                    new XAttribute("CaptureMode", (this.FullCapture) ? "Full" : "Pen"),
                    new XElement("Name", this.Name),
                    this.Addresses.Store(),
                    new XElement("FileNamePrefix", this.FileNamePrefix,
                        new XAttribute("AppendEmail", (this.AppendEmail) ? "1" : "0")),
                    new XElement("OutputDirectory", this.OutputDirectory),
                    protocols,
                    new XElement("DisplayPackets", PropBoolean(this.DisplayPackets)),
                    new XElement("WritePackets", PropBoolean(this.WritePackets)),
                    new XElement("DisplayStats", PropBoolean(this.DisplayStatistics)),
                    new XElement("LogMessages", PropBoolean(this.LogMessages)),
                    pen,
                    full,
                    new XElement("StartTime", this.StartTime.ToString(@"yyyy-MM-ddTHH.mm.ss.fffZ")),
                    new XElement("StopTime", this.StopTime.ToString(@"yyyy-MM-ddTHH.mm.ss.fffZ")));

                return kase;
            }
        }

        public class CaseList
            : List<Case>
        {
            public CaseList(XElement node=null)
            {
                this.Load(node);
            }

            public void Load(XElement node)
            {
                if (node == null) return;

                // kase instead of case, because case is keyword.
                IEnumerable<XElement> kases = node.Elements("Case");
                foreach (XElement kase in kases)
                {
                    this.Add(new Case(kase));
                }
            }

            public XElement Store()
            {
                XElement cases = new XElement("CaseList");
                this.ForEach(delegate(Case kase)
                {
                    cases.Add(kase.Store());
                });
                return cases;
            }
        }

        public static OmniId ClassId { get; set; }
        public static String ClassIdName = "EmailCaptureOE";
        public static String RootName = "EmailCapture";

        static EmailCaptureOE()
        {
            EmailCaptureOE.ClassId = new OmniId("{75AC6E1C-55AD-4A5A-A2D9-4973521EF88F}");
        }

        public CaseList Cases { get; set; }

        public EmailCaptureOE(PluginSettings settings = null)
            : base(settings)
        {
            if (settings != null)
            {
                XDocument doc = XDocument.Parse(settings.Settings);
                this.Load(doc.Root);
            }
        }

        public override void Load(XElement node)
        {
            if (node == null) return;
            if (node.Name == "plugin")
            {
                base.Load(node);
                node = node.Element(RootName);
            }

            this.Cases = new CaseList(node.Element("CaseList"));
        }

        public override XElement Store()
        {
            XElement plugin = new XElement(RootName,
                new XAttribute("Version", "1"),
                this.Cases.Store());
            this.Settings = plugin.ToString(SaveOptions.DisableFormatting);

            return base.Store();
        }
    }
}
