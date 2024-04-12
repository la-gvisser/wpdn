// ============================================================================
//	CMIPacketList.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#pragma once

#include "CMIPacket.h"
#include "PeekTime.h"
#include <list>
#include <map>
#include <memory>
#include <vector>

#define kMaxFragmentSize	1518


// ============================================================================
//		CPacketList
// ============================================================================

typedef std::vector<CCmiPacketPtr>	CCmiPacketPtrList;


// ============================================================================
//		CCmiIpV4PacketList
// ============================================================================

class CCmiIpV4PacketList
	: public std::vector<CCmiIpV4PacketPtr>
{
protected:
	UInt64	m_nTimeStamp;

public:
	;	CCmiIpV4PacketList() {}
	;	~CCmiIpV4PacketList() {}

	size_t	Add( CCmiIpV4PacketPtr&	inItem ) {
		tLongTime lt = CPeekTime::Now();
		m_nTimeStamp = lt.i;

		push_back( std::move( inItem ) );

		return (size() - 1);
	}

	UInt64	GetTimeStamp() const { return m_nTimeStamp; }

	bool	IsComplete() const;

	bool	Reassemble( std::vector<UInt8>& outPayload );

	void	Sort();
};


// ============================================================================
//		CCmiIpV4PacketListPtr
// ============================================================================

typedef std::unique_ptr<CCmiIpV4PacketList>		CCmiIpV4PacketListPtr;


// ============================================================================
//		CFragmentMap
// ============================================================================

class CFragmentMap
	:	public std::map<UInt64, CCmiIpV4PacketListPtr>
{
public:
	size_t	GetDeepCount() const {
		size_t	nCount( 0 );
		for ( auto itr( begin() ); itr != end(); ++itr ) {
			nCount += itr->second->size();
		}
		return nCount;
	}
};
