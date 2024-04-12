// ============================================================================
//	PeekUnits.h
//		interface for the CSizeUnits class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#pragma once

#include "WPTypes.h"
#include "PeekStrings.h"


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
