// =============================================================================
// <copyright file="PeekPlugin.cs" company="WildPackets, Inc.">
//  Copyright (c) 2013 WildPackets, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace OEMigrate
{
    using System;
    using System.Runtime.InteropServices;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

    static class NativeMethods
    {
        [DllImport("kernel32.dll")]
        public static extern IntPtr LoadLibrary(String dllToLoad);

        [DllImport("kernel32.dll")]
        public static extern IntPtr GetProcAddress(IntPtr hModule, String procName);

        [DllImport("kernel32.dll")]
        public static extern bool FreeLibrary(IntPtr hModule);
    }

    public class PeekPlugin
    {
        static private short msgLoad = 0;

        [StructLayoutAttribute(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Unicode)]
        public struct LoadParams
        {
            public UInt32 apiVersion;
            public UInt32 appCapabilities;
            public IntPtr name;
            public Guid id;
            public UInt16 attributes;
            public UInt16 supportedActions;
            public UInt16 defaultActions;
            public UInt32 supportedCount;
            public IntPtr supportedProtoSpecs;
            public IntPtr clientAppData;
            public IntPtr appContextData;
            public IntPtr loadErrorMessage;
            public UInt32 localeId;
            public UInt64 attributesEx;
        }
        
        [StructLayoutAttribute(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Unicode)]
        public struct LoadResults
        {
            public UInt32 apiVersion;
            public UInt32 appCapabilities;
            public IntPtr name;
            public Guid id;
        }

        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        private delegate int PlugInMain(short message, IntPtr ptr);

        public String FileName { get; set; }
        public String Name { get; set; }
        public Guid Id { get; set; }

        public PeekPlugin(String name, Guid id)
        {
            this.Name = name;
            this.Id = id;
        }

        public PeekPlugin(String filename)
        {
            this.FileName = filename;
        }

        public bool Load()
        {
            IntPtr pPlugin = NativeMethods.LoadLibrary(this.FileName);
            if (pPlugin == (IntPtr)0) return false;

            IntPtr pPlugInMain = NativeMethods.GetProcAddress(pPlugin, "PlugInMain");
            if (pPlugInMain == (IntPtr)0)
            {
                NativeMethods.FreeLibrary(pPlugin);
                return false;
            }
            
            PlugInMain pluginmain = (PlugInMain)Marshal.GetDelegateForFunctionPointer(pPlugInMain, typeof(PlugInMain));

            IntPtr name = Marshal.AllocHGlobal(Marshal.SizeOf(512));
            IntPtr errormessage = Marshal.AllocHGlobal(Marshal.SizeOf(2048));
            IntPtr protospecs = Marshal.AllocHGlobal(Marshal.SizeOf(2048));

            LoadParams args = new LoadParams();
            args.apiVersion = 12;
            args.appCapabilities = 1991;    // 0x07C7
            args.localeId = 0x0409;
            args.name = name;
            args.loadErrorMessage = errormessage;
            args.supportedProtoSpecs = protospecs;

            IntPtr ptr = Marshal.AllocHGlobal(Marshal.SizeOf(args));
            Marshal.StructureToPtr(args, ptr, false);
            
            try
            {
                // The OmniPeek plugins will GetProcAddress for all the callbacks.
                // Some of the plugins call GetAppResoucePath, since this app does not have the
                // callback the plugin will try to call through a null-pointer.
                int result = pluginmain(msgLoad, ptr);
                LoadResults outParams = (LoadResults)Marshal.PtrToStructure(ptr, typeof(LoadResults));

                this.Name = Marshal.PtrToStringAuto(outParams.name);
                this.Id = outParams.id;
            }
            catch (System.AccessViolationException ex)
            {
                Console.WriteLine(this.FileName);
                Console.WriteLine(ex.ToString());
            }
            catch (System.Exception ex)
            {
                Console.WriteLine(this.FileName);
                Console.WriteLine(ex.ToString());
            }

            args.name = (IntPtr)0;
            args.loadErrorMessage = (IntPtr)0;
            args.supportedProtoSpecs = (IntPtr)0;

            //Marshal.FreeHGlobal(name);
            //Marshal.FreeHGlobal(errormessage);
            //Marshal.FreeHGlobal(protospecs);
            //Marshal.FreeHGlobal(ptr);
            NativeMethods.FreeLibrary(pPlugin);
            return (this.Id != Guid.Empty);
        }
    }
}
