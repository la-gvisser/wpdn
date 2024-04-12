// =============================================================================
//	PluginTab.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef PEEK_UI

#include "PluginTab.h"
#include "OmniPlugin.h"
#include "PluginMessages.h"
#include "GlobalTags.h"


// =============================================================================
//		CPluginTab
// =============================================================================

CPluginTab::CPluginTab(
	COmniPlugin*	inPlugin,
	CWnd*			inParent /*= NULL*/  )
	:	CDialog( CPluginTab::IDD, inParent )
	,	m_pPlugin( inPlugin )
{
}

CPluginTab::~CPluginTab()
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

	SendGetAllOptions();

	return TRUE;
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


// -----------------------------------------------------------------------------
//		Refresh
// -----------------------------------------------------------------------------

void
CPluginTab::Refresh()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CListBox*	pContextList = reinterpret_cast<CListBox*>( GetDlgItem( IDC_CONTEXT_LIST ) );
	if ( pContextList == NULL ) return;

	const size_t	nCount( m_ayNamedOptions.GetCount() );
	int				nCurSel = pContextList->GetCurSel();
	CGlobalId		nCurId;
	if ( (nCurSel >= 0) && (nCurSel < static_cast<int>( nCount )) ) {
		nCurId = m_ayNamedOptions[nCurSel].fId;
	}

	m_ayOptStat.Reset();
	pContextList->ResetContent();

	for ( size_t i = 0; i < nCount; i++ ) {
		pContextList->AddString( m_ayNamedOptions[i].fName );
		COptionsStatus*	pOptStat = new COptionsStatus( m_ayNamedOptions[i].fOptions );
		_ASSERTE( pOptStat );
		if ( pOptStat ) {
			BOOL	bResult( pOptStat->Create( COptionsStatus::IDD, this ) );
			_ASSERTE( bResult );
			if ( bResult ) {
				m_ayOptStat.Add( pOptStat );
			}
			else {
				delete pOptStat;
			}
		}
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

	pContextList->SetCurSel( nCurSel );
	OnLbnSelchangeContextList();
}


// -----------------------------------------------------------------------------
//		SendGetAllOptions
// -----------------------------------------------------------------------------

void
CPluginTab::SendGetAllOptions()
{
	_ASSERTE( m_pPlugin != NULL );
	if ( m_pPlugin == NULL ) return;

	CMessageGetAllOptions*	pGetAllOptions( new CMessageGetAllOptions() );
	FnResponseProcessor	fnProcess = &CPeekMessageProcess::ProcessPluginResponse;
	m_pPlugin->PostPluginMessage( CPeekMessagePtr( pGetAllOptions ), this, fnProcess );
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP( CPluginTab, CDialog )
	ON_BN_CLICKED(IDC_REFRESH, &CPluginTab::OnBnClickedRefresh)
	ON_LBN_SELCHANGE(IDC_CONTEXT_LIST, &CPluginTab::OnLbnSelchangeContextList)
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnBnClickedRefresh
// ----------------------------------------------------------------------------

void
CPluginTab::OnBnClickedRefresh()
{
	SendGetAllOptions();
}


// ----------------------------------------------------------------------------
//		OnLbnSelchangeContextList
// ----------------------------------------------------------------------------

void
CPluginTab::OnLbnSelchangeContextList()
{
	CListBox*	pContextList = reinterpret_cast<CListBox*>( GetDlgItem( IDC_CONTEXT_LIST ) );
	if ( pContextList == NULL ) return;

	m_ayOptStat.Hide();

	int	nItem = pContextList->GetCurSel();
	if ( (nItem < 0) || (nItem >= static_cast<int>( m_ayOptStat.GetCount())) ) return;

	if ( m_ayOptStat[nItem] && m_ayOptStat[nItem]->GetSafeHwnd() ) {
		m_ayOptStat[nItem]->ShowWindow( SW_SHOW );
	}
}


// ----------------------------------------------------------------------------
//		OnShowWindow
// ----------------------------------------------------------------------------

void
CPluginTab::OnShowWindow(
	BOOL	bShow,
	UINT	nStatus )
{
	CDialog::OnShowWindow( bShow, nStatus );

	Refresh();
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

	CListBox*	pContextList = reinterpret_cast<CListBox*>( GetDlgItem( IDC_CONTEXT_LIST ) );
	if ( !pContextList ) return;

	const int	nBorder( 12 );
	CRect		rcContextList;

	pContextList->GetWindowRect( &rcContextList );
	ScreenToClient( &rcContextList );
	rcContextList.bottom = cy - nBorder;
	pContextList->MoveWindow( &rcContextList );

	// CRect( left, top, right, bottom );
	CRect	rcOpts(
		(rcContextList.right + nBorder),
		nBorder,
		(cx - nBorder),
		(cy - nBorder) );
	m_ayOptStat.ResizeAll( rcOpts );
}

#endif // PEEK_UI
