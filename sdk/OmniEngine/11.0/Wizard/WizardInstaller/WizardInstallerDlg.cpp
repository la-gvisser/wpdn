// ============================================================================
// WizardInstallerDlg.cpp : implementation file
// ============================================================================
//	Copyright (c) Savvius, Inc. 2009-2016. All rights reserved.

#include "stdafx.h"
#include "WizardInstaller.h"
#include "WizardInstallerDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const CString	g_strWizardName( L"OmniEngine" );
const CString	g_strWizardVersionName( L"OmniEngine11" );
const CString	g_strSourcePath( g_strWizardName + L"\\" );
const CString	g_strWizardPath( g_strWizardVersionName + L"\\" );
const CString	g_strVcProjects( L"vcprojects\\" );
const CString	g_strVcWizards( L"VCWizards\\" );


// ////////////////////////////////////////////////////////////////////////////
//		CFileList
// ////////////////////////////////////////////////////////////////////////////

class CFileList
{
protected:
	CAtlArray<CString>	m_strSrcList;
	CAtlArray<CString>	m_strDstList;

public:
	;	CFileList() {};
	virtual	~CFileList() {
		m_strSrcList.RemoveAll();
		m_strDstList.RemoveAll();
	}

	size_t	GetCount() const { return m_strSrcList.GetCount(); }
	CString	GetSrc( size_t in ) const { return m_strSrcList[in]; }
	CString	GetDst( size_t in ) const { return m_strDstList[in]; }
};


class CProjectFileListV140
	:	public CFileList
{
public:
	;	CProjectFileListV140();
};

CProjectFileListV140::CProjectFileListV140()
{
	m_strSrcList.Add( g_strWizardVersionName + L".vsdir" );
	m_strDstList.Add( g_strWizardVersionName + L".vsdir" );

	m_strSrcList.Add( g_strWizardVersionName + L".14.0.vsz" );
	m_strDstList.Add( g_strWizardVersionName + L".vsz" );

	m_strSrcList.Add( g_strWizardVersionName + L".ico" );
	m_strDstList.Add( g_strWizardVersionName + L".ico" );
}


class CProjectFileListV170
	:	public CFileList
{
public:
	;	CProjectFileListV170();
};

CProjectFileListV170::CProjectFileListV170()
{
	m_strSrcList.Add( g_strWizardVersionName + L".vsdir" );
	m_strDstList.Add( g_strWizardVersionName + L".vsdir" );

	m_strSrcList.Add( g_strWizardVersionName + L".17.0.vsz" );
	m_strDstList.Add( g_strWizardVersionName + L".vsz" );

	m_strSrcList.Add( g_strWizardVersionName + L".ico" );
	m_strDstList.Add( g_strWizardVersionName + L".ico" );
}


// ////////////////////////////////////////////////////////////////////////////
//		CWizardFileList
// ////////////////////////////////////////////////////////////////////////////

class CWizardFileArray
	:	public CAtlArray<CString>
{
public:
	size_t	m_nDefaultProject;

	;	CWizardFileArray();
};

CWizardFileArray::CWizardFileArray()
	:	m_nDefaultProject( 0 )
{
	Add( L"1033\\styles.css" );
	Add( L"HTML\\1033\\default.htm" );
	Add( L"Images\\OmniEngine.gif" );
	Add( L"Images\\OmniEngine.ico" );
	Add( L"Images\\OmniEngine_Background.gif" );
	Add( L"Images\\spacer.gif" );
	Add( L"Scripts\\1033\\default.js" );

	Add( L"Templates\\1033\\libheruntime.32.lib" );
	Add( L"Templates\\1033\\libheruntime.64.lib" );
	Add( L"Templates\\1033\\libheruntime.so" );
	Add( L"Templates\\1033\\peekcore.so" );
	Add( L"Templates\\1033\\Protospecs.32.lib" );
	Add( L"Templates\\1033\\Protospecs.64.lib" );

	Add( L"Templates\\1033\\Framework.inf" );
	Add( L"Templates\\1033\\Helium.inf" );
	Add( L"Templates\\1033\\Library.inf" );
	Add( L"Templates\\1033\\Plugin.inf" );
	Add( L"Templates\\1033\\Templates.inf" );

	Add( L"Templates\\1033\\ByteVectRef.cpp" );
	Add( L"Templates\\1033\\ByteVectRef.h" );
	Add( L"Templates\\1033\\CaptureBuffer.h" );
	Add( L"Templates\\1033\\CaptureFile.cpp" );
	Add( L"Templates\\1033\\CaptureFile.h" );
	Add( L"Templates\\1033\\ContextManager.cpp" );
	Add( L"Templates\\1033\\ContextManager.h" );
	Add( L"Templates\\1033\\Ethernet.h" );
	Add( L"Templates\\1033\\FileCaptureBuffer.cpp" );
	Add( L"Templates\\1033\\FileCaptureBuffer.h" );
	Add( L"Templates\\1033\\FileEx.cpp" );
	Add( L"Templates\\1033\\FileEx.h" );
	Add( L"Templates\\1033\\FilePacketArray.h" );
	Add( L"Templates\\1033\\GlobalId.h" );
	Add( L"Templates\\1033\\GPSInfo.cpp" );
	Add( L"Templates\\1033\\GPSInfo.h" );
	Add( L"Templates\\1033\\IpHeaders.cpp" );
	Add( L"Templates\\1033\\IpHeaders.h" );
	Add( L"Templates\\1033\\Layers.cpp" );
	Add( L"Templates\\1033\\Layers.h" );
	Add( L"Templates\\1033\\MediaTypes.h" );
	Add( L"Templates\\1033\\Memutil.h" );
	Add( L"Templates\\1033\\NotifyService.cpp" );
	Add( L"Templates\\1033\\NotifyService.h" );
	Add( L"Templates\\1033\\Packet.cpp" );
	Add( L"Templates\\1033\\Packet.h" );
	Add( L"Templates\\1033\\PacketArray.cpp" );
	Add( L"Templates\\1033\\PacketArray.h" );
	Add( L"Templates\\1033\\PacketHeaders.h" );
	Add( L"Templates\\1033\\PCapCaptureFile.cpp" );
	Add( L"Templates\\1033\\PCapCaptureFile.h" );
	Add( L"Templates\\1033\\Peek.h" );
	Add( L"Templates\\1033\\PeekArray.h" );
	Add( L"Templates\\1033\\PeekCaptureFile.cpp" );
	Add( L"Templates\\1033\\PeekCaptureFile.h" );
	Add( L"Templates\\1033\\PeekConsoleContext.cpp" );
	Add( L"Templates\\1033\\PeekConsoleContext.h" );
	Add( L"Templates\\1033\\PeekContext.cpp" );
	Add( L"Templates\\1033\\PeekContext.h" );
	Add( L"Templates\\1033\\PeekContextProxy.cpp" );
	Add( L"Templates\\1033\\PeekContextProxy.h" );
	Add( L"Templates\\1033\\PeekDataModeler.cpp" );
	Add( L"Templates\\1033\\PeekDataModeler.h" );
	Add( L"Templates\\1033\\PeekEngineContext.h" );
	Add( L"Templates\\1033\\PeekFile.h" );
	Add( L"Templates\\1033\\PeekHash.h" );
	Add( L"Templates\\1033\\PeekLock.h" );
	Add( L"Templates\\1033\\PeekMessage.cpp" );
	Add( L"Templates\\1033\\PeekMessage.h" );
	Add( L"Templates\\1033\\PeekPacket.h" );
	Add( L"Templates\\1033\\PeekPlugin.cpp" );
	Add( L"Templates\\1033\\PeekPlugin.h" );
	Add( L"Templates\\1033\\PeekProxy.cpp" );
	Add( L"Templates\\1033\\PeekProxy.h" );
	Add( L"Templates\\1033\\PeekStream.cpp" );
	Add( L"Templates\\1033\\PeekStream.h" );
	Add( L"Templates\\1033\\PeekStrings.cpp" );
	Add( L"Templates\\1033\\PeekStrings.h" );
	Add( L"Templates\\1033\\PeekTable.h" );
	Add( L"Templates\\1033\\PeekTime.cpp" );
	Add( L"Templates\\1033\\PeekTime.h" );
	Add( L"Templates\\1033\\PeekUnits.cpp" );
	Add( L"Templates\\1033\\PeekUnits.h" );
	Add( L"Templates\\1033\\PeekXml.cpp" );
	Add( L"Templates\\1033\\PeekXml.h" );
	Add( L"Templates\\1033\\PluginController.h" );
	Add( L"Templates\\1033\\PluginHandlersInterface.h" );
	Add( L"Templates\\1033\\PluginManager.cpp" );
	Add( L"Templates\\1033\\PluginManager.h" );
	Add( L"Templates\\1033\\PluginModule.cpp" );
	Add( L"Templates\\1033\\Protospecs.h" );
	Add( L"Templates\\1033\\PSDefs.h" );
	Add( L"Templates\\1033\\PSIDs.h" );
	Add( L"Templates\\1033\\RefreshFile.cpp" );
	Add( L"Templates\\1033\\RefreshFile.h" );
	Add( L"Templates\\1033\\RemotePlugin.cpp" );
	Add( L"Templates\\1033\\RemotePlugin.h" );
	Add( L"Templates\\1033\\Snapshot.cpp" );
	Add( L"Templates\\1033\\Snapshot.h" );
	Add( L"Templates\\1033\\tagfile.h" );
	Add( L"Templates\\1033\\Version.cpp" );
	Add( L"Templates\\1033\\Version.h" );
	Add( L"Templates\\1033\\wppoppack.h" );
	Add( L"Templates\\1033\\wppushpack.h" );
	Add( L"Templates\\1033\\WPTypes.h" );

	Add( L"Templates\\1033\\encodehex.h" );
	Add( L"Templates\\1033\\hecom.h" );
	Add( L"Templates\\1033\\hecomweak.h" );
	Add( L"Templates\\1033\\hecore.h" );
	Add( L"Templates\\1033\\heid.h" );
	Add( L"Templates\\1033\\heinttypes.h" );
	Add( L"Templates\\1033\\helib.h" );
	Add( L"Templates\\1033\\hepoppack.h" );
	Add( L"Templates\\1033\\hepushpack.h" );
	Add( L"Templates\\1033\\heresult.h" );
	Add( L"Templates\\1033\\hestdint.h" );
	Add( L"Templates\\1033\\hestr.h" );
	Add( L"Templates\\1033\\heunk.h" );
	Add( L"Templates\\1033\\hevariant.h" );
	Add( L"Templates\\1033\\heweakref.h" );
	Add( L"Templates\\1033\\hlalloc.h" );
	Add( L"Templates\\1033\\hlatomic.h" );
	Add( L"Templates\\1033\\hlbase.h" );
	Add( L"Templates\\1033\\hlclstab.h" );
	Add( L"Templates\\1033\\hlcom.h" );
	Add( L"Templates\\1033\\hldef.h" );
	Add( L"Templates\\1033\\hlexcept.h" );
	Add( L"Templates\\1033\\hlid.h" );
	Add( L"Templates\\1033\\hlptr.h" );
	Add( L"Templates\\1033\\hlruntime.h" );
	Add( L"Templates\\1033\\hlstr.h" );
	Add( L"Templates\\1033\\hlstrconv.h" );
	Add( L"Templates\\1033\\hlsync.h" );
	Add( L"Templates\\1033\\hlvariant.h" );
	Add( L"Templates\\1033\\hlweakref.h" );
	Add( L"Templates\\1033\\idatatable.h" );
	Add( L"Templates\\1033\\imemorystream.h" );
	Add( L"Templates\\1033\\iomniengineplugin.h" );
	Add( L"Templates\\1033\\ixmldom.h" );
	Add( L"Templates\\1033\\objectwithsiteimpl.h" );
	Add( L"Templates\\1033\\omnicommands.h" );
	Add( L"Templates\\1033\\omnigraphs.h" );
	Add( L"Templates\\1033\\peekcore.h" );
	Add( L"Templates\\1033\\peekfilters.h" );
	Add( L"Templates\\1033\\peeknames.h" );
	Add( L"Templates\\1033\\peeknotify.h" );
	Add( L"Templates\\1033\\peekstats.h" );
	Add( L"Templates\\1033\\peekutil.h" );
	Add( L"Templates\\1033\\peekvoipcall.h" );

	Add( L"Templates\\1033\\ac-index.html" );
	Add( L"Templates\\1033\\ah-index.html" );
	Add( L"Templates\\1033\\AboutDialog.cpp" );
	Add( L"Templates\\1033\\AboutDialog.h" );
	Add( L"Templates\\1033\\cc-index.html" );
	Add( L"Templates\\1033\\ch-index.html" );
	Add( L"Templates\\1033\\ContextTab.cpp" );
	Add( L"Templates\\1033\\ContextTab.h" );
	Add( L"Templates\\1033\\ec-index.html" );
	Add( L"Templates\\1033\\eh-index.html" );
	Add( L"Templates\\1033\\oc-index.html" );
	Add( L"Templates\\1033\\oh-index.html" );
	Add( L"Templates\\1033\\OmniConsoleContext.cpp" );
	Add( L"Templates\\1033\\OmniConsoleContext.h" );
	Add( L"Templates\\1033\\OmniEngineContext.cpp" );
	Add( L"Templates\\1033\\OmniEngineContext.h" );
	Add( L"Templates\\1033\\OmniPlugin.cpp" );
	Add( L"Templates\\1033\\OmniPlugin.h" );
	Add( L"Templates\\1033\\Options.cpp" );
	Add( L"Templates\\1033\\Options.h" );
	Add( L"Templates\\1033\\OptionsDialog.cpp" );
	Add( L"Templates\\1033\\OptionsDialog.h" );
	Add( L"Templates\\1033\\OptionsView.cpp" );
	Add( L"Templates\\1033\\OptionsView.h" );
	Add( L"Templates\\1033\\Plugin.rc" );
	Add( L"Templates\\1033\\PluginMessages.cpp" );
	Add( L"Templates\\1033\\PluginMessages.h" );
	Add( L"Templates\\1033\\PluginTab.cpp" );
	Add( L"Templates\\1033\\PluginTab.h" );
	Add( L"Templates\\1033\\resource.h" );
	Add( L"Templates\\1033\\StdAfx.cpp" );
	Add( L"Templates\\1033\\StdAfx.h" );
	Add( L"Templates\\1033\\TabHost.cpp" );
	Add( L"Templates\\1033\\TabHost.h" );
	Add( L"Templates\\1033\\savviuslogo.bmp" );

	Add( L"Templates\\1033\\Makefile" );
	Add( L"Templates\\1033\\makezips.cmd" );
	Add( L"Templates\\1033\\module.map" );
	Add( L"Templates\\1033\\root.he" );
}


class CWizardFileListV140
	: public CFileList {
public:
	;	CWizardFileListV140( const CWizardFileArray& inFileArray );
};

CWizardFileListV140::CWizardFileListV140(
	const CWizardFileArray&	inFileArray )
{
	m_strSrcList.Add( L"default.14.0.vcxproj" );
	m_strDstList.Add( L"default.vcxproj" );

	for ( size_t i = 0; i < inFileArray.GetCount(); i++ ) {
		m_strSrcList.Add( inFileArray[i] );
		m_strDstList.Add( inFileArray[i] );
	}
}


class CWizardFileListV170
	: public CFileList {
public:
	;	CWizardFileListV170( const CWizardFileArray& inFileArray );
};

CWizardFileListV170::CWizardFileListV170(
	const CWizardFileArray&	inFileArray )
{
	m_strSrcList.Add( L"default.17.0.vcxproj" );
	m_strDstList.Add( L"default.vcxproj" );

	for ( size_t i = 0; i < inFileArray.GetCount(); i++ ) {
		m_strSrcList.Add( inFileArray[i] );
		m_strDstList.Add( inFileArray[i] );
	}
}


// ////////////////////////////////////////////////////////////////////////////
//		CExtraUninstallFileList
// ////////////////////////////////////////////////////////////////////////////

class CExtraUninstallFileList
	:	public CAtlArray<CString>
{
public:
	;	CExtraUninstallFileList();
};

CExtraUninstallFileList::CExtraUninstallFileList()
{
	Add( L"Templates\\1033\\AGTypes.h" );
}

// The list of files to install.
const CWizardFileArray	g_strWizardFileArray;
const CFileList*		g_strProjectFiles[CWizardInstallerDlg::g_nMaxVersions] = {
	new CProjectFileListV140(),
	new CProjectFileListV170()
};
const CFileList*		g_strWizardFiles[CWizardInstallerDlg::g_nMaxVersions] = {
	new CWizardFileListV140( g_strWizardFileArray ),
	new CWizardFileListV170( g_strWizardFileArray )
};
const CExtraUninstallFileList	g_strExtraUninstallFiles;

// Registry Keys and strings.
static CString	g_strSubKey[CWizardInstallerDlg::g_nMaxVersions] = {
	L"SOFTWARE\\Microsoft\\VisualStudio\\14.0\\Setup\\VC",
	L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\36102503"
};

const CString	g_strDirLabel[CWizardInstallerDlg::g_nMaxVersions] = {
	L"ProductDir",
	L"InstallLocation"
};


// ////////////////////////////////////////////////////////////////////////////
//	DeleteFileLists

void DeleteFileLists()
{
	for ( int i = 0; i < CWizardInstallerDlg::g_nMaxVersions; i++ ) {
		delete g_strProjectFiles[i];
		g_strProjectFiles[i] = nullptr;

		delete g_strWizardFiles[i];
		g_strWizardFiles[i] = nullptr;
	}
}


// ////////////////////////////////////////////////////////////////////////////
// CWizardInstallerDlg dialog
// ////////////////////////////////////////////////////////////////////////////

CWizardInstallerDlg::CWizardInstallerDlg(
	CWnd* pParent /*=nullptr*/)
	:	CDialog( CWizardInstallerDlg::IDD, pParent )
	,	m_nState( kState_Load )
{
	m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
}


CWizardInstallerDlg::~CWizardInstallerDlg()
{
}

// ----------------------------------------------------------------------------
//		DoDataExchange
// ----------------------------------------------------------------------------

void
CWizardInstallerDlg::DoDataExchange(
	CDataExchange*	pDX )
{
	CDialog::DoDataExchange( pDX );

	DDX_Control( pDX, IDC_INSTALL_LBL, m_lblInstall);
	DDX_Control( pDX, IDC_STATUS_LBL, m_lblStatus );
	DDX_Control( pDX, IDOK, m_btnOK );
	DDX_Control( pDX, IDCANCEL, m_btnCancel );
	DDX_Control( pDX, IDC_UNINSTALL, m_btnUninstall );
	DDX_Control( pDX, IDC_VS_2015, m_btnVS[0] );
	DDX_Control( pDX, IDC_VS_2015_LBL, m_lblVS[0] );
	DDX_Control( pDX, IDC_VS_2017, m_btnVS[1] );
	DDX_Control( pDX, IDC_VS_2017_LBL, m_lblVS[1] );
}


// ----------------------------------------------------------------------------
//		CheckInstalls
// ----------------------------------------------------------------------------

bool
CWizardInstallerDlg::CheckInstalls()
{
	bool	bFound = false;

	for ( int i = 0; i < g_nMaxVersions; i++ ) {
		bool	bAddVC = false;
		CRegKey	rkVS;
		LONG	nResult = rkVS.Open( HKEY_LOCAL_MACHINE, g_strSubKey[i], KEY_READ );
		if ( nResult != ERROR_SUCCESS ) {
			nResult = rkVS.Open( HKEY_LOCAL_MACHINE, g_strSubKey[i], KEY_READ | KEY_WOW64_64KEY );
			bAddVC = true;
		}
		if ( nResult == ERROR_SUCCESS ) {
			ULONG	nDirLen = MAX_PATH;
			nResult = rkVS.QueryStringValue(
				g_strDirLabel[i], m_strVSPath[i].GetBuffer( nDirLen ), &nDirLen );
			m_strVSPath[i].ReleaseBuffer();
			if ( bAddVC ) {
				m_strVSPath[i].Append( L"\\Common7\\IDE\\VC\\" );
			}
			if ( nResult == ERROR_SUCCESS ) {
				m_btnVS[i].ShowWindow( SW_SHOW );
				m_btnVS[i].SetCheck( TRUE );
				m_lblVS[i].SetWindowText( m_strVSPath[i] );
				m_lblVS[i].ShowWindow( SW_SHOW );
				bFound = true;
			}
			else {
				m_strVSPath[i].Empty();
			}
		}
	}

	m_btnUninstall.EnableWindow( FALSE );
	m_btnUninstall.ShowWindow( SW_SHOW );

	for ( int i = 0; i < g_nMaxVersions; i++ ) {
		if ( m_strVSPath[i].GetLength() ) {
			CString	strBasePath[2] = {
				m_strVSPath[i] + g_strVcProjects,
				m_strVSPath[i] + g_strVcProjects + g_strWizardPath,
			};
			for ( size_t p = 0; p < g_strProjectFiles[i]->GetCount(); p++ ) {
				for ( size_t f = 0; f < 2; f++ ) {
					CString	strFile( strBasePath[f] + g_strProjectFiles[i]->GetDst( p ) );
					if ( ::PathFileExists( strFile ) ) {
						m_btnUninstall.EnableWindow( TRUE );
						return bFound;
					}
				}
			}
		}
	}

	return bFound;
}


// ----------------------------------------------------------------------------
//		Setup
// ----------------------------------------------------------------------------

CWizardInstallerDlg::tInstall_States
CWizardInstallerDlg::Setup()
{
	CString	strInstall;
	CString	strUninstall;
	CString	strCancel;
	CString	strVsNotFound;

	strInstall.LoadString( IDS_INSTALL );
	strUninstall.LoadString( IDS_UNINSTALL );
	strCancel.LoadString( IDS_CANCEL );
	strVsNotFound.LoadString( IDS_NO_VS );

	if ( !CheckInstalls() ) {
		m_lblStatus.SetWindowText( strVsNotFound );

		m_btnCancel.ShowWindow( SW_HIDE );

		CString	strLblFinish;
		strLblFinish.LoadString( IDS_LBL_FINISH );
		m_btnOK.SetWindowText( strLblFinish );
		return kState_Done;
	}

	CString	strText( strInstall );
	if ( m_btnUninstall.IsWindowEnabled() ) {
		strText += L"\n\n" + strUninstall;
	}
	strText += L"\n\n" + strCancel;
	m_lblStatus.SetWindowText( strText );

	CString	strLblInstall;
	strLblInstall.LoadString( IDS_LBL_INSTALL );
	m_btnOK.SetWindowText( strLblInstall );

	return kState_Asking;
}


// ----------------------------------------------------------------------------
//		MakePath
// ----------------------------------------------------------------------------

bool
CWizardInstallerDlg::MakePath(
	const CString&	inPath )
{
	CString	strPath( inPath );
	ASSERT( strPath.GetLength() > 0 );
	if ( strPath.GetLength() == 0 ) return false;

	if ( ::PathFileExists( strPath ) ) return true;

	bool bRetVal = false;

	// Make sure the directory name ends in a slash.
	if ( strPath[strPath.GetLength() - 1] != L'\\' ) {
		strPath += L'\\';
	}

	// Create each directory in the path.
	CString	dirName;
	int		nIndex = 0;
	bool	bDone = false;
	while ( !bDone ) {
		// Extract one directory.
		nIndex = strPath.Find( L'\\' );
		if ( nIndex != -1 ) {
			dirName = dirName + strPath.Left( nIndex );
			strPath = strPath.Right( strPath.GetLength() - nIndex - 1 );

			// The first time through, we might have a drive name.
			if ( (dirName.GetLength() >= 1) && (dirName[dirName.GetLength() - 1] != L':') ) {
				bRetVal = (::CreateDirectory( dirName, nullptr ) != FALSE);
			}
			dirName += L'\\';
		}
		else {
			bDone = true;
		}
	}

	// Return the last MakeDirectory() return value.
	return bRetVal;
}


// ----------------------------------------------------------------------------
//		InstallFile
// ----------------------------------------------------------------------------

bool
CWizardInstallerDlg::InstallFile(
	CString	inSrc,
	CString	inDst ) throw( ... )
{
	BOOL	bResult;

	int	nLastSlash = inDst.ReverseFind( '\\' );
	if ( nLastSlash < 0 ) return false;
	CString	strDstPath( inDst.Left( nLastSlash ) );

	if ( !MakePath( strDstPath ) ) return false;

	bResult = ::CopyFile( inSrc, inDst, FALSE );
	if ( bResult == FALSE ) {
		DWORD	dwError = GetLastError();
		CString	strError;
		strError.Format( L"%08X", dwError );

		CString	strMissing;
		strMissing.FormatMessage( IDS_MISSING, inSrc, strError );

		CString	strCaption;
		strCaption.LoadString( IDS_TITLE );

		int nResult = MessageBox(
			strMissing, strCaption, MB_ICONWARNING | MB_OKCANCEL );
		if ( nResult == IDCANCEL ) throw 0;
		return false;
	}

	DWORD	dwAttribs = ::GetFileAttributes( inDst );
	if ( dwAttribs & FILE_ATTRIBUTE_READONLY ) {
		dwAttribs &= ~FILE_ATTRIBUTE_READONLY;
		::SetFileAttributes( inDst, dwAttribs );
	}

	return true;
}


// ----------------------------------------------------------------------------
//		InstallWizard
// ----------------------------------------------------------------------------

bool
CWizardInstallerDlg::InstallWizard(
	int inIndex )
{
	if ( (inIndex < 0) || (inIndex >= g_nMaxVersions ) ) return false;

	bool	bResult = true;
	size_t	nCount = 0;
	size_t	nFailures = 0;	// debug only.
#ifdef _DEBUG
	CString	strSrcBase( L"C:\\wp\\wpdn\\sdk\\OmniEngine\\11.0\\Wizard\\" + g_strWizardName + L"\\" );
#else // _DEBUG
	CString	strSrcBase( g_strSourcePath );
#endif // _DEBUG
	CString	strCurrentDirectory;
	::GetCurrentDirectory( MAX_PATH, strCurrentDirectory.GetBuffer( MAX_PATH ) );
	strCurrentDirectory.ReleaseBuffer();

	try {
		// Destination is Visual Studio's vcprojects.
		CString	strProjBase = m_strVSPath[inIndex] + g_strVcProjects + g_strWizardPath;
		bResult = MakePath( strProjBase );
		if ( bResult ) {
			for ( size_t p = 0; p < g_strProjectFiles[inIndex]->GetCount(); p++ ) {
				CString	strSrc = strSrcBase + g_strProjectFiles[inIndex]->GetSrc( p );
				CString	strDst = strProjBase + g_strProjectFiles[inIndex]->GetDst( p );
				if ( InstallFile( strSrc, strDst ) ) {
					nCount++;
				}
				else {
					nFailures++;
				}
			}

			// Destination is Visual Studio's VCWizards + the Wizard's name.
			CString	strWizBase = m_strVSPath[inIndex] + g_strVcWizards + g_strWizardPath;
			bResult = MakePath( strWizBase );
			if ( bResult ) {
				for ( size_t p = 0; p < g_strWizardFiles[inIndex]->GetCount(); p++ ) {
					CString	strSrc = strSrcBase + g_strWizardFiles[inIndex]->GetSrc( p );
					CString	strDst = strWizBase + g_strWizardFiles[inIndex]->GetDst( p );
					if ( InstallFile( strSrc, strDst ) ) {
						nCount++;
					}
					else {
						nFailures++;
					}
				}
			}
		}
	}
	catch ( int ) {
		return false;
	}

	ASSERT( nFailures == 0 );

	if ( nCount != (g_strProjectFiles[inIndex]->GetCount() + g_strWizardFiles[inIndex]->GetCount()) ) {
		return false;
	}

	return true;
}


// ----------------------------------------------------------------------------
//		Install
// ----------------------------------------------------------------------------

CWizardInstallerDlg::tInstall_States
CWizardInstallerDlg::Install()
{
	CString	strInstallOk;
	CString	strInstallFail;

	strInstallOk.LoadString( IDS_INSTALL_OK );
	strInstallFail.LoadString( IDS_INSTALL_FAIL );

	bool	bSuccess = true;
	for ( int i = 0; i < g_nMaxVersions; i++ ) {
		if ( m_strVSPath[i].GetLength() ) {
			if ( m_btnVS[i].GetCheck() == BST_CHECKED ) {
				if ( !InstallWizard( i ) ) {
					bSuccess = false;
				}
			}
		}
	}
	if ( bSuccess ) {
		m_lblStatus.SetWindowText( strInstallOk );
	}
	else {
		m_lblStatus.SetWindowText( strInstallFail );
	}

	CString	strLblFinish;
	strLblFinish.LoadString( IDS_LBL_FINISH );
	m_btnOK.SetWindowText( strLblFinish );
	m_btnUninstall.ShowWindow( SW_HIDE );
	m_btnCancel.ShowWindow( SW_HIDE );

	return kState_Done;
}


// ----------------------------------------------------------------------------
//		RemovePlugin
// ----------------------------------------------------------------------------

bool
CWizardInstallerDlg::RemovePlugin(
	int inIndex )
{
	if ( (inIndex < 0) || (inIndex >= g_nMaxVersions ) ) return false;

	size_t	nCount = 0;
	size_t	nFailures = 0;	// Just for debugging.
	CString	strFile;

	// Older versions store the project files in VC\vcprojects.
	// Current versions store the project files in VC\vcprojects\OmniEngineXX.
	CString	strProjFolder[2] = {
		m_strVSPath[inIndex] + g_strVcProjects,
		m_strVSPath[inIndex] + g_strVcProjects + g_strWizardPath
	};
	for ( size_t p = 0; p < g_strProjectFiles[inIndex]->GetCount(); p++ ) {
		for ( size_t i = 0; i < 2; i++ ) {
			strFile = strProjFolder[i] + g_strProjectFiles[inIndex]->GetDst( p );
			if ( ::DeleteFile( strFile ) ) {
				nCount++;
			}
			else {
				nFailures++;
			}
		}
	}

	::FolderExists();
	// Remove the folder VC\vcprojects\OmniEngineXX.
	if ( ::RemoveDirectory( strProjFolder[1] ) ) {
		nCount++;
	}
	else {
		nFailures++;
	}

	// Older versions may have fewer files, so don't count the files
	// removed or note errors.
	CString	strWizFolder( m_strVSPath[inIndex] + g_strVcWizards + g_strWizardPath );
	for ( size_t i = 0; i < g_strWizardFiles[inIndex]->GetCount(); i++ ) {
		strFile = strWizFolder + g_strWizardFiles[inIndex]->GetDst( i );
		if ( !::DeleteFile( strFile ) ) {
			nFailures++;	// Just for debugging.
		}

		CString	strFileFolder(g_strWizardFiles[inIndex]->GetDst( i ) );
		CString	strFolderToRemove;
		do {
			int	nSlashIndex = strFileFolder.ReverseFind( '\\' );
			if ( nSlashIndex < 0 ) break;
			strFileFolder = strFileFolder.Left( nSlashIndex );
			strFolderToRemove = strWizFolder + strFileFolder;
		} while ( ::RemoveDirectory( strFolderToRemove ) );
	}

	for ( size_t i = 0; i < g_strExtraUninstallFiles.GetCount(); i++ ) {
		strFile = strWizFolder + g_strExtraUninstallFiles[i];
		if ( !::DeleteFile( strFile ) ) {
			nFailures++;	// Just for debugging.
		}

		CString	strFileFolder( g_strExtraUninstallFiles[i] );
		CString	strFolderToRemove;
		do {
			int	nSlashIndex = strFileFolder.ReverseFind( '\\' );
			if ( nSlashIndex < 0 ) break;
			strFileFolder = strFileFolder.Left( nSlashIndex );
			strFolderToRemove = strWizFolder + strFileFolder;
		} while ( ::RemoveDirectory( strFolderToRemove ) );
	}

	if ( ::RemoveDirectory( strWizFolder ) ) {
		nCount++;
	}
	else {
		nFailures++;
	}

	// Count is all the files and one folder deleted.
	if ( nCount <= (g_strProjectFiles[inIndex]->GetCount() + 1) ) {
		return false;
	}

	return true;
}


// ----------------------------------------------------------------------------
//		Uninstall
// ----------------------------------------------------------------------------

CWizardInstallerDlg::tInstall_States
CWizardInstallerDlg::Uninstall()
{
	CString	strUninstallOk;
	CString	strUninstallFail;

	strUninstallOk.LoadString( IDS_UNINSTALL_OK );
	strUninstallFail.LoadString( IDS_UNINSTALL_FAIL );

	bool	bSuccess = true;
	for ( int i = 0; i < g_nMaxVersions; i++ ) {
		if ( m_strVSPath[i].GetLength() ) {
			if ( m_btnVS[i].GetCheck() == BST_CHECKED ) {
				if ( !RemovePlugin( i ) ) {
					bSuccess = false;
				}
			}
		}
	}
	if ( bSuccess ) {
		m_lblStatus.SetWindowText( strUninstallOk );
	}
	else {
		m_lblStatus.SetWindowText( strUninstallFail );
	}

	CString	strLblFinish;
	strLblFinish.LoadString( IDS_LBL_FINISH );
	m_btnOK.SetWindowText( strLblFinish );
	m_btnUninstall.ShowWindow( SW_HIDE );
	m_btnCancel.ShowWindow( SW_HIDE );

	return kState_Done;
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CWizardInstallerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_UNINSTALL, OnBnClickedUninstall)
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnInitDialog
// ----------------------------------------------------------------------------

BOOL CWizardInstallerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon( m_hIcon, TRUE );			// Set big icon
	SetIcon( m_hIcon, FALSE );		// Set small icon

	return TRUE;  // return TRUE  unless you set the focus to a control
}


// ----------------------------------------------------------------------------
//		OnPaint
//
//	If you add a minimize button to your dialog, you will need the code below
//	to draw the icon.  For MFC applications using the document/view model,
//	this is automatically done for you by the framework.
// ----------------------------------------------------------------------------

void CWizardInstallerDlg::OnPaint()
{
	if ( IsIconic() ) {
		CPaintDC	dc( this ); // device context for painting

		SendMessage( WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0 );

		// Center icon in client rectangle
		int		cxIcon = GetSystemMetrics( SM_CXICON );
		int		cyIcon = GetSystemMetrics( SM_CYICON );
		CRect	rect;
		GetClientRect( &rect );

		int		x = (rect.Width() - cxIcon + 1) / 2;
		int		y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon( x, y, m_hIcon );
	}
	else {
		CDialog::OnPaint();
	}
}


// ----------------------------------------------------------------------------
//		OnQueryDragIcon\
//
//	The system calls this function to obtain the cursor to display while the
//	user drags the minimized window.
// ----------------------------------------------------------------------------

HCURSOR
CWizardInstallerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>( m_hIcon );
}


// ----------------------------------------------------------------------------
//		OnBnClickedOk
// ----------------------------------------------------------------------------

void
CWizardInstallerDlg::OnBnClickedOk()
{
	switch ( m_nState ) {
	case kState_Load:
		m_nState = Setup();
		break;

	case kState_Asking:
		m_nState = Install();
		break;

	case kState_Done:
		OnOK();
		break;
	}
}


// ----------------------------------------------------------------------------
//		OnBnClickedUninstall
// ----------------------------------------------------------------------------

void
CWizardInstallerDlg::OnBnClickedUninstall()
{
	m_nState = Uninstall();
}
