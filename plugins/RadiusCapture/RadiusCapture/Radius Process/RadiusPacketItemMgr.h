// ============================================================================
// RadiusPacketItemMgr.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "RadiusPacketItem.h"


// ============================================================================
// CRadiusPacketItemMgr
// ============================================================================

class CRadiusPacketItemMgr
{

public:

	CRadiusPacketItemMgr()
		: m_pActiveUsersList( NULL )
	{}

	~CRadiusPacketItemMgr() {
		RemoveAll();
	}

	inline void Add( CRadiusPacketItem& theItem );

	void Initialize( CActiveUsersList& inActiveUsersList ) {
		RemoveAll();
		m_pActiveUsersList = &inActiveUsersList;
	}
	POSITION GetFirstPosition() {
		return m_listRadiusPacketItems.GetHeadPosition();
	}
	CRadiusPacketItem& GetNextRadiusPacktetItem( POSITION& pos ) {
		return m_listRadiusPacketItems.GetNext( pos );
	}
	void CleanupItemsForUnusedNames( COptions& inOptions ) {
		POSITION thePos = m_listRadiusPacketItems.GetHeadPosition();
		POSITION theCurrentPos;
		while ( thePos ) {
			theCurrentPos = thePos;
			CRadiusPacketItem& theListPacketItem = m_listRadiusPacketItems.GetNext( thePos );
			CPeekString strUserName = theListPacketItem.GetRadiusUserName();
			if ( inOptions.IsUserAnActiveTarget(strUserName ) == false ) {
				Remove( theCurrentPos );
			}
		}
	}
	void Remove( POSITION thePos ) { 
		m_listRadiusPacketItems.RemoveAt( thePos );	
	}
	void RemoveAll() { 
		m_listRadiusPacketItems.RemoveAll();	
	}
	size_t GetPacketItemCount() {
		return m_listRadiusPacketItems.GetCount();
	}
	void SetAt( POSITION thePos, CRadiusPacketItem& theItem ) {
		m_listRadiusPacketItems.SetAt( thePos, theItem );
	}

  #ifndef TEST_NO_MONITORING
	bool CleanupExpired();
  #endif

protected:
	CAtlList< CRadiusPacketItem >	m_listRadiusPacketItems;
	CActiveUsersList*				m_pActiveUsersList;

private:
	// Disallow copy constructor
	CRadiusPacketItemMgr( const CRadiusPacketItemMgr& OtherItemMgr ) {
		OtherItemMgr;
		ASSERT( 0 );
	}
	CRadiusPacketItemMgr& operator=( CRadiusPacketItemMgr& OtherItemMgr ) {
		OtherItemMgr;
		ASSERT( 0 );
	}
};

inline void CRadiusPacketItemMgr::Add( CRadiusPacketItem& theAddItem )
{
	POSITION thePos = 0;

	theAddItem.SetLastUpdateTime();
	thePos = m_listRadiusPacketItems.AddTail( theAddItem );
	ASSERT( thePos != 0 );
}
