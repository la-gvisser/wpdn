// =============================================================================
//	PeekTime.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.
//
// Peek Time is nanoseconds since 1/1/1601. 64 bits, unsigned.
// Overflows in the year 2185 AD.
//

#include "StdAfx.h"
#include "PeekTime.h"
#include <string.h>


// =============================================================================
//		Globals
// =============================================================================

CPeekTime	g_pkLastTime( true );
CTimeUnits	g_TimeUnits;


// =============================================================================
//		CTimeUnits
// =============================================================================

CPeekArray<tTimeUnitsItem>	CTimeUnits::s_aTimeUnits;
int							CTimeUnits::s_nCount = g_TimeUnits.Init();


// ----------------------------------------------------------------------------
//		Init
// ----------------------------------------------------------------------------

int
CTimeUnits::Init()
{
	tTimeUnitsItem tuiNanoSeconds = { kNanoseconds, 1, CPeekString( L"Nanoseconds" ) };
	CTimeUnits::s_aTimeUnits.Add( tuiNanoSeconds );

	tTimeUnitsItem tuiMilliSeconds = { kMilliseconds, WPTIME_MILLISECONDS, CPeekString( L"Milliseconds" ) };
	CTimeUnits::s_aTimeUnits.Add( tuiMilliSeconds );

	tTimeUnitsItem tuiMicroSeconds = { kMicroseconds, WPTIME_MICROSECONDS, CPeekString( L"Microseconds" ) };
	CTimeUnits::s_aTimeUnits.Add( tuiMicroSeconds );

	tTimeUnitsItem tuiSeconds = { kSeconds, WPTIME_SECONDS, CPeekString( L"Seconds" ) };
	CTimeUnits::s_aTimeUnits.Add( tuiSeconds );

	tTimeUnitsItem tuiMinutess = { kMinutes, WPTIME_MINUTES, CPeekString( L"Minutes" ) };
	CTimeUnits::s_aTimeUnits.Add( tuiMinutess );

	tTimeUnitsItem tuiHours = { kHours, WPTIME_HOURS, CPeekString( L"Hours" ) };
	CTimeUnits::s_aTimeUnits.Add( tuiHours );

	tTimeUnitsItem tuiDays = { kDays, WPTIME_DAYS, CPeekString( L"Days" ) };
	CTimeUnits::s_aTimeUnits.Add( tuiDays );

	return static_cast<int>( s_aTimeUnits.GetCount() );
}


// ----------------------------------------------------------------------------
//		GetConversion
// ----------------------------------------------------------------------------

UInt64
CTimeUnits::GetConversion(
	tTimeUnits	inType )
{
	return (inType < s_nCount)
		? s_aTimeUnits[inType].fConversion
		: s_aTimeUnits[0].fConversion;
}

UInt64
CTimeUnits::GetConversion(
	const CPeekString&	inLabel )
{
	for ( int i = (s_nCount - 1); i > 0; --i ) {
		if ( inLabel.CompareNoCase( s_aTimeUnits[i].fLabel ) == 0 ) {
			return s_aTimeUnits[i].fConversion;
		}
	}

	return s_aTimeUnits[0].fConversion;
}


// ----------------------------------------------------------------------------
//		GetLabel
// ----------------------------------------------------------------------------

const wchar_t*
CTimeUnits::GetLabel(
	tTimeUnits	inType )
{
	return (inType < s_nCount)
		? s_aTimeUnits[inType].fLabel
		: s_aTimeUnits[0].fLabel;
}


// ----------------------------------------------------------------------------
//		GetUnits
// ----------------------------------------------------------------------------

tTimeUnits
CTimeUnits::GetUnits(
	const CPeekString&	inLabel )
{
	for ( int i = (s_nCount - 1); i > 0; --i ) {
		if ( inLabel.CompareNoCase( s_aTimeUnits[i].fLabel ) == 0 ) {
			return s_aTimeUnits[i].fUnits;
		}
	}

	return s_aTimeUnits[0].fUnits;
}

tTimeUnits
CTimeUnits::GetUnits(
	UInt64	inDuration )
{
	for ( int i = (s_nCount - 1); i > 0; --i ) {
		if ( (inDuration % s_aTimeUnits[i].fConversion) == 0 ) {
			return s_aTimeUnits[i].fUnits;
		}
	}

	return s_aTimeUnits[0].fUnits;
}


// ----------------------------------------------------------------------------
//		Round
// ----------------------------------------------------------------------------

UInt64
CTimeUnits::Round(
	UInt64		inDuration,
	tTimeUnits	inType )
{
	UInt64	nConversion = CTimeUnits::GetUnits( inType );
	UInt64	nRounded = (inDuration + (nConversion / 2)) / nConversion;

	return nRounded;
}


// =============================================================================
//		CPeekTime
// =============================================================================

CPeekTime::CPeekTime(
	const CPeekTimeLocal& inTime )
{
	FILETIME	ftLocal = inTime;
	::LocalFileTimeToFileTime( &ftLocal, &m_ltTime.ft );
}


// ----------------------------------------------------------------------------
//		Format
// ----------------------------------------------------------------------------

CPeekString
CPeekTime::Format(
	const SYSTEMTIME&	inTime )
{
	// 25 characters: 'YYYY-MM-DDTHH.MM.SS.mmmZ'
	// As close to ISO 8601 as possible periods instead of colons
	CPeekOutString	strTime;
	strTime << std::setfill( L'0' );
	strTime << std::setw( 4 ) << inTime.wYear << L"-"
			<< std::setw( 2 ) << inTime.wMonth << L"-"
			<< std::setw( 2 ) << inTime.wDay << L"T"
			<< std::setw( 2 ) << inTime.wHour << L"."
			<< std::setw( 2 ) << inTime.wMinute << L"."
			<< std::setw( 2 ) << inTime.wSecond << L"."
			<< std::setw( 3 ) << inTime.wMilliseconds << L"Z";

	return strTime;
}


// ----------------------------------------------------------------------------
//		FormatDate
// ----------------------------------------------------------------------------

CPeekString
CPeekTime::FormatDate(
	const SYSTEMTIME&	inTime )
{
	// 10 characters: 'YYYY-MM-DD'
	CPeekOutString	strDate;
	strDate << std::setfill( L'0' );
	strDate << std::setw( 4 ) << inTime.wYear << L"-"
			<< std::setw( 2 ) << inTime.wMonth << L"-"
			<< std::setw( 2 ) << inTime.wDay;

	return strDate;
}


// ----------------------------------------------------------------------------
//		FormatTime
// ----------------------------------------------------------------------------

CPeekString
CPeekTime::FormatTime(
	const SYSTEMTIME&	inTime )
{
	// 13 characters: 'HH.MM.SS.mmmZ'
	CPeekOutString	strTime;
	strTime << std::setfill( L'0' );
	strTime << std::setw( 2 ) << inTime.wHour << L"."
			<< std::setw( 2 ) << inTime.wMinute << L"."
			<< std::setw( 2 ) << inTime.wSecond << L"."
			<< std::setw( 3 ) << inTime.wMilliseconds << L"Z";

	return strTime;
}


// ----------------------------------------------------------------------------
//		GetRandomizedMilliSec
// ----------------------------------------------------------------------------

UInt16
CPeekTime::GetRandomizedMilliSec(
	bool	inSeedRandomizer ) throw( ... )
{
	static UInt64	uSeed = 0;

	if ( inSeedRandomizer ) {
		uSeed = GetTickCount();
	}
	srand( static_cast<UInt32>( GetTickCount() + uSeed ) );
	int	nRandNumber;
	while( (nRandNumber = rand()) > 999 );

	return static_cast<UInt16>( nRandNumber );
}


// ----------------------------------------------------------------------------
//		GetTimeStringFileName
// ----------------------------------------------------------------------------

CPeekString
CPeekTime::GetTimeStringFileName(
	bool	inRandomMilli ) throw( ... )
{
	CPeekTime	pkTime( true );
	int			x = 0;

	while ( pkTime == g_pkLastTime ) {
		Sleep( 1 );
		pkTime.SetNow();
		if ( x++ > 1000 ) throw 0;
	}
	g_pkLastTime = pkTime;

	SYSTEMTIME	stTime = pkTime;
	if ( inRandomMilli ) {
		stTime.wMilliseconds = GetRandomizedMilliSec( true );
	}

	return Format( stTime );
}


// ----------------------------------------------------------------------------
//		GetLocalTimeDate
// ----------------------------------------------------------------------------

void
CPeekTime::GetLocalTimeDate(
	FILETIME&	outDate,
	FILETIME&	outTime )
{
	FILETIME	ftDateTime = *this;		// convert to FILETIME.

	// Convert the local file time to UTC file time.
	LONGTIME	ltDateTime;
	::FileTimeToLocalFileTime( &ftDateTime, &ltDateTime.ft );

	LONGTIME	ltDate = { (ltDateTime.i / kFileTimeUnitsInADay) * kFileTimeUnitsInADay };
	LONGTIME	ltTime = { ltDateTime.i % kFileTimeUnitsInADay };
	outDate = ltDate.ft;
	outTime = ltTime.ft;
}


// ----------------------------------------------------------------------------
//		Parse
// ----------------------------------------------------------------------------

void
CPeekTime::Parse(
	const CPeekString&	inTime )
{
	SYSTEMTIME	stTime;
	memset( &stTime, 0, sizeof( SYSTEMTIME ) );

	TIME_ZONE_INFORMATION	tzInfo;
	memset( &tzInfo, 0, sizeof( tzInfo ) );

	UInt32	nYear( 0 );
	UInt32	nMonth( 0 );
	UInt32	nDay( 0 );
	UInt32	nHour( 0 );
	UInt32	nMinute( 0 );
	UInt32	nSecond( 0 );
	UInt32	nMilliseconds( 0 );
	swscanf_s( inTime, L"%u-%u-%uT%u.%u.%u.%uZ",
		&nYear, &nMonth, &nDay,
		&nHour, &nMinute, &nSecond, &nMilliseconds );

	stTime.wYear = static_cast<WORD>( nYear );
	stTime.wMonth = static_cast<WORD>( nMonth );
	stTime.wDay = static_cast<WORD>( nDay );
	stTime.wHour = static_cast<WORD>( nHour );
	stTime.wMinute = static_cast<WORD>( nMinute );
	stTime.wSecond = static_cast<WORD>( nSecond );
	stTime.wMilliseconds = static_cast<WORD>( nMilliseconds );

	FILETIME ftFileTime;
	::SystemTimeToFileTime( &stTime, &ftFileTime );

	// Convert the local file time to UTC file time.
	//FILETIME ftUtcFileTime;
	//::LocalFileTimeToFileTime( &ftFileTime, &ftUtcFileTime );

	m_ltTime.ft = ftFileTime; // ftUtcFileTime;
	m_ltTime.i *= WPTIME_FILETIME;
}


// ----------------------------------------------------------------------------
//		SetLocal
// ----------------------------------------------------------------------------

void
CPeekTime::SetLocal(
	FILETIME	inDate,
	FILETIME	inTime )
{
	LONGTIME	ltDate;
	LONGTIME	ltTime;

	ltDate.ft = inDate;
	ltTime.ft = inTime;

	LONGTIME	ltCleanDate = { (ltDate.i / kFileTimeUnitsInADay) * kFileTimeUnitsInADay };
	LONGTIME	ltCleanTime = { ltTime.i % kFileTimeUnitsInADay };

	LONGTIME	ltLocalTime = { (ltCleanDate.i + ltCleanTime.i) };
	::LocalFileTimeToFileTime( &ltLocalTime.ft, &m_ltTime.ft );
	m_ltTime.i *= WPTIME_FILETIME;
}


// =============================================================================
//		CPeekDuration
// =============================================================================

// ----------------------------------------------------------------------------
//		Format
// ----------------------------------------------------------------------------

CPeekString
CPeekDuration::Format() const
{
	tTimeUnits	Units = CTimeUnits::GetUnits( m_nDuration );
	CPeekOutString	strValue;
	strValue << (m_nDuration / CTimeUnits::GetConversion( Units )) << L" " << CTimeUnits::GetLabel( Units );

	return strValue;
}


// ----------------------------------------------------------------------------
//		Parse
// ----------------------------------------------------------------------------

void
CPeekDuration::Parse(
	const CPeekString& inValue )
{
	wchar_t*	pLabel = NULL;
	UInt64		nCount = _wcstoui64( inValue, &pLabel, 10 );
	CPeekString	strLabel( pLabel );
	strLabel.Trim( L" :" );
	UInt64		nConversion = CTimeUnits::GetConversion( strLabel );
	m_nDuration = nCount * nConversion;
}
