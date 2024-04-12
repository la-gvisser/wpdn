// =============================================================================
//	PeekProxy.cpp
// =============================================================================
//	Copyright (c) 2008-2015 Savvius, Inc. All rights reserved.

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
CPeekProxy::SetInterfacePointers(
	IHeUnknown*	inUnkSite )
{
#ifdef _WIN32
	// TODO
	ProtoSpecs::IsProtospecsLoaded();
#endif

	if ( s_MediaUtils.IsNotValid() ) {
		CHePtr<IMediaUtils>		spMediaUtils;
		Peek::ThrowIfFailed( spMediaUtils.CreateInstance( "PeekCore.MediaUtils" ) );
		s_MediaUtils.SetPtr( spMediaUtils );
	}

	if ( inUnkSite && m_AppConfig.IsNotValid() ) {
		CHePtr<Helium::IHeServiceProvider>	spServiceProvider;
		HeResult	hr( inUnkSite->QueryInterface( &spServiceProvider.p ) );
		if ( HE_SUCCEEDED( hr ) ) {
			m_AppConfig.SetPtr( spServiceProvider );
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
	(void)pRemote;

	CPeekProxy::SetInterfacePointers( inUnkSite );

	HeResult	hr = HE_S_OK;

	_ASSERTE( inUnkSite );
	if ( !inUnkSite ) return;

	CHePtr<Helium::IHeServiceProvider>	spServiceProvider;
	Peek::ThrowIfFailed( inUnkSite->QueryInterface( &spServiceProvider.p ) );
	if ( HE_SUCCEEDED( hr ) ) {
		if ( m_spFilterCollection == nullptr ) {
			hr = spServiceProvider->QueryService( IFilterCollection::GetIID(), &m_spFilterCollection.p );
		}

		m_LogService.SetPtr( spServiceProvider );

		m_NameTable.SetPtr( spServiceProvider );

		if ( m_spHardwareOptionsCollection ) {
			hr = spServiceProvider->QueryService( IHardwareOptionsCollection::GetIID(), &m_spHardwareOptionsCollection.p );
		}

		//if ( m_spExpertPrefs == nullptr ) {
		//	hr = spServiceProvider->QueryService( IExpertPrefs::GetIID(), &m_spExpertPrefs.p );
		//}

#ifdef IMPLEMENT_NOTIFICATIONS
		m_NotifyService.SetPtr( spServiceProvider, pRemote );
#endif // IMPLEMENT_NOTIFICATIONS

		//if ( m_spEngine == nullptr ) {
		//	hr = spServiceProvider->QueryService( IEngine::GetIID(), &m_spEngine.p );
		//}

		if ( m_spSimplePropBag == nullptr ) {
			hr = spServiceProvider->QueryService( Helium::IHePropertyBag::GetIID(), &m_spSimplePropBag.p );
		}

		//if ( m_spActivationInfo == nullptr ) {
		//	hr = spServiceProvider->QueryService( IActivationInfo::GetIID(), &m_spActivationInfo.p );
		//}

		//if ( m_spFileLog == nullptr ) {
		//	hr = spServiceProvider->QueryService( IFileLog::GetIID(), &m_spFileLog.p );
		//}

		//if ( m_spEngine == nullptr ) {
		//	hr = spServiceProvider->QueryService( IEngine::GetIID(), &m_spEngine.p );
		//}

		if ( m_spAlarmCollection == nullptr ) {
			hr = spServiceProvider->QueryService( IAlarmCollection::GetIID(), &m_spAlarmCollection.p );
		}

		if ( m_spEnginePluginCollection == nullptr ) {
			hr = spServiceProvider->QueryService( IEnginePluginCollection::GetIID(), &m_spEnginePluginCollection.p );
		}

		if ( m_spAdapterManager == nullptr ) {
			hr = spServiceProvider->QueryService( IAdapterManager::GetIID(), &m_spAdapterManager.p );
		}

		if ( m_spCaptureTemplateCollection == nullptr ) {
			hr = spServiceProvider->QueryService( ICaptureTemplateCollection::GetIID(), &m_spCaptureTemplateCollection.p );
		}

		//if ( m_spEngineManagement == nullptr ) {
		//	hr = spServiceProvider->QueryService( IEngineManagement::GetIID(), &m_spEngineManagement.p );
		//}

		//if ( m_spGraphTemplateMgr == nullptr ) {
		//	hr = spServiceProvider->QueryService( IGraphTemplateMgr::GetIID(), &m_spGraphTemplateMgr.p );
		//}

		m_LookupNameEntry.SetPtr( spServiceProvider );
	}
}
