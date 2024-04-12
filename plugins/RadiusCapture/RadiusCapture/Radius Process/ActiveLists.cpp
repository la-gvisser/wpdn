// ============================================================================
//	ActiveLists.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "ActiveLists.h"


// =============================================================================
//		CActiveRadiusPortsList
// =============================================================================

// -----------------------------------------------------------------------------
//		RebuildList
// -----------------------------------------------------------------------------

void
CActiveRadiusPortsList::RebuildList( CCaseOptionsList& inCaseOptionsList ) 
{
	Reset();

	const size_t nCaseCount = inCaseOptionsList.GetCount();

	for ( size_t i = 0; i < nCaseCount; i++ ) {
		CCaseOptions* theCaseOptions = inCaseOptionsList.GetAt( i );
		if ( theCaseOptions->IsActive() ) {
			CPortNumberList& thePortNumberList = theCaseOptions->GetPortNumberList();

			const size_t nPortCount = thePortNumberList.GetCount();

			for ( size_t i = 0; i < nPortCount; i++ ) {
				CPortNumber* pPortNumber = thePortNumberList.GetAt( i );
				ASSERT( pPortNumber );
				if ( pPortNumber->IsEnabled() ) {
					UInt16 nPort = pPortNumber->GetPort();
					if ( !IsInList( nPort ) ) {
						Add( nPort );
					}
				}
			}
		}
	}
}


// =============================================================================
//		CActiveUsersList
// =============================================================================

// -----------------------------------------------------------------------------
//		RebuildList
// -----------------------------------------------------------------------------

void 
CActiveUsersList::RebuildList( CCaseOptionsList& theCaseOptionsList ) 
{
	Reset();

	for ( size_t i = 0; i < theCaseOptionsList.GetCount(); i++ ) {
		CCaseOptions* theCaseOptions = theCaseOptionsList.GetAt( i );
		if ( theCaseOptions->IsActive() == false ) {
			continue;
		}

		CUserNameList& theUserList = theCaseOptions->GetUserList();
		for ( size_t j = 0; j < theUserList.GetCount(); j++ ) {
			const CUserName& theUserItem = theUserList.GetAt( j );
			if ( theUserItem.IsEnabled() ) {
				const CPeekString& theUserName = theUserItem.GetName();
				if ( !IsUserAnActiveTarget( theUserName ) ) {
					CActiveUser theActiveUser( theUserName );
					Add( theActiveUser );
				}
			}
		}
	}
}
