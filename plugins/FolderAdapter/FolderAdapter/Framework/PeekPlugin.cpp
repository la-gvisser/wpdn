// =============================================================================
//	PeekPlugin.cpp
// =============================================================================
//	Copyright (c) 2008-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekPlugin.h"
#include "RemotePlugin.h"
#include <algorithm>
#include <sys/stat.h>
#include <fcntl.h>


// =============================================================================
//		IsValidPointer
// =============================================================================

bool
IsValidPointer(
	void*	inPointer )
{
	// https://stackoverflow.com/questions/551069/testing-pointers-for-validity-c-c
	if ( inPointer ) {
		int fh = open( reinterpret_cast<const char *>( inPointer ), 0, 0 );
		int e = errno;
		if ( (fh == -1) && (e == EFAULT) ) return false;

		if ( fh != -1 ) {
			close( fh );
		}

		return true;
	}

	return false;
}


// =============================================================================
//		CPeekPlugin
// =============================================================================

int
CPeekPlugin::OnLoad(
	CRemotePlugin*	inPlugin )
{
	PeekLock lock( m_Mutex );
	m_ayRemotePlugins.push_back( inPlugin );

	CHePtr<IHeUnknown> spSite;
	inPlugin->GetSite( HE_IHEUNKNOWN_IID, (void**)&spSite.p );
	m_EngineProxy.SetInterfacePointers( spSite, inPlugin );

	return PEEK_PLUGIN_SUCCESS;
}

int
CPeekPlugin::OnUnload(
	CRemotePlugin*	inPlugin )
{
	PeekLock lock( m_Mutex );
	auto	itr = std::find( m_ayRemotePlugins.begin(), m_ayRemotePlugins.end(), inPlugin );
	if ( itr != m_ayRemotePlugins.end() ) {
		m_ayRemotePlugins.erase( itr );
	}
	m_pRemoteStatsPlugin = nullptr;

	return PEEK_PLUGIN_SUCCESS;
}

CRemotePlugin*
CPeekPlugin::GetRemotePlugin() const
{
	PeekLock lock( m_Mutex );

	// Prune the RemotePlugin list of stale pointers.
	bool	bRemoved = true;
	while ( bRemoved && !m_ayRemotePlugins.empty() ) {
		bRemoved = false;
		for ( auto itr = m_ayRemotePlugins.begin(); (itr != m_ayRemotePlugins.end()) && !bRemoved; ++itr ) {
			if ( IsValidPointer( *itr ) ) {
				if ( ((*itr)->m_nState != CRemotePlugin::kState_Uninitialized) &&
					((*itr)->m_nState != CRemotePlugin::kState_Initialized) ) {
					m_ayRemotePlugins.erase( itr );
					bRemoved = true;
				}
			}
			else {
				m_ayRemotePlugins.erase( itr );
				bRemoved = true;
			}
		}
	}

	if ( !m_ayRemotePlugins.empty() ) {
		return *m_ayRemotePlugins.begin();
	}

	return nullptr;
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
