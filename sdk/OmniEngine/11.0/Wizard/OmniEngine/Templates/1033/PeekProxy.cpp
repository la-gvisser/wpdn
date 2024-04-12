// =============================================================================
//	PeekProxy.cpp
// =============================================================================
//	Copyright (c) 2008-2017 Savvius, Inc. All rights reserved.

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
		CHeQIPtr<Helium::IHeServiceProvider>	spServiceProvider( m_pUnkSite );
		Peek::ThrowIfNull( spServiceProvider );

		CHePtr<ILogMessage>		spMessageService;
		Peek::ThrowIfFailed( spServiceProvider->QueryService(
			ILogMessage::GetIID(), &spMessageService.p ) );

		Peek::ThrowIfFailed( spMessageService->LogMessage(
			inContextId, inSourceId, inSourceKey, inTimeStamp,
			static_cast<PeekSeverity>( inSeverity ), inShortMessage,
			inLongMessage ) );
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

	m_LogService.SetSite( inUnkSite );

	if ( s_MediaUtils.IsNotValid() ) {
		CHePtr<IMediaUtils>		spMediaUtils;
		Peek::ThrowIfFailed( spMediaUtils.CreateInstance( "PeekCore.MediaUtils" ) );
		s_MediaUtils.SetPtr( spMediaUtils );
	}
}
[!if !NOTIFY_UIHANDLER]


// =============================================================================
//		CPeekConsoleProxy
// =============================================================================

// -----------------------------------------------------------------------------
//		SetInterfacePointers
// -----------------------------------------------------------------------------

void
CPeekConsoleProxy::SetInterfacePointers(
	IHeUnknown*	inUnkSite )
{
	HeResult	hr = HE_S_OK;

	_ASSERTE( inUnkSite );
	if ( !inUnkSite ) return;

	CPeekProxy::SetInterfacePointers( inUnkSite );
	
	CHeQIPtr<Helium::IHeServiceProvider>	spServiceProvider( inUnkSite );
	Peek::ThrowIfNull( spServiceProvider );
	hr = spServiceProvider->QueryService( IAdapterManager::GetIID(), &m_spAdapterManager.p );
}
[!endif]


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

	CHeQIPtr<Helium::IHeServiceProvider>	spServiceProvider( inUnkSite );
	Peek::ThrowIfNull( spServiceProvider );
	if ( m_spFilterCollection == nullptr ) {
		hr = spServiceProvider->QueryService( IFilterCollection::GetIID(), &m_spFilterCollection.p );
	}

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
