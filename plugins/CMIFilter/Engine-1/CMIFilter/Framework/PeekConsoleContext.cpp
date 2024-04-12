// =============================================================================
//	PeekConsoleContext.cpp
// =============================================================================
//	Copyright (c) 2009-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef _WIN32

#include "PeekConsoleContext.h"
#include "ContextManager.h"
#include "OmniPlugin.h"
#include "RemotePlugin.h"

extern CContextManagerPtr	GetContextManager();


// =============================================================================
//		CPeekConsoleContext
// =============================================================================

#ifdef IMPLEMENT_PLUGINMESSAGE
// ----------------------------------------------------------------------------
//		IsInstalledOnEngine
// ----------------------------------------------------------------------------

bool
CPeekConsoleContext::IsInstalledOnEngine() const
{
	return ((m_pPlugin != nullptr) && m_pPlugin->IsInstalledOnEngine());
}


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
	_ASSERTE( m_pPlugin );
	if ( !m_pPlugin ) return false;

	return m_pPlugin->SendPluginMessage( m_Id, inMessage, inTimeout, outTransId );
}
#endif // IMPLEMENT_PLUGINMESSAGE

#endif // _WIN32

