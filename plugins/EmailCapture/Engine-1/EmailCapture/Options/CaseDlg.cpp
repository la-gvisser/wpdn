// ============================================================================
//	CaseDlg.cpp
//		implementation of the CCaseDlg class.
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef HE_WIN32

#include "CaseDlg.h"
#include "FileEx.h"
#include <afxwin.h>
#include <afxcmn.h>


// ============================================================================
//		Global Tags
// ============================================================================

namespace Tags
{
	const CPeekString	kMode( L"Mode" );
	const CPeekString	kAddress( L"Address" );
	const CPeekString	kTriggers( L"Triggers" );
	const CPeekString	kIntegrity( L"Integrity" );
#if SAVE_ATTACHMENTS
	const CPeekString	kAttachments( L"Attachments" );
#endif
}


// ============================================================================
//		CCaseDlg
// ============================================================================

IMPLEMENT_DYNAMIC( CCaseDlg, CDialog )

CCaseDlg::CCaseDlg(
	CCase&						inCase,
	COptions::COptionsCases*	inCaseList,
	CWnd*						pParent /*=NULL*/ )
	:	CDialog( CCaseDlg::IDD, pParent )
	,	m_Case( inCase )
	,	m_pCaseList( inCaseList )
	,	m_ModePage( m_Case )
	,	m_AddressPage( m_Case )
	,	m_TriggerPage( m_Case )
#if SAVE_ATTACHMENTS
	,	m_AttachmentPage( m_Case )
#endif
{
}


// ----------------------------------------------------------------------------
//		DoDataExchange
// ----------------------------------------------------------------------------

void
CCaseDlg::DoDataExchange(
	CDataExchange*	pDX )
{
	CDialog::DoDataExchange( pDX );

	CString	strCaseName;
	if ( !pDX->m_bSaveAndValidate ) {
		strCaseName = static_cast<PCTSTR>( m_Case.GetName() );
	}

	DDX_Text( pDX, IDC_CASE_NAME, strCaseName );

	if ( pDX->m_bSaveAndValidate ) {
		m_Case.SetName( strCaseName.GetString() );
	}
}



// ----------------------------------------------------------------------------
//		OnInitDialog
// ----------------------------------------------------------------------------

BOOL
CCaseDlg::OnInitDialog()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CDialog::OnInitDialog();

	CWnd*	pPageWnd( GetDlgItem( IDC_CASE_VIEW ) );
	if ( pPageWnd == NULL ) return TRUE;
	pPageWnd->ShowWindow( SW_HIDE );

	CRect	rcPage;
	pPageWnd->GetWindowRect( &rcPage );
	ScreenToClient( &rcPage );

	CTreeCtrl*	pCaseTree( static_cast<CTreeCtrl*>( GetDlgItem( IDC_CASE_TREE ) ) );
	if ( pCaseTree == NULL ) return TRUE;

	BOOL	bResult;

	bResult = m_ModePage.Create( CModePage::IDD, this );
	if ( bResult && (m_ModePage.GetSafeHwnd() != NULL)  ) {
		m_ModePage.MoveWindow( &rcPage );
		m_ModePage.ShowWindow( SW_SHOW );
		HTREEITEM	hTreeItem( pCaseTree->InsertItem( Tags::kMode ) );
		pCaseTree->SetItemData( hTreeItem, reinterpret_cast<DWORD_PTR>( &m_ModePage ) );
		pCaseTree->SelectItem( hTreeItem );
	}
	bResult = m_AddressPage.Create( CAddressPage::IDD, this );
	if ( bResult && (m_AddressPage.GetSafeHwnd() != NULL) ) {
		m_AddressPage.MoveWindow( &rcPage );
		HTREEITEM	hTreeItem( pCaseTree->InsertItem( Tags::kAddress ) );
		pCaseTree->SetItemData( hTreeItem, reinterpret_cast<DWORD_PTR>( &m_AddressPage ) );
	}
	bResult = m_TriggerPage.Create( CTriggerPage::IDD, this );
	if ( bResult && (m_TriggerPage.GetSafeHwnd() != NULL) ) {
		m_TriggerPage.MoveWindow( &rcPage );
		HTREEITEM	hTreeItem( pCaseTree->InsertItem( Tags::kTriggers ) );
		pCaseTree->SetItemData( hTreeItem, reinterpret_cast<DWORD_PTR>( &m_TriggerPage ) );
	}
#if SAVE_ATTACHMENTS
	bResult = m_AttachmentPage.Create( CAttachmentPage::IDD, this );
	if ( bResult && (m_AttachmentPage.GetSafeHwnd() != NULL) ) {
		m_AttachmentPage.MoveWindow( &rcPage );
		HTREEITEM	hTreeItem( pCaseTree->InsertItem( Tags::kAttachments ) );
		pCaseTree->SetItemData( hTreeItem, reinterpret_cast<DWORD_PTR>( &m_AttachmentPage ) );
	}
#endif

	return TRUE;
}


// ----------------------------------------------------------------------------
//		OnOK
// ----------------------------------------------------------------------------

void 
CCaseDlg::OnOK()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_ModePage.UpdateData();
	m_AddressPage.UpdateData();
	m_TriggerPage.UpdateData();
#if SAVE_ATTACHMENTS
	m_AttachmentPage.UpdateData();
#endif

	CString	strName;
	GetDlgItemText( IDC_CASE_NAME, strName );
	if ( !CFileEx::IsLegalFileName( strName.GetString() ) ) {
		MessageBox(
			L"The Case name must not contain any of the following characters:\n \\ / : * ? \" < > |",
			L"Invalid Case Name" );
		return;
	}

	//if ( m_pCaseList ) {
	//	if ( m_pCaseList->Find( strName ) >= 0 ) {
	//		MessageBox(
	//			L"The Case name must be unique.",
	//			L"Invalid Case Name" );
	//		return;
	//	}
	//}

	CWnd*	pFolder = m_ModePage.GetDlgItem( IDC_OUTPUT_DIRECTORY );
	CString	strFolder;
	if ( pFolder != NULL ) {
		pFolder->GetWindowText( strFolder );
		if ( strFolder.IsEmpty() ) {
			MessageBox(
				L"Please enter an Output Directory.",
				L"Invalid Output Directory" );
			return;
		}
		if ( !CFileEx::IsLegalFilePath( strFolder.GetString() ) ) {
			MessageBox(
				L"The Output Directory must not contain any of the following characters:\n * ? \" < > | \n or multiple ':' characters.",
				L"Invalid Output Directory" );
			return;
		}
	}

	CWnd*	pFacility = m_AddressPage.GetDlgItem( IDC_FACILITY );
	CString	strFacility;
	if ( pFacility != NULL ) {
		pFacility->GetWindowText( strFacility );
		if ( strFacility.IsEmpty() ) {
			MessageBox(
				L"Please enter a Facility name.",
				L"Invalid Facility" );
			return;
		}
		if ( !CFileEx::IsLegalFileName( strFacility.GetString() ) ) {
			MessageBox(
				L"The Facility must not contain any of the following characters:\n \\ / * ? \" < > | \n or multiple ':' characters.",
				L"Invalid Facility" );
			return;
		}
	}

	CDialog::OnOK();
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCaseDlg, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_CASE_TREE, &CCaseDlg::OnTvnSelchangedCaseTree)
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnTvnSelchangedCaseTree
// ----------------------------------------------------------------------------

void
CCaseDlg::OnTvnSelchangedCaseTree(
	NMHDR	*pNMHDR,
	LRESULT	*pResult )
{
	*pResult = 0;
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>( pNMHDR );
	if ( pNMTreeView == NULL ) return;

	CTreeCtrl*	pCaseTree( static_cast<CTreeCtrl*>( GetDlgItem( IDC_CASE_TREE ) ) );
	if ( pCaseTree == NULL ) return;

	HTREEITEM	hOldItem( pNMTreeView->itemOld.hItem );
	if ( hOldItem != NULL ) {
		CDialog*	pOldDlg( reinterpret_cast<CDialog*>( pCaseTree->GetItemData( hOldItem ) ) );
		if ( (pOldDlg != NULL) && (pOldDlg->GetSafeHwnd() != NULL) ) {
			pOldDlg->ShowWindow( SW_HIDE );
		}
	}

	HTREEITEM	hNewItem( pNMTreeView->itemNew.hItem );
	if ( hNewItem != NULL ) {
		CDialog*	pNewDlg( reinterpret_cast<CDialog*>( pCaseTree->GetItemData( hNewItem ) ) );
		if ( (pNewDlg != NULL) && (pNewDlg->GetSafeHwnd() != NULL) ) {
			pNewDlg->ShowWindow( SW_SHOW );
		}
	}
}

#endif // HE_WIN32
