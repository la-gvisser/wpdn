// =============================================================================
//	TimeStamp.h
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2005. All rights reserved.
//	Copyright (c) AG Group, Inc. 1996-2000. All rights reserved.

#ifndef TIMESTAMP_H
#define	TIMESTAMP_H

#ifndef _WIN32
#include <sys/time.h>
#endif
#include "WPTypes.h"
#include "TimeConv.h"

namespace PeekTimeStamp
{

const UInt64	kTimeStamp_Invalid = 0xffffffffffffffffull;	// ULLONG_MAX

inline UInt64
GetTimeStamp()
{
#ifdef _WIN32
	UInt64		ts;
	::GetSystemTimeAsFileTime( (LPFILETIME) &ts );
	return TimeConv::FileToPeek( ts );
#else
	struct timeval	tv;
	struct timezone	tz;
	gettimeofday( &tv, &tz );
	return TimeConv::TimevalToPeek( tv.tv_sec, tv.tv_usec );
#endif
}

inline UInt64
ConvertOldTimeStamp(
	UInt32	inOldTimeStamp,
	UInt32	inBaseTime = 0 )
{
	return (UInt64)((1000000ull * inBaseTime) + (1000ull * inOldTimeStamp));
}

inline UInt64
ToSeconds(
	UInt64	ts )
{
	return (ts / RESOLUTION_PER_SECOND);
}

inline UInt64
ToMilliseconds(
	UInt64	ts )
{
	return (ts / (RESOLUTION_PER_SECOND/1000));
}

inline UInt64
ToMicroseconds(
	UInt64	ts )
{
	return (ts / (RESOLUTION_PER_SECOND/1000000));
}

inline UInt64
ToNanoseconds(
	UInt64	ts )
{
	return (ts / (RESOLUTION_PER_SECOND/1000000000));
}

inline SInt64
ToSeconds(
	SInt64	ts )
{
	return (ts / RESOLUTION_PER_SECOND);
}

inline SInt64
ToMilliseconds(
	SInt64	ts )
{
	return (ts / (RESOLUTION_PER_SECOND/1000));
}

inline SInt64
ToMicroseconds(
	SInt64	ts )
{
	return (ts / (RESOLUTION_PER_SECOND/1000000));
}

inline SInt64
ToNanoseconds(
	SInt64	ts )
{
	return (ts / (RESOLUTION_PER_SECOND/1000000000));
}

} /* namespace PeekTimeStamp */

#endif
