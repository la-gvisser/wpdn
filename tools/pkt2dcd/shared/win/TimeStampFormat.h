// =============================================================================
//	TimeStampFormat.h
// =============================================================================
//	Copyright (c) 2000-2007 WildPackets, Inc. All rights reserved.
//	Copyright (c) 1996-2000 AG Group, Inc. All rights reserved.

#pragma once

#include "AGTypes.h"

namespace TimeStampFormat
{

enum Format
{
	kFormat_Milliseconds,
	kFormat_Microseconds,
	kFormat_Nanoseconds,
	kFormat_Seconds
};

extern const Format	DEFAULT;

// for programmatic keys, not user-visible text
CString	FormatToString( const Format inFormat );
Format  StringToFormat( const CString & inString );

HRESULT
TimeStampToDateString(
	IN  UInt64	ts,
	OUT LPTSTR	lpszStr,
	IN  int		cchStr,
	IN	bool	bLocalTime );

HRESULT
TimeStampToTimeString(
	IN  UInt64	ts,
	IN  Format	fmt,
	OUT LPTSTR	lpszStr,
	IN  int		cchStr,
	IN	bool	bLocalTime );

HRESULT
DeltaTimeStampToString(
	IN  UInt64	ts1,
	IN  UInt64	ts2,
	IN  Format	fmt,
	IN  LPCTSTR	lpszDaysFmt,
	OUT LPTSTR	lpszStr,
	IN  int		cchStr );

HRESULT
DurationToString(
	IN  SInt64	duration,
	IN  Format	fmt,
	IN  LPCTSTR	lpszDaysFmt,
	OUT LPTSTR	lpszStr,
	IN  int		cchStr );

HRESULT
TimeStampToISO8601String(
	IN  UInt64	ts,
	IN  Format	fmt,
	OUT LPTSTR	lpszStr,
	IN  int		cchStr );

HRESULT
ISO8601StringToTimeStamp(
	IN  LPCTSTR	lpszStr,
	OUT UInt64*	ts );

Format
CalcDynamicDurationFormat(
	IN  UInt64	duration );

} /* namespace TimeStampFormat */
