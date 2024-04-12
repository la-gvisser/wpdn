// ============================================================================
//	DataTransferPage.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#include "StdAfx.h"
#include "DataTransferPage.h"

#ifdef IMPLEMENT_DATA_CAPTURE

// -----------------------------------------------------------------------------
//		CDataTransferPage
// -----------------------------------------------------------------------------

CDataTransferPage::CDataTransferPage(
	COptions&		inOptions )
	:	CPropertyPage( CDataTransferPage::IDD )
	,	m_Options( inOptions )
	,	m_DataTransfer( inOptions.GetDataTransfer() )
{
}


// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
CDataTransferPage::DoDataExchange(
	CDataExchange* pDX )
{
	CPropertyPage::DoDataExchange(pDX);

	BOOL	bDataTransferEnabled( FALSE );
		
	BOOL	bCaptureToFileEnabled( FALSE );
	CString strFileCaptureDir;
	BOOL	bListResultsToFileEnabled( FALSE );
	CString strListResultsToFileDir;
	BOOL	bLogFtpCmdsEnabled( FALSE );
	CString strLogFtpCmdsDir;

	if ( !pDX->m_bSaveAndValidate ) {
		bDataTransferEnabled = static_cast<BOOL>( m_DataTransfer.IsDataTransferEnabled() );

		bCaptureToFileEnabled = static_cast<BOOL>( m_DataTransfer.IsFileCaptureEnabled() );
		strFileCaptureDir =  m_DataTransfer.GetFileCaptureDir();
		bListResultsToFileEnabled = static_cast<BOOL>( m_DataTransfer.IsListResultsToFileEnabled() );		
		strListResultsToFileDir = m_DataTransfer.GetListResultsToFileDir();
		bLogFtpCmdsEnabled = static_cast<BOOL>( m_DataTransfer.IsLogFtpCmdsEnabled() );
		strLogFtpCmdsDir = m_DataTransfer.GetLogFtpCmdsDir();
	}

	DDX_Check( pDX, IDC_DO_DATATRANSFER, bDataTransferEnabled );

	DDX_Check( pDX, IDC_CAPTURE_FILE_TO_DISK_CHK, bCaptureToFileEnabled );
	DDX_Text( pDX, IDC_CAPTURE_FILE_TO_DISK_EDT, strFileCaptureDir );
	DDX_Check( pDX, IDC_LIST_RESULTS_TO_FILE_CHK, bListResultsToFileEnabled );
	DDX_Text( pDX, IDC_LIST_RESULTS_TO_FILE_EDT, strListResultsToFileDir );
	DDX_Check( pDX, IDC_LOG_FTP_CMDS_CHK, bLogFtpCmdsEnabled );
	DDX_Text( pDX, IDC_LOG_FTP_CMDS_EDT, strLogFtpCmdsDir );

	if ( pDX->m_bSaveAndValidate ) {
		m_DataTransfer.SetDataTransferEnabled( (bDataTransferEnabled == TRUE) ? true : false );
		m_DataTransfer.SetFileCaptureEnabled( (bCaptureToFileEnabled == TRUE) ? true : false );
		m_DataTransfer.SetFileCaptureDir( strFileCaptureDir.GetString() );
		m_DataTransfer.SetListResultsToFileEnabled( (bListResultsToFileEnabled == TRUE) ? true : false );
		m_DataTransfer.SetListResultsToFileDir( strListResultsToFileDir.GetString() );
		m_DataTransfer.SetLogFtpCmdsEnabled( (bLogFtpCmdsEnabled == TRUE) ? true : false );
		m_DataTransfer.SetLogFtpCmdsDir( strLogFtpCmdsDir.GetString() );
	}
}


// -----------------------------------------------------------------------------
//		EnableControls
// -----------------------------------------------------------------------------

void			
CDataTransferPage::EnableControls(
	bool inEnable )
{
	BOOL bEnable( inEnable ? TRUE : FALSE );

	CWnd* pCaptureToDirChkWnd	= GetDlgItem( IDC_CAPTURE_FILE_TO_DISK_CHK );
	CWnd* pCaptureToDirEdtWnd	= GetDlgItem( IDC_CAPTURE_FILE_TO_DISK_EDT );
	CWnd* pLogToFileChkWnd		= GetDlgItem( IDC_LIST_RESULTS_TO_FILE_CHK );
	CWnd* pLogToFileEdtWnd		= GetDlgItem( IDC_LIST_RESULTS_TO_FILE_EDT );
	CWnd* pLogFtpCmdsChkWnd		= GetDlgItem( IDC_LOG_FTP_CMDS_CHK );
	CWnd* pLogFtpCmdsEdtWnd		= GetDlgItem( IDC_LOG_FTP_CMDS_EDT );
	CWnd* pExplanationMsgWnd	= GetDlgItem( IDC_DATA_CAPTURE_EXPLANATION );

	if ( bEnable ) {
		if ( pCaptureToDirChkWnd )	pCaptureToDirChkWnd->EnableWindow( TRUE );
		if ( pLogToFileChkWnd )		pLogToFileChkWnd->EnableWindow( TRUE );
		if ( pLogFtpCmdsChkWnd )	pLogFtpCmdsChkWnd->EnableWindow( TRUE );
		if ( pExplanationMsgWnd )	pExplanationMsgWnd->EnableWindow( TRUE );

		OnBnClickedCaptureToDirChk();
		OnBnClickedLogToFileChk();
		OnBnClickedLogFtpCmdsChk();
	}
	else {
		if ( pCaptureToDirChkWnd )	pCaptureToDirChkWnd->EnableWindow( FALSE );
		if ( pCaptureToDirEdtWnd )	pCaptureToDirEdtWnd->EnableWindow( FALSE );
		if ( pLogToFileChkWnd )		pLogToFileChkWnd->EnableWindow( FALSE );
		if ( pLogToFileEdtWnd )		pLogToFileEdtWnd->EnableWindow( FALSE );
		if ( pLogFtpCmdsChkWnd )	pLogFtpCmdsChkWnd->EnableWindow( FALSE );
		if ( pLogFtpCmdsEdtWnd )	pLogFtpCmdsEdtWnd->EnableWindow( FALSE );
		if ( pExplanationMsgWnd )	pExplanationMsgWnd->EnableWindow( FALSE );
	}
}

/*
// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CDataTransferPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
*/

// -----------------------------------------------------------------------------
//		OnSetActive
// -----------------------------------------------------------------------------

BOOL
CDataTransferPage::OnSetActive()
{
	bool	bIsObscuring( IsObscuring() );
	
	if ( bIsObscuring ) {
		CheckDlgButton( IDC_DO_DATATRANSFER, 0 );
	}

	CWnd*	pObscureMsgWnd( GetDlgItem( IDC_OBSCURE_MESSAGE ) );
	if ( pObscureMsgWnd )		pObscureMsgWnd->ShowWindow( bIsObscuring ? 1 : 0 );

	CWnd*	pDoDataTransferWnd( GetDlgItem( IDC_DO_DATATRANSFER ) );
	if ( pDoDataTransferWnd )	pDoDataTransferWnd->EnableWindow( !bIsObscuring ? TRUE : FALSE );

	OnBnClickedDoDatatransfer();

	return CPropertyPage::OnSetActive();
}


BEGIN_MESSAGE_MAP( CDataTransferPage, CPropertyPage )
	ON_BN_CLICKED(IDC_DO_DATATRANSFER, &CDataTransferPage::OnBnClickedDoDatatransfer)
	ON_BN_CLICKED(IDC_CAPTURE_FILE_TO_DISK_CHK, &CDataTransferPage::OnBnClickedCaptureToDirChk)
	ON_BN_CLICKED(IDC_LIST_RESULTS_TO_FILE_CHK, &CDataTransferPage::OnBnClickedLogToFileChk)
	ON_BN_CLICKED(IDC_LOG_FTP_CMDS_CHK, &CDataTransferPage::OnBnClickedLogFtpCmdsChk)
END_MESSAGE_MAP()


void CDataTransferPage::OnBnClickedDoDatatransfer()
{
	bool bEnable = IsDlgButtonChecked( IDC_DO_DATATRANSFER ) == TRUE ? true : false;

	EnableControls( bEnable );
}


void CDataTransferPage::OnBnClickedCaptureToDirChk()
{
	BOOL bEnable = IsDlgButtonChecked( IDC_CAPTURE_FILE_TO_DISK_CHK );

	CWnd* pCaptureFilesToDiskEdtWnd = GetDlgItem( IDC_CAPTURE_FILE_TO_DISK_EDT );
	if ( pCaptureFilesToDiskEdtWnd ) pCaptureFilesToDiskEdtWnd->EnableWindow( bEnable );
}


void CDataTransferPage::OnBnClickedLogToFileChk()
{
	BOOL bEnable = IsDlgButtonChecked( IDC_LIST_RESULTS_TO_FILE_CHK );

	CWnd* pLogToFileEdtWnd = GetDlgItem( IDC_LIST_RESULTS_TO_FILE_EDT );
	if ( pLogToFileEdtWnd ) pLogToFileEdtWnd->EnableWindow( bEnable );
}


void CDataTransferPage::OnBnClickedLogFtpCmdsChk()
{
	BOOL bEnable = IsDlgButtonChecked( IDC_LOG_FTP_CMDS_CHK );

	CWnd* pLogFtpCmdsEdtWnd = GetDlgItem( IDC_LOG_FTP_CMDS_EDT );
	if ( pLogFtpCmdsEdtWnd ) pLogFtpCmdsEdtWnd->EnableWindow( bEnable );
}

#endif // IMPLEMENT_DATA_CAPTURE
