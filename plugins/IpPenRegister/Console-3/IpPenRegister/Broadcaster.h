// ============================================================================
//	Broadcaster.h
//		interface for the CBroadcaster class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "BLMessage.h"

class CListener;


// ============================================================================
//		CBroadcaster
// ============================================================================

class CBroadcaster
{
protected:
	bool					m_bBroadcasting;
	CAtlArray<CListener*>	m_Listeners;

	SInt32			FindListener( const CListener* inListener ) const;

public:
	;				CBroadcaster();
	;				CBroadcaster( const CBroadcaster& inOriginal );
	virtual			~CBroadcaster();

	void			AddListener( CListener* inListener );
	void			RemoveListener( CListener* inListener );

	virtual void	StartBroadcasting() { m_bBroadcasting = true; }
	virtual void	StopBroadcasting() { m_bBroadcasting = false; }
	bool			IsBroadcasting() const { return m_bBroadcasting; }

	bool			HasListener( CListener* inListener ) const;

	virtual void	BroadcastMessage( const CBLMessage& ioMessage );
};
