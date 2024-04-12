// ============================================================================
//	AlertingPage.cpp
//		Implementation fothe CAlertPage class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2009. All rights reserved.

#include "StdAfx.h"
#include "AlertingPage.h"
#include "Options.h"
#include "shlobj.h"


// ============================================================================
//		CAlertingPage
// ============================================================================

CAlertingPage::CAlertingPage(
	COptions& inOptions )
	:	CPropertyPage( CAlertingPage::IDD )
	,	m_Options( inOptions )
	,	m_Alerting( inOptions.GetAlerting() )
	,	m_Logging( m_Options.GetLogging() )
{
}


// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
CAlertingPage::DoDataExchange(
	CDataExchange* pDX )
{
	CPropertyPage::DoDataExchange( pDX );

	CString strSource;
	CString strFacility;
	CString strOutputDirectory;
	BOOL	bAlerting( FALSE );
		
	if ( !pDX->m_bSaveAndValidate ) {
		strSource =  m_Alerting.m_strSource;
		strFacility = m_Alerting.m_strFacility;
		strOutputDirectory = m_Alerting.m_strOutputDirectory;
		bAlerting = m_Alerting.IsAlerting() ? TRUE: FALSE;
	}

	DDX_Check( pDX, IDC_ENABLE_LOGGING, m_Logging.m_bLoggingToScreen );
	DDX_Check( pDX, IDC_OBSCURE_NAMES, m_Logging.m_bObscureNames );
  #ifdef IMPLEMENT_PASSWORD_LOGGING
	DDX_Check( pDX,	IDC_SHOW_PASSWORD, m_Logging.m_bShowPassword );
  #endif // IMPLEMENT_PASSWORD_LOGGING

	DDX_Check( pDX, IDC_DO_ALERTS, bAlerting );
	DDX_Text( pDX, IDC_ALERT_SOURCE, strSource );
	DDX_Text( pDX, IDC_FACILITY, strFacility );
	DDX_Text( pDX, IDC_OUTPUT_DIRECTORY, strOutputDirectory );

	if ( pDX->m_bSaveAndValidate ) {
		m_Alerting.SetAlerting( bAlerting == TRUE ? true : false );
		m_Alerting.m_strSource = strSource.GetString();
		m_Alerting.m_strFacility = strFacility.GetString();
		m_Alerting.m_strOutputDirectory = strOutputDirectory.GetString();
	}

}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CAlertingPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CWnd*	pShowPasswordWnd( GetDlgItem( IDC_SHOW_PASSWORD ) );
	if ( pShowPasswordWnd != NULL ) {
	  #ifdef IMPLEMENT_PASSWORD_LOGGING
		pShowPasswordWnd->ShowWindow( TRUE );

		BOOL	bEnable( !m_Logging.m_bObscureNames );
		pShowPasswordWnd->EnableWindow( bEnable );
	  #else // IMPLEMENT_PASSWORD_LOGGING
		pShowPasswordWnd->ShowWindow( FALSE );
	  #endif // IMPLEMENT_PASSWORD_LOGGING
	}

//	OnBnClickedEnableLogging();

	OnBnClickedAlerting();

	return TRUE;
}


// -----------------------------------------------------------------------------
//		Message Map
// -----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CAlertingPage, CPropertyPage)
	ON_BN_CLICKED(IDC_DO_ALERTS, OnBnClickedAlerting)
//	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_OBSCURE_NAMES, OnBnClickedObscureNames)
//	ON_BN_CLICKED(IDC_ENABLE_LOGGING, OnBnClickedEnableLogging)
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnBnClickedAlerting
// ----------------------------------------------------------------------------

void
CAlertingPage::OnBnClickedAlerting()
{
	BOOL	bEnable = (IsDlgButtonChecked( IDC_DO_ALERTS ) == BST_CHECKED) ? TRUE : FALSE;

	CWnd*	pAlertSource = GetDlgItem( IDC_ALERT_SOURCE );
	if ( pAlertSource ) pAlertSource->EnableWindow( bEnable );

	CWnd*	pFacility = GetDlgItem( IDC_FACILITY);
	if ( pFacility ) pFacility->EnableWindow( bEnable );

	CWnd*	pFolder = GetDlgItem( IDC_OUTPUT_DIRECTORY );
	if ( pFolder ) pFolder->EnableWindow( bEnable );

	CWnd*	pBrowse = GetDlgItem( IDC_BROWSE );
	if ( pBrowse ) pBrowse->EnableWindow( bEnable );
}

/*
// ----------------------------------------------------------------------------
//		OnBnClickedBrowse
// ----------------------------------------------------------------------------

void
CAlertingPage::OnBnClickedBrowse()
{
	LPMALLOC	pMalloc( NULL );
	if ( SUCCEEDED( ::SHGetMalloc( &pMalloc ) ) && pMalloc ) {
		LPITEMIDLIST	pidlRoot( NULL );
		::SHGetSpecialFolderLocation( NULL, CSIDL_DRIVES, &pidlRoot );

		BROWSEINFO	bInfo;
		::ZeroMemory( static_cast<PVOID>( &bInfo ), sizeof( BROWSEINFO ) );
		bInfo.hwndOwner = m_hWnd;
		bInfo.pidlRoot = pidlRoot;
		bInfo.pszDisplayName = NULL;
		bInfo.lpszTitle = _T( "Choose Output Directory:" );
		bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
		bInfo.lpfn = NULL;

		LPITEMIDLIST	pidlPath( ::SHBrowseForFolder( &bInfo ) );
		if ( pidlPath != NULL ) {

			TCHAR	TempLocation[ MAX_PATH ];
			TCHAR*	pszTempLocation( TempLocation );
			BOOL	bResult( ::SHGetPathFromIDList( pidlPath, pszTempLocation ) );

			if ( bResult ) {
				CPeekString	strTempLocation( pszTempLocation );

				m_Alerting.SetOutputDirectory( strTempLocation );
				SetDlgItemText( IDC_OUTPUT_DIRECTORY, m_Alerting.GetOutputDirectory() );
			}
		}
		pMalloc->Free( pidlRoot );
		pMalloc->Free( pidlPath );
	}
}
*/

// ----------------------------------------------------------------------------
//		OnBnClickedObscureNames
// ----------------------------------------------------------------------------

void CAlertingPage::OnBnClickedObscureNames()
{
	BOOL	bObscuring = (IsDlgButtonChecked( IDC_OBSCURE_NAMES ) == BST_CHECKED) ? TRUE : FALSE;

  #ifdef IMPLEMENT_PASSWORD_LOGGING
	BOOL	bEnable( !bObscuring );
	CWnd*	pShowPassword = GetDlgItem( IDC_SHOW_PASSWORD );
	if ( pShowPassword ) pShowPassword->EnableWindow( bEnable );
  #endif // IMPLEMENT_PASSWORD_LOGGING

	m_Logging.SetObscureNames( (bObscuring == TRUE) ? true : false );

  #ifdef IMPLEMENT_DATA_CAPTURE
	if ( bObscuring ) {
		m_Options.GetDataTransfer().SetDataTransferEnabled( false );
	}
  #endif // IMPLEMENT_DATA_CAPTURE
}


/*
// ----------------------------------------------------------------------------
//		OnBnClickedEnableLogging
// ----------------------------------------------------------------------------

void CAlertingPage::OnBnClickedEnableLogging()
{
	BOOL	bLogging = (IsDlgButtonChecked( IDC_ENABLE_LOGGING ) == BST_CHECKED) ? TRUE : FALSE;

	m_Logging.SetLogging( (bLogging == TRUE) ? true : false );

	BOOL	bEnable( bLogging );

	CWnd*	pObscuring = GetDlgItem( IDC_OBSCURE_NAMES );
  #ifdef IMPLEMENT_PASSWORD_LOGGING
	CWnd*	pShowPassword = GetDlgItem( IDC_SHOW_PASSWORD );
  #endif // IMPLEMENT_PASSWORD_LOGGING

	if ( !pObscuring 
	  #ifdef IMPLEMENT_PASSWORD_LOGGING
		|| !pShowPassword 
	  #endif IMPLEMENT_PASSWORD_LOGGING
		)	 return;

	if ( bEnable == TRUE ) {
		pObscuring->EnableWindow( TRUE );
		OnBnClickedObscureNames();
	}
	else {
		pObscuring->EnableWindow( FALSE );
	  #ifdef IMPLEMENT_PASSWORD_LOGGING
		pShowPassword->EnableWindow( FALSE );
	  #endif // IMPLEMENT_PASSWORD_LOGGING
	}
		
  #ifdef IMPLEMENT_DATA_CAPTURE
	if ( bLogging ) {
		m_Options.GetDataTransfer().SetDataTransferEnabled( false );
	}
  #endif // IMPLEMENT_DATA_CAPTURE
}
*/