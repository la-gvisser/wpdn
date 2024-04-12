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
	if ( pDX->m_bSaveAndValidate ) {
		return;
	}

	BOOL	bCaptureAnyNames( m_Options.GetTargetNames().IsCaptureAll() );
	BOOL	bMessageLogging( m_Options.IsLoggingToScreen() );
	BOOL	bObscure( (BOOL)m_Options.IsObscureNames() );
	BOOL	bAlerting( (BOOL)m_Options.IsAlerting() );

	CWnd* 	pAlertGrp( GetDlgItem( IDC_ALERT_2_GRP ) );
	CWnd* 	pFacilityLbl( GetDlgItem( IDC_FACILITY_2_LBL ) ); 
	CWnd* 	pFacilityEdit( GetDlgItem( IDC_FACILITY_2 ) );
	CWnd* 	pOutputDirLbl( GetDlgItem( IDC_OUTPUT_DIRECTORY_2_LBL ) );
	CWnd* 	pOutputDirEdit( GetDlgItem( IDC_OUTPUT_DIRECTORY_2 ) );
	CWnd* 	pAlertSourceLbl( GetDlgItem( IDC_ALERT_SOURCE_2_LBL ) );
	CWnd* 	pAlertSourceEdit( GetDlgItem( IDC_ALERT_SOURCE_2 ) );

	int nCmdShow( bAlerting ? SW_SHOW : SW_HIDE );

	pAlertGrp->ShowWindow( nCmdShow );
	pFacilityLbl->ShowWindow( nCmdShow );
	pFacilityEdit->ShowWindow( nCmdShow );
	pOutputDirLbl->ShowWindow( nCmdShow );
	pOutputDirEdit->ShowWindow( nCmdShow );
	pAlertSourceLbl->ShowWindow( nCmdShow );
	pAlertSourceEdit->ShowWindow( nCmdShow );

	if ( bAlerting == TRUE ) {
		CAlerting&	Alerting( m_Options.GetAlerting() );
		CString		strSource( Alerting.GetSource() );
		CString		strFacility( Alerting.GetFacility() );
		CString		strOutputDirectory( Alerting.GetOutputDirectory() );

		DDX_Text( pDX, IDC_ALERT_SOURCE_2, strSource );
		DDX_Text( pDX, IDC_FACILITY_2, strFacility );
		DDX_Text( pDX, IDC_OUTPUT_DIRECTORY_2, strOutputDirectory );
	}

	CString strText;

	if ( bMessageLogging )	strText = L"Message logging to the screen is enabled.";
	else					strText = L"Message logging to the screen is disabled.";
	DDX_Text( pDX, IDC_LOG_MESSAGES_VIEW, strText) ;

	if ( bCaptureAnyNames ) strText = L"Capturing all accounts.";
	else					strText = L"Capturing the following accounts:";
	DDX_Text( pDX, IDC_CAPTURE_ALL_VIEW, strText) ;

	if ( bObscure )			strText = L"Blocking account names from logs and files.";
	else					strText = L"Displaying account names in logs and files.";
	DDX_Text( pDX, IDC_OBSCURE_VIEW, strText) ;

	if ( bAlerting )		strText = L"Alerts are enabled.";
	else					strText = L"Alerts are disabled.";
	DDX_Text( pDX, IDC_DO_ALERTS_VIEW, strText) ;

	SetPortList();
	SetTargetNames();
}


// -----------------------------------------------------------------------------
//		SetPortList
// -----------------------------------------------------------------------------

void
COptionsView::SetPortList()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_PORT_LIST_2 );
	if ( pList == NULL ) return;

	CPortNumberList& PortList( m_Options.GetPortList() );

	pList->DeleteAllItems();

	size_t	nCount = PortList.GetCount();
	for ( DWORD i = 0; i < nCount; i++ ) {
		bool		bCheck;
		CPeekString	str;

		PortList[i].Get( bCheck, str );
		if ( bCheck ) {
			pList->InsertItem( i, str );
		}
		//		pList->SetCheck( i, bCheck );
	}
}


// -----------------------------------------------------------------------------
//		SetTargetNames
// -----------------------------------------------------------------------------

void
COptionsView::SetTargetNames()
{
	CListCtrl* pList = static_cast<CListCtrl*>( GetDlgItem( IDC_TARGET_LIST_2 ) );
	if ( pList == NULL ) return;

	CTargetNameList&	TargetList( m_Options.GetTargetNames() );

	pList->DeleteAllItems();

	if ( TargetList.IsCaptureAll() ) return;

	size_t	nCount = TargetList.GetCount();
	for ( DWORD i = 0; i < nCount; i++ ) {
		bool	bCheck;
		CPeekString	str;

		TargetList[i].Get( bCheck, str );
		if ( bCheck ) {
			pList->InsertItem( i, str );
		}
		//		pList->SetCheck( i, bCheck );
	}
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
COptionsView::OnInitDialog()
{
	CDialog::OnInitDialog();

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
	CGlobalId			inId,
	const CPeekString&	inName,
	const COptions&		inOptions )
{
	m_idContext = inId;
	m_strName = inName;
	m_Options = inOptions;

	if ( ::IsWindow( m_hWnd ) ) {
		m_bInitialized = true;
		UpdateData( FALSE );
	}
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP( COptionsView, CDialog )
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
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
//		OnSize
// ----------------------------------------------------------------------------

void
COptionsView::OnSize(
	UINT	nType,
	int		cx,
	int		cy )
{
	CDialog::OnSize( nType, cx, cy );
}

#endif // HE_WIN32
#endif // IMPLEMENT_TAB
