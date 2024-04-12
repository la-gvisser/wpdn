// ============================================================================
// PacketList.cpp:
//      implementation of the CPacketList class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2009. All rights reserved.

#include "StdAfx.h"
#include "PacketList.h"


// ============================================================================
//		CIPv4PacketList
// ============================================================================

// ----------------------------------------------------------------------------
//		GetSortList
// ----------------------------------------------------------------------------

void
CIPv4PacketList::GetSortList(
	CUIntArray&	outList )
{
	size_t		nCount( GetCount() );
	CUIntArray	ayOffsets;

	outList.SetSize( nCount );
	outList[0] = 0;
	if ( nCount == 1 ) return;

	ayOffsets.SetSize( nCount );
	ayOffsets[0] = GetAt( 0 )->GetIPHeader().GetFragmentOffset();

	for ( size_t i = 1; i < nCount; i++ ) {
		outList[i] = static_cast<UINT>( i );
		ayOffsets[i] = GetAt( i )->GetIPHeader().GetFragmentOffset();
	}

	// Bubble Sort.
	size_t	nUpper( nCount );
	do {
		for ( size_t i = 1; i < nUpper; i++ ) {
			if ( ayOffsets[i - 1] > ayOffsets[i] ) {
				UINT	nTemp = outList[i];
				outList[i] = outList[i-1];
				outList[i-1] = nTemp;

				UINT	nOffset = ayOffsets[i];
				ayOffsets[i] = ayOffsets[i-1];
				ayOffsets[i-1] = nOffset;
			}
		}
		nUpper--;
	} while ( nUpper > 1 );
}


// ----------------------------------------------------------------------------
//		IsComplete
// ----------------------------------------------------------------------------

bool
CIPv4PacketList::IsComplete(
	bool	inAddMac ) const
{
	size_t	nPacketCount = GetCount();
	if ( nPacketCount == 0 ) return false;

	// Check for single fragment.
	if ( nPacketCount == 1 ) {
		CIPv4Packet*	pFragment( GetAt( 0 ) );
		ASSERT( pFragment );
		if ( (pFragment) && (pFragment->IsPacketType( CPacket::kPacketType_UDP )) ) {
			CUDPPacket*	pUdpPacket( static_cast<CUDPPacket*>( pFragment ) );
			ASSERT( pUdpPacket );
			if ( pUdpPacket && !pUdpPacket->GetIPHeader().HasFragments() ) {
				// Done, encapsulated in one packet.
				return true;
			}
			// There are additional fragments.
			return false;
		}
	}

	bool	bHaveFirst( false );
	bool	bHaveLast( false );
	UInt16	nAdditional( (inAddMac) ? sizeof( EthernetPacketHeader ) : 0 );
	UInt16	nExpectedSize( 0 );			// From first UDP Header.
	UInt16	nAltExpectedSize( 0 );		// Last fragment offset + payload size.
	UInt16	nTotalPayloadSize( nAdditional );
	for ( size_t i = 0; i < nPacketCount; i++ ) {
		CIPv4Packet*	pFragment( GetAt( i ) );
		UInt16			nPayloadSize( 0 );
		ASSERT( pFragment );
		if ( pFragment == NULL ) return false;
		// The first packet is UDP, the rest are not.
		if ( pFragment->GetPacketType() == CPacket::kPacketType_UDP ) {
			CUDPPacket*	pUdpPacket( static_cast<CUDPPacket*>( pFragment ) );
			ASSERT( pUdpPacket );
			if ( pUdpPacket && pUdpPacket->GetIPHeader().HasFragments() ) {
				bHaveFirst = true;
				nPayloadSize = pUdpPacket->GetUDPHeader().GetPayloadDataLength();
				nExpectedSize = pUdpPacket->GetUDPHeader().GetLengthField() -
					sizeof( tUdpHeader ) + nAdditional;
			}
		}
		else {
			// The rest are fragments.
			nPayloadSize = pFragment->GetIPHeader().GetPayloadLength();
			if ( !pFragment->GetIPHeader().HasFragments() && (pFragment->GetIPHeader().GetFragmentOffset() != 0) ) {
				bHaveLast = true;
				nAltExpectedSize = pFragment->GetIPHeader().GetFragmentOffset() + nPayloadSize;
			}
		}
		// Accumulate payload size taking into account the Security Id.
		nTotalPayloadSize += nPayloadSize;
	}
	bool bHaveBytes( (nTotalPayloadSize == nExpectedSize) || (nTotalPayloadSize == nAltExpectedSize) );
	return (bHaveFirst && bHaveLast && bHaveBytes);
}


// ----------------------------------------------------------------------------
//		Reassemble
// ----------------------------------------------------------------------------

bool
CIPv4PacketList::Reassemble(
	CByteArray&	outData )
{
	CUIntArray	iaSort;
	GetSortList( iaSort );

	outData.SetSize( 1518 );
	UInt16	nLength( static_cast<UInt16>( outData.GetSize() ) );
	UInt16	nCount( 0 );
	UInt8*	pData( outData.GetData() );
	size_t	nPacketCount = GetCount();
	for ( size_t i = 0; i < nPacketCount; i++ ) {
		CIPv4Packet*	pFragment( GetAt( iaSort[i] ) );
		ASSERT( pFragment );
		if ( pFragment == NULL ) continue;

		// Get the Data
		UInt16			nFragmentLength( 0 );
		const UInt8*	pFragmentData( NULL );
		// CMI Specialization (or just the fragmentation protocol):
		// The first packet has both IP and UDP Headers.
		// The rest only have IP Headers, but not UDP Headers.
		// CMI: the Fragment Offset could be used to reassemble packets (sorted and unsorted)
		//   but the offset is wrong. It is too large by 8.
		if ( (i == 0) && (pFragment->IsPacketType( CPacket::kPacketType_UDP )) ) {
			// This is the first packet.
			CUDPPacket*	pUdpPacket( static_cast<CUDPPacket*>( pFragment ) );
			nFragmentLength = pUdpPacket->GetUDPHeader().GetPayloadDataLength();
			pFragmentData = pUdpPacket->GetUDPHeader().GetPayload();

			// Adjust for the Security Id or Ericsson header.
			// CIPv4Packet has been modified to carry the Data Offset.
			UInt16	nDataOffset( pFragment->GetDataOffset() );
			if ( nDataOffset ) {
				nFragmentLength -= nDataOffset;
				pFragmentData += nDataOffset;
			}
		}
		else {
			// These are subsequent packets.
			nFragmentLength = pFragment->GetIPHeader().GetPayloadLength();
			pFragmentData = pFragment->GetIPHeader().GetPayload();
		}

		ASSERT( pFragmentData );
		if ( pFragmentData == NULL ) continue;

		if ( static_cast<size_t>( nCount + nFragmentLength ) > nLength ) {
			outData.SetSize( 0 );
			return false;
		}

		memcpy( pData, pFragmentData, nFragmentLength );
		pData += nFragmentLength;
		nCount += nFragmentLength;
	}
	outData.SetSize( nCount );
	return true;
}


// ============================================================================
//		CUDPPacketList
// ============================================================================

// ----------------------------------------------------------------------------
//		GetSortList
// ----------------------------------------------------------------------------

void
CUDPPacketList::GetSortList(
	CUIntArray&	outList )
{
	outList.SetSize( GetCount() );
	for ( size_t i = 0; i < GetCount(); i++ ) {
		outList[static_cast<int>( i )] = static_cast<UINT>( i );
	}
	bool	bSorted( true );
	do {
		bSorted = true;
		for ( size_t i = 1; i < GetCount(); i++ ) {
			UInt64	nLeftTime( GetAt( i - 1 )->GetTimeStamp() );
			UInt64	nRightTime( GetAt( i )->GetTimeStamp() );
			if ( nLeftTime > nRightTime ) {
				UINT	nTemp = outList[i];
				outList[i] = outList[i-1];
				outList[i-1] = nTemp;
				bSorted = false;
			}
		}
	} while ( !bSorted );
}

// ----------------------------------------------------------------------------
//		Reassemble
// ----------------------------------------------------------------------------

bool
CUDPPacketList::Reassemble(
	CByteArray&	outData )
{
	CUIntArray	iaSort;
	GetSortList( iaSort );

	outData.SetSize( 1518 );
	UInt16	nLength( static_cast<UInt16>( outData.GetSize() ) );
	UInt16	nCount( 0 );
	UInt8*	pData( outData.GetData() );
	for ( size_t i = 0; i < GetCount(); i++ ) {
		CUDPPacket*	pFragment( GetAt( iaSort[i] ) );
		ASSERT( pFragment );
		if ( pFragment == NULL ) continue;

		// Get the Data
		UInt16			nFragmentLength( 0 );
		const UInt8*	pFragmentData( NULL );
		// CMI Specialization: the first packet has both IP and UPD Headers.
		// The rest only have IP Headers.
		if ( i == 0 ) {
			nFragmentLength = pFragment->GetUDPHeader().GetPayloadDataLength();
			pFragmentData = pFragment->GetUDPHeader().GetPayload();
		}
		else {
			nFragmentLength = pFragment->GetIPHeader().GetPayloadLength();
			pFragmentData = pFragment->GetIPHeader().GetPayload();
		}

		ASSERT( pFragmentData );
		if ( pFragmentData == NULL ) continue;

		if ( static_cast<size_t>( nCount + nFragmentLength ) > nLength ) {
			outData.SetSize( 0 );
			return false;
		}

		memcpy( pData, pFragmentData, nFragmentLength );
		pData += nFragmentLength;
		nCount += nFragmentLength;
	}
	outData.SetSize( nCount );
	return true;
}
