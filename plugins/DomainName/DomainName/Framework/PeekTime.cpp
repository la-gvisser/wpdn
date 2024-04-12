// =============================================================================
//	PeekTime.cpp
// =============================================================================
//	Copyright (c) 2009-2011 WildPackets, Inc. All rights reserved.
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
//		tTimeFields
// =============================================================================

// -----------------------------------------------------------------------------
//		Decode
// -----------------------------------------------------------------------------

UInt64
tTimeFields::Decode() const
{
	const UInt32	nMonths = (fMonth < 3) ? (fMonth + 13) : (fMonth + 1);
	const UInt32	nYears = (fMonth < 3) ? (fYear - 1) : fYear;
	const UInt32	nLeaps = (3 * (nYears / 100) + 3) / 4;
	const UInt32	nDays  = ((36525 * nYears) / 100) - nLeaps + ((1959 * nMonths) / 64) + fDay - 584817;

	return (((((((static_cast<UInt64>( nDays * 24 ) + fHour) * 60) +
		fMinute) * 60) + fSecond) * WPTIME_SECONDS) + fNanosecond);
}


// -----------------------------------------------------------------------------
//		Encode
// -----------------------------------------------------------------------------

void
tTimeFields::Encode(
	UInt64	inTime )
{
	if ( inTime == 0 ) {
		memset( this, 0, sizeof( tTimeFields ) );
		return;
	}

	// Convert to seconds.
	const UInt64	nSeconds = inTime / WPTIME_SECONDS;

	// Split into days and seconds within the day.
	UInt32	nDays = static_cast<UInt32>( nSeconds / kSecondsInADay );
	UInt32	nSecondsInDay = static_cast<UInt32>( nSeconds % kSecondsInADay );

	// Time.
	fHour = nSecondsInDay / kSecondsInAnHour;
	nSecondsInDay =  nSecondsInDay % kSecondsInAnHour;
	fMinute = nSecondsInDay / kSecondsInAMinute;
	fSecond = nSecondsInDay % kSecondsInAMinute;
	fNanosecond = static_cast<UInt32>( inTime % WPTIME_SECONDS );

	// Date.
	// See, e.g., "30.6 Days Hath September" by James Miller,
	// http://www.amsat.org/amsat/articles/g3ruh/100.html.
	const UInt32	nLeaps = (3 * (((4 * nDays) + 1227) / kDaysPerQuadriCentennium) + 3 ) / 4;
	nDays += 28188 + nLeaps;
	const UInt32	nYears = ((20 * nDays) - 2442) / (5 * kDaysPerNormalQuadrennium);
	const UInt32	nYearday = nDays - ((nYears * kDaysPerNormalQuadrennium) / 4);
	const UInt32	nMonths = (64 * nYearday) / 1959;
	if ( nMonths < 14 ) {
		fMonth = nMonths - 1;
		fYear = nYears  + 1524;
	}
	else {
		fMonth = nMonths - 13;
		fYear = nYears + 1525;
	}
	fDay = nYearday - (1959 * nMonths) / 64;
}


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
	UInt64	nConversion = GetConversion( inType );
	_ASSERTE( nConversion != 0 );
	if ( nConversion == 0 ) {
		return inDuration;
	}

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
	const tTimeFields&	inTime,
	tTimeUnits			inFormat /*= kMilliseconds*/ )
{
	// As close to ISO 8601 as possible periods instead of colons
	// 25 characters: 'YYYY-MM-DDTHH.MM.SS.mmmZ'
	// 31 characters: 'YYYY-MM-DDTHH.MM.SS.nnnnnnnnnZ'
	CPeekOutString	ssTime;
	ssTime << std::setfill( L'0' )
			<< std::setw( 4 ) << inTime.fYear << L"-"
			<< std::setw( 2 ) << inTime.fMonth << L"-"
			<< std::setw( 2 ) << inTime.fDay << L"T"
			<< std::setw( 2 ) << inTime.fHour << L"."
			<< std::setw( 2 ) << inTime.fMinute << L"."
			<< std::setw( 2 ) << inTime.fSecond << L".";
	if ( inFormat == kMilliseconds ) {
		ssTime << std::setw( 3 ) << (inTime.fNanosecond / 1000000) << L"Z";
	}
	else {
		ssTime << std::setw( 9 ) << inTime.fNanosecond << L"Z";
	}

	return ssTime;
}

CPeekString
CPeekTime::Format(
	const SYSTEMTIME&	inTime )
{
	// 25 characters: 'YYYY-MM-DDTHH.MM.SS.mmmZ'
	// As close to ISO 8601 as possible periods instead of colons
	CPeekOutString	strTime;
	strTime << std::setfill( L'0' )
			<< std::setw( 4 ) << inTime.wYear << L"-"
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
	CPeekOutString	ssDate;
	ssDate << std::setfill( L'0' )
			<< std::setw( 4 ) << inTime.wYear << L"-"
			<< std::setw( 2 ) << inTime.wMonth << L"-"
			<< std::setw( 2 ) << inTime.wDay;

	return ssDate;
}

CPeekString
CPeekTime::FormatDate(
	const tTimeFields&	inTime )
{
	// 10 characters: 'YYYY-MM-DD'
	CPeekOutString	ssDate;
	ssDate << std::setfill( L'0' )
			<< std::setw( 4 ) << inTime.fYear << L"-"
			<< std::setw( 2 ) << inTime.fMonth << L"-"
			<< std::setw( 2 ) << inTime.fDay;

	return ssDate;
}


// ----------------------------------------------------------------------------
//		FormatTime
// ----------------------------------------------------------------------------

CPeekString
CPeekTime::FormatTime(
	const tTimeFields&	inTime,
	tTimeUnits			inFormat /*= kMilliseconds*/ )
{
	// 13 characters: 'HH.MM.SS.mmmZ'
	// 19 characters: 'HH.MM.SS.nnnnnnnnnZ'
	CPeekOutString	ssTime;
	ssTime << std::setfill( L'0' )
			<< std::setw( 2 ) << inTime.fHour << L"."
			<< std::setw( 2 ) << inTime.fMinute << L"."
			<< std::setw( 2 ) << inTime.fSecond << L".";
	if ( inFormat == kMilliseconds ) {
		ssTime << std::setw( 3 ) << (inTime.fNanosecond / 1000000) << L"Z";
	}
	else {
		ssTime << std::setw( 9 ) << inTime.fNanosecond << L"Z";
	}

	return ssTime;
}

CPeekString
CPeekTime::FormatTime(
	const SYSTEMTIME&	inTime )
{
	// 13 characters: 'HH.MM.SS.mmmZ'
	CPeekOutString	ssTime;
	ssTime << std::setfill( L'0' )
			<< std::setw( 2 ) << inTime.wHour << L"."
			<< std::setw( 2 ) << inTime.wMinute << L"."
			<< std::setw( 2 ) << inTime.wSecond << L"."
			<< std::setw( 3 ) << inTime.wMilliseconds << L"Z";

	return ssTime;
}


// ----------------------------------------------------------------------------
//		FormatA
// ----------------------------------------------------------------------------

CPeekStringA
CPeekTime::FormatA(
	const tTimeFields&	inTime,
	tTimeUnits			inFormat /*= kMilliseconds*/ )
{
	// As close to ISO 8601 as possible periods instead of colons
	// 25 characters: 'YYYY-MM-DDTHH.MM.SS.mmmZ'
	// 31 characters: 'YYYY-MM-DDTHH.MM.SS.nnnnnnnnnZ'
	CPeekOutStringA	ssTime;
	ssTime << std::setfill( '0' )
			<< std::setw( 4 ) << inTime.fYear << "-"
			<< std::setw( 2 ) << inTime.fMonth << "-"
			<< std::setw( 2 ) << inTime.fDay << "T"
			<< std::setw( 2 ) << inTime.fHour << "."
			<< std::setw( 2 ) << inTime.fMinute << "."
			<< std::setw( 2 ) << inTime.fSecond << ".";
	if ( inFormat == kMilliseconds ) {
		ssTime << std::setw( 3 ) << (inTime.fNanosecond / 1000000) << "Z";
	}
	else {
		ssTime << std::setw( 9 ) << inTime.fNanosecond << "Z";
	}

	return ssTime;
}

CPeekStringA
CPeekTime::FormatA(
	const SYSTEMTIME&	inTime )
{
	// 25 characters: 'YYYY-MM-DDTHH.MM.SS.mmmZ'
	// As close to ISO 8601 as possible periods instead of colons
	CPeekOutStringA	ssTime;
	ssTime << std::setfill( '0' )
			<< std::setw( 4 ) << inTime.wYear << "-"
			<< std::setw( 2 ) << inTime.wMonth << "-"
			<< std::setw( 2 ) << inTime.wDay << "T"
			<< std::setw( 2 ) << inTime.wHour << "."
			<< std::setw( 2 ) << inTime.wMinute << "."
			<< std::setw( 2 ) << inTime.wSecond << "."
			<< std::setw( 3 ) << inTime.wMilliseconds << "Z";

	return ssTime;
}


// ----------------------------------------------------------------------------
//		FormatDateA
// ----------------------------------------------------------------------------

CPeekStringA
CPeekTime::FormatDateA(
	const tTimeFields&	inTime )
{
	// 10 characters: 'YYYY-MM-DD'
	CPeekOutStringA	ssDate;
	ssDate << std::setfill( '0' )
			<< std::setw( 4 ) << inTime.fYear << "-"
			<< std::setw( 2 ) << inTime.fMonth << "-"
			<< std::setw( 2 ) << inTime.fDay;

	return ssDate;
}

CPeekStringA
CPeekTime::FormatDateA(
	const SYSTEMTIME&	inTime )
{
	// 10 characters: 'YYYY-MM-DD'
	CPeekOutStringA	ssDate;
	ssDate << std::setfill( '0' )
			<< std::setw( 4 ) << inTime.wYear << "-"
			<< std::setw( 2 ) << inTime.wMonth << "-"
			<< std::setw( 2 ) << inTime.wDay;

	return ssDate;
}


// ----------------------------------------------------------------------------
//		FormatTimeA
// ----------------------------------------------------------------------------

CPeekStringA
CPeekTime::FormatTimeA(
	const tTimeFields&	inTime,
	tTimeUnits			inFormat /*= kMilliseconds*/ )
{
	// 13 characters: 'HH.MM.SS.mmmZ'
	// 19 characters: 'HH.MM.SS.nnnnnnnnnZ'
	CPeekOutStringA	ssTime;
	ssTime << std::setfill( '0' )
			<< std::setw( 2 ) << inTime.fHour << "."
			<< std::setw( 2 ) << inTime.fMinute << "."
			<< std::setw( 2 ) << inTime.fSecond << ".";
	if ( inFormat == kMilliseconds ) {
		ssTime << std::setw( 3 ) << (inTime.fNanosecond / 1000000) << "Z";
	}
	else {
		ssTime << std::setw( 9 ) << inTime.fNanosecond << "Z";
	}

	return ssTime;
}

CPeekStringA
CPeekTime::FormatTimeA(
	const SYSTEMTIME&	inTime )
{
	// 13 characters: 'HH.MM.SS.mmmZ'
	CPeekOutStringA	ssTime;
	ssTime << std::setfill( '0' )
			<< std::setw( 2 ) << inTime.wHour << "."
			<< std::setw( 2 ) << inTime.wMinute << "."
			<< std::setw( 2 ) << inTime.wSecond << "."
			<< std::setw( 3 ) << inTime.wMilliseconds << "Z";

	return ssTime;
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
	FILETIME	ftDateTime = m_ltTime;		// convert to FILETIME.

	// Convert the local file time to UTC file time.
	tLongTime	ltDateTime;
	::FileTimeToLocalFileTime( &ftDateTime, &ltDateTime.ft );

	tLongTime	ltDate( (ltDateTime.i / kFileTimeUnitsInADay) * kFileTimeUnitsInADay );
	tLongTime	ltTime( ltDateTime.i % kFileTimeUnitsInADay );
	outDate = ltDate.ft;
	outTime = ltTime.ft;
}


// -----------------------------------------------------------------------------
//		GetLocalTimeOffset		[static]
// -----------------------------------------------------------------------------
//	Return the current offset to UTC time in PeekTime units (nanoseconds).

SInt64
CPeekTime::GetLocalTimeOffset()
{
	static int		s_bInitialized = false;
	static SInt64	s_nLocalTimeOffset = 0;

	if ( !s_bInitialized ) {
		s_bInitialized = true;
#ifdef _WIN32
		DWORD					dwResult;
		TIME_ZONE_INFORMATION	tzi;
		dwResult = ::GetTimeZoneInformation( &tzi );
		if ( dwResult != TIME_ZONE_ID_INVALID ) {
			s_nLocalTimeOffset = tzi.Bias;
			if ( dwResult == TIME_ZONE_ID_STANDARD ) {
				s_nLocalTimeOffset += tzi.StandardBias;	// in minutes.
			}
			else if ( dwResult == TIME_ZONE_ID_DAYLIGHT ) {
				s_nLocalTimeOffset += tzi.DaylightBias;	// in minutes.
			}
			s_nLocalTimeOffset *= 60;				// Minutes to seconds
			s_nLocalTimeOffset *= WPTIME_SECONDS;	// To PeekTime units (nanoseconds)
		}
#else
		const time_t        t = time( 0 );
		const struct tm*    ptm = localtime( &t );
		s_nLocalTimeOffset = -ptm->tm_gmtoff;	// Seconds
		s_nLocalTimeOffset *= WPTIME_SECONDS;	// PeekTime units (nanoseconds)
#endif
	}

	return s_nLocalTimeOffset;
}


// ----------------------------------------------------------------------------
//		Parse
// ----------------------------------------------------------------------------

void
CPeekTime::Parse(
	const CPeekString&	inTime )
{
	wchar_t		cSeparators[7] = { L' ', L' ', L' ', L' ', L' ', L' ', L' ' };
	tTimeFields	tfTime;
	memset( &tfTime, 0, sizeof( tfTime ) );

	std::wistringstream	ssTime( inTime );
	ssTime >> tfTime.fYear >> cSeparators[0] >>
		tfTime.fMonth >> cSeparators[1] >>
		tfTime.fDay >> cSeparators[2] >>
		tfTime.fHour >> cSeparators[3] >>
		tfTime.fMinute >> cSeparators[4] >>
		tfTime.fSecond >> cSeparators[5] >>
		tfTime.fNanosecond >> cSeparators[6];

	// Convert milliseconds to nanoseconds.
	if ( tfTime.fNanosecond < 1000 ) {
		tfTime.fNanosecond *= 1000000;
	}

	m_ltTime.i = tfTime;
}

void
CPeekTime::Parse(
	const CPeekStringA&	inTime )
{
	char		cSeparators[7] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
	tTimeFields	tfTime;
	memset( &tfTime, 0, sizeof( tfTime ) );

	std::istringstream	ssTime( inTime );
	ssTime >> tfTime.fYear >> cSeparators[0] >>
		tfTime.fMonth >> cSeparators[1] >>
		tfTime.fDay >> cSeparators[2] >>
		tfTime.fHour >> cSeparators[3] >>
		tfTime.fMinute >> cSeparators[4] >>
		tfTime.fSecond >> cSeparators[5] >>
		tfTime.fNanosecond >> cSeparators[6];

	// Convert milliseconds to nanoseconds.
	if ( tfTime.fNanosecond < 1000 ) {
		tfTime.fNanosecond *= 1000000;
	}

	m_ltTime.i = tfTime;
}


// ----------------------------------------------------------------------------
//		SetLocal
// ----------------------------------------------------------------------------

void
CPeekTime::SetLocal(
	FILETIME	inDate,
	FILETIME	inTime )
{
	tLongTime	ltDate( inDate );
	tLongTime	ltTime( inTime );
	tLongTime	ltCleanDate( (ltDate.i / kPeekTimeUnitsInADay) * kPeekTimeUnitsInADay );
	tLongTime	ltCleanTime( ltTime.i % kPeekTimeUnitsInADay );

	tLongTime	ltLocalTime( ltCleanDate.i + ltCleanTime.i );
	m_ltTime.i = ltLocalTime.i + GetLocalTimeOffset();
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
	CPeekOutString	ssValue;
	ssValue << (m_nDuration / CTimeUnits::GetConversion( Units )) <<
		L" " << CTimeUnits::GetLabel( Units );

	return ssValue;
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
