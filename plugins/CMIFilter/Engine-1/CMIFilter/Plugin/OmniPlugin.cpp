// =============================================================================
//	OmniPlugin.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "Peek.h"
#include "MemUtil.h"
#include "Packet.h"
#include "PeekMessage.h"
#include "Snapshot.h"
#include "MediaTypes.h"
#include "RemotePlugin.h"
#include "ContextManager.h"
#include "FileEx.h"

#ifdef _WIN32
#include "AboutDialog.h"
#include "OptionsDialog.h"
#include "TabHost.h"
#include "PluginTab.h"
#include <wtypes.h>
#endif // _WIN32


// =============================================================================
//		Global Data
// =============================================================================

extern CContextManagerPtr	GetContextManager();

GUID			g_PluginId = { 0xB790B045, 0x1E0C, 0x4389, { 0x97, 0x56, 0x6, 0xEF, 0x7C, 0x34, 0xEF, 0xD3 } };
CGlobalId		COmniPlugin::s_idClass( g_PluginId );
CPeekString		COmniPlugin::s_strName( L"CMIFilterOE" );


// =============================================================================
//		COmniPlugin
// =============================================================================

COmniPlugin::COmniPlugin()
	:	CPeekPlugin()
	,	m_bInitialized( false )
#if defined(_WIN32) && defined(IMPLEMENT_PLUGINMESSAGE)
	,	m_MessageQueue( this )
#endif // _WIN32 && IMPLEMENT_PLUGINMESSAGE
{
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
//		GetPrefsFileName
// -----------------------------------------------------------------------------

const CPeekString&
COmniPlugin::GetPrefsFileName()
{
	if ( m_strPrefsFileName.empty() ) {
		CRemotePlugin*	pRemotePlugin( GetRemotePlugin() );
		_ASSERTE( pRemotePlugin != nullptr );
		if ( pRemotePlugin != nullptr ) {
			CAppConfig	AppConfig( pRemotePlugin->GetPeekProxy().GetAppConfig() );
			if ( AppConfig.IsValid() ) {
				CPeekString	strDirectory;
	// TODO determine path for Linux
#if NDEBUG
				strDirectory = AppConfig.GetConfigDir();
#else
				strDirectory = AppConfig.GetLogDir();
#endif
				if ( !strDirectory.empty() ) {
					FilePath::Path	path( strDirectory );
#ifdef _WIN32
					CPeekString		pathDir( path.GetLastDir() );
					pathDir.MakeLower();
					if ( pathDir == L"omniengine" ) {
						path.PopDir( true );
					}
#endif // _WIN32

					path.Append( L"Plugins", true );
					CFileEx::MakePath( path.get() );

					path.SetFileName( L"CMIFilterOE.xml" );
					m_strPrefsFileName = path.get();
				}
			}
		}
	}

	_ASSERTE( !m_strPrefsFileName.empty() );
	return m_strPrefsFileName;
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
	// the preferences when the Id is nullptr (case 2 above).
	if ( ioPrefs.IsLoading() && (m_pRemotePlugin && m_pRemotePlugin->GetContextId().IsNull()) ) {
		m_Options.Reset();
	}
	else {
		m_Options.Model( ioPrefs );
	}

	return PEEK_PLUGIN_SUCCESS;
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

#if defined(_WIN32) && defined(IMPLEMENT_PLUGINMESSAGE)
	if ( HasRemotePlugin() && m_pRemotePlugin->GetPeekProxy().IsConsole() ) {
		CMessagePing*	pPing( new CMessagePing() );
		FnResponseProcessor	fnProcess = &CPeekMessageProcess::ProcessPluginResponse;
		PostPluginMessage( CPeekMessagePtr( pPing ), this, fnProcess );
	}
#endif // _WIN32 && IMPLEMENT_PLUGINMESSAGE

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
	m_Options.Model( inPrefs );

	OnWritePrefs();
}


// .............................................................................
//		Engine-Only Methods
// .............................................................................

// -----------------------------------------------------------------------------
//		OnGetAboutData
// -----------------------------------------------------------------------------

int		COmniPlugin::OnGetAboutData(
	UInt32*			outFlags,
	CPeekStream&	outData )
{
	(void)outFlags;
	(void)outData;
	return PEEK_PLUGIN_FAILURE;
}


// -----------------------------------------------------------------------------
//		OnGetOptionsData
// -----------------------------------------------------------------------------

int
COmniPlugin::OnGetOptionsData(
	UInt32*			outFlags,
	CPeekStream&	outData )
{
	(void)outFlags;
	(void)outData;
	return PEEK_PLUGIN_FAILURE;
}


// -----------------------------------------------------------------------------
//		OnGetOptionsHandlerId
// -----------------------------------------------------------------------------

int
COmniPlugin::OnGetOptionsHandlerId(
	CGlobalId&	outId )
{
	(void)outId;
	return PEEK_PLUGIN_FAILURE;
}


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
	if ( spMessage.get() != nullptr ) {
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
	_ASSERTE( pMessage != nullptr );
	if ( pMessage == nullptr ) return PEEK_PLUGIN_FAILURE;

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
	_ASSERTE( pMessage != nullptr );
	if ( pMessage == nullptr ) return PEEK_PLUGIN_FAILURE;

	// No input to process.

	// Create the response modeler.
	if ( !pMessage->StartResponse() ) return PEEK_PLUGIN_FAILURE;

	// Build (set) the Options list.
	CMessageGetAllOptions::CNamedOptionsList&	ayNamedOptions = pMessage->GetNamedOptions();

#if (0)
	// This plugin does not support Forensics searches.
	{	// confine the scope of these variables.
		const CPeekString&	strForensics = COptions::GetTagForensics();
		const COptions&		optForensics = GetOptions();
		CMessageGetAllOptions::TNamedOptions	noDefaults = { GUID_NULL, strForensics, optForensics };
		ayNamedOptions.Add( noDefaults );
	}
#endif

	CContextManagerPtr	spContextManager( GetContextManager() );
	if ( spContextManager.IsValid() ) {
		size_t	nCount = spContextManager->GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CPeekContext*		pPeekContext = spContextManager->GetItem( i );
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
#endif // IMPLEMENT_PLUGINMESSAGE


#ifdef _WIN32
// .............................................................................
//		Console-Only Methods
// .............................................................................

#ifdef IMPLEMENT_TAB
// -----------------------------------------------------------------------------
//		CreateTab
// -----------------------------------------------------------------------------

void
COmniPlugin::CreateTab()
{
	if ( CPeekProxy::IsConsole() && IsInstalledOnEngine() && !m_TabHost.HasChild() ) {
		AFX_MANAGE_STATE( AfxGetStaticModuleState() );

		CPeekString	strCmd( COmniPlugin::GetClassId().Format() );
		CPeekString	strClass( L"STATIC" );
		HWND		hWnd( nullptr );

		_ASSERTE( m_pRemotePlugin != nullptr );
		if ( m_pRemotePlugin == nullptr ) return;

		CConsoleUI	ConsoleUI = m_pRemotePlugin->GetConsole();
		hWnd = ConsoleUI.AddTabWindow( strCmd, strClass, s_strName );
		if ( hWnd ) {
			// Subclass the static window Peek created.
#undef SubclassWindow							// The MSVC compiler picks up this macro
			m_TabHost.SubclassWindow( hWnd );	// instead of this function.

			 //Create a dialog to be embedded in the Parent window. The Parent window will delete it.
			CPluginTab*	pTab = new CPluginTab( this );
			_ASSERTE( pTab != nullptr );
			if ( pTab != nullptr ) {
				pTab->Create( IDD_PLUGIN_TAB, &m_TabHost );
				m_TabHost.SetChild( pTab );
			}
		}
	}
}
#endif // IMPLEMENT_TAB


// -----------------------------------------------------------------------------
//		GetInstanceHandle
// -----------------------------------------------------------------------------

HINSTANCE
COmniPlugin::GetInstanceHandle() const
{
	// Can not move this function to the header file.
	// CRemotePlugin is not defined there.
	if ( m_pRemotePlugin != nullptr ) {
		return m_pRemotePlugin->GetInstanceHandle();
	}
	return nullptr;
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
//		OnLoadView
// -----------------------------------------------------------------------------

int
COmniPlugin::OnLoadView()
{
	int	nResult = CPeekPlugin::OnLoadView();
	if ( nResult != PEEK_PLUGIN_SUCCESS ) {
		return nResult;
	}

#if defined(_WIN32) && defined(IMPLEMENT_TAB)
	CreateTab();
#endif // _WIN32 && IMPLEMENT_TAB

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


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		OnProcessPluginResponse
// -----------------------------------------------------------------------------

int
COmniPlugin::OnProcessPluginResponse(
	CPeekStream&	inMessage,
	HeResult		inMsgResult,
	UInt32			inTransId )
{
	int	nResult = PEEK_PLUGIN_FAILURE;

	CPeekMessageItem&	miMessage( m_MessageQueue.GetAt( inTransId ) );
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
#endif // IMPLEMENT_TAB
			break;
	}
}
#endif // IMPLEMENT_PLUGINMESSAGE

#endif // _WIN32
