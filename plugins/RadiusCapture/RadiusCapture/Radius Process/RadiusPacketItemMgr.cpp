// ============================================================================
// RadiusPacketItemMgr.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "RadiusPacketItemMgr.h"


// ============================================================================
// CRadiusPacketItemMgr
// ============================================================================


#ifndef TEST_NO_MONITORING
// -----------------------------------------------------------------------------
//		CleanupExpired
// -----------------------------------------------------------------------------

bool
CRadiusPacketItemMgr::CleanupExpired()
{
	POSITION thePos = m_listRadiusPacketItems.GetHeadPosition();
	if ( !thePos ) return false;

	POSITION theCurrentPos;
	bool	 bExpiredCleaned = false;

	CAtlArray<POSITION>	Morgue;

	time_t theTime;
	time( &theTime );

	while ( thePos ) {
		theCurrentPos = thePos;
		CRadiusPacketItem& theRadiusPacket = m_listRadiusPacketItems.GetNext( thePos );

		time_t	timeOut = theRadiusPacket.GetLastUpdateTime() + RADIUS_PACKET_TIMEOUT;
		if( timeOut < theTime ) {
			Morgue.Add( theCurrentPos );
		}
	}

	// Remove the items in reverse order.
	int nCount = static_cast<int>( Morgue.GetCount() );
	if ( nCount > 0 ) {
		for ( int i = nCount - 1; i >= 0; i-- ) {
			POSITION thePos = Morgue[i];
			ASSERT( thePos );
#ifdef _DEBUG
			CRadiusPacketItem& theRadiusPacket = m_listRadiusPacketItems.GetAt( thePos );
			time_t	timeOut = theRadiusPacket.GetLastUpdateTime() + RADIUS_PACKET_TIMEOUT;
			ASSERT( timeOut < theTime );
#endif
			m_listRadiusPacketItems.RemoveAt( thePos );
		}
		bExpiredCleaned = true;
	}

	return bExpiredCleaned;
}
#endif // TEST_NO_MONITORING
