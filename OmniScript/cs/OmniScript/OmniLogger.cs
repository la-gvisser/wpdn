// =============================================================================
// <copyright file="Adapter.cs" company="LiveAction, Inc.">
//  Copyright (c) 2018-2021 Savvius, Inc. All rights reserved.
//  Copyright (c) 2015-2018 Savvius, Inc. All rights reserved.
//  Copyright (c) 2013-2015 WildPackets, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System;
    using System.IO;

    /// <summary>
    /// Logging service: to file and/or console.
    /// </summary>
    public class OmniLogger
    {
        public enum Verboseness
        {
            None,
            Critical,
            Error,
            Info,
            Debug
        }

        /// <summary>
        /// Verboseness level
        /// </summary>
        private Verboseness verbose;

        /// <summary>
        /// Log to Standard Out flag.
        /// </summary>
        private bool standardOut;

        /// <summary>
        /// Log to Standard Out flag.
        /// </summary>
        private bool timestamp;

        /// <summary>
        /// The log file writer.
        /// </summary>
        private String fileName;

        /// <summary>
        /// The log file writer.
        /// </summary>
        private StreamWriter file;

        /// <summary>
        /// Initializes a new instance of the OmniLogger class.
        /// </summary>
        public OmniLogger( Verboseness verbose = Verboseness.Error, bool standardOut = true, bool timestamp = false )
        {
            this.verbose = verbose;
            this.standardOut = standardOut;
            this.timestamp = false;
            this.fileName = null;
            this.file = null;
        }

        ~OmniLogger()
        {
            this.file = null;
        }

        /// <summary>
        /// Log a message.
        /// </summary>
        /// <param name="text">The text to log.</param>
        public void Log(String text)
        {
            if (!this.standardOut && (this.file == null))
            {
                return;
            }

            String now = "";
            if (this.timestamp)
            {
                now = DateTime.UtcNow.ToString(@"MM/dd/yyyy HH:mm:ss ");
            }
            String line = now + text;

            if (this.standardOut)
            {
                Console.WriteLine(line);
            }

            if (this.file != null)
            {
                this.file.WriteLine(line);
            }
        }

        /// <summary>
        /// Log a message.
        /// </summary>
        public void Log()
        {
        }

        /// <summary>
        /// Log messages to the file specified.
        /// </summary>
        /// <param name="fileName">The name of the file to log to.</param>
        public void LogToFile(String fileName)
        {
            if (!String.IsNullOrEmpty(fileName))
            {
                this.fileName = fileName;
                this.file = new StreamWriter(fileName);
                this.file.AutoFlush = true;
            }
        }

        /// <summary>
        /// Log a critical error message.
        /// </summary>
        /// <param name="text">The text to log.</param>
        public void Critical(String text)
        {
            if (this.verbose >= Verboseness.Critical) this.Log(text);
        }

        /// <summary>
        /// Log an error message.
        /// </summary>
        /// <param name="text">The text to log.</param>
        public void Error(String text)
        {
            if (this.verbose >= Verboseness.Error) this.Log(text);
        }

        /// <summary>
        /// Log an informational message.
        /// </summary>
        /// <param name="text">The text to log.</param>
        public void Info(String text)
        {
            if (this.verbose >= Verboseness.Info) this.Log(text);
        }

        /// <summary>
        /// Will message be logged at this level.
        /// </summary>
        /// <param name="lvel">The level to check.</param>
        public bool IsLogging(Verboseness level)
        {
            return (level >= this.verbose);
        }

        /// <summary>
        /// Log a debug message.
        /// </summary>
        /// <param name="text">The text to log.</param>
        public void Debug(String text)
        {
            if (this.verbose >= Verboseness.Debug) this.Log(text);
        }

        /// <summary>
        /// Log an informational message.
        /// </summary>
        /// <param name="text">The text to log.</param>
        public void WriteLine(String text)
        {
            this.Info(text);
        }

        /// <summary>
        /// Log an informational Writes the text representation of the 
        /// specified array of objects, followed by the current line 
        /// terminator, to the standard output stream using the specified 
        /// format information.
        /// </summary>
        /// <param name="text">The text to log.</param>
        public void WriteLine(String format, params Object[] arg)
        {
            this.Info(String.Format(format, arg));
        }
    }
}
