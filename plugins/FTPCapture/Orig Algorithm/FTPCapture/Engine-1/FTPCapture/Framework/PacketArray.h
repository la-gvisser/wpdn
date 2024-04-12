// =============================================================================
//	PacketArray.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekArray.h"
#include "Packet.h"
#include "CaptureBuffer.h"


// =============================================================================
//		CPacketArray
// =============================================================================

class CPacketArray
	: public CPeekArray<CPacket>
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
	virtual		~CPacketArray() { RemoveAll(); }

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
		CPacket&	thePacket = GetAt( inIndex );
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
