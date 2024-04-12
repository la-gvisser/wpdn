// ============================================================================
//	Stats.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#ifdef IMPLEMENT_SUMMARYMODIFYENTRY

#ifndef STATS_H
#define STATS_H

#include "MediaTypes.h"
#include "PeekTime.h"

class CPacket;

class CStats
{
public:
					CStats() {}
	virtual			~CStats() {}

	virtual void	ProcessPacket( const CPacket& inPacket ) = 0;
	virtual void	ProcessNoPackets( const WPTime /*inPacketTime*/ ) { }
	virtual void	ProcessNoPackets( const WPTime inPacketTime, const bool inIsCapturing );

	virtual void	Reset() = 0;
	virtual void	Reset( TMediaType inMediaType, TMediaSubType inSubType ) = 0;
};

#endif

#endif // IMPLEMENT_SUMMARYMODIFYENTRY
