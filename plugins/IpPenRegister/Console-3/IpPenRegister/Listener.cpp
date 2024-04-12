// ============================================================================
//	Listener.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "stdafx.h"
#include "Broadcaster.h"
#include "Listener.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif


// ============================================================================
//		CListener
// ============================================================================

CListener::CListener()
	:	m_bListening( true )
{
}

CListener::CListener(
	const CListener&	inOriginal )
{
	m_bListening = inOriginal.m_bListening;
}

CListener::~CListener()
{
	// Detach from all broadcasters.
	size_t	nCount = m_Broadcasters.GetCount();
	for ( size_t i = 0; i < nCount; i++ ) {
		// Get the next broadcaster.
		CBroadcaster*	pBroadcaster = m_Broadcasters.GetAt( i );
		ASSERT( pBroadcaster != NULL );
		if ( pBroadcaster != NULL ) {
			// Tell the broadcaster to remove this listener.
			pBroadcaster->RemoveListener( this );
		}
	}
	m_Broadcasters.RemoveAll();
}


// ----------------------------------------------------------------------------
//		FindBroadcaster
// ----------------------------------------------------------------------------

SInt32
CListener::FindBroadcaster(
	const CBroadcaster*	inBroadcaster ) const
{
	// Sanity check.
	ASSERT( inBroadcaster != NULL );

	size_t	nCount = m_Broadcasters.GetCount();
	for ( size_t i = 0; i < nCount; i++ ) {
		CBroadcaster*	pItem = m_Broadcasters.GetAt( i );
		if ( pItem == inBroadcaster ) {
			return static_cast<SInt32>( i );
		}
	}
	return -1;
}


// ----------------------------------------------------------------------------
//		HasBroadcaster
// ----------------------------------------------------------------------------

bool
CListener::HasBroadcaster(
	CBroadcaster*	inBroadcaster ) const
{
	// Sanity check.
	ASSERT( inBroadcaster != NULL );

	return (FindBroadcaster( inBroadcaster ) != -1);
}


// ----------------------------------------------------------------------------
//		AddBroadcaster
// ----------------------------------------------------------------------------

void
CListener::AddBroadcaster(
	CBroadcaster*	inBroadcaster )
{
	// Sanity check.
	ASSERT( inBroadcaster != NULL );
	if ( inBroadcaster == NULL ) return;

	// Try and find the broadcaster already in the list.
	if ( !HasBroadcaster( inBroadcaster ) ) {
		// Add the broadcaster to the list.
		m_Broadcasters.Add( inBroadcaster );
	}
}


// ----------------------------------------------------------------------------
//		RemoveBroadcaster
// ----------------------------------------------------------------------------

void
CListener::RemoveBroadcaster(
	CBroadcaster*	inBroadcaster )
{
	// Sanity check.
	ASSERT( inBroadcaster != NULL );

	// Try and find the broadcaster in the list.
	SInt32	nIndex = FindBroadcaster( inBroadcaster );

	if ( nIndex != -1 ) {
		// Remove the broadcaster from the list.
		m_Broadcasters.RemoveAt( static_cast<size_t>( nIndex ) );
	}
}


// ----------------------------------------------------------------------------
//		ListenToMessage
// ----------------------------------------------------------------------------

void
CListener::ListenToMessage(
	const CBLMessage&	ioMessage )
{
	switch ( ioMessage.GetId() ) {
	case kBLM_BroadcasterDied:
		break;
	}
}
