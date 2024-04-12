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
	,	m_bInPortEdit( false )
	,	m_nLastPort( 0 )
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

	size_t	nCount = m_PortNumbers.GetCount();
	for ( DWORD i = 0; i < nCount; i++ ) {
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
	ON_EN_CHANGE(IDC_PORT_NUMBER, &CPortNumberPage::OnEnChangePortNumber)
END_MESSAGE_MAP()


// -----------------------------------------------------------------------------
//		OnBnClickedAddPort
// -----------------------------------------------------------------------------

void
CPortNumberPage::OnBnClickedAddPort()
{
	CEdit* pAddPort( reinterpret_cast<CEdit*>( GetDlgItem( IDC_PORT_NUMBER ) ) );
	if ( !pAddPort ) return;

	CString	strPort;
	pAddPort->GetWindowText( strPort );
	if ( strPort.GetLength() == 0 ) return;

	CListCtrl*	pList = (CListCtrl*)GetDlgItem( IDC_PORT_LIST );
	if ( pList == NULL ) return;

	while ( strPort.Left(1) == L"0" ) {
		strPort = strPort.Right( strPort.GetLength() - 1 );
	}

	if ( strPort.GetLength() == 0 ) return;

#ifdef _DEBUG
	CPeekString strTemp( strPort );
	ASSERT( CPortNumber::Validate( strTemp ) == true );
#endif

	LVFINDINFO	FindInfo;
	memset( &FindInfo, 0, sizeof( FindInfo ) );
	FindInfo.flags = LVFI_STRING;
	FindInfo.psz = strPort;
	int	nItem = pList->FindItem( &FindInfo );
	if ( nItem >= 0 ) return;

	int	nCount = pList->GetItemCount();

	nItem = pList->InsertItem( nCount, strPort );
	pList->SetCheck( nItem );

	pAddPort->SetWindowText( L"" );
	pAddPort->SetFocus();
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


// -----------------------------------------------------------------------------
//		OnEnChangePortNumber
//
//		Edit the port number to make sure it is a numeric value and a valid
//		port number.
// -----------------------------------------------------------------------------

void CPortNumberPage::OnEnChangePortNumber()
{
	if ( m_bInPortEdit == true ) return;

	CString	strPortNumber;
	GetDlgItemText( IDC_PORT_NUMBER, strPortNumber );

	int nLength( strPortNumber.GetLength() );

	CEdit* wndPortEdit = (CEdit*)GetDlgItem( IDC_PORT_NUMBER );

	if ( nLength == 0 ) {
		ASSERT( m_bInPortEdit == false );
		m_nLastPort = 0;
		return;
	}

	bool	bAllGood( true );
	CString	strNew( L"" );

	for ( int i = 0; i < nLength; i++ ) {
		if ( isdigit(strPortNumber[i] ) ) {
			strNew += strPortNumber[i];
		}
		else {
			bAllGood = false;
		}
	}

	if ( _wtoi( strNew ) > (static_cast<UInt32>(kMaxUInt16) ) ) {
		if ( m_nLastPort == 0 ) {
			strNew = L"";
		}
		else
		{
			CPeekOutString strOutNew;
			strOutNew << m_nLastPort;
			strNew = (CPeekString)strOutNew;
		}

		bAllGood = false;
	}

	if ( !bAllGood ) {
		nLength = strNew.GetLength();

		m_bInPortEdit = true;
		SetDlgItemText( IDC_PORT_NUMBER, strNew );	
		wndPortEdit->SetSel( nLength, nLength );
		wndPortEdit->SetFocus();
		m_bInPortEdit = false;
	}

	if ( nLength == 0 ) {
		m_nLastPort = 0;
	}
	else {
		m_nLastPort = static_cast<UInt16>( _wtoi( strNew ) );
	}

	ASSERT ( m_bInPortEdit == false );
}
