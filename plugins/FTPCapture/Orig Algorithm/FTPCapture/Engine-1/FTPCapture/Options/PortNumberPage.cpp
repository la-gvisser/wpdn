// ============================================================================
//	PortNumberPage.h
//		interface for the CPortNumberPage class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2005. All rights reserved.

#include "StdAfx.h"
#include "PortNumberPage.h"

// -----------------------------------------------------------------------------
//		CPortNumberPage
// -----------------------------------------------------------------------------

CPortNumberPage::CPortNumberPage(
	CPortNumberList&	inOther )
	:	CPropertyPage( CPortNumberPage::IDD )
	,	m_PortNumbers( inOther )
{
}


// -----------------------------------------------------------------------------
//		ControlToList
// -----------------------------------------------------------------------------

void
CPortNumberPage::ControlToList()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_PORT_LIST );
	if ( pList == NULL ) return;

	DWORD	dwCount = pList->GetItemCount();

	m_PortNumbers.RemoveAll();

	for ( DWORD i = 0; i < dwCount; i++ ) {
		bool	bChecked = (pList->GetCheck( i ) == BST_CHECKED);
		CPeekString	str = pList->GetItemText( i, 0 );

		m_PortNumbers.Add( CPortNumber( bChecked, str ) );
	}
}


// -----------------------------------------------------------------------------
//		ListToControl
// -----------------------------------------------------------------------------

void
CPortNumberPage::ListToControl()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_PORT_LIST );
	if ( pList == NULL ) return;

	pList->DeleteAllItems();

	DWORD	dwCount = m_PortNumbers.GetCount();
	for ( DWORD i = 0; i < dwCount; i++ ) {
		bool	bCheck;
		CPeekString	str;

        m_PortNumbers[i].Get( bCheck, str );
		pList->InsertItem( i, str );
		pList->SetCheck( i, bCheck );
	}
}


// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
CPortNumberPage::DoDataExchange(
	CDataExchange* pDX )
{
	CPropertyPage::DoDataExchange(pDX);

	if ( pDX->m_bSaveAndValidate ) {
		ControlToList();
	}
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CPortNumberPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_PORT_LIST );
	if ( pList != NULL  ) {
		pList->SetExtendedStyle( LVS_EX_CHECKBOXES );

		CRect		rc;
		pList->GetClientRect( &rc );
		pList->SetColumnWidth( 0, rc.Width() );
	}
	ListToControl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BEGIN_MESSAGE_MAP(CPortNumberPage, CPropertyPage)
	ON_BN_CLICKED(IDC_ADD_PORT, OnBnClickedAddPort)
	ON_BN_CLICKED(IDC_REMOVE_PORT, OnBnClickedRemovePort)
END_MESSAGE_MAP()


// -----------------------------------------------------------------------------
//		OnBnClickedAddPort
// -----------------------------------------------------------------------------

void
CPortNumberPage::OnBnClickedAddPort()
{
	CString	str;
	GetDlgItemText( IDC_PORT_NUMBER, str );
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
	}
}


// -----------------------------------------------------------------------------
//		OnBnClickedRemovePort
// -----------------------------------------------------------------------------

void
CPortNumberPage::OnBnClickedRemovePort()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_PORT_LIST );
	if ( pList == NULL ) return;

	int	nItem = pList->GetSelectionMark();
	if ( nItem < 0 ) return;

	pList->DeleteItem( nItem );
	pList->SetSelectionMark( -1 );
}
