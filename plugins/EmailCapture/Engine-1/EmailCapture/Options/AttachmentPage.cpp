// ============================================================================
//	AttachmentPage.cpp
//		implementation of the CAttachmentPage class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"

#ifdef HE_WIN32

#include "AttachmentPage.h"
#include <afxwin.h>


// ============================================================================
//		CAttachmentPage
// ============================================================================

IMPLEMENT_DYNAMIC(CAttachmentPage, CDialog)

CAttachmentPage::CAttachmentPage(
	CCase&	inCase,
	CWnd*	inParentWnd )
	:	CDialog( CAttachmentPage::IDD, inParentWnd )
	,	m_Settings( inCase.GetSettings() )
{
}


// ----------------------------------------------------------------------------
//		DoDataExchange
// ----------------------------------------------------------------------------

void
CAttachmentPage::DoDataExchange(
	CDataExchange* pDX )
{
#if SAVE_ATTACHMENTS
	if ( !pDX->m_bSaveAndValidate ) {
		m_strAttachmentDirectory = static_cast<PCTSTR>( m_Settings.m_strAttachmentDirectory );
	}
#endif

	CDialog::DoDataExchange( pDX );

#if SAVE_ATTACHMENTS
	DDX_Check( pDX, IDC_ATTACHMENT_SAVE, m_Settings.m_bSaveAttachments );
	DDX_Text( pDX, IDC_ATTACHMENT_DIRECTORY, m_strAttachmentDirectory );

	if ( pDX->m_bSaveAndValidate ) {
		m_Settings.m_strAttachmentDirectory = m_strAttachmentDirectory;
	}
#endif
}


// ----------------------------------------------------------------------------
//		OnInitDialog
// ----------------------------------------------------------------------------

BOOL
CAttachmentPage::OnInitDialog()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CDialog::OnInitDialog();

	OnBnClickedAttachmentSave();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


// ----------------------------------------------------------------------------
//		OnOK
// ----------------------------------------------------------------------------

void 
CAttachmentPage::OnOK()
{
}


// ----------------------------------------------------------------------------
//		OnCancel
// ----------------------------------------------------------------------------

void 
CAttachmentPage::OnCancel()
{
	CDialog*	pParent( static_cast<CDialog*>( GetParent() ) );
	if ( pParent ) {
		pParent->EndDialog( IDCANCEL );
	}
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CAttachmentPage, CDialog)
	ON_BN_CLICKED(IDC_ATTACHMENT_SAVE, OnBnClickedAttachmentSave)
	ON_BN_CLICKED(IDC_ATTACHMENT_BROWSE, OnBnClickedBrowse)
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnBnClickedAttachmentSave
// ----------------------------------------------------------------------------

void
CAttachmentPage::OnBnClickedAttachmentSave()
{
	BOOL	bEnable( (IsDlgButtonChecked( IDC_ATTACHMENT_SAVE ) == BST_CHECKED) ? TRUE : FALSE );

	const int	nIdCount( 3 );
	int	nIds[nIdCount] = {
		IDC_ATTACHMENT_LABEL,
		IDC_ATTACHMENT_DIRECTORY,
		IDC_ATTACHMENT_BROWSE
	};

	for ( int i = 0; i < nIdCount; i++ ) {
		CWnd*	pCtrl( GetDlgItem( nIds[i] ) );
		if ( pCtrl ) {
			pCtrl->EnableWindow( bEnable );
		}
	}
}


// ----------------------------------------------------------------------------
//		OnBnClickedBrowse
// ----------------------------------------------------------------------------

void
CAttachmentPage::OnBnClickedBrowse()
{
	LPMALLOC	pMalloc( NULL );
	if ( SUCCEEDED( ::SHGetMalloc( &pMalloc ) ) ) {
		// Get a place to put the path temporarily
		CString	strTempLocation( m_strAttachmentDirectory );

		BROWSEINFO	bInfo;
		::ZeroMemory( static_cast<PVOID>( &bInfo ), sizeof( BROWSEINFO ) );
		bInfo.hwndOwner = m_hWnd;
		bInfo.pidlRoot = NULL;
		bInfo.pszDisplayName = NULL;
		bInfo.lpszTitle = L"Choose Attachment Directory:";
		bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
		bInfo.lpfn = NULL;

		LPITEMIDLIST	pidlPath( ::SHBrowseForFolder( &bInfo ) );
		if ( pidlPath != NULL ) {
			if ( ::SHGetPathFromIDList( pidlPath, strTempLocation.GetBufferSetLength( MAX_PATH ) ) ) {
				strTempLocation.ReleaseBuffer();
				m_strAttachmentDirectory = strTempLocation.GetString();
				SetDlgItemText( IDC_ATTACHMENT_DIRECTORY, m_strAttachmentDirectory );
			}
			else {
				strTempLocation.ReleaseBuffer();
			}
		}
		pMalloc->Free( pidlPath );
	}
}

#endif // HE_WIN32
