// ============================================================================
//	PeekUnits.cpp
//		implementation of the CFileEx class.
// ============================================================================
//	Copyright (c) 2010-2017 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekUnits.h"
#include <vector>

CSizeUnits	g_SizeUnits;


// =============================================================================
//		CSizeUnits
// =============================================================================

std::vector<tSizeUnitsItem>	CSizeUnits::s_aSizeUnits;
int							CSizeUnits::s_nCount = g_SizeUnits.Init();


// ----------------------------------------------------------------------------
//		Init
// ----------------------------------------------------------------------------

int
CSizeUnits::Init()
{
	tSizeUnitsItem tuiBytes = { kBytes, 1, CPeekString( L"Bytes" ) };
	CSizeUnits::s_aSizeUnits.push_back( tuiBytes );

	tSizeUnitsItem tuiKiloBytes = { kKilobytes, kBytesInKilobyte, CPeekString( L"Kilobytes" ) };
	CSizeUnits::s_aSizeUnits.push_back( tuiKiloBytes );

	tSizeUnitsItem tuiMegaBytes = { kMegabytes, kBytesInMegabyte, CPeekString( L"Megabytes" ) };
	CSizeUnits::s_aSizeUnits.push_back( tuiMegaBytes );

	tSizeUnitsItem tuiGigaBytes = { kGigabytes, kBytesInGigabyte, CPeekString( L"Gigabytes" ) };
	CSizeUnits::s_aSizeUnits.push_back( tuiGigaBytes );

	tSizeUnitsItem tuiTeraBytes = { kTerabytes, kBytesInTerabyte, CPeekString( L"Terabytes" ) };
	CSizeUnits::s_aSizeUnits.push_back( tuiTeraBytes );

	tSizeUnitsItem tuiPetaBytes = { kPetabytes, kBytesInPetabyte, CPeekString( L"Petabytes" ) };
	CSizeUnits::s_aSizeUnits.push_back( tuiPetaBytes );

	return static_cast<int>( s_aSizeUnits.size() );
}


// ----------------------------------------------------------------------------
//		GetConversion
// ----------------------------------------------------------------------------

UInt64
CSizeUnits::GetConversion(
	tSizeUnits	inType )
{
	return (inType <= s_nCount)
		? s_aSizeUnits[inType].fConversion
		: s_aSizeUnits[0].fConversion;
}

UInt64
CSizeUnits::GetConversion(
	const CPeekString&	inLabel )
{
	for ( int i = s_nCount; i > 0; i-- ) {
		if ( inLabel.CompareNoCase( s_aSizeUnits[i].fLabel ) == 0 ) {
			return s_aSizeUnits[i].fConversion;
		}
	}
	return s_aSizeUnits[0].fConversion;
}


// ----------------------------------------------------------------------------
//		GetLabel
// ----------------------------------------------------------------------------

CPeekString
CSizeUnits::GetLabel(
	tSizeUnits	inType )
{
	return (inType <= s_nCount)
		? s_aSizeUnits[inType].fLabel
		: s_aSizeUnits[0].fLabel;
}


// ----------------------------------------------------------------------------
//		GetUnits
// ----------------------------------------------------------------------------

tSizeUnits
CSizeUnits::GetUnits(
	const CPeekString&	inLabel )
{
	for ( int i = s_nCount; i > 0; i-- ) {
		if ( inLabel.CompareNoCase( s_aSizeUnits[i].fLabel ) == 0 ) {
			return s_aSizeUnits[i].fUnits;
		}
	}
	return s_aSizeUnits[0].fUnits;
}

tSizeUnits
CSizeUnits::GetUnits(
	UInt64	inCapacity )
{
	for ( int i = s_nCount; i > 0; i-- ) {
		if ( (inCapacity % s_aSizeUnits[i].fConversion) == 0 ) {
			return s_aSizeUnits[i].fUnits;
		}
	}
	return s_aSizeUnits[0].fUnits;
}


// ----------------------------------------------------------------------------
//		Round
// ----------------------------------------------------------------------------

UInt64
CSizeUnits::Round(
	UInt64		inCapacity,
	tSizeUnits	inType )
{
	UInt64	nConversion = CSizeUnits::GetUnits( inType );
	UInt64	nRounded = (inCapacity + (nConversion / 2)) / nConversion;
	return nRounded;
}