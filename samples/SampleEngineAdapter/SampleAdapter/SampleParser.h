// =============================================================================
//	SampleParser.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "StdAfx.h"
#include "peekcore.h"
#include "PeekPacketImpl.h"
#include <vector>
#include <map>


// =============================================================================
//		CSampleContext
// =============================================================================

class CSampleParser
{
protected:
	CHeObjStack<Packet>		m_DispatchPacket;
	IPacketHandler*			m_pPackethandler;
	std::vector< CHePtr<IPacketHandler> > m_vecPackethandler;	

	CHeCriticalSection		m_heCritSection;
	std::map<HeID, UInt32>	m_mapCapIdToIp;

protected:
	void	GetCaptureSrcIP( const Helium::HeID& capId, UInt32* pSrcIP );
	void	ProcessBuffer( UInt8* CiscoBuf, UInt32 nBufLen, UInt32 nRecvAddress );
	void	RemoveCaptureSrcIP( const Helium::HeID& capId );
	void	SetCaptureSrcIP( const Helium::HeID& capId, UInt32 nSrcIP );

public:
	;		CSampleParser() : m_pPackethandler( 0 ) {}
	;		~CSampleParser() {}
};
