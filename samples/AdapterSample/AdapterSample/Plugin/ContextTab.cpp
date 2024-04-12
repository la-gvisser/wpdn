// =============================================================================
//	ContextTab.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef HE_WIN32
#ifdef IMPLEMENT_TAB

#include "ContextTab.h"
#include "PeekContext.h"
#include "OmniConsoleContext.h"
#include "OptionsDialog.h"
#ifdef IMPLEMENT_PLUGINMESSAGE
#include "PluginMessages.h"
#endif // IMPLEMENT_PLUGINMESSAGE
#include "RemotePlugin.h"


// =============================================================================
//		CContextTab
// =============================================================================

CContextTab::CContextTab(
	COmniConsoleContext*	inContext,
	CWnd*					inParent /*= NULL*/  )
	:	CDialog( CContextTab::IDD, inParent )
	,	m_pContext( inContext )
	,	m_pView( NULL )
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
//		GetOptions
// -----------------------------------------------------------------------------

void
CContextTab::GetOptions()
{
#ifdef IMPLEMENT_PLUGINMESSAGE
	_ASSERT( m_pContext != NULL );
	if ( m_pContext == NULL ) return;

	CMessageGetOptions*	pGetOptions( new CMessageGetOptions( m_pContext->GetId() ) );
	FnResponseProcessor	fnProcess = &CPeekMessageProcess::ProcessPluginResponse;
	m_pContext->PostPluginMessage( CPeekMessagePtr( pGetOptions ), this, fnProcess );
#endif // IMPLEMENT_PLUGINMESSAGE
}


#ifdef IMPLEMENT_PLUGINMESSAGE
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
	ASSERT( pMessage != NULL );
	if ( pMessage == NULL ) return;

	if ( !pMessage->LoadResponse( inResponse ) ) return;

	if ( m_pView != NULL ) {
		m_pView->Refresh( m_pContext->GetId(), m_pContext->GetCaptureName(), pMessage->GetOptions() );
	}

	return;
}
#endif // IMPLEMENT_PLUGINMESSAGE


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CContextTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pView = new COptionsView( this );
	ASSERT( m_pView != NULL );
	if ( m_pView != NULL ) {
		CWnd*	pOptions = GetDlgItem( IDC_CONTEXT_OPTIONS );
		if ( pOptions ) {
			m_pView->Create( IDD_OPTIONS_VIEW, pOptions );
			m_pView->ShowWindow( SW_SHOW );
		}
	}

	GetOptions();

	return TRUE;
}


#ifdef IMPLEMENT_PLUGINMESSAGE
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
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnBnClickedRefresh
// ----------------------------------------------------------------------------

void
CContextTab::OnBnClickedRefresh()
{
	GetOptions();
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
#endif // HE_WIN32
#endif // IMPLEMENT_TAB
