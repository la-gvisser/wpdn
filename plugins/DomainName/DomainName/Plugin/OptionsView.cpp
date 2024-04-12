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
#include "OptionsDialog.h"
#include "ContextManager.h"
#include "PluginMessages.h"
#include "RemotePlugin.h"

extern CContextManagerPtr	GetContextManager();


// =============================================================================
//		COptionsView
// =============================================================================

COptionsView::COptionsView(
	CWnd*			inParent /*= NULL*/  )
	:	CDialog( COptionsView::IDD, inParent )
	,	m_pParent( dynamic_cast<CPluginTab*>( inParent ) )
	,	m_bInitialized( false )
{
}


// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
COptionsView::DoDataExchange(
	CDataExchange* pDX )
{
	pDX;
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
COptionsView::OnInitDialog()
{
	CDialog::OnInitDialog();

	ActiveWindowDisplay();

	CListCtrl*	pDomains = (CListCtrl*) GetDlgItem( IDC_DOMAIN_LIST );
	if ( pDomains ) {
		CRect	rcClient;
		pDomains->GetClientRect( rcClient );
		int		nWidth = rcClient.Width() - (75 * 2);
		pDomains->InsertColumn( 0, _T( "Domain" ), LVCFMT_LEFT, nWidth );
		pDomains->InsertColumn( 1, _T( "Query" ), LVCFMT_CENTER, 75 );
		pDomains->InsertColumn( 2, _T( "Response" ), LVCFMT_CENTER, 75 );
	}

	CListCtrl*	pActive = (CListCtrl*) GetDlgItem( IDC_ACTIVE_LIST );
	if ( pActive ) {
		CRect	rcClient;
		pActive->GetClientRect( rcClient );
		int		nWidth = rcClient.Width() - 185;
		pActive->InsertColumn( 0, _T( "Domain" ), LVCFMT_LEFT, 185 );
		pActive->InsertColumn( 1, _T( "Addresses" ), LVCFMT_LEFT, nWidth  );
	}

	Update();

	return TRUE;
}


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		ProcessPluginResponse
// -----------------------------------------------------------------------------

void
COptionsView::ProcessPluginResponse(
	HeResult			inResult,
	CPeekMessagePtr		/*inMessage*/,
	const CPeekStream&	/*inResponse*/ )
{
	if ( inResult != HE_S_OK ) return;

	// New options were set.
	if ( m_pParent != NULL ) {
		m_pParent->GetAllOptions();
	}

	return;
}
#endif // IMPLEMENT_PLUGINMESSAGE

// -----------------------------------------------------------------------------
//		Refresh
// -----------------------------------------------------------------------------

void
COptionsView::Refresh(
	const COptions&		inOptions )
{
	m_Options = inOptions;

	if ( ::IsWindow( m_hWnd ) ) {
		m_bInitialized = true;
		Update();
	}
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP( COptionsView, CDialog )
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
//	ON_BN_CLICKED(IDC_MODIFIY_NAMES, OnBnClickedModifiyNames)
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnShowWindow
// ----------------------------------------------------------------------------

void
COptionsView::OnShowWindow(
	BOOL	bShow,
	UINT	nStatus )
{
	CDialog::OnShowWindow( bShow, nStatus );
}


// ----------------------------------------------------------------------------
//		Update
// ----------------------------------------------------------------------------

void
COptionsView::Update()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	UpdateCountedDomains();
	UpdateResolvedDomains();
}


// ----------------------------------------------------------------------------
//		UpdateResolvedDomains
// ----------------------------------------------------------------------------

void
COptionsView::UpdateResolvedDomains()
{
	try {
		const CResolvedDomainList&	ayResolvedNames = m_Options.GetResolvedDomainList();

		CListCtrl*		pActive = (CListCtrl*) GetDlgItem( IDC_ACTIVE_LIST );

		bool bDisplayActive = ActiveWindowDisplay();
		if ( !bDisplayActive ) return;

		if ( pActive ) {
			const int		nCount( static_cast<int>(ayResolvedNames.GetCount()) );
			pActive->DeleteAllItems();
			for ( int i = 0; i < nCount; i++ ) {
				const CResolvedDomain&	theResolved( ayResolvedNames.GetAt( i ) );
				pActive->InsertItem( i, theResolved.GetName() );
				CPeekString theAddressString( theResolved.GetAddressListString() );
				pActive->SetItemText( i, 1, theAddressString );
			}
		}
	}
	catch ( ... ) {
//		There is currently no error logging methodology on the console.
//		LogError( _T( "Failure in Tab Window: Update Domains being monitored." ) );
	}
}


// ----------------------------------------------------------------------------
//		UpdateCountedDomains
// ----------------------------------------------------------------------------

void
COptionsView::UpdateCountedDomains()
{
	try {
		const CDomainCountedList&	ayDomainNames = m_Options.GetDomainCountedList();

		CListCtrl*	pDomains = (CListCtrl*) GetDlgItem( IDC_DOMAIN_LIST );
		if ( pDomains ) {
			pDomains->DeleteAllItems();
			size_t	nCount = ayDomainNames.GetCount();
			for ( size_t i = 0; i < nCount; i++ ) {
 				const CDomainCounted&	theDomain( ayDomainNames.GetAt( i ) );
				CString					strQuery;
				CString					strResponse;

				strQuery.Format( _T( "%u" ), theDomain.GetQueryCount() ); 
				strResponse.Format( _T( "%u" ), theDomain.GetResponseCount() ); 

				int	nIndex = pDomains->InsertItem( 0, theDomain.GetName() );
				pDomains->SetItemText( nIndex, 1, strQuery );
				pDomains->SetItemText( nIndex, 2, strResponse );
			}
		}
	}
	catch ( ... ) {
//		There is currently no error logging methodology on the console.
//		LogError( _T( "Failure in Tab Window: Update Domain List." ) );
	}
}

// ----------------------------------------------------------------------------
//		ActiveWindowDisplay
// ----------------------------------------------------------------------------

bool
COptionsView::ActiveWindowDisplay()
{
	CWnd*		pActiveGroup =	 GetDlgItem( IDC_ACTIVE_GROUP );
	CListCtrl*	pActiveList =	 reinterpret_cast<CListCtrl*>( GetDlgItem( IDC_ACTIVE_LIST ) );
	CWnd*		pNoShowMessage = GetDlgItem( IDC_VIEW_MONITOR_OFF_MSG );

	if ( !pActiveGroup || !pActiveList || !pNoShowMessage ) return true;

	bool bIsTrackIP( m_Options.IsTrackIP() );
	int	 nActiveDisplay( bIsTrackIP ? SW_NORMAL : SW_HIDE );

	pNoShowMessage->ShowWindow( bIsTrackIP ? SW_HIDE : SW_NORMAL );
	pActiveGroup->ShowWindow( nActiveDisplay );
	pActiveList->ShowWindow( nActiveDisplay );

	if ( !bIsTrackIP ) {
		pActiveList->DeleteAllItems();
	}

	return bIsTrackIP;
}


// ----------------------------------------------------------------------------
//		OnSize
// ----------------------------------------------------------------------------

void
COptionsView::OnSize(
	UINT	nType,
	int		cx,
	int		cy )
{
	CDialog::OnSize( nType, cx, cy );

	CWnd*		pDomainGroup = GetDlgItem( IDC_DOMAIN_GROUP );
	CWnd*		pDomains = GetDlgItem( IDC_DOMAIN_LIST );
	CWnd*		pActiveGroup = GetDlgItem( IDC_ACTIVE_GROUP );
	CListCtrl*	pActiveList = reinterpret_cast<CListCtrl*>( GetDlgItem( IDC_ACTIVE_LIST ) );

	if ( !pDomainGroup || !pDomains || !pActiveGroup || !pActiveList ) return;

	// The left hand side Domain Names group:
	CRect	rcDomainGroup;
	pDomainGroup->GetWindowRect( rcDomainGroup );
	ScreenToClient( rcDomainGroup );
	rcDomainGroup.bottom = cy - rcDomainGroup.top - 5;
	pDomainGroup->MoveWindow( rcDomainGroup );

	// The left hand side Domain Names list box:
	CRect	rcDomains;
	pDomains->GetWindowRect( rcDomains );
	ScreenToClient( rcDomains );
	rcDomains.bottom = cy - rcDomains.top;
	pDomains->MoveWindow( rcDomains );

	// The right hand side Monitoring group:
	CRect	rcActiveGroup;
	pActiveGroup->GetWindowRect( rcActiveGroup );
	ScreenToClient( rcActiveGroup );
	rcActiveGroup.right = cx - rcActiveGroup.top - 10;
	rcActiveGroup.bottom = rcDomainGroup.bottom;
	pActiveGroup->MoveWindow( rcActiveGroup );

	// The right hand side Monitoring list box:
	CRect	rcActive;
	pActiveList->GetWindowRect( rcActive );
	ScreenToClient( rcActive );
	rcActive.right = cx - rcActive.top - 8;
	rcActive.bottom = rcDomains.bottom;
	pActiveList->MoveWindow( rcActive );

	// The columns for the Monitoring list box:
	CRect	rcClient;
	pActiveList->GetClientRect( rcClient );
	int		nCol0Width = pActiveList->GetColumnWidth( 0 );
	int		nCol1Width = pActiveList->GetColumnWidth( 1 );
	nCol1Width = rcClient.Width() - nCol0Width;
	pActiveList->SetColumnWidth( 1, nCol1Width );

	// The Modify button:
	//	CRect	rcModify;
	//	pModify->GetWindowRect( rcModify );
	//	ScreenToClient( rcModify );
}


/*
// ----------------------------------------------------------------------------
//		OnBnClickedModifiyNames
// ----------------------------------------------------------------------------

void
COptionsView::OnBnClickedModifiyNames()
{
	ASSERT( m_pContext );
	if ( m_pContext == NULL ) return;

	m_pContext->OnContextOptions();
}
*/

#endif // HE_WIN32
#endif // IMPLEMENT_TAB
