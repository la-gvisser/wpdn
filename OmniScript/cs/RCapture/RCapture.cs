// =============================================================================
// <copyright file="Program.cs" company="WildPackets, Inc.">
//  Copyright (c) 2013 WildPackets, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace RCapture
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Text;
    using Savvius.Omni.OmniScript;

    /// <summary>
    /// Program
    /// </summary>
    class Program
    {
        static void Main(String[] args)
        {
            CaptureManager captman = new CaptureManager();
            captman.ParseCommandLine(args);
            captman.Process();
        }
    }
}
