// ============================================================================
//	PeekUnits.h
//		interface for the CSizeUnits class.
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "WPTypes.h"
#include "PeekStrings.h"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Peek Units
//
//	Peek Units contains and defines various conversion values and display names
//	for various quantities: kilobyte, megabyte, gigabyte, terabyte and petabyte.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//	SI Decimal Prefix		IEC Binary Prefix
//	kilobyte  (kB)  10^3	kibibyte (KiB) 2^10
//	megabyte  (MB)  10^6	mebibyte (MiB) 2^20
//	gigabyte  (GB)  10^9	gibibyte (GiB) 2^30
//	terabyte  (TB) 10^12	tebibyte (TiB) 2^40
//	petabyte  (PB) 10^15	pebibyte (PiB) 2^50
//	exabyte   (EB) 10^18	exbibyte (EiB) 2^60
//	zettabyte (ZB) 10^21	zebibyte (ZiB) 2^70
//	yottabyte (YB) 10^24	yobibyte (YiB) 2^80
// =============================================================================

// =============================================================================
//		Constants
// =============================================================================

const	UInt64	kBytesInKilobyte = 1024ull;
const	UInt64	kBytesInMegabyte = 1024ull * 1024ull;
const	UInt64	kBytesInGigabyte = 1024ull * 1024ull * 1024ull;
const	UInt64	kBytesInTerabyte = 1024ull * 1024ull * 1024ull * 1024ull;
const	UInt64	kBytesInPetabyte = 1024ull * 1024ull * 1024ull * 1024ull * 1024ull;

const	UInt16  kMaxUint16 = 0xFFFF;
const	UInt32  kMaxUint32 = 0xFFFFFFFF;
const	UInt64	kMaxUint64 = 0xFFFFFFFFFFFFFFFFull;
#ifdef WIN64
const	UInt64	kMaxSizeT = kMaxUint64;
#else
const	UInt64	kMaxSizeT = kMaxUint32;
#endif

const	size_t	kIndex_Invalid = static_cast<size_t>( -1 );


// =============================================================================
//		tSizeUnits
// =============================================================================

typedef enum {
	kBytes,
	kKilobytes,
	kMegabytes,
	kGigabytes,
	kTerabytes,
	kPetabytes
} tSizeUnits;


// =============================================================================
//		tSizeUnitsItem
// =============================================================================

typedef struct {
	tSizeUnits	fUnits;
	UInt64		fConversion;
	CPeekString	fLabel;
} tSizeUnitsItem;


// =============================================================================
//		CSizeUnits
// =============================================================================

class CSizeUnits
{
protected:
	static int							s_nCount;
	static CPeekArray<tSizeUnitsItem>	s_aSizeUnits;

public:
	static	int			Init();

public:
	;					CSizeUnits() {}

	static UInt64		Capacity( tSizeUnits inType, UInt64 inCount ) {
		return (inCount * GetConversion( inType ));
	}

	static UInt64		GetConversion( tSizeUnits inType );
	static UInt64		GetConversion( const CPeekString& inLabel );
	static UInt32		GetCount() { return s_nCount; }
	static CPeekString	GetLabel( tSizeUnits inType );
	static tSizeUnits	GetUnits( const CPeekString& inLabel );
	static tSizeUnits	GetUnits( UInt64 inCapacity );

	static UInt64		Round( UInt64 inCapacity, tSizeUnits inType );
};
