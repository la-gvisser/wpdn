// =============================================================================
//	TimeConv.h
// =============================================================================
//	Copyright (c) 1996-2006 WildPackets, Inc. All rights reserved.

#pragma once

#include "WPTypes.h"
#include <math.h>
#include <time.h>

#define RESOLUTION_PER_SECOND	(1000000000ull)	// nanoseconds

namespace TimeConv
{

#ifdef _WIN32
typedef union
{
	UInt64		ft_scalar;
	FILETIME	ft_struct;
} FileTimeConv;
#endif

#include "wppushpack.h"
typedef union
{
	struct
	{
		UInt16	Second : 5; 	// Second divided by 2.
		UInt16	Minute : 6; 	// Minute (0-59).
		UInt16	Hour   : 5;		// Hour (0-23).
	} WP_PACK_STRUCT u;
	UInt16	WordPart;
} WP_PACK_STRUCT DOSTIME;

typedef union
{
	struct
	{
		UInt16	Day   : 5;		// Day of month (1-31).
		UInt16	Month : 4;		// Month (1 = Jan, etc.).
		UInt16	Year  : 7;		// Year offset from 1980.
	} WP_PACK_STRUCT u;
	UInt16	WordPart;
} WP_PACK_STRUCT DOSDATE;
#include "wppoppack.h"

// ANSI time base: Jan 1 1970
// Mac time base:  Jan 1 1904
// AFP time base:  Jan 1 2000
// Peek time base: Jan 1 1601
const UInt32 TIMECONV_MAC_TO_ANSI       = 2082844800ul;
const UInt32 TIMECONV_ANSI_TO_AFP       = 946710000ul;
const UInt32 TIMECONV_MAC_TO_AFP        = 3029529600ul;
const UInt64 TIMECONV_FILE_TO_ANSI      = 11644473600ull;			// Seconds between 1/1/1601 and 1/1/1970 (see KB167296: http://support.microsoft.com/default.aspx?scid=KB;en-us;q167296).
const UInt64 TIMECONV_PEEK_TO_ANSI      = TIMECONV_FILE_TO_ANSI;
const UInt64 TIMECONV_PEEK_TO_MICROPEEK = 9561628800000000000ull;	// Nanoseconds between 1/1/1601 and 1/1/1904.

inline UInt64	FileToPeek( UInt64 nFileTime );
inline UInt64 	PeekToFile( UInt64 nPeekTime );

inline UInt64	AnsiToPeek( time_t nAnsiTime );
inline time_t	PeekToAnsi( UInt64 nPeekTime );

inline UInt64	TimevalToPeek( SInt32 tv_sec, SInt32 tv_usec );
inline void		PeekToTimeval( UInt64 nPeekTime, SInt32* tv_sec, SInt32* tv_usec );

inline UInt64	AnsiToFile( time_t nAnsiTime );
inline time_t	FileToAnsi( UInt64 nFileTime );

inline UInt32	AnsiToMac( time_t nAnsiTime );
inline time_t	MacToAnsi( UInt32 nMacTime );

inline UInt32	AnsiToAFP( time_t nAnsiTime );
inline time_t	AFPToAnsi( UInt32 nAFPTime );

inline UInt32	AnsiToDos( time_t nAnsiTime );
inline time_t	DosToAnsi( UInt16 nDate, UInt16 nTime );

inline UInt64	MicroPeekToPeek( UInt64 nMicroPeekTime );
inline UInt64	PeekToMicroPeek( UInt64 nPeekTime );

inline UInt64	ERFToPeek( UInt64 nERFTime );

#ifdef _WIN32
inline UInt64	OleToPeek( DATE nOleTime );
inline DATE		PeekToOle( UInt64 nPeekTime );
#endif

inline SInt32	GetOffsetToUTCInSeconds();

} /* namespace TimeConv */


// -----------------------------------------------------------------------------
//		FileToPeek
// -----------------------------------------------------------------------------

UInt64
TimeConv::FileToPeek(
	UInt64	nFileTime )
{
	// Now in 100-nanosecond units since Jan 1 1601.
	nFileTime = nFileTime * 100;
	// Now in nanoseconds since Jan 1 1601.

	return nFileTime;
}


// -----------------------------------------------------------------------------
//		PeekToFile
// -----------------------------------------------------------------------------

UInt64
TimeConv::PeekToFile(
	UInt64	nPeekTime )
{
	// Now in nanoseconds since Jan 1 1601.
	nPeekTime = nPeekTime / 100;
	// Now in 100-nanosecond units since Jan 1 1601.

	return nPeekTime;
}


// -----------------------------------------------------------------------------
//		AnsiToPeek
// -----------------------------------------------------------------------------

UInt64
TimeConv::AnsiToPeek(
	time_t	nAnsiTime )
{
	UInt64	nPeekTime = (UInt64) nAnsiTime;
	// Now in seconds since Jan 1 1970.
	nPeekTime = nPeekTime + TIMECONV_PEEK_TO_ANSI;
	// Now in seconds since Jan 1 1601.
	nPeekTime = nPeekTime * 1000000000;
	// Now in nanoseconds since Jan 1 1601.

	return nPeekTime;
}


// -----------------------------------------------------------------------------
//		PeekToAnsi
// -----------------------------------------------------------------------------

time_t
TimeConv::PeekToAnsi(
	UInt64	nPeekTime )
{
	// Now in nanoseconds since Jan 1 1601.
	UInt64	nTime = nPeekTime / 1000000000;
	// Now in seconds since Jan 1 1601.

	// Check for underflow (sometimes time_t is unsigned!).
	if ( nTime < TIMECONV_PEEK_TO_ANSI )
	{
		return (time_t) 0;
	}
	time_t	nAnsiTime = (time_t)(nTime - TIMECONV_PEEK_TO_ANSI);
	// Now in seconds since Jan 1 1970.

	return nAnsiTime;
}


// -----------------------------------------------------------------------------
//		TimevalToPeek
// -----------------------------------------------------------------------------

UInt64
TimeConv::TimevalToPeek(
	SInt32	tv_sec,
	SInt32	tv_usec )
{
	UInt64	nPeekTime = AnsiToPeek( tv_sec );
	nPeekTime += tv_usec * (RESOLUTION_PER_SECOND / 1000000);
	return nPeekTime;
}


// -----------------------------------------------------------------------------
//		PeekToTimeval
// -----------------------------------------------------------------------------

void
TimeConv::PeekToTimeval(
	UInt64	nPeekTime,
	SInt32*	tv_sec,
	SInt32*	tv_usec )
{
	// Now in nanoseconds since Jan 1 1601.
	UInt64	t = nPeekTime / RESOLUTION_PER_SECOND;
	// Now in seconds since Jan 1 1601.

	// Check for underflow.
	if ( t < TIMECONV_PEEK_TO_ANSI )
	{
		*tv_sec = 0;
		*tv_usec = 0;
		return;
	}

	*tv_sec = (SInt32)(t - TIMECONV_PEEK_TO_ANSI);
	// Now in seconds since Jan 1 1970.

	// Calculate remaining microseconds.
	*tv_usec = (SInt32)((nPeekTime % RESOLUTION_PER_SECOND) / (RESOLUTION_PER_SECOND / 1000000));
}


// -----------------------------------------------------------------------------
//		AnsiToFile
// -----------------------------------------------------------------------------

UInt64
TimeConv::AnsiToFile(
	time_t	nAnsiTime )
{
	UInt64	nFileTime = (UInt64) nAnsiTime;
	// Now in seconds since Jan 1 1970.
	nFileTime = nFileTime + TIMECONV_FILE_TO_ANSI;
	// Now in seconds since Jan 1 1601.
	nFileTime = nFileTime * 10000000;
	// Now in 100-nanosecond units since Jan 1 1601.

	return nFileTime;
}


// -----------------------------------------------------------------------------
//		FileToAnsi
// -----------------------------------------------------------------------------

time_t
TimeConv::FileToAnsi(
	UInt64	nFileTime )
{
	// Now in 100-nanosecond units since Jan 1 1601.
	UInt64	nTime = nFileTime / 10000000;
	// Now in seconds since Jan 1 1601.

	// Check for underflow (sometimes time_t is unsigned!).
	if ( nTime < TIMECONV_FILE_TO_ANSI )
	{
		return (time_t) 0;
	}
	time_t	nAnsiTime = (time_t)(nTime - TIMECONV_FILE_TO_ANSI);
	// Now in seconds since Jan 1 1970.

	return nAnsiTime;
}


// -----------------------------------------------------------------------------
//		AnsiToMac
// -----------------------------------------------------------------------------

UInt32
TimeConv::AnsiToMac(
	time_t	nAnsiTime )
{
	return (UInt32)(nAnsiTime + TIMECONV_MAC_TO_ANSI);
}


// -----------------------------------------------------------------------------
//		MacToAnsi
// -----------------------------------------------------------------------------

time_t
TimeConv::MacToAnsi(
	UInt32	nMacTime )
{
	if ( nMacTime < TIMECONV_MAC_TO_ANSI )
	{
		return (time_t) 0;
	}

	return (time_t)(nMacTime - TIMECONV_MAC_TO_ANSI);
}


// -----------------------------------------------------------------------------
//		AnsiToAFP
// -----------------------------------------------------------------------------

UInt32
TimeConv::AnsiToAFP(
	time_t	nAnsiTime )
{
	return (UInt32)(nAnsiTime + TIMECONV_ANSI_TO_AFP);
}


// -----------------------------------------------------------------------------
//		AFPToAnsi
// -----------------------------------------------------------------------------

time_t
TimeConv::AFPToAnsi(
	UInt32	nAFPTime )
{
	return (time_t)(nAFPTime + TIMECONV_ANSI_TO_AFP);
}


// -----------------------------------------------------------------------------
//		AnsiToDos
// -----------------------------------------------------------------------------

UInt32
TimeConv::AnsiToDos(
	time_t	nAnsiTime )
{
	struct tm	AnsiTime;
	int			nError = localtime_s( &AnsiTime, &nAnsiTime );
	if ( nError != 0 ) return 0;

	DOSDATE theDate;
	theDate.u.Year   = AnsiTime.tm_year - 80;
	theDate.u.Month  = AnsiTime.tm_mon + 1;
	theDate.u.Day    = AnsiTime.tm_mday;

	DOSTIME theTime;
	theTime.u.Hour   = AnsiTime.tm_hour;
	theTime.u.Minute = AnsiTime.tm_min;
	theTime.u.Second = AnsiTime.tm_sec / 2;

	return (UInt32)((theDate.WordPart << 16) | theTime.WordPart);
}


// -----------------------------------------------------------------------------
//		DosToAnsi
// -----------------------------------------------------------------------------

time_t
TimeConv::DosToAnsi(
	UInt16	inDate,
	UInt16	inTime )
{
	DOSDATE theDate;
	theDate.WordPart = inDate;
	DOSTIME theTime;
	theTime.WordPart = inTime;

	struct tm	theAnsiTm;
	theAnsiTm.tm_sec   = theTime.u.Second * 2;
	theAnsiTm.tm_min   = theTime.u.Minute;
	theAnsiTm.tm_hour  = theTime.u.Hour;
	theAnsiTm.tm_mday  = theDate.u.Day;
	theAnsiTm.tm_mon   = theDate.u.Month;
	theAnsiTm.tm_mon  -= 1;
	theAnsiTm.tm_year  = theDate.u.Year;
	theAnsiTm.tm_year += 80;
	theAnsiTm.tm_wday  = 0;
	theAnsiTm.tm_yday  = 0;
	theAnsiTm.tm_isdst = -1;	// -1 is "use system to compute if DST applies", 0 is "don't use DST"

	return mktime( &theAnsiTm );
}


// -----------------------------------------------------------------------------
//		MicroPeekToPeek
// -----------------------------------------------------------------------------

UInt64
TimeConv::MicroPeekToPeek(
	UInt64	nMicroPeekTime )
{
	UInt64	nPeekTime = nMicroPeekTime;
	// Now in microseconds since 1/1/1904 local time
	nPeekTime *= 1000;
	// Now in nanoseconds since 1/1/1904 local time.
	nPeekTime += TIMECONV_PEEK_TO_MICROPEEK;
	// Now in nanoseconds units since 1/1/1601 local time.
	nPeekTime += (GetOffsetToUTCInSeconds() * RESOLUTION_PER_SECOND);
	// Now in nanoseconds since 1/1/1601 UTC time.

	return nPeekTime;
}


// -----------------------------------------------------------------------------
//		PeekToMicroPeek
// -----------------------------------------------------------------------------

UInt64
TimeConv::PeekToMicroPeek(
	UInt64	nPeekTime )
{
	UInt64	nMicroPeekTime = nPeekTime;
	// Now in nanoseconds since 1/1/1601 UTC time.
	nPeekTime -= (GetOffsetToUTCInSeconds() * RESOLUTION_PER_SECOND);
	// Now in nanoseconds since 1/1/1601 local time.
	nMicroPeekTime -= TIMECONV_PEEK_TO_MICROPEEK;
	// Now in nanoseconds units since 1/1/1904 local time.
	nMicroPeekTime /= 1000;
	// Now in microseconds since 1/1/1904 local time.

	return nMicroPeekTime;
}


// -----------------------------------------------------------------------------
//		ERFToPeek
// -----------------------------------------------------------------------------

UInt64
TimeConv::ERFToPeek(
	UInt64 nERFTime )
{
	// ERF is in time since Jan 1 1970.  The top 32 bits are seconds
	// and the bottom 32 bits are the binary fraction of the second.
	UInt64	nPeekTime = AnsiToPeek( (time_t)(nERFTime >> 32) );
	UInt64  nDecimalSeconds = nERFTime&0xFFFFFFFFULL;
	nDecimalSeconds *= 1000000000ULL;
	nPeekTime += (nDecimalSeconds >> 32);
	return nPeekTime;
}


#ifdef _WIN32

// -----------------------------------------------------------------------------
//		OleToPeek
// -----------------------------------------------------------------------------

UInt64
TimeConv::OleToPeek(
	DATE	nOleTime )
{
	// Get the integer and fractional parts of the OLE time.
	double	i, f;
	f = modf( nOleTime, &i );

	// Convert OLE time to PEEK time without the fractional day.
	SYSTEMTIME	st;
	HE_VERIFY( ::VariantTimeToSystemTime( i, &st ) );
	UInt64	nFileTime;
	HE_VERIFY( ::SystemTimeToFileTime( &st, (FILETIME*) &nFileTime ) );
	UInt64	nPeekTime = TimeConv::FileToPeek( nFileTime );

	// Add the fractional day in nanoseconds.
	nPeekTime += (SInt64)(f * 86400000000000.0);

	return nPeekTime;
}


// -----------------------------------------------------------------------------
//		PeekToOle
// -----------------------------------------------------------------------------

DATE
TimeConv::PeekToOle(
	UInt64	nPeekTime )
{
	// Convert PEEK time to OLE time without fractional seconds.
	UInt64		nFileTime = TimeConv::PeekToFile( nPeekTime );
	SYSTEMTIME	st;
	HE_VERIFY( ::FileTimeToSystemTime( (FILETIME*) &nFileTime, &st ) );
	st.wMilliseconds = 0;	// TODO: zero hours, minutes, seconds too?
	DATE		nOleTime;
	HE_VERIFY( ::SystemTimeToVariantTime( &st, &nOleTime ) != 0 );

	// Get the whole integer part of the OLE time (this is days).
	double	i;
	modf( nOleTime, &i );

	// Get the time of day in seconds.
	double	n;
	n = (st.wHour * 60 * 60) + (st.wMinute * 60) + st.wSecond;

	// Convert to nanoseconds.
	n *= 1000000000.0;

	// Add the number of nanoseconds from PEEK time.
	n += (nPeekTime % 1000000000UL);
	
	// Calculate the fractional part of the OLE time.
	double	f = n / 86400000000000.0;

	// Put the OLE time back together.
	nOleTime = i + f;

	return nOleTime;
}

#endif /* _WIN32 */


// -----------------------------------------------------------------------------
//		GetOffsetToUTCInSeconds
// -----------------------------------------------------------------------------
//	Return the current offset to UTC time in seconds. The results includes
//	the adjustment for daylight savings time if it is currently in effect.

SInt32
TimeConv::GetOffsetToUTCInSeconds()
{
	static int		s_Cached         = -1;
	static SInt32	s_nOffsetFromUTC = 0;

	if ( s_Cached < 0 )
	{
		s_Cached = 1;
#ifdef _WIN32
		DWORD					dwRet;
		TIME_ZONE_INFORMATION	tzi;
		dwRet = ::GetTimeZoneInformation( &tzi );
		if ( dwRet != TIME_ZONE_ID_INVALID )
		{
			s_nOffsetFromUTC = tzi.Bias;
			if ( dwRet == TIME_ZONE_ID_STANDARD )
			{
				s_nOffsetFromUTC += tzi.StandardBias;
			}
			else if ( dwRet == TIME_ZONE_ID_DAYLIGHT )
			{
				s_nOffsetFromUTC += tzi.DaylightBias;
			}
			s_nOffsetFromUTC *= 60;
		}
#else
		const time_t        t   = time( 0 );
		const struct tm*    ptm = localtime( &t );
		s_nOffsetFromUTC = -ptm->tm_gmtoff;
#endif
	}

	return s_nOffsetFromUTC;
}
