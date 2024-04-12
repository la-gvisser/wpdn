// =============================================================================
//	Utils.cpp
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "stdafx.h"
#include "Utils.h"

#if TARGET_OS_WIN32
#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif
#endif


// =============================================================================
//		Constants and Data Structures
// =============================================================================

typedef union _LONGTIME {
	UInt64		n;
	FILETIME	ft;
} LONGTIME;

LONGTIME	g_ltLastTime;

const	UInt32	kSecondsInAMinute( 60 );
const	UInt32	kSecondsInAnHour( 60 * kSecondsInAMinute );
const	UInt32	kSecondsInADay( 24 * kSecondsInAnHour );

const	UInt64	kKilobyte( 1024 );
const	UInt64	kMegabyte( 1024 * 1024 );
const	UInt64	kGigabyte( 1024 * 1024 * 1024 );

const	UInt64	kFileTimeInASecond( 10000000 );


// -----------------------------------------------------------------------------
//		GetPeekTimeStamp
// -----------------------------------------------------------------------------

UInt64
GetPeekTimeStamp()
{
	UInt64		thePeekTime( GetTimeStamp() );
	thePeekTime *= 100;
	return thePeekTime;
}


// -----------------------------------------------------------------------------
//		GetTimeStamp
// -----------------------------------------------------------------------------

UInt64
GetTimeStamp()
{
	LONGTIME	uTime;

	::GetSystemTimeAsFileTime( &uTime.ft );
	return uTime.n;
}


// -----------------------------------------------------------------------------
//		GetTimeStringFileName
// -----------------------------------------------------------------------------

CString
GetTimeStringFileName()
{
	LONGTIME	ltTime;

	::GetSystemTimeAsFileTime( &ltTime.ft );
	int x = 0;
	while ( ltTime.n == g_ltLastTime.n ) {
		Sleep( 1 );
		::GetSystemTimeAsFileTime( &ltTime.ft );
		if ( x++ > 1000 ) throw -1;
	}
	g_ltLastTime = ltTime;

	SYSTEMTIME	stTime;
	::FileTimeToSystemTime( &ltTime.ft, &stTime );
	// ISO 8601 - as compliant as we can.
	//    periods instead of colons
	// 25 characters: 'YYYY-MM-DDTHH.MM.SS.mmmZ'

	CString	strTime;
	strTime.Format(
		_T("%4.4u-%2.2u-%2.2uT%2.2u.%2.2u.%2.2u.%3.3uZ"),
		stTime.wYear,
		stTime.wMonth,
		stTime.wDay,
		stTime.wHour,
		stTime.wMinute,
		stTime.wSecond,
		stTime.wMilliseconds );

	return strTime;
}
