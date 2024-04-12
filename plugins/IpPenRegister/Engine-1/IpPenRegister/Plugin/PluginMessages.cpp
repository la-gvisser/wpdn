// =============================================================================
//	MessageOptions.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef IMPLEMENT_PLUGINMESSAGE

#include "PluginMessages.h"
#include "RemotePlugin.h"
#include "PeekEngineContext.h"


// =============================================================================
//	MessageFactory
// =============================================================================

CPeekMessagePtr
MessageFactory(
	const CPeekStream& inMessage )
{
	CPeekMessage*	pMessage( NULL );

	CPeekMessage	msgReceived( inMessage );
	CPeekString		strRoot = msgReceived.GetModelerRoot();

	if ( strRoot == CMessagePing::s_strMessageName ) {
		pMessage = new CMessagePing( inMessage );
	}

	return CPeekMessagePtr( pMessage );
}


// =============================================================================
//	CMessagePing
// =============================================================================

const CPeekString	CMessagePing::s_strMessageName( L"Ping" );

CMessagePing::CMessagePing()
	:	CPeekMessage( s_strMessageName )
{
}

#endif // IMPLEMENT_PLUGINMESSAGE
