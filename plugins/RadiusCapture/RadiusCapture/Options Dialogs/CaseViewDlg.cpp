// =============================================================================
//	CaseViewDlg.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef IMPLEMENT_TAB

#include "CaseViewDlg.h"
#include "afxcmn.h"

////////////////////////////////////////////////////////////////////////////////
//		CCaseViewDlg
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//		Construction
// -----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CCaseViewDlg, CDialog)

CCaseViewDlg::CCaseViewDlg(
	CCaseOptions* inCaseOptions, 
	COptions* inOptions, 
	bool bIsCapturing,
	int nIdx,  
	CWnd* pParent /*=NULL*/)
	: m_pCaseOptions( inCaseOptions )
	, CDialog(CCaseViewDlg::IDD, pParent)
	, m_CaseOptionsList( inOptions->GetCaseOptionsList() )
	, m_CaseOptionsListIdx( nIdx )
	, m_bIsCapturing( bIsCapturing )
	, m_pOptions( inOptions )
	, m_strGlobalOutputDirectory( inOptions->GetOutputRootDirectory() )
	, m_bInitializing( false )
{
}


// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
CCaseViewDlg::DoDataExchange(
	CDataExchange* pDX )
{
	if ( pDX->m_bSaveAndValidate ) return;

	// Set the case title
	CString	strCaseTitle;
	strCaseTitle.Format( L"%s Options", m_pCaseOptions->GetName().Format() );
	DDX_Text( pDX, IDC_VIEW_CASE_NAME, strCaseTitle );

	CString	thePrefix( m_pCaseOptions->GetPrefix() );
	CString theOutputDir( m_pCaseOptions->GetOutputDirectory() );

	DDX_Text( pDX, IDC_VIEW_PREFIX, thePrefix );
	DDX_Text( pDX, IDC_VIEW_OUTPUT_DIR, theOutputDir );

	{
		CString theStartTime;
		SYSTEMTIME theSystemTime;
		UInt64 theFileTime = m_pCaseOptions->GetStartTime();
		UInt64 theLocalFileTime;
		::FileTimeToLocalFileTime( (const FILETIME*)&theFileTime, (FILETIME*)&theLocalFileTime );
		::FileTimeToSystemTime( (const FILETIME*)&theLocalFileTime, &theSystemTime );
		theStartTime = FormatTimeLocal( &theSystemTime );
		DDX_Text( pDX, IDC_VIEW_START_TIME, theStartTime );
	}

	{
		CString theStopTime;
		SYSTEMTIME theSystemTime;
		UInt64 theFileTime = m_pCaseOptions->GetStopTime();
		UInt64 theLocalFileTime;
		::FileTimeToLocalFileTime( (const FILETIME*)&theFileTime, (FILETIME*)&theLocalFileTime );
		::FileTimeToSystemTime( (const FILETIME*)&theLocalFileTime, &theSystemTime );
		theStopTime = FormatTimeLocal( &theSystemTime );		
		DDX_Text( pDX, IDC_VIEW_STOP_TIME, theStopTime );
	}

	const bool	bCaptureToScreen( m_pCaseOptions->IsCaptureToScreen() );
	const bool	bCaptureToFile( m_pCaseOptions->IsCaptureToFile() );
	ASSERT( bCaptureToScreen || bCaptureToFile );

	const bool	bLogMsgsToScreen( m_pCaseOptions->IsLogMsgsToScreen() );
	const bool	bLogMsgsToFile( m_pCaseOptions->IsLogMsgsToFile() );

	CString		strCaptureMsg;
	if ( bCaptureToScreen && bCaptureToFile )	strCaptureMsg = L"Both to screen packets window and to disk file.";
	else if ( bCaptureToScreen )				strCaptureMsg = L"To screen packets window only.";
	else										strCaptureMsg = L"To disk file only.";
	DDX_Text( pDX, IDC_VIEW_CAPTURED_DEST, strCaptureMsg );

	CString		strLogMsg;
	CString		strLogToMsg;
	if ( bLogMsgsToScreen || bLogMsgsToFile ) {
		strLogMsg = L" CAPTURE EVENTS WILL BE LOGGED:";

		if ( bLogMsgsToFile ) {
			if ( bLogMsgsToScreen )		strLogToMsg = L"Both to screen log window and to disk file.";
			else						strLogToMsg = L"To disk file only.";
		}
		else if ( bLogMsgsToScreen ) {
			strLogToMsg = L"To screen log window only.";
		}
	}

	DDX_Text( pDX, IDC_VIEW_LOGGING_MSG, strLogMsg );
	DDX_Text( pDX, IDC_VIEW_LOG_DEST, strLogToMsg );

	CString		strAgeSizeMessage1;
	CString		strAgeSizeMessage2;
	CString		strAgeMessage;
	CString		strSizeMessage;
	CString		strNewFileCaptionMsg;

	CCaptureOptions& theCaptureOptions( m_pCaseOptions->GetCaptureOptions() );
	const bool bIsSaveAge = theCaptureOptions.IsSaveAgeSelected();
	const bool bIsSaveSize = theCaptureOptions.IsSaveSizeSelected();

	if ( bIsSaveAge ) {
		UInt32		nAgeCount;
		CString		strAgeUnits;

		const UInt32 nSaveAgeValue = m_pCaseOptions->GetSaveAgeSeconds();
		if ( (nSaveAgeValue % kSecondsInADay) == 0 ) {
			nAgeCount = nSaveAgeValue / kSecondsInADay;
			strAgeUnits = L"day";
		}
		else if ( (nSaveAgeValue % kSecondsInAnHour) == 0 ) {
			nAgeCount = nSaveAgeValue / kSecondsInAnHour;
			strAgeUnits = L"hour";
		}
		else {
			ASSERT( (nSaveAgeValue % kSecondsInAMinute) == 0 );
			nAgeCount = nSaveAgeValue / kSecondsInAMinute;
			strAgeUnits = L"minute";
		}

		strAgeMessage.Format( L"%u %s%s has elapsed since the last refresh.", nAgeCount, strAgeUnits, nAgeCount == 1 ? L"" : L"s" );
	}

	if ( bIsSaveSize ) {
		UInt64		nSizeCount;
		CString		strSizeUnits;

		const UInt64 nSaveSizeValue = m_pCaseOptions->GetSaveSizeBytes();
		if ( (nSaveSizeValue % kGigabyte) == 0 ) {
			nSizeCount = nSaveSizeValue / kGigabyte;
			strSizeUnits = L"gigabyte";
		}
		else if ( (nSaveSizeValue % kMegabyte) == 0 ) {
			nSizeCount = nSaveSizeValue / kMegabyte;
			strSizeUnits = L"megabyte";
		}
		else {
			nSizeCount = nSaveSizeValue / kKilobyte;
			strSizeUnits = L"kilobyte";
		}

		strSizeMessage.Format( L"File size has exceeded %llu %s%s.", nSizeCount, strSizeUnits, nSizeCount == 1 ? L"" : L"s" );
	}

	if ( bIsSaveAge && bIsSaveSize ) {
		strNewFileCaptionMsg = L"Capture file will be refreshed when the first of the following occurs:";
		strAgeSizeMessage1 = strAgeMessage;
		strAgeSizeMessage2 = strSizeMessage;
	}
	else if ( bIsSaveAge ) {
		strNewFileCaptionMsg = L"Capture file will be refreshed when the following occurs:";
		strAgeSizeMessage1 = strAgeMessage;
	}
	else if ( bIsSaveSize ) {
		strNewFileCaptionMsg = L"Capture file will be refreshed when the following occurs:";
		strAgeSizeMessage1 = strSizeMessage;
	}

	DDX_Text( pDX, IDC_NEW_FILE_CAPTION, strNewFileCaptionMsg );
	DDX_Text( pDX, IDC_VIEW_AGE_SIZE_MSG_1, strAgeSizeMessage1 );
	DDX_Text( pDX, IDC_VIEW_AGE_SIZE_MSG_2, strAgeSizeMessage2 );

	CDialog::DoDataExchange(pDX);
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CCaseViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Create a 12-point Arial font
	m_Font.CreatePointFont(120, _T("Arial"));

	// Assign it to the Case Title control
	GetDlgItem(IDC_VIEW_CASE_NAME)->SetFont(&m_Font);

	// Display the ports
	PortListToControl();

	// Display the enabled users for this case
	UserListToControl();

	return TRUE;
}


// -----------------------------------------------------------------------------
//		PortListToControl
// -----------------------------------------------------------------------------

void
CCaseViewDlg::PortListToControl()
{
	wchar_t	szPort[20];

	CPortNumberList& thePortNumberList = m_pCaseOptions->GetPortNumberList();

	CListCtrl* pPortList = (CListCtrl*)GetDlgItem( IDC_PORT_LIST );		
	ASSERT( pPortList );
	if ( pPortList == NULL ) return;

	pPortList->DeleteAllItems();

	int nCount = 0;

	const int nNumPortOptions = static_cast<int>( thePortNumberList.GetCount() );
	for ( int i = 0; i < nNumPortOptions; i++ ) {
		CPortNumber* thePortNumber = thePortNumberList.GetAt( i );
		if ( thePortNumber->IsEnabled() ) {
			wsprintf( szPort, _T("%d"), thePortNumber->GetPort() );
			pPortList->InsertItem( nCount, static_cast<LPCTSTR>(szPort) );
			nCount++;
		}
	}
}


// -----------------------------------------------------------------------------
//		UserListToControl
// -----------------------------------------------------------------------------

void
CCaseViewDlg::UserListToControl()
{
	CListCtrl* pUserNameList = (CListCtrl*)GetDlgItem( IDC_VIEW_USERNAME_LIST );
	if ( pUserNameList == NULL ) return;

	pUserNameList->DeleteAllItems();

	CUserItemArray	aUsers = m_pCaseOptions->GetUserList();
	int	nCount = static_cast<int>( aUsers.GetCount() );
	for ( int i = 0; i < nCount; i++ ) {
		if ( aUsers[i].IsEnabled() ) {
			BOOL	bCheck;
			CPeekString	strName;

			aUsers[i].Get( &bCheck, strName );
			pUserNameList->InsertItem( i, strName );
			pUserNameList->SetCheck( i, bCheck );
		}
	}
}

#endif // IMPLEMENT_TAB
