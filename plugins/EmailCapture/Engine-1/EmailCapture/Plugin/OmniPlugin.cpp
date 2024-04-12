// =============================================================================
//	OmniPlugin.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "Peek.h"
#include "AboutDialog.h"
#include "TabHost.h"
#include "PluginTab.h"
#include "MemUtil.h"
#include "Packet.h"
#include "PeekMessage.h"
#include "Snapshot.h"
#include "MediaTypes.h"
#include "OptionsDialog.h"
#include "RemotePlugin.h"
#include "ContextManager.h"
#include "FileEx.h"
#include <wtypes.h>
#ifdef _WIN32
#include "DelayImp.h"
#endif // _WIN32


// =============================================================================
//		Global Data
// =============================================================================

extern CContextManagerPtr	GetContextManager();

GUID			g_PluginId = { 0x75AC6E1C, 0x55AD, 0x4A5A, { 0xA2, 0xD9, 0x49, 0x73, 0x52, 0x1E, 0xF8, 0x8F } };
CGlobalId		COmniPlugin::s_idClass( g_PluginId );
CPeekString		COmniPlugin::s_strName( L"EmailCaptureOE" );

#if _WIN32
// ============================================================================
//		ExpFilter
// ============================================================================

int
ExpFilter(
	EXCEPTION_POINTERS*	pPtrs,
	const CArrayString&	inPathList )
{
	if ( (pPtrs == NULL) || (pPtrs->ExceptionRecord->NumberParameters == 0) ) {
		return EXCEPTION_EXECUTE_HANDLER;
	}

	PDelayLoadInfo	pInfo( reinterpret_cast<PDelayLoadInfo>( pPtrs->ExceptionRecord->ExceptionInformation[0] ) );
	if ( pInfo == NULL ) return EXCEPTION_EXECUTE_HANDLER;

	CPeekStringA	strNameA( pInfo->szDll );
	CPeekString		strName( strNameA );
	const size_t	nCount( inPathList.GetCount() );
	for ( size_t i = 0; i < nCount; i++ ) {
		CPeekString	strPath( inPathList[i] );
		if ( strPath.Right( 1 ) != L"\\" ) {
			strPath += L"\\";
		}
		strPath += strName;

		HMODULE	hLib( LoadLibrary( strPath ) );
		if ( hLib != NULL ) {
			pInfo->pfnCur = GetProcAddress( hLib, pInfo->dlp.szProcName );
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}

	return EXCEPTION_EXECUTE_HANDLER;
}


// ============================================================================
//		LoadAll
// ============================================================================

bool
LoadAll(
	LPCSTR				inDllName,
	const CArrayString&	inPathList  )
{
	__try { 
		HRESULT	hr( __HrLoadAllImportsForDll( inDllName ) );
		if ( FAILED( hr ) ) return false;
	} 
	__except( ExpFilter( GetExceptionInformation(), inPathList ) ) {
		return false;
	}
	return true;
}
#endif	// _WIN32


// =============================================================================
//		COmniPlugin
// =============================================================================

COmniPlugin::COmniPlugin()
	:	CPeekPlugin()
	,	m_bInitialized( false )
	,	m_MessageQueue( this )
{
}


// -----------------------------------------------------------------------------
//		CreateTab
// -----------------------------------------------------------------------------

void
COmniPlugin::CreateTab()
{
	if ( GetInstalledOnEngine() == CPeekPlugin::kOnEngine_Unknown ) return;

	if ( CPeekProxy::IsConsole() && IsInstalledOnEngine() && !m_TabHost.HasChild() ) {
		AFX_MANAGE_STATE( AfxGetStaticModuleState() );

		CPeekString	strCmd( COmniPlugin::GetClassId().Format() );
		CPeekString	strClass( L"STATIC" );
		HWND		hWnd( NULL );

		_ASSERTE( m_pRemotePlugin != NULL );
		if ( m_pRemotePlugin == NULL ) return;

		CConsoleUI	ConsoleUI = m_pRemotePlugin->GetConsole();
		hWnd = ConsoleUI.AddTabWindow( strCmd, strClass, s_strName );
		if ( hWnd ) {
			// Subclass the static window Peek created.
#undef SubclassWindow							// The MSVC compiler picks up this macro
			m_TabHost.SubclassWindow( hWnd );	// instead of this function.

			 //Create a dialog to be embedded in the Parent window. The Parent window will delete it.
			CPluginTab*	pTab = new CPluginTab( this );
			_ASSERTE( pTab != NULL );
			if ( pTab != NULL ) {
				pTab->Create( IDD_PLUGIN_TAB, &m_TabHost );
				m_TabHost.SetChild( pTab );
			}
		}
	}
}


// -----------------------------------------------------------------------------
//		GetInstanceHandle
// -----------------------------------------------------------------------------

HINSTANCE
COmniPlugin::GetInstanceHandle() const
{
	// Can not move this function to the header file.
	// CRemotePlugin is not defined there.
	if ( m_pRemotePlugin != NULL ) {
		return m_pRemotePlugin->GetInstanceHandle();
	}
	return NULL;
}


// -----------------------------------------------------------------------------
//		GetName
// -----------------------------------------------------------------------------

const CPeekString&
COmniPlugin::GetName()
{
	if ( s_strName.IsEmpty() ) {
#ifdef PEEK_UI
		s_strName.LoadString( IDS_PLUGIN_NAME );
#endif
	}

	_ASSERTE( !s_strName.IsEmpty() );

	return s_strName;
}


// -----------------------------------------------------------------------------
//		OnLoad
// -----------------------------------------------------------------------------

int
COmniPlugin::OnLoad(
	CRemotePlugin*	inRemotePlugin )
{
	int	nResult = CPeekPlugin::OnLoad( inRemotePlugin );
	if ( nResult != PEEK_PLUGIN_SUCCESS ) {
		return nResult;
	}
	m_bInitialized = true;

	if ( HasRemotePlugin() && m_pRemotePlugin->GetPeekProxy().IsConsole() ) {
		CMessagePing*	pPing( new CMessagePing() );
		FnResponseProcessor	fnProcess = &CPeekMessageProcess::ProcessPluginResponse;
		PostPluginMessage( CPeekMessagePtr( pPing ), this, fnProcess );
	}

	CArrayString	saPathList;
	{
		CPeekString strPath;
		strPath.GetEnvironmentVariable( L"CommonProgramFiles" );
		strPath.Append( L"\\WildPackets" );
		saPathList.Add( strPath );
	}

#ifdef WIN64
	LoadAll( "mimepp64.dll", saPathList );
#else
	LoadAll( "mimepp.dll", saPathList );
#endif

	try {
		CPeekString	strEnvProgramData;
		strEnvProgramData.GetEnvironmentVariable( L"ALLUSERSPROFILE" );

		CPeekString	strPrefsFilePath( strEnvProgramData );
		strPrefsFilePath.Append( L"\\Application Data\\WildPackets\\Plugins" );
		CFileEx::MakePath( strPrefsFilePath );

		strPrefsFilePath.Append( L"\\" );

		CPeekString	strLogFilePath( strPrefsFilePath );

		// Build the Config file name.
		strPrefsFilePath.Append( s_strName );
		strPrefsFilePath.Append( L".xml" );

		// Build the Change Log file name.
		strLogFilePath.Append( L"ChangeLog" );
		strLogFilePath.Append( s_strName );
		strLogFilePath.Append( L".txt" );

		m_strPrefsFileName = strPrefsFilePath;
		m_strChangeLogFileName = strLogFilePath;

		mimepp::Initialize();
	}
	catch (...) {
		return PEEK_PLUGIN_FAILURE;
	}

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnLoadView
// -----------------------------------------------------------------------------

int
COmniPlugin::OnLoadView()
{
	int	nResult = CPeekPlugin::OnLoadView();
	if ( nResult != PEEK_PLUGIN_SUCCESS ) {
		return nResult;
	}

	CreateTab();

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnUnload
// -----------------------------------------------------------------------------

int
COmniPlugin::OnUnload()
{
	m_bInitialized = false;

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnAbout
//		: Implements IPeekPluginAbout::ShowAbout
// -----------------------------------------------------------------------------

int
COmniPlugin::OnAbout()
{
#ifdef PEEK_UI
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CAboutDialog	dlgAbout( GetInstanceHandle() );
	dlgAbout.DoModal();
#endif	// PEEK_UI

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnOptions
// -----------------------------------------------------------------------------

bool
COmniPlugin::OnOptions()
{
#ifdef PEEK_UI
	COptionsDialog theOptionsDlg( m_Options );
	if ( theOptionsDlg.DoModal() == IDOK ) {
		m_Options = theOptionsDlg.GetOptions();
		return true;
	}
#endif	// PEEK_UI

	return false;
}


// -----------------------------------------------------------------------------
//		OnReadPrefs
// -----------------------------------------------------------------------------

int
COmniPlugin::OnReadPrefs()
{
	// Prevent reading the Options from file on the Console.
	if ( !(HasRemotePlugin() && m_pRemotePlugin->GetPeekProxy().IsConsole()) ) {
		CPeekDataModeler	dmForensicsPrefs( GetPrefsFileName(), L"plugin" );
		m_Options.Model( dmForensicsPrefs );
	}

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnWritePrefs
// -----------------------------------------------------------------------------

int
COmniPlugin::OnWritePrefs()
{
	// Prevent saving the Options to file on the Console.
	if ( !(HasRemotePlugin() && m_pRemotePlugin->GetPeekProxy().IsConsole()) ) {
		CPeekDataModeler	dmForensicsPrefs( L"plugin" );
		m_Options.Model( dmForensicsPrefs );

		CPeekPersistFile	spFile = dmForensicsPrefs.GetXml();
		if ( spFile ) spFile->Save( GetPrefsFileName(), FALSE );
	}

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		SetOptions
// -----------------------------------------------------------------------------

void
COmniPlugin::SetOptions(
	CPeekDataModeler&	inPrefs )
{
	COptions	optNew;
	optNew.Model( inPrefs );

	m_Options.LogUpdate( Tags::kxDefaultOptions.GetA(), optNew, GetLogFileName() );

	OnWritePrefs();
}


// .............................................................................
//		Engine-Only Methods
// .............................................................................

// -----------------------------------------------------------------------------
//		OnProcessPluginMessage
//		: Implements IProcessPluginMessage::ProcessPluginMessage
// -----------------------------------------------------------------------------

int
COmniPlugin::OnProcessPluginMessage(
	CPeekStream& inMessage,
	CPeekStream& outResponse )
{
	int	nResult = PEEK_PLUGIN_FAILURE;

	// Create and Load the appropriate Message object.
	CPeekMessagePtr	spMessage = MessageFactory( inMessage );
	if ( spMessage.get() != NULL ) {
		switch ( spMessage->GetType() ) {
			case CMessagePing::s_nMessageType:
				nResult = ProcessPing( spMessage.get() );
				break;
			case CMessageGetAllOptions::s_nMessageType:
				nResult = ProcessGetAllOptions( spMessage.get() );
				break;
		}

		if ( nResult == PEEK_PLUGIN_SUCCESS ) {
			spMessage->StoreResponse( outResponse );
		}
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		ProcessPing
// -----------------------------------------------------------------------------

int
COmniPlugin::ProcessPing(
	CPeekMessage* ioMessage )
{
	CMessagePing*	pMessage = dynamic_cast<CMessagePing*>( ioMessage );
	_ASSERTE( pMessage != NULL );
	if ( pMessage == NULL ) return PEEK_PLUGIN_FAILURE;

	// No input to process.

	// Create the response modeler.
	if ( !pMessage->StartResponse() ) return PEEK_PLUGIN_FAILURE;

	// No response content.

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		ProcessGetAllOptions
// -----------------------------------------------------------------------------

int
COmniPlugin::ProcessGetAllOptions(
	CPeekMessage* ioMessage )
{
	CMessageGetAllOptions*	pMessage = dynamic_cast<CMessageGetAllOptions*>( ioMessage );
	_ASSERTE( pMessage != NULL );
	if ( pMessage == NULL ) return PEEK_PLUGIN_FAILURE;

	// No input to process.

	// Create the response modeler.
	if ( !pMessage->StartResponse() ) return PEEK_PLUGIN_FAILURE;

	// Build (set) the Options list.
	CMessageGetAllOptions::CNamedOptionsList&	ayNamedOptions = pMessage->GetNamedOptions();

	{	// confine the scope of these variables.
		const CPeekString&	strDefaults = COptions::GetTagDefaults();
		const COptions&		optDefaults = GetOptions();
		CMessageGetAllOptions::TNamedOptions	noDefaults = { GUID_NULL, strDefaults, optDefaults };
		ayNamedOptions.Add( noDefaults );
	}

	CContextManagerPtr	pContextManager = GetContextManager();
	if ( pContextManager ) {
		size_t	nCount = pContextManager->GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CPeekContext*		pPeekContext = pContextManager->GetItem( i );
			CPeekEngineContext*	pContext = dynamic_cast<CPeekEngineContext*>( pPeekContext );
			if ( pContext ) {
				if ( pContext->GetId().IsNull() ) continue;
				CGlobalId	idContext( pContext->GetId() );
				CPeekString	strName( pContext->GetCaptureName() );
				if ( strName.IsEmpty() ) continue;

				COptionsPtr	optContext = pContext->GetOptions();

				CMessageGetAllOptions::TNamedOptions	noContext = { idContext, strName, *optContext };
				ayNamedOptions.Add( noContext );
			}
		}
	}

	return PEEK_PLUGIN_SUCCESS;
}


// .............................................................................
//		Console-Only Methods
// .............................................................................

// -----------------------------------------------------------------------------
//		PostPluginMessage
// -----------------------------------------------------------------------------

bool
COmniPlugin::PostPluginMessage(
	CPeekMessagePtr			inMessage,
	CPeekMessageProcess*	inMsgProc,
	FnResponseProcessor		inResponseProc )
{
	return m_MessageQueue.PostPluginMessage( inMessage, inMsgProc, inResponseProc );
}


// -----------------------------------------------------------------------------
//		OnContextPrefs
// -----------------------------------------------------------------------------

int
COmniPlugin::OnContextPrefs(
	CPeekDataModeler& ioPrefs )
{
	// Test ioPrefs.IsLoading() or ioPrefs->IsStoring()
	//  to take load/store specific actions.

	// To always display the default options, don't load
	// the preferences when the Id is NULL (case 2 above).
	if ( ioPrefs.IsLoading() && (m_pRemotePlugin && m_pRemotePlugin->GetContextId().IsNull()) ) {
		m_Options.Reset();
	}
	else {
		m_Options.Model( ioPrefs );
	}

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnProcessPluginResponse
// -----------------------------------------------------------------------------

int
COmniPlugin::OnProcessPluginResponse(
	CPeekStream&	inMessage,
	HeResult		inMsgResult,
	DWORD			inTransId )
{
	int	nResult = PEEK_PLUGIN_FAILURE;

	CPeekMessageItem&	miMessage = m_MessageQueue.GetAt( inTransId );
	if ( miMessage.IsValid() ) {
		miMessage.CallResponseProcessor( inMsgResult, inMessage );
		nResult = PEEK_PLUGIN_SUCCESS;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		ProcessPluginResponse
// -----------------------------------------------------------------------------

void
COmniPlugin::ProcessPluginResponse(
	HeResult			inResult,
	CPeekMessagePtr		inMessage,
	const CPeekStream&	/*inResponse*/ )
{
	switch ( inMessage->GetType() ) {
		case CMessagePing::s_nMessageType:
			SetInstalledOnEngine( (HE_SUCCEEDED( inResult ))
				? kOnEngine_True
				: kOnEngine_False );
			CreateTab();
			break;
	}
}
