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
#include "FTPCaptureOptions.h"
#include <wtypes.h>

// PLUGIN MESSAGE
extern CContextManagerPtr	GetContextManager();

GUID			g_PluginId = { 0x2993A72B, 0x27F4, 0x4EEF, { 0xAF, 0x86, 0x3, 0x14, 0xB9, 0x6F, 0x8A, 0xE } };
CGlobalId		COmniPlugin::s_idClass( g_PluginId );
CPeekString		COmniPlugin::s_strName( L"FTPCapture" );


// =============================================================================
//		COmniPlugin
// =============================================================================

COmniPlugin::COmniPlugin()
	:	CPeekPlugin()
	,	m_bInitialized( false )
#ifdef IMPLEMENT_PLUGINMESSAGE
	,	m_MessageQueue( NULL )
#endif // IMPLEMENT_PLUGINMESSAGE
{
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

	ASSERT( !s_strName.IsEmpty() );

	return s_strName;
}


// -----------------------------------------------------------------------------
//		OnLoad
// -----------------------------------------------------------------------------

int
COmniPlugin::OnLoad(
	CRemotePlugin*		inPlugin, 
	const CPeekProxy&	inPeekProxy )
{
	int	nResult = CPeekPlugin::OnLoad( inPlugin, inPeekProxy );
	if ( nResult != PEEK_PLUGIN_SUCCESS ) {
		return nResult;
	}
	m_bInitialized = true;

	if ( GetPeekProxy().IsConsole() ) {
		AFX_MANAGE_STATE( AfxGetStaticModuleState() );

		CPeekString	strCmd( COmniPlugin::GetClassId().Format() );
		CPeekString	strClass( L"STATIC" );
		HWND		hWnd( NULL );

		CConsoleUI	ConsoleUI = GetConsoleUI();
		hWnd = ConsoleUI.AddTabWindow( strCmd, strClass, s_strName );
		if ( hWnd ) {
			// Subclass the static window Peek created.
#undef SubclassWindow							// The MSVC compiler picks up this macro
			m_TabHost.SubclassWindow( hWnd );	// instead of this function.

			 //Create a dialog to be embedded in the Parent window. The Parent window will delete it.
			CPluginTab*	pTab = new CPluginTab( this );
			ASSERT( pTab != NULL );
			if ( pTab != NULL ) {
				pTab->Create( IDD_PLUGIN_TAB, &m_TabHost );
				m_TabHost.SetChild( pTab );
			}
		}
	}

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
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CAboutDialog	dlgAbout( GetInstanceHandle() );
	dlgAbout.DoModal();

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnOptions
// -----------------------------------------------------------------------------

bool
COmniPlugin::OnOptions(
	COptions&	ioOptions )
{

#ifdef PEEK_UI

	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CFTPCaptureOptions theOptionsDlg( IDS_OPTIONS_TITLE );
	theOptionsDlg.SetOptions( ioOptions );
	if ( theOptionsDlg.DoModal() == IDOK ) {
		ioOptions = theOptionsDlg.GetOptions();
		return true;
	}

#endif

	return false;
}


// -----------------------------------------------------------------------------
//		OnReadPrefs
// -----------------------------------------------------------------------------

int
COmniPlugin::OnReadPrefs()
{
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnWritePrefs
// -----------------------------------------------------------------------------

int
COmniPlugin::OnWritePrefs()
{
	return PEEK_PLUGIN_SUCCESS;
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
		case 'MGAO':
		nResult = ProcessGetAllOptions( spMessage.get() );
			break;

		case 'MSOP':
			nResult = ProcessSetOptions( spMessage.get() );
			break;
		}

		if ( nResult == PEEK_PLUGIN_SUCCESS ) {
			spMessage->StoreResponse( outResponse );
		}
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		ProcessGetAllOptions
// -----------------------------------------------------------------------------

int
COmniPlugin::ProcessGetAllOptions(
	CPeekMessage* ioMessage )
{
	CMessageGetAllOptions*	pMessage = dynamic_cast<CMessageGetAllOptions*>( ioMessage );
	ASSERT( pMessage != NULL );
	if ( pMessage == NULL ) return PEEK_PLUGIN_FAILURE;

	// No input to process.

	// Create the response modeler.
	if ( !pMessage->StartResponse() ) return PEEK_PLUGIN_FAILURE;

	// Build (set) the Options list.
	CMessageGetAllOptions::CNamedOptionsList&	ayNamedOptions = pMessage->GetNamedOptions();


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
//		ProcessSetOptions
// -----------------------------------------------------------------------------

int
COmniPlugin::ProcessSetOptions(
	CPeekMessage* ioMessage )
{
	CMessageSetOptions*	pMessage = dynamic_cast<CMessageSetOptions*>( ioMessage );
	ASSERT( pMessage != NULL );
	if ( pMessage == NULL ) return PEEK_PLUGIN_FAILURE;

	// Find the Context and update it's options.
	CContextManagerPtr	pContextManager = GetContextManager();
	if ( pContextManager ) {
		CPeekContextPtr	spContext;
		if ( !pContextManager->Find( pMessage->GetContextId(), spContext ) || (spContext == NULL) ) {
			return PEEK_PLUGIN_FAILURE;
		}
		spContext->SetOptions( pMessage->GetContextOptions() );
	}

	// Create the response modeler.
	if ( !pMessage->StartResponse() ) return PEEK_PLUGIN_FAILURE;

	// No response members, message result is the response.

	return PEEK_PLUGIN_SUCCESS;
}
#endif // IMPLEMENT_PLUGINMESSAGE


// .............................................................................
//		Console-Only Methods
// .............................................................................

#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		DoSendPluginMessage
// -----------------------------------------------------------------------------

bool
COmniPlugin::DoSendPluginMessage(
	CGlobalId		inId,
	CPeekStream&	inMessage,
	DWORD			inTimeout,
	DWORD&			outTransId )
{
	CRemotePlugin*	pRemoteParent = reinterpret_cast<CRemotePlugin*>( m_pRemotePlugin );
	if ( pRemoteParent ) pRemoteParent->SendPluginMessage( inId, inMessage, inTimeout, outTransId );

	return true;
}


// -----------------------------------------------------------------------------
//		DoPostPluginMessage
// -----------------------------------------------------------------------------

bool
COmniPlugin::DoPostPluginMessage(
	CPeekMessagePtr			inMessage,
	CPeekMessageProcess*	inMsgProc,
	FnResponseProcessor		inResponseProc )
{
	return m_MessageQueue.PostPluginMessage( inMessage, inMsgProc, inResponseProc );
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
#endif // IMPLEMENT_PLUGINMESSAGE
