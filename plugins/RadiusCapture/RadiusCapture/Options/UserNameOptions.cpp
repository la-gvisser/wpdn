// =============================================================================
//	UserNameOptions.cpp
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "UserNameOptions.h"
#include "afxcmn.h"

///////////////////////////////////////////////////////////////////////////////
//		CUserNameEdit
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//		Message Map
// -----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CUserNameEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()

// -----------------------------------------------------------------------------
//		OnChar
// -----------------------------------------------------------------------------

void
CUserNameEdit::OnChar(
	UINT nChar,
	UINT nRepCnt,
	UINT nFlags )
{
	if ( nChar == '\r' ) {
		GetParent()->PostMessage( WM_COMMAND, MAKEWPARAM( IDC_ADD_USERNAME, BN_CLICKED ), 0 );
	}
	else {
		CEdit::OnChar( nChar, nRepCnt, nFlags );
	}
}


////////////////////////////////////////////////////////////////////////////////
//		CUserNameOptionsDlg
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//		Construction
// -----------------------------------------------------------------------------

CUserNameOptionsDlg::CUserNameOptionsDlg(
	CCaseOptions::CUserNameOpts& inOptions, bool bIsDockedTab )
	: CDialog( CUserNameOptionsDlg::IDD )
	, m_Options( inOptions )
	, m_bIsDockedTab( bIsDockedTab )
{
}

// -----------------------------------------------------------------------------
//		ControlToUserList
// -----------------------------------------------------------------------------

void
CUserNameOptionsDlg::ControlToUserList()
{
	if ( m_bIsDockedTab ) return;

	CListCtrl* pUserNameList = (CListCtrl*)GetDlgItem( IDC_USERNAME_LIST );
	if ( pUserNameList == NULL ) return;

	DWORD	dwCount = pUserNameList->GetItemCount();

	CUserItemArray	aUsers;
	aUsers.SetCount( dwCount );
	for ( DWORD i = 0; i < dwCount; i++ ) {
		BOOL	bChecked = (pUserNameList->GetCheck( i ) == BST_CHECKED) ? TRUE : FALSE;
		CString	strUserName = pUserNameList->GetItemText( i, 0 );
		strUserName.Trim();
		if ( strUserName.GetLength() > 0 ) {
			aUsers[i].Put( bChecked, (CPeekString)strUserName );
		}
	}

	if ( !m_Options.m_UserList.SetList( aUsers ) ) {
		// note failure to save settings.
	}
}

// -----------------------------------------------------------------------------
//		UserListToControl
// -----------------------------------------------------------------------------

void
CUserNameOptionsDlg::UserListToControl()
{
	CListCtrl* pUserNameList = (CListCtrl*)GetDlgItem( IDC_USERNAME_LIST );
	if ( pUserNameList == NULL ) return;

	pUserNameList->DeleteAllItems();

	CUserItemArray	aUsers = m_Options.m_UserList;
	size_t	dwCount = aUsers.GetCount();
	for ( DWORD i = 0; i < dwCount; i++ ) {
		BOOL	bCheck;
		CPeekString	strName;

        aUsers[i].Get( &bCheck, strName );
		pUserNameList->InsertItem( i, strName );
		pUserNameList->SetCheck( i, bCheck );
	}
}


// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
CUserNameOptionsDlg::DoDataExchange(
	CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
  #ifdef CAPTURE_ALL
	DDX_Check( pDX, IDC_CAPTURE_ALL, m_bCaptureAll );
  #endif
	if ( pDX->m_bSaveAndValidate ) {
		if ( m_bIsDockedTab ) return;

		ControlToUserList();
  #ifdef CAPTURE_ALL
		m_Options.m_UserList.SetCaptureAll( (m_bCaptureAll != FALSE) );
  #endif
	}
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CUserNameOptionsDlg::OnInitDialog()
{
  #ifdef CAPTURE_ALL
	m_bCaptureAll = m_Options.m_UserList.CaptureAll();
  #endif
	CDialog::OnInitDialog();

	CListCtrl* pUserNameList = (CListCtrl*)GetDlgItem( IDC_USERNAME_LIST );
	if ( pUserNameList == NULL  ) return TRUE;

	pUserNameList->SetExtendedStyle( LVS_EX_CHECKBOXES );

	CRect		rc;
	pUserNameList->GetClientRect( &rc );
	pUserNameList->SetColumnWidth( 0, rc.Width() );

	UserListToControl();
  #ifdef CAPTURE_ALL
	OnBnClickedCaptureAll();
  #endif

	SetControlsInitialState();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// -----------------------------------------------------------------------------
//		Message Map
// -----------------------------------------------------------------------------
	  
BEGIN_MESSAGE_MAP(CUserNameOptionsDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_USERNAME, OnBnClickedAddUserName)
	ON_BN_CLICKED(IDC_DELETE_USERNAME, OnBnClickedDeleteUserName)
	ON_NOTIFY(NM_DBLCLK, IDC_USERNAME_LIST, OnNotifyDoubleClickedUserNameList)
	ON_BN_CLICKED(IDC_CLEAR, OnBnClickedClear)
	ON_EN_CHANGE(IDC_EDIT_USERNAME, OnUserNameChange)
#ifdef CAPTURE_ALL
	ON_BN_CLICKED(IDC_CAPTURE_ALL, OnBnClickedCaptureAll)
#endif
END_MESSAGE_MAP()


#ifdef CAPTURE_ALL
// -----------------------------------------------------------------------------
//		OnBnClickedCaptureAll
// -----------------------------------------------------------------------------

void
CUserNameOptionsDlg::OnBnClickedCaptureAll()
{
	BOOL	bChecked = (IsDlgButtonChecked( IDC_CAPTURE_ALL ) == BST_CHECKED) ? TRUE : FALSE;

	CWnd*	pUserNameList = GetDlgItem( IDC_USERNAME_LIST );
	if ( pUserNameList ) pUserNameList->EnableWindow( !bChecked );

	CWnd*	pEdit = GetDlgItem( IDC_EDIT_USERNAME );
	if ( pEdit ) pEdit->EnableWindow( !bChecked );

	CWnd*	pAdd = GetDlgItem( IDC_ADD_USERNAME );
	if ( pAdd ) pAdd->EnableWindow( !bChecked );

	CWnd*	pDelete = GetDlgItem( IDC_DELETE_USERNAME );
	if ( pDelete ) pDelete->EnableWindow( !bChecked );

	CWnd*	pLabel = GetDlgItem( IDC_LABEL_USERNAME );
	if ( pLabel ) pLabel->EnableWindow( !bChecked );
}
#endif

// -----------------------------------------------------------------------------
//		OnBnClickedAddUserName
// -----------------------------------------------------------------------------

void
CUserNameOptionsDlg::OnBnClickedAddUserName()
{
	if ( m_bIsDockedTab ) return;

	CListCtrl*	pList = (CListCtrl*)GetDlgItem( IDC_USERNAME_LIST );
	if ( pList == NULL ) return;

	CString	strUserName;
	GetDlgItemText( IDC_EDIT_USERNAME, strUserName );

	strUserName.Trim();

	if ( strUserName.GetLength() == 0 ) return;

		LVFINDINFO	FindInfo;
		memset( &FindInfo, 0, sizeof( FindInfo ) );
		FindInfo.flags = LVFI_STRING;
		FindInfo.psz = strUserName;
		int	nItem = pList->FindItem( &FindInfo );
		if ( nItem >= 0 ) return;

		int	nCount = pList->GetItemCount();

		nItem = pList->InsertItem( nCount, strUserName );
		pList->SetCheck( nItem );
}

// -----------------------------------------------------------------------------
//		OnBnClickedDeleteUserName
// -----------------------------------------------------------------------------

void
CUserNameOptionsDlg::OnBnClickedDeleteUserName()
{
	if ( m_bIsDockedTab ) return;

	CListCtrl* pUserNameList = (CListCtrl*)GetDlgItem( IDC_USERNAME_LIST );
	if ( pUserNameList == NULL ) return;

	int	nItem = pUserNameList->GetSelectionMark();
	if ( nItem < 0 ) return;

	pUserNameList->DeleteItem( nItem );
	pUserNameList->SetSelectionMark( -1 );;
}


// -----------------------------------------------------------------------------
//		OnNotifyDoubleClickedUserNameList
// -----------------------------------------------------------------------------

void
CUserNameOptionsDlg::OnNotifyDoubleClickedUserNameList(
	NMHDR*		pNMHDR,
	LRESULT*	pResult )
{

	if ( m_bIsDockedTab ) return;

	*pResult = 0;

	if ( (pNMHDR == NULL) || (pNMHDR->idFrom != IDC_USERNAME_LIST) ) return;

	CListCtrl* pList = (CListCtrl*) GetDlgItem( IDC_USERNAME_LIST );
	if ( pList == NULL ) return;

	int	nItem = pList->GetSelectionMark();
	if ( nItem < 0 ) return;

	CString	strItemText = pList->GetItemText( nItem, 0 );
	SetDlgItemText( IDC_EDIT_USERNAME, strItemText );
}

// -----------------------------------------------------------------------------
//		OnNotifyDoubleClickedUserNameList
// -----------------------------------------------------------------------------

void
CUserNameOptionsDlg::OnBnClickedClear()
{
	if ( m_bIsDockedTab ) return;

	CListCtrl* pUserNameList = (CListCtrl*)GetDlgItem( IDC_USERNAME_LIST );
	ASSERT( pUserNameList );
	if ( pUserNameList == NULL ) return;
	pUserNameList->DeleteAllItems();
	CUserItemArray	aUsers = m_Options.m_UserList;
	aUsers.RemoveAll();
}

// -----------------------------------------------------------------------------
//		OnUserNameChange
// -----------------------------------------------------------------------------

void 
CUserNameOptionsDlg::OnUserNameChange()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( m_bIsDockedTab ) return;

	CString strUserName, strTemp;

	CEdit* pEditName = (CEdit*)GetDlgItem( IDC_EDIT_USERNAME );

	pEditName->GetWindowText( strUserName );

	strTemp = strUserName;
	strUserName.TrimLeft();

	int iLen = strUserName.GetLength();
	if ( iLen > MAX_NAME_LENGTH ) { 
		strUserName = strUserName.Left( MAX_NAME_LENGTH );
		iLen = strUserName.GetLength();
	}

	if ( strUserName != strTemp ) {
		pEditName->SetWindowText( strUserName );
		pEditName->SetSel( iLen, iLen );
	}
}

// -----------------------------------------------------------------------------
//		SetControlsInitialState
// -----------------------------------------------------------------------------

void
CUserNameOptionsDlg::SetControlsInitialState()
{
	if ( !m_bIsDockedTab ) return;

	CWnd* pWnd;

	pWnd = GetDlgItem( IDC_EDIT_USERNAME );
	pWnd->ShowWindow( SW_HIDE );
	pWnd = GetDlgItem( IDC_ADD_USERNAME );
	pWnd->ShowWindow( SW_HIDE );
	pWnd = GetDlgItem( IDC_DELETE_USERNAME );
	pWnd->ShowWindow( SW_HIDE );
	pWnd = GetDlgItem( IDC_CLEAR );
	pWnd->ShowWindow( SW_HIDE );

	pWnd = GetDlgItem( IDC_USERNAME_LIST );
	pWnd->EnableWindow( FALSE );
}
