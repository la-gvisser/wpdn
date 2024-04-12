// =============================================================================
//	PeekAdapter.cpp
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekAdapter.h"
#include <map>

using std::map;


// =============================================================================
//		CIdIpMap
// =============================================================================

void
CIdIpMap::Add(
	const CGlobalId		inId,
	const CIpAddress&	inAddress )
{
	map<CGlobalId, CIpAddress>::iterator mapIter;
	mapIter = find( inId );

	if ( mapIter == end() ) {
		insert( std::make_pair( inId, inAddress ) );
	}
	else {
		mapIter->second = inAddress;
	}
}

bool
CIdIpMap::Find(
	const CGlobalId	inId,
	CIpAddress&		outAddress ) const
{
	map<CGlobalId, CIpAddress>::const_iterator	mapIter;
	mapIter = find( inId );
	if ( mapIter == end() ) return false;

	outAddress = mapIter->second;
	return true;
}

void
CIdIpMap::Remove(
	const CGlobalId	inId )
{
	map<CGlobalId, CIpAddress>::iterator mapIter;
	mapIter = find( inId );

	if ( mapIter != end() ) {
		erase( mapIter );
	}
}


// =============================================================================
//		CPeekAdapter
// =============================================================================

// -----------------------------------------------------------------------------
//		HandlePacket
// -----------------------------------------------------------------------------

int
CPeekAdapter::HandlePacket(
	const CPacket&		inPacket,
	const CIpAddress&	inAddress )
{
	CPacketHandlerIndex	item = m_ayPacketHandlers.begin();
	for ( ; item != m_ayPacketHandlers.end(); item++ ) {
		CPeekCapture	theCapture( item->GetPeekCapture() );
		if ( theCapture.IsNotValid() ) continue;

		CGlobalId	idCapture = theCapture.GetId();

		// New Code
		CIpAddress	ipAddress;
		if ( !m_CaptureIdToIpAddress.Find( idCapture, ipAddress ) ) {
			continue;
		}

		if ( ipAddress.IsNull() || (ipAddress == inAddress) ) {
			HRESULT hr = item->HandlePacket( inPacket );
			if ( HE_FAILED( hr ) ) {
				;
			}
		}
	}

	return PEEK_PLUGIN_SUCCESS;
}

