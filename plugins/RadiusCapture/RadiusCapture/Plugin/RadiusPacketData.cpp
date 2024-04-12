// ============================================================================
//	RadiusPacketData.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "RadiusPacketData.h"


// ============================================================================
//	CRadiusPacketData
// ============================================================================

/*
// -----------------------------------------------------------------------------
//		NewMediaSpecInfoBlock
// -----------------------------------------------------------------------------

UInt8*
CRadiusPacketData::NewMediaSpecInfoBlock( MediaSpecificHeaderAll* theHeaderAll ) {
	if ( theHeaderAll == NULL ) {
		return NULL;
	}
	UInt8* pData = NULL;
	const size_t nUint32Size = sizeof(UInt32);
	UInt32 nSize = theHeaderAll->StdHeader.nSize;
	if ( nSize > nUint32Size + nUint32Size ) {
		pData = (UInt8*)malloc( nSize );
		if ( pData ) {
			memcpy( pData, theHeaderAll, nSize );
		}
		else {
			ASSERT( 0 );
		}
	}
	return pData;
}


// ============================================================================
//	CRadiusPacketDataMgr
// ============================================================================

// -----------------------------------------------------------------------------
//		AppendDataFromList
// -----------------------------------------------------------------------------

void
CRadiusPacketDataMgr::AppendDataFromList( CRadiusPacketDataMgr& theOtherPacketDataList ) 
{
	
	ASSERT( m_uLatestTimestamp != 0 && theOtherPacketDataList.m_uLatestTimestamp != 0 );

	if ( theOtherPacketDataList.m_uLatestTimestamp > m_uLatestTimestamp ) {
		const size_t nOtherCount = theOtherPacketDataList.GetCount();
		for ( size_t i = 0; i < nOtherCount; i++ ) {
			CRadiusPacketData& theOtherPacketDataItem = theOtherPacketDataList.GetAt( i );
			Add( theOtherPacketDataItem );
		}
		m_uLatestTimestamp = theOtherPacketDataList.m_uLatestTimestamp;
	}
	else {
		const size_t nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CRadiusPacketData& thePacketDataItem = GetAt( i );
			theOtherPacketDataList.Add( thePacketDataItem );
		}
		theOtherPacketDataList.m_uLatestTimestamp = m_uLatestTimestamp;
		*this = theOtherPacketDataList;
	}

	theOtherPacketDataList.Clean();
}
*/
