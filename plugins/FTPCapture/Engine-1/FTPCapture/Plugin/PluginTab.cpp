// =============================================================================
//	PluginTab.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef HE_WIN32
#ifdef IMPLEMENT_TAB

#include "PluginTab.h"
#include "OmniPlugin.h"
#include "PeekContext.h"
#include "ContextManager.h"
#include "PluginMessages.h"
#include "RemotePlugin.h"

extern CContextManagerPtr	GetContextManager();


// =============================================================================
//		CPluginTab
// =============================================================================

CPluginTab::CPluginTab(
	COmniPlugin*	inPlugin,
	CWnd*			inParent /*= NULL*/  )
	:	CDialog( CPluginTab::IDD, inParent )
	,	m_pPlugin( inPlugin )
	,	m_pContextList( NULL )
	,	m_pView( NULL )
{
}


// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
CPluginTab::DoDataExchange(
	CDataExchange* pDX )
{
	pDX;
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CPluginTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pContextList = reinterpret_cast<CListBox*>( GetDlgItem( IDC_CONTEXT_LIST ) );

	m_pView = new COptionsView( this );
	_ASSERTE( m_pView != NULL );
	if ( m_pView != NULL ) {
		CWnd*	pOptions = GetDlgItem( IDC_CONTEXT_OPTIONS );
		if ( pOptions ) {
			m_pView->Create( IDD_OPTIONS_VIEW, pOptions );
			m_pView->ShowWindow( SW_SHOW );
		}
	}

	return TRUE;
}


// -----------------------------------------------------------------------------
//		Refresh
// -----------------------------------------------------------------------------

void
CPluginTab::Refresh()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if ( m_pContextList == NULL ) return;

	int			nCurSel = m_pContextList->GetCurSel();
	CGlobalId	nCurId;
	if ( (nCurSel >= 0) && (nCurSel < static_cast<int>( m_ayNamedOptions.GetCount() )) ) {
		nCurId = m_ayNamedOptions[nCurSel].fId;
	}

	m_pContextList->ResetContent();

	size_t	nCount = m_ayNamedOptions.GetCount();
	for ( size_t i = 0; i < nCount; i++ ) {
		m_pContextList->AddString( m_ayNamedOptions[i].fName );
	}

	nCurSel = 0;
	if ( nCurId.IsValid() ) {
		for ( size_t i = 0; i < m_ayNamedOptions.GetCount(); i++ ) {
			if ( m_ayNamedOptions[i].fId == nCurId ) {
				nCurSel = static_cast<int>( i );
				break;
			}
		}
	}
	m_pContextList->SetCurSel( nCurSel );
	OnLbnSelchangeContextList();
}


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		GetAllOptions
// -----------------------------------------------------------------------------

void
CPluginTab::GetAllOptions()
{
	_ASSERTE( m_pPlugin != NULL );
	if ( m_pPlugin == NULL ) return;

	CMessageGetAllOptions*	pGetAllOptions( new CMessageGetAllOptions() );
	FnResponseProcessor	fnProcess = &CPeekMessageProcess::ProcessPluginResponse;
	m_pPlugin->PostPluginMessage( CPeekMessagePtr( pGetAllOptions ), this, fnProcess );
}


// -----------------------------------------------------------------------------
//		ProcessGetAllOptions
// -----------------------------------------------------------------------------

void
CPluginTab::ProcessGetAllOptions(
	HeResult			inResult,
	CPeekMessagePtr		inMessage,
	const CPeekStream&	inResponse )
{
	if ( inResult != HE_S_OK ) return;

	CMessageGetAllOptions*	pMessage =
		dynamic_cast<CMessageGetAllOptions*>( inMessage.get() );
	_ASSERTE( pMessage != NULL );
	if ( pMessage == NULL ) return;

	if ( !pMessage->LoadResponse( inResponse ) ) return;

	m_ayNamedOptions.Copy( pMessage->GetNamedOptions() );

	Refresh();

	return;
}


// -----------------------------------------------------------------------------
//		ProcessPluginResponse
// -----------------------------------------------------------------------------

void
CPluginTab::ProcessPluginResponse(
	HeResult			inResult,
	CPeekMessagePtr		inMessage,
	const CPeekStream&	inResponse )
{
	switch ( inMessage->GetType() ) {
		case CMessageGetAllOptions::s_nMessageType:
			ProcessGetAllOptions( inResult, inMessage, inResponse );
			break;
	}
}
#endif // IMPLEMENT_PLUGINMESSAGE


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP( CPluginTab, CDialog )
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_LBN_SELCHANGE(IDC_CONTEXT_LIST, &CPluginTab::OnLbnSelchangeContextList)
	ON_BN_CLICKED(IDC_REFRESH, &CPluginTab::OnBnClickedRefresh)
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnShowWindow
// ----------------------------------------------------------------------------

void
CPluginTab::OnShowWindow(
	BOOL	bShow,
	UINT	nStatus )
{
	CDialog::OnShowWindow( bShow, nStatus );

	if ( bShow && m_pPlugin ) {
		GetAllOptions();
	}
}


// ----------------------------------------------------------------------------
//		OnSize
// ----------------------------------------------------------------------------

void
CPluginTab::OnSize(
	UINT	nType,
	int		cx,
	int		cy )
{
	CDialog::OnSize( nType, cx, cy );

	if ( m_pContextList ) {
		CRect	rcList;
		m_pContextList->GetWindowRect( &rcList );
		ScreenToClient( &rcList );
		rcList.bottom = cy - rcList.top;
		m_pContextList->MoveWindow( &rcList );
	}

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


// ----------------------------------------------------------------------------
//		OnLbnSelchangeContextList
// ----------------------------------------------------------------------------

void
CPluginTab::OnLbnSelchangeContextList()
{
	if ( m_pContextList && m_pView ) {
		_ASSERTE( m_pView );	// If there is a Context List, there should be a view.

		int		nItem = m_pContextList->GetCurSel();
		if ( (nItem == -1) || (nItem >= static_cast<int>( m_ayNamedOptions.GetCount())) ) return;

		CMessageGetAllOptions::TNamedOptions&	NamedOpt = m_ayNamedOptions[nItem];
		m_pView->Refresh( NamedOpt.fId, NamedOpt.fName, NamedOpt.fOptions );
	}
}


// ----------------------------------------------------------------------------
//		OnBnClickedRefresh
// ----------------------------------------------------------------------------

void
CPluginTab::OnBnClickedRefresh()
{
	GetAllOptions();
}

#endif // HE_WIN32
#endif // IMPLEMENT_TAB
