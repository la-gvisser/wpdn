// ============================================================================
//	ut_bytevectref.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2010. All rights reserved.

#include "StdAfx.h"
#include "ByteVectRef.h"

int
UT_ByteVectRef()
{
	int		nFailures( 0 );

	const size_t	cbData( 250 );
	UInt8			ayData[cbData];
	for ( size_t i = 0; i < cbData; i++ ) {
		ayData[i] = static_cast<UInt8>( i );
	}

	CByteVectRef	bvData( cbData, ayData );
	if ( bvData.GetCount() != cbData ) nFailures++;
	if ( bvData.GetData( cbData ) == nullptr ) nFailures;
	if ( bvData.GetData( cbData + 1 ) != nullptr ) nFailures++;
	if ( bvData.IsEmpty() ) nFailures++;

	CByteVectRef	bvEmpty;
	if ( !bvEmpty.IsEmpty() ) nFailures++;
	if ( bvEmpty.GetCount() != 0 ) nFailures++;

	CByteVectRef	bvLeft10 = bvData.Left( 10 );
	if ( bvLeft10.IsEmpty() ) nFailures++;
	if ( bvLeft10.GetCount() != 10 ) nFailures++;
	{
		const UInt8*	pLeft = bvLeft10.GetData( 10 );
		if ( pLeft[0] != 0 ) nFailures++;
		if ( pLeft[9] != 9 ) nFailures++;
	}

	CByteVectRef	bvRight10 = bvData.Right( 10 );
	if ( bvRight10.IsEmpty() ) nFailures++;
	if ( bvRight10.GetCount() != 10 ) nFailures++;
	{
		const UInt8*	pRight = bvRight10.GetData( 10 );
		if ( pRight[0] != 240 ) nFailures++;
		if ( pRight[9] != 249 ) nFailures++;
	}

	CByteVectRef	bvMid150 = bvData.Seek( 100 );
	if ( bvMid150.IsEmpty() ) nFailures++;
	if ( bvMid150.GetCount() != 150 ) nFailures++;
	{
		const UInt8*	pMid = bvMid150.GetData( 150 );
		if ( pMid[0] != 100 ) nFailures++;
		if ( pMid[149] != 249 ) nFailures++;
	}

	CByteVectRef	bvMid50 = bvMid150.Seek( 100 );
	if ( bvMid50.IsEmpty() ) nFailures++;
	if ( bvMid50.GetCount() != 50 ) nFailures++;
	{
		const UInt8*	pMid = bvMid50.GetData( 50 );
		if ( pMid[0] != 200 ) nFailures++;
		if ( pMid[49] != 249 ) nFailures++;
	}
	return nFailures;
}
