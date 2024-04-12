// =============================================================================
//	FtpPacketArray.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "FtpPacketArray.h"


// =============================================================================
//		CFtpPacketArray
// =============================================================================

// -----------------------------------------------------------------------------
//		Add
// -----------------------------------------------------------------------------

bool
CFtpPacketArray::Add(
	const CPacket&		inPacket,
	CByteVectRef&		inTcpData,
	bool				inIsFin )
{
	if ( !inPacket.IsValid() ) return false;

	const UInt16	nPayloadLength( static_cast<UInt16>( inTcpData.GetCount() ) );

	if ( nPayloadLength == 0 ) return false;

	if ( inIsFin ) {
		const UInt8* theData( inTcpData.GetData( nPayloadLength ) );
		if ( nPayloadLength == 6 && memcmp( theData, "\0\0\0\0\0\0", 6 ) == 0 ) {
			return false;
		}
	}

	CPacket*	pPacket( new CPacket( inPacket, true ) );
	if ( !pPacket )	Peek::Throw( HE_E_OUT_OF_MEMORY );
	PacketPtr	spPacket( pPacket );

	CLayerTCP layerTCP;
	if( !spPacket->GetLayer(layerTCP) ) {
		ASSERT( 0 );
		return false;
	}

	const CTcpHeader& theTcpHeader(	layerTCP.GetHeader() );

	FtpPacket* pFtpPacket( new FtpPacket );
	if ( !pFtpPacket )	Peek::Throw( HE_E_OUT_OF_MEMORY );

	pFtpPacket->spPacket = spPacket;
	pFtpPacket->nSequenceNumber = theTcpHeader.GetSequenceNumber();
	pFtpPacket->DataRef = layerTCP.GetRemainingRef();
	pFtpPacket->nPayloadLength = static_cast<UInt16>( pFtpPacket->DataRef.GetCount() );

	FtpPacketPtr	theFtpPacketPtr( pFtpPacket );

	CAtlArray::Add( theFtpPacketPtr );

	return true;
}


// -----------------------------------------------------------------------------
//      Sort
//  Sort the array based on Tcp sequence number.
// -----------------------------------------------------------------------------

bool
CFtpPacketArray::Sort()
{
	size_t nCount( GetCount() );
	if ( nCount < 2 ) return true;

	bool		bChanged( true );
	bool		bDuplicate( false );

	for ( size_t j = nCount - 1; j > 0, bChanged == true; j-- ) {

		if ( bChanged == false )	break;
		if ( j == 0 )	break;

		bChanged = false;

		for ( size_t i = 0; i < j; i++ ) {
			ASSERT ( (i + 1) < nCount );
			FtpPacketPtr&	pCurrent( GetAt(i) );
			FtpPacketPtr&	pNext( GetAt(i+1) );
			if (  pCurrent->nSequenceNumber >= pNext->nSequenceNumber ) {
				// Duplicate sequence number - will be marked for deletion later.
				if (  pCurrent->nSequenceNumber == pNext->nSequenceNumber ) {
					bDuplicate = true;
				}
				// Out of order - swap
				else {
					FtpPacketPtr pTemp1( pCurrent );
					FtpPacketPtr pTemp2( pNext );
					SetAt( i, pTemp2 );
					SetAt( i+1, pTemp1 );
					bChanged = true;
				}
			}
		}
	}

	nCount = GetCount();
	if ( bDuplicate ) {
		for ( size_t i = 0; i < nCount - 1; i++ ) {
			FtpPacketPtr&	pCurrent( GetAt(i) );
			if ( pCurrent->bDeleted == true ) continue;
			FtpPacketPtr&	pNext( GetAt(i+1) );
			if ( pNext->bDeleted == true ) continue;
			if (  pCurrent->nSequenceNumber == pNext->nSequenceNumber ) {
				if ( pCurrent->nPayloadLength <= pNext->nPayloadLength ) { pCurrent->bDeleted = true; }
				else { pNext->bDeleted = true; }
			}
		}
	}

	// Remove no-op packets at the beginning of the sort
	nCount = GetCount();
	for ( size_t i = 0; i < nCount; i++ ) {
		FtpPacketPtr&	pCurrent( GetAt(i) );
		if ( pCurrent->bDeleted == true ) continue;

		if ( (pCurrent->nPayloadLength == 0) || 
			 (pCurrent->nPayloadLength == 6 && memcmp( pCurrent->DataRef.GetData( 6 ), "\0\0\0\0\0\0", 6 ) == 0 ) ) {
				 pCurrent->bDeleted = true;
		}
		else {
			break;
		}
	}

	// Remove no-op packets at the end of the sort
	nCount = GetCount();
	for ( size_t i = nCount - 1; i >= 0; i-- ) {
		FtpPacketPtr&	pCurrent( GetAt(i) );
		if ( pCurrent->bDeleted == true ) continue;
		if ( (pCurrent->nPayloadLength == 0) || 
			(pCurrent->nPayloadLength == 6 && memcmp( pCurrent->DataRef.GetData( 6 ), "\0\0\0\0\0\0", 6 ) == 0 ) ) {
				pCurrent->bDeleted = true;
		}
		else {
			break;
		}
	}

	return true;
}


// -----------------------------------------------------------------------------
//      Verify
//  Verify that all packets are sequenced properly.
// -----------------------------------------------------------------------------

bool
CFtpPacketArray::Verify()
{
	const size_t nCount( GetCount() );
	if ( nCount < 2 ) return true;

	size_t i( 0 );
	while( i < nCount - 1 ) {

		while( i < nCount - 1 ) {
			if ( GetAt(i)->bDeleted == false ) break;
			i++;
		}

		if ( i >= nCount - 1 ) break;

		FtpPacketPtr&	pCurrent( GetAt(i) );
		
		i++;

		while( i < nCount ) {
			if ( GetAt(i)->bDeleted == false ) break;
			i++;
		}

		if ( i >= nCount ) break;

		FtpPacketPtr&	pNext( GetAt(i) );

		UInt32	nNextSequenceNum( pCurrent->nSequenceNumber + pCurrent->nPayloadLength );
		if ( nNextSequenceNum != pNext->nSequenceNumber ) {
			return false;
		}
	}

	return true;
}
