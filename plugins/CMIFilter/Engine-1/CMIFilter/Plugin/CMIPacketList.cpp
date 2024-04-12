// ============================================================================
//	CMIPacketList.cpp
// ============================================================================
//	Copyright (c) Savvius, Inc. 2011-2017. All rights reserved.

#include "StdAfx.h"
#include "CMIPacketList.h"
#include <algorithm>
#include <vector>


// ============================================================================
//		CIPv4PacketList
// ============================================================================

const UInt16 kUdpHeaderLen( 8 );

bool
IPv4Compare(
	const CCmiIpV4PacketPtr& i,
	const CCmiIpV4PacketPtr& j )
{
	return (i->GetFragmentOffset() < j->GetFragmentOffset());
}


// ----------------------------------------------------------------------------
//		Sort
// ----------------------------------------------------------------------------

void
CCmiIpV4PacketList::Sort()
{
	if ( size() < 2 ) return;
	std::sort( begin(), end(), IPv4Compare );
}


// ----------------------------------------------------------------------------
//		IsComplete
// ----------------------------------------------------------------------------

bool
CCmiIpV4PacketList::IsComplete() const
{
	if ( empty() ) return false;

	if ( size() == 1 ) {
		_ASSERTE( front() != nullptr );
		if ( front() != nullptr ) {
			if ( front()->IsPacketType( CCmiUdpPacket::kPacketType_UDP ) ) {
				return !front()->IsMoreFragments();
			}
		}
		return false;
	}

	bool	bHaveFirst( false );
	bool	bHaveLast( false );
	UInt16	nTotPayloadLenPerFirstFrag( 0 );
	UInt16	nTotPayloadLenPerLastFragment( 0 );
	UInt16	nTotPayloadLenCalc( 0 );
	UInt16  nFragmentOffset( 0 );

	for ( auto itr( begin() ); itr != end(); ++itr ) {
		UInt16	nPayloadSize( 0 );
		ASSERT( *itr != nullptr );
		if ( *itr == nullptr ) return false;
		if ( (*itr)->GetPacketType() == CCmiUdpPacket::kPacketType_UDP ) {
			CCmiUdpPacket*	pUdpPacket( reinterpret_cast<CCmiUdpPacket*>( (*itr).get() ) );
			ASSERT( pUdpPacket );
			if ( pUdpPacket && pUdpPacket->IsMoreFragments() ) {
				bHaveFirst = true;
				nPayloadSize = pUdpPacket->GetPayloadDataLength();
				nTotPayloadLenPerFirstFrag = pUdpPacket->GetLengthField() - kUdpHeaderLen;
			}
		}
		else {
			nPayloadSize = (*itr)->GetPayloadLength();
			nFragmentOffset = (*itr)->GetFragmentOffsetBytes();

			if ( (!(*itr)->IsMoreFragments()) && (nFragmentOffset != 0) ) {
				bHaveLast = true;
				nTotPayloadLenPerLastFragment = nFragmentOffset + nPayloadSize - kUdpHeaderLen;
			}
		}
		nTotPayloadLenCalc += nPayloadSize;
	}

	bool bHaveBytes( (nTotPayloadLenCalc == nTotPayloadLenPerFirstFrag) ||
		             (nTotPayloadLenCalc == nTotPayloadLenPerLastFragment) );

#ifdef _DEBUG
	if ( bHaveBytes ) {
		ASSERT( nTotPayloadLenPerFirstFrag == nTotPayloadLenPerLastFragment );
	}
#endif // _DEBUG

	return (bHaveFirst && bHaveLast && bHaveBytes);
}


// ----------------------------------------------------------------------------
//		Reassemble
// ----------------------------------------------------------------------------

bool
CCmiIpV4PacketList::Reassemble(
	std::vector<UInt8>&	outPayload )
{
	Sort();

	outPayload.reserve( kMaxFragmentSize );

	for ( auto itr( begin() ); itr != end(); ++itr ) {
		ASSERT( *itr != nullptr );
		if ( *itr == nullptr ) continue;

		// Get the Data
		const UInt8*	pFragmentData( nullptr );
		UInt16			nFragmentLength( 0 );

		// The first packet has both IP and UPD Headers.
		// The rest only have IP Headers.
		if ( (itr == begin()) && ((*itr)->IsPacketType( CCmiUdpPacket::kPacketType_UDP )) ) {
			CCmiUdpPacket* pUdpPacket( static_cast<CCmiUdpPacket*>( (*itr).get() ) );
			if ( pUdpPacket == nullptr ) continue;

			pFragmentData = pUdpPacket->GetPayload();
			nFragmentLength = pUdpPacket->GetPayloadDataLength();

			UInt16	nDataOffset = (*itr)->GetDataOffset();
			if ( nDataOffset > 0 ) {
				nFragmentLength -= nDataOffset;
				pFragmentData += nDataOffset;
			}
		}
		else {
			pFragmentData = (*itr)->GetPayload();
			nFragmentLength = (*itr)->GetPayloadLength();
		}

		ASSERT( nFragmentLength < kMaxFragmentSize );
		if ( nFragmentLength > kMaxFragmentSize ) continue;
		const UInt8*	pFragmentEnd( pFragmentData + nFragmentLength );

		ASSERT( pFragmentData );
		if ( pFragmentData == nullptr ) continue;

		outPayload.insert( outPayload.end(), pFragmentData, pFragmentEnd );
	}
	return true;
} 
