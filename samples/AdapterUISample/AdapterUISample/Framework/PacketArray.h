// =============================================================================
//	PacketArray.h
// =============================================================================
//	Copyright (c) 2009-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "Packet.h"
#include "CaptureBuffer.h"
#include <vector>

using std::vector;


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	CPacketArray
//
//	A Packet Array is a simple in memory array of Packets. Use the Load method
//	to retrieve all the Packets in a Capture Buffer. The Packets will all be
//	Attached to the Capture Buffer. The Detach method will detach all the
//	(attached) Packets in the array.
//
//	Use the Capture File class to write the Packets to a file.
//
//	Use a Capture Buffer to efficiently manage a large number packets in memory.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		CPacketArray
// =============================================================================

class CPacketArray
	: public vector<CPacket>
{
public:
	enum {
		kType_Mixed,
		kType_Attached,
		kType_Detached
	};

protected:
	int		m_Type;

public:
	;			CPacketArray( int inType = kType_Mixed ) : m_Type( inType ) {}
	virtual		~CPacketArray() { clear(); }

	CPacketArray&	operator=( const CPacketArray& inOther ) {
		if ( this != &inOther ) {
			Copy( inOther );
		}
		return *this;
	}

	size_t			Add( const CPacket& inPacket );
	CPacketArray&	Append( const CPacketArray& inOther );

	CPacketArray&	Copy( const CPacketArray& inOther );

	void			Detach();

	CPacket&		Get( size_t inIndex, bool inDetached = false ) {
		CPacket&	thePacket = at( inIndex );
		if ( inDetached && thePacket.IsAttached() ) {
			thePacket.Detach();

		}
		return thePacket;
	}
	int				GetType() const { return m_Type; }

	size_t			Insert( size_t inPosition, const CPacket& inPacket );
	bool			IsAttached() const { return (m_Type == kType_Attached); }
	bool			IsDetached() const { return (m_Type == kType_Detached); }
	bool			IsMixed() const { return (m_Type == kType_Mixed); }

	size_t			Load( const CCaptureBuffer& inPacketBuffer );

	bool			Replace( size_t inPosition, const CPacket& inPacket );
};
