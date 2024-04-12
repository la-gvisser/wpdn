// =============================================================================
//	ContextTab.cpp
// =============================================================================
//	Copyright (c) 2011-2017 Savvius, Inc. All rights reserved.

#include "StdAfx.h"

#if defined(_WIN32) && defined(IMPLEMENT_TAB)

#include "ContextTab.h"
#include "PeekContext.h"
#include "OmniConsoleContext.h"
#include "OptionsDialog.h"
#include "PluginMessages.h"
#include "RemotePlugin.h"


// =============================================================================
//		CContextTab
// =============================================================================

CContextTab::CContextTab(
	COmniConsoleContext*	inContext,
	CWnd*					inParent /*= nullptr*/  )
	:	CDialog( CContextTab::IDD, inParent )
	,	m_pContext( inContext )
	,	m_pView( nullptr )
{
}


// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
CContextTab::DoDataExchange(
	CDataExchange* pDX )
{
	pDX;
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CContextTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pView = new COptionsView( this );
	_ASSERTE( m_pView != nullptr );
	if ( m_pView != nullptr ) {
		CWnd*	pOptions = GetDlgItem( IDC_CONTEXT_OPTIONS );
		if ( pOptions ) {
			m_pView->Create( IDD_OPTIONS_VIEW, pOptions );
			m_pView->ShowWindow( SW_SHOW );
		}
	}

#ifdef IMPLEMENT_PLUGINMESSAGE
	GetOptions();
#endif // IMPLEMENT_PLUGINMESSAGE

	return TRUE;
}


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		GetOptions
// -----------------------------------------------------------------------------

void
CContextTab::GetOptions()
{
	_ASSERTE( m_pContext != nullptr );
	if ( m_pContext == nullptr ) return;

	CMessageGetOptions*	pGetOptions( new CMessageGetOptions( m_pContext->GetId() ) );
	FnResponseProcessor	fnProcess = &CPeekMessageProcess::ProcessPluginResponse;
	m_pContext->PostPluginMessage( CPeekMessagePtr( pGetOptions ), this, fnProcess );
}


// -----------------------------------------------------------------------------
//		GetOptionsResponse
// -----------------------------------------------------------------------------

void
CContextTab::GetOptionsResponse(
	HeResult			inResult,
	CPeekMessagePtr		inMessage,
	const CPeekStream&	inResponse )
{
	if ( inResult != HE_S_OK ) return;

	CMessageGetOptions*	pMessage = dynamic_cast<CMessageGetOptions*>( inMessage.get() );
	_ASSERTE( pMessage != nullptr );
	if ( pMessage == nullptr ) return;

	if ( !pMessage->LoadResponse( inResponse ) ) return;

	if ( m_pView != nullptr ) {
		m_pView->Refresh( m_pContext->GetId(), m_pContext->GetCaptureName(), pMessage->GetOptions() );
	}

	return;
}


// -----------------------------------------------------------------------------
//		ProcessPluginResponse
// -----------------------------------------------------------------------------

void
CContextTab::ProcessPluginResponse(
	HeResult			inResult,
	CPeekMessagePtr		inMessage,
	const CPeekStream&	inResponse )
{
	int	nType = inMessage->GetType();
	switch ( nType ) {
		case CMessageGetOptions::s_nMessageType:
			GetOptionsResponse( inResult, inMessage, inResponse );
			break;
[!if NOTIFY_CODEEXAMPLES]

#ifdef IMPLEMENT_CODEEXAMPLES
		case CMessageExample::s_nMessageType:
			ExampleResponse( inResult, inMessage, inResponse );
			break;
#endif // IMPLEMENT_CODEEXAMPLES
[!endif]
	}
}
#endif // IMPLEMENT_PLUGINMESSAGE


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP( CContextTab, CDialog )
	ON_BN_CLICKED(IDC_REFRESH, &CContextTab::OnBnClickedRefresh)
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
[!if NOTIFY_CODEEXAMPLES]
#ifdef IMPLEMENT_CODEEXAMPLES
	ON_BN_CLICKED(IDC_MESSAGE, &CContextTab::OnBnClickedMessage)
#endif // IMPLEMENT_CODEEXAMPLES
[!endif]
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnBnClickedRefresh
// ----------------------------------------------------------------------------

void
CContextTab::OnBnClickedRefresh()
{
#ifdef IMPLEMENT_PLUGINMESSAGE
	GetOptions();
#endif // IMPLEMENT_PLUGINMESSAGE
}


// ----------------------------------------------------------------------------
//		OnShowWindow
// ----------------------------------------------------------------------------

void
CContextTab::OnShowWindow(
	BOOL	bShow,
	UINT	nStatus )
{
	CDialog::OnShowWindow( bShow, nStatus );
}


// ----------------------------------------------------------------------------
//		OnSize
// ----------------------------------------------------------------------------

void
CContextTab::OnSize(
	UINT	nType,
	int		cx,
	int		cy )
{
	CDialog::OnSize( nType, cx, cy );

	CWnd*	pOptions = GetDlgItem( IDC_CONTEXT_OPTIONS );
	if ( pOptions ) {
		CRect	rcOptions;
		pOptions->GetWindowRect( &rcOptions );
		ScreenToClient( &rcOptions );
		rcOptions.right = cx - rcOptions.top;
		rcOptions.bottom = cy - rcOptions.top;
		pOptions->MoveWindow( &rcOptions );
	}
}
[!if NOTIFY_CODEEXAMPLES]


#ifdef IMPLEMENT_PLUGINMESSAGE
#ifdef IMPLEMENT_CODEEXAMPLES
// -----------------------------------------------------------------------------
//		ExampleResponse
// -----------------------------------------------------------------------------

void
CContextTab::ExampleResponse(
	HeResult			inResult,
	CPeekMessagePtr		inMessage,
	const CPeekStream&	inResponse )
{
	if ( inResult != HE_S_OK ) return;

	CMessageExample*	pMessage = dynamic_cast<CMessageExample*>( inMessage.get() );
	_ASSERTE( pMessage != nullptr );
	if ( pMessage == nullptr ) return;

	if ( !pMessage->LoadResponse( inResponse ) ) return;

	// Validate response.
	const CPeekString&	strResponse = pMessage->GetResponse();
	_ASSERTE( strResponse == L"Data received and sending some back!" );

	const CPeekStream&	psData = pMessage->GetResponseData();
	_ASSERTE( psData.GetLength() == 500 );
	if ( psData.GetLength() == 500 ) {
		const UInt8* pData = psData.GetData();
		_ASSERTE( pData != nullptr );
		if ( pData != nullptr ) {
			const UInt8* pDataEnd = pData + psData.GetLength();
			while ( pData < pDataEnd ) {
				_ASSERTE( *pData == 0x56 );
				pData++;
			}

		}
	}
}


// ----------------------------------------------------------------------------
//		OnBnClickedMessage
// ----------------------------------------------------------------------------

void
CContextTab::OnBnClickedMessage()
{
	_ASSERTE( m_pContext != nullptr );
	if ( m_pContext == nullptr ) return;

	CMessageExample*	pMsgExample = new CMessageExample();
	if ( (pMsgExample == nullptr) || (!pMsgExample->IsModeled()) ) return;

	UInt8	data[30];
	memset( data, 0x55, sizeof( data ) );

	pMsgExample->SetMessage( L"I'm sending you some data." );
	pMsgExample->SetMessageData( sizeof( data ), data );

	FnResponseProcessor	fnProcess = &CPeekMessageProcess::ProcessPluginResponse;
	m_pContext->PostPluginMessage( CPeekMessagePtr( pMsgExample ), this, fnProcess );
}
#endif // IMPLEMENT_CODEEXAMPLES
#endif // IMPLEMENT_PLUGINMESSAGE
[!endif]
#endif // _WIN32 && IMPLEMENT_TAB
