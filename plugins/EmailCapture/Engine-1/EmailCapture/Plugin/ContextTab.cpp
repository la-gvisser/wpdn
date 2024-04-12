// =============================================================================
//	ContextTab.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef PEEK_UI

#include "ContextTab.h"
#include "OmniConsoleContext.h"
#include "PluginMessages.h"


// =============================================================================
//		CContextTab
// =============================================================================

CContextTab::CContextTab(
	COmniConsoleContext*	inContext,
	const COptions&			inOptions,
	CWnd*					inParent /*= NULL*/  )
	:	CDialog( CContextTab::IDD, inParent )
	,	m_pContext( inContext )
	,	m_OptStat( inOptions )
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

	const size_t	nBorder( 12 );
	const size_t	nButtonHeight( 23 );
	BOOL			bResult( m_OptStat.Create( COptionsStatus::IDD, this ) );
	if ( bResult ) {
		CRect	rcClient;
		GetClientRect( rcClient );

		rcClient.DeflateRect( nBorder, nBorder );
		rcClient.top += nBorder + nButtonHeight;

		m_OptStat.MoveWindow( rcClient );
		m_OptStat.ShowWindow( SW_SHOW );
	}

	SendGetOptions();

	return TRUE;
}


// -----------------------------------------------------------------------------
//		ProcessGetOptions
// -----------------------------------------------------------------------------

void
CContextTab::ProcessGetOptions(
	HeResult			inResult,
	CPeekMessagePtr		inMessage,
	const CPeekStream&	inResponse )
{
	if ( inResult != HE_S_OK ) return;

	CMessageGetOptions*	pMessage = dynamic_cast<CMessageGetOptions*>( inMessage.get() );
	_ASSERTE( pMessage != NULL );
	if ( pMessage == NULL ) return;

	if ( !pMessage->LoadResponse( inResponse ) ) return;

	m_Options = pMessage->GetOptions();

	Refresh();

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
			ProcessGetOptions( inResult, inMessage, inResponse );
			break;
	}
}


// -----------------------------------------------------------------------------
//		Refresh
// -----------------------------------------------------------------------------

void
CContextTab::Refresh()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_OptStat.Refresh( m_Options );
}


// -----------------------------------------------------------------------------
//		SendGetOptions
// -----------------------------------------------------------------------------

void
CContextTab::SendGetOptions()
{
	_ASSERTE( m_pContext != NULL );
	if ( m_pContext == NULL ) return;

	CMessageGetOptions*	pGetOptions( new CMessageGetOptions( m_pContext->GetId() ) );
	FnResponseProcessor	fnProcess = &CPeekMessageProcess::ProcessPluginResponse;
	m_pContext->PostPluginMessage( CPeekMessagePtr( pGetOptions ), this, fnProcess );
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP( CContextTab, CDialog )
	ON_BN_CLICKED(IDC_REFRESH, &CContextTab::OnBnClickedRefresh)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnBnClickedRefresh
// ----------------------------------------------------------------------------

void
CContextTab::OnBnClickedRefresh()
{
	SendGetOptions();
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

	const size_t	nBorder( 12 );
	const size_t	nButtonHeight( 23 );

	if ( m_OptStat.GetSafeHwnd() ) {
		CRect	rcClient;
		GetClientRect( rcClient );

		rcClient.DeflateRect( nBorder, nBorder );
		rcClient.top += nBorder + nButtonHeight;

		m_OptStat.MoveWindow( rcClient );
	}
}

#endif // PEEK_UI
