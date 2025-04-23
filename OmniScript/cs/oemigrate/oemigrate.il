
//  Microsoft (R) .NET Framework IL Disassembler.  Version 4.0.30319.1
//  Copyright (c) Microsoft Corporation.  All rights reserved.



// Metadata version: v4.0.30319
.module extern kernel32.dll
.assembly extern mscorlib
{
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                         // .z\V.4..
  .ver 4:0:0:0
}
.assembly extern OmniScript
{
  .ver 1:0:0:0
}
.assembly oemigrate
{
  .custom instance void [mscorlib]System.Reflection.AssemblyDescriptionAttribute::.ctor(string) = ( 01 00 00 00 00 ) 
  .custom instance void [mscorlib]System.Reflection.AssemblyConfigurationAttribute::.ctor(string) = ( 01 00 00 00 00 ) 
  .custom instance void [mscorlib]System.Runtime.CompilerServices.RuntimeCompatibilityAttribute::.ctor() = ( 01 00 01 00 54 02 16 57 72 61 70 4E 6F 6E 45 78   // ....T..WrapNonEx
                                                                                                             63 65 70 74 69 6F 6E 54 68 72 6F 77 73 01 )       // ceptionThrows.
  .custom instance void [mscorlib]System.Reflection.AssemblyProductAttribute::.ctor(string) = ( 01 00 09 6F 65 6D 69 67 72 61 74 65 00 00 )       // ...oemigrate..
  .custom instance void [mscorlib]System.Reflection.AssemblyCopyrightAttribute::.ctor(string) = ( 01 00 23 43 6F 70 79 72 69 67 68 74 20 C2 A9 20   // ..#Copyright .. 
                                                                                                  57 69 6C 64 50 61 63 6B 65 74 73 2C 20 49 6E 63   // WildPackets, Inc
                                                                                                  2E 20 32 30 31 33 00 00 )                         // . 2013..
  .custom instance void [mscorlib]System.Runtime.CompilerServices.CompilationRelaxationsAttribute::.ctor(int32) = ( 01 00 08 00 00 00 00 00 ) 
  .custom instance void [mscorlib]System.Reflection.AssemblyTitleAttribute::.ctor(string) = ( 01 00 09 6F 65 6D 69 67 72 61 74 65 00 00 )       // ...oemigrate..
  .custom instance void [mscorlib]System.Reflection.AssemblyCompanyAttribute::.ctor(string) = ( 01 00 11 57 69 6C 64 50 61 63 6B 65 74 73 2C 20   // ...WildPackets, 
                                                                                                49 6E 63 2E 00 00 )                               // Inc...
  .custom instance void [mscorlib]System.Runtime.InteropServices.GuidAttribute::.ctor(string) = ( 01 00 24 30 35 35 36 38 66 34 30 2D 38 66 37 37   // ..$05568f40-8f77
                                                                                                  2D 34 64 62 65 2D 38 33 39 37 2D 31 32 62 66 30   // -4dbe-8397-12bf0
                                                                                                  63 37 30 34 39 38 30 00 00 )                      // c704980..

  // --- The following custom attribute is added automatically, do not uncomment -------
  //  .custom instance void [mscorlib]System.Diagnostics.DebuggableAttribute::.ctor(valuetype [mscorlib]System.Diagnostics.DebuggableAttribute/DebuggingModes) = ( 01 00 02 00 00 00 00 00 ) 

  .custom instance void [mscorlib]System.Runtime.InteropServices.ComVisibleAttribute::.ctor(bool) = ( 01 00 00 00 00 ) 
  .custom instance void [mscorlib]System.Reflection.AssemblyTrademarkAttribute::.ctor(string) = ( 01 00 00 00 00 ) 
  .custom instance void [mscorlib]System.Reflection.AssemblyFileVersionAttribute::.ctor(string) = ( 01 00 07 31 2E 30 2E 30 2E 30 00 00 )             // ...1.0.0.0..
  .custom instance void [mscorlib]System.Runtime.Versioning.TargetFrameworkAttribute::.ctor(string) = ( 01 00 29 2E 4E 45 54 46 72 61 6D 65 77 6F 72 6B   // ..).NETFramework
                                                                                                        2C 56 65 72 73 69 6F 6E 3D 76 34 2E 30 2C 50 72   // ,Version=v4.0,Pr
                                                                                                        6F 66 69 6C 65 3D 43 6C 69 65 6E 74 01 00 54 0E   // ofile=Client..T.
                                                                                                        14 46 72 61 6D 65 77 6F 72 6B 44 69 73 70 6C 61   // .FrameworkDispla
                                                                                                        79 4E 61 6D 65 1F 2E 4E 45 54 20 46 72 61 6D 65   // yName..NET Frame
                                                                                                        77 6F 72 6B 20 34 20 43 6C 69 65 6E 74 20 50 72   // work 4 Client Pr
                                                                                                        6F 66 69 6C 65 )                                  // ofile
  .hash algorithm 0x00008004
  .ver 1:0:0:0
}
.module oemigrate.exe
// MVID: {331B1D5B-D9CD-45BB-B1FA-272A0CE64B2A}
.imagebase 0x00400000
.file alignment 0x00000200
.stackreserve 0x00100000
.subsystem 0x0003       // WINDOWS_CUI
.corflags 0x00000003    //  ILONLY 32BITREQUIRED

// To export GetAppResourcePathCallback add the next two uncommented lines.
// And the two commented lines below to where they are uncommented
// in this file. (They are included here to keep the changes in one place.)
.vtfixup [1] int32 fromunmanaged at VT_01
.data VT_01 = int32(0)

//.vtentry 1 : 1
//.export [1] as GetAppResourcePathCallback


// Image base: 0x00270000


// =============== CLASS MEMBERS DECLARATION ===================

.class private auto ansi beforefieldinit OEMigrate.Program
       extends [mscorlib]System.Object
{
  .method private hidebysig static string 
          GetProgramFilesX86() cil managed
  {
    // Code size       49 (0x31)
    .maxstack  2
    .locals init ([0] string progFiles,
             [1] string progFilesX86)
    IL_0000:  ldstr      "ProgramFiles"
    IL_0005:  call       string [mscorlib]System.Environment::GetEnvironmentVariable(string)
    IL_000a:  stloc.0
    IL_000b:  ldstr      "ProgramFiles(x86)"
    IL_0010:  call       string [mscorlib]System.Environment::GetEnvironmentVariable(string)
    IL_0015:  stloc.1
    IL_0016:  ldloc.1
    IL_0017:  brtrue.s   IL_001b

    IL_0019:  ldloc.0
    IL_001a:  ret

    IL_001b:  ldloc.0
    IL_001c:  ldloc.1
    IL_001d:  call       bool [mscorlib]System.String::op_Equality(string,
                                                                   string)
    IL_0022:  brfalse.s  IL_002f

    IL_0024:  ldstr      "ProgramW6432"
    IL_0029:  call       string [mscorlib]System.Environment::GetEnvironmentVariable(string)
    IL_002e:  stloc.0
    IL_002f:  ldloc.1
    IL_0030:  ret
  } // end of method Program::GetProgramFilesX86

  .method private hidebysig static class OEMigrate.PeekPluginList 
          GetPeekPlugins(string path) cil managed
  {
    // Code size       91 (0x5b)
    .maxstack  2
    .locals init ([0] string currentPath,
             [1] class OEMigrate.PeekPluginList pluginList,
             [2] string[] files,
             [3] string file,
             [4] class OEMigrate.PeekPlugin plugin,
             [5] string[] CS$6$0000,
             [6] int32 CS$7$0001)
    IL_0000:  call       string [mscorlib]System.IO.Directory::GetCurrentDirectory()
    IL_0005:  stloc.0
    IL_0006:  ldarg.0
    IL_0007:  call       void [mscorlib]System.IO.Directory::SetCurrentDirectory(string)
    IL_000c:  newobj     instance void OEMigrate.PeekPluginList::.ctor()
    IL_0011:  stloc.1
    IL_0012:  ldarg.0
    IL_0013:  ldstr      "*.dll"
    IL_0018:  call       string[] [mscorlib]System.IO.Directory::GetFiles(string,
                                                                          string)
    IL_001d:  stloc.2
    IL_001e:  ldloc.2
    IL_001f:  stloc.s    CS$6$0000
    IL_0021:  ldc.i4.0
    IL_0022:  stloc.s    CS$7$0001
    IL_0024:  br.s       IL_004b

    IL_0026:  ldloc.s    CS$6$0000
    IL_0028:  ldloc.s    CS$7$0001
    IL_002a:  ldelem.ref
    IL_002b:  stloc.3
    IL_002c:  ldloc.3
    IL_002d:  newobj     instance void OEMigrate.PeekPlugin::.ctor(string)
    IL_0032:  stloc.s    plugin
    IL_0034:  ldloc.s    plugin
    IL_0036:  callvirt   instance bool OEMigrate.PeekPlugin::Load()
    IL_003b:  brfalse.s  IL_0045

    IL_003d:  ldloc.1
    IL_003e:  ldloc.s    plugin
    IL_0040:  callvirt   instance void class [mscorlib]System.Collections.Generic.List`1<class OEMigrate.PeekPlugin>::Add(!0)
    IL_0045:  ldloc.s    CS$7$0001
    IL_0047:  ldc.i4.1
    IL_0048:  add
    IL_0049:  stloc.s    CS$7$0001
    IL_004b:  ldloc.s    CS$7$0001
    IL_004d:  ldloc.s    CS$6$0000
    IL_004f:  ldlen
    IL_0050:  conv.i4
    IL_0051:  blt.s      IL_0026

    IL_0053:  ldloc.0
    IL_0054:  call       void [mscorlib]System.IO.Directory::SetCurrentDirectory(string)
    IL_0059:  ldloc.1
    IL_005a:  ret
  } // end of method Program::GetPeekPlugins

  .method private hidebysig static class [OmniScript]WildPackets.Omni.OmniScript.Transform/GuidMap 
          GetPluginGuidMap(class OEMigrate.PeekPluginList peekPlugins,
                           class [OmniScript]WildPackets.Omni.OmniScript.OmniPluginList enginePlugins) cil managed
  {
    // Code size       8 (0x8)
    .maxstack  1
    .locals init ([0] class [OmniScript]WildPackets.Omni.OmniScript.Transform/GuidMap map)
    IL_0000:  newobj     instance void [OmniScript]WildPackets.Omni.OmniScript.Transform/GuidMap::.ctor()
    IL_0005:  stloc.0
    IL_0006:  ldloc.0
    IL_0007:  ret
  } // end of method Program::GetPluginGuidMap

  .method private hidebysig static void  Main(string[] args) cil managed
  {
    .entrypoint
    // Code size       493 (0x1ed)
    .maxstack  5
    .locals init ([0] string str,
             [1] string progFilesX86,
             [2] string omnipeekFolder,
             [3] string peekFiltersPath,
             [4] class [OmniScript]WildPackets.Omni.OmniScript.ConsoleFilterList peekFilters,
             [5] string peekPluginsPath,
             [6] class OEMigrate.PeekPluginList peekPlugins,
             [7] class OEMigrate.PeekPlugin plugin,
             [8] string appData,
             [9] string peekFolder,
             [10] string peekCapturesPath,
             [11] class [OmniScript]WildPackets.Omni.OmniScript.ConsoleTemplateList peekCaptures,
             [12] string userFiltersPath,
             [13] class [OmniScript]WildPackets.Omni.OmniScript.ConsoleFilterList userFilters,
             [14] class [OmniScript]WildPackets.Omni.OmniScript.ConsoleTemplate capture,
             [15] string[] CS$6$0000,
             [16] int32 CS$7$0001,
             [17] int32 CS$0$0002,
             [18] valuetype [mscorlib]System.Collections.Generic.List`1/Enumerator<class OEMigrate.PeekPlugin> CS$5$0003,
             [19] valuetype [mscorlib]System.Guid CS$0$0004,
             [20] int32 CS$0$0005,
             [21] valuetype [mscorlib]System.Collections.Generic.List`1/Enumerator<class [OmniScript]WildPackets.Omni.OmniScript.ConsoleTemplate> CS$5$0006,
             [22] int32 CS$0$0007,
             [23] int32 CS$0$0008)
    IL_0000:  ldarg.0
    IL_0001:  stloc.s    CS$6$0000
    IL_0003:  ldc.i4.0
    IL_0004:  stloc.s    CS$7$0001
    IL_0006:  br.s       IL_0024

    IL_0008:  ldloc.s    CS$6$0000
    IL_000a:  ldloc.s    CS$7$0001
    IL_000c:  ldelem.ref
    IL_000d:  stloc.0
    IL_000e:  ldstr      "oeMigrate"
    IL_0013:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0018:  ldloc.0
    IL_0019:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_001e:  ldloc.s    CS$7$0001
    IL_0020:  ldc.i4.1
    IL_0021:  add
    IL_0022:  stloc.s    CS$7$0001
    IL_0024:  ldloc.s    CS$7$0001
    IL_0026:  ldloc.s    CS$6$0000
    IL_0028:  ldlen
    IL_0029:  conv.i4
    IL_002a:  blt.s      IL_0008

    IL_002c:  call       string OEMigrate.Program::GetProgramFilesX86()
    IL_0031:  stloc.1
    IL_0032:  ldloc.1
    IL_0033:  ldstr      "\\WildPackets\\OmniPeek"
    IL_0038:  call       string [mscorlib]System.String::Concat(string,
                                                                string)
    IL_003d:  stloc.2
    IL_003e:  ldloc.2
    IL_003f:  ldstr      "\\1033\\Filters\\default.flt"
    IL_0044:  call       string [mscorlib]System.String::Concat(string,
                                                                string)
    IL_0049:  stloc.3
    IL_004a:  ldloc.3
    IL_004b:  newobj     instance void [OmniScript]WildPackets.Omni.OmniScript.ConsoleFilterList::.ctor(string)
    IL_0050:  stloc.s    peekFilters
    IL_0052:  ldloc.2
    IL_0053:  ldstr      "\\Plugins"
    IL_0058:  call       string [mscorlib]System.String::Concat(string,
                                                                string)
    IL_005d:  stloc.s    peekPluginsPath
    IL_005f:  ldstr      "C:\\wp\\eng\\private\\gary\\main\\src\\omni\\peek\\Debug\\Pl"
    + "ugins"
    IL_0064:  stloc.s    peekPluginsPath
    IL_0066:  ldloc.s    peekPluginsPath
    IL_0068:  call       class OEMigrate.PeekPluginList OEMigrate.Program::GetPeekPlugins(string)
    IL_006d:  stloc.s    peekPlugins
    IL_006f:  ldstr      "Found "
    IL_0074:  ldloc.s    peekPlugins
    IL_0076:  callvirt   instance int32 class [mscorlib]System.Collections.Generic.List`1<class OEMigrate.PeekPlugin>::get_Count()
    IL_007b:  stloc.s    CS$0$0002
    IL_007d:  ldloca.s   CS$0$0002
    IL_007f:  call       instance string [mscorlib]System.Int32::ToString()
    IL_0084:  ldstr      " Peek plugins."
    IL_0089:  call       string [mscorlib]System.String::Concat(string,
                                                                string,
                                                                string)
    IL_008e:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0093:  ldloc.s    peekPlugins
    IL_0095:  callvirt   instance valuetype [mscorlib]System.Collections.Generic.List`1/Enumerator<!0> class [mscorlib]System.Collections.Generic.List`1<class OEMigrate.PeekPlugin>::GetEnumerator()
    IL_009a:  stloc.s    CS$5$0003
    .try
    {
      IL_009c:  br.s       IL_00d7

      IL_009e:  ldloca.s   CS$5$0003
      IL_00a0:  call       instance !0 valuetype [mscorlib]System.Collections.Generic.List`1/Enumerator<class OEMigrate.PeekPlugin>::get_Current()
      IL_00a5:  stloc.s    plugin
      IL_00a7:  ldstr      "  "
      IL_00ac:  ldloc.s    plugin
      IL_00ae:  callvirt   instance string OEMigrate.PeekPlugin::get_Name()
      IL_00b3:  ldstr      " : "
      IL_00b8:  ldloc.s    plugin
      IL_00ba:  callvirt   instance valuetype [mscorlib]System.Guid OEMigrate.PeekPlugin::get_Id()
      IL_00bf:  stloc.s    CS$0$0004
      IL_00c1:  ldloca.s   CS$0$0004
      IL_00c3:  ldstr      "B"
      IL_00c8:  call       instance string [mscorlib]System.Guid::ToString(string)
      IL_00cd:  call       string [mscorlib]System.String::Concat(string,
                                                                  string,
                                                                  string,
                                                                  string)
      IL_00d2:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_00d7:  ldloca.s   CS$5$0003
      IL_00d9:  call       instance bool valuetype [mscorlib]System.Collections.Generic.List`1/Enumerator<class OEMigrate.PeekPlugin>::MoveNext()
      IL_00de:  brtrue.s   IL_009e

      IL_00e0:  leave.s    IL_00f0

    }  // end .try
    finally
    {
      IL_00e2:  ldloca.s   CS$5$0003
      IL_00e4:  constrained. valuetype [mscorlib]System.Collections.Generic.List`1/Enumerator<class OEMigrate.PeekPlugin>
      IL_00ea:  callvirt   instance void [mscorlib]System.IDisposable::Dispose()
      IL_00ef:  endfinally
    }  // end handler
    IL_00f0:  ldstr      "APPDATA"
    IL_00f5:  call       string [mscorlib]System.Environment::GetEnvironmentVariable(string)
    IL_00fa:  stloc.s    appData
    IL_00fc:  ldloc.s    appData
    IL_00fe:  ldstr      "\\WildPackets\\OmniPeek"
    IL_0103:  call       string [mscorlib]System.String::Concat(string,
                                                                string)
    IL_0108:  stloc.s    peekFolder
    IL_010a:  ldloc.s    peekFolder
    IL_010c:  ldstr      "\\CaptureSettings.xml"
    IL_0111:  call       string [mscorlib]System.String::Concat(string,
                                                                string)
    IL_0116:  stloc.s    peekCapturesPath
    IL_0118:  ldloc.s    peekCapturesPath
    IL_011a:  newobj     instance void [OmniScript]WildPackets.Omni.OmniScript.ConsoleTemplateList::.ctor(string)
    IL_011f:  stloc.s    peekCaptures
    IL_0121:  ldloc.s    peekFolder
    IL_0123:  ldstr      "\\Filters.flt"
    IL_0128:  call       string [mscorlib]System.String::Concat(string,
                                                                string)
    IL_012d:  stloc.s    userFiltersPath
    IL_012f:  ldloc.s    userFiltersPath
    IL_0131:  newobj     instance void [OmniScript]WildPackets.Omni.OmniScript.ConsoleFilterList::.ctor(string)
    IL_0136:  stloc.s    userFilters
    IL_0138:  ldstr      "Found "
    IL_013d:  ldloc.s    peekCaptures
    IL_013f:  callvirt   instance int32 class [mscorlib]System.Collections.Generic.List`1<class [OmniScript]WildPackets.Omni.OmniScript.ConsoleTemplate>::get_Count()
    IL_0144:  stloc.s    CS$0$0005
    IL_0146:  ldloca.s   CS$0$0005
    IL_0148:  call       instance string [mscorlib]System.Int32::ToString()
    IL_014d:  ldstr      " templates in "
    IL_0152:  ldloc.s    peekCapturesPath
    IL_0154:  call       string [mscorlib]System.String::Concat(string,
                                                                string,
                                                                string,
                                                                string)
    IL_0159:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_015e:  ldloc.s    peekCaptures
    IL_0160:  callvirt   instance valuetype [mscorlib]System.Collections.Generic.List`1/Enumerator<!0> class [mscorlib]System.Collections.Generic.List`1<class [OmniScript]WildPackets.Omni.OmniScript.ConsoleTemplate>::GetEnumerator()
    IL_0165:  stloc.s    CS$5$0006
    .try
    {
      IL_0167:  br.s       IL_0188

      IL_0169:  ldloca.s   CS$5$0006
      IL_016b:  call       instance !0 valuetype [mscorlib]System.Collections.Generic.List`1/Enumerator<class [OmniScript]WildPackets.Omni.OmniScript.ConsoleTemplate>::get_Current()
      IL_0170:  stloc.s    capture
      IL_0172:  ldstr      "  "
      IL_0177:  ldloc.s    capture
      IL_0179:  callvirt   instance string [OmniScript]WildPackets.Omni.OmniScript.ConsoleTemplate::get_CaptureName()
      IL_017e:  call       string [mscorlib]System.String::Concat(string,
                                                                  string)
      IL_0183:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_0188:  ldloca.s   CS$5$0006
      IL_018a:  call       instance bool valuetype [mscorlib]System.Collections.Generic.List`1/Enumerator<class [OmniScript]WildPackets.Omni.OmniScript.ConsoleTemplate>::MoveNext()
      IL_018f:  brtrue.s   IL_0169

      IL_0191:  leave.s    IL_01a1

    }  // end .try
    finally
    {
      IL_0193:  ldloca.s   CS$5$0006
      IL_0195:  constrained. valuetype [mscorlib]System.Collections.Generic.List`1/Enumerator<class [OmniScript]WildPackets.Omni.OmniScript.ConsoleTemplate>
      IL_019b:  callvirt   instance void [mscorlib]System.IDisposable::Dispose()
      IL_01a0:  endfinally
    }  // end handler
    IL_01a1:  ldstr      "Found "
    IL_01a6:  ldloc.s    peekFilters
    IL_01a8:  callvirt   instance int32 class [mscorlib]System.Collections.Generic.List`1<class [OmniScript]WildPackets.Omni.OmniScript.ConsoleFilter>::get_Count()
    IL_01ad:  stloc.s    CS$0$0007
    IL_01af:  ldloca.s   CS$0$0007
    IL_01b1:  call       instance string [mscorlib]System.Int32::ToString()
    IL_01b6:  ldstr      " default filters in "
    IL_01bb:  ldloc.3
    IL_01bc:  call       string [mscorlib]System.String::Concat(string,
                                                                string,
                                                                string,
                                                                string)
    IL_01c1:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_01c6:  ldstr      "Found "
    IL_01cb:  ldloc.s    userFilters
    IL_01cd:  callvirt   instance int32 class [mscorlib]System.Collections.Generic.List`1<class [OmniScript]WildPackets.Omni.OmniScript.ConsoleFilter>::get_Count()
    IL_01d2:  stloc.s    CS$0$0008
    IL_01d4:  ldloca.s   CS$0$0008
    IL_01d6:  call       instance string [mscorlib]System.Int32::ToString()
    IL_01db:  ldstr      " filters in "
    IL_01e0:  ldloc.s    userFiltersPath
    IL_01e2:  call       string [mscorlib]System.String::Concat(string,
                                                                string,
                                                                string,
                                                                string)
    IL_01e7:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_01ec:  ret
  } // end of method Program::Main

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method Program::.ctor

} // end of class OEMigrate.Program

.class private abstract auto ansi sealed beforefieldinit OEMigrate.NativeMethods
       extends [mscorlib]System.Object
{
  .method public hidebysig static pinvokeimpl("kernel32.dll" winapi) 
          native int  LoadLibrary(string dllToLoad) cil managed preservesig
  {
  }
  .method public hidebysig static pinvokeimpl("kernel32.dll" winapi) 
          native int  GetProcAddress(native int hModule,
                                     string procName) cil managed preservesig
  {
  }
  .method public hidebysig static pinvokeimpl("kernel32.dll" winapi) 
          bool  FreeLibrary(native int hModule) cil managed preservesig
  {
  }
} // end of class OEMigrate.NativeMethods

.class public auto ansi beforefieldinit OEMigrate.PeekPlugin
       extends [mscorlib]System.Object
{
  .class sequential unicode sealed nested public beforefieldinit LoadParams
         extends [mscorlib]System.ValueType
  {
    .pack 1
    .size 0
    .field public uint32 apiVersion
    .field public uint32 appCapabilities
    .field public uint16[] name
    .field public valuetype [mscorlib]System.Guid id
    .field public uint16 attributes
    .field public uint16 supportedActions
    .field public uint16 defaultActions
    .field public uint32 supportedCount
    .field public native int supportedProtoSpecs
    .field public native int clientAppData
    .field public native int appContextData
    .field public native int loadErrorMessage
    .field public uint32 localeId
    .field public uint64 attributesEx
  } // end of class LoadParams

  .class sequential unicode sealed nested public beforefieldinit LoadResults
         extends [mscorlib]System.ValueType
  {
    .pack 1
    .size 0
    .field public uint32 apiVersion
    .field public uint32 appCapabilities
    .field public native int name
    .field public valuetype [mscorlib]System.Guid id
  } // end of class LoadResults

  .class auto ansi sealed nested private PlugInMain
         extends [mscorlib]System.MulticastDelegate
  {
    .custom instance void [mscorlib]System.Runtime.InteropServices.UnmanagedFunctionPointerAttribute::.ctor(valuetype [mscorlib]System.Runtime.InteropServices.CallingConvention) = ( 01 00 01 00 00 00 00 00 ) 
    .method public hidebysig specialname rtspecialname 
            instance void  .ctor(object 'object',
                                 native int 'method') runtime managed
    {
    } // end of method PlugInMain::.ctor

    .method public hidebysig newslot virtual 
            instance int32  Invoke(int16 message,
                                   native int ptr) runtime managed
    {
    } // end of method PlugInMain::Invoke

    .method public hidebysig newslot virtual 
            instance class [mscorlib]System.IAsyncResult 
            BeginInvoke(int16 message,
                        native int ptr,
                        class [mscorlib]System.AsyncCallback callback,
                        object 'object') runtime managed
    {
    } // end of method PlugInMain::BeginInvoke

    .method public hidebysig newslot virtual 
            instance int32  EndInvoke(class [mscorlib]System.IAsyncResult result) runtime managed
    {
    } // end of method PlugInMain::EndInvoke

  } // end of class PlugInMain

  .field private static int16 msgLoad
  .field private string '<FileName>k__BackingField'
  .custom instance void [mscorlib]System.Runtime.CompilerServices.CompilerGeneratedAttribute::.ctor() = ( 01 00 00 00 ) 
  .field private string '<Name>k__BackingField'
  .custom instance void [mscorlib]System.Runtime.CompilerServices.CompilerGeneratedAttribute::.ctor() = ( 01 00 00 00 ) 
  .field private valuetype [mscorlib]System.Guid '<Id>k__BackingField'
  .custom instance void [mscorlib]System.Runtime.CompilerServices.CompilerGeneratedAttribute::.ctor() = ( 01 00 00 00 ) 
  .method public hidebysig specialname instance string 
          get_FileName() cil managed
  {
    .custom instance void [mscorlib]System.Runtime.CompilerServices.CompilerGeneratedAttribute::.ctor() = ( 01 00 00 00 ) 
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldfld      string OEMigrate.PeekPlugin::'<FileName>k__BackingField'
    IL_0006:  ret
  } // end of method PeekPlugin::get_FileName

  .method public hidebysig specialname instance void 
          set_FileName(string 'value') cil managed
  {
    .custom instance void [mscorlib]System.Runtime.CompilerServices.CompilerGeneratedAttribute::.ctor() = ( 01 00 00 00 ) 
    // Code size       8 (0x8)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      string OEMigrate.PeekPlugin::'<FileName>k__BackingField'
    IL_0007:  ret
  } // end of method PeekPlugin::set_FileName

  .method public hidebysig specialname instance string 
          get_Name() cil managed
  {
    .custom instance void [mscorlib]System.Runtime.CompilerServices.CompilerGeneratedAttribute::.ctor() = ( 01 00 00 00 ) 
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldfld      string OEMigrate.PeekPlugin::'<Name>k__BackingField'
    IL_0006:  ret
  } // end of method PeekPlugin::get_Name

  .method public hidebysig specialname instance void 
          set_Name(string 'value') cil managed
  {
    .custom instance void [mscorlib]System.Runtime.CompilerServices.CompilerGeneratedAttribute::.ctor() = ( 01 00 00 00 ) 
    // Code size       8 (0x8)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      string OEMigrate.PeekPlugin::'<Name>k__BackingField'
    IL_0007:  ret
  } // end of method PeekPlugin::set_Name

  .method public hidebysig specialname instance valuetype [mscorlib]System.Guid 
          get_Id() cil managed
  {
    .custom instance void [mscorlib]System.Runtime.CompilerServices.CompilerGeneratedAttribute::.ctor() = ( 01 00 00 00 ) 
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldfld      valuetype [mscorlib]System.Guid OEMigrate.PeekPlugin::'<Id>k__BackingField'
    IL_0006:  ret
  } // end of method PeekPlugin::get_Id

  .method public hidebysig specialname instance void 
          set_Id(valuetype [mscorlib]System.Guid 'value') cil managed
  {
    .custom instance void [mscorlib]System.Runtime.CompilerServices.CompilerGeneratedAttribute::.ctor() = ( 01 00 00 00 ) 
    // Code size       8 (0x8)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldarg.1
    IL_0002:  stfld      valuetype [mscorlib]System.Guid OEMigrate.PeekPlugin::'<Id>k__BackingField'
    IL_0007:  ret
  } // end of method PeekPlugin::set_Id

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(string filename) cil managed
  {
    // Code size       14 (0xe)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ldarg.0
    IL_0007:  ldarg.1
    IL_0008:  call       instance void OEMigrate.PeekPlugin::set_FileName(string)
    IL_000d:  ret
  } // end of method PeekPlugin::.ctor

  .method public hidebysig instance bool 
          Load() cil managed
  {
    // Code size       435 (0x1b3)
    .maxstack  3
    .locals init ([0] native int pPlugin,
             [1] native int pPlugInMain,
             [2] class OEMigrate.PeekPlugin/PlugInMain pluginmain,
             [3] valuetype OEMigrate.PeekPlugin/LoadParams args,
             [4] native int ptr,
             [5] valuetype OEMigrate.PeekPlugin/LoadResults outParams,
             [6] class [mscorlib]System.AccessViolationException ex,
             [7] class [mscorlib]System.Exception V_7)
    IL_0000:  ldarg.0
    IL_0001:  call       instance string OEMigrate.PeekPlugin::get_FileName()
    IL_0006:  call       native int OEMigrate.NativeMethods::LoadLibrary(string)
    IL_000b:  stloc.0
    IL_000c:  ldloc.0
    IL_000d:  ldc.i4.0
    IL_000e:  call       native int [mscorlib]System.IntPtr::op_Explicit(int32)
    IL_0013:  call       bool [mscorlib]System.IntPtr::op_Equality(native int,
                                                                   native int)
    IL_0018:  brfalse.s  IL_001c

    IL_001a:  ldc.i4.0
    IL_001b:  ret

    IL_001c:  ldloc.0
    IL_001d:  ldstr      "PlugInMain"
    IL_0022:  call       native int OEMigrate.NativeMethods::GetProcAddress(native int,
                                                                            string)
    IL_0027:  stloc.1
    IL_0028:  ldloc.1
    IL_0029:  ldc.i4.0
    IL_002a:  call       native int [mscorlib]System.IntPtr::op_Explicit(int32)
    IL_002f:  call       bool [mscorlib]System.IntPtr::op_Equality(native int,
                                                                   native int)
    IL_0034:  brfalse.s  IL_003f

    IL_0036:  ldloc.0
    IL_0037:  call       bool OEMigrate.NativeMethods::FreeLibrary(native int)
    IL_003c:  pop
    IL_003d:  ldc.i4.0
    IL_003e:  ret

    IL_003f:  ldloc.1
    IL_0040:  ldtoken    OEMigrate.PeekPlugin/PlugInMain
    IL_0045:  call       class [mscorlib]System.Type [mscorlib]System.Type::GetTypeFromHandle(valuetype [mscorlib]System.RuntimeTypeHandle)
    IL_004a:  call       class [mscorlib]System.Delegate [mscorlib]System.Runtime.InteropServices.Marshal::GetDelegateForFunctionPointer(native int,
                                                                                                                                         class [mscorlib]System.Type)
    IL_004f:  castclass  OEMigrate.PeekPlugin/PlugInMain
    IL_0054:  stloc.2
    IL_0055:  ldloca.s   args
    IL_0057:  initobj    OEMigrate.PeekPlugin/LoadParams
    IL_005d:  ldloca.s   args
    IL_005f:  ldc.i4.s   12
    IL_0061:  stfld      uint32 OEMigrate.PeekPlugin/LoadParams::apiVersion
    IL_0066:  ldloca.s   args
    IL_0068:  ldc.i4     0x7c7
    IL_006d:  stfld      uint32 OEMigrate.PeekPlugin/LoadParams::appCapabilities
    IL_0072:  ldloca.s   args
    IL_0074:  ldc.i4     0x409
    IL_0079:  stfld      uint32 OEMigrate.PeekPlugin/LoadParams::localeId
    IL_007e:  ldloca.s   args
    IL_0080:  ldc.i4     0x100
    IL_0085:  newarr     [mscorlib]System.UInt16
    IL_008a:  stfld      uint16[] OEMigrate.PeekPlugin/LoadParams::name
    IL_008f:  ldloca.s   args
    IL_0091:  ldc.i4     0x800
    IL_0096:  box        [mscorlib]System.Int32
    IL_009b:  call       int32 [mscorlib]System.Runtime.InteropServices.Marshal::SizeOf(object)
    IL_00a0:  call       native int [mscorlib]System.Runtime.InteropServices.Marshal::AllocHGlobal(int32)
    IL_00a5:  stfld      native int OEMigrate.PeekPlugin/LoadParams::loadErrorMessage
    IL_00aa:  ldloca.s   args
    IL_00ac:  ldc.i4     0x800
    IL_00b1:  box        [mscorlib]System.Int32
    IL_00b6:  call       int32 [mscorlib]System.Runtime.InteropServices.Marshal::SizeOf(object)
    IL_00bb:  call       native int [mscorlib]System.Runtime.InteropServices.Marshal::AllocHGlobal(int32)
    IL_00c0:  stfld      native int OEMigrate.PeekPlugin/LoadParams::supportedProtoSpecs
    IL_00c5:  ldloc.3
    IL_00c6:  box        OEMigrate.PeekPlugin/LoadParams
    IL_00cb:  call       int32 [mscorlib]System.Runtime.InteropServices.Marshal::SizeOf(object)
    IL_00d0:  call       native int [mscorlib]System.Runtime.InteropServices.Marshal::AllocHGlobal(int32)
    IL_00d5:  stloc.s    ptr
    IL_00d7:  ldloc.3
    IL_00d8:  box        OEMigrate.PeekPlugin/LoadParams
    IL_00dd:  ldloc.s    ptr
    IL_00df:  ldc.i4.0
    IL_00e0:  call       void [mscorlib]System.Runtime.InteropServices.Marshal::StructureToPtr(object,
                                                                                               native int,
                                                                                               bool)
    .try
    {
      IL_00e5:  ldloc.2
      IL_00e6:  ldsfld     int16 OEMigrate.PeekPlugin::msgLoad
      IL_00eb:  ldloc.s    ptr
      IL_00ed:  callvirt   instance int32 OEMigrate.PeekPlugin/PlugInMain::Invoke(int16,
                                                                                  native int)
      IL_00f2:  pop
      IL_00f3:  ldloc.s    ptr
      IL_00f5:  ldtoken    OEMigrate.PeekPlugin/LoadResults
      IL_00fa:  call       class [mscorlib]System.Type [mscorlib]System.Type::GetTypeFromHandle(valuetype [mscorlib]System.RuntimeTypeHandle)
      IL_00ff:  call       object [mscorlib]System.Runtime.InteropServices.Marshal::PtrToStructure(native int,
                                                                                                   class [mscorlib]System.Type)
      IL_0104:  unbox.any  OEMigrate.PeekPlugin/LoadResults
      IL_0109:  stloc.s    outParams
      IL_010b:  ldarg.0
      IL_010c:  ldloca.s   outParams
      IL_010e:  ldfld      native int OEMigrate.PeekPlugin/LoadResults::name
      IL_0113:  call       string [mscorlib]System.Runtime.InteropServices.Marshal::PtrToStringAuto(native int)
      IL_0118:  call       instance void OEMigrate.PeekPlugin::set_Name(string)
      IL_011d:  ldarg.0
      IL_011e:  ldloca.s   outParams
      IL_0120:  ldfld      valuetype [mscorlib]System.Guid OEMigrate.PeekPlugin/LoadResults::id
      IL_0125:  call       instance void OEMigrate.PeekPlugin::set_Id(valuetype [mscorlib]System.Guid)
      IL_012a:  leave.s    IL_0162

    }  // end .try
    catch [mscorlib]System.AccessViolationException 
    {
      IL_012c:  stloc.s    ex
      IL_012e:  ldarg.0
      IL_012f:  call       instance string OEMigrate.PeekPlugin::get_FileName()
      IL_0134:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_0139:  ldloc.s    ex
      IL_013b:  callvirt   instance string [mscorlib]System.Object::ToString()
      IL_0140:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_0145:  leave.s    IL_0162

    }  // end handler
    catch [mscorlib]System.Exception 
    {
      IL_0147:  stloc.s    V_7
      IL_0149:  ldarg.0
      IL_014a:  call       instance string OEMigrate.PeekPlugin::get_FileName()
      IL_014f:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_0154:  ldloc.s    V_7
      IL_0156:  callvirt   instance string [mscorlib]System.Object::ToString()
      IL_015b:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_0160:  leave.s    IL_0162

    }  // end handler
    IL_0162:  ldloca.s   args
    IL_0164:  ldfld      native int OEMigrate.PeekPlugin/LoadParams::loadErrorMessage
    IL_0169:  call       void [mscorlib]System.Runtime.InteropServices.Marshal::FreeHGlobal(native int)
    IL_016e:  ldloca.s   args
    IL_0170:  ldc.i4.0
    IL_0171:  call       native int [mscorlib]System.IntPtr::op_Explicit(int32)
    IL_0176:  stfld      native int OEMigrate.PeekPlugin/LoadParams::loadErrorMessage
    IL_017b:  ldloca.s   args
    IL_017d:  ldfld      native int OEMigrate.PeekPlugin/LoadParams::supportedProtoSpecs
    IL_0182:  call       void [mscorlib]System.Runtime.InteropServices.Marshal::FreeHGlobal(native int)
    IL_0187:  ldloca.s   args
    IL_0189:  ldc.i4.0
    IL_018a:  call       native int [mscorlib]System.IntPtr::op_Explicit(int32)
    IL_018f:  stfld      native int OEMigrate.PeekPlugin/LoadParams::supportedProtoSpecs
    IL_0194:  ldloc.s    ptr
    IL_0196:  call       void [mscorlib]System.Runtime.InteropServices.Marshal::FreeHGlobal(native int)
    IL_019b:  ldloc.0
    IL_019c:  call       bool OEMigrate.NativeMethods::FreeLibrary(native int)
    IL_01a1:  pop
    IL_01a2:  ldarg.0
    IL_01a3:  call       instance valuetype [mscorlib]System.Guid OEMigrate.PeekPlugin::get_Id()
    IL_01a8:  ldsfld     valuetype [mscorlib]System.Guid [mscorlib]System.Guid::Empty
    IL_01ad:  call       bool [mscorlib]System.Guid::op_Inequality(valuetype [mscorlib]System.Guid,
                                                                   valuetype [mscorlib]System.Guid)
    IL_01b2:  ret
  } // end of method PeekPlugin::Load

  .method private hidebysig specialname rtspecialname static 
          void  .cctor() cil managed
  {
    // Code size       1 (0x1)
    .maxstack  8
    IL_0000:  ret
  } // end of method PeekPlugin::.cctor

  .property instance string FileName()
  {
    .get instance string OEMigrate.PeekPlugin::get_FileName()
    .set instance void OEMigrate.PeekPlugin::set_FileName(string)
  } // end of property PeekPlugin::FileName
  .property instance string Name()
  {
    .get instance string OEMigrate.PeekPlugin::get_Name()
    .set instance void OEMigrate.PeekPlugin::set_Name(string)
  } // end of property PeekPlugin::Name
  .property instance valuetype [mscorlib]System.Guid
          Id()
  {
    .get instance valuetype [mscorlib]System.Guid OEMigrate.PeekPlugin::get_Id()
    .set instance void OEMigrate.PeekPlugin::set_Id(valuetype [mscorlib]System.Guid)
  } // end of property PeekPlugin::Id
} // end of class OEMigrate.PeekPlugin

.class private auto ansi beforefieldinit OEMigrate.PeekPluginList
       extends class [mscorlib]System.Collections.Generic.List`1<class OEMigrate.PeekPlugin>
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void class [mscorlib]System.Collections.Generic.List`1<class OEMigrate.PeekPlugin>::.ctor()
    IL_0006:  ret
  } // end of method PeekPluginList::.ctor

} // end of class OEMigrate.PeekPluginList

.class public auto ansi beforefieldinit OEMigrate.OmniPeek
       extends [mscorlib]System.Object
{
  .method public hidebysig static int32  GetProtocolNameCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::GetProtocolNameCallback

  .method public hidebysig static int32  GetProtocolLongNameCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::GetProtocolLongNameCallback

  .method public hidebysig static int32  GetProtocolHierNameCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::GetProtocolHierNameCallback

  .method public hidebysig static int32  GetProtocolParentCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::GetProtocolParentCallback

  .method public hidebysig static int32  GetProtocolColorCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::GetProtocolColorCallback

  .method public hidebysig static int32  IsDescendentOfCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::IsDescendentOfCallback

  .method public hidebysig static int32  LookupNameCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::LookupNameCallback

  .method public hidebysig static int32  LookupEntryCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::LookupEntryCallback

  .method public hidebysig static int32  AddNameEntryCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::AddNameEntryCallback

  .method public hidebysig static int32  InvokeNameEditDialogCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::InvokeNameEditDialogCallback

  .method public hidebysig static int32  LookupNameCallbackEx() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::LookupNameCallbackEx

  .method public hidebysig static int32  LookupEntryCallbackEx() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::LookupEntryCallbackEx

  .method public hidebysig static int32  AddNameEntryCallbackEx() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::AddNameEntryCallbackEx

  .method public hidebysig static int32  InvokeNameEditDialogCallbackEx() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::InvokeNameEditDialogCallbackEx

  .method public hidebysig static int32  ResolveAddressCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::ResolveAddressCallback

  .method public hidebysig static int32  SummaryGetEntryCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::SummaryGetEntryCallback

  .method public hidebysig static int32  SummaryModifyEntryCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::SummaryModifyEntryCallback

  .method public hidebysig static int32  PacketGetLayerCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::PacketGetLayerCallback

  .method public hidebysig static int32  PacketGetDataLayerCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::PacketGetDataLayerCallback

  .method public hidebysig static int32  PacketGetHeaderLayerCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::PacketGetHeaderLayerCallback

  .method public hidebysig static int32  PacketGetAddressCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::PacketGetAddressCallback

  .method public hidebysig static int32  PacketGetGPSInfoCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::PacketGetGPSInfoCallback

  .method public hidebysig static int32  PrefsGetValueCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::PrefsGetValueCallback

  .method public hidebysig static int32  PrefsSetValueCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::PrefsSetValueCallback

  .method public hidebysig static int32  PrefsGetPrefsPathCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::PrefsGetPrefsPathCallback

  .method public hidebysig static int32  InsertPacketCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::InsertPacketCallback

  .method public hidebysig static int32  SelectPacketsCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::SelectPacketsCallback

  .method public hidebysig static int32  SelectPacketsExCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::SelectPacketsExCallback

  .method public hidebysig static int32  GetPacketCountCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::GetPacketCountCallback

  .method public hidebysig static int32  GetFirstPacketIndexCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::GetFirstPacketIndexCallback

  .method public hidebysig static int32  GetPacketCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::GetPacketCallback

  .method public hidebysig static int32  ClaimPacketStringCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::ClaimPacketStringCallback

  .method public hidebysig static int32  DecodeOpCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::DecodeOpCallback

  .method public hidebysig static int32  CallUserDecodeCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::CallUserDecodeCallback

  .method public hidebysig static int32  AddUserDecodeCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::AddUserDecodeCallback

  .method public hidebysig static int32  AddTabCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::AddTabCallback

  .method public hidebysig static int32  AddTabWithGroupCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::AddTabWithGroupCallback

  .method public hidebysig static int32  GetThemeColorsCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::GetThemeColorsCallback

  .method public hidebysig static int32  NotifyCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::NotifyCallback

  .method public hidebysig static int32  SendPluginMessageCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::SendPluginMessageCallback

  .method public hidebysig static int32  MakeFilterCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::MakeFilterCallback

  .method public hidebysig static int32  GetAppResourcePathCallback() cil managed
  {
.vtentry 1 : 1
.export [1] as GetAppResourcePathCallback
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::GetAppResourcePathCallback

  .method public hidebysig static int32  SendPacketCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::SendPacketCallback

  .method public hidebysig static int32  SetPacketListColumnCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::SetPacketListColumnCallback

  .method public hidebysig static int32  SaveContextCallback() cil managed
  {
    // Code size       2 (0x2)
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  ret
  } // end of method OmniPeek::SaveContextCallback

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method OmniPeek::.ctor

} // end of class OEMigrate.OmniPeek


// =============================================================

// *********** DISASSEMBLY COMPLETE ***********************
// WARNING: Created Win32 resource file oemigrate.res
