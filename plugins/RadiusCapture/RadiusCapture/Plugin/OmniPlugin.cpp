// =============================================================================
//	OmniPlugin.cpp
// =============================================================================
//	Copyright (c) 2010-2011 WildPackets, Inc. All rights reserved.

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

// =============================================================================
//		Global Data
// =============================================================================

extern CContextManagerPtr	GetContextManager();

GUID			g_PluginId = { 0xE5FB76FA, 0x95FB, 0x41DE, { 0xA7, 0x6C, 0x12, 0x14, 0x2C, 0x8C, 0xC7, 0x10 } };
CGlobalId		COmniPlugin::s_idClass( g_PluginId );
CPeekString		COmniPlugin::s_strName( L"RadiusCaptureOE" );


// =============================================================================
//		COmniPlugin
// =============================================================================

COmniPlugin::COmniPlugin()
	:	CPeekPlugin()
	,	m_bInitialized( false )
#ifdef IMPLEMENT_PLUGINMESSAGE
	,	m_MessageQueue( this )
#endif // IMPLEMENT_PLUGINMESSAGE
{
	CPeekString	strEnvProgramData;
	strEnvProgramData.GetEnvironmentVariable( L"ALLUSERSPROFILE" );

	CPeekOutString	strPrefsFilePath;
	strPrefsFilePath << strEnvProgramData << L"\\Application Data\\WildPackets\\Plugins";
	CFileEx::MakePath( strPrefsFilePath );

	strPrefsFilePath << L"\\RadiusCaptureOE.xml";
	m_strPrefsFileName = strPrefsFilePath;
}


#ifdef IMPLEMENT_TAB
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
#endif


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

#ifdef IMPLEMENT_PLUGINMESSAGE
	if ( HasRemotePlugin() && m_pRemotePlugin->GetPeekProxy().IsConsole() ) {
		CMessagePing*	pPing( new CMessagePing() );
		FnResponseProcessor	fnProcess = &CPeekMessageProcess::ProcessPluginResponse;
		PostPluginMessage( CPeekMessagePtr( pPing ), this, fnProcess );
	}
#endif // IMPLEMENT_PLUGINMESSAGE

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

#ifdef IMPLEMENT_TAB
	CreateTab();
#endif

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
	// Never called
	ASSERT( 0 );

	return false;
}


// -----------------------------------------------------------------------------
//		OnReadPrefs
// -----------------------------------------------------------------------------

int
COmniPlugin::OnReadPrefs()
{
/*
	// Prevent reading the Options from file on the Console.
	if ( !(HasRemotePlugin() && m_pRemotePlugin->GetPeekProxy().IsConsole()) ) {
		CPeekDataModeler	dmForensicsPrefs( GetPrefsFileName(), L"plugin" );
		m_Options.Model( dmForensicsPrefs );
	}
*/
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnWritePrefs
// -----------------------------------------------------------------------------

int
COmniPlugin::OnWritePrefs()
{
/*
	// Prevent saving the Options to file on the Console.
	if ( !(HasRemotePlugin() && m_pRemotePlugin->GetPeekProxy().IsConsole()) ) {
		CPeekDataModeler	dmForensicsPrefs( L"plugin" );
		m_Options.Model( dmForensicsPrefs );

		CPeekPersistFile	spFile = dmForensicsPrefs.GetXml();
		if ( spFile ) spFile->Save( GetPrefsFileName(), FALSE );
	}
*/
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		SetOptions
// -----------------------------------------------------------------------------

void
COmniPlugin::SetOptions(
	CPeekDataModeler&	inPrefs )
{
	m_Options.Model( inPrefs );

	OnWritePrefs();
}


// .............................................................................
//		Engine-Only Methods
// .............................................................................

#ifdef IMPLEMENT_PLUGINMESSAGE
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
			case CMessageImportExport::s_nMessageType:
				nResult = ProcessImportExportOptions( spMessage.get() );
				break;
			default:
				nResult = PEEK_PLUGIN_FAILURE;
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
		const CPeekString&	strForensics = COptions::GetTagForensics();
		const COptions&		optForensics = GetOptions();
		CMessageGetAllOptions::TNamedOptions	noDefaults = { GUID_NULL, strForensics, optForensics };
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

				COptions&	optContext = pContext->GetOptions();

				CMessageGetAllOptions::TNamedOptions	noContext = { idContext, strName, optContext };
				ayNamedOptions.Add( noContext );
			}
		}
	}

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		ProcessImportExportOptions
// -----------------------------------------------------------------------------

int
COmniPlugin::ProcessImportExportOptions(
	CPeekMessage* ioMessage )
{
	CPeekString strResultMessage( L"Unknown failure reading file." );
	bool	bSuccess( true );

	CMessageImportExport*	pMessage = dynamic_cast<CMessageImportExport*>( ioMessage );
	ASSERT( pMessage != NULL );
	if ( pMessage == NULL ) return PEEK_PLUGIN_FAILURE;

	CPeekString strFilePath = pMessage->GetImportExportMessage();
	const bool		bIsImport = pMessage->IsImport();

	if ( !ValidateImportExportFile( strFilePath, strResultMessage, bIsImport ) ) {
		bSuccess = false;
	}

	COptions theOptions;

	if ( bSuccess ) {
		if ( bIsImport ) {

			CPeekDataModeler	dmImportOptions( strFilePath, L"PluginImport" );
			if ( !theOptions.Model( dmImportOptions ) ) {
				strResultMessage = L"Unable to read options file from disk.";
				bSuccess = false;
			}
		}
		else {  // Is export
			CPeekDataModeler	dmExportOptions( L"PluginImport" );
			theOptions = pMessage->GetOptions();
			if ( !theOptions.Model( dmExportOptions ) ) {
				strResultMessage = L"Unable to read export options from the network.";
				bSuccess = false;
			}

			CPeekPersistFile	spFile = dmExportOptions.GetXml();
			if ( !spFile ) {
				strResultMessage = L"Unable to retrieve xml for writing options file to disk.";
				bSuccess = false;
			}
			
			if ( bSuccess ) {
				HeResult hr = spFile->Save( strFilePath, FALSE );
				if ( hr != HE_S_OK ) {
					strResultMessage = L"Unable to write options file to disk.";
					bSuccess = false;
				}
			}
		}
	}

	// Create the modeler.
	if ( !pMessage->StartResponse() ) {
		return PEEK_PLUGIN_FAILURE;
	}

	if ( bSuccess ) {
		strResultMessage = L"Success";
	}

	pMessage->SetImportExportMessage( strResultMessage );

	// Set the response (output).
	if ( bIsImport && bSuccess ) {
		pMessage->SetOptions( theOptions );
	}

	return PEEK_PLUGIN_SUCCESS;
}

#endif // IMPLEMENT_PLUGINMESSAGE


// .............................................................................
//		Console-Only Methods
// .............................................................................

#ifdef IMPLEMENT_PLUGINMESSAGE
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
#endif // IMPLEMENT_PLUGINMESSAGE


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


#ifdef IMPLEMENT_PLUGINMESSAGE
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
#ifdef IMPLEMENT_TAB
			CreateTab();
#endif
			break;
	}
}


// -----------------------------------------------------------------------------
//  ValidateImportExportFile
// -----------------------------------------------------------------------------

bool
COmniPlugin::ValidateImportExportFile( 
	CPeekString& inoutFilePath,
	CPeekString& outResultMessage,
	bool		 bIsImport )
{
	ASSERT( inoutFilePath.GetLength() > 3 );
	if ( inoutFilePath.GetLength() < 4 ) {
		outResultMessage = L"Options file invalid length.";
		return false;
	}

	ASSERT( inoutFilePath.GetAt(1) == L':' );
	
	const size_t nLength( inoutFilePath.GetLength() );

#ifdef _DEBUG
	for ( size_t i = 0; i < nLength; i++ ) {
		ASSERT( FilePath::IsValidPathChar( inoutFilePath[i] ) );
	}
#endif // _DEBUG

	CPeekString	strNormFilePath;
	CPeekString	strPath;
	
	// Normalize separators
	for ( size_t i = 0; i < nLength; i++ ) {
		wchar_t ch = inoutFilePath[i];
		if ( ch == L'\\' || ch == L'/' ) {
			ch = L'\\';
		}
		strNormFilePath += ch; 
	}

	// Extract the path
	bool bHasExtension( false );
	for ( size_t i = nLength - 1; i >= 0; i-- ) {
		wchar_t ch = strNormFilePath[i];
		if ( ch == L'\\' ) {
			strPath = strNormFilePath.Left( i );
			break;
		}
		else if ( ch == L'.' ) {
			bHasExtension = true;
		}
	}

	// Add file extension if missing
	if ( !bHasExtension ) {
		strNormFilePath += L".xml";
	}

	// For export, create the path if it doesn't already exist
	if ( !bIsImport ) {
		bool bSuccess = MakePath( strPath );
		if ( !bSuccess ) {
			outResultMessage = L"Unable to create options file path on the engine.";
			return false;
		}
	}

	UINT uOpenFlags( 0 );

	if ( bIsImport ) {
		uOpenFlags = CFileEx::modeRead;
	}
	else {
		uOpenFlags = CFileEx::modeWrite | CFileEx::modeCreate;
	}

	CFileEx fImportExport;

	// Open the file as a test
	bool bOpenSuccess = fImportExport.Open( strNormFilePath, uOpenFlags );

	if ( !bOpenSuccess ) {
		wchar_t szError[MAX_PATH + 1];
		DWORD dw = GetLastError(); 

		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) szError,
			MAX_PATH, NULL );

		if ( lstrlenW( szError ) > 0 ) {
			outResultMessage = (CPeekString)szError;
		}
		else {
			if ( bIsImport )	outResultMessage = L"Unable to open file.";
			else				outResultMessage = L"Unable to create file.";
		}

		return false;
	}

	fImportExport.Close();

	if ( bOpenSuccess )	inoutFilePath = strNormFilePath;

	return true;
}

#endif // IMPLEMENT_PLUGINMESSAGE