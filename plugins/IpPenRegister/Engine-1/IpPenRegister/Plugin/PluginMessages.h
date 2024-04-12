// =============================================================================
//	PluginMessages.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef IMPLEMENT_PLUGINMESSAGE

#include "GlobalId.h"
#include "PeekMessage.h"
#include "Options.h"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Plugin Message
//
//	The plugin specific messages that a plugin sends and receives. All Plugin
//	Messages are sub-classed from Peek Message.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		MessageFactory
// =============================================================================

CPeekMessagePtr	MessageFactory( const CPeekStream& inMessage );


// =============================================================================
//		CMessagePing
// =============================================================================

class CMessagePing
	:	public CPeekMessage
{
public:
	static const int			s_nMessageType = 'PING';
	static const CPeekString	s_strMessageName;

protected:
	// Message

	// Response

public:
	;		CMessagePing();
	;		CMessagePing( const CPeekStream& inMessage )
		: CPeekMessage( inMessage )
	{
	}

	bool			StartResponse() { return CPeekMessage::StartResponse( s_strMessageName ); }

	// Factory Interface
	virtual int					GetType() { return s_nMessageType; }
	virtual const CPeekString&	GetName() { return s_strMessageName; }
};

#endif // IMPLEMENT_PLUGINMESSAGE
