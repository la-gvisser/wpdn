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


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Peek Time
//
//	Peek Time is the number of nanoseconds since 1/1/1601 in the Universal Time
//	Coordinate (UTC). The GetLocalOffset member returns the number of
//	nanoseconds to add to convert to Local time.
//
//	Time Units
//
//	Time Units contain various conversion values and the display names of units
//	of time.
//
//	Peek Local Time
//
//	Peek Local Time store is a Peek Time in the Local time coordinates.
//
//	Peek Duration
//
//	A Peek Duration is the difference between two Peek Times.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		Constants
// =============================================================================

static const UInt32	kMicrosecondsInASecond = 100;
static const UInt32	kMillisecondsInASecond = 1000;
static const UInt32	kSecondsInAMinute = 60;
static const UInt32	kSecondsInAnHour = 60 * kSecondsInAMinute;
static const UInt32	kSecondsInADay = 24 * kSecondsInAnHour;
static const UInt32	kDaysPerQuadriCentennium  = (365 * 400) + 97;
static const SInt32	kDaysPerNormalQuadrennium = (365 * 4) + 1;

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
static const WPTime WPTIME_SECONDS = 1000000000ull;

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

static const UInt64	kPeekTimeUnitsInADay = static_cast<UInt64>( kSecondsInADay ) * WPTIME_SECONDS;
static const UInt64	kFileTimeUnitsInADay = kPeekTimeUnitsInADay / WPTIME_FILETIME;

extern WPTime	WPTimeMinValid( const WPTime a, const WPTime b );
extern WPTime	WPTimeMaxValid( const WPTime a, const WPTime b );
extern WPTime	WPTimeDiff( const WPTime inStart, const WPTime inEnd );
extern int		WPTimeCompare( const WPTime a, const WPTime b );
extern bool		WPTimeOverlap( const WPTime aBegin, const WPTime aEnd, const WPTime bBegin, const WPTime bEnd );


// -----------------------------------------------------------------------------
//		tTimeFields
// -----------------------------------------------------------------------------

typedef struct _tTimeFields{
	UInt32	fYear;
	UInt32	fMonth;
	UInt32	fDay;
	UInt32	fHour;
	UInt32	fMinute;
	UInt32	fSecond;
	UInt32	fNanosecond;

	struct _tTimeFields( UInt64 inTime = 0 ) {
		Encode( inTime );
	}
	struct _tTimeFields( const SYSTEMTIME& inTime ) {
		fYear = inTime.wYear; fMonth = inTime.wMonth; fDay = inTime.wDay;
		fHour = inTime.wHour; fMinute = inTime.wMinute; fSecond = inTime.wSecond;
		fNanosecond = inTime.wMilliseconds * WPTIME_MILLISECONDS;
	}
	operator UInt64() const { return Decode(); }
	//typedef struct _SYSTEMTIME {
	//	WORD wYear;
	//	WORD wMonth;
	//	WORD wDayOfWeek;
	//	WORD wDay;
	//	WORD wHour;
	//	WORD wMinute;
	//	WORD wSecond;
	//	WORD wMilliseconds;
	//} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;
	operator SYSTEMTIME() const {
		SYSTEMTIME	st = { static_cast<WORD>( fYear ), static_cast<WORD>( fMonth ),
			0, static_cast<WORD>( fDay ), static_cast<WORD>( fHour ),
			static_cast<WORD>( fMinute ), static_cast<WORD>( fSecond ),
			static_cast<WORD>( fNanosecond / WPTIME_MILLISECONDS ) };
		return st;
	}
	UInt64	Decode() const;
	void	Encode( UInt64 inTime );
	void	Reset() { fYear = 0; fMonth = 0; fDay = 0; fHour = 0;
		fMinute = 0; fSecond = 0; fNanosecond = 0;
	}
} tTimeFields;


// =============================================================================
//		tLongTime
// =============================================================================

typedef union _tLongTime {
	UInt64		i;
	struct {
		UInt32	TimeLo;
		UInt32	TimeHi;
	} sTime;
	FILETIME	ft;

	union _tLongTime( UInt64 inTime = 0 ) : i( inTime ) {}
	union _tLongTime( FILETIME inTime )
		: i( *(reinterpret_cast<UInt64*>( &inTime )) * WPTIME_FILETIME )
	{}
	operator tTimeFields() const {
		tTimeFields	tf( i );
		return tf;
	}
	operator FILETIME() const {
		tLongTime	lt( i / WPTIME_FILETIME );
		return lt.ft;
	}
	bool	IsNull() const { return (i == 0); }
	static _tLongTime Now() {
		tLongTime ltNow;
		::GetSystemTimeAsFileTime( &ltNow.ft );
		ltNow.i *= WPTIME_FILETIME;
		return ltNow;
	}
	void	Reset() { i = 0; }
} tLongTime;


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
	friend	CPeekTimeLocal;

protected:
	tLongTime	m_ltTime;	// Nanoseconds since 1/1/1601.

public:
	static CPeekString	Format( const tTimeFields& inTime, tTimeUnits inFormat = kMilliseconds );
	static CPeekString	Format( const SYSTEMTIME& inTime );
	static CPeekString	Format( FILETIME inTime ) { return Format( ToSystemTime( inTime ) ); }
	static CPeekString	FormatDate( const tTimeFields& inTime );
	static CPeekString	FormatDate( const SYSTEMTIME& inTime );
	static CPeekString	FormatDate( FILETIME inTime ) { return FormatDate( ToSystemTime( inTime ) ); }
	static CPeekString	FormatTime( const tTimeFields& inTime, tTimeUnits inFormat = kMilliseconds );
	static CPeekString	FormatTime( const SYSTEMTIME& inTime );
	static CPeekString	FormatTime( FILETIME inTime ) { return FormatTime( ToSystemTime( inTime ) ); }
	static CPeekStringA	FormatA( const tTimeFields& inTime, tTimeUnits inFormat = kMilliseconds );
	static CPeekStringA	FormatA( const SYSTEMTIME& inTime );
	static CPeekStringA	FormatA( FILETIME inTime ) { return FormatA( ToSystemTime( inTime ) ); }
	static CPeekStringA	FormatDateA( const tTimeFields& inTime );
	static CPeekStringA	FormatDateA( const SYSTEMTIME& inTime );
	static CPeekStringA	FormatDateA( FILETIME inTime ) { return FormatDateA( ToSystemTime( inTime ) ); }
	static CPeekStringA	FormatTimeA( const tTimeFields& inTime, tTimeUnits inFormat = kMilliseconds );
	static CPeekStringA	FormatTimeA( const SYSTEMTIME& inTime );
	static CPeekStringA	FormatTimeA( FILETIME inTime ) { return FormatTimeA( ToSystemTime( inTime ) ); }

	static SInt64		GetLocalTimeOffset();
	static UInt16		GetRandomizedMilliSec( bool	inSeedRandomizer );
	static CPeekString	GetTimeStringFileName( bool inRandomMilli = true ) throw( ... );

	static tLongTime	Now() { return tLongTime::Now(); }
	static tLongTime	NowLocal() {
		return ToLocal( tLongTime::Now() );
	}

	static tLongTime	ToLocal( tLongTime inTime ) {
		return (inTime.i - GetLocalTimeOffset());
	}
	static tLongTime	ToLocal( const SYSTEMTIME& inTime ) {
		FILETIME	ftSystem;
		::SystemTimeToFileTime( &inTime, &ftSystem );
		FILETIME	ftLocal;
		::FileTimeToLocalFileTime( &ftSystem, &ftLocal );
		return tLongTime( ftLocal );
	}
	// Input argument is FILETIME so that Peek Times will be converted from
	//   nanoseconds to 100 nanosecond units.
	static SYSTEMTIME	ToSystemTime( FILETIME inTime ) {
		SYSTEMTIME	stTime;
		::FileTimeToSystemTime( &inTime, &stTime );
		return stTime;
	}

public:
	;			CPeekTime( UInt64 inTime = 0 ) { m_ltTime.i = inTime; }
	;			CPeekTime( tTimeUnits inType, UInt64 inCount ) {
		m_ltTime.i = CTimeUnits::Duration( inType, inCount );
	}
	;			CPeekTime( tLongTime inTime ) : m_ltTime( inTime ) {}
	explicit	CPeekTime( bool inNow ) { (inNow) ? SetNow() : m_ltTime.i = 0; }
	explicit	CPeekTime( FILETIME inTime ) {
		m_ltTime.ft = inTime;
		m_ltTime.i *= WPTIME_FILETIME;
	}
	explicit	CPeekTime( const SYSTEMTIME& inTime ) {
		::SystemTimeToFileTime( &inTime, &m_ltTime.ft );
		m_ltTime.i *= WPTIME_FILETIME;
	}
	;			CPeekTime( const SYSTEMTIME& inDate, const SYSTEMTIME& inTime ) {
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
	;		CPeekTime( const CPeekString& inTime )
	{
		Parse( inTime );
	}
	;		CPeekTime( const CPeekStringA& inTime )
	{
		Parse( inTime );
	}
	;		~CPeekTime() {}

	;		operator UInt64() const { return m_ltTime.i; }
	;		operator tLongTime() const { return m_ltTime; }
	;		operator FILETIME() const { return static_cast<FILETIME>( m_ltTime ); }
	;		operator SYSTEMTIME() const { return ToSystemTime( static_cast<FILETIME>( m_ltTime ) ); }

	CPeekString		Format( tTimeUnits inFormat = kMilliseconds ) const {
		return Format( m_ltTime, inFormat );
	}
	CPeekString		FormatDate() const { return FormatDate( static_cast<FILETIME>( *this ) ); }
	CPeekString		FormatTime( tTimeUnits inFormat = kMilliseconds ) const {
		return FormatTime( m_ltTime, inFormat );
	}
	CPeekStringA	FormatA( tTimeUnits inFormat = kMilliseconds ) const {
		return FormatA( m_ltTime, inFormat );
	}
	CPeekStringA	FormatDateA() const { return FormatDateA( static_cast<FILETIME>( *this ) ); }
	CPeekStringA	FormatTimeA( tTimeUnits inFormat = kMilliseconds ) const {
		return FormatTimeA( m_ltTime, inFormat );
	}

	UInt64		Get( tTimeUnits inType = kNanoseconds ) const {
		return CTimeUnits::Duration( inType, m_ltTime.i );
	}
	UInt32		GetMilliseconds() const { return static_cast<UInt32>( m_ltTime.i / WPTIME_MILLISECONDS ); }
	void		GetLocalTimeDate( FILETIME& outDate, FILETIME& outTime );
	UInt32		GetSeconds() const { return static_cast<UInt32>( m_ltTime.i / WPTIME_SECONDS ); }

	void	Parse( const CPeekString& inTime );
	void	Parse( const CPeekStringA& inTime );

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
	;		CPeekTimeLocal( tLongTime inTime ) : CPeekTime( inTime ) {}
	;		CPeekTimeLocal( const CPeekTimeLocal& inTime ) : CPeekTime( inTime.m_ltTime.i ) {}
	;		CPeekTimeLocal( bool inNow ) : CPeekTime( (inNow) ? NowLocal() : 0 ) {}
	;		CPeekTimeLocal( FILETIME inTime ) : CPeekTime( ToLocal( inTime ) ) {}
	;		CPeekTimeLocal( SYSTEMTIME inTime ) : CPeekTime( ToLocal( inTime ) ) {}
	;		CPeekTimeLocal( SYSTEMTIME inDate, SYSTEMTIME inTime ) : CPeekTime( inDate, inTime ) {}
	;		CPeekTimeLocal( const CPeekTime& inTime )
		: CPeekTime( ToLocal( static_cast<tLongTime>( inTime.m_ltTime ) ) )
	{}

	virtual void	SetNow() { *this = CPeekTimeLocal( CPeekTime::NowLocal() ); }
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
