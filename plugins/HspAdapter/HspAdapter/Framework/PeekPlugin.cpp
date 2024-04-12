// =============================================================================
//	PeekPlugin.cpp
// =============================================================================
//	Copyright (c) 2008-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekPlugin.h"
#include "RemotePlugin.h"
#ifdef IMPLEMENT_SUMMARY
#include "RemoteStatsPlugin.h"
#endif	// IMPLEMENT_SUMMARY


// =============================================================================
//		CPeekPlugin
// =============================================================================

int
CPeekPlugin::OnLoad(
	CRemotePlugin*	inPlugin )
{
	if ( !m_pRemotePlugin ) {
		m_pRemotePlugin = inPlugin;
	}

	CHePtr<IHeUnknown> spSite;
	inPlugin->GetSite( HE_IHEUNKNOWN_IID, (void**)&spSite.p );
	m_EngineProxy.SetInterfacePointers( spSite, inPlugin );

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		RegisterAdapter
// -----------------------------------------------------------------------------

bool
CPeekPlugin::RegisterAdapter(
	CAdapter	inAdapter )
{
	if ( inAdapter.IsNotValid() ) return false;

	try {
		CAdapterManager	adptManager( m_EngineProxy.GetAdapterManager() );
		if ( adptManager.IsNotValid() ) return false;

		CAdapterInfo	aiAdapterInfo( inAdapter.GetAdapterInfo() );
		UInt64			nLinkSpeed( aiAdapterInfo.GetLinkSpeed() );

		CAdapterConfig acConfig;
		acConfig.Create();
		acConfig.SetDefaultLinkSpeed( nLinkSpeed );
		acConfig.SetLinkSpeed( nLinkSpeed );
		acConfig.SetRingBufferSize( 0 );

		adptManager.AddAdapter( inAdapter, aiAdapterInfo, acConfig );
		// The adapter manager does not add a reference to the Adapter!
		inAdapter.AddReference();
	}
	catch( ... ) {
		return false;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		ReleaseAdapter
// -----------------------------------------------------------------------------

bool
CPeekPlugin::ReleaseAdapter(
	CAdapter	inAdapter )
{
	try {
		CAdapterManager	adptManager( m_EngineProxy.GetAdapterManager() );
		if ( adptManager.IsNotValid() ) return false;

		adptManager.RemoveAdapter( inAdapter );
		inAdapter = nullptr;
	}
	catch ( ... ) {
		return false;
	}

	return true;
}
