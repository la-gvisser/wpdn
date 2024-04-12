// ============================================================================
//	PortOptionsDlg.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "PortOptionsDlg.h"
#include "afxcmn.h"


////////////////////////////////////////////////////////////////////////////////
//		CPortOptionsDlg
////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CPortOptionsDlg, CDialog)

// -----------------------------------------------------------------------------
//		ControlToList
// -----------------------------------------------------------------------------

void
CPortOptionsDlg::ControlToList()
{
	CListCtrl*	pList = (CListCtrl*)GetDlgItem( IDC_PORT_LIST );
	if ( pList == NULL ) return;

	const DWORD	dwCount = pList->GetItemCount();

	m_PortNumberList.Clean();

	for ( DWORD i = 0; i < dwCount; i++ ) {
		const bool	bChecked = (pList->GetCheck( i ) == BST_CHECKED);
		CPeekString	str = pList->GetItemText( i, 0 );

		m_PortNumberList.Add( new CPortNumber( bChecked, str ) );
	}
}

// -----------------------------------------------------------------------------
//		ListToControl
// -----------------------------------------------------------------------------

void
CPortOptionsDlg::ListToControl()
{
	CListCtrl*	pList = (CListCtrl*)GetDlgItem( IDC_PORT_LIST );
	if ( pList == NULL ) return;

	pList->DeleteAllItems();

	int	nCount = static_cast<int>( m_PortNumberList.GetCount() );
	for ( int i = 0; i < nCount; i++ ) {
		bool		bCheck;
		CPeekString	str;

        m_PortNumberList[i]->Get( bCheck, str );
		pList->InsertItem( i, str );
		pList->SetCheck( i, static_cast<BOOL>(bCheck) );
	}
}


// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
CPortOptionsDlg::DoDataExchange(
	CDataExchange* pDX )
{
	__super::DoDataExchange(pDX);

	if ( pDX->m_bSaveAndValidate ) {
		ControlToList();
	}
}

// -----------------------------------------------------------------------------
//		Message Map
// -----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CPortOptionsDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_PORT, OnBnClickedAddPort)
	ON_BN_CLICKED(IDC_DELETE_PORT, OnBnClickedDeletePort)
	ON_EN_CHANGE(IDC_EDIT_PORT, OnEnChangeEditPort)
	ON_BN_CLICKED(IDC_BTN_SETDEFAULTS, OnBnClickedSetDefaultPorts)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PORT_LIST, OnLvnItemchangedPortList)
END_MESSAGE_MAP()


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CPortOptionsDlg::OnInitDialog()
{
	m_bInitializing = true;

	__super::OnInitDialog();

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_PORT_LIST );
	if ( pList != NULL  ) {
		pList->SetExtendedStyle( LVS_EX_CHECKBOXES );

		CRect	rc;
		pList->GetClientRect( &rc );
		pList->SetColumnWidth( 0, rc.Width() );
	}
	
	SetControlsInitialState();

	ListToControl();

	m_bInitializing = false;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// -----------------------------------------------------------------------------
//		OnBnClickedAddPort
// -----------------------------------------------------------------------------

void
CPortOptionsDlg::OnBnClickedAddPort()
{
	CString	str;
	GetDlgItemText( IDC_EDIT_PORT, str );
	if ( str.GetLength() == 0 ) return;

	CListCtrl*	pList = (CListCtrl*)GetDlgItem( IDC_PORT_LIST );
	if ( pList == NULL ) return;

	CPeekString strPeek( str );
	if ( CPortNumber::Validate( strPeek ) ) {
		LVFINDINFO	FindInfo;
		memset( &FindInfo, 0, sizeof( FindInfo ) );
		FindInfo.flags = LVFI_STRING;
		FindInfo.psz = str;
		int	nItem = pList->FindItem( &FindInfo );
		if ( nItem >= 0 ) return;

		int	nCount = pList->GetItemCount();

		nItem = pList->InsertItem( nCount, str );
		pList->SetCheck( nItem );
		SetDlgItemText( IDC_EDIT_PORT, _T( "" ) );
	}
	else {
		CPeekString	strTitle;
		CPeekString	strMessage;

		strTitle.LoadString( IDS_PORT_INVALID_TITLE );
		strMessage.LoadString( IDS_PORT_INVALID_MESSAGE );

		MessageBox( strMessage, strTitle, MB_OK );
	}
}

// -----------------------------------------------------------------------------
//		OnBnClickedDeletePort
// -----------------------------------------------------------------------------

void
CPortOptionsDlg::OnBnClickedDeletePort()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_PORT_LIST );
	if ( pList == NULL ) return;

	int	nItem = pList->GetSelectionMark();
	if ( nItem < 0 ) return;

	pList->DeleteItem( nItem );
	pList->SetSelectionMark( -1 );
}

// -----------------------------------------------------------------------------
//		OnEnChangeEditPort
// -----------------------------------------------------------------------------

void CPortOptionsDlg::OnEnChangeEditPort()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strPort, strTemp;

	CEdit* pEditPort = (CEdit*)GetDlgItem( IDC_EDIT_PORT );
	
	pEditPort->GetWindowText( strPort );

	strTemp = strPort;
	strPort.Trim();

	UInt32 nPort = _ttol( strPort);
	if ( nPort == 0 ) {
		if ( strTemp.GetLength() > 0 ) {
			pEditPort->SetWindowText( _T("") );
		}
		return;
	}

	strPort.Format( _T("%d"), nPort );

	int iLen = strPort.GetLength();
	if ( iLen > 5 ) {
		strPort = strPort.Left( 5 );
		iLen = strPort.GetLength();
	}

	if ( strPort != strTemp ) {
		pEditPort->SetWindowText( strPort );
		pEditPort->SetSel( iLen, iLen );
	}
}

// -----------------------------------------------------------------------------
//		OnBnClickedSetDefaultPorts
// -----------------------------------------------------------------------------

void CPortOptionsDlg::OnBnClickedSetDefaultPorts()
{
	m_bInitializing = true;

	m_PortOptions.InitializePortNumberDefaults();

	ListToControl();

	m_bInitializing = false;
}

// -----------------------------------------------------------------------------
//		SetControlsInitialState
// -----------------------------------------------------------------------------

void
CPortOptionsDlg::SetControlsInitialState()
{
	if ( m_bDisableControls ) {
		CWnd* pWnd;

		// Hide or disable controls
		pWnd = GetDlgItem( IDC_ADD_PORT );
		pWnd->ShowWindow( SW_HIDE );
		pWnd = GetDlgItem( IDC_DELETE_PORT );
		pWnd->ShowWindow( SW_HIDE );
		pWnd = GetDlgItem( IDC_EDIT_PORT );
		pWnd->ShowWindow( SW_HIDE );
		pWnd = GetDlgItem( IDC_BTN_SETDEFAULTS );
		pWnd->ShowWindow( SW_HIDE );
		pWnd = GetDlgItem( IDOK );
		pWnd->ShowWindow( SW_HIDE );

		// Center the Cancel button
		CRect theClientRect;
		GetClientRect( &theClientRect );
		int nMiddle = theClientRect.Width() / 2;

		CRect theCancelButtonRect;
		CWnd* pCancelButton;

		pCancelButton = GetDlgItem( IDCANCEL );

		pCancelButton->GetWindowRect( &theCancelButtonRect );
		ScreenToClient( theCancelButtonRect );

		int nNewLeftPos = nMiddle - 35;
		int nNewTopPos = theCancelButtonRect.top - 20;
		pCancelButton->MoveWindow( nNewLeftPos, nNewTopPos, theCancelButtonRect.Width(), theCancelButtonRect.Height()  );

		pWnd = GetDlgItem( IDC_EDIT_PORT );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_PORT_LIST );
		pWnd->EnableWindow( FALSE );
	}
}

// -----------------------------------------------------------------------------
//		OnLvnItemchangedPortList
// -----------------------------------------------------------------------------

void CPortOptionsDlg::OnLvnItemchangedPortList(NMHDR* /*pNMHDR*/, LRESULT *pResult)
{
	if ( !m_bDisableControls || m_bInitializing ) {
		return;
	}

	m_bInitializing = true;

	ListToControl();

	m_bInitializing = false;

	*pResult = 0;
}


////////////////////////////////////////////////////////////////////////////////
//		CCasePortOptionsDlg
////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(CCasePortOptionsDlg, CDialog)

// -----------------------------------------------------------------------------
//		ControlToList
// -----------------------------------------------------------------------------

void
CCasePortOptionsDlg::ControlToList()
{
	CListCtrl*	pList = (CListCtrl*)GetDlgItem( IDC_PORT_PROP_LIST );
	if ( pList == NULL ) return;

	const DWORD	dwCount = pList->GetItemCount();

	m_PortNumberList.Clean();

	for ( DWORD i = 0; i < dwCount; i++ ) {
		const bool	bChecked = (pList->GetCheck( i ) == BST_CHECKED);
		CPeekString	str = pList->GetItemText( i, 0 );

		m_PortNumberList.Add( new CPortNumber( bChecked, str ) );
	}
}


// -----------------------------------------------------------------------------
//		ListToControl
// -----------------------------------------------------------------------------

void
CCasePortOptionsDlg::ListToControl()
{
	CListCtrl*	pList = (CListCtrl*)GetDlgItem( IDC_PORT_PROP_LIST );
	if ( pList == NULL ) return;

	pList->DeleteAllItems();

	int	nCount = static_cast<int>( m_PortNumberList.GetCount() );

	for ( int i = 0; i < nCount; i++ ) {
		bool	bCheck;
		CPeekString	str;

		m_PortNumberList[i]->Get( bCheck, str );
		pList->InsertItem( i, str );
		pList->SetCheck( i, static_cast<BOOL>(bCheck) );
	}
}

// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
CCasePortOptionsDlg::DoDataExchange(
								CDataExchange* pDX )
{
	__super::DoDataExchange(pDX);

	if ( pDX->m_bSaveAndValidate ) {
		ControlToList();
	}
}


// -----------------------------------------------------------------------------
//	Message Map
// -----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCasePortOptionsDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_PROP_PORT, OnBnClickedAddPort)
	ON_BN_CLICKED(IDC_DELETE_PROP_PORT, OnBnClickedDeletePort)
	ON_EN_CHANGE(IDC_EDIT_PROP_PORT, OnEnChangeEditPort)
	ON_BN_CLICKED(IDC_CHK_USE_GLOBAL_PORTS, OnBnClickedChkUseGlobalPorts)
END_MESSAGE_MAP()


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CCasePortOptionsDlg::OnInitDialog()
{
	__super::OnInitDialog();

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_PORT_PROP_LIST );
	if ( pList != NULL  ) {
		pList->SetExtendedStyle( LVS_EX_CHECKBOXES );

		CRect	rc;
		pList->GetClientRect( &rc );
		pList->SetColumnWidth( 0, rc.Width() );
	}

	SetDefaultCheckbox();
	ListToControl();
	SetControlsInitialState();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// -----------------------------------------------------------------------------
//		OnBnClickedAddPort
// -----------------------------------------------------------------------------

void
CCasePortOptionsDlg::OnBnClickedAddPort()
{
	CString	str;
	GetDlgItemText( IDC_EDIT_PROP_PORT, str );
	if ( str.GetLength() == 0 ) return;

	CListCtrl*	pList = (CListCtrl*)GetDlgItem( IDC_PORT_PROP_LIST );
	if ( pList == NULL ) return;

	CPeekString strPeek( str );
	if ( CPortNumber::Validate( strPeek ) ) {
		LVFINDINFO	FindInfo;
		memset( &FindInfo, 0, sizeof( FindInfo ) );
		FindInfo.flags = LVFI_STRING;
		FindInfo.psz = str;
		int	nItem = pList->FindItem( &FindInfo );
		if ( nItem >= 0 ) return;

		int	nCount = pList->GetItemCount();

		nItem = pList->InsertItem( nCount, str );
		pList->SetCheck( nItem );
		SetDlgItemText( IDC_EDIT_PROP_PORT, _T( "" ) );
	}
	else {
		CPeekString	strTitle;
		CPeekString	strMessage;

		strTitle.LoadString( IDS_PORT_INVALID_TITLE );
		strMessage.LoadString( IDS_PORT_INVALID_MESSAGE );

		MessageBox( strMessage, strTitle, MB_OK );
	}
}

// -----------------------------------------------------------------------------
//		OnBnClickedDeletePort
// -----------------------------------------------------------------------------

void
CCasePortOptionsDlg::OnBnClickedDeletePort()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_PORT_PROP_LIST );
	if ( pList == NULL ) return;

	int	nItem = pList->GetSelectionMark();
	if ( nItem < 0 ) return;

	pList->DeleteItem( nItem );
	pList->SetSelectionMark( -1 );
}

// -----------------------------------------------------------------------------
//		OnEnChangeEditPort
// -----------------------------------------------------------------------------

void CCasePortOptionsDlg::OnEnChangeEditPort()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strPort, strTemp;

	CEdit* pEditPort = (CEdit*)GetDlgItem( IDC_EDIT_PROP_PORT );

	pEditPort->GetWindowText( strPort );

	strTemp = strPort;
	strPort.Trim();

	UInt32 nPort = _ttol( strPort);
	if ( nPort == 0 ) {
		if ( strTemp.GetLength() > 0 ) {
			pEditPort->SetWindowText( _T("") );
		}
		return;
	}

	strPort.Format( _T("%d"), nPort );

	int iLen = strPort.GetLength();
	if ( iLen > 5 ) {
		strPort = strPort.Left( 5 );
		iLen = strPort.GetLength();
	}

	if ( strPort != strTemp ) {
		pEditPort->SetWindowText( strPort );
		pEditPort->SetSel( iLen, iLen );
	}
}

// -----------------------------------------------------------------------------
//		OnBnClickedChkUseGlobalPorts
// -----------------------------------------------------------------------------

void CCasePortOptionsDlg::OnBnClickedChkUseGlobalPorts()
{
	if ( m_bIsCapturing || m_bIsDockedTab ) {
		ASSERT( 0 );
		return;
	}

	CButton* chkUseGlobalDefaults = (CButton*)GetDlgItem( IDC_CHK_USE_GLOBAL_PORTS );
	ASSERT( chkUseGlobalDefaults );

	bool bUseGlobalDefaults = (chkUseGlobalDefaults->GetCheck() == TRUE) ? true : false;

	m_PortOptions.SetGlobalDefaultsFlag( bUseGlobalDefaults );

	if ( bUseGlobalDefaults == true ) {
		m_PortOptions.SyncGlobalDefaults( m_GlobalPortsList );
		ListToControl();	
	}

	SetControlsState();
}

// -----------------------------------------------------------------------------
//		SetControlsEnabled
// -----------------------------------------------------------------------------

void
CCasePortOptionsDlg::SetControlsState()
{
	if( m_bIsDockedTab ) {
		ASSERT( 0 );
		return;
	}

	CEdit* pEditPortNum = (CEdit*)GetDlgItem( IDC_EDIT_PROP_PORT );
	CButton* pBtnAddPort = (CButton*)GetDlgItem( IDC_ADD_PROP_PORT );
	CButton* pBtnDeletePort = (CButton*)GetDlgItem( IDC_DELETE_PROP_PORT );
	CListCtrl* pPortList = (CListCtrl*)GetDlgItem( IDC_PORT_PROP_LIST );
	CButton* chkUseGlobalDefaults = (CButton*)GetDlgItem( IDC_CHK_USE_GLOBAL_PORTS );

	if ( m_bIsCapturing  ) {
		chkUseGlobalDefaults->EnableWindow( FALSE );
		pEditPortNum->EnableWindow( FALSE );
		pBtnAddPort->EnableWindow( FALSE );
		pBtnDeletePort->EnableWindow( FALSE );
		pPortList->EnableWindow( FALSE );
	}
	else {
		chkUseGlobalDefaults->EnableWindow( TRUE );
		BOOL bEnable = (m_PortOptions.IsUsingGlobalDefaults()) ? FALSE : TRUE;

		pEditPortNum->EnableWindow( bEnable );
		pBtnAddPort->EnableWindow( bEnable );
		pBtnDeletePort->EnableWindow( bEnable );
		pPortList->EnableWindow( bEnable );
	}
}

// -----------------------------------------------------------------------------
//		SetControlsInitialState
// -----------------------------------------------------------------------------

void
CCasePortOptionsDlg::SetControlsInitialState()
{
	if ( !m_bIsDockedTab ) {
		SetControlsState();
		return;
	}

	CWnd* pWnd;
	pWnd = GetDlgItem( IDC_EDIT_PROP_PORT );
	pWnd->ShowWindow( SW_HIDE );
	pWnd = GetDlgItem( IDC_ADD_PROP_PORT );
	pWnd->ShowWindow( SW_HIDE );
	pWnd = GetDlgItem( IDC_DELETE_PROP_PORT );
	pWnd->ShowWindow( SW_HIDE );

	pWnd = GetDlgItem( IDC_PORT_PROP_LIST );
	pWnd->EnableWindow( FALSE );
	pWnd = GetDlgItem( IDC_CHK_USE_GLOBAL_PORTS );
	pWnd->EnableWindow( FALSE );
}
