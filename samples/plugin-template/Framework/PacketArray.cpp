// =============================================================================
//	PacketArray.cpp
// =============================================================================
//	Copyright (c) 2009-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "PacketArray.h"
#include <vector>

using std::vector;


// =============================================================================
//		CPacketArray
// =============================================================================

// -----------------------------------------------------------------------------
//		Add
// -----------------------------------------------------------------------------

size_t
CPacketArray::Add(
	const CPacket&	inPacket )
{
	this->push_back( inPacket );
	size_t	nIndex = this->size() - 1;
	if ( IsDetached() ) {
		CPacket&	thePacket = this->at( nIndex );
		if ( thePacket.IsAttached() ) {
			thePacket.Detach();
		}
	}
	return nIndex;
}


// -----------------------------------------------------------------------------
//		Append
//
//	Append the contents of one array to the other
// -----------------------------------------------------------------------------

CPacketArray&
CPacketArray::Append(
	const CPacketArray&	inOther )
{
	size_t	nCount = inOther.size();
	for ( size_t i = 0; i < nCount; i++ ) {
		Add( inOther.at( i ) );
	}
	return *this;
}


// -----------------------------------------------------------------------------
//		Copy
// -----------------------------------------------------------------------------

CPacketArray&
CPacketArray::Copy(
	const CPacketArray&	inOther )
{
	clear();
	return Append( inOther );
}


// -----------------------------------------------------------------------------
//		Detach
//
//	Detach all the packets in the array and set the type to Detached.
// -----------------------------------------------------------------------------

void
CPacketArray::Detach()
{
	size_t	nCount = size();
	for ( size_t i = 0; i < nCount; i++ ) {
		CPacket&	thePacket( at( i ) );
		if ( thePacket.IsAttached() ) {
			thePacket.Detach();
		}
	}
	m_Type = kType_Detached;
}


// -----------------------------------------------------------------------------
//		Insert
//
//	Insert a packet into the array.
//  If inPosition is outside the bounds of the array, then the packet is added.
// -----------------------------------------------------------------------------

size_t
CPacketArray::Insert(
	size_t			inPosition,
	const CPacket&	inPacket )
{
	if ( inPosition > size() ) return Add( inPacket );
	vector<CPacket>::insert( (begin() + inPosition), inPacket );
	if ( IsDetached() ) {
		CPacket&	thePacket = at( inPosition );
		if ( thePacket.IsAttached() ) {
			thePacket.Detach();
		}
	}
	return inPosition;
}


// -----------------------------------------------------------------------------
//		Load
//
//	Rebuild the array from a capture buffer.
//	Returns the number of packets added to the array.
// -----------------------------------------------------------------------------

size_t
CPacketArray::Load(
	const CCaptureBuffer&	inPacketBuffer )
{
	clear();
	const UInt32 nCount = inPacketBuffer.GetCount();
	CPacket thePacket;
	for ( UInt32 i = 0; i < nCount; i++ ) {
		Add( inPacketBuffer.GetPacket( i ) );
	}
	return size();
}


// -----------------------------------------------------------------------------
//		Replace
//
//	Replace a packet at the specified position.
// -----------------------------------------------------------------------------

bool
CPacketArray::Replace(
	size_t			inPosition,
	const CPacket&	inPacket )
{
	if ( inPosition >= size() || !inPacket.IsValid() ) return false;
	CPacket& thePacket( at( inPosition ) );
	if ( IsDetached() ) {
		if ( thePacket.IsAttached() ) {
			thePacket.Detach();
		}
	}
	return true;
}
