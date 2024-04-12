// =============================================================================
//	PeekPlugin.cpp
// =============================================================================
//	Copyright (c) 2008-2017 Savvius, Inc. All rights reserved.

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

[!if !NOTIFY_UIHANDLER]
#ifdef IMPLEMENT_PLUGINMESSAGE
	if ( m_pRemotePlugin ) {
		if ( m_pRemotePlugin->GetPeekProxy().IsEngine() ) {
			m_nOnEngine = kOnEngine_True;	// Plugin is on an Engine.
		}
	}
#endif // IMPLEMENT_PLUGINMESSAGE

[!endif]
	return PEEK_PLUGIN_SUCCESS;
}
[!if !NOTIFY_UIHANDLER]


#if defined(_WIN32) && defined(IMPLEMENT_PLUGINMESSAGE)
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
#endif // _WIN32 && IMPLEMENT_PLUGINMESSAGE
[!endif]
