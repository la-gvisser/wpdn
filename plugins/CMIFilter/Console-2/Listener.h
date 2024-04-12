// ============================================================================
//	Listener.h
//		interface for the CListener class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "BLMessage.h"

class CBroadcaster;


// ============================================================================
//		CListener
// ============================================================================

class CListener
{
	friend class CBroadcaster;

protected:
	bool						m_bListening;
	CAtlArray<CBroadcaster*>	m_Broadcasters;

	void			AddBroadcaster( CBroadcaster* inBroadcaster );
	void			RemoveBroadcaster( CBroadcaster* inBroadcaster );

	SInt32			FindBroadcaster( const CBroadcaster* inBroadcaster ) const;

public:
	;				CListener();
	;				CListener( const CListener& inOriginal );
	virtual			~CListener();

	virtual void	StartListening() { m_bListening = true; }
	virtual void	StopListening() { m_bListening = false; }
	bool			IsListening() const { return m_bListening; }

	bool			HasBroadcaster( CBroadcaster* inBroadcaster ) const;

	virtual void	ListenToMessage( CBLMessage& ioMessage ) = 0;
};
