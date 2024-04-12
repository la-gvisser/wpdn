// ============================================================================
//	ModePage.cpp
//		implementation of the CModePage class.
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef HE_WIN32

#include "ModePage.h"
#include <afxwin.h>


// ===========================================================================
//		CModePage
// ===========================================================================

IMPLEMENT_DYNAMIC(CModePage, CDialog)

CModePage::CModePage(
	CCase&	inCase )
	:	CDialog()
	,	m_Settings( inCase.GetSettings() )
	,	m_ReportMode( m_Settings.GetReportMode() )
{
}

CModePage::~CModePage()
{
}


// ----------------------------------------------------------------------------
//		DoDataExchange
// ----------------------------------------------------------------------------

void
CModePage::DoDataExchange(
	CDataExchange* pDX )
{
	if ( !pDX->m_bSaveAndValidate ) {
		m_strFileNamePrefix = static_cast<PCTSTR>( m_Settings.GetFileNamePrefix() );
		m_strOutputDirectory = static_cast<PCTSTR>( m_Settings.GetOutputDirectory() );

		if ( !m_ReportMode.m_bSaveAge && !m_ReportMode.m_bSaveSize ) {
			m_ReportMode.m_bSaveAge = TRUE;
			m_ReportMode.m_SaveAgeValue.Set( kSeconds, kSecondsInADay );
		}

		UInt32	nSeconds = static_cast<UInt32>( m_ReportMode.m_SaveAgeValue.Get( kSeconds ) );
		if ( (nSeconds % kSecondsInADay) == 0 ) {
			m_nTimeCount = nSeconds / kSecondsInADay;
			m_nTimeUnits = kSave_Days;
		}
		else if ( (nSeconds % kSecondsInAnHour) == 0 ) {
			m_nTimeCount = nSeconds / kSecondsInAnHour;
			m_nTimeUnits = kSave_Hours;
		}
		else {
			m_nTimeCount = nSeconds / kSecondsInAMinute;
			m_nTimeUnits = kSave_Minutes;
		}

		if ( (m_ReportMode.m_nSaveSizeValue % kBytesInGigabyte) == 0 ) {
			m_nSizeCount = static_cast<UInt32>( m_ReportMode.m_nSaveSizeValue / kBytesInGigabyte );
			m_nSizeUnits = kMax_Gigabytes;
		}
		else if ( (m_ReportMode.m_nSaveSizeValue % kBytesInMegabyte) == 0 ) {
			m_nSizeCount = static_cast<UInt32>( m_ReportMode.m_nSaveSizeValue / kBytesInMegabyte );
			m_nSizeUnits = kMax_Megabytes;
		}
		else {
			m_nSizeCount = static_cast<UInt32>( m_ReportMode.m_nSaveSizeValue / kBytesInKilobyte );
			m_nSizeUnits = kMax_Kilobytes;
		}
	}

	CDialog::DoDataExchange( pDX );

	DDX_Check( pDX, IDC_PROTOCOL_SMTP, m_Settings.m_Protocols[kProtocol_SMTP] );
	DDX_Check( pDX, IDC_PROTOCOL_POP3, m_Settings.m_Protocols[kProtocol_POP3] );

	DDX_Check( pDX, IDC_ACCEPT_PACKETS, m_Settings.m_bAcceptPackets );
	DDX_Check( pDX, IDC_WRITE_PACKETS, m_Settings.m_bWritePackets );
	DDX_Check( pDX, IDC_DISPLAY_SUMMARY, m_Settings.m_bSummaryStats );
	DDX_Check( pDX, IDC_DISPLAY_LOG, m_Settings.m_bLogMessages );

	DDX_Text( pDX, IDC_FILENAME_PREFIX, m_strFileNamePrefix );
	DDX_Check( pDX, IDC_APPEND_EMAIL, m_Settings.m_bAppendEmail );
	DDX_Text( pDX, IDC_OUTPUT_DIRECTORY, m_strOutputDirectory );

	DDX_Radio( pDX, IDC_MODE_PEN, m_ReportMode.m_Mode );
	DDX_Check( pDX, IDC_PEN_EML, m_ReportMode.m_bPenEmail );
	DDX_Check( pDX, IDC_PEN_XML, m_ReportMode.m_bPenXml );
	DDX_Check( pDX, IDC_PEN_TEXT, m_ReportMode.m_bPenText );
	DDX_Check( pDX, IDC_PEN_PER_ADDRESS, m_ReportMode.m_bPenPerAddress );
	DDX_Check( pDX, IDC_FULL_EML, m_ReportMode.m_bFullEmail );
	DDX_Check( pDX, IDC_FULL_PKT, m_ReportMode.m_bFullPacket );

	DDX_Check( pDX, IDC_SAVE_TIME, m_ReportMode.m_bSaveAge );
	DDX_Text( pDX, IDC_SAVE_TIME_COUNT, m_nTimeCount );
	DDX_CBIndex( pDX, IDC_SAVE_TIME_UNITS, m_nTimeUnits );

	DDX_Check( pDX, IDC_SAVE_SIZE, m_ReportMode.m_bSaveSize );
	DDX_Text( pDX, IDC_SAVE_SIZE_COUNT, m_nSizeCount );
	DDX_CBIndex( pDX, IDC_SAVE_SIZE_UNITS, m_nSizeUnits );

	if ( pDX->m_bSaveAndValidate ) {
		m_Settings.SetFileNamePrefix( m_strFileNamePrefix.GetString() );
		m_Settings.SetOutputDirectory( m_strOutputDirectory.GetString() );

		switch ( m_nTimeUnits ) {
		case kSave_Days:
			m_ReportMode.m_SaveAgeValue.Set( kDays, m_nTimeCount );
			break;
		case kSave_Hours:
			m_ReportMode.m_SaveAgeValue.Set( kHours, m_nTimeCount );
			break;
		default:	// kSave_Minutes
			m_ReportMode.m_SaveAgeValue.Set( kMinutes, m_nTimeCount );
			break;
		}

		switch ( m_nSizeUnits ) {
		case kMax_Gigabytes:
			m_ReportMode.m_nSaveSizeValue = m_nSizeCount * kBytesInGigabyte;
			break;
		case kMax_Megabytes:
			m_ReportMode.m_nSaveSizeValue = m_nSizeCount * kBytesInMegabyte;
			break;
		default:	// kMax_Kilobytes
			m_ReportMode.m_nSaveSizeValue = m_nSizeCount * kBytesInKilobyte;
			break;
		}

		if ( !m_ReportMode.m_bSaveAge && !m_ReportMode.m_bSaveSize ) {
			m_ReportMode.m_bSaveAge = TRUE;
			m_ReportMode.m_SaveAgeValue.Set( kSeconds, kSecondsInADay );
		}
	}
}


// ----------------------------------------------------------------------------
//		OnInitDialog
// ----------------------------------------------------------------------------

BOOL
CModePage::OnInitDialog()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CDialog::OnInitDialog();

	OnBnClickedMode();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


// ----------------------------------------------------------------------------
//		OnOK
// ----------------------------------------------------------------------------

void 
CModePage::OnOK()
{
}


// ----------------------------------------------------------------------------
//		OnCancel
// ----------------------------------------------------------------------------

void 
CModePage::OnCancel()
{
	CDialog*	pParent( static_cast<CDialog*>( GetParent() ) );
	if ( pParent ) {
		pParent->EndDialog( IDCANCEL );
	}
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CModePage, CDialog)
	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_PROTOCOL_SMTP, OnBnClickedSmtp )
	ON_BN_CLICKED(IDC_PROTOCOL_POP3, OnBnClickedPop3 )
	ON_BN_CLICKED(IDC_MODE_PEN, OnBnClickedMode)
	ON_BN_CLICKED(IDC_MODE_FULL, OnBnClickedMode)
	ON_BN_CLICKED(IDC_PEN_XML, OnBnClickedMode)
	ON_BN_CLICKED(IDC_PEN_TEXT, OnBnClickedMode)
	ON_BN_CLICKED(IDC_SAVE_TIME, OnBnClickedSaveAge)
	ON_BN_CLICKED(IDC_SAVE_SIZE, OnBnClickedSaveSize)
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnBnClickedBrowse
// ----------------------------------------------------------------------------

void
CModePage::OnBnClickedBrowse()
{
	LPMALLOC	pMalloc( NULL );
	if ( SUCCEEDED( ::SHGetMalloc( &pMalloc ) ) ) {

		BROWSEINFO	bInfo;
		::ZeroMemory( static_cast<PVOID>( &bInfo ), sizeof( BROWSEINFO ) );
		bInfo.hwndOwner = m_hWnd;
		bInfo.pidlRoot = NULL;
		bInfo.pszDisplayName = NULL;
		bInfo.lpszTitle = L"Choose Output Directory:";
		bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
		bInfo.lpfn = NULL;

		LPITEMIDLIST	pidlPath( ::SHBrowseForFolder( &bInfo ) );
		if ( pidlPath != NULL ) {
			CString	strTempLocation( m_strOutputDirectory );
			TCHAR*	pszTempLocation( strTempLocation.GetBufferSetLength( MAX_PATH ) );
			BOOL	bResult( ::SHGetPathFromIDList( pidlPath, pszTempLocation ) );
			strTempLocation.ReleaseBuffer();
			if ( bResult ) {
				m_strOutputDirectory = strTempLocation;
				SetDlgItemText( IDC_OUTPUT_DIRECTORY, m_strOutputDirectory );
			}
		}
		pMalloc->Free( pidlPath );
	}
}


// ----------------------------------------------------------------------------
//		OnBnClickedSmtp
// ----------------------------------------------------------------------------

void
CModePage::OnBnClickedSmtp()
{
	BOOL	bSmtp = (IsDlgButtonChecked( IDC_PROTOCOL_SMTP ) == BST_CHECKED) ? TRUE : FALSE;
	BOOL	bPop3 = (IsDlgButtonChecked( IDC_PROTOCOL_POP3 ) == BST_CHECKED) ? TRUE : FALSE;

	if ( !(bSmtp || bPop3) ) {
		CheckDlgButton( IDC_PROTOCOL_POP3, BST_CHECKED );
	}
}


// ----------------------------------------------------------------------------
//		OnBnClickedPop3
// ----------------------------------------------------------------------------

void
CModePage::OnBnClickedPop3()
{
	BOOL	bSmtp = (IsDlgButtonChecked( IDC_PROTOCOL_SMTP ) == BST_CHECKED) ? TRUE : FALSE;
	BOOL	bPop3 = (IsDlgButtonChecked( IDC_PROTOCOL_POP3 ) == BST_CHECKED) ? TRUE : FALSE;

	if ( !(bSmtp || bPop3) ) {
		CheckDlgButton( IDC_PROTOCOL_SMTP, BST_CHECKED );
	}
}


// ----------------------------------------------------------------------------
//		OnBnClickedMode
// ----------------------------------------------------------------------------

void
CModePage::OnBnClickedMode()
{
	BOOL	bPenMode = (IsDlgButtonChecked( IDC_MODE_PEN ) == BST_CHECKED) ? TRUE : FALSE;

	CWnd*	pPenEml = GetDlgItem( IDC_PEN_EML );
	if ( pPenEml ) pPenEml->EnableWindow( bPenMode );

	CWnd*	pPenXml = GetDlgItem( IDC_PEN_XML );
	if ( pPenXml ) pPenXml->EnableWindow( bPenMode );

	CWnd*	pPenText = GetDlgItem( IDC_PEN_TEXT );
	if ( pPenText ) pPenText->EnableWindow( bPenMode );

	BOOL	bSaveXml = (IsDlgButtonChecked( IDC_PEN_XML ) == BST_CHECKED) ? TRUE : FALSE;
	BOOL	bSaveText = (IsDlgButtonChecked( IDC_PEN_TEXT ) == BST_CHECKED) ? TRUE : FALSE;

	BOOL	bEnable = bPenMode && (bSaveXml || bSaveText);

	{
		UINT	aSaveTextItmes[] = {
			IDC_SAVE_GROUP,
			IDC_SAVE_TIME,
			IDC_SAVE_TIME_COUNT,
			IDC_SAVE_TIME_UNITS,
			IDC_SAVE_SIZE,
			IDC_SAVE_SIZE_COUNT,
			IDC_SAVE_SIZE_UNITS,
			IDC_PEN_PER_ADDRESS
		};

		for ( size_t i = 0; i < COUNTOF( aSaveTextItmes ); i++ ) {
			CWnd*	pWnd = GetDlgItem( aSaveTextItmes[i] );
			if ( pWnd ) pWnd->EnableWindow( bEnable );
		}

		OnBnClickedSaveAge();
		OnBnClickedSaveSize();
	}

	CWnd*	pFullEml = GetDlgItem( IDC_FULL_EML );
	if ( pFullEml ) pFullEml->EnableWindow( !bPenMode );

	CWnd*	pFullPkt = GetDlgItem( IDC_FULL_PKT );
	if ( pFullPkt ) pFullPkt->EnableWindow( !bPenMode );
}


// ----------------------------------------------------------------------------
//		OnBnClickedSaveAge
// ----------------------------------------------------------------------------

void
CModePage::OnBnClickedSaveAge()
{
	BOOL	bPenMode = (IsDlgButtonChecked( IDC_MODE_PEN ) == BST_CHECKED) ? TRUE : FALSE;
	BOOL	bSaveXml = (IsDlgButtonChecked( IDC_PEN_XML ) == BST_CHECKED) ? TRUE : FALSE;
	BOOL	bSaveText = (IsDlgButtonChecked( IDC_PEN_TEXT ) == BST_CHECKED) ? TRUE : FALSE;
	BOOL	bSaveAge = (IsDlgButtonChecked( IDC_SAVE_TIME ) == BST_CHECKED) ? TRUE : FALSE;

	BOOL	bEnable = bPenMode && (bSaveXml || bSaveText) && bSaveAge;

	CWnd*	pCount = GetDlgItem( IDC_SAVE_TIME_COUNT );
	if ( pCount ) pCount->EnableWindow( bEnable );

	CWnd*	pUnits = GetDlgItem( IDC_SAVE_TIME_UNITS );
	if ( pUnits ) pUnits->EnableWindow( bEnable );
}


// ----------------------------------------------------------------------------
//		OnBnClickedSaveSize
// ----------------------------------------------------------------------------

void
CModePage::OnBnClickedSaveSize()
{
	BOOL	bPenMode = (IsDlgButtonChecked( IDC_MODE_PEN ) == BST_CHECKED) ? TRUE : FALSE;
	BOOL	bSaveXml = (IsDlgButtonChecked( IDC_PEN_XML ) == BST_CHECKED) ? TRUE : FALSE;
	BOOL	bSaveText = (IsDlgButtonChecked( IDC_PEN_TEXT ) == BST_CHECKED) ? TRUE : FALSE;
	BOOL	bSaveSize = (IsDlgButtonChecked( IDC_SAVE_SIZE ) == BST_CHECKED) ? TRUE : FALSE;

	BOOL	bEnable = bPenMode && (bSaveXml || bSaveText) && bSaveSize;

	CWnd*	pCount = GetDlgItem( IDC_SAVE_SIZE_COUNT );
	if ( pCount ) pCount->EnableWindow( bEnable );

	CWnd*	pUnits = GetDlgItem( IDC_SAVE_SIZE_UNITS );
	if ( pUnits ) pUnits->EnableWindow( bEnable );
}

#endif // HE_WIN32
