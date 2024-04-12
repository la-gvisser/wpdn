// =============================================================================
//	PeekTime.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.
//
// Peek Time is nanoseconds since 1/1/1601. 64 bits, unsigned.
// Overflows in the year 2185 AD.
//

#pragma once

#include "WPTypes.h"
#include "Peek.h"
#include "PeekArray.h"
#include "PeekStrings.h"

class CPeekTimeLocal;


// =============================================================================
//		Constants
// =============================================================================

const UInt32	kMicrosecondsInASecond = 100;
const UInt32	kMillisecondsInASecond = 1000;
const UInt32	kSecondsInAMinute = 60;
const UInt32	kSecondsInAnHour = 60 * kSecondsInAMinute;
const UInt32	kSecondsInADay = 24 * kSecondsInAnHour;

const UInt64	kFileTimeUnitsInADay = static_cast<UInt64>( kSecondsInADay ) * 10000000;

typedef enum {
	kNanoseconds,
	kMilliseconds,
	kMicroseconds,
	kSeconds,
	kMinutes,
	kHours,
	kDays
} tTimeUnits;


// =============================================================================
//		WPTime
// =============================================================================

typedef UInt64	WPTime;

// more descriptive than "(UInt64) -1"
static const WPTime WPTIME_INVALID = static_cast<WPTime>( -1 );

// number of WPTime counts in a single second.
static const WPTime WPTIME_SECONDS = 1000000000;

// number of WPTime counts in a single Day.
static const WPTime WPTIME_DAYS = (WPTIME_SECONDS * kSecondsInADay);

// number of WPTime counts in a single minute.
static const WPTime WPTIME_HOURS = WPTIME_SECONDS * 60 * 60;

// number of WPTime counts in a single minute.
static const WPTime WPTIME_MINUTES = WPTIME_SECONDS * 60;

// number of WPTime counts in a millisecond
static const WPTime	WPTIME_MILLISECONDS = WPTIME_SECONDS / 1000;

// number of WPTime counts in a microsecond
static const WPTime	WPTIME_MICROSECONDS = WPTIME_SECONDS / 1000000;

// number of WPTime counts in a File Time.
static const WPTime WPTIME_FILETIME = 100;

// the smallest legal WPTime value ('0', aka 1/1/1601)
static const WPTime	WPTIME_MIN = 0;

// the largest legal WPTime value (sometime in 2185 AD)
static const WPTime WPTIME_MAX = 0xFFFFFFFFFFFFFFFEull;

extern WPTime	WPTimeMinValid( const WPTime a, const WPTime b );
extern WPTime	WPTimeMaxValid( const WPTime a, const WPTime b );
extern WPTime	WPTimeDiff( const WPTime inStart, const WPTime inEnd );
extern int		WPTimeCompare( const WPTime a, const WPTime b );
extern bool		WPTimeOverlap( const WPTime aBegin, const WPTime aEnd, const WPTime bBegin, const WPTime bEnd );


// =============================================================================
//		LONGTIME
// =============================================================================

typedef union _LONGTIME {
	UInt64		i;
	FILETIME	ft;

	static inline const _LONGTIME& Null()
	{
		static const LONGTIME ltNull = { 0 };
		return ltNull;
	}
} LONGTIME;


// =============================================================================
//		tTimeUnitsItem
// =============================================================================

typedef struct {
	tTimeUnits	fUnits;
	UInt64		fConversion;
	CPeekString	fLabel;
} tTimeUnitsItem;


// =============================================================================
//		CTimeUnits
// =============================================================================

class CTimeUnits
{
protected:
	static int							s_nCount;
	static CPeekArray<tTimeUnitsItem>	s_aTimeUnits;

public:
	static	int			Init();

public:
	;		CTimeUnits() {}

	static UInt64		Duration( tTimeUnits inType, UInt64 inCount ) {
		return (inCount * GetConversion( inType ));
	}

	static UInt64			GetConversion( tTimeUnits inType );
	static UInt64			GetConversion( const CPeekString& inLabel );
	static UInt32			GetCount() { return s_nCount; }
	static const wchar_t*	GetLabel( tTimeUnits inType );
	static tTimeUnits		GetUnits( const CPeekString& inLabel );
	static tTimeUnits		GetUnits( UInt64 inDuration );

	static UInt64		Round( UInt64 inDuration, tTimeUnits inType );
};


// =============================================================================
//		CPeekTime
// =============================================================================

class CPeekTime
	:	public COmniModeledData
{
protected:
	LONGTIME	m_ltTime;	// Nanoseconds since 1/1/1601.

public:
	;		CPeekTime( UInt64 inTime = 0 ) { m_ltTime.i = inTime; }
	;		CPeekTime( tTimeUnits inType, UInt64 inCount ) {
		m_ltTime.i = CTimeUnits::Duration( inType, inCount );
	}
	;		CPeekTime( LONGTIME inTime ) : m_ltTime( inTime ) {}
	;		CPeekTime( bool inNow ) { (inNow) ? SetNow() : m_ltTime.i = 0; }
	;		CPeekTime( FILETIME inTime ) {
		m_ltTime.ft = inTime;
		m_ltTime.i *= WPTIME_FILETIME;
	}
	;		CPeekTime( SYSTEMTIME inTime ) {
		::SystemTimeToFileTime( &inTime, &m_ltTime.ft );
		m_ltTime.i *= WPTIME_FILETIME;
	}
	;		CPeekTime( SYSTEMTIME inDate, SYSTEMTIME inTime ) {
		SYSTEMTIME	stTime;
		
		stTime.wYear = inDate.wYear;		// Copy Date fields from inDate.
		stTime.wMonth = inDate.wMonth;
		stTime.wDay = inDate.wDay;
		stTime.wDayOfWeek = inDate.wDayOfWeek;

		stTime.wHour = inTime.wHour;		// Copy Time fields from inTime;
		stTime.wMinute = inTime.wMinute;
		stTime.wSecond = inTime.wSecond;
		stTime.wMilliseconds = inTime.wMilliseconds;

		::SystemTimeToFileTime( &stTime, &m_ltTime.ft );
		m_ltTime.i *= WPTIME_FILETIME;
	}
	;		CPeekTime( const CPeekTimeLocal& inTime );
	;		~CPeekTime() {}

	;		operator UInt64() const { return m_ltTime.i; }
	;		operator FILETIME() const {
		LONGTIME	ltTime = { (m_ltTime.i / WPTIME_FILETIME) };
		return ltTime.ft;
	}
	;		operator SYSTEMTIME() const {
		return GetSystemTime( *this );
	}

	// Input argument is FILETIME so that Peek Times will be converted from
	//   nanoseconds to 100 nanosecond units.
	static SYSTEMTIME	GetSystemTime( FILETIME inTime ) {
		SYSTEMTIME	stTime;
		::FileTimeToSystemTime( &inTime, &stTime );
		return stTime;
	}

	static CPeekString	Format( const SYSTEMTIME& inTime );
	static CPeekString	Format( FILETIME inTime ) { return Format( GetSystemTime( inTime ) ); }
	static CPeekString	FormatDate( const SYSTEMTIME& inTime );
	static CPeekString	FormatDate( FILETIME inTime ) { return FormatDate( GetSystemTime( inTime ) ); }
	static CPeekString	FormatTime( const SYSTEMTIME& inTime );
	static CPeekString	FormatTime( FILETIME inTime ) { return FormatTime( GetSystemTime( inTime ) ); }

	//petertest - I added this - need to check whether is converted!
	static CPeekString	GetTimeString() {
		SYSTEMTIME	theTime;
		::GetSystemTime( &theTime );
		return Format( theTime );
	}

	static UInt16		GetRandomizedMilliSec( bool	inSeedRandomizer );
	static CPeekString	GetTimeStringFileName( bool inRandomMilli = true ) throw( ... );

	static LONGTIME	Now() {
		LONGTIME	ltNow;
		::GetSystemTimeAsFileTime( &ltNow.ft );
		ltNow.i *= WPTIME_FILETIME;
		return ltNow;
	}
	//petertest - I added this
	static UInt64 Now64() {
		LONGTIME	ltNow;
		::GetSystemTimeAsFileTime( &ltNow.ft );
		return ltNow.i;
	}
	static LONGTIME	NowLocal() {
		LONGTIME	ltNow;
		FILETIME	ftUTC;
		::GetSystemTimeAsFileTime( &ftUTC );
		ltNow = ToLocal( ftUTC );
		return ltNow;
	}

	static LONGTIME	ToLocal( FILETIME inTime ) {
		LONGTIME	ltLocal;
		::FileTimeToLocalFileTime( &inTime, &ltLocal.ft );
		ltLocal.i *= WPTIME_FILETIME;
		return ltLocal;
	}
	static LONGTIME	ToLocal( SYSTEMTIME inTime ) {
		FILETIME	ftLocal;
		::SystemTimeToFileTime( &inTime, &ftLocal );
		LONGTIME	ltLocal;
		::FileTimeToLocalFileTime( &ftLocal, &ltLocal.ft );
		ltLocal.i *= WPTIME_FILETIME;
		return ltLocal;
	}

	CPeekString	Format() const { return Format( static_cast<FILETIME>( *this ) ); }
	CPeekString	FormatDate() const { return FormatDate( static_cast<FILETIME>( *this ) ); }
	CPeekString	FormatTime() const { return FormatTime( static_cast<FILETIME>( *this ) ); }

	UInt64		Get( tTimeUnits inType = kNanoseconds ) {
		return CTimeUnits::Duration( inType, m_ltTime.i );
	}
	UInt32		GetMilliseconds() const { return static_cast<UInt32>( m_ltTime.i / WPTIME_MILLISECONDS ); }
	void		GetLocalTimeDate( FILETIME& outDate, FILETIME& outTime );
	UInt32		GetSeconds() const { return static_cast<UInt32>( m_ltTime.i / WPTIME_SECONDS ); }

	void	Parse( const CPeekString& inTime );

	void	Reset() { m_ltTime.i = 0; }

	void	Set( tTimeUnits inType, UInt64 inCount ) {
		m_ltTime.i = CTimeUnits::Duration( inType, inCount );
	}
	void	SetLocal( FILETIME inDate, FILETIME inTime );
	virtual void	SetNow() { m_ltTime = CPeekTime::Now(); }
	void	SetNowLocal() { m_ltTime = CPeekTime::NowLocal(); }

	// COmniModeledData
	virtual CPeekString	ModeledDataGet() const { return Format(); }
	virtual void		ModeledDataSet( const CPeekString& inValue ) { Parse( inValue ); }
};


// =============================================================================
//		CPeekTimeLocal
// =============================================================================

class CPeekTimeLocal
	:	public CPeekTime
{
public:
	;		CPeekTimeLocal( UInt64 inTime = 0 ) : CPeekTime( inTime ) {}
	;		CPeekTimeLocal( const CPeekTimeLocal& inTime ) : CPeekTime( inTime.m_ltTime.i ) {}
	;		CPeekTimeLocal( bool inNow ) : CPeekTime( (inNow) ? NowLocal() : LONGTIME::Null() ) {}
	;		CPeekTimeLocal( FILETIME inTime ) : CPeekTime( ToLocal( inTime ) ) {}
	;		CPeekTimeLocal( SYSTEMTIME inTime ) : CPeekTime( ToLocal( inTime ) ) {}
	;		CPeekTimeLocal( SYSTEMTIME inDate, SYSTEMTIME inTime ) : CPeekTime( inDate, inTime ) {}
	;		CPeekTimeLocal( const CPeekTime& inTime ) : CPeekTime( ToLocal( static_cast<FILETIME>( inTime ) ) ) {}

	CPeekTimeLocal&	operator=( const CPeekTime& inTime ) {
		FILETIME	ftUTC = inTime;
		::FileTimeToLocalFileTime( &ftUTC, &m_ltTime.ft );
		m_ltTime.i *= WPTIME_FILETIME;
		return *this;
	}

	virtual void	SetNow() { *this = CPeekTime::Now(); }
};


// =============================================================================
//		CPeekDuration
// =============================================================================

class CPeekDuration
	:	public COmniModeledData
{
protected:
	UInt64	m_nDuration;	// In Nanoseconds.

public:
	;		CPeekDuration( UInt64 inDuration = 0 ) { m_nDuration = inDuration; }
	;		CPeekDuration( tTimeUnits inType, UInt64 inCount )
		: m_nDuration( CTimeUnits::Duration( inType, inCount ) )
	{}
	;		~CPeekDuration() {}

	;		operator UInt64() const { return m_nDuration; }

	CPeekDuration& operator=( UInt64 inDuration ) {
		m_nDuration = inDuration;
		return *this;
	}

	CPeekString	Format() const;

	UInt64	Get( tTimeUnits inType ) const {
		return (m_nDuration / CTimeUnits::GetConversion( inType ));
	}

	void	Parse( const CPeekString& inValue );

	void	Round( tTimeUnits inType ) {
		m_nDuration = CTimeUnits::Round( m_nDuration, inType );
	}

	void	Set( UInt64 inDuration ) { m_nDuration = inDuration; }
	void	Set( tTimeUnits inType, UInt64 inCount ) {
		m_nDuration = (inCount * CTimeUnits::GetConversion( inType ));
	}

	// COmniModeledData
	virtual CPeekString	ModeledDataGet() const { return Format(); }
	virtual void		ModeledDataSet( const CPeekString& inValue ) { Parse( inValue ); }
};
