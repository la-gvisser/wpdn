// ============================================================================
//	TriggerPage.cpp
//		implementation of the CTriggerPage class.
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef HE_WIN32

#include "TriggerPage.h"
#include <afxwin.h>


// ============================================================================
//		CTriggerPage
// ============================================================================

IMPLEMENT_DYNAMIC( CTriggerPage, CDialog )

CTriggerPage::CTriggerPage(
	CCase&	inCase )
	:	CDialog()
	,	m_Settings( inCase.GetSettings() )
{
}


// ----------------------------------------------------------------------------
//		GetStartTime
// ----------------------------------------------------------------------------

CPeekTime
CTriggerPage::GetStartTime()
{
	// Get the start date.
	CDateTimeCtrl*	pStartDate( static_cast<CDateTimeCtrl*>( GetDlgItem( IDC_START_DATE ) ) );
	SYSTEMTIME		stStartDate;
	pStartDate->GetTime( &stStartDate );

	// Get the start time.
	CDateTimeCtrl*	pStartTime( static_cast<CDateTimeCtrl*>( GetDlgItem( IDC_START_TIME ) ) );
	SYSTEMTIME		stStartTime;
	pStartTime->GetTime( &stStartTime );

	CPeekTimeLocal	ptlStartTime( stStartDate, stStartTime );
	ptlStartTime.Round( kSeconds );

	CPeekTime	ptStartTime( ptlStartTime );

#if _DEBUG
	CPeekString	strStart( ptStartTime.Format() );
	CPeekString	strLocal( ptlStartTime.Format() );
#endif

	return ptStartTime;
}

void
CTriggerPage::GetStartTime(
	SYSTEMTIME&	outDateTime )
{
	// Get the start date.
	CDateTimeCtrl*	pStartDate( static_cast<CDateTimeCtrl*>( GetDlgItem( IDC_START_DATE ) ) );
	SYSTEMTIME		stStartDate;
	pStartDate->GetTime( &stStartDate );

	// Get the start time.
	CDateTimeCtrl*	pStartTime( static_cast<CDateTimeCtrl*>( GetDlgItem( IDC_START_TIME ) ) );
	SYSTEMTIME		stStartTime;
	pStartTime->GetTime( &stStartTime );

	memset( &outDateTime, 0, sizeof( outDateTime ) );
	outDateTime.wYear	= stStartDate.wYear;
	outDateTime.wMonth	= stStartDate.wMonth;
	outDateTime.wDay	= stStartDate.wDay;
	outDateTime.wHour	= stStartTime.wHour;
	outDateTime.wMinute	= stStartTime.wMinute;
	outDateTime.wSecond	= stStartTime.wSecond;
}


// ----------------------------------------------------------------------------
//		GetStopTime
// ----------------------------------------------------------------------------

CPeekTime
CTriggerPage::GetStopTime()
{
	// Get the stop date.
	CDateTimeCtrl*	pStopDate( static_cast<CDateTimeCtrl*>( GetDlgItem( IDC_STOP_DATE ) ) );
	SYSTEMTIME		stStopDate;
	pStopDate->GetTime( &stStopDate );

	// Get the stop time.
	CDateTimeCtrl*	pStopTime( static_cast<CDateTimeCtrl*>( GetDlgItem( IDC_STOP_TIME ) ) );
	SYSTEMTIME		stStopTime;
	pStopTime->GetTime( &stStopTime );

	CPeekTimeLocal	ptlStopTime( stStopDate, stStopTime );
	ptlStopTime.Round( kSeconds );

	CPeekTime	ptStopTime( ptlStopTime );

#if _DEBUG
	CPeekString	strStop( ptStopTime.Format() );
	CPeekString	strLocal( ptlStopTime.Format() );
#endif

	return ptStopTime;
}


// ----------------------------------------------------------------------------
//		DoDataExchange
// ----------------------------------------------------------------------------

void
CTriggerPage::DoDataExchange(
	CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );

	if ( pDX->m_bSaveAndValidate == FALSE ) return;

	// Save and Validate
	m_Settings.SetStartTime( GetStartTime() );
	m_Settings.SetStopTime( GetStopTime() );
}


// ----------------------------------------------------------------------------
//		OnInitDialog
// ----------------------------------------------------------------------------

BOOL
CTriggerPage::OnInitDialog()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// Load
	BOOL	bResult;

	// Controls display current time/date by default.
	{
		SYSTEMTIME		stStartTime;
		m_Settings.GetStartTime().GetLocalTime( stStartTime );

		// Get the start date
		CDateTimeCtrl*	pDate( static_cast<CDateTimeCtrl*>( GetDlgItem( IDC_START_DATE ) ) );
		bResult = pDate->SetTime( &stStartTime );
		ASSERT( bResult );

		// Get the start time
		CDateTimeCtrl*	pTime( static_cast<CDateTimeCtrl*>( GetDlgItem( IDC_START_TIME ) ) );
		bResult = pTime->SetTime( &stStartTime );
		ASSERT( bResult );
	}
	{
		SYSTEMTIME		stStopTime;
		m_Settings.GetStopTime().GetLocalTime( stStopTime );

		// Get the stop date
		CDateTimeCtrl*	pDate( static_cast<CDateTimeCtrl*>( GetDlgItem( IDC_STOP_DATE ) ) );
		bResult = pDate->SetTime( &stStopTime );
		ASSERT( bResult );

		// Get the stop time
		CDateTimeCtrl*	pTime( static_cast<CDateTimeCtrl*>( GetDlgItem( IDC_STOP_TIME ) ) );
		bResult = pTime->SetTime( &stStopTime );
		ASSERT( bResult );
	}

#ifdef _DEBUG
	int	nIds[] = {
		IDC_TIME_NOW,
		IDC_TIME_ONE_DAY,
		IDC_TIME_ONE_WEEK,
		IDC_TIME_ONE_MONTH
	};
	const int	nIdCount( COUNTOF( nIds ) );

	for ( int i = 0; i < nIdCount; i++ ) {
		CWnd*	pCtrl( GetDlgItem( nIds[i] ) );
		if ( pCtrl != NULL ) {
			pCtrl->ShowWindow( SW_SHOW );
		}
	}
#else
	CWnd*	pStart( GetDlgItem( IDC_START_GROUP ) );
	CWnd*	pEnd( GetDlgItem( IDC_END_GROUP ) );
	if ( (pStart != NULL) && (pEnd != NULL) ) {
		CRect	rcStart;
		pStart->GetWindowRect( &rcStart );

		CRect	rcEnd;
		pEnd->GetWindowRect( &rcEnd );
		rcEnd.bottom = rcEnd.top + rcStart.Height();
		ScreenToClient( &rcEnd );
		pEnd->MoveWindow( &rcEnd );
	}
#endif

	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


// ----------------------------------------------------------------------------
//		OnOK
// ----------------------------------------------------------------------------

void 
CTriggerPage::OnOK()
{
}


// ----------------------------------------------------------------------------
//		OnCancel
// ----------------------------------------------------------------------------

void 
CTriggerPage::OnCancel()
{
}


// ----------------------------------------------------------------------------
//		EndDialog
// ----------------------------------------------------------------------------

void 
CTriggerPage::EndDialog(
	int	nEndID )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// Save and Validate
	m_Settings.SetStartTime( GetStartTime() );
	m_Settings.SetStopTime( GetStopTime() );

	CDialog::EndDialog( nEndID );
}


// ----------------------------------------------------------------------------
//		OnOneUnit
// ----------------------------------------------------------------------------

void 
CTriggerPage::OnOneUnit(
	tOneUnit	inUnit )
{
	BOOL		bResult( FALSE );
	CTime		StopTime;
	SYSTEMTIME	stStartTime;
	GetStartTime( stStartTime );

	switch ( inUnit ) {
	case kOne_Day:
		StopTime = stStartTime;
		StopTime += CTimeSpan( 1, 0, 0, 0 );	// days, hours, minutes, seconds
		break;

	case kOne_Week:
		StopTime = stStartTime;
		StopTime += CTimeSpan( 7, 0, 0, 0 );
		break;

	case kOne_Month:
		{
			WORD	nMonth( (stStartTime.wMonth % 12) + 1 );
			if ( nMonth < stStartTime.wMonth ) {
				stStartTime.wYear++;
			}
			stStartTime.wMonth = nMonth;
			StopTime = stStartTime;
		}
		break;
	}

	// Get the stop date
	CDateTimeCtrl*	pDate( static_cast<CDateTimeCtrl*>( GetDlgItem( IDC_STOP_DATE ) ) );
	bResult = pDate->SetTime( &StopTime );
	ASSERT( bResult );

	// Get the stop time
	CDateTimeCtrl*	pTime( static_cast<CDateTimeCtrl*>( GetDlgItem( IDC_STOP_TIME ) ) );
	bResult = pTime->SetTime( &StopTime );
	ASSERT( bResult );
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTriggerPage, CDialog)
	ON_BN_CLICKED(IDC_TIME_NOW, OnBnClickedTimeNow)
	ON_BN_CLICKED(IDC_TIME_ONE_DAY, OnBnClickedTimeOneDay)
	ON_BN_CLICKED(IDC_TIME_ONE_WEEK, OnBnClickedTimeOneWeek)
	ON_BN_CLICKED(IDC_TIME_ONE_MONTH, OnBnClickedTimeOneMonth)
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnBnClickedTimeNow
// ----------------------------------------------------------------------------

void
CTriggerPage::OnBnClickedTimeNow()
{
	BOOL		bResult( FALSE );
	CPeekTime	ptNow( true );
	SYSTEMTIME	StartTime = ptNow;

	// Get the start date
	CDateTimeCtrl*	pDate( static_cast<CDateTimeCtrl*>( GetDlgItem( IDC_START_DATE ) ) );
	bResult = pDate->SetTime( &StartTime );
	ASSERT( bResult );

	// Get the start time
	CDateTimeCtrl*	pTime( static_cast<CDateTimeCtrl*>( GetDlgItem( IDC_START_TIME ) ) );
	bResult = pTime->SetTime( &StartTime );
	ASSERT( bResult );
}


// ----------------------------------------------------------------------------
//		OnBnClickedTimeOneDay
// ----------------------------------------------------------------------------

void
CTriggerPage::OnBnClickedTimeOneDay()
{
	OnOneUnit( kOne_Day );
}


// ----------------------------------------------------------------------------
//		OnBnClickedTimeOneWeek
// ----------------------------------------------------------------------------

void
CTriggerPage::OnBnClickedTimeOneWeek()
{
	OnOneUnit( kOne_Week );
}


// ----------------------------------------------------------------------------
//		OnBnClickedTimeOneMonth
// ----------------------------------------------------------------------------

void
CTriggerPage::OnBnClickedTimeOneMonth()
{
	OnOneUnit( kOne_Month );
}

#endif // HE_WIN32
