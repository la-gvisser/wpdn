// ============================================================================
//	TimeConv.h
// ============================================================================
//	Coypright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Coypright (c) AG Group, Inc. 1996-2000. All rights reserved.

#ifndef TIMECONV_H
#define TIMECONV_H

#include "AGTypes.h"
#include <time.h>
#include <math.h>

namespace TimeConv
{
#pragma pack(push,1)
typedef union
{
	struct
	{
		UInt16	Second : 5; 	// Second divided by 2.
		UInt16	Minute : 6; 	// Minute (0-59).
		UInt16	Hour : 5;		// Hour (0-23).
	} u;
	UInt16	WordPart;
} DOSTIME;

typedef union
{
	struct
	{
		UInt16	Day : 5;		// Day of month (1-31).
		UInt16	Month : 4;		// Month (1 = Jan, etc.).
		UInt16	Year : 7;		// Year offset from 1980.
	} u;
	UInt16	WordPart;
} DOSDATE;
#pragma pack(pop)

	// Ansi time base: Jan 1 1970
	// Mac time base: Jan 1 1904
	// AFP time base: Jan 1 2000
	// Peek time base: Jan 1 1601
	const UInt32 TIMECONV_MAC_TO_ANSI = 2082844800L;
	const UInt32 TIMECONV_ANSI_TO_AFP = 946710000L;
	const UInt32 TIMECONV_MAC_TO_AFP = 3029529600L;
	const UInt64 TIMECONV_FILE_TO_ANSI = 11644473600ull; // Seconds between 1/1/1601 and 1/1/1970 (see KB167296: http://support.microsoft.com/default.aspx?scid=KB;en-us;q167296).
	const UInt64 TIMECONV_PEEK_TO_ANSI = TIMECONV_FILE_TO_ANSI;
	const UInt64 TIMECONV_PEEK_TO_MICROPEEK = 95616288000000000ull; // 100 nanosecond units between 1/1/1601 and 1/1/1904.

	inline UInt64	FileToPeek( UInt64 nFileTime );
	inline UInt64 	PeekToFile( UInt64 nPeekTime );

	inline UInt64	AnsiToPeek( time_t nAnsiTime );
	inline time_t	PeekToAnsi( UInt64 nPeekTime );

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

	inline UInt64	OleToPeek( DATE nOleTime );
	inline DATE		PeekToOle( UInt64 nPeekTime );
};


// ----------------------------------------------------------------------------
//		FileToPeek
// ----------------------------------------------------------------------------

UInt64
TimeConv::FileToPeek(
	UInt64	nFileTime )
{
	// Now in 100-nanosecond units since Jan 1 1601.
	nFileTime = nFileTime * 100;
	// Now in nanoseconds since Jan 1 1601.

	return nFileTime;
}


// ----------------------------------------------------------------------------
//		PeekToFile
// ----------------------------------------------------------------------------

UInt64
TimeConv::PeekToFile(
	UInt64	nPeekTime )
{
	// Now in nanoseconds since Jan 1 1601.
	nPeekTime = nPeekTime / 100;
	// Now in 100-nanosecond units since Jan 1 1601.

	return nPeekTime;
}


// ----------------------------------------------------------------------------
//		AnsiToPeek
// ----------------------------------------------------------------------------

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


// ----------------------------------------------------------------------------
//		PeekToAnsi
// ----------------------------------------------------------------------------

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


// ----------------------------------------------------------------------------
//		AnsiToFile
// ----------------------------------------------------------------------------

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


// ----------------------------------------------------------------------------
//		FileToAnsi
// ----------------------------------------------------------------------------

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


// ----------------------------------------------------------------------------
//		AnsiToMac
// ----------------------------------------------------------------------------

UInt32
TimeConv::AnsiToMac(
	time_t	nAnsiTime )
{
	return (UInt32)(nAnsiTime + TIMECONV_MAC_TO_ANSI);
}


// ----------------------------------------------------------------------------
//		MacToAnsi
// ----------------------------------------------------------------------------

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


// ----------------------------------------------------------------------------
//		AnsiToAFP
// ----------------------------------------------------------------------------

UInt32
TimeConv::AnsiToAFP(
	time_t	nAnsiTime )
{
	return (UInt32)(nAnsiTime + TIMECONV_ANSI_TO_AFP);
}


// ----------------------------------------------------------------------------
//		AFPToAnsi
// ----------------------------------------------------------------------------

time_t
TimeConv::AFPToAnsi(
	UInt32	nAFPTime )
{
	return (time_t)(nAFPTime + TIMECONV_ANSI_TO_AFP);
}


// ----------------------------------------------------------------------------
//		AnsiToDos
// ----------------------------------------------------------------------------

UInt32
TimeConv::AnsiToDos(
	time_t	nAnsiTime )
{
	struct tm*	pAnsiTm = localtime( &nAnsiTime );
	if ( pAnsiTm == NULL ) return 0;

	DOSDATE theDate;
	theDate.u.Year = pAnsiTm->tm_year - 80;
	theDate.u.Month = pAnsiTm->tm_mon + 1;
	theDate.u.Day = pAnsiTm->tm_mday;

	DOSTIME theTime;
	theTime.u.Hour = pAnsiTm->tm_hour;
	theTime.u.Minute = pAnsiTm->tm_min;
	theTime.u.Second = pAnsiTm->tm_sec / 2;

	return MAKELONG( theTime.WordPart, theDate.WordPart );
}


// ----------------------------------------------------------------------------
//		DosToAnsi
// ----------------------------------------------------------------------------

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
	theAnsiTm.tm_sec = theTime.u.Second * 2;
	theAnsiTm.tm_min = theTime.u.Minute;
	theAnsiTm.tm_hour = theTime.u.Hour;
	theAnsiTm.tm_mday = theDate.u.Day;
	theAnsiTm.tm_mon = theDate.u.Month;
	theAnsiTm.tm_mon -= 1;
	theAnsiTm.tm_year = theDate.u.Year;
	theAnsiTm.tm_year += 80;
	theAnsiTm.tm_wday = 0;
	theAnsiTm.tm_yday = 0;
	theAnsiTm.tm_isdst = -1;	// -1 is "use system to compute if DST applies", 0 is "don't use DST"

	return mktime( &theAnsiTm );
}

// ----------------------------------------------------------------------------
//		MicroPeekToPeek
// ----------------------------------------------------------------------------

UInt64
TimeConv::MicroPeekToPeek(
	UInt64	nMicroPeekTime )
{
	UInt64	nAdjustedTime = nMicroPeekTime;
	// Now in microseconds since 1/1/1904 local time
	nAdjustedTime *= 10;
	// Now in 100 nanosecond units since 1/1/1904 local time.
	nAdjustedTime += TIMECONV_PEEK_TO_MICROPEEK;
	// Now in 100 nanosecond units since 1/1/1601 local time.
	UInt64	nPeekTime;
	::LocalFileTimeToFileTime( (FILETIME*) &nAdjustedTime, (FILETIME*) &nPeekTime );
	// Now in 100 nanosecond units since 1/1/1601 UTC time.
	nPeekTime *= 100;
	// Now in nanoseconds since 1/1/1601 UTC time.

	return ( nPeekTime );
}


// ----------------------------------------------------------------------------
//		PeekToMicroPeek
// ----------------------------------------------------------------------------

UInt64
TimeConv::PeekToMicroPeek(
	UInt64	nPeekTime )
{
	UInt64	nOriginalTime = nPeekTime;
	// Now in nanoseconds since 1/1/1601 UTC time.
	nOriginalTime /= 100;
	// Now in 100 nanosecond units since 1/1/1601 UTC time.
	UInt64	nAdjustedTime;
	::FileTimeToLocalFileTime( (FILETIME *) &nOriginalTime, (FILETIME *) &nAdjustedTime );
	// Now in 100 nanosecond units since 1/1/1601 local time.
	nAdjustedTime -= TIMECONV_PEEK_TO_MICROPEEK;
	// Now in 100 nanosecond units since 1/1/1904 local time.
	nAdjustedTime /= 10;
	// Now in microseconds since 1/1/1904 local time.

	return ( nAdjustedTime );
}


// ----------------------------------------------------------------------------
//		OleToPeek
// ----------------------------------------------------------------------------

UInt64
TimeConv::OleToPeek(
	DATE	nOleTime )
{
	// Get the integer and fractional parts of the OLE time.
	double	i, f;
	f = modf( nOleTime, &i );

	// Convert OLE time to PEEK time without the fractional day.
	SYSTEMTIME	st;
	VERIFY( ::VariantTimeToSystemTime( i, &st ) );
	UInt64	nFileTime;
	VERIFY( ::SystemTimeToFileTime( &st, (FILETIME*) &nFileTime ) );
	UInt64	nPeekTime = TimeConv::FileToPeek( nFileTime );

	// Add the fractional day in nanoseconds.
	nPeekTime += (SInt64)(f * 86400000000000.0);

	return nPeekTime;
}


// ----------------------------------------------------------------------------
//		PeekToOle
// ----------------------------------------------------------------------------

DATE
TimeConv::PeekToOle(
	UInt64	nPeekTime )
{
	// Convert PEEK time to OLE time without fractional seconds.
	UInt64		nFileTime = TimeConv::PeekToFile( nPeekTime );
	SYSTEMTIME	st;
	VERIFY( ::FileTimeToSystemTime( (FILETIME*) &nFileTime, &st ) );
	st.wMilliseconds = 0;	// TODO: zero hours, minutes, seconds too?
	DATE		nOleTime;
	VERIFY( ::SystemTimeToVariantTime( &st, &nOleTime ) != 0 );

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

#endif /* TIMECONV_H */
