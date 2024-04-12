// ============================================================================
//	TargetNamePage.cpp:
//		implementation of the CTargetNamePage class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2009. All rights reserved.

#include "StdAfx.h"
#include "TargetNamePage.h"


// -----------------------------------------------------------------------------
//		CTargetNamePage
// -----------------------------------------------------------------------------

CTargetNamePage::CTargetNamePage(
	CTargetNameList&	inOther )
	:	CPropertyPage( CTargetNamePage::IDD )
	,	m_bCaptureAll( inOther.IsCaptureAll() )
	,	m_TargetNames( inOther )
{
}


// -----------------------------------------------------------------------------
//		ControlToList
// -----------------------------------------------------------------------------

void
CTargetNamePage::ControlToList()
{
	CListCtrl* pList = static_cast<CListCtrl*>( GetDlgItem( IDC_TARGET_LIST ) );
	if ( pList == NULL ) return;

	DWORD	dwCount = pList->GetItemCount();

	m_TargetNames.Reset();
	m_TargetNames.SetCaptureAll( IsCaptureAll() );

	for ( DWORD i = 0; i < dwCount; i++ ) {
		bool	bChecked = (pList->GetCheck( i ) == BST_CHECKED);
		CString	str = pList->GetItemText( i, 0 );

		CPeekString strPeek( str );
		m_TargetNames.Add( CTargetName( bChecked, strPeek ) );
	}
}


// -----------------------------------------------------------------------------
//		ListToControl
// -----------------------------------------------------------------------------

void
CTargetNamePage::ListToControl()
{
	CListCtrl* pList = static_cast<CListCtrl*>( GetDlgItem( IDC_TARGET_LIST ) );
	if ( pList == NULL ) return;

	pList->DeleteAllItems();

	size_t	nCount = m_TargetNames.GetCount();
	for ( DWORD i = 0; i < nCount; i++ ) {
		bool	bCheck;
		CPeekString	str;

        m_TargetNames[i].Get( bCheck, str );
		pList->InsertItem( i, str );
		pList->SetCheck( i, bCheck );
	}
}


// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
CTargetNamePage::DoDataExchange(
	CDataExchange* pDX )
{
	CPropertyPage::DoDataExchange( pDX );

	if ( !pDX->m_bSaveAndValidate ) {
		m_bCaptureAll = m_TargetNames.IsCaptureAll();
	}

	DDX_Check( pDX, IDC_ANY_NAME, m_bCaptureAll );

	if ( pDX->m_bSaveAndValidate ) {
		ControlToList();
	}
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CTargetNamePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CListCtrl* pList = static_cast<CListCtrl*>( GetDlgItem( IDC_TARGET_LIST ) );
	if ( pList != NULL  ) {
		pList->SetExtendedStyle( LVS_EX_CHECKBOXES );

		CRect	rc;
		pList->GetClientRect( &rc );
		pList->SetColumnWidth( 0, rc.Width() );
	}

	ListToControl();
	OnBnClickedAnyName();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BEGIN_MESSAGE_MAP(CTargetNamePage, CPropertyPage)
	ON_BN_CLICKED(IDC_ANY_NAME, OnBnClickedAnyName)
	ON_BN_CLICKED(IDC_ADD_TARGET, OnBnClickedAddTarget)
	ON_BN_CLICKED(IDC_REMOVE_TARGET, OnBnClickedRemoveTarget)
END_MESSAGE_MAP()


// -----------------------------------------------------------------------------
//		OnBnClickedAnyName
// -----------------------------------------------------------------------------

void
CTargetNamePage::OnBnClickedAnyName()
{
	UINT	ayIds[] = {
		IDC_TARGET_LIST,
		IDC_REMOVE_TARGET,
		IDC_TARGET_GROUP,
		IDC_TARGET_NAME,
		IDC_ADD_TARGET
	};

	bool	bChecked = !(IsDlgButtonChecked( IDC_ANY_NAME ) == BST_CHECKED);
	for ( int i = 0; i < COUNTOF( ayIds ); i++ ) {
		CWnd*	pWnd = GetDlgItem( ayIds[i] );
		if ( pWnd ) pWnd->EnableWindow( bChecked );
	}
}


// -----------------------------------------------------------------------------
//		OnBnClickedAddTarget
// -----------------------------------------------------------------------------

void
CTargetNamePage::OnBnClickedAddTarget()
{

	CEdit* pAddName( reinterpret_cast<CEdit*>( GetDlgItem( IDC_TARGET_NAME ) ) );
	if ( !pAddName ) return;

	CString	strName;

	pAddName->GetWindowText( strName );
	if ( strName.GetLength() == 0 ) return;

	CListCtrl*	pList = (CListCtrl*)GetDlgItem( IDC_TARGET_LIST );
	if ( pList == NULL ) return;

	CPeekString strPeek( strName );
	if ( CTargetName::Validate( strPeek ) ) {
		LVFINDINFO	FindInfo;
		memset( &FindInfo, 0, sizeof( FindInfo ) );
		FindInfo.flags = LVFI_STRING;
		FindInfo.psz = strName;
		int	nItem = pList->FindItem( &FindInfo );
		if ( nItem >= 0 ) return;

		int	nCount = pList->GetItemCount();

		nItem = pList->InsertItem( nCount, strName );
		pList->SetCheck( nItem );
	}

	pAddName->SetWindowText( L"" );
	pAddName->SetFocus();
}


// -----------------------------------------------------------------------------
//		OnBnClickedRemoveTarget
// -----------------------------------------------------------------------------

void
CTargetNamePage::OnBnClickedRemoveTarget()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_TARGET_LIST );
	if ( pList == NULL ) return;

	int	nItem = pList->GetSelectionMark();
	if ( nItem < 0 ) return;

	pList->DeleteItem( nItem );
	pList->SetSelectionMark( -1 );
}
