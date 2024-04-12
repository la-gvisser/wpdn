// =============================================================================
//	PeekProxy.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "resource.h"
#include "Peek.h"
#include "RemotePlugin.h"
#include "PeekProxy.h"
#include "Protospecs.h"
#include "peeknames.h"
#include "peeknotify.h"

bool	CPeekProxy::s_bEngineHost = true;
CMediaUtils			CPeekProxy::s_MediaUtils;
CProtospecs			CPeekProxy::s_Protospecs;


// =============================================================================
//		CPeekProxy
// =============================================================================

// -----------------------------------------------------------------------------
//		SetInterfacePointers
// -----------------------------------------------------------------------------

void
CPeekProxy::SetInterfacePointers()
{
	ProtoSpecs::IsProtospecsLoaded();

	if ( s_MediaUtils.IsNotValid() ) {
		CHePtr<IMediaUtils>		spMediaUtils;
		Peek::ThrowIfFailed( spMediaUtils.CreateInstance( "PeekCore.MediaUtils" ) );
		s_MediaUtils.SetPtr( spMediaUtils );
	}
}


// =============================================================================
//		CPeekConsoleProxy
// =============================================================================

// -----------------------------------------------------------------------------
//		SetInterfacePointers
// -----------------------------------------------------------------------------

void
CPeekConsoleProxy::SetInterfacePointers(
	IHeUnknown*		inUnkSite,
	CRemotePlugin* /*pRemote*/ )
{
	HeResult	hr = HE_S_OK;

	ASSERT( inUnkSite );
	if ( !inUnkSite ) return;

	CHePtr<Helium::IHeServiceProvider>	spServiceProvider;
	Peek::ThrowIfFailed( inUnkSite->QueryInterface( &spServiceProvider.p ) );
	if ( HE_SUCCEEDED( hr ) ) {
		m_LogService.SetPtr( spServiceProvider );

		if ( m_spAdapterManager == NULL ) {
			hr = spServiceProvider->QueryService( IAdapterManager::GetIID(), &m_spAdapterManager.p );
		}

		if ( m_spAppConfig == NULL ) {
			hr = spServiceProvider->QueryService( IAppConfig::GetIID(), &m_spAppConfig.p );
		}
	}
}


// =============================================================================
//		CPeekEngineProxy
// =============================================================================

// -----------------------------------------------------------------------------
//		ReleaseInterfacePointers
// -----------------------------------------------------------------------------

void
CPeekEngineProxy::ReleaseInterfacePointers()
{
#ifdef IMPLEMENT_NOTIFICATIONS
	m_NotifyService.Close();
#endif // IMPLEMENT_NOTIFICATIONS
}


// -----------------------------------------------------------------------------
//		SetInterfacePointers
// -----------------------------------------------------------------------------

void
CPeekEngineProxy::SetInterfacePointers(
	Helium::IHeUnknown* inUnkSite,
	CRemotePlugin*		pRemote )
{
	pRemote;

	HeResult	hr = HE_S_OK;

	ASSERT( inUnkSite );
	if ( !inUnkSite ) return;

	CHePtr<Helium::IHeServiceProvider>	spServiceProvider;
	Peek::ThrowIfFailed( inUnkSite->QueryInterface( &spServiceProvider.p ) );
	if ( HE_SUCCEEDED( hr ) ) {
		if ( m_spFilterCollection == NULL ) {
			hr = spServiceProvider->QueryService( IFilterCollection::GetIID(), &m_spFilterCollection.p );
		}

		m_LogService.SetPtr( spServiceProvider );

		m_NameTable.SetPtr( spServiceProvider );

		if ( m_spHardwareOptionsCollection ) {
			hr = spServiceProvider->QueryService( IHardwareOptionsCollection::GetIID(), &m_spHardwareOptionsCollection.p );
		}

		//if ( m_spExpertPrefs == NULL ) {
		//	hr = spServiceProvider->QueryService( IExpertPrefs::GetIID(), &m_spExpertPrefs.p );
		//}

#ifdef IMPLEMENT_NOTIFICATIONS
		m_NotifyService.SetPtr( spServiceProvider, pRemote );
#endif // IMPLEMENT_NOTIFICATIONS

		//if ( m_spEngine == NULL ) {
		//	hr = spServiceProvider->QueryService( IEngine::GetIID(), &m_spEngine.p );
		//}

		if ( m_spSimplePropBag == NULL ) {
			hr = spServiceProvider->QueryService( Helium::IHePropertyBag::GetIID(), &m_spSimplePropBag.p );
		}

		//if ( m_spActivationInfo == NULL ) {
		//	hr = spServiceProvider->QueryService( IActivationInfo::GetIID(), &m_spActivationInfo.p );
		//}

		//if ( m_spFileLog == NULL ) {
		//	hr = spServiceProvider->QueryService( IFileLog::GetIID(), &m_spFileLog.p );
		//}

		//if ( m_spEngine == NULL ) {
		//	hr = spServiceProvider->QueryService( IEngine::GetIID(), &m_spEngine.p );
		//}

		if ( m_spAlarmCollection == NULL ) {
			hr = spServiceProvider->QueryService( IAlarmCollection::GetIID(), &m_spAlarmCollection.p );
		}

		if ( m_spEnginePluginCollection == NULL ) {
			hr = spServiceProvider->QueryService( IEnginePluginCollection::GetIID(), &m_spEnginePluginCollection.p );
		}

		if ( m_spAdapterManager == NULL ) {
			hr = spServiceProvider->QueryService( IAdapterManager::GetIID(), &m_spAdapterManager.p );
		}

		if ( m_spCaptureTemplateCollection == NULL ) {
			hr = spServiceProvider->QueryService( ICaptureTemplateCollection::GetIID(), &m_spCaptureTemplateCollection.p );
		}

		//if ( m_spEngineManagement == NULL ) {
		//	hr = spServiceProvider->QueryService( IEngineManagement::GetIID(), &m_spEngineManagement.p );
		//}

		if ( m_spAppConfig == NULL ) {
			hr = spServiceProvider->QueryService( IAppConfig::GetIID(), &m_spAppConfig.p );
		}

		//if ( m_spGraphTemplateMgr == NULL ) {
		//	hr = spServiceProvider->QueryService( IGraphTemplateMgr::GetIID(), &m_spGraphTemplateMgr.p );
		//}

		m_LookupNameEntry.SetPtr( spServiceProvider );
	}
}
