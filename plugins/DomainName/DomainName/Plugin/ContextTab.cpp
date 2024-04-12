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
#include "PluginMessages.h"
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
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CContextTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pView = new COptionsView( this );
	_ASSERTE( m_pView != NULL );
	if ( m_pView != NULL ) {
		CWnd*	pOptions = GetDlgItem( IDC_CONTEXT_OPTIONS_CT );
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
	_ASSERTE( m_pContext != NULL );
	if ( m_pContext == NULL ) return;

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
	_ASSERTE( pMessage != NULL );
	if ( pMessage == NULL ) return;

	if ( !pMessage->LoadResponse( inResponse ) ) return;

	if ( m_pView != NULL ) {
		m_pView->Refresh( pMessage->GetOptions() );
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
	const LONG kAdjust( 20 );

	CDialog::OnSize( nType, cx, cy );

	CWnd*	pOptions = GetDlgItem( IDC_CONTEXT_OPTIONS_CT );
	if ( !pOptions ) return;

	CRect	rcOptions;
	pOptions->GetWindowRect( &rcOptions );
	ScreenToClient( &rcOptions );
	rcOptions.right = cx - rcOptions.top + kAdjust;
	rcOptions.bottom = cy - rcOptions.top + kAdjust;
	pOptions->MoveWindow( &rcOptions );
	m_pView->MoveWindow( 0, 0, rcOptions.Width(), rcOptions.Height() );
}
#endif // HE_WIN32
#endif // IMPLEMENT_TAB
