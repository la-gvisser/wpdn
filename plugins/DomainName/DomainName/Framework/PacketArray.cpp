// =============================================================================
//	PacketArray.cpp
// =============================================================================
//	Copyright (c) 2009-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PacketArray.h"


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
	size_t	nIndex = CPeekArray<CPacket>::Add( inPacket );
	if ( IsDetached() ) {
		CPacket&	thePacket = GetAt( nIndex );
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
	size_t	nCount = inOther.GetCount();
	for ( size_t i = 0; i < nCount; i++ ) {
		Add( inOther.GetAt( i ) );
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
	RemoveAll();
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
	size_t	nCount = GetCount();
	for ( size_t i = 0; i < nCount; i++ ) {
		CPacket&	thePacket( GetAt( i ) );
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
	if ( inPosition > GetCount() ) return Add( inPacket );
	CPeekArray<CPacket>::InsertAt( inPosition, inPacket );
	if ( IsDetached() ) {
		CPacket&	thePacket = GetAt( inPosition );
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
	RemoveAll();
	const UInt32 nCount = inPacketBuffer.GetCount();
	CPacket thePacket;
	for ( UInt32 i = 0; i < nCount; i++ ) {
		Add( inPacketBuffer.GetPacket( i ) );
	}
	return GetCount();
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
	if ( inPosition >= GetCount() || !inPacket.IsValid() ) return false;
	CPacket& thePacket( GetAt( inPosition ) );
	if ( IsDetached() ) {
		if ( thePacket.IsAttached() ) {
			thePacket.Detach();
		}
	}
	return true;
}
