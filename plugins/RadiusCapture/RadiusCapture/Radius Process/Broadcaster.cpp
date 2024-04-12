// ============================================================================
//	Broadcaster.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2005. All rights reserved.
//	Copyright (c) AG Group, Inc. 1996-2000. All rights reserved.

#include "stdafx.h"
#include "Listener.h"
#include "Broadcaster.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

using namespace BLMessage;

// ----------------------------------------------------------------------------
//		CBroadcaster
// ----------------------------------------------------------------------------

CBroadcaster::CBroadcaster()
	: m_bBroadcasting( true )
{
}

CBroadcaster::CBroadcaster(
	const CBroadcaster&	inOriginal )
	:	m_bBroadcasting( inOriginal.m_bBroadcasting )
{
}

CBroadcaster::~CBroadcaster()
{
	CBLMessage msgNotify( kBLM_BroadcasterDied );

	// Notify listeners that the broadcaster is going away.
	BroadcastMessage( msgNotify );
//	BroadcastMessage( CBLMessage( kBLM_BroadcasterDied ) );

	// Detach from all listeners.
	size_t	nCount = m_Listeners.GetCount();
	for ( size_t i = 0; i < nCount; i++ ) {
		// Get the next listener.
		CListener*	pListener = m_Listeners.GetAt( i );
		ASSERT( pListener != NULL );
		if ( pListener != NULL ) {
			// Tell the listener to remove this broadcaster.
			pListener->RemoveBroadcaster( this );
		}
	}
	m_Listeners.RemoveAll();
}


// ----------------------------------------------------------------------------
//		FindListener
// ----------------------------------------------------------------------------

SInt32
CBroadcaster::FindListener(
	const CListener*	inListener ) const
{
	// Sanity check.
	ASSERT( inListener != NULL );

	size_t	nCount = m_Listeners.GetCount();
	for ( size_t i = 0; i < nCount; i++ ) {
		CListener*	pItem = m_Listeners.GetAt( i );
		if ( pItem == inListener ) {
			return static_cast<SInt32>( i );
		}
	}
	return -1;
}


// ----------------------------------------------------------------------------
//		AddListener
// ----------------------------------------------------------------------------

void
CBroadcaster::AddListener(
	CListener*	inListener )
{
	// Sanity check.
	ASSERT( inListener != NULL );
	if ( inListener == NULL ) return;

	// Try and find the listener already in the list.
	if ( !HasListener( inListener ) ) {
		// Add the listener to the list.
		m_Listeners.Add( inListener );

		// Hook up this broadcaster to the listener.
		inListener->AddBroadcaster( this );
	}
}


// ----------------------------------------------------------------------------
//		RemoveListener
// ----------------------------------------------------------------------------

void
CBroadcaster::RemoveListener(
	CListener*	inListener )
{
	// Sanity check.
	ASSERT( inListener != NULL );

	// Try and find the listener in the list.
	SInt32	nIndex = FindListener( inListener );

	if ( nIndex != -1 ) {
		// Remove the listener from the list.
		m_Listeners.RemoveAt( static_cast<size_t>( nIndex ) );

		// Unhook this broadcaster from listener.
		inListener->RemoveBroadcaster( this );
	}
}


// ----------------------------------------------------------------------------
//		HasListener
// ----------------------------------------------------------------------------

bool
CBroadcaster::HasListener(
	CListener*	inListener ) const
{
	// Sanity check.
	ASSERT( inListener != NULL );

	return (FindListener( inListener ) != -1);
}


// ----------------------------------------------------------------------------
//		BroadcastMessage
// ----------------------------------------------------------------------------

void
CBroadcaster::BroadcastMessage(
	CBLMessage&	ioMessage )
{
	if ( IsBroadcasting() ) {
		// Broadcast to all listening listeners.
		size_t	nCount = m_Listeners.GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			// Get the next listener.
			CListener*	pListener = m_Listeners.GetAt( i );
			ASSERT( pListener != NULL );

			// Check if the listener is listening.
			if ( (pListener != NULL) && pListener->IsListening() ) {
				// Pass the message to the listener.
				pListener->ListenToMessage( ioMessage );
			}
		}
	}
}
