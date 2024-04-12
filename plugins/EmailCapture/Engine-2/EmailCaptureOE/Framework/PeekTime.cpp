// =============================================================================
//	PeekTime.cpp
// =============================================================================
//	Copyright (c) 2009-2015 Savvius, Inc. All rights reserved.
//
// Peek Time is nanoseconds since 1/1/1601. 64 bits, unsigned.
// Overflows in the year 2185 AD.
//

#include "StdAfx.h"
#include "PeekTime.h"
#include <string.h>
#include <vector>

#ifdef WP_LINUX
#include <chrono>
#include <unistd.h>
#endif


// =============================================================================
//		Globals
// =============================================================================

static CTimeUnits	g_TimeUnits;
CPeekTime			g_pkLastTime( true );

#ifdef _WIN32
typedef UInt64 (*GetTimeStampProc)();
static GetTimeStampProc	s_fnGetTimeStamp = nullptr;


// =============================================================================
//		ResolveGetTimeStamp
// =============================================================================

static HMODULE			s_hPeekDll = nullptr;

static bool
ResolveGetTimeStamp()
{
	if ( s_hPeekDll == nullptr ) {
		s_hPeekDll = LoadLibraryW( L"peek.dll" );
	}
	if ( (s_hPeekDll != nullptr) && (s_fnGetTimeStamp == nullptr) ) {
		s_fnGetTimeStamp = (GetTimeStampProc)GetProcAddress( s_hPeekDll, "PeekGetTimeStamp" );
	}
}
#endif


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
//		tLongTime
// =============================================================================

_tLongTime
_tLongTime::Now()
{
	_tLongTime ltNow;
#ifdef _WIN32
	if ( s_fnGetTimeStamp ) {
		ltNow.i = (*s_fnGetTimeStamp)();
	}
	else {
		::GetSystemTimeAsFileTime( &ltNow.ft );
		ltNow.i *= WPTIME_FILETIME;
	}
#else
	auto	now( std::chrono::time_point_cast<std::chrono::nanoseconds>(
				std::chrono::high_resolution_clock::now()) );
	ltNow.i = now.time_since_epoch().count() + WPTIME_ANSI_OFFSET;
#endif

	return ltNow;
}


// =============================================================================
//		CTimeUnits
// =============================================================================

std::vector<tTimeUnitsItem>	CTimeUnits::s_aTimeUnits;
size_t						CTimeUnits::s_nCount = g_TimeUnits.Init();


// ----------------------------------------------------------------------------
//		Init
// ----------------------------------------------------------------------------

size_t
CTimeUnits::Init()
{
	tTimeUnitsItem tuiNanoSeconds = { kNanoseconds, 1, CPeekString( L"Nanoseconds" ) };
	CTimeUnits::s_aTimeUnits.push_back( tuiNanoSeconds );

	tTimeUnitsItem tuiMilliSeconds = { kMilliseconds, WPTIME_MILLISECONDS, CPeekString( L"Milliseconds" ) };
	CTimeUnits::s_aTimeUnits.push_back( tuiMilliSeconds );

	tTimeUnitsItem tuiMicroSeconds = { kMicroseconds, WPTIME_MICROSECONDS, CPeekString( L"Microseconds" ) };
	CTimeUnits::s_aTimeUnits.push_back( tuiMicroSeconds );

	tTimeUnitsItem tuiSeconds = { kSeconds, WPTIME_SECONDS, CPeekString( L"Seconds" ) };
	CTimeUnits::s_aTimeUnits.push_back( tuiSeconds );

	tTimeUnitsItem tuiMinutess = { kMinutes, WPTIME_MINUTES, CPeekString( L"Minutes" ) };
	CTimeUnits::s_aTimeUnits.push_back( tuiMinutess );

	tTimeUnitsItem tuiHours = { kHours, WPTIME_HOURS, CPeekString( L"Hours" ) };
	CTimeUnits::s_aTimeUnits.push_back( tuiHours );

	tTimeUnitsItem tuiDays = { kDays, WPTIME_DAYS, CPeekString( L"Days" ) };
	CTimeUnits::s_aTimeUnits.push_back( tuiDays );

	return s_aTimeUnits.size();
}


// ----------------------------------------------------------------------------
//		GetConversion
// ----------------------------------------------------------------------------

UInt64
CTimeUnits::GetConversion(
	tTimeUnits	inType )
{
	return (inType < static_cast<int>( s_nCount ))
		? s_aTimeUnits[inType].fConversion
		: s_aTimeUnits[0].fConversion;
}

UInt64
CTimeUnits::GetConversion(
	const CPeekString&	inLabel )
{
	for ( size_t i = (s_nCount - 1); i > 0; --i ) {
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
	return (inType < static_cast<int>( s_nCount ))
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
	for ( size_t i = (s_nCount - 1); i > 0; --i ) {
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
	for ( size_t i = (s_nCount - 1); i > 0; --i ) {
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

#ifdef _WIN32
CPeekTime::CPeekTime(
	const CPeekTimeLocal& inTime )
{
	FILETIME	ftLocal = inTime;
	::LocalFileTimeToFileTime( &ftLocal, &m_ltTime.ft );
}
#endif

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

#ifdef _WIN32
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
#endif

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

#ifdef _WIN32
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
#endif

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

#ifdef _WIN32
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
#endif

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

#ifdef _WIN32
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
#endif

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

#ifdef _WIN32
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
#endif

// ----------------------------------------------------------------------------
//		GetRandomizedMilliSec
// ----------------------------------------------------------------------------

UInt16
CPeekTime::GetRandomizedMilliSec(
	bool	inSeedRandomizer )
{
	static UInt64	uSeed = 0;

	if ( inSeedRandomizer ) {
#ifdef _WIN32
		uSeed = GetTickCount();
#else
		uSeed = static_cast<UInt64>( time( nullptr ) );
#endif
	}

	srand( static_cast<UInt32>( uSeed ) );

	int	nRandNumber;
	while( (nRandNumber = rand()) > 999 );

	return static_cast<UInt16>( nRandNumber );
}


// ----------------------------------------------------------------------------
//		GetTimeStringFileName
// ----------------------------------------------------------------------------

CPeekString
CPeekTime::GetTimeStringFileName(
	bool	inRandomMilli )
{
	CPeekTime	pkTime( true );
	int			x = 0;

	while ( pkTime == g_pkLastTime ) {
#ifdef _WIN32
		Sleep( 1 );
#else
		sleep( 1 );
#endif
		pkTime.SetNow();
		if ( x++ > 1000 ) throw 1;
	}
	g_pkLastTime = pkTime;

#ifdef _WIN32
	SYSTEMTIME	stTime = pkTime;
	if ( inRandomMilli ) {
		stTime.wMilliseconds = GetRandomizedMilliSec( true );
	}

	return Format( stTime );
#else
	// TODO
	return L"";
#endif
}

#ifdef _WIN32
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
#endif

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
//		Set
// ----------------------------------------------------------------------------

void
CPeekTime::SetUnits(
	tTimeUnits	inType,
	UInt32		inValue,
	bool		inClearLessSignificant /*= false*/ )
{
	tTimeFields fields( m_ltTime.i );
	switch ( inType )
	{
		case kNanoseconds:
			fields.fNanosecond = ((fields.fNanosecond / 1000) * 1000) + inValue;
			break;

		case kMicroseconds:
			// Nanoseconds are cleared.
			fields.fNanosecond = ((fields.fNanosecond / 1000000) * 1000000) + (inValue * 1000);
			break;

		case kMilliseconds:
			// Microseconds and nanoseconds are cleared.
			fields.fNanosecond = inValue * 1000000;
			break;

		case kSeconds:
			fields.fSecond = inValue;
			break;

		case kMinutes:
			fields.fMinute = inValue;
			break;

		case kHours:
			fields.fHour = inValue;
			break;

		case kMonths:
			fields.fMonth = inValue;
			break;

		case kYears:
			fields.fYear = inValue;
			break;

		default:
			break;
	}

	if ( inClearLessSignificant ) {
		// No 'breaks'!
		switch ( inType )
		{
			case kYears:				// No break, continue to the next case.
				fields.fMonth = 0;

			case kMonths:				// No break, continue to the next case.
				fields.fDay = 0;

			case kDays:					// No break, continue to the next case.
				fields.fHour = 0;

			case kHours:				// No break, continue to the next case.
				fields.fMinute= 0;

			case kMinutes:				// No break, continue to the next case.
				fields.fSecond = 0;

			case kSeconds:				// No break, continue to the next case.
				fields.fNanosecond = 0;

#if (0)
		// Setting millisecond or microsecond clears the less significant digits.
			case kMilliseconds:			// No break, continue to the next case.
				fields.fNanosecond = (fields.fNanosecond / 1000000) * 1000000;

			case kMicroseconds:			// No break, continue to the next case.
				fields.fNanosecond = (fields.fNanosecond / 1000) * 1000;
#endif
			default:
				break;
		}
	}

	m_ltTime.i = fields.Decode();
}

#ifdef _WIN32
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
#endif


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
	wchar_t*	pLabel = nullptr;
	// TODO does wcstooul return a 64-bit value?
	UInt64		nCount = wcstoul( inValue, &pLabel, 10 );
	CPeekString	strLabel( pLabel );
	strLabel.Trim( L" :" );
	UInt64		nConversion = CTimeUnits::GetConversion( strLabel );
	m_nDuration = nCount * nConversion;
}
