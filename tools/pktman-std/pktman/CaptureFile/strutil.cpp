// =============================================================================
//	strutil.cpp
// =============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#include "strutil.h"
#include <chrono>
#include <cstring>
#include <iomanip>
#include <ostream>
#include <string>


// Seconds between 1/1/1601 and 1/1/1970
// (see KB167296: http://support.microsoft.com/default.aspx?scid=KB;en-us;q167296).
const UInt64	kFileTimeToAnsi         = 11644473600ull;
const UInt64 kPeekTimeToAnsi         = kFileTimeToAnsi;
// number of Peek Time counts in a File Time.
const UInt64 kPeekTimePerFileTime    = 100ull;

// number of WPTime counts from 1/1/1601 to 1/1/1970.
// (Microsoft KB167296: 116444736000000000 * 100).
const UInt64	kPeekTimeANSIOffset     = 11644473600000000000ull;
// number of PeekTime counts in a single second.
const UInt64	kPeekTimeSeconds        = 1000000000ull;
const UInt64 kResolutionPerSecond    	= 1000000000ull;	// nanoseconds
const UInt64	kDaysPerQuadriCentennium    = (365 * 400) + 97;
const SInt64	kDaysPerNormalQuadrennium   = (365 * 4) + 1;

const UInt64	kMillisecondsPerSecond  = 1000;
const UInt64	kMicrosecondsPerSecond  = 1000000;
const UInt64	kNanosecondsPerSecond   = 1000000000;
const UInt64	kSecondsPerMinute		= 60;
const UInt64	kSecondsPerHour			= 60 * kSecondsPerMinute;
const UInt64	kSecondsPerDay			= 24 * kSecondsPerHour;
const UInt64	kSecondsPerWeek			= 7 * kSecondsPerDay;
const UInt64	kSecondsPerYear			= (3652422 * kSecondsPerDay) / 10000;


// -----------------------------------------------------------------------------
//      AtoW
// -----------------------------------------------------------------------------

wstring
AtoW(
	const char*	inOther,
	size_t		inLength )
{
	wstring	strResult;

	if ( inOther == nullptr ) return strResult;

	size_t	nLength = (inLength > 0) ? inLength : std::strlen( inOther );
	if ( nLength == 0 ) return strResult;

	size_t	nSize = ((inLength > 0) ? inLength : std::strlen( inOther )) + 1;
	std::wstring	strBuf( nSize, L' ' );

#ifdef _WIN32
	size_t	nNewSize( 0 );
	errno_t	nErr = mbstowcs_s( &nNewSize, &strBuf[0], nLength, inOther, nSize);
#else
	size_t	nNewSize = mbstowcs(&strBuf[0], inOther, nSize);
#endif

	if ( nNewSize > 0 ) {
		strResult = std::move( strBuf );
	}

	return strResult;
}

wstring
AtoW(
	const string&	inOther )
{
	return AtoW( inOther.c_str(), inOther.length() );
}


// -----------------------------------------------------------------------------
//      WtoA
// -----------------------------------------------------------------------------

string
WtoA(
	const wchar_t*	inOther,
	size_t			inLength )
{
	string	strResult;

	if ( inOther == nullptr ) return strResult;

	size_t	nSize = ((inLength > 0) ? inLength : wcslen( inOther )) + 1;
	std::string	strBuf( nSize, ' ' );

#ifdef _WIN32
	size_t	nNewSize(0);
	errno_t	nErr = wcstombs_s( &nNewSize, &strBuf[0], nSize, inOther, nSize );
#else
	size_t	nNewSize = wcstombs( &strBuf[0], inOther, nSize );
#endif

	if ( nNewSize > 0 ) {
		strResult = std::move( strBuf );
	}

	return strResult;
}

string
WtoA(
	const wstring&	inOther )
{
	return WtoA( inOther.c_str(), inOther.length() );
}


// =============================================================================
//		Now
// =============================================================================

UInt64
Now()
{
	auto	now( std::chrono::time_point_cast<std::chrono::nanoseconds>(
				std::chrono::high_resolution_clock::now()) );
	return now.time_since_epoch().count() + kPeekTimeANSIOffset;
}


// =============================================================================
//		ParseTime
// =============================================================================

CTimeFields
ParseTime(
	const wstring&	inTime )
{
	// #pragma(TODO)
	CTimeFields	tfTime( 0 );
	return tfTime;
}
