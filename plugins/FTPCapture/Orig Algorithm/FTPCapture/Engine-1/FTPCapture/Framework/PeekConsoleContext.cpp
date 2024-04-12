// =============================================================================
//	PeekConsoleContext.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekConsoleContext.h"
#include "RemotePlugin.h"


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		DoSendPluginMessage
// -----------------------------------------------------------------------------

bool
CPeekConsoleContext::DoSendPluginMessage( 
	CPeekStream&	inMessage,
	DWORD			inTimeout,
	DWORD&			outTransId )
{
	ASSERT( m_pPlugin );		
	if ( !m_pPlugin ) return false;

	HeResult hr = m_pPlugin->SendPluginMessage( m_Id, inMessage, inTimeout, outTransId );
	return (hr == HE_S_OK);
}
#endif // IMPLEMENT_PLUGINMESSAGE
