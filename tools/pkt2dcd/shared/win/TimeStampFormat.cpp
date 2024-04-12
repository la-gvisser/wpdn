// ============================================================================
//	TimeStampFormat.cpp
// ============================================================================
//	Copyright (c) 2000-2007 WildPackets, Inc. All rights reserved.
//	Copyright (c) 1996-2000 AG Group, Inc. All rights reserved.

#include "StdAfx.h"
#include "TimeConv.h"
#include "TimeStamp.h"
#include "TimeStampFormat.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

namespace TimeStampFormat
{

// PRODUCT TODO: for does this need to be fixed somehow?
#if (PRODUCT & GIG)
const Format	DEFAULT = kFormat_Nanoseconds;
#else
const Format	DEFAULT = kFormat_Microseconds;
#endif

/// programmatic keys for logging and XML output
static const CString
s_strFormat[] =
{
	_T( "milliseconds" ),
	_T( "microseconds" ),
	_T( "nanoseconds" ),
	_T( "seconds" )
};
	
// ----------------------------------------------------------------------------
//		FormatToString
// ----------------------------------------------------------------------------
/// Return a string like "milliseconds" or "microseconds".
/// Counterpart to @see StringToFormat().

CString
FormatToString(
	const Format inFormat )
{
	const int nIndex = (int) inFormat;
	if (( 0 <= nIndex ) && ( nIndex < COUNTOF( s_strFormat ) ))
	{
		return s_strFormat[ nIndex ];
	}
	return CString();
}


// ----------------------------------------------------------------------------
//		StringToFormat
// ----------------------------------------------------------------------------
/// Convert a string like "milliseconds" or "microseconds" to a Format enum.
/// Counterpart to @see FormatToString().

Format
StringToFormat( const CString & inString )
{
	for ( int i = 0; i < COUNTOF( s_strFormat ); ++i )
	{
		if ( 0 == s_strFormat[ i ].Compare( inString ) ) return (Format) i;
	}
	return DEFAULT;
}

// ----------------------------------------------------------------------------
//		TimeStampToDateString
// ----------------------------------------------------------------------------
// Formats a 64-bit Peek time into a date string, format dependent on flags.

HRESULT
TimeStampToDateString(
	UInt64	ts,
	LPTSTR	lpszStr,
	int		/*cchStr*/,
	bool	bLocalTime )
{
	// Sanity checks.
	if ( lpszStr == NULL ) return E_POINTER;

	// Clear the string.
	lpszStr[0] = 0;

	// Get the time in system time format.
	UInt64		ft = TimeConv::PeekToFile( ts );
	SYSTEMTIME	st;
	if ( !::FileTimeToSystemTime( (FILETIME*)&ft, &st ) )
	{
		DWORD dwResult = ::GetLastError();
		return HRESULT_FROM_WIN32(dwResult);
	}

	// Convert to local system time if flag set. Note: can't use FileTimeToLocalFileTime
	// because that function uses the current settings for daylight savings time.
	SYSTEMTIME	lst;
	if ( bLocalTime )
	{
		if ( !::SystemTimeToTzSpecificLocalTime( NULL, &st, &lst ) )
		{
			DWORD dwResult = ::GetLastError();
			return HRESULT_FROM_WIN32(dwResult);
		}
	}
	else 
	{
		lst = st;
	}

	// Format the date string.
	if ( ::GetDateFormat( LOCALE_USER_DEFAULT, DATE_SHORTDATE,
		&lst, NULL, lpszStr, 256 ) == 0 )
	{
		DWORD dwResult = ::GetLastError();
		return HRESULT_FROM_WIN32(dwResult);
	}

	return S_OK;
}


// ----------------------------------------------------------------------------
//		TimeStampToTimeString
// ----------------------------------------------------------------------------
// Formats a 64-bit Peek time into a time string, format dependent on flags.

HRESULT
TimeStampToTimeString(
	UInt64	ts,
	Format	fmt,
	LPTSTR	lpszStr,
	int		/*cchStr*/,
	bool	bLocalTime)
{
	// Sanity checks.
	if ( lpszStr == NULL ) return E_POINTER;

	// Clear the string.
	lpszStr[0] = 0;

	// Get the time in system time format.
	UInt64		ft = TimeConv::PeekToFile( ts );
	SYSTEMTIME	st;
	if ( !::FileTimeToSystemTime( (FILETIME*)&ft, &st ) )
	{
		DWORD dwResult = ::GetLastError();
		return HRESULT_FROM_WIN32(dwResult);
	}

	// Convert to local system time if flag set. Note: can't use FileTimeToLocalFileTime
	// because that function uses the current settings for daylight savings time.
	SYSTEMTIME	lst;
	if ( bLocalTime )
	{
		if ( !::SystemTimeToTzSpecificLocalTime( NULL, &st, &lst ) )
		{
			DWORD dwResult = ::GetLastError();
			return HRESULT_FROM_WIN32(dwResult);
		}
	}
	else
	{
		lst = st;
	}

	// Format the time string.
	TCHAR	szTime[256];
	if ( ::GetTimeFormat( LOCALE_USER_DEFAULT,
		TIME_FORCE24HOURFORMAT | TIME_NOTIMEMARKER,
		&lst, NULL, szTime, 256 ) == 0 )
	{
		DWORD dwResult = ::GetLastError();
		return HRESULT_FROM_WIN32(dwResult);
	}

	switch ( fmt )
	{
		case kFormat_Milliseconds:
		{
			// Get the decimal separator.
			TCHAR	szDecimalSeparator[8];
			if ( ::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SDECIMAL,
				szDecimalSeparator, 8 ) == 0 )
			{
				// GetLocaleInfo failed!
				DWORD dwResult = ::GetLastError();
				return HRESULT_FROM_WIN32(dwResult);
			}

			// Note: the millisecond value is truncated, NOT rounded on
			// purpose ('cause that's not the way clocks display time!).
#if RESOLUTION_PER_SECOND != 1000000000
#error "The following calculation is incorrect!"
#endif
			UInt32	nMilliseconds = (UInt32)((ts % RESOLUTION_PER_SECOND) / 1000000);

			// Build the full time string.
			_stprintf( lpszStr, _T("%s%s%3.3lu"),
				szTime, szDecimalSeparator, nMilliseconds );
		}
		break;

		case kFormat_Microseconds:
		{
			// Get the decimal separator.
			TCHAR	szDecimalSeparator[8];
			if ( ::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SDECIMAL,
				szDecimalSeparator, 8 ) == 0 )
			{
				// GetLocaleInfo failed!
				DWORD dwResult = ::GetLastError();
				return HRESULT_FROM_WIN32(dwResult);
			}

			// Note: the microsecond value is truncated, NOT rounded on
			// purpose ('cause that's not the way clocks display time!).
#if RESOLUTION_PER_SECOND != 1000000000
#error "The following calculation is incorrect!"
#endif
			UInt32	nMicroseconds = (UInt32)((ts % RESOLUTION_PER_SECOND) / 1000);

			// Build the full time string.
			_stprintf( lpszStr, _T("%s%s%6.6lu"),
				szTime, szDecimalSeparator, nMicroseconds );
		}
		break;

		case kFormat_Nanoseconds:
		{
			// Get the decimal separator.
			TCHAR	szDecimalSeparator[8];
			if ( ::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SDECIMAL,
				szDecimalSeparator, 8 ) == 0 )
			{
				// GetLocaleInfo failed!
				DWORD dwResult = ::GetLastError();
				return HRESULT_FROM_WIN32(dwResult);
			}

			// Get the nanoseconds remaining.
			UInt32	nNanoseconds = (UInt32)(ts % RESOLUTION_PER_SECOND);

			// Build the full time string.
			_stprintf( lpszStr, _T("%s%s%9.9lu"),
				szTime, szDecimalSeparator, nNanoseconds );
		}
		break;

		case kFormat_Seconds:
		{
			// Use the straight time string.
			_tcscpy( lpszStr, szTime );
		}
		break;
	}

	// Add 'Z' to time if UTC time.
	if ( !bLocalTime )
	{
		_tcscat( lpszStr, _T("Z") );
	}

	return S_OK;
}


// ----------------------------------------------------------------------------
//		DeltaTimeStampToString
// ----------------------------------------------------------------------------

HRESULT
DeltaTimeStampToString(
	UInt64	ts1,
	UInt64	ts2,
	Format	fmt,
	LPCTSTR	lpszDaysFmt,
	LPTSTR	lpszStr,
	int		cchStr )
{
	// Sanity checks.
	if ( lpszDaysFmt == NULL ) return E_POINTER;
	if ( lpszStr == NULL ) return E_POINTER;

	// Subtract the two time stamps.
	SInt64	d = ts2 - ts1;

	if ( fmt == kFormat_Milliseconds )
	{
		// Round the value for milliseconds.
		// Note: rounding is used instead of truncation
		// because this is a time difference.
		if ( d > 0 )
		{
			d = ((d / 1000000i64) * 1000000i64) + (((d % 1000000i64) >= 500000i64) * 1000000i64);
		}
		else
		{
			d = ((d / 1000000i64) * 1000000i64) - (((d % 1000000i64) <= -500000i64) * 1000000i64);
		}
	}
	else if ( fmt == kFormat_Microseconds )
	{
		// Round the value for microseconds.
		// Note: rounding is used instead of truncation
		// because this is a time difference.
		if ( d > 0 )
		{
			d = ((d / 1000i64) * 1000i64) + (((d % 1000i64) >= 500i64) * 1000i64);
		}
		else
		{
			d = ((d / 1000i64) * 1000i64) - (((d % 1000i64) <= -500i64) * 1000i64);
		}
	}

	return DurationToString( d, fmt, lpszDaysFmt, lpszStr, cchStr );
}


// ----------------------------------------------------------------------------
//		DurationToString
// ----------------------------------------------------------------------------

HRESULT
DurationToString(
	SInt64	duration,
	Format	fmt,
	LPCTSTR	lpszDaysFmt,
	LPTSTR	lpszStr,
	int		cchStr )
{
	// Sanity checks.
	if ( lpszDaysFmt == NULL ) return E_POINTER;
	if ( lpszStr == NULL ) return E_POINTER;

	// Start with an empty string.
	lpszStr[0] = 0;

	// Get the sign.
	if ( duration < 0 )
	{
		// Get the negative sign.
		if ( ::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SNEGATIVESIGN,
			lpszStr, cchStr ) == 0 )
		{
			// GetLocaleInfo failed!
			DWORD dwResult = ::GetLastError();
			return HRESULT_FROM_WIN32(dwResult);
		}

		// Skip over the sign (kinda gross).
		lpszStr += _tcslen( lpszStr );

		// Use the absolute value.
		duration = -duration;
	}

	// Convert to seconds.
	UInt32	t = (UInt32)(duration / 1000000000i64);

	// Get the remainder (nanoseconds).
	UInt32	nNanoseconds = (UInt32)(duration - (t * 1000000000i64));

	// Get the number of days, hours, minutes and seconds.
	int		nDays = t / (24 * 60 * 60);
	t = t % (24 * 60 * 60);
	int		nHours = t / (60 * 60);
	t = t % (60 * 60);
	int		nMinutes = t / 60;
	int		nSeconds = t % 60;

	switch ( fmt )
	{
		case kFormat_Milliseconds:
		case kFormat_Microseconds:
		case kFormat_Nanoseconds:
		{
			// Get the decimal separator.
			TCHAR	szDecimalSeparator[8];
			if ( ::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SDECIMAL,
				szDecimalSeparator, 8 ) == 0 )
			{
				// GetLocaleInfo failed!
				DWORD dwResult = ::GetLastError();
				return HRESULT_FROM_WIN32(dwResult);
			}

			// Format the fraction seconds.
			TCHAR	szFraction[32];
			if ( fmt == kFormat_Milliseconds )
			{
				// Note: the millisecond value is truncated, NOT rounded.
				UInt32	nMilliseconds = nNanoseconds / 1000000L;
				_stprintf( szFraction, _T("%3.3lu"), nMilliseconds );
			}
			else if ( fmt == kFormat_Microseconds )
			{
				// Note: the microsecond value is truncated, NOT rounded.
				UInt32	nMicroseconds = nNanoseconds / 1000L;
				_stprintf( szFraction, _T("%6.6lu"), nMicroseconds );
			}
			else
			{
				_stprintf( szFraction, _T("%9.9lu"), nNanoseconds );
			}

			if ( nDays > 0 )
			{
				// Format the hours:minutes:seconds.
				SYSTEMTIME	st;
				memset( &st, 0, sizeof(st) );
				st.wHour   = (WORD) nHours;
				st.wMinute = (WORD) nMinutes;
				st.wSecond = (WORD) nSeconds;
				TCHAR	szHMS[128];
				if ( ::GetTimeFormat( LOCALE_USER_DEFAULT,
					TIME_FORCE24HOURFORMAT | TIME_NOTIMEMARKER,
					&st, NULL, szHMS, 128 ) == 0 )
				{
					DWORD dwResult = ::GetLastError();
					return HRESULT_FROM_WIN32(dwResult);
				}

				// Build the full time string.
				TCHAR	szTime[256];
				_stprintf( szTime, _T("%s%s%s"),
					szHMS, szDecimalSeparator, szFraction );

				// Format the days string.
				TCHAR	szDays[32];
				_stprintf( szDays, _T("%d"), nDays );

				// Build the final result string.
				_stprintf( lpszStr, lpszDaysFmt, szDays, szTime );
			}
			else if ( nHours > 0 )
			{
				// Format the hours:minutes:seconds.
				SYSTEMTIME	st;
				memset( &st, 0, sizeof(st) );
				st.wHour   = (WORD) nHours;
				st.wMinute = (WORD) nMinutes;
				st.wSecond = (WORD) nSeconds;
				TCHAR	szHMS[128];
				if ( ::GetTimeFormat( LOCALE_USER_DEFAULT,
					TIME_FORCE24HOURFORMAT | TIME_NOTIMEMARKER,
					&st, NULL, szHMS, 128 ) == 0 )
				{
					DWORD dwResult = ::GetLastError();
					return HRESULT_FROM_WIN32(dwResult);
				}

				// Build the full time string.
				_stprintf( lpszStr, _T("%s%s%s"),
					szHMS, szDecimalSeparator, szFraction );
			}
			else if ( nMinutes > 0 )
			{
				// TODO: would like to knock off the hours from this
				// format, but I don't see how without digging into
				// GetTimeFormat formatting strings (too hard).

				// Format the hours:minutes:seconds.
				SYSTEMTIME	st;
				memset( &st, 0, sizeof(st) );
				st.wHour   = (WORD) nHours;
				st.wMinute = (WORD) nMinutes;
				st.wSecond = (WORD) nSeconds;
				TCHAR	szHMS[128];
				if ( ::GetTimeFormat( LOCALE_USER_DEFAULT,
					TIME_FORCE24HOURFORMAT | TIME_NOTIMEMARKER,
					&st, NULL, szHMS, 128 ) == 0 )
				{
					DWORD dwResult = ::GetLastError();
					return HRESULT_FROM_WIN32(dwResult);
				}

				// Build the full time string.
				_stprintf( lpszStr, _T("%s%s%s"),
					szHMS, szDecimalSeparator, szFraction );
			}
			else
			{
				// Build the full time string.
				_stprintf( lpszStr, _T("%1.1lu%s%s"),
					nSeconds, szDecimalSeparator, szFraction );
			}
		}
		break;

		case kFormat_Seconds:
		{
			if ( nDays > 0 )
			{
				// Format the hours:minutes:seconds.
				SYSTEMTIME	st;
				memset( &st, 0, sizeof(st) );
				st.wHour   = (WORD) nHours;
				st.wMinute = (WORD) nMinutes;
				st.wSecond = (WORD) nSeconds;
				TCHAR	szHMS[128];
				if ( ::GetTimeFormat( LOCALE_USER_DEFAULT,
					TIME_FORCE24HOURFORMAT | TIME_NOTIMEMARKER,
					&st, NULL, szHMS, 128 ) == 0 )
				{
					DWORD dwResult = ::GetLastError();
					return HRESULT_FROM_WIN32(dwResult);
				}

				// Format the days string.
				TCHAR	szDays[32];
				_stprintf( szDays, _T("%d"), nDays );

				// Build the final result string.
				_stprintf( lpszStr, lpszDaysFmt, szDays, szHMS );
			}
			else
			{
				// Format the hours:minutes:seconds.
				SYSTEMTIME	st;
				memset( &st, 0, sizeof(st) );
				st.wHour   = (WORD) nHours;
				st.wMinute = (WORD) nMinutes;
				st.wSecond = (WORD) nSeconds;
				if ( ::GetTimeFormat( LOCALE_USER_DEFAULT,
					TIME_FORCE24HOURFORMAT | TIME_NOTIMEMARKER,
					&st, NULL, lpszStr, cchStr ) == 0 )
				{
					DWORD dwResult = ::GetLastError();
					return HRESULT_FROM_WIN32(dwResult);
				}
			}
		}
		break;
	}

	return S_OK;
}


// ----------------------------------------------------------------------------
//		TimeStampToISO8601String
// ----------------------------------------------------------------------------

HRESULT
TimeStampToISO8601String(
	UInt64	ts,
	Format  fmt,
	LPTSTR  lpszStr,
	int     /*cchStr*/ )
{
	// Sanity checks.
	if ( lpszStr == NULL ) return E_POINTER;

	// Clear the string.
	lpszStr[0] = 0;

	// Get the time in system time format.
	UInt64		ft = TimeConv::PeekToFile( ts );
	SYSTEMTIME	st;
	if ( ::FileTimeToSystemTime( (FILETIME*)&ft, &st ) == 0 )
	{
		DWORD dwResult = ::GetLastError();
		return HRESULT_FROM_WIN32(dwResult);
	}

	switch ( fmt )
	{
		case kFormat_Milliseconds:
		{
			UInt32	nMilliseconds = (UInt32)((ts % RESOLUTION_PER_SECOND) / 1000000);

			// Time is ISO 8601 format: CCYY-MM-DDThh:mm:ss.sssZ
			// C = century; Y = year; M = month, D = day,
			// h = hour, m = minute, s = second. Z indicates UTC.
			_stprintf( lpszStr, _T("%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%3.3uZ"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, nMilliseconds );
		}
		break;

		case kFormat_Microseconds:
		{
			UInt32	nMicroseconds = (UInt32)((ts % RESOLUTION_PER_SECOND) / 1000);

			// Time is ISO 8601 format: CCYY-MM-DDThh:mm:ss.ssssssZ
			// C = century; Y = year; M = month, D = day,
			// h = hour, m = minute, s = second. Z indicates UTC.
			_stprintf( lpszStr, _T("%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%6.6uZ"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, nMicroseconds );
		}
		break;

		case kFormat_Nanoseconds:
		{
			UInt32	nNanoseconds = (UInt32)(ts % RESOLUTION_PER_SECOND);

			// Time is ISO 8601 format: CCYY-MM-DDThh:mm:ss.sssssssssZ
			// C = century; Y = year; M = month, D = day,
			// h = hour, m = minute, s = second. Z indicates UTC.
			_stprintf( lpszStr, _T("%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9uZ"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, nNanoseconds );
		}
		break;

		case kFormat_Seconds:
		{
			// Time is ISO 8601 format: CCYY-MM-DDThh:mm:ssZ
			// C = century; Y = year; M = month, D = day,
			// h = hour, m = minute, s = second. Z indicates UTC.
			_stprintf( lpszStr, _T("%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2dZ"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
		}
		break;
	}

	return S_OK;
}


// ----------------------------------------------------------------------------
//		ISO8601StringToTimeStamp
// ----------------------------------------------------------------------------

HRESULT
ISO8601StringToTimeStamp(
	LPCTSTR	lpszStr,
	UInt64*	ts )
{
	// Sanity checks.
	if ( lpszStr == NULL ) return E_POINTER;
	if ( ts == NULL ) return E_POINTER;

	// Initialize OUT params.
	*ts = 0;

	// ISO 8601 format: CCYY-MM-DDThh:mm:ss.sssssssssZ
	int	year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
	int	n = _stscanf( lpszStr, _T("%d-%d-%dT%d:%d:%d"),
				&year, &month, &day, &hour, &minute, &second );
	if ( n != 6 ) return E_FAIL;

	// Convert to filetime.
	SYSTEMTIME	st;
	memset( &st, 0, sizeof(st) );
	st.wYear   = (WORD) year;
	st.wMonth  = (WORD) month;
	st.wDay    = (WORD) day;
	st.wHour   = (WORD) hour;
	st.wMinute = (WORD) minute;
	st.wSecond = (WORD) second;
	st.wMilliseconds = 0;
	if ( !::SystemTimeToFileTime( &st, (FILETIME*) ts ) )
	{
		DWORD dwResult = ::GetLastError();
		return HRESULT_FROM_WIN32(dwResult);
	}

	// Convert to PEEK timestamp format.
	*ts = TimeConv::FileToPeek( *ts );

	// Look for subseconds.
	LPCTSTR	p = lpszStr;
	while ( *p )
	{
		if ( *p == _T('.') )
		{
			p++;	// Skip the period.

			TCHAR*	pp = NULL;
			UInt32	nNanoseconds = _tcstoul( p, &pp, 10 );

			if ( pp > p )
			{
				// Convert to nanoseconds.
				size_t	len = pp - p;	// No need for divide by sizeof(TCHAR).
				if ( len > 9 )
				{
					for ( size_t i = (len - 9); i != 0; i-- )
					{
						nNanoseconds /= 10;
					}
				}
				else if ( len < 9 )
				{
					for ( size_t i = (9 - len); i != 0; i-- )
					{
						nNanoseconds *= 10;
					}
				}

				*ts += nNanoseconds;	// Add the nanoseconds.
			}

			break;
		}

		p++;
	}

	// Look for 'Z' and convert to local time if not found.
	const size_t	cch = _tcslen( lpszStr );
	if ( !((cch > 0) && (lpszStr[cch-1] == 'Z')) )
	{
		*ts += TimeConv::GetOffsetToUTCInSeconds() * RESOLUTION_PER_SECOND;
	}

	return S_OK;
}


// ----------------------------------------------------------------------------
//		CalcDynamicDurationFormat
// ----------------------------------------------------------------------------
// When displaying a tiny time range, display fractional seconds
// down to a tiny enough precision that you can actually see 
// the difference between two labels. When displaying a big range,
// skip all the trivial noise after the decimal point.

Format
CalcDynamicDurationFormat(
	UInt64	duration )
{
	Format	fmt = kFormat_Seconds;
	if ( duration < (10 * (RESOLUTION_PER_SECOND / 1000000)) )
	{
		fmt = kFormat_Nanoseconds;
	}
	else if ( duration < (10 * (RESOLUTION_PER_SECOND / 1000)) )
	{
		fmt = kFormat_Microseconds;
	}
	else if ( duration < (10 * RESOLUTION_PER_SECOND) )
	{
		fmt	= kFormat_Milliseconds;
	}
	else
	{
		fmt	= kFormat_Seconds;
	}

	return fmt;
}

} /* namespace TimeStampFormat */
