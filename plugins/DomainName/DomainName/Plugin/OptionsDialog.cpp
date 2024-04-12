// =============================================================================
//	OptionsDialog.cpp
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef PEEK_UI

#include "OptionsDialog.h"
#include "AboutDialog.h"
#include "PeekTime.h"
#include "Version.h"


// =============================================================================
//		COptionsDialog
// =============================================================================

COptionsDialog::COptionsDialog(
	const COptions&	inOptions,
	HINSTANCE		inInstance,
	CWnd*			inParent /*= NULL*/  )
	:	CDialog( COptionsDialog::IDD, inParent )
	,	m_Options( inOptions )
	,	m_hInstance( inInstance )
{
}


// -----------------------------------------------------------------------------
//		ControlToList
// -----------------------------------------------------------------------------

void
COptionsDialog::ControlToList()
{
	CListBox* pList = static_cast<CListBox*>( GetDlgItem( IDC_NAME_LIST ) );
	if ( pList == NULL ) return;

	CDomainList&	theDomainList( m_Options.GetDomainList() );
	theDomainList.RemoveAll();

	int		dwCount = static_cast<int>( pList->GetCount() );
	for ( int i = 0; i < dwCount; i++ ) {
		CString	str;
		pList->GetText( i, str );
		theDomainList.Add( (CPeekString)str );
	}
}


// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
COptionsDialog::DoDataExchange(
	CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );

	BOOL	bIsTrackIP( false );

	if ( !pDX->m_bSaveAndValidate ) {
		ListToControl();
		bIsTrackIP = (BOOL)m_Options.IsTrackIP();
	}

	DDX_Check( pDX, IDC_TRACK_IP, bIsTrackIP );

	if ( pDX->m_bSaveAndValidate ) {
		ControlToList();
		m_Options.SetTrackIP( bIsTrackIP != FALSE );
	}
}


// -----------------------------------------------------------------------------
//		ListToControl
// -----------------------------------------------------------------------------

void
COptionsDialog::ListToControl()
{
	CListBox* pList = static_cast<CListBox*>( GetDlgItem( IDC_NAME_LIST ) );
	if ( pList == NULL ) return;
	pList->ResetContent();

	CDomainList&	theDomainList( m_Options.GetDomainList() );
	size_t			dwCount = theDomainList.GetCount();
	for ( size_t i = 0; i < dwCount; i++ ) {
		pList->AddString( theDomainList[i] );
	}
}


// ----------------------------------------------------------------------------
//		OnInitDialog
// ----------------------------------------------------------------------------

BOOL
COptionsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CVersion	verFile;
	CVersion	verProduct;
	if ( Environment::GetModuleVersion( verFile, verProduct, m_hInstance ) ) {
		CPeekString m_strVersion;
		m_strVersion = verFile.Format();
		m_Options.SetVersion( m_strVersion );
	}

	return TRUE;
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(COptionsDialog, CDialog )
	ON_BN_CLICKED(IDC_ADD_NAME, &OnBnClickedAddName)
	ON_BN_CLICKED(IDC_DELETE_NAME, &OnBnClickedDeleteName)
	ON_LBN_SELCHANGE(IDC_NAME_LIST, &OnLbnSelchangeNameList)
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnBnClickedAddName
// ----------------------------------------------------------------------------

void COptionsDialog::OnBnClickedAddName()
{
	CString strName;
	GetDlgItemText( IDC_NAME, strName );
	CDomain	theDomain( (CPeekString)strName );
	if ( theDomain.Validate() ) {
		CListBox* pList = static_cast<CListBox*>( GetDlgItem( IDC_NAME_LIST) );
		if ( pList ) {
			int	nItem = pList->FindString( -1, theDomain );
			if ( nItem < 0 ) {
				pList->AddString( theDomain );
			}
		}
	}

	return;
}


// ----------------------------------------------------------------------------
//		OnBnClickedDeleteName
// ----------------------------------------------------------------------------


void COptionsDialog::OnBnClickedDeleteName()
{
	CListBox* pList = static_cast<CListBox*>( GetDlgItem( IDC_NAME_LIST) );
	if ( pList == NULL ) return;

	int	nItem = pList->GetCurSel();
	if ( nItem < 0 ) return;
	pList->DeleteString( nItem );
}


// ----------------------------------------------------------------------------
//		OnLbnSelchangeNameList
// ----------------------------------------------------------------------------

void COptionsDialog::OnLbnSelchangeNameList()
{
	CListBox* pList = static_cast<CListBox*>( GetDlgItem( IDC_NAME_LIST) );
	if ( pList == NULL ) return;
	int	nItem = pList->GetCurSel();
	if ( nItem < 0 ) return;
	CString	strText;
	pList->GetText( nItem, strText );
	if ( !strText.IsEmpty() ) {
		SetDlgItemText( IDC_NAME, strText );
	}
}

#endif // HE_WIN32
