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

CMediaUtils			CPeekProxy::s_MediaUtils;
CProtospecs			CPeekProxy::s_Protospecs;


// =============================================================================
//		CLogService
// =============================================================================

// -----------------------------------------------------------------------------
//		SetInterfacePointers
// -----------------------------------------------------------------------------

void
CLogService::DoLogMessage(
	CGlobalId&			inContextId,
	CGlobalId&			inSourceId,
	UInt32				inSourceKey,
	UInt64				inTimeStamp,
	int					inSeverity,
	const CPeekString&	inShortMessage,
	const CPeekString&	inLongMessage )
{
	if ( IsValid() ) {
		Helium::IHeUnknown*	pUnkSite = m_pProxy->GetSite();
		if ( pUnkSite != nullptr ) {
			CHePtr<Helium::IHeServiceProvider>	spServiceProvider;
			Peek::ThrowIfFailed( pUnkSite->QueryInterface(
				&spServiceProvider.p ) );

			CHePtr<ILogMessage>		spMessageService;
			Peek::ThrowIfFailed( spServiceProvider->QueryService(
				ILogMessage::GetIID(), &spMessageService.p ) );

			Peek::ThrowIfFailed( spMessageService->LogMessage(
				inContextId, inSourceId, inSourceKey, inTimeStamp,
				static_cast<PeekSeverity>( inSeverity ), inShortMessage,
				inLongMessage ) );
		}
	}
}


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

	m_pUnkSite = inUnkSite;

	if ( s_MediaUtils.IsNotValid() ) {
		CHePtr<IMediaUtils>		spMediaUtils;
		Peek::ThrowIfFailed( spMediaUtils.CreateInstance( "PeekCore.MediaUtils" ) );
		s_MediaUtils.SetPtr( spMediaUtils.get() );
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

	m_spUnkSite = inUnkSite;

	CHePtr<Helium::IHeServiceProvider>	spServiceProvider;
	Peek::ThrowIfFailed( inUnkSite->QueryInterface( &spServiceProvider.p ) );
	if ( HE_SUCCEEDED( hr ) ) {
		if ( m_spFilterCollection.get() == nullptr ) {
			hr = spServiceProvider->QueryService( IFilterCollection::GetIID(), &m_spFilterCollection.p );
		}

		m_AdapterManager.SetPtr( spServiceProvider.get() );

		m_LogService.SetPtr( this );

		m_NameTable.SetPtr( spServiceProvider.get() );

		if ( m_spHardwareOptionsCollection ) {
			hr = spServiceProvider->QueryService( IHardwareOptionsCollection::GetIID(), &m_spHardwareOptionsCollection.p );
		}

		//if ( m_spExpertPrefs.get() == nullptr ) {
		//	hr = spServiceProvider->QueryService( IExpertPrefs::GetIID(), &m_spExpertPrefs.p );
		//}

#ifdef IMPLEMENT_NOTIFICATIONS
		m_NotifyService.SetPtr( spServiceProvider.get(), pRemote );
#endif // IMPLEMENT_NOTIFICATIONS

		//if ( m_spEngine.get == nullptr ) {
		//	hr = spServiceProvider->QueryService( IEngine::GetIID(), &m_spEngine.p );
		//}

		if ( m_spSimplePropBag.get() == nullptr ) {
			hr = spServiceProvider->QueryService( Helium::IHePropertyBag::GetIID(), &m_spSimplePropBag.p );
		}

		//if ( m_spActivationInfo.get == nullptr ) {
		//	hr = spServiceProvider->QueryService( IActivationInfo::GetIID(), &m_spActivationInfo.p );
		//}

		//if ( !_spFileLog.get == nullptr ) {
		//	hr = spServiceProvider->QueryService( IFileLog::GetIID(), &m_spFileLog.p );
		//}

		//if ( m_spEngine.get == nullptr ) {
		//	hr = spServiceProvider->QueryService( IEngine::GetIID(), &m_spEngine.p );
		//}

		if ( m_spAlarmCollection.get() == nullptr ) {
			hr = spServiceProvider->QueryService( IAlarmCollection::GetIID(), &m_spAlarmCollection.p );
		}

		if ( m_spEnginePluginCollection.get() == nullptr ) {
			hr = spServiceProvider->QueryService( IEnginePluginCollection::GetIID(), &m_spEnginePluginCollection.p );
		}

		if ( m_spCaptureTemplateCollection.get() == nullptr ) {
			hr = spServiceProvider->QueryService( ICaptureTemplateCollection::GetIID(), &m_spCaptureTemplateCollection.p );
		}

		//if ( m_spEngineManagement.get == nullptr ) {
		//	hr = spServiceProvider->QueryService( IEngineManagement::GetIID(), &m_spEngineManagement.p );
		//}

		//if ( m_spGraphTemplateMgr.get == nullptr ) {
		//	hr = spServiceProvider->QueryService( IGraphTemplateMgr::GetIID(), &m_spGraphTemplateMgr.p );
		//}

		m_LookupNameEntry.SetPtr( spServiceProvider.get() );
	}
}
