// =============================================================================
//	OmniConsoleContext.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniConsoleContext.h"
#include "OmniPlugin.h"
#include "OptionsDialog.h"
#include "ContextTab.h"
#include "MemUtil.h"
#include "Packet.h"
#include "PluginMessages.h"
#include "Snapshot.h"
#include "MediaTypes.h"


// -----------------------------------------------------------------------------
//		OnCreateContext
// -----------------------------------------------------------------------------

int
COmniConsoleContext::OnCreateContext()
{
	int	nResult = CPeekConsoleContext::OnCreateContext();

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnDisposeContext
// -----------------------------------------------------------------------------

int
COmniConsoleContext::OnDisposeContext()
{
	int	nResult = CPeekConsoleContext::OnDisposeContext();

	return nResult;
}


#ifdef IMPLEMENT_SUMMARIZEPACKET
// -----------------------------------------------------------------------------
//		OnGetPacketString
//		: Implements ISummarizePacket::SummarizePacket
// -----------------------------------------------------------------------------

int
COmniConsoleContext::OnGetPacketString(
	CPacket&			inPacket,
	CPeekString&		outSummary,
	UInt32&				outColor,
	Peek::SummaryLayer*	inSummaryLayer )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try {
		// No error packets.
		if ( inPacket.IsError() ) return PEEK_PLUGIN_FAILURE;

		outSummary;
		outColor;
		inSummaryLayer;

	}
	catch( int ) {
		;
	}

	return nResult;
}
#endif // IMPLEMENT_SUMMARIZEPACKET


// -----------------------------------------------------------------------------
//		OnContextPrefs
// -----------------------------------------------------------------------------

int
COmniConsoleContext::OnContextPrefs(
	CPeekDataModeler& ioPrefs )
{
	CPeekContext::OnContextPrefs( ioPrefs );

	// The Id is NULL (all zeros) when:
	// 1. This object is the Global Context.
	// 2. This object is displaying options for a new Capture.

	// To always display the default options, don't load
	// the preferences when the Id is NULL (case 2 above).
	if ( ioPrefs.IsLoading() && m_Id.IsNull() ) {
	  #ifdef IMPLEMENT_RESET_OPTIONS
		m_Options.Reset();
	  #else
		m_Options.Model( ioPrefs );
	  #endif
	}
	else {
		m_Options.Model( ioPrefs );
	}

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnLoadView
//		: Implements IPluginUI::LoadView
// -----------------------------------------------------------------------------

int
COmniConsoleContext::OnLoadView()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

#ifdef IMPLEMENT_TAB
	if ( IsInstalledOnEngine() && !HasTab() ) {
		AFX_MANAGE_STATE( AfxGetStaticModuleState() );

		CPeekString	strName( COmniPlugin::GetName() );
		CPeekString	strCmd( COmniPlugin::GetClassId().Format() );
		CPeekString	strClass( L"STATIC" );
		HWND		hWnd( NULL );

		CConsoleUI	ConsoleUI = GetConsoleUI();
		hWnd = ConsoleUI.AddTabWindow( strCmd, strClass, strName );
		if ( hWnd ) {
			// Subclass the static window Peek created.
#undef SubclassWindow							// The MSVC compiler picks up this macro
			m_TabHost.SubclassWindow( hWnd );	// instead of this function.

			 //Create a dialog to be embedded in the Parent window. The Parent window will delete it.
			CContextTab*	pTab = new CContextTab( this );
			_ASSERTE( pTab != NULL );
			if ( pTab != NULL ) {
				pTab->Create( IDD_CONTEXT_TAB, &m_TabHost );
				m_TabHost.SetChild( pTab );
			}
		}
	}
#endif // IMPLEMENT_TAB

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnOptions
// -----------------------------------------------------------------------------

bool
COmniConsoleContext::OnOptions()
{
#ifdef PEEK_UI
	COptionsDialog theOptionsDlg( m_Options, GetInstanceHandle() );
	if ( theOptionsDlg.DoModal() == IDOK ) {
		m_Options = theOptionsDlg.GetOptions();
		return true;
	}
#endif

	return false;
}


// -----------------------------------------------------------------------------
//		OnUnloadView
//		: Implements IPluginUI::UnloadView
// -----------------------------------------------------------------------------

int
COmniConsoleContext::OnUnloadView()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		OnProcessPluginResponse
//		: Implements IPluginRecv::Recv
// -----------------------------------------------------------------------------

int
COmniConsoleContext::OnProcessPluginResponse(
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
//		ProcessPluginResponse							[FnResponseProcessor]
// -----------------------------------------------------------------------------

void
COmniConsoleContext::ProcessPluginResponse(
	HeResult			inResult,
	CPeekMessagePtr		inMessage,
	const CPeekStream&	inResponse )
{
	inResult;
	inMessage;
	inResponse;

	int	nType = inMessage->GetType();
	switch ( nType ) {
		case CMessageGetAllOptions::s_nMessageType:
			GetOptionsResponse( inResult, inMessage, inResponse );
			break;
	}

	return;
}


// -----------------------------------------------------------------------------
//		GetOptionsResponse
// -----------------------------------------------------------------------------

void
COmniConsoleContext::GetOptionsResponse(
	HeResult			inResult,
	CPeekMessagePtr		inMessage,
	const CPeekStream&	inResponse )
{
	CMessageGetOptions*	pMessage = dynamic_cast<CMessageGetOptions*>( inMessage.get() );
	if ( pMessage != NULL ) {
		if ( HE_SUCCEEDED( inResult ) ) {
			if ( pMessage->LoadResponse( inResponse ) ) {
				;
			}
		}
	}
}
#endif // IMPLEMENT_PLUGINMESSAGE
