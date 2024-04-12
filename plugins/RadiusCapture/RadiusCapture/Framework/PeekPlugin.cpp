// =============================================================================
//	PeekPlugin.cpp
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekPlugin.h"
#include "RemotePlugin.h"


// =============================================================================
//		CPeekPlugin
// =============================================================================

int
CPeekPlugin::OnLoad(
	CRemotePlugin*	inPlugin )
{
	m_pRemotePlugin = inPlugin;

#ifdef IMPLEMENT_PLUGINMESSAGE
	if ( m_pRemotePlugin ) {
		if ( m_pRemotePlugin->GetPeekProxy().IsEngine() ) {
			m_nOnEngine = kOnEngine_True;	// Plugin is on an Engine.
		}
	}
#endif // IMPLEMENT_PLUGINMESSAGE

	return PEEK_PLUGIN_SUCCESS;
}


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		SendPluginMessage
// -----------------------------------------------------------------------------

bool
CPeekPlugin::SendPluginMessage(
	CGlobalId		inContextId,
	CPeekStream&	inMessage,
	DWORD			inTimeout,
	DWORD&			outTransId )
{
	//if ( !IsInitialized() ) return false;

	// _ASSERTE( m_ConsoleSend.IsValid() );
	if ( m_ConsoleSend.IsNotValid() ) return false;

	try {
		const UInt8*	pMessage = NULL;

		size_t cbLength = inMessage.GetLength();
		if ( (cbLength > 0) && (cbLength < kMaxUInt32)) {
			pMessage = inMessage.GetData();
			_ASSERTE( pMessage );
			if ( !pMessage ) return true;
			outTransId = m_ConsoleSend.SendPluginMessage(
				COmniPlugin::GetClassId(), inContextId, m_pRemotePlugin,
				static_cast<UInt32>( cbLength ), const_cast<UInt8*>( pMessage ),
				inTimeout );
		}
	}
	catch( ... ) {
		return false;
	}

	return true;
}
#endif // IMPLEMENT_PLUGINMESSAGE
