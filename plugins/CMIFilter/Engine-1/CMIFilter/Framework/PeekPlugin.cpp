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

	return PEEK_PLUGIN_SUCCESS;
}


#ifdef _WIN32
#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		SendPluginMessage
// -----------------------------------------------------------------------------

bool
CPeekPlugin::SendPluginMessage(
	CGlobalId		inContextId,
	CPeekStream&	inMessage,
	UInt32			inTimeout,
	UInt32&			outTransId )
{
	//if ( !IsInitialized() ) return false;

	// _ASSERTE( m_ConsoleSend.IsValid() );
	if ( m_ConsoleSend.IsNotValid() ) return false;

	try {
		const UInt8*	pMessage = nullptr;

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
#endif // _WIN32
