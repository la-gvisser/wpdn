//	Copyright (c) 2008-2016 Savvius, Inc. All rights reserved.

function OnFinish(selProj, selObj) {
    // wizard.ReportError('Begin');	// Documents how to display a message dialog.
    try {
        var strProjectPath = wizard.FindSymbol('PROJECT_PATH');
        var strProjectName = wizard.FindSymbol('PROJECT_NAME');
        var strSafeProjectName = wizard.FindSymbol('SAFE_PROJECT_NAME');

        selProj = CreateCustomProject(strSafeProjectName, strProjectPath);
        selProj.Object.Keyword = "Win32Proj";

        AddConfig(selProj, strSafeProjectName);
        AddFilters(selProj);

        var nInfCount = 0;
        var InfFiles = new Array();
        InfFiles[0] = CreateInfByName("Templates");
        InfFiles[1] = CreateInfByName("Helium");
        InfFiles[2] = CreateInfByName("Framework");
        InfFiles[3] = CreateInfByName("Library")
        InfFiles[4] = CreateInfByName("Plugin");
        nInfCount = 5;

        var strFolderNames = new Array("", "Helium", "Framework", "Library", "Plugin");
        AddFilesToCustomProj(selProj, strSafeProjectName, strProjectPath, nInfCount, InfFiles, strFolderNames);
        PchSettings(selProj);
        ExcludeFiles(selProj, ".so");

        selProj.Object.Save();

        for (var i = 0; i < nInfCount; i++) {
            InfFiles[i].Delete();
        }
    }
    catch(e) {
        if (e.description.length != 0) {
            SetErrorInfo(e);
        }
        return e.number
    }
    // wizard.ReportError('End');
}

function CreateCustomProject(strProjectName, strProjectPath)
{
    try {
        var strWizardName = wizard.FindSymbol('WIZARD_NAME');
        if ( strWizardName == undefined ) {
            // wizard.ReportError('Hey, Wizard Name is not defined');
            strWizardName = 'OmniEngine10';
        }

        var strWizardPath = wizard.FindSymbol('RELATIVE_PATH');
        if ( strWizardPath != undefined ) {
            var strInstallDir = wizard.FindSymbol('PRODUCT_INSTALLATION_DIR');
            if ( strInstallDir == undefined ) {
                // wizard.ReportError('Hey, Product Installation Dir is not defined');
            }
            strWizardPath = strInstallDir + strWizardPath + '\\' + strWizardName;
        }
        else {
            strWizardPath = wizard.FindSymbol('ABSOLUTE_PATH');
            if ( strWizardPath == undefined ) {
                strWizardPath =  wizard.FindSymbol('PROJECT_TEMPLATE_PATH');
                if ( strWizardPath != undefined ) {
                    strWizardPath = strWizardPath + '\\' + strWizardName;
                }
                else {
                    // error
                }
            }
        }
        // wizard.ReportError('Project Template Path: ' + strProjTemplatePath);

        var strVcprojExt = 'vcproj';
        var fxtarget = wizard.FindSymbol("TARGET_FRAMEWORK_VERSION");
        if (fxtarget != null && fxtarget != "") {
            fxtarget = fxtarget.split('.', 2);
            if (fxtarget.length == 2) {
                if (fxtarget[0] >= '4') {
                    strVcprojExt = 'vcxproj';
                }
            }
        }

        var strProjTemplate = strWizardPath + '\\default.' + strVcprojExt;
        // wizard.ReportError('Project Template: ' + strProjTemplate);

        var Solution = dte.Solution;
        var strSolutionName = "";
        if (wizard.FindSymbol("CLOSE_SOLUTION")) {
            Solution.Close();
            strSolutionName = wizard.FindSymbol("VS_SOLUTION_NAME");
            if (strSolutionName.length) {
                var strSolutionPath = strProjectPath.substr(0, strProjectPath.length - strProjectName.length);
                Solution.Create(strSolutionPath, strSolutionName);
            }
        }

        var strProjectNameWithExt = '';
        strProjectNameWithExt = strProjectName + '.' + strVcprojExt;

        var oTarget = wizard.FindSymbol("TARGET");
        var prj;
        if (wizard.FindSymbol("WIZARD_TYPE") == vsWizardAddSubProject) {
            var prjItem = oTarget.AddFromTemplate(strProjTemplate, strProjectNameWithExt);
            prj = prjItem.SubProject;
        }
        else {
            prj = oTarget.AddFromTemplate(strProjTemplate, strProjectPath, strProjectNameWithExt);
        }

        if (fxtarget != null && fxtarget != "") {
            if (fxtarget.length == 2) {
                if (fxtarget[0] >= '4') {
                    prj.Object.TargetFrameworkVersion = parseInt(fxtarget[0]) * 0x10000 + parseInt(fxtarget[1])
                }
            }
        }
        
        return prj;
    }
    catch(e) {
        throw e;
    }
}

function AddFilters(proj)
{
    try {
        // Add the folders to your project
        var strSrcFilter = wizard.FindSymbol('SOURCE_FILTER');
        var group = proj.Object.AddFilter('Source Files');
        group.Filter = strSrcFilter;

        var strSrcFilter = wizard.FindSymbol('HEADER_FILTER');
        var group = proj.Object.AddFilter('Header Files');
        group.Filter = strSrcFilter;

        var strSrcFilter = wizard.FindSymbol('RESOURCE_FILTER');
        var group = proj.Object.AddFilter('Resource Files');
        group.Filter = strSrcFilter;
    }
    catch(e) {
        throw e;
    }
}

function AddConfig(proj, strProjectName)
{
    try {
//        wizard.ReportError('Projects: ' + proj.Object.Configurations.Count);
//        wizard.ReportError('Project 1: ' + proj.Object.Configurations(1).Name);
//        wizard.ReportError('Project 2: ' + proj.Object.Configurations(2).Name);
//        wizard.ReportError('Project 3: ' + proj.Object.Configurations(3).Name);
//        wizard.ReportError('Project 4: ' + proj.Object.Configurations(4).Name);

        var bInstallZips = false;
        var bUIHandler = wizard.FindSymbol('NOTIFY_UIHANDLER');
        var bTabView = wizard.FindSymbol('NOTIFY_TAB');

        // machineX64 is undefined. The value 17 comes from a project where x64 has been added.
        var varMachineX64 = 17;

        var varRunTimeDebug;
        var varRunTimeRelease;
        var varMFC;
        var varExceptions;
        var varLibraries = ".\\lib\\$(Platform)\\libheruntime.lib .\\lib\\$(Platform)\\Protospecs.lib Version.lib";

        varRunTimeDebug = rtMultiThreadedDebug;
        varRunTimeRelease = rtMultiThreaded;
        varMFC = useOfMfc.UseMfcStatic;
        varExceptions = cppExceptionHandlingYesWithSEH;

        var wshell = new ActiveXObject("WScript.Shell");

        var strMakeZips = "makezips.cmd"
        var strDescZips = "Make ZIP resources.";

        var strEnvProgFiles = "%ProgramFiles%";
        var strProgramFiles = wshell.ExpandEnvironmentStrings(strEnvProgFiles);

        var strEnvProgFilesX86 = "%PROGRAMFILES(X86)%";
        var strProgramFilesX86 = wshell.ExpandEnvironmentStrings(strEnvProgFilesX86);

        var bIs64bit = (strEnvProgFilesX86 != strProgramFilesX86);
        
        var nIndexOfx86 = strProgramFiles.indexOf(" (x86)");
        if (nIndexOfx86 > 0) {
            strProgramFiles = strProgramFiles.substring(0, nIndexOfx86)
            bIs64bit = true;
        }

        var strBasePath32;
        var strBasePath64;

        if (bIs64bit) {
            strBasePath32 = strProgramFilesX86 + "\\Savvius";
            strBasePath64 = strProgramFiles + "\\Savvius";
        }
        else {
            strBasePath32 = strProgramFiles + "\\Savvius";
            strBasePath64 = strProgramFiles + "\\Savvius";
        }

        var strEnginePath32 = strBasePath32 + "\\Capture Engine";
        var strEnginePath64 = strBasePath64 + "\\Capture Engine";
        var strPeekPath32 = strBasePath32 + "\\OmniPeek";
        var strPeekPath64 = strBasePath64 + "\\OmniPeek";

        var strEnvVar32 = "%OEALTINSTALL%";
        var strAltInstall32 = wshell.ExpandEnvironmentStrings(strEnvVar32);
        if (strAltInstall32 != strEnvVar32) {
            strEnginePath32 = strAltInstall32;
            strPeekPath32 = strAltInstall32;
        }

        var strEnvVar64 = "%OEX64INSTALL%";
        var strAltInstall64 = wshell.ExpandEnvironmentStrings(strEnvVar64);
        if (strAltInstall64 != strEnvVar64) {
            strEnginePath64 = strAltInstall64;
        }

        var strDebugCmd32 = strEnginePath32 + "\\OmniEngine.exe";
        var strDebugCmd64 = strEnginePath64 + "\\OmniEngine.exe";
        var strDebugArgs = "-d";

        var strCopyCmd32 =
            "set INSTALL_DIR=" + strEnginePath32 + "\r\n" +
            "if exist \"%INSTALL_DIR%\" (\r\n" +
            "if not exist \"%INSTALL_DIR%\\Plugins\" mkdir \"%INSTALL_DIR%\\Plugins\"\r\n" +
            "copy \"$(TargetPath)\" \"%INSTALL_DIR%\\Plugins\\\"\r\n" +
            "copy \".\\$(TargetName).he\" \"%INSTALL_DIR%\\Plugins\\\"\r\n";
        if (bUIHandler) {
            if (bInstallZips) {
                strCopyCmd32 = strCopyCmd32 +
                    "copy \".\\Plugin\\$(TargetName)-About.zip\" \"%INSTALL_DIR%\\Plugins\\\"\r\n" +
                    "copy \".\\Plugin\\$(TargetName)-Options.zip\" \"%INSTALL_DIR%\\Plugins\\\"\r\n";
                if (bTabView) {
                    strCopyCmd32 = strCopyCmd32 +
                        "copy \".\\Plugin\\$(TargetName)-CaptureTab.zip\" \"%INSTALL_DIR%\\Plugins\\\"\r\n";
                    strCopyCmd32 = strCopyCmd32 +
                        "copy \".\\Plugin\\$(TargetName)-EngineTab.zip\" \"%INSTALL_DIR%\\Plugins\\\"\r\n";
                }
            }
        }
        else {    
            strCopyCmd32 = strCopyCmd32 +
                ")\r\n" +
                "set PEEK_DIR=" + strPeekPath32 + "\r\n" +
                "if exist \"%PEEK_DIR%\" (\r\n" +
                "if not exist \"%PEEK_DIR%\\Engine Plugins\" mkdir \"%PEEK_DIR%\\Engine Plugins\"\r\n" +
                "copy \"$(TargetPath)\" \"%PEEK_DIR%\\Engine Plugins\\\"\r\n" +
                "copy \".\\$(TargetName).he\" \"%PEEK_DIR%\\Engine Plugins\\\"\r\n";
        }
        strCopyCmd32 = strCopyCmd32 + ")\r\n";

        var strCopyCmd64 =
            "set INSTALL_DIR=" + strEnginePath64 + "\r\n" +
            "if exist \"%INSTALL_DIR%\" (\r\n" +
            "if not exist \"%INSTALL_DIR%\\Plugins\" mkdir \"%INSTALL_DIR%\\Plugins\"\r\n" +
            "copy \"$(TargetPath)\" \"%INSTALL_DIR%\\Plugins\\\"\r\n" +
            "copy \".\\$(TargetName).he\" \"%INSTALL_DIR%\\Plugins\\\"\r\n";
        if (bUIHandler) {
            if (bInstallZips) {
                strCopyCmd64 = strCopyCmd64 +
                    "copy \".\\Plugin\\$(TargetName)-About.zip\" \"%INSTALL_DIR%\\Plugins\\\"\r\n" +
                    "copy \".\\Plugin\\$(TargetName)-Options.zip\" \"%INSTALL_DIR%\\Plugins\\\"\r\n";
                if (bTabView) {
                    strCopyCmd64 = strCopyCmd64 +
                        "copy \".\\Plugin\\$(TargetName)-CaptureTab.zip\" \"%INSTALL_DIR%\\Plugins\\\"\r\n";
                    strCopyCmd64 = strCopyCmd64 +
                        "copy \".\\Plugin\\$(TargetName)-EngineTab.zip\" \"%INSTALL_DIR%\\Plugins\\\"\r\n";
                }
            }
        }
        else {
            strCopyCmd64 = strCopyCmd64 +
                ")\r\n" +
                "set PEEK_DIR=" + strPeekPath64 + "\r\n" +
                "if exist \"%PEEK_DIR%\" (\r\n" +
                "if not exist \"%PEEK_DIR%\\Engine Plugins\" mkdir \"%PEEK_DIR%\\Engine Plugins\"\r\n" +
                "copy \"$(TargetPath)\" \"%PEEK_DIR%\\Engine Plugins\\\"\r\n" +
                "copy \".\\$(TargetName).he\" \"%PEEK_DIR%\\Engine Plugins\\\"\r\n";
        }
        strCopyCmd64 = strCopyCmd64 + ")\r\n";

        var strDescInstall = "Install the plugin.";

        var config;

        //-------------------------------------------------------
        // Config Debug Win32
        config = proj.Object.Configurations("Debug|Win32");

        config.IntermediateDirectory = '$(Platform)\\$(Configuration)\\';
        config.OutputDirectory = '$(Platform)\\$(Configuration)\\';
        config.ConfigurationType = typeDynamicLibrary;
        config.CharacterSet = charSet.charSetUnicode;
        config.UseOfMFC = varMFC;
        config.UseOfATL = false;
        config.DebugSettings.Command = strDebugCmd32;
        config.DebugSettings.CommandArguments = strDebugArgs;

        // Resource settings
        var RCTool = config.Tools("VCResourceCompilerTool");
        RCTool.AdditionalIncludeDirectories = "$(IntDir); ./; ./Plugin; ./Helium; ./Framework";
        RCTool.PreprocessorDefinitions = RCTool.PreprocessorDefinitions + ";WIN32";

        // Compiler settings
        var CLTool = config.Tools("VCCLCompilerTool");
        CLTool.AdditionalIncludeDirectories = "./; ./Plugin; ./Helium; ./Framework";
        CLTool.RuntimeLibrary = varRunTimeDebug;
        CLTool.UsePrecompiledHeader = pchUseUsingSpecific;
        CLTool.WarningLevel = warningLevel_4;
        CLTool.DisableSpecificWarnings = "4005;4127;4505"
        CLTool.MinimalRebuild = true;
        CLTool.DebugInformationFormat = debugEnabled;
        CLTool.Optimization = optimizeDisabled;
        CLTool.ExceptionHandling = varExceptions;
        CLTool.BasicRuntimeChecks = runtimeBasicCheckAll;
        CLTool.Detect64BitPortabilityProblems = false;
        CLTool.RuntimeTypeInfo = true;

        var strDefines = GetPlatformDefine(config);
        strDefines += "_DEBUG;_WINDOWS;WIN32;HE_WIN32;";
        strDefines += "_AFX;_USRDLL;"
        CLTool.PreprocessorDefinitions = strDefines;

        // Linker settings
        var LinkTool = config.Tools("VCLinkerTool");
        LinkTool.SubSystem = subSystemWindows;
        LinkTool.TargetMachine = machineX86;
        LinkTool.GenerateDebugInformation = true;
        LinkTool.ProgramDatabaseFile = "$(OutDir)" + strProjectName + ".pdb";
        LinkTool.LinkIncremental = linkIncrementalNo;
        LinkTool.IgnoreImportLibrary = true;
        LinkTool.ImportLibrary = "$(OutDir)" + strProjectName + ".lib";
        LinkTool.OutputFile = "$(OutDir)" + strProjectName + ".dll";
        LinkTool.AdditionalDependencies = varLibraries;

        // Build Events settings
        if (bUIHandler) {
            var PreBuildEventTool = config.Tools("VCPreBuildEventTool");
            PreBuildEventTool.CommandLine = strMakeZips;
            PreBuildEventTool.Description = strDescZips;
        }

        var PostBuildEventTool = config.Tools("VCPostBuildEventTool");
        PostBuildEventTool.CommandLine = strCopyCmd32;
        PostBuildEventTool.Description = strDescInstall;

        //-------------------------------------------------------
        // Configure Release Win32
        config = proj.Object.Configurations("Release|Win32");

        config.IntermediateDirectory = "$(Platform)\\$(Configuration)\\";
        config.OutputDirectory = "$(Platform)\\$(Configuration)\\";
        config.ConfigurationType = typeDynamicLibrary;
        config.CharacterSet = charSet.charSetUnicode;
        config.UseOfMFC = varMFC;
        config.UseOfATL = false;
        config.DebugSettings.Command = strDebugCmd32;
        config.DebugSettings.CommandArguments = strDebugArgs;

        // Resource settings
        var RCTool = config.Tools("VCResourceCompilerTool");
        RCTool.AdditionalIncludeDirectories = "$(IntDir); ./; ./Plugin; ./Helium; ./Framework";
        RCTool.PreprocessorDefinitions = RCTool.PreprocessorDefinitions + ";WIN32";

        // Compiler settings
        CLTool = config.Tools("VCCLCompilerTool");
        CLTool.AdditionalIncludeDirectories = "./; ./Plugin; ./Helium; ./Framework";
        CLTool.RuntimeLibrary = varRunTimeRelease;
        CLTool.InlineFunctionExpansion = expandOnlyInline;
        CLTool.WarningLevel = warningLevel_4;
        CLTool.DisableSpecificWarnings = "4005;4127;4505"
        CLTool.UsePrecompiledHeader = pchUseUsingSpecific;
        CLTool.Optimization = optimizeMaxSpeed;
        CLTool.ExceptionHandling = varExceptions;
        CLTool.OmitFramePointers = true;
        CLTool.EnableFunctionLevelLinking = true;
        CLTool.StringPooling = true;
        CLTool.Detect64BitPortabilityProblems = false;
        CLTool.DebugInformationFormat = debugEnabled;
        CLTool.RuntimeTypeInfo = true;

        var strDefines = GetPlatformDefine(config);
        strDefines += "NDEBUG;_WINDOWS;WIN32;HE_WIN32;";
        strDefines += "_AFX;_USRDLL;"
        CLTool.PreprocessorDefinitions = strDefines;

        // Linker settings
        LinkTool = config.Tools("VCLinkerTool");
        LinkTool.SubSystem = subSystemWindows;
        LinkTool.TargetMachine = machineX86;
        LinkTool.GenerateDebugInformation = true;
        LinkTool.EnableCOMDATFolding = optFolding;
        LinkTool.OptimizeReferences = optReferences;
        LinkTool.LinkIncremental = linkIncrementalNo;
        LinkTool.IgnoreImportLibrary = true;
        LinkTool.ImportLibrary = "$(OutDir)" + strProjectName + ".lib";
        LinkTool.OutputFile = "$(OutDir)" + strProjectName + ".dll";
        LinkTool.AdditionalDependencies = varLibraries;

        // Build Events settings
        if (bUIHandler) {
            var PreBuildEventTool = config.Tools("VCPreBuildEventTool");
            PreBuildEventTool.CommandLine = strMakeZips;
            PreBuildEventTool.Description = strDescZips;
        }

        var PostBuildEventTool = config.Tools("VCPostBuildEventTool");
        PostBuildEventTool.CommandLine = strCopyCmd32;
        PostBuildEventTool.Description = strDescInstall;

        //-------------------------------------------------------
        // Config Debug x64
        config = proj.Object.Configurations("Debug|x64");
        if (config == null) {
            // x64 compiler is not installed.
            return;
        }

        config.IntermediateDirectory = '$(Platform)\\$(Configuration)\\';
        config.OutputDirectory = '$(Platform)\\$(Configuration)\\';
        config.ConfigurationType = typeDynamicLibrary;
        config.CharacterSet = charSet.charSetUnicode;
        config.UseOfMFC = varMFC;
        config.UseOfATL = false;
        if (bIs64bit) {
            config.DebugSettings.Command = strDebugCmd64;
            config.DebugSettings.CommandArguments = strDebugArgs;
        }

        // Resource settings
        var RCTool = config.Tools("VCResourceCompilerTool");
        RCTool.AdditionalIncludeDirectories = "$(IntDir); ./; ./Plugin; ./Helium; ./Framework";
        RCTool.PreprocessorDefinitions = RCTool.PreprocessorDefinitions + ";X64";

        // Compiler settings
        var CLTool = config.Tools("VCCLCompilerTool");
        CLTool.AdditionalIncludeDirectories = "./; ./Plugin; ./Helium; ./Framework";
        CLTool.RuntimeLibrary = varRunTimeDebug;
        CLTool.UsePrecompiledHeader = pchUseUsingSpecific;
        CLTool.WarningLevel = warningLevel_4;
        CLTool.DisableSpecificWarnings = "4005;4127;4505"
        CLTool.MinimalRebuild = true;
        CLTool.DebugInformationFormat = debugEnabled;
        CLTool.Optimization = optimizeDisabled;
        CLTool.ExceptionHandling = varExceptions;
        CLTool.BasicRuntimeChecks = runtimeBasicCheckAll;
        CLTool.Detect64BitPortabilityProblems = false;
        CLTool.RuntimeTypeInfo = true;

        var strDefines = GetPlatformDefine(config);
        strDefines += "_DEBUG;_WINDOWS;WIN64;HE_WIN32;";
        strDefines += "_AFX;_USRDLL;"
        CLTool.PreprocessorDefinitions = strDefines;

        // Linker settings
        var LinkTool = config.Tools("VCLinkerTool");
        LinkTool.SubSystem = subSystemWindows;
        LinkTool.TargetMachine = varMachineX64;
        LinkTool.GenerateDebugInformation = true;
        LinkTool.ProgramDatabaseFile = "$(OutDir)" + strProjectName + ".pdb";
        LinkTool.LinkIncremental = linkIncrementalNo;
        LinkTool.IgnoreImportLibrary = true;
        LinkTool.ImportLibrary = "$(OutDir)" + strProjectName + ".lib";
        LinkTool.OutputFile = "$(OutDir)" + strProjectName + ".dll";
        LinkTool.AdditionalDependencies = varLibraries;

        // Build Events settings
        if (bIs64bit) {
            if (bUIHandler) {
                var PreBuildEventTool = config.Tools("VCPreBuildEventTool");
                PreBuildEventTool.CommandLine = strMakeZips;
                PreBuildEventTool.Description = strDescZips;
            }

            var PostBuildEventTool = config.Tools("VCPostBuildEventTool");
            PostBuildEventTool.CommandLine = strCopyCmd64;
            PostBuildEventTool.Description = strDescInstall;
        }

        //-------------------------------------------------------
        // Configure Release x64
        config = proj.Object.Configurations("Release|x64");

        config.IntermediateDirectory = "$(Platform)\\$(Configuration)\\";
        config.OutputDirectory = "$(Platform)\\$(Configuration)\\";
        config.ConfigurationType = typeDynamicLibrary;
        config.CharacterSet = charSet.charSetUnicode;
        config.UseOfMFC = varMFC;
        config.UseOfATL = false;
        if (bIs64bit) {
            config.DebugSettings.Command = strDebugCmd64;
            config.DebugSettings.CommandArguments = strDebugArgs;
        }

        // Resource settings
        var RCTool = config.Tools("VCResourceCompilerTool");
        RCTool.AdditionalIncludeDirectories = "$(IntDir); ./; ./Plugin; ./Helium; ./Framework";
        RCTool.PreprocessorDefinitions = RCTool.PreprocessorDefinitions + ";X64";

        // Compiler settings
        CLTool = config.Tools("VCCLCompilerTool");
        CLTool.AdditionalIncludeDirectories = "./; ./Plugin; ./Helium; ./Framework";
        CLTool.RuntimeLibrary = varRunTimeRelease;
        CLTool.InlineFunctionExpansion = expandOnlyInline;
        CLTool.WarningLevel = warningLevel_4;
        CLTool.DisableSpecificWarnings = "4005;4127;4505"
        CLTool.UsePrecompiledHeader = pchUseUsingSpecific;
        CLTool.Optimization = optimizeMaxSpeed;
        CLTool.ExceptionHandling = varExceptions;
        CLTool.OmitFramePointers = true;
        CLTool.EnableFunctionLevelLinking = true;
        CLTool.StringPooling = true;
        CLTool.Detect64BitPortabilityProblems = false;
        CLTool.DebugInformationFormat = debugEnabled;
        CLTool.RuntimeTypeInfo = true;

        var strDefines = GetPlatformDefine(config);
        strDefines += "NDEBUG;_WINDOWS;WIN64;HE_WIN32;";
        strDefines += "_AFX;_USRDLL;"
        CLTool.PreprocessorDefinitions = strDefines;

        // Linker settings
        LinkTool = config.Tools("VCLinkerTool");
        LinkTool.SubSystem = subSystemWindows;
        LinkTool.TargetMachine = varMachineX64;
        LinkTool.GenerateDebugInformation = true;
        LinkTool.EnableCOMDATFolding = optFolding;
        LinkTool.OptimizeReferences = optReferences;
        LinkTool.LinkIncremental = linkIncrementalNo;
        LinkTool.IgnoreImportLibrary = true;
        LinkTool.ImportLibrary = "$(OutDir)" + strProjectName + ".lib";
        LinkTool.OutputFile = "$(OutDir)" + strProjectName + ".dll";
        LinkTool.AdditionalDependencies = varLibraries;

        // Build Events settings
        if (bIs64bit) {
            if (bUIHandler) {
                var PreBuildEventTool = config.Tools("VCPreBuildEventTool");
                PreBuildEventTool.CommandLine = strMakeZips;
                PreBuildEventTool.Description = strDescZips;
            }

            var PostBuildEventTool = config.Tools("VCPostBuildEventTool");
            PostBuildEventTool.CommandLine = strCopyCmd64;
            PostBuildEventTool.Description = strDescInstall;
        }
    }
    catch(e) {
        throw e;
    }
}

function PchSettings(proj)
{
    try {
        // Debug-Unicode and Release-Unicode
        // setup /Yu (using precompiled headers)
        var configs = proj.Object.Configurations;

        for (i = 1; i <= configs.Count; i++) {
            config = configs(i);
            var CLTool = config.Tools("VCCLCompilerTool");
            CLTool.UsePrecompiledHeader = pchUseUsingSpecific;
        }

        // setup /Yc (create precompiled header)
        var files = proj.Object.Files;
        file = files("StdAfx.cpp");

        for (i = 1; i <= configs.Count; i++) {
            config = file.FileConfigurations(i);
            config.Tool.UsePrecompiledHeader = pchCreateUsingSpecific;
        }
    }
    catch(e) {
        throw e;
    }
}

function DelFile(fso, strWizTempFile)
{
    try {
        if (fso.FileExists(strWizTempFile)) {
            var tmpFile = fso.GetFile(strWizTempFile);
            tmpFile.Delete();
        }
    }
    catch(e) {
        throw e;
    }
}

function CreateInfByName(strFileName)
{
    try {
        var fso, TemplatesFolder, TemplateFiles, strTemplate;
        fso = new ActiveXObject('Scripting.FileSystemObject');

        var TemporaryFolder = 2;
        var tfolder = fso.GetSpecialFolder(TemporaryFolder);
        var strTempFolder = tfolder.Drive + "\\" + tfolder.Name;

        var strWizTempFile = strTempFolder + "\\" + fso.GetTempName();

        var strTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');
        var strInfFile = strTemplatePath + "\\" + strFileName + ".inf";
        wizard.RenderTemplate(strInfFile, strWizTempFile);

        var WizTempFile = fso.GetFile(strWizTempFile);
        return WizTempFile;
    }
    catch(e) {
        throw e;
    }
}

function GetTargetName(strName, strProjectName)
{
    try {
        var strTarget = strName;

        if (strName.substr(0, 4) == "root") {
            var strlen = strName.length;
            strTarget = strProjectName + strName.substr(4, strlen - 4);
        }
        else {
            var re = /(\.32)|(\.64)/i;
            var index = strName.search(re);
            if (index >= 0) {
                strTarget = strName.replace(re,"");
            }
        }
        return strTarget;
    }
    catch(e) {
        throw e;
    }
}

function AddFilesToCustomProj(proj, strProjectName, strProjectPath, InfCount, InfFiles, strFolderNames )
{
    try {
        var projItems = proj.ProjectItems;
        var strTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');
        var strTpl = '';
        var strName = '';
        var bUIHandler = wizard.FindSymbol('NOTIFY_UIHANDLER');
        var bCefPrefs = wizard.FindSymbol('NOTIFY_CEFPREFS');
        var bHtmlPrefs = wizard.FindSymbol('NOTIFY_HTMLPREFS');
        var bCheckExt = true;
        var group = proj.Object;
        var group32;
        var group64;
        var groupLib;
        var groupAbout;
        var groupCaptureTab;
        var groupEngineTab;
        var groupOptions;
        for (var i = 0; i < InfCount; i++) {
            // wizard.ReportError('Folder: ' + strFolderNames[i]);
            var strFolderName = strFolderNames[i];
            var bPlugin = (strFolderName == "Plugin");
            var bLibrary = (strFolderName == "Library");
            var strFilterName = strFolderName + ' Files';
            if (i > 0) {
                group = proj.Object.AddFilter(strFilterName);
                bCheckExt = false;
                if (bPlugin && bUIHandler) {
                    groupAbout = group.AddFilter('About');
                    groupCaptureTab = group.AddFilter('CaptureTab');
                    groupEngineTab = group.AddFilter('EngineTab');
                    groupOptions = group.AddFilter('Options');
                }
                else {
                    if (bLibrary) {
                        group32 = group.AddFilter('Win32');
                        group64 = group.AddFilter('x64');
                    }
                }
            }
            var infFile = InfFiles[i];
            var strStream = infFile.OpenAsTextStream(1, -2);
            while (!strStream.AtEndOfStream) {
                strTpl = strStream.ReadLine();
                if (strTpl == '') continue;

                var bLib32 = false;
                var bLib64 = false;
                var strHtml = '';

                if (strTpl.charAt(0) == '*') {
                    if (bUIHandler) continue;
                    strTpl = strTpl.substr(1);
                }
                else {
                    // -ac-index.html or -ah-index.html
                    if (strTpl.charAt(0) == '-') {
                        if (!bUIHandler) continue;

                        strTpl = strTpl.substr(1);
                        if (strTpl.substr(3) == 'index.html') {
                            if (bCefPrefs && (strTpl.charAt(1) == 'c')) {
                                strHtml = strTpl.charAt(0);
                            }
                            else if (bHtmlPrefs && (strTpl.charAt(1) == 'h')) {
                                strHtml = strTpl.charAt(0);
                            }
                            else {
                                continue;
                            }
                        }
                    }
                }

                if (strHtml == '') {
                    strName = strTpl;
                }
                else {
                    strName = strTpl.substr(3);
                }

                var strTarget = GetTargetName(strName, strProjectName);
                var strTemplate = strTemplatePath + '\\' + strTpl;
                var strFile = strProjectPath + '\\';
                if (i > 0) {
                    if (bLibrary) {
                        var re32 = /\.32/i;
                        var re64 = /\.64/i;
                        var index32 = strName.search(re32);
                        if (index32 >= 0) {
                            bLib32 = true;
                            strFile += 'lib\\Win32\\';
                        }
                        else {
                            var index64 = strName.search(re64);
                            if (index64 >= 0) {
                                bLib64 = true;
                                strFile += 'lib\\x64\\';
                            }
                            else {
                                strFile += 'lib\\';
                            }
                        }
                    }
                    else {
                        if (bUIHandler && (strHtml != '')) {
                            if (strHtml == 'a') {
                                strFile += 'Plugin\\About\\';
                            }
                            if (strHtml == 'c') {
                                strFile += 'Plugin\\CaptureTab\\';
                            }
                            if (strHtml == 'e') {
                                strFile += 'Plugin\\EngineTab\\';
                            }
                            if (strHtml == 'o') {
                                strFile += 'Plugin\\Options\\';
                            }
                        }
                        else {
                            strFile += strFolderName + '\\';
                        }
                    }
                }

                strFile += strTarget;
                var strExt = strName.substr(strName.lastIndexOf(".")).toLowerCase();
                // bCopyOnly: "true" will only copy the file from strTemplate to strTarget without
                // rendering the [!if] macros.
                var bCopyOnly =
                    (strExt == ".bmp") || (strExt == ".css") || (strExt == ".gif") ||
                    (strExt == ".ico") || (strExt == ".lib") || (strExt == ".map") ||
                    (strExt == ".rtf") || (strExt == ".so") || (strExt == ".zip");
                wizard.RenderTemplate(strTemplate, strFile, bCopyOnly);
                if (bLibrary) {
                    if (bLib32) {
                        group32.AddFile(strFile);
                    }
                    else {
                        if (bLib64) {
                            group64.AddFile(strFile);
                        }
                        else {
                            group.AddFile(strFile);
                        }
                    }
                }
                else {
                    if (bUIHandler && (strHtml != '')) {
                        if (strHtml == 'a') {
                            groupAbout.AddFile(strFile);
                        }
                        if (strHtml == 'c') {
                            groupCaptureTab.AddFile(strFile);
                        }
                        if (strHtml == 'e') {
                            groupEngineTab.AddFile(strFile);
                        }
                        if (strHtml == 'o') {
                            groupOptions.AddFile(strFile);
                        }
                    }
                    else {
                        group.AddFile(strFile);
                    }
                }
            }
            strStream.Close();
        }
    }
    catch(e) {
        throw e;
    }
}

function ExcludeFiles(proj, ext) {
    try {
        var extLen = ext.length;
        var files = proj.Object.Files;
        for (var i = 1; i <= files.Count; i++) {
            file = files.Item(i);
            if (file.name.substr(file.name.length - extLen) == ext) {
                var configs = file.FileConfigurations;
                for (var ii = 1; ii <= configs.Count; ii++) {
                    configs.Item(ii).ExcludedFromBuild = true;
                }
            }
        }
    }
    catch (e) {
        wizard.ReportError('Failed to exclude the Linux .so files from building.');
    }
}