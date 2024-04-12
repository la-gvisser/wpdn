// =============================================================================
//	TriggerOptions.cpp
//		implementation of the CCaseOptionsSheet class.
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.


#include "stdafx.h"
#include "TriggerOptions.h"
#include "afxdtctl.h"


////////////////////////////////////////////////////////////////////////////////
//		CTriggerOptionsDlg
////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CTriggerOptionsDlg, CDialog)

// -----------------------------------------------------------------------------
//		Construction
// -----------------------------------------------------------------------------

CTriggerOptionsDlg::CTriggerOptionsDlg(	CCaseOptions::CTriggerOptions& inOptions, bool bIsDockedTab )
	: CDialog( CTriggerOptionsDlg::IDD )
	, m_Options( inOptions )
	, m_bIsDockedTab( bIsDockedTab )
{
}

// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
CTriggerOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if (pDX->m_bSaveAndValidate == FALSE) return;

	// Get the first date
	CDateTimeCtrl* pDate1 = (CDateTimeCtrl*)GetDlgItem( IDC_DATE1 );
	SYSTEMTIME theDate1;
	pDate1->GetTime(&theDate1);

	// Get the first time
	CDateTimeCtrl* pTime1 = (CDateTimeCtrl*)GetDlgItem( IDC_TIME1 );
	SYSTEMTIME theTime1;
	pTime1->GetTime(&theTime1);

	SYSTEMTIME	theDateTime1;
	memset( &theDateTime1, 0, sizeof(theDateTime1) );
	theDateTime1.wYear		= theDate1.wYear;
	theDateTime1.wMonth		= theDate1.wMonth;
	theDateTime1.wDay		= theDate1.wDay;
	theDateTime1.wHour		= theTime1.wHour;
	theDateTime1.wMinute	= theTime1.wMinute;
	theDateTime1.wSecond	= theTime1.wSecond;

	// Convert to local file time.
	UInt64	nFileTime1;
	::SystemTimeToFileTime( &theDateTime1, (FILETIME*) &nFileTime1 );

	// Convert to UTC file time.
	UInt64 nUtcFileTime1;
	::LocalFileTimeToFileTime( (FILETIME*) &nFileTime1, (FILETIME *) &nUtcFileTime1 );

	m_Options.SetStartTime( nUtcFileTime1 );


	// Get the second date
	CDateTimeCtrl* pDate2 = (CDateTimeCtrl*)GetDlgItem( IDC_DATE2 );
	SYSTEMTIME theDate2;
	pDate2->GetTime(&theDate2);

	// Get the second time
	CDateTimeCtrl* pTime2 = (CDateTimeCtrl*)GetDlgItem( IDC_TIME2 );
	SYSTEMTIME theTime2;
	pTime2->GetTime(&theTime2);

	SYSTEMTIME	theDateTime2;
	memset( &theDateTime2, 0, sizeof(theDateTime2) );
	theDateTime2.wYear		= theDate2.wYear;
	theDateTime2.wMonth		= theDate2.wMonth;
	theDateTime2.wDay		= theDate2.wDay;
	theDateTime2.wHour		= theTime2.wHour;
	theDateTime2.wMinute	= theTime2.wMinute;
	theDateTime2.wSecond	= theTime2.wSecond;

	// Convert to local file time.
	UInt64	nFileTime2;
	::SystemTimeToFileTime( &theDateTime2, (FILETIME*) &nFileTime2 );

	// Convert to UTC file time.
	UInt64 nUtcFileTime2;
	::LocalFileTimeToFileTime( (FILETIME*) &nFileTime2, (FILETIME *) &nUtcFileTime2 );

	m_Options.SetStopTime( nUtcFileTime2 );
}

// -----------------------------------------------------------------------------
//		Message Map
// -----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTriggerOptionsDlg, CDialog)
END_MESSAGE_MAP()

// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CTriggerOptionsDlg::OnInitDialog()
{
	BOOL bResult = TRUE;

	if ( m_Options.GetStartTime() > 0 )
	{
		// Set first date and time controls
		UInt64 nUtcFileTime1 = m_Options.GetStartTime();

		// Convert from UTC file time.
		UInt64	nFileTime1;
		::FileTimeToLocalFileTime( (FILETIME *) &nUtcFileTime1, (FILETIME*) &nFileTime1 );

		// Convert to local file time.
		SYSTEMTIME theDateTime1;
		::FileTimeToSystemTime( (FILETIME*) &nFileTime1, &theDateTime1 );

		// Get the first date
		CDateTimeCtrl* pDate1 = (CDateTimeCtrl*)GetDlgItem( IDC_DATE1 );
		bResult = pDate1->SetTime(&theDateTime1);
		ASSERT(bResult);

		// Get the first time
		CDateTimeCtrl* pTime1 = (CDateTimeCtrl*)GetDlgItem( IDC_TIME1 );
		bResult = pTime1->SetTime(&theDateTime1);
		ASSERT(bResult);
	}

	if ( m_Options.GetStopTime() > 0 )
	{
		// Set second date and time controls
		UInt64 nUtcFileTime2 = m_Options.GetStopTime();

		// Convert from UTC file time.
		UInt64	nFileTime2;
		::FileTimeToLocalFileTime( (FILETIME *) &nUtcFileTime2, (FILETIME*) &nFileTime2 );

		// Convert to local file time.
		SYSTEMTIME theDateTime2;
		::FileTimeToSystemTime( (FILETIME*) &nFileTime2, &theDateTime2 );

		// Get the first date
		CDateTimeCtrl* pDate2 = (CDateTimeCtrl*)GetDlgItem( IDC_DATE2 );
		bResult = pDate2->SetTime(&theDateTime2);
		ASSERT(bResult);

		// Get the first time
		CDateTimeCtrl* pTime2 = (CDateTimeCtrl*)GetDlgItem( IDC_TIME2 );
		bResult = pTime2->SetTime(&theDateTime2);
		ASSERT(bResult);
	}

	SetControlsInitialState();

	CDialog::OnInitDialog();

	return TRUE;  
}

// -----------------------------------------------------------------------------
//		SetControlsInitialState
// -----------------------------------------------------------------------------

void
CTriggerOptionsDlg::SetControlsInitialState()
{
	if ( !m_bIsDockedTab ) return;

	CWnd* pWnd;

	pWnd = GetDlgItem( IDC_DATE1 );
	pWnd->EnableWindow( FALSE );
	pWnd = GetDlgItem( IDC_TIME1 );
	pWnd->EnableWindow( FALSE );
	pWnd = GetDlgItem( IDC_DATE2 );
	pWnd->EnableWindow( FALSE );
	pWnd = GetDlgItem( IDC_TIME2 );
	pWnd->EnableWindow( FALSE );
}
