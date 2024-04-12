// ============================================================================
//	AddressPage.cpp
//		implementation of the CAddressPage class.
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "AddressPage.h"
#include <afxwin.h>
#include <afxcmn.h>


// ============================================================================
//		CAddressEdit
// ============================================================================

BEGIN_MESSAGE_MAP(CAddressEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()

void
CAddressEdit::OnChar(
	UINT nChar,
	UINT nRepCnt,
	UINT nFlags )
{
	if ( nChar == '\r' ) {
		GetParent()->PostMessage( WM_COMMAND, MAKEWPARAM( IDC_ADD_ADDRESS, BN_CLICKED ), 0 );
	}
	else {
		CEdit::OnChar( nChar, nRepCnt, nFlags );
	}
}


// ============================================================================
//		CAddressPage
// ============================================================================

IMPLEMENT_DYNAMIC(CAddressPage, CDialog )

CAddressPage::CAddressPage(
	CCase&	inCase )
	:	CDialog()
	,	m_AddressList( inCase.GetAddressList() )
{
}


// ----------------------------------------------------------------------------
//		ControlToUserList
// ----------------------------------------------------------------------------

void
CAddressPage::ControlToUserList()
{
	CListCtrl* pAddressList( static_cast<CListCtrl*>( GetDlgItem( IDC_ADDRESS_LIST ) ) );
	if ( pAddressList == NULL ) return;

	DWORD	dwCount( pAddressList->GetItemCount() );

	m_AddressList.SetCount( dwCount );
	for ( DWORD i = 0; i < dwCount; i++ ) {
		BOOL		bChecked( (pAddressList->GetCheck( i ) == BST_CHECKED) ? TRUE : FALSE );
		CPeekString	strAddress( pAddressList->GetItemText( i, 0 ) );

		m_AddressList[i].Put( bChecked, strAddress );
	}
}


// ----------------------------------------------------------------------------
//		UserListToControl
// ----------------------------------------------------------------------------

void
CAddressPage::UserListToControl()
{
	CListCtrl* pAddressList( static_cast<CListCtrl*>( GetDlgItem( IDC_ADDRESS_LIST ) ) );
	if ( pAddressList == NULL ) return;

	pAddressList->DeleteAllItems();

	DWORD	dwCount = m_AddressList.GetCount32();
	for ( DWORD i = 0; i < dwCount; i++ ) {
		BOOL		bCheck;
		CPeekString	strName = m_AddressList[i].Get( bCheck );

		int nIndex = pAddressList->InsertItem( i, strName );
		pAddressList->SetCheck( nIndex, bCheck );
	}
}


// ----------------------------------------------------------------------------
//		DoDataExchange
// ----------------------------------------------------------------------------

void
CAddressPage::DoDataExchange(
	CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );

	DDX_Check( pDX, IDC_CAPTURE_ALL, m_AddressList.m_bCaptureAll );

	if ( pDX->m_bSaveAndValidate ) {
		ControlToUserList();
	}
}


// ----------------------------------------------------------------------------
//		OnInitDialog
// ----------------------------------------------------------------------------

BOOL
CAddressPage::OnInitDialog()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CDialog::OnInitDialog();

	m_AddrEdit.SubclassDlgItem( IDC_EDIT_ADDRESS, this );   

	CListCtrl* pAddressList( static_cast<CListCtrl*>( GetDlgItem( IDC_ADDRESS_LIST ) ) );
	if ( pAddressList == NULL  ) return TRUE;

	pAddressList->SetExtendedStyle( LVS_EX_CHECKBOXES );

	CRect	rc;
	pAddressList->GetClientRect( &rc );
	pAddressList->SetColumnWidth( 0, rc.Width() );

	UserListToControl();
	OnBnClickedCaptureAll();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


// ----------------------------------------------------------------------------
//		OnOK
// ----------------------------------------------------------------------------

void 
CAddressPage::OnOK()
{
	bool	bChecked( (IsDlgButtonChecked( IDC_CAPTURE_ALL ) == BST_CHECKED) ? TRUE : FALSE );
	if ( !bChecked ) {
		OnBnClickedAddAddress();
	}
}


// ----------------------------------------------------------------------------
//		OnCancel
// ----------------------------------------------------------------------------

void 
CAddressPage::OnCancel()
{
	CDialog*	pParent( static_cast<CDialog*>( GetParent() ) );
	if ( pParent ) {
		pParent->EndDialog( IDCANCEL );
	}
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CAddressPage, CDialog)
	ON_BN_CLICKED(IDC_CAPTURE_ALL, OnBnClickedCaptureAll)
	ON_BN_CLICKED(IDC_ADD_ADDRESS, OnBnClickedAddAddress)
	ON_BN_CLICKED(IDC_DELETE_ADDRESS, OnBnClickedDeleteAddress)
	ON_NOTIFY(NM_DBLCLK, IDC_ADDRESS_LIST, OnNotifyDoubleClickedAddressList)
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnBnClickedCaptureAll
// ----------------------------------------------------------------------------

void
CAddressPage::OnBnClickedCaptureAll()
{
	BOOL	bChecked( (IsDlgButtonChecked( IDC_CAPTURE_ALL ) == BST_CHECKED) ? TRUE : FALSE );

	// Items with opposite state.
	{
		CWnd*	pAddressList( GetDlgItem( IDC_ADDRESS_LIST ) );
		if ( pAddressList ) pAddressList->EnableWindow( !bChecked );

		CWnd*	pEdit( GetDlgItem( IDC_EDIT_ADDRESS ) );
		if ( pEdit ) pEdit->EnableWindow( !bChecked );

		CWnd*	pAdd( GetDlgItem( IDC_ADD_ADDRESS ) );
		if ( pAdd ) pAdd->EnableWindow( !bChecked );

		CWnd*	pDelete( GetDlgItem( IDC_DELETE_ADDRESS ) );
		if ( pDelete ) pDelete->EnableWindow( !bChecked );

		CWnd*	pLabel( GetDlgItem( IDC_LABEL_ADDRESS ) );
		if ( pLabel ) pLabel->EnableWindow( !bChecked );
	}

	// Items with same state.
	{
		CWnd*	pFacility( GetDlgItem( IDC_FACILITY ) );
		if ( pFacility ) pFacility->EnableWindow( bChecked );

		CWnd*	pLabel( GetDlgItem( IDC_FACILITY_LABEL ) );
		if ( pLabel ) pLabel->EnableWindow( bChecked );
	}
}


// ----------------------------------------------------------------------------
//		OnBnClickedAddAddress
// ----------------------------------------------------------------------------

void
CAddressPage::OnBnClickedAddAddress()
{
	CListCtrl* pAddressList( static_cast<CListCtrl*>( GetDlgItem( IDC_ADDRESS_LIST ) ) );
	if ( pAddressList == NULL ) return;

	CString	str;
	GetDlgItemText( IDC_EDIT_ADDRESS, str );
	if ( str.IsEmpty() ) return;

	if ( CTargetAddress::Validate( CPeekString( str ) ) ) {
		LVFINDINFO	FindInfo;
		memset( &FindInfo, 0, sizeof( FindInfo ) );
		FindInfo.flags = LVFI_STRING;
		FindInfo.psz = str;

		int	nItem( pAddressList->FindItem( &FindInfo ) );
		if ( nItem >= 0 ) return;

		int	nCount( pAddressList->GetItemCount() );

		nItem = pAddressList->InsertItem( nCount, str );
		pAddressList->SetCheck( nItem );
	}

}


// ----------------------------------------------------------------------------
//		OnBnClickedDeleteAddress
// ----------------------------------------------------------------------------

void
CAddressPage::OnBnClickedDeleteAddress()
{
	CListCtrl* pAddressList( static_cast<CListCtrl*>( GetDlgItem( IDC_ADDRESS_LIST ) ) );
	if ( pAddressList == NULL ) return;

	int	nItem = pAddressList->GetSelectionMark();
	if ( nItem < 0 ) return;

	pAddressList->DeleteItem( nItem );
	pAddressList->SetSelectionMark( -1 );;
}


// ----------------------------------------------------------------------------
//		OnNotifyDoubleClickedAddressList
// ----------------------------------------------------------------------------

void
CAddressPage::OnNotifyDoubleClickedAddressList(
	NMHDR*		pNMHDR,
	LRESULT*	pResult )
{
	*pResult = 0;

	if ( (pNMHDR == NULL) || (pNMHDR->idFrom != IDC_ADDRESS_LIST) ) return;

	CListCtrl* pAddressList( static_cast<CListCtrl*>( GetDlgItem( IDC_ADDRESS_LIST ) ) );
	if ( pAddressList == NULL ) return;

	int	nItem = pAddressList->GetSelectionMark();
	if ( nItem < 0 ) return;

	CString	strItemText = pAddressList->GetItemText( nItem, 0 );
	SetDlgItemText( IDC_EDIT_ADDRESS, strItemText );
}
