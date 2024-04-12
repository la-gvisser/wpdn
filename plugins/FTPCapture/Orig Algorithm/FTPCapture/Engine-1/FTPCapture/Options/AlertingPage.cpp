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
	CAlerting&	inAlerting,
	CLogging&	inLogging )
	:	CPropertyPage( CAlertingPage::IDD )
	,	m_Alerting( inAlerting )
	,	m_Logging( inLogging )
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

	if ( !pDX->m_bSaveAndValidate ) {
		strSource =  m_Alerting.m_strSource;
		strFacility = m_Alerting.m_strFacility;
		strOutputDirectory = m_Alerting.m_strOutputDirectory;
	}

	DDX_Check( pDX, IDC_OBSCURE_NAMES, m_Logging.m_bObscureNames );
	DDX_Check( pDX, IDC_DO_ALERTS, m_Alerting.m_bEnabled );
	DDX_Text( pDX, IDC_ALERT_SOURCE, strSource );
	DDX_Text( pDX, IDC_FACILITY, strFacility );
	DDX_Text( pDX, IDC_OUTPUT_DIRECTORY, strOutputDirectory );

	if ( pDX->m_bSaveAndValidate ) {
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
	OnBnClickedAlerting();
	return TRUE;
}


// -----------------------------------------------------------------------------
//		Message Map
// -----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CAlertingPage, CPropertyPage)
	ON_BN_CLICKED(IDC_DO_ALERTS, OnBnClickedAlerting)
	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
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

/*
			CPeekString strTempLocation1( m_Alerting.GetOutputDirectory() );
			CPeekString	strTempLocation( strTempLocation1.Format(), MAX_PATH );
//			CPeekString	strTempLocation( m_Alerting.GetOutputDirectory().Format() );
			TCHAR*	pszTempLocation( strTempLocation );
//			TCHAR*	pszTempLocation( strTempLocation.GetBufferSetLength( MAX_PATH ) );
			BOOL	bResult( ::SHGetPathFromIDList( pidlPath, pszTempLocation ) );
			strTempLocation.ReleaseBuffer();
			if ( bResult ) {
				m_Alerting.SetOutputDirectory( strTempLocation.GetString() );
				SetDlgItemText( IDC_OUTPUT_DIRECTORY, m_Alerting.GetOutputDirectory() );
			}
*/
		}
		pMalloc->Free( pidlRoot );
		pMalloc->Free( pidlPath );
	}
}
