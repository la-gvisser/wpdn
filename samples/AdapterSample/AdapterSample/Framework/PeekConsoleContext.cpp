// =============================================================================
//	PeekConsoleContext.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekConsoleContext.h"
#include "OmniPlugin.h"
#include "RemotePlugin.h"


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		PostPluginMessage
// -----------------------------------------------------------------------------

bool
CPeekConsoleContext::PostPluginMessage(
	CPeekMessagePtr			inMessage,
	CPeekMessageProcess*	inMsgProc,
	FnResponseProcessor		inResponseProc )
{
	return m_MessageQueue.PostPluginMessage( inMessage, inMsgProc, inResponseProc );
}


// -----------------------------------------------------------------------------
//		SendPluginMessage
// -----------------------------------------------------------------------------

bool
CPeekConsoleContext::SendPluginMessage(
	CPeekStream&	inMessage,
	DWORD			inTimeout,
	DWORD&			outTransId )
{
	ASSERT( m_pPlugin );
	if ( !m_pPlugin ) return false;

	return m_pPlugin->SendPluginMessage( m_Id, inMessage, inTimeout, outTransId );
}
#endif // IMPLEMENT_PLUGINMESSAGE
