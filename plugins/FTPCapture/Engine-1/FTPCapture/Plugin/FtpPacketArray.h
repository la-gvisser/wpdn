// =============================================================================
//	FtpPacketArray.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekArray.h"
#include "Packet.h"
#include "CaptureBuffer.h"

typedef NSTR1::shared_ptr<CPacket>	PacketPtr;

typedef struct _FtpPacket
{
	PacketPtr		spPacket;
	UInt32			nSequenceNumber;
	UInt16			nPayloadLength;
	CByteVectRef	DataRef;
	bool			bDeleted;

	_FtpPacket() : nSequenceNumber( 0 ), nPayloadLength( 0 ), bDeleted( false ) {}

}	FtpPacket;

typedef NSTR1::shared_ptr<FtpPacket>	FtpPacketPtr;

// =============================================================================
//		CFtpPacketArray
// =============================================================================

class CFtpPacketArray
	: public CAtlArray<FtpPacketPtr>
{

public:
	;			CFtpPacketArray() {}
	virtual		~CFtpPacketArray() { RemoveAll(); }

	bool		Add( const CPacket&	inPacket, CByteVectRef& inTcpData, bool	inIsFin );

	bool		Sort();

	bool		Verify();
};
