// ============================================================================
//	ut_peektime.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#include "StdAfx.h"
#include "PeekTime.h"
#include "stdlib.h"
#include <iostream>
#include <iomanip>


int
UT_PeekTime()
{
	int		nFailures( 0 );

	SInt64		nLocalOffset = CPeekTime::GetLocalTimeOffset();
	SInt64		nLocalOffsetSeconds = nLocalOffset / WPTIME_SECONDS;
	SInt64		nLocalOffsetMinutes = nLocalOffsetSeconds / 60;

	CPeekTime	pt( true );
	CPeekString	strTime = pt.Format( kNanoseconds );
	CPeekTime	ptCalc( strTime );
	if ( pt != ptCalc ) {
		nFailures++;
	}

	CPeekTimeLocal	ptl( true );
	CPeekString		strTimeLocal = ptl.Format( kNanoseconds );
	CPeekTime		ptlCalc( strTimeLocal );
	if ( ptl != ptlCalc ) {
		nFailures++;
	}

	CPeekTimeLocal	ptlCast( pt );
	CPeekString		strCast = ptlCast.Format();

	time_t	now = time( nullptr );
	CPeekTime nowPeek( true );
	CPeekTime peek( now );
	CPeekTime peekNow( time( nullptr ) );

	time_t soon( peek );
	if ( soon != now ) {
		soon = 0;
	}

	nowPeek.SetUnits( kYears, 1999 );
	CPeekString strPeek1( nowPeek.Format( kNanoseconds ) );
	nowPeek.SetUnits( kDays, 1 );
	CPeekString strPeek2( nowPeek.Format( kNanoseconds ) );
	nowPeek.SetUnits( kHours, 12 );
	CPeekString strPeek3( nowPeek.Format( kNanoseconds ) );
	nowPeek.SetUnits( kMilliseconds, 345, true );
	CPeekString strPeek4( nowPeek.Format( kNanoseconds ) );

	return nFailures;
}
