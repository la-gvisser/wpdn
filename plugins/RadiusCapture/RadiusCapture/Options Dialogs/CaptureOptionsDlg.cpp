// ============================================================================
// CaptureOptionsDlg.cpp:
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "stdafx.h"
#include "CaptureOptionsDlg.h"


////////////////////////////////////////////////////////////////////////////////
//		CCaptureOptionsDlg
////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CCaptureOptionsDlg, CDialog)

// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CCaptureOptionsDlg::OnInitDialog()
{
	// Call inherited.
	__super::OnInitDialog();

	m_bInitializing  = true;

	OnBnClickedChkUseGlobalDefaults();

	SetControlsInitialState();

	m_bInitializing  = false;

	return TRUE;
}

// -----------------------------------------------------------------------------
//  Message Map
// -----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCaptureOptionsDlg, CDialog)
	ON_BN_CLICKED(IDC_CASE_CAPTURE_TO_FILE, OnBnClickedCaptureToFile)
	ON_BN_CLICKED(IDC_CASE_CAPTURE_TO_SCREEN, OnBnClickedCaptureToScreen)
	ON_EN_CHANGE(IDC_CASE_FILE_AGE_COUNT, OnEnChangeFileAgeCount)
	ON_CBN_SELCHANGE(IDC_CASE_FILE_AGE_UNITS, OnCbnSelchangeFileAgeUnits)
	ON_EN_CHANGE(IDC_CASE_FILE_SIZE_COUNT, OnEnChangeFileSizeCount)
	ON_CBN_SELCHANGE(IDC_CASE_FILE_SIZE_UNITS, OnCbnSelchangeFileSizeUnits)
	ON_BN_CLICKED(IDC_CASE_FILE_AGE, OnBnClickedFileAge)
	ON_BN_CLICKED(IDC_CASE_FILE_SIZE, OnBnClickedFileSize)
	ON_BN_CLICKED(IDC_CHK_USE_GLOBAL_DEFAULTS, OnBnClickedChkUseGlobalDefaults)
	ON_BN_CLICKED(IDC_CASE_LOG_MESSAGES_TO_FILE, OnBnClickedLogMsgsToFile)
	ON_BN_CLICKED(IDC_CASE_LOG_MESSAGES_TO_SCREEN, OnBnClickedLogMsgsToScreen)
	ON_BN_CLICKED(IDC_CASE_FOLDERS_FOR_CASES, OnBnClickedFoldersForCases)
END_MESSAGE_MAP()


// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
CCaptureOptionsDlg::DoDataExchange( CDataExchange* pDX )
{
	UInt32				nAgeCount;
	int					nAgeUnits;
	UInt64				nSizeCount;
	int					nSizeUnits;

	if ( !pDX->m_bSaveAndValidate ) {
		BOOL bIsCaptureToFile = static_cast<BOOL>( m_CaptureOptions.IsCaptureToFile() );
		DDX_Check( pDX, IDC_CASE_CAPTURE_TO_FILE, bIsCaptureToFile );

		BOOL bIsCaptureToScreen = static_cast<BOOL>( m_CaptureOptions.IsCaptureToScreen() );
		DDX_Check( pDX, IDC_CASE_CAPTURE_TO_SCREEN, bIsCaptureToScreen );

		BOOL bOptionsLogMsgsToFileButton = static_cast<BOOL>( m_CaptureOptions.IsLogMsgsToFileButton() );
		DDX_Check( pDX, IDC_CASE_LOG_MESSAGES_TO_FILE, bOptionsLogMsgsToFileButton );

		BOOL bOptionsFoldersForCasesButton = static_cast<BOOL>( m_CaptureOptions.IsFoldersForCasesButton() );
		DDX_Check( pDX, IDC_CASE_FOLDERS_FOR_CASES, bOptionsFoldersForCasesButton );

		BOOL bOptionsLogMsgsToScreen = static_cast<BOOL>( m_CaptureOptions.IsLogMsgsToScreen() );
		DDX_Check( pDX, IDC_CASE_LOG_MESSAGES_TO_SCREEN, bOptionsLogMsgsToScreen );

		BOOL bIsSaveAge = m_CaptureOptions.IsSaveAgeSelected();
		DDX_Check( pDX, IDC_CASE_FILE_AGE, bIsSaveAge );

		BOOL bIsSaveSize = m_CaptureOptions.IsSaveSizeSelected();
		DDX_Check( pDX, IDC_CASE_FILE_SIZE, bIsSaveSize );

		if ( !m_CaptureOptions.IsSaveAgeSelected() && !m_CaptureOptions.IsSaveSizeSelected() ) {
			m_CaptureOptions.SetSaveAgeSelected( true );
			m_CaptureOptions.SetSaveAgeValue( kSecondsInADay );	
		}

		const UInt32 nSaveAgeValue = m_CaptureOptions.GetSaveAgeSeconds();
		if ( (nSaveAgeValue % kSecondsInADay) == 0 ) {
			nAgeCount = nSaveAgeValue / kSecondsInADay;
			nAgeUnits = kSave_Days;
		}
		else if ( (nSaveAgeValue % kSecondsInAnHour) == 0 ) {
			nAgeCount = nSaveAgeValue / kSecondsInAnHour;
			nAgeUnits = kSave_Hours;
		}
		else {
			ASSERT( (nSaveAgeValue % kSecondsInAMinute) == 0 );
			nAgeCount = nSaveAgeValue / kSecondsInAMinute;
			nAgeUnits = kSave_Minutes;
		}

		const UInt64 nSaveSizeValue = m_CaptureOptions.GetSaveSizeBytes();
		if ( (nSaveSizeValue % kGigabyte) == 0 ) {
			nSizeCount = nSaveSizeValue / kGigabyte;
			nSizeUnits = kMax_Gigabytes;
		}
		else if ( (nSaveSizeValue % kMegabyte) == 0 ) {
			nSizeCount = nSaveSizeValue / kMegabyte;
			nSizeUnits = kMax_Megabytes;
		}
		else {
			nSizeCount = nSaveSizeValue / kKilobyte;
			nSizeUnits = kMax_Kilobytes;
		}

		DDX_Text( pDX, IDC_CASE_FILE_AGE_COUNT, nAgeCount );
		DDX_CBIndex( pDX, IDC_CASE_FILE_AGE_UNITS, nAgeUnits );

		DDX_Text( pDX, IDC_CASE_FILE_SIZE_COUNT, nSizeCount );
		DDX_CBIndex( pDX, IDC_CASE_FILE_SIZE_UNITS, nSizeUnits );

		BOOL bUseGlobalDefaults = static_cast<BOOL>( m_CaptureOptions.IsUseGlobalDefaults() );
		DDX_Check( pDX, IDC_CHK_USE_GLOBAL_DEFAULTS, bUseGlobalDefaults );

	}

	CDialog::DoDataExchange(pDX);

	if ( pDX->m_bSaveAndValidate ) {

		if ( m_bIsDockedTab ) return;

		BOOL bIsCaptureToFile = FALSE;
		DDX_Check( pDX, IDC_CASE_CAPTURE_TO_FILE, bIsCaptureToFile );
		if ( bIsCaptureToFile != static_cast<BOOL>(m_CaptureOptions.IsCaptureToFile()) ) {
			m_CaptureOptions.SetCaptureToFile( bIsCaptureToFile == TRUE );
		}

		BOOL bIsCaptureToScreen = FALSE;
		DDX_Check( pDX, IDC_CASE_CAPTURE_TO_SCREEN, bIsCaptureToScreen );
		if ( bIsCaptureToScreen != static_cast<BOOL>(m_CaptureOptions.IsCaptureToScreen()) ) {
			m_CaptureOptions.SetCaptureToScreen( bIsCaptureToScreen == TRUE );
		}

		if ( !bIsCaptureToFile && !bIsCaptureToScreen) {
			ASSERT ( 0 );
			bIsCaptureToScreen = TRUE;
		}

		BOOL bLogMsgsToFileButton;
		DDX_Check( pDX, IDC_CASE_LOG_MESSAGES_TO_FILE, bLogMsgsToFileButton );
		if ( bLogMsgsToFileButton != static_cast<BOOL>(m_CaptureOptions.IsLogMsgsToFileButton()) ) {
			m_CaptureOptions.SetLogMsgsToFileButton( (bLogMsgsToFileButton == TRUE) );
		}

		BOOL bOptionsFoldersForCasesButton;
		DDX_Check( pDX, IDC_CASE_FOLDERS_FOR_CASES, bOptionsFoldersForCasesButton );
		if ( bOptionsFoldersForCasesButton != static_cast<BOOL>(m_CaptureOptions.IsFoldersForCasesButton()) ) {
			m_CaptureOptions.SetFoldersForCasesButton( (bOptionsFoldersForCasesButton == TRUE ) );
		} 

		BOOL bLogMessagesToScreen;
		DDX_Check( pDX, IDC_CASE_LOG_MESSAGES_TO_SCREEN, bLogMessagesToScreen );
		if ( bLogMessagesToScreen != static_cast<BOOL>(m_CaptureOptions.IsLogMsgsToScreen()) ) {
			m_CaptureOptions.SetLogMsgsToScreen( (bLogMessagesToScreen == TRUE) );
		}

		BOOL bIsSaveAge = false;
		DDX_Check( pDX, IDC_CASE_FILE_AGE, bIsSaveAge );
		if ( bIsSaveAge != static_cast<BOOL>(m_CaptureOptions.IsSaveAgeSelected()) ) {
			m_CaptureOptions.SetSaveAgeSelected( bIsSaveAge == TRUE );
		}

		BOOL bIsSaveSize = false;
		DDX_Check( pDX, IDC_CASE_FILE_SIZE, bIsSaveSize );
		if ( bIsSaveSize != static_cast<BOOL>(m_CaptureOptions.IsSaveSizeSelected()) ) {
			m_CaptureOptions.SetSaveSizeSelected( bIsSaveSize == TRUE );
		}

		DDX_Text( pDX, IDC_CASE_FILE_AGE_COUNT, nAgeCount );
		DDX_CBIndex( pDX, IDC_CASE_FILE_AGE_UNITS, nAgeUnits );

		DDX_Text( pDX, IDC_CASE_FILE_SIZE_COUNT, nSizeCount );
		DDX_CBIndex( pDX, IDC_CASE_FILE_SIZE_UNITS, nSizeUnits );

		UInt32 nSaveAgeValue = 0;
		switch ( nAgeUnits ) {
			case kSave_Days:
				nSaveAgeValue = nAgeCount * kSecondsInADay;
				break;
			case kSave_Hours:
				nSaveAgeValue = nAgeCount * kSecondsInAnHour;
				break;
			default:	// kSave_Minutes
				nSaveAgeValue = nAgeCount * kSecondsInAMinute;
				break;
		}

		if ( nSaveAgeValue !=  m_CaptureOptions.GetSaveAgeSeconds() ) {
			m_CaptureOptions.SetSaveAgeValue( nSaveAgeValue );
		}

		UInt64 nSaveSizeValue = 0;
		switch ( nSizeUnits ) {
			case kMax_Gigabytes:
				nSaveSizeValue = nSizeCount * kGigabyte;
				break;
			case kMax_Megabytes:
				nSaveSizeValue = nSizeCount * kMegabyte;
				break;
			default:	// kMax_Kilobytes
				nSaveSizeValue = nSizeCount * kKilobyte;
				break;
		}

		if ( nSaveSizeValue !=  m_CaptureOptions.GetSaveSizeBytes() ) {
			m_CaptureOptions.SetSaveSizeValue( nSaveSizeValue );
		}

		BOOL bUseGlobalDefaults = false;
		DDX_Check( pDX, IDC_CHK_USE_GLOBAL_DEFAULTS, bUseGlobalDefaults );
		if ( bUseGlobalDefaults != static_cast<BOOL>(m_CaptureOptions.IsUseGlobalDefaults()) ) {
			m_CaptureOptions.SetUseGlobalDefaults( bUseGlobalDefaults == TRUE );
		}
	}
}

// -----------------------------------------------------------------------------
//  OnBnClickedCaptureToFile
// -----------------------------------------------------------------------------

void
CCaptureOptionsDlg::OnBnClickedCaptureToFile()
{
	if ( m_bIsDockedTab ) {
		CButton* pControl = (CButton*)GetDlgItem( IDC_CASE_CAPTURE_TO_FILE );
		pControl->SetCheck( m_CaptureOptions.IsCaptureToFile() );
	}

	CButton* pCaptureToFile = (CButton*)GetDlgItem( IDC_CASE_CAPTURE_TO_FILE );
	CButton* pCaptureToScreen = (CButton*)GetDlgItem( IDC_CASE_CAPTURE_TO_SCREEN );

	BOOL bCaptureToFileChecked = pCaptureToFile->GetCheck();
	BOOL bCaptureToScreenChecked = pCaptureToScreen->GetCheck();

	if ( !bCaptureToFileChecked && !bCaptureToScreenChecked ) {
		pCaptureToScreen->SetCheck( TRUE );
	}

	CWnd* pWnd;

	pWnd = GetDlgItem( IDC_CASE_LOG_MESSAGES_TO_FILE );
	pWnd->EnableWindow( bCaptureToFileChecked );
	pWnd = GetDlgItem( IDC_CASE_FOLDERS_FOR_CASES );
	pWnd->EnableWindow( bCaptureToFileChecked );
	pWnd = GetDlgItem( IDC_CASE_FILE_AGE );
	pWnd->EnableWindow( bCaptureToFileChecked );
	pWnd = GetDlgItem( IDC_CASE_FILE_SIZE );
	pWnd->EnableWindow( bCaptureToFileChecked );
	pWnd = GetDlgItem( IDC_NEW_FILE_EVERY );
	pWnd->EnableWindow( bCaptureToFileChecked );

	if ( !bCaptureToFileChecked ) {
		pWnd = GetDlgItem( IDC_CASE_FILE_AGE_COUNT );
		pWnd->EnableWindow( bCaptureToFileChecked );
		pWnd = GetDlgItem( IDC_CASE_FILE_AGE_UNITS );
		pWnd->EnableWindow( bCaptureToFileChecked );
		pWnd = GetDlgItem( IDC_CASE_FILE_SIZE_COUNT );
		pWnd->EnableWindow( bCaptureToFileChecked );
		pWnd = GetDlgItem( IDC_CASE_FILE_SIZE_UNITS );
		pWnd->EnableWindow( bCaptureToFileChecked );
	}
	else {
		OnBnClickedFileAge();
		OnBnClickedFileSize();
	}

}

// -----------------------------------------------------------------------------
//  OnBnClickedLogMsgsToFile
// -----------------------------------------------------------------------------

void
CCaptureOptionsDlg::OnBnClickedLogMsgsToFile()
{
	if ( m_bInitializing ) return;

	if ( m_bIsDockedTab ) {
		m_bInitializing = true;
		CButton* pControl = (CButton*)GetDlgItem( IDC_CASE_LOG_MESSAGES_TO_FILE );
		pControl->SetCheck( m_CaptureOptions.IsLogMsgsToFileButton() );
		m_bInitializing = false;
	}
}

// -----------------------------------------------------------------------------
//  OnBnClickedLogMsgsToScreen
// -----------------------------------------------------------------------------

void
CCaptureOptionsDlg::OnBnClickedLogMsgsToScreen()
{
	if ( m_bInitializing ) return;

	if ( m_bIsDockedTab ) {
		m_bInitializing = true;
		CButton* pControl = (CButton*)GetDlgItem( IDC_CASE_LOG_MESSAGES_TO_SCREEN );
		pControl->SetCheck( m_CaptureOptions.IsLogMsgsToScreen() );
		m_bInitializing = false;
	}
}

// -----------------------------------------------------------------------------
//  SetControlsInitialState
// -----------------------------------------------------------------------------

void
CCaptureOptionsDlg::SetControlsInitialState()
{
	if ( m_bIsCapturing || m_bIsDockedTab ) {
		CWnd* pWnd;

		CButton* pChkUseGlobalDefaults = (CButton*)GetDlgItem( IDC_CHK_USE_GLOBAL_DEFAULTS );
		BOOL bUseGlobalDefaults = pChkUseGlobalDefaults->GetCheck();
		if ( bUseGlobalDefaults && !m_bIsDockedTab ) {
			return;
		}

		OnBnClickedCaptureToFile();

		pWnd = GetDlgItem( IDC_CASE_CAPTURE_TO_SCREEN );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_CASE_CAPTURE_TO_FILE );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_CASE_LOG_MESSAGES_TO_FILE );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_CASE_FOLDERS_FOR_CASES );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_CASE_LOG_MESSAGES_TO_SCREEN );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_CHK_USE_GLOBAL_DEFAULTS );
		pWnd->EnableWindow( FALSE );

		if ( m_bIsDockedTab ) {
			pWnd = GetDlgItem( IDC_CASE_FILE_AGE );
			pWnd->EnableWindow( FALSE );
			pWnd = GetDlgItem( IDC_CASE_FILE_AGE_COUNT );
			pWnd->EnableWindow( FALSE );
			pWnd = GetDlgItem( IDC_CASE_FILE_AGE_UNITS );
			pWnd->EnableWindow( FALSE );
			pWnd = GetDlgItem( IDC_CASE_FILE_SIZE );
			pWnd->EnableWindow( FALSE );
			pWnd = GetDlgItem( IDC_CASE_FILE_SIZE_COUNT );
			pWnd->EnableWindow( FALSE );
			pWnd = GetDlgItem( IDC_CASE_FILE_SIZE_UNITS );
			pWnd->EnableWindow( FALSE );
		}
	}
}

// -----------------------------------------------------------------------------
//  OnBnClickedCaptureToScreen
// -----------------------------------------------------------------------------

void
CCaptureOptionsDlg::OnBnClickedCaptureToScreen()
{
	if ( m_bIsDockedTab ) {
		CButton* pControl = (CButton*)GetDlgItem( IDC_CASE_CAPTURE_TO_SCREEN );
		pControl->SetCheck( m_CaptureOptions.IsCaptureToScreen() );
	}

	CButton* pCaptureToFile = (CButton*)GetDlgItem( IDC_CASE_CAPTURE_TO_FILE );
	CButton* pCaptureToScreen = (CButton*)GetDlgItem( IDC_CASE_CAPTURE_TO_SCREEN );

	BOOL bCaptureToFileChecked = pCaptureToFile->GetCheck();
	BOOL bCaptureToScreenChecked = pCaptureToScreen->GetCheck();

	if ( !bCaptureToFileChecked && !bCaptureToScreenChecked ) {
		pCaptureToFile->SetCheck( TRUE );
		OnBnClickedCaptureToFile();
	}
}


// -----------------------------------------------------------------------------
//  OnEnChangeFileAgeCount
// -----------------------------------------------------------------------------

void
CCaptureOptionsDlg::OnEnChangeFileAgeCount()
{
	if ( m_bInitializing ) return;

	if ( m_bIsDockedTab ) {
		SetAgeValues();
		return;
	}

	CString strAgeCount;
	int		nAgeUnits;

	CEdit* pAgeCountEdit = (CEdit*)GetDlgItem( IDC_CASE_FILE_AGE_COUNT );
	CComboBox* pAgeCountCombo = (CComboBox*)GetDlgItem( IDC_CASE_FILE_AGE_UNITS );

	pAgeCountEdit->GetWindowText( strAgeCount );

	if ( strAgeCount.GetLength() == 0 ) {
		return;
	}

	if ( strAgeCount == _T("0")) {
		pAgeCountEdit->SetWindowText( _T("1"));
		pAgeCountEdit->SetFocus();
		pAgeCountEdit->SetSel( 1, 1 );
		return;
	}

	UInt32 nAgeCount = _ttol( strAgeCount );

	if ( nAgeCount == 0 ) {
		pAgeCountEdit->SetWindowText( _T(""));
		pAgeCountEdit->SetFocus();
		return;
	}

	nAgeUnits = pAgeCountCombo->GetCurSel();

	if ( nAgeUnits == kSave_Minutes ) {
		if ( nAgeCount > kMinutesInAnHour * kAgeSizeEditFactor ) {
			CString strAgeMax;;
			strAgeMax.Format( _T("%d"), kMinutesInAnHour * kAgeSizeEditFactor );
			pAgeCountEdit->SetWindowText( strAgeMax );
			pAgeCountEdit->SetFocus();
			int nLen = strAgeMax.GetLength();
			pAgeCountEdit->SetSel( nLen, nLen );
			return;
		}
	}
	else if ( nAgeUnits == kSave_Hours ) {
		if ( nAgeCount > kHoursInADay * kAgeSizeEditFactor ) {
			CString strAgeMax;;
			strAgeMax.Format( _T("%d"), kHoursInADay * kAgeSizeEditFactor );
			pAgeCountEdit->SetWindowText( strAgeMax );
			pAgeCountEdit->SetFocus();
			int nLen = strAgeMax.GetLength();
			pAgeCountEdit->SetSel( nLen, nLen );
			return;
		}
	}
	else if ( nAgeUnits == kSave_Days ) {
		if ( nAgeCount > kDaysEditMax ) {
			CString strAgeMax;;
			strAgeMax.Format( _T("%d"), kDaysEditMax );
			pAgeCountEdit->SetWindowText( strAgeMax );
			pAgeCountEdit->SetFocus();
			int nLen = strAgeMax.GetLength();
			pAgeCountEdit->SetSel( nLen, nLen );
			return;
		}
	}

	for ( int i = 0; i < strAgeCount.GetLength(); i++ ) {
		wchar_t c = strAgeCount[i];
		if ( !isdigit(c) ) {
			strAgeCount.Format( _T("%d"), nAgeCount );
			pAgeCountEdit->SetWindowText( strAgeCount );
			pAgeCountEdit->SetFocus();
			int nLen = strAgeCount.GetLength();
			pAgeCountEdit->SetSel( nLen, nLen );
			return;
		}
	}
}

// -----------------------------------------------------------------------------
//  OnCbnSelchangeFileAgeUnits
// -----------------------------------------------------------------------------

void
CCaptureOptionsDlg::OnCbnSelchangeFileAgeUnits()
{
	if ( m_bInitializing ) return;

	if ( m_bIsDockedTab ) {
		SetAgeValues();
		return;
	}

	OnEnChangeFileAgeCount();
}

// -----------------------------------------------------------------------------
//  OnEnChangeFileSizeCount
// -----------------------------------------------------------------------------

void
CCaptureOptionsDlg::OnEnChangeFileSizeCount()
{
	if ( m_bInitializing ) return;

	if ( m_bIsDockedTab ) {
		SetSizeValues();
		return;
	}

	CString strSizeCount;

	CEdit* pSizeCountEdit = (CEdit*)GetDlgItem( IDC_CASE_FILE_SIZE_COUNT );

	pSizeCountEdit->GetWindowText( strSizeCount );

	if ( strSizeCount.GetLength() == 0 ) {
		return;
	}

	if ( strSizeCount == _T("0")) {
		pSizeCountEdit->SetWindowText( _T("1"));
		pSizeCountEdit->SetFocus();
		pSizeCountEdit->SetSel( 1, 1 );
		return;
	}

	UInt32 nSizeCount = _ttol( strSizeCount );

	if ( nSizeCount == 0 ) {
		pSizeCountEdit->SetWindowText( _T(""));
		pSizeCountEdit->SetFocus();
		return;
	}

	const UInt32 nSizeEditMax = 1024 * kAgeSizeEditFactor;

	if ( nSizeCount > nSizeEditMax ) {
		CString strSizeEditMax;
		strSizeEditMax.Format( _T("%d"), nSizeEditMax );
		pSizeCountEdit->SetWindowText( strSizeEditMax );
		pSizeCountEdit->SetFocus();
		int nLen = strSizeEditMax.GetLength();
		pSizeCountEdit->SetSel( nLen, nLen );
		return;
	}

	for ( int i = 0; i < strSizeCount.GetLength(); i++ ) {
		wchar_t c = strSizeCount[i];
		if ( !isdigit(c) ) {
			strSizeCount.Format( _T("%d"), nSizeCount );
			pSizeCountEdit->SetWindowText( strSizeCount );
			pSizeCountEdit->SetFocus();
			int nLen = strSizeCount.GetLength();
			pSizeCountEdit->SetSel( nLen, nLen );
			return;
		}
	}
}

// -----------------------------------------------------------------------------
//  OnCbnSelchangeFileSizeUnits
// -----------------------------------------------------------------------------

void
CCaptureOptionsDlg::OnCbnSelchangeFileSizeUnits()
{
	if ( m_bInitializing ) return;

	if ( m_bIsDockedTab ) {
		SetSizeValues();
		return;
	}

	OnEnChangeFileSizeCount();
}

// -----------------------------------------------------------------------------
//		OnBnClickedFileAge
// -----------------------------------------------------------------------------

void
CCaptureOptionsDlg::OnBnClickedFileAge()
{
	if ( m_bIsDockedTab ) {
		CButton* pControl = (CButton*)GetDlgItem( IDC_CASE_FILE_AGE );
		pControl->SetCheck( m_CaptureOptions.IsSaveAgeSelected() );
	}

	CButton* chkFileAge = (CButton*)GetDlgItem( IDC_CASE_FILE_AGE );
	CButton* chkSizeAge = (CButton*)GetDlgItem( IDC_CASE_FILE_SIZE );

	BOOL bFileAgeChecked = chkFileAge->GetCheck();
	BOOL bFileSizeChecked = chkSizeAge->GetCheck();

	if ( !bFileAgeChecked && !bFileSizeChecked ) {
		chkSizeAge->SetCheck( TRUE );
		OnBnClickedFileSize();
	}

	CWnd* pWnd;

	pWnd = GetDlgItem( IDC_CASE_FILE_AGE_COUNT );
	pWnd->EnableWindow( bFileAgeChecked );
	pWnd = GetDlgItem( IDC_CASE_FILE_AGE_UNITS );
	pWnd->EnableWindow( bFileAgeChecked );
}

// -----------------------------------------------------------------------------
//		OnBnClickedFileSize
// -----------------------------------------------------------------------------

void
CCaptureOptionsDlg::OnBnClickedFileSize()
{
	if ( m_bIsDockedTab ) {
		CButton* pControl = (CButton*)GetDlgItem( IDC_CASE_FILE_SIZE );
		pControl->SetCheck( m_CaptureOptions.IsSaveSizeSelected() );
	}

	CButton* chkFileAge = (CButton*)GetDlgItem( IDC_CASE_FILE_AGE );
	CButton* chkSizeAge = (CButton*)GetDlgItem( IDC_CASE_FILE_SIZE );

	BOOL bFileAgeChecked = chkFileAge->GetCheck();
	BOOL bFileSizeChecked = chkSizeAge->GetCheck();

	if ( !bFileAgeChecked && !bFileSizeChecked ) {
		chkFileAge->SetCheck( TRUE );
		OnBnClickedFileAge();
	}

	CWnd* pWnd;

	pWnd = GetDlgItem( IDC_CASE_FILE_SIZE_COUNT );
	pWnd->EnableWindow( bFileSizeChecked );
	pWnd = GetDlgItem( IDC_CASE_FILE_SIZE_UNITS );
	pWnd->EnableWindow( bFileSizeChecked );
}

// -----------------------------------------------------------------------------
//		OnBnClickedChkUseGlobalDefaults
// -----------------------------------------------------------------------------

void
CCaptureOptionsDlg::OnBnClickedChkUseGlobalDefaults()
{
	if ( m_bIsDockedTab ) {
		CButton* pControl = (CButton*)GetDlgItem( IDC_CHK_USE_GLOBAL_DEFAULTS );
		pControl->SetCheck( m_CaptureOptions.IsUseGlobalDefaults() );
	}

	CButton* chkUseGlobalDefaults = (CButton*)GetDlgItem( IDC_CHK_USE_GLOBAL_DEFAULTS );
	ASSERT( chkUseGlobalDefaults );
	bool bUseGlobalDefaults = (chkUseGlobalDefaults->GetCheck() == TRUE) ? true : false;

	m_CaptureOptions.SetGlobalDefaultsFlag( bUseGlobalDefaults );

	if ( bUseGlobalDefaults == true ) {
		m_CaptureOptions.SyncGlobalDefaults( m_GlobalCaptureOptions );
		UpdateData( FALSE );
	}

	SetControlsEnabled();
}

// -----------------------------------------------------------------------------
//		SetControlsEnabled
// -----------------------------------------------------------------------------

void
CCaptureOptionsDlg::SetControlsEnabled()
{
	BOOL bEnableFileOrScreenCtls;
	BOOL bEnableFileAgeCtls;

	CButton* pChkUseGlobalDefaults = (CButton*)GetDlgItem( IDC_CHK_USE_GLOBAL_DEFAULTS );
	BOOL bUseGlobalDefaults = pChkUseGlobalDefaults->GetCheck();

	CWnd* pWnd = NULL;

	if ( m_bIsCapturing ) {
		pChkUseGlobalDefaults->EnableWindow( FALSE );
	}
	else {
		pChkUseGlobalDefaults->EnableWindow( TRUE );
	}

	if ( bUseGlobalDefaults ) {
		bEnableFileOrScreenCtls = FALSE;
		bEnableFileAgeCtls = FALSE;

		pWnd = GetDlgItem( IDC_CASE_CAPTURE_TO_FILE );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_CASE_CAPTURE_TO_SCREEN );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_CASE_LOG_MESSAGES_TO_FILE );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_CASE_FOLDERS_FOR_CASES );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_CASE_LOG_MESSAGES_TO_SCREEN );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_CASE_FILE_AGE );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_CASE_FILE_SIZE );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_CASE_FILE_AGE_COUNT );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_CASE_FILE_AGE_UNITS );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_CASE_FILE_SIZE_COUNT );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_CASE_FILE_SIZE_UNITS );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_NEW_FILE_EVERY );
		pWnd->EnableWindow( FALSE );
	}
	else {
		if ( m_bIsCapturing ) {
			bEnableFileOrScreenCtls = FALSE;
		}
		else {
			bEnableFileOrScreenCtls = TRUE;
		}

		pWnd = GetDlgItem( IDC_CASE_CAPTURE_TO_FILE );
		pWnd->EnableWindow( TRUE );
		pWnd = GetDlgItem( IDC_CASE_CAPTURE_TO_SCREEN );
		pWnd->EnableWindow( TRUE );
		pWnd = GetDlgItem( IDC_CASE_LOG_MESSAGES_TO_SCREEN );
		pWnd->EnableWindow( TRUE );

		OnBnClickedCaptureToFile();
	}
}

// -----------------------------------------------------------------------------
//  SetAgeValues
// -----------------------------------------------------------------------------

void
CCaptureOptionsDlg::SetAgeValues()
{
	if ( m_bInitializing ) return;
	m_bInitializing  = true;

	UInt32				nAgeCount;
	int					nAgeUnits;

	const UInt32 nSaveAgeValue = m_CaptureOptions.GetSaveAgeSeconds();
	if ( (nSaveAgeValue % kSecondsInADay) == 0 ) {
		nAgeCount = nSaveAgeValue / kSecondsInADay;
		nAgeUnits = kSave_Days;
	}
	else if ( (nSaveAgeValue % kSecondsInAnHour) == 0 ) {
		nAgeCount = nSaveAgeValue / kSecondsInAnHour;
		nAgeUnits = kSave_Hours;
	}
	else {
		ASSERT( (nSaveAgeValue % kSecondsInAMinute) == 0 );
		nAgeCount = nSaveAgeValue / kSecondsInAMinute;
		nAgeUnits = kSave_Minutes;
	}

	CString strSaveAge;
	strSaveAge.Format( _T("%u"),  nAgeCount );

	CEdit* pAgeCountEdit = (CEdit*)GetDlgItem( IDC_CASE_FILE_AGE_COUNT );
	pAgeCountEdit->SetWindowText( strSaveAge );
	CComboBox* pAgeCountCombo = (CComboBox*)GetDlgItem( IDC_CASE_FILE_AGE_UNITS );
	pAgeCountCombo->SetCurSel( nAgeUnits );

	m_bInitializing  = false;
}

// -----------------------------------------------------------------------------
//  SetSizeValues
// -----------------------------------------------------------------------------

void
CCaptureOptionsDlg::SetSizeValues()
{
	if ( m_bInitializing ) return;
	m_bInitializing  = true;

	UInt64				nSizeCount;
	int					nSizeUnits;

	const UInt64 nSaveSizeValue = m_CaptureOptions.GetSaveSizeBytes();
	if ( (nSaveSizeValue % kGigabyte) == 0 ) {
		nSizeCount = nSaveSizeValue / kGigabyte;
		nSizeUnits = kMax_Gigabytes;
	}
	else if ( (nSaveSizeValue % kMegabyte) == 0 ) {
		nSizeCount = nSaveSizeValue / kMegabyte;
		nSizeUnits = kMax_Megabytes;
	}
	else {
		nSizeCount = nSaveSizeValue / kKilobyte;
		nSizeUnits = kMax_Kilobytes;
	}

	CString strSaveSize;
	strSaveSize.Format( _T("%u"),  nSizeCount );

	CEdit* pSizeCountEdit = (CEdit*)GetDlgItem( IDC_CASE_FILE_SIZE_COUNT );
	pSizeCountEdit->SetWindowText( strSaveSize );
	CComboBox* pSizeCountCombo = (CComboBox*)GetDlgItem( IDC_CASE_FILE_SIZE_UNITS );
	pSizeCountCombo->SetCurSel( nSizeUnits );

	m_bInitializing  = false;
}


// -----------------------------------------------------------------------------
//  OnBnClickedFoldersForCases
// -----------------------------------------------------------------------------

void
CCaptureOptionsDlg::OnBnClickedFoldersForCases()
{
	if ( m_bInitializing ) return;

	CButton* pControl = (CButton*)GetDlgItem( IDC_CASE_FOLDERS_FOR_CASES );
	ASSERT( pControl );
	if ( !pControl ) return;

	if ( m_bIsDockedTab ) {
		m_bInitializing = true;
		pControl->SetCheck( m_CaptureOptions.IsFoldersForCases() );
		m_bInitializing = false;
		return;
	}

	BOOL bCheck = pControl->GetCheck();
	m_CaptureOptions.SetFoldersForCasesButton( bCheck ? true : false );
}
