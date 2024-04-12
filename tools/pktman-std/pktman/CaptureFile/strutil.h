// ============================================================================
//	strutil.h
//		String utility functions.
// ============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#include "WPTypes.h"
#include <cstring>
#include <iomanip>
#include <memory>
#include <string>
#include <sstream>

using std::string;
using std::wstring;

#pragma once

// Seconds between 1/1/1601 and 1/1/1970
// (see KB167296: http://support.microsoft.com/default.aspx?scid=KB;en-us;q167296).
extern const UInt64	kFileTimeToAnsi;
extern const UInt64 kPeekTimeToAnsi;
// number of Peek Time counts in a File Time.
extern const UInt64 kPeekTimePerFileTime;

// number of WPTime counts from 1/1/1601 to 1/1/1970.
// (Microsoft KB167296: 116444736000000000 * 100).
extern const UInt64	kPeekTimeANSIOffset;
// number of PeekTime counts in a single second.
extern const UInt64	kPeekTimeSeconds;
extern const UInt64 kResolutionPerSecond;	// nanoseconds
extern const UInt64	kDaysPerQuadriCentennium;
extern const SInt64	kDaysPerNormalQuadrennium;

extern const UInt64	kMillisecondsPerSecond;
extern const UInt64	kMicrosecondsPerSecond;
extern const UInt64	kNanosecondsPerSecond;
extern const UInt64	kSecondsPerMinute;
extern const UInt64	kSecondsPerHour;
extern const UInt64	kSecondsPerDay;
extern const UInt64	kSecondsPerWeek;
extern const UInt64	kSecondsPerYear;

wstring	AtoW( const char* inOther, size_t inLength );
wstring AtoW( const string& inOther );
string	WtoA( const wchar_t* inOther, size_t inLength );
string	WtoA( const wstring& inOther );

UInt64	Now();


class CTimeFields {
public:
	UInt32	fYear;
	UInt32	fMonth;
	UInt32	fDay;
	UInt32	fHour;
	UInt32	fMinute;
	UInt32	fSecond;
	UInt32	fNanosecond;

	CTimeFields( UInt64 inTime = 0 ) {
		Encode( inTime );
	}

	UInt64	Decode() {
		const UInt32	nMonths = (fMonth < 3) ? (fMonth + 13) : (fMonth + 1);
		const UInt32	nYears = (fMonth < 3) ? (fYear - 1) : fYear;
		const UInt32	nLeaps = (3 * (nYears / 100) + 3) / 4;
		const UInt32	nDays  = ((36525 * nYears) / 100) - nLeaps + ((1959 * nMonths) / 64)
									+ fDay - 584817;

		return (((((((static_cast<UInt64>( nDays * 24 ) + fHour) * 60)
			+ fMinute) * 60) + fSecond) * kResolutionPerSecond) + fNanosecond);
	}

	void Encode( UInt64 inTime ) {
		if ( inTime == 0 ) {
            fYear = 0;
            fMonth = 0;
            fDay = 0;
            fHour = 0;
            fMinute = 0;
            fSecond = 0;
            fNanosecond = 0;
			return;
		}

		// Convert to seconds.
		const UInt64	nSeconds = inTime / kPeekTimeSeconds;

		// Split into days and seconds within the day.
		UInt32	nDays = static_cast<UInt32>( nSeconds / kSecondsPerDay );
		UInt32	nSecondsInDay = static_cast<UInt32>( nSeconds % kSecondsPerDay );

		// Time.
		fHour = static_cast<UInt32>( nSecondsInDay / kSecondsPerHour );
		nSecondsInDay =  nSecondsInDay % kSecondsPerHour;
		fMinute = static_cast<UInt32>( nSecondsInDay / kSecondsPerMinute );
		fSecond = nSecondsInDay % kSecondsPerMinute;
		fNanosecond = static_cast<UInt32>( inTime % kPeekTimeSeconds );

		// Date.
		// See, e.g., "30.6 Days Hath September" by James Miller,
		// http://www.amsat.org/amsat/articles/g3ruh/100.html.
		const UInt32	nLeaps = static_cast<UInt32>( (3 * (((4 * nDays) + 1227) / kDaysPerQuadriCentennium) + 3 ) / 4 );
		nDays += 28188 + nLeaps;
		const UInt32	nYears = static_cast<UInt32>( ((20 * nDays) - 2442) / (5 * kDaysPerNormalQuadrennium ) );
		const UInt32	nYearday = static_cast<UInt32>( nDays - ((nYears * kDaysPerNormalQuadrennium) / 4) );
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

    wstring Format( bool inMilliseconds = false ) {
        // As close to ISO 8601 as possible periods instead of colons
        // 25 characters: 'YYYY-MM-DDTHH.MM.SS.mmmZ'
        // 31 characters: 'YYYY-MM-DDTHH.MM.SS.nnnnnnnnnZ'
        std::wostringstream	ssTime;
        ssTime << std::setfill( L'0' )
                << std::setw( 4 ) << fYear << L"-"
                << std::setw( 2 ) << fMonth << L"-"
                << std::setw( 2 ) << fDay << L"T"
                << std::setw( 2 ) << fHour << L"."
                << std::setw( 2 ) << fMinute << L"."
                << std::setw( 2 ) << fSecond << L".";
        if ( inMilliseconds ) {
            ssTime << std::setw( 3 ) << (fNanosecond / 1000000) << L"Z";
        }
        else {
            ssTime << std::setw( 9 ) << fNanosecond << L"Z";
        }

        return ssTime.str();
    }
};
