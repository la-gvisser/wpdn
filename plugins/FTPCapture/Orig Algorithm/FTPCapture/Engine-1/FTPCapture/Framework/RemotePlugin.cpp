// =============================================================================
//	RemotePlugin.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "resource.h"
#include "RemotePlugin.h"
#include "ContextManager.h"
#include "PeekProxy.h"
#include "Protospecs.h"
#include "omnigraphs.h"
#include "peekfilters.h"
#include "peeknames.h"
#include "Snapshot.h"
#include "PeekXml.h"
#include "PeekDataModeler.h"
#include "omnicommands.h"
#include "imemorystream.h"
#include "Options.h"
#include "OmniConsoleContext.h"
#include "OmniEngineContext.h"
#include <memory>

#ifdef _DEBUG
static bool		s_bDebugEngine = true;
#endif

#ifdef PEEK_UI
CPluginApp		gPluginApp;
HINSTANCE		GetInstanceHandle() { return gPluginApp.m_hInstance; }
#endif

CPeekProxy		CRemotePlugin::s_PeekProxy;
COmniPlugin		CRemotePlugin::s_Plugin;
CConsoleSend	CRemotePlugin::s_ConsoleSend;
CConsoleUI		CRemotePlugin::s_ConsoleUI;

extern CContextManagerPtr	GetContextManager();


// ============================================================================
//		SE_Exception
// ============================================================================

class SE_Exception
{
private:
	unsigned int	m_nSE;

public:
	;				SE_Exception() {}
	;				SE_Exception( unsigned int n ) : m_nSE( n ) {}
	;				~SE_Exception() {}
	unsigned int	GetSeNumber() { return m_nSE; }
};


// ============================================================================
//		SE_TranslationFunc
// ============================================================================

void
SE_TranslationFunc(
	unsigned int		/*u*/,
	EXCEPTION_POINTERS*	/*pExp*/ )
{
	throw SE_Exception();
}


#ifdef PEEK_UI
// ============================================================================
//		CPluginApp
// ============================================================================

CPluginApp::CPluginApp()
{
	int	se_set = 0;
	try {
		_set_se_translator( SE_TranslationFunc );
		se_set = 1;
	}
	catch ( ... ) {
		// Note the error...
		se_set = -1;
	}
	ASSERT( se_set == 1 );
}


// -----------------------------------------------------------------------------
//		InitInstance
// -----------------------------------------------------------------------------

BOOL
CPluginApp::InitInstance()
{
	return CWinApp::InitInstance();
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CPluginApp, CWinApp)
END_MESSAGE_MAP()

#endif


// =============================================================================
//		CRemotePlugin
// =============================================================================

CRemotePlugin::CRemotePlugin()
	:	m_hInstance( NULL )
	,	m_bGraphsContext( false )
	,	m_pfnReset( NULL )
  #ifdef IMPLEMENT_ALL_UPDATESUMMARY
	,	m_pfnSummary( NULL )
  #endif // IMPLEMENT_ALL_UPDATESUMMARY
{
  #ifdef PEEK_UI
	m_hInstance = gPluginApp.m_hInstance;
  #endif

  #ifdef IMPLEMENT_NOTIFICATIONS
	m_idAction.SetNull();
  #endif // IMPLEMENT_NOTIFICATIONS
}

CRemotePlugin::~CRemotePlugin()
{
	ReleaseContext();
}


// -----------------------------------------------------------------------------
//		FinalConstruct
// -----------------------------------------------------------------------------

HeResult
CRemotePlugin::FinalConstruct()
{
	try {
		CPeekString	strCommandLine( ::GetCommandLine() );
		if ( !strCommandLine.IsEmpty() ) {
			strCommandLine.MakeLower();
			int	nIndex = strCommandLine.Find( L"omnipeek.exe" );
			if ( nIndex >= 0 ) {
				s_PeekProxy.SetConsoleHost();
			}
		}
	}
	catch ( ... ) {
	}

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		FinalRelease
// -----------------------------------------------------------------------------

void
CRemotePlugin::FinalRelease()
{
}


// .............................................................................
//		Interfaces Implemented Both In Engine and Console
// .............................................................................

// -----------------------------------------------------------------------------
//		IPeekPlugin::GetName
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::GetName(
	Helium::HEBSTR*	outName )
{
	HE_ASSERT( outName != NULL );
	if ( outName == NULL ) return HE_E_NULL_POINTER;

//	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;

	try {
		HeLib::HeHEBSTRFromString( s_Plugin.GetName(), outName );
		if ( *outName == NULL ) return HE_E_OUT_OF_MEMORY;
		hr = HE_S_OK;
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		IPeekPlugin::GetClassID
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::GetClassID(
	Helium::HeCID*	outClassId )
{
	//	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;

	try {
		HE_ASSERT( outClassId != NULL );
		if ( outClassId == NULL ) return HE_E_NULL_POINTER;

		*outClassId = COmniPlugin::GetClassId();

		hr = HE_S_OK;
	}
	HE_CATCH( hr );

	return hr;
}


// ----------------------------------------------------------------------------
//		IObjectWithSiteImpl::SetSite
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::SetSite(
	IHeUnknown *inUnkSite )
{
	ObjectLock	lock( this );

	if ( HasSite() ) return HE_S_OK;

	HeResult hr = HE_S_OK;
	try {
		IHeObjectWithSiteImpl<CRemotePlugin>::SetSite( inUnkSite );
		LoadContext();
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		IPeekPluginLoad::Load
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::Load(
	const HeID&	inContextId )
{
	ObjectLock	lock( this );

	HeResult	hr = HE_S_OK;
	try {
		SetContextId( inContextId );
		LoadContext();
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		IPeekPluginLoad::Unload
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::Unload()
{
	ObjectLock	lock( this );

	HeResult	hr = HE_S_OK;
	try {
		ReleaseContext();

		CContextManagerPtr	spContextManager = GetContextManager();
		HE_ASSERT( spContextManager );
		if ( spContextManager && spContextManager->IsEmpty() ) {
			DoUnloadPlugin();
		}
		// Not seeing Graphs plugins being deleted...
		if ( spContextManager ) {
			CPeekContextPtr		spContext;
			if ( spContextManager->Find( m_idContext, spContext ) ) {
				CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
				if ( pEngineContext ) {
					CRemotePlugin*	pGraphsPlugin = pEngineContext->GetGraphsPlugin();
					if ( pGraphsPlugin && (pGraphsPlugin != this)  ) {
						pGraphsPlugin->Unload();
					}
				}
			}
		}

		InvalidateContextId();
	}
	HE_CATCH( hr );

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IHePersistXml::Load
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::Load(
	IHeUnknown*	inDocument,
	IHeUnknown*	inNode )
{
	if ( !s_Plugin.IsInitialized() ) return HE_E_FAIL;

	ObjectLock	lock( this );

	HeResult	hr = HE_S_OK;
	try {
		CreateContext();
		Peek::ThrowIf( !CPeekXml::Validate( inDocument, inNode ) );
		m_spdmPrefs = CPeekDataModelerPtr( new CPeekDataModeler( inDocument, inNode, kModeler_Load ) );
		if ( m_spdmPrefs.get() != NULL ) {
			ASSERT( m_spdmPrefs->IsOpen() );
			if ( HasContext() && m_spContext->CheckState( CPeekContext::kContextState_Created ) ) {
				m_spContext->OnContextPrefs( *m_spdmPrefs.get() );
				m_spdmPrefs->End();
			}
			else {
				m_Options.Model( *m_spdmPrefs );
			}
		}
		hr = HE_S_OK;
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		IHePersistXml::Save
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::Save(
	IHeUnknown*	ioDocument,
	IHeUnknown*	ioNode,
	int			/*inClearDirty*/ )
{
	if ( !s_Plugin.IsInitialized() ) return HE_E_FAIL;

	ObjectLock	lock( this );

	HeResult	hr = HE_S_OK;
	try {
		CreateContext();
		
		Peek::ThrowIf( !CPeekXml::Validate( ioDocument, ioNode ) );
		CPeekDataModeler pdmPrefs( ioDocument, ioNode, kModeler_Store );

		if ( HasContext() )  {
			m_spContext->OnContextPrefs( pdmPrefs );
			pdmPrefs.End();
		}
		else {
			m_Options.Model( pdmPrefs );
			pdmPrefs.End();
		}
		hr = HE_S_OK;
	}
	HE_CATCH( hr );

	return hr;
}


// .............................................................................
//		Engine-Only Interfaces
// .............................................................................

#ifdef IMPLEMENT_NOTIFICATIONS
////////////////////////////////////////////////////////////////////////////////
// IAction

// ----------------------------------------------------------------------------
//		IAction::GetID
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::GetID(
	HeID*	pID )
{
	ObjectLock	lock( this );

	HE_ASSERT( pID != NULL );
	if ( pID == NULL ) return E_POINTER;

	HeResult	hr = HE_S_OK;

	try {
		*pID = m_idAction;
	}
	catch ( ... ) {
		hr = E_UNEXPECTED;
	}

	return hr;
}


// ----------------------------------------------------------------------------
//		IAction::SetID
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::SetID(
	const HeID&	id )
{
	ObjectLock	lock( this );

	HeResult	hr = HE_S_OK;

	try {
		m_idAction = id;
	}
	catch ( ... ) {
		hr = E_UNEXPECTED;
	}

	return hr;
}


/*
// ----------------------------------------------------------------------------
//		IAction::GetName
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::GetName(
	Helium::HEBSTR* pbstrName )
{
	ObjectLock	lock( this );

	HE_ASSERT( pbstrName != NULL );
	if ( pbstrName == NULL ) return E_POINTER;

	HeResult	hr = HE_S_OK;

	try {
		*pbstrName = static_cast<Helium::HEBSTR>( m_strName.AllocSysString() );
	}
	catch ( ... ) {
		hr = E_UNEXPECTED;
	}

	return hr;
}
*/


// ----------------------------------------------------------------------------
//		IAction::SetName
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::SetName(
	Helium::HEBSTR	/*bstrName*/ )
{
	ObjectLock	lock( this );

	HeResult hr = HE_S_OK;

//	try {
//		m_strName = bstrName;
//	}
//	catch ( ... ) {
//		hr = E_UNEXPECTED;
//	}

	return hr;
}


// ----------------------------------------------------------------------------
//		IAction::EnableSeverity
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::EnableSeverity(
	PeekSeverity	/*severity*/,
	BOOL			/*bEnable*/ )
{
	if ( !s_Plugin.IsInitialized() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_S_OK;

	//	try {
	//		m_EnabledSeverities.SetAt( severity, bEnable );
	//	}
	//	catch ( ... ) {
	//		hr = E_UNEXPECTED;
	//	}

	return hr;
}


// ----------------------------------------------------------------------------
//		IAction::IsSeverityEnabled
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::IsSeverityEnabled(
	PeekSeverity	severity,
	BOOL*			pbEnabled )
{
	if ( !s_Plugin.IsInitialized() ) return HE_S_OK;

	ObjectLock	lock( this );

	HE_ASSERT( pbEnabled != NULL );
	if ( pbEnabled == NULL ) return E_POINTER;

	HeResult	hr = HE_S_OK;

	try {

		*pbEnabled = FALSE;

		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		if ( pEngineContext ) {
			CPeekEngineProxy* pProxy = pEngineContext->GetProxy();
			if ( pProxy ) {
				CNotifyService& NotifySvc = pProxy->GetNotifyService();
				*pbEnabled = NotifySvc.IsSeverityEnabled( severity );
			}
		}
	}
	catch ( ... ) {
		hr = E_UNEXPECTED;
	}

	return hr;
}


// ----------------------------------------------------------------------------
//		IAction::GetDisabledSources
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::GetDisabledSources(
	HeID**	ppSources,
	UInt32*	pnSources )
{
	if ( !s_Plugin.IsInitialized() ) return HE_S_OK;

	ObjectLock	lock( this );

	HE_ASSERT( ppSources != NULL );
	if ( ppSources == NULL ) return E_POINTER;
	HE_ASSERT( pnSources != NULL );
	if ( pnSources == NULL ) return E_POINTER;

	HeResult	hr = HE_S_OK;

	try {
		*ppSources = NULL;
		*pnSources = 0;
/*
		if ( m_ayDisabledSources.GetCount() > 0 ) {
			const size_t	cbSources = m_ayDisabledSources.GetCount() * sizeof(GUID);
			*ppSources = static_cast<HeID*>( HeTaskMemAlloc( cbSources ) );

			if ( *ppSources == NULL ) AtlThrow( E_OUTOFMEMORY );
			memcpy( *ppSources, m_ayDisabledSources.GetData(), cbSources );
			*pnSources = (ULONG) m_ayDisabledSources.GetCount();
		}
*/
	}
	catch ( ... ) {
		hr = E_UNEXPECTED;
	}

	return hr;
}


// ----------------------------------------------------------------------------
//		IAction::SetDisabledSources
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::SetDisabledSources(
	const HeID*	/*pSources*/,
	UInt32		/*nSources*/ )
{
	if ( !s_Plugin.IsInitialized() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_S_OK;

	try {
/*
		m_ayDisabledSources.RemoveAll();

		if ( (pSources != NULL) && (nSources > 0) ) {
			for ( ULONG i = 0; i < nSources; i++ ) {
				m_ayDisabledSources.Add( pSources[i] );
			}
		}
*/
	}
	catch ( ... ) {
		hr = E_UNEXPECTED;
	}

	return hr;
}


// ----------------------------------------------------------------------------
//		IAction::IsSourceEnabled
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::IsSourceEnabled(
	const HeID&	guidSource,
	BOOL*		pbEnabled )
{
	if ( !s_Plugin.IsInitialized() ) return HE_S_OK;

	ObjectLock	lock( this );

	HE_ASSERT( pbEnabled != NULL );
	if ( pbEnabled == NULL ) return E_POINTER;

	HeResult hr = HE_S_OK;

	try {
		*pbEnabled = FALSE;

		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		if ( !pEngineContext ) return hr;

		CPeekEngineProxy* pProxy = pEngineContext->GetProxy();
		if ( !pProxy ) return hr;

		CNotifyService& NotifySvc = pProxy->GetNotifyService();
		UInt8 nSeveritySrcExclusions = NotifySvc.GetSeveritySourceExclusions();

		*pbEnabled = FALSE;

		switch ( nSeveritySrcExclusions ) {
			case NOTIFY_SOURCES_EXCLUDE_NONE:
				*pbEnabled = TRUE;
				break;

			case NOTIFY_SOURCES_EXCLUDE_CURRENT:
				*pbEnabled = TRUE;
				if ( guidSource.Equals( m_idContext ) ) {
					*pbEnabled = FALSE;
				}
				break;

			case NOTIFY_SOURCES_EXCLUDE_OTHER_INSTANCES:
				{
					*pbEnabled = TRUE;
					if ( guidSource.Equals( m_idContext ) ) {
						ASSERT( *pbEnabled == TRUE );
					}
					else {
						CContextManagerPtr	spContextManager = GetContextManager();
						HE_ASSERT( spContextManager );
						if ( spContextManager && spContextManager->IsIdInManager( CGlobalId( guidSource ) ) ) {
							*pbEnabled = FALSE;
						}
					}
				}
				break;

			case NOTIFY_SOURCES_EXCLUDE_ALL_INSTANCES:
				{
					*pbEnabled = TRUE;
					CContextManagerPtr	spContextManager = GetContextManager();
					HE_ASSERT( spContextManager );
					if ( spContextManager && spContextManager->IsIdInManager( CGlobalId( guidSource ) ) ) {
						*pbEnabled = FALSE;
					}
				}
				break;

			case NOTIFY_SOURCES_INCLUDE_CURRENT:
				*pbEnabled = FALSE;
				if ( guidSource.Equals( m_idContext ) ) {
					*pbEnabled = TRUE;
				}
				break;

			case NOTIFY_SOURCES_INCLUDE_OTHER_INSTANCES:
				{
					*pbEnabled = FALSE;
					if ( guidSource.Equals( m_idContext ) ) {
						ASSERT( *pbEnabled == FALSE );
					}
					else {
						CContextManagerPtr	spContextManager = GetContextManager();
						HE_ASSERT( spContextManager );
						if ( spContextManager && spContextManager->IsIdInManager( CGlobalId( guidSource ) ) ) {
							*pbEnabled = TRUE;
						}
					}
				}
				break;

			case NOTIFY_SOURCES_INCLUDE_ALL_INSTANCES:
				{
					*pbEnabled = FALSE;
					CContextManagerPtr	spContextManager = GetContextManager();
					HE_ASSERT( spContextManager );
					if ( spContextManager && spContextManager->IsIdInManager( CGlobalId( guidSource ) ) ) {
						*pbEnabled = TRUE;
					}
				}
				break;

			default:
				*pbEnabled = FALSE;
				break;
		}
	}
	catch ( ... ) {
		hr = E_UNEXPECTED;
	}

	return hr;
}


// ----------------------------------------------------------------------------
//		INotify::Notify
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::Notify( 
	const HeID&		guidContext,
	const HeID&		guidSource,
	UInt32			/*ulSourceKey*/,
	UInt64			ullTimeStamp,
	PeekSeverity	severity,
	const wchar_t*	pszShortMessage,
	const wchar_t*	pszLongMessage )
{
	if ( !s_Plugin.IsInitialized() ) return HE_S_OK;

	ObjectLock	lock( this );

	if ( guidContext.IsNull() ) {
		return HE_S_OK;
	}

	HeResult	hr = HE_S_OK;

	try {
		CPeekEngineContext*	pEngineContext =
			dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		if ( pEngineContext ) {
			CPeekString strShortMessage( pszShortMessage );
			CPeekString strLongMessage( pszLongMessage );

			CGlobalId gContext( guidContext );
			CGlobalId gSource( guidSource );
			int	nResult = pEngineContext->OnNotify(
				gContext, gSource, ullTimeStamp, severity, 
				strShortMessage, strLongMessage );

			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
	}
	catch ( ... ) {
		hr = E_UNEXPECTED;
	}

	return hr;
}
#endif // IMPLEMENT_NOTIFICATIONS


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		IProcessPluginMessage::ProcessPluginMessage
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::ProcessPluginMessage(
	const Helium::HeCID&	inId,
	Helium::IHeStream*		inMessage,
	Helium::IHeStream*		outResponse )
{
	if ( !s_Plugin.IsInitialized() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekStream psMessage( inMessage );
		CPeekStream psResponse( outResponse );

		psMessage.Rewind();

		PluginMessageHdr	hdrReqPluginMessage;
		size_t				nResultBytes = psMessage.Read( sizeof( PluginMessageHdr ), &hdrReqPluginMessage );
		ASSERT( nResultBytes == sizeof( PluginMessageHdr ) );
		if ( nResultBytes != sizeof( PluginMessageHdr ) ) Peek::Throw( E_FAIL );

		ASSERT( inId == COmniPlugin::GetClassId() );
		ASSERT( hdrReqPluginMessage.PluginID == COmniPlugin::GetClassId() );
		if ( (inId != COmniPlugin::GetClassId()) ||
			(hdrReqPluginMessage.PluginID != COmniPlugin::GetClassId()) ) {
				Peek::Throw( E_FAIL );
		}

		UInt32				cbMsgLen = hdrReqPluginMessage.MessageSize;
		CPeekEngineContext*	pEngineContext = NULL;

		pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );

		int			nResult = PEEK_PLUGIN_SUCCESS;
		CPeekStream	psPayload;
		if ( !psMessage.CopyTo( psPayload, cbMsgLen ) ) Peek::Throw( E_FAIL );

		if ( pEngineContext ) {
			ASSERT( hdrReqPluginMessage.CaptureID == m_idContext );
			if ( hdrReqPluginMessage.CaptureID != m_idContext ) Peek::Throw( E_FAIL );
			nResult = pEngineContext->OnProcessPluginMessage( psPayload, psResponse );
		}
		else {
			ASSERT( hdrReqPluginMessage.CaptureID == HeID::Null() );
			if ( hdrReqPluginMessage.CaptureID != HeID::Null() ) Peek::Throw( E_FAIL );
			nResult = s_Plugin.OnProcessPluginMessage( psPayload, psResponse );
		}

		if ( nResult == PEEK_PLUGIN_SUCCESS ) {
			outResponse = psResponse.GetIStreamPtr();
			hr = HE_S_OK;
		}
	}
	HE_CATCH( hr );

	return hr;
}
#endif // IMPLEMENT_PLUGINMESSAGE


#ifdef IMPLEMENT_RESETPROCESSING
// -----------------------------------------------------------------------------
//		IResetProcessing::ResetProcessing
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::ResetProcessing()
{
	if ( !s_Plugin.IsInitialized() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		HE_ASSERT( pEngineContext && m_pfnReset );
		if ( pEngineContext && m_pfnReset ) {
			int	nResult = ((*pEngineContext).*m_pfnReset)();
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}
#endif // IMPLEMENT_RESETPROCESSING


#ifdef IMPLEMENT_PACKETPROCESSOREVENTS
//---------------------------------------------------------------------
//		IPacketProcessorEvents::OnStartPacketProcessing
//---------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::OnStartPacketProcessing(
	UInt64 ullStartTime )
{
	if ( !s_Plugin.IsInitialized() ) return HE_S_OK;

	ObjectLock	lock( this );

	HE_ASSERT( ullStartTime != 0 );

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		HE_ASSERT( pEngineContext );
		if ( pEngineContext ) {
			int	nResult = pEngineContext->OnStartCapture( ullStartTime );
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}


//---------------------------------------------------------------------
//		IPacketProcessorEvents::OnStopPacketProcessing
//---------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::OnStopPacketProcessing(
	UInt64 ullStopTime )
{
	if ( !s_Plugin.IsInitialized() ) return HE_S_OK;

	ObjectLock	lock( this );

	HE_ASSERT( ullStopTime != 0 );

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		HE_ASSERT( pEngineContext );
		if ( pEngineContext ) {
			int	nResult = pEngineContext->OnStopCapture( ullStopTime );
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}


//---------------------------------------------------------------------
//		IPacketProcessorEvents::OnResetPacketProcessing
//---------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::OnResetPacketProcessing()
{
	if ( !s_Plugin.IsInitialized() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_S_OK;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		HE_ASSERT( pEngineContext );
		if ( pEngineContext ) {
			int	nResult = pEngineContext->OnResetCapture();
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}
#endif // IMPLEMENT_PACKETPROCESSOREVENTS


#ifdef IMPLEMENT_FILTERPACKET
// -----------------------------------------------------------------------------
//		IFilterPacket::FilterPacket
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::FilterPacket(
	IPacket*		pPacket,
	UInt16*			pcbBytesToAccept,
	Helium::HeID*	pFilterID )
{
	if ( !s_Plugin.IsInitialized() ) return HE_S_OK;
	if ( IsGraphs() ) return HE_S_OK;		// Only filter on the Primary instance.

	HE_ASSERT( pPacket != NULL );
	if ( pPacket == NULL ) return HE_E_NULL_POINTER;
	HE_ASSERT( pcbBytesToAccept != NULL );
	if ( pcbBytesToAccept == NULL ) return HE_E_NULL_POINTER;
	HE_ASSERT( pFilterID != NULL );
	if ( pFilterID == NULL ) return HE_E_NULL_POINTER;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		HE_ASSERT( pEngineContext );
		if ( pEngineContext ) {
			CPacket		Packet( pPacket );
			CGlobalId	idFilter( GUID_NULL );		// Set to *pFilterID, if it gets initialized.
			int			nResult = pEngineContext->OnFilter( Packet, pcbBytesToAccept, idFilter );
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}
#endif // IMPLEMENT_FILTERPACKET


#ifdef IMPLEMENT_PROCESSPACKET
// -----------------------------------------------------------------------------
//		IProcessPacket::ProcessPacket
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::ProcessPacket(
	IPacket*	pPacket )
{
	if ( !s_Plugin.IsInitialized() ) return HE_S_OK;
	if ( IsGraphs() ) return HE_S_OK;

	HE_ASSERT( pPacket != NULL );
	if ( pPacket == NULL ) return HE_E_NULL_POINTER;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		HE_ASSERT( pEngineContext );
		if ( pEngineContext ) {
			CPacket		Packet( pPacket );
			int			nResult = pEngineContext->OnProcessPacket( Packet );
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}
#endif // IMPLEMENT_PROCESSPACKET


#ifdef IMPLEMENT_PROCESSTIME
// ----------------------------------------------------------------------------
//		IProcessTime::ProcessTime
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::ProcessTime(
	UInt64 ullCurrentTime )
{
	if ( !s_Plugin.IsInitialized() ) return HE_S_OK;

	HE_ASSERT( ullCurrentTime != 0 );

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		HE_ASSERT( pEngineContext );
		if ( pEngineContext ) {
			int	nResult = pEngineContext->OnProcessTime( ullCurrentTime );
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}
#endif // IMPLEMENT_PROCESSTIME


#ifdef IMPLEMENT_ALL_UPDATESUMMARY
// -----------------------------------------------------------------------------
//		IUpdateSummaryStats::UpdateSummaryStats
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::UpdateSummaryStats(
	IHeUnknown*	pSnapshot )
{
	if ( !s_Plugin.IsInitialized() ) return HE_E_FAIL;

	HE_ASSERT( pSnapshot != NULL );
	if ( pSnapshot == NULL ) return HE_E_NULL_POINTER;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		if ( pEngineContext && m_pfnSummary ) {
			int			nResult = PEEK_PLUGIN_FAILURE;
			CSnapshot	Snapshot( pSnapshot );
		  #ifdef IMPLEMENT_UPDATESUMMARYSTATS
			nResult = ((*pEngineContext).*m_pfnSummary)( Snapshot );
		  #else // IMPLEMENT_UPDATESUMMARYSTATS
			// Is defined IMPLEMENT_SUMMARYMODIFYENTRY:
			nResult = ((*pEngineContext).*m_pfnSummary)();
		  #endif //  IMPLEMENT_UPDATESUMMARYSTATS
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;

			#ifdef IMPLEMENT_SUMMARYMODIFYENTRY
				CPeekContextProxy* pProxy = pEngineContext->GetPeekContextProxy();
				HE_ASSERT( pProxy );
				if ( pProxy ) {
					hr = pProxy->UpdateSummaryStats( pSnapshot );
				}
			#endif

			}
		}
	}
	HE_CATCH( hr );

	return hr;
}
#endif // IMPLEMENT_ALL_UPDATESUMMARY


// .............................................................................
//		Console-Only Interfaces
// .............................................................................

#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		IPluginRecv::Recv
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::Recv( 
	HeResult			inMsgResult,
	DWORD				inTransId,
	Helium::IHeStream*	inMessage )
{
	if ( !s_Plugin.IsInitialized() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult hr = HE_S_OK;

	try {
		CPeekStream	psMessage( inMessage );
		psMessage.Rewind();

		PluginMessageHdr	hdrReqPluginMessage;

		size_t nResultBytes = psMessage.Read( sizeof( PluginMessageHdr ), &hdrReqPluginMessage );
		ASSERT( nResultBytes == sizeof( PluginMessageHdr ) );
		if ( nResultBytes != sizeof( PluginMessageHdr ) ) Peek::Throw( E_FAIL );

		ASSERT( hdrReqPluginMessage.PluginID == COmniPlugin::GetClassId() );
		ASSERT( (hdrReqPluginMessage.CaptureID == m_idContext) || (hdrReqPluginMessage.CaptureID.IsNull()) );
		if ( (hdrReqPluginMessage.PluginID != COmniPlugin::GetClassId()) ||
			( (hdrReqPluginMessage.CaptureID != m_idContext) && (!hdrReqPluginMessage.CaptureID.IsNull()) ) ) {
				Peek::Throw( E_FAIL );
		}

		UInt32	cbMsgLen = hdrReqPluginMessage.MessageSize;
		CPeekStream		psPayload;
		bool			bResult = psMessage.CopyTo( psPayload, cbMsgLen );
		if ( !bResult ) Peek::Throw( E_FAIL );

		int						nResult = PEEK_PLUGIN_FAILURE;
		CPeekConsoleContext*	pConsoleContext = NULL;

		if ( !m_idContext.IsNull() ) {
			pConsoleContext = dynamic_cast<CPeekConsoleContext*>( m_spContext.get() );
		}

		if ( pConsoleContext ) {
			nResult = pConsoleContext->OnProcessPluginResponse( psPayload, inMsgResult, inTransId );
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
		else {
			nResult = s_Plugin.OnProcessPluginResponse( psPayload, inMsgResult, inTransId );
		}

		if HE_FAILED( hr ) Peek::Throw( hr );
	}
	HE_CATCH(hr);

	return hr;
}
#endif // IMPLEMENT_PLUGINMESSAGE


// -----------------------------------------------------------------------------
//		IPluginUI::SetEngine
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::SetEngine(
	IHeUnknown*	inEngine )
{
	s_PeekProxy.ValidateConsole();

	ObjectLock	lock( this );

	s_PeekProxy.SetInterfacePointers();
	s_ConsoleSend.SetPtr( inEngine );

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IPluginUI::SetConsoleUI
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::SetConsoleUI(
	IConsoleUI* inConsoleUI )
{
	s_PeekProxy.ValidateConsole();
	if ( IsGraphs() ) return HE_S_OK;

	s_PeekProxy.SetInterfacePointers();
	s_ConsoleUI.SetPtr( inConsoleUI );

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IPluginUI::GetUIInfo
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::GetUIInfo(
	Helium::HEBSTR*	outCommand,
	Helium::HEBSTR*	outText )
{
	s_PeekProxy.ValidateConsole();
	if ( IsGraphs() ) return HE_E_FAIL;

	ASSERT( outCommand != NULL );
	ASSERT( outText != NULL );
	if ( (outCommand == NULL) || (outText == NULL ) ) return HE_E_FAIL;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekString	strCmd( COmniPlugin::GetClassId().Format() );
		CHeBSTR	bstrCommand( strCmd );
		CHeBSTR		bstrText( s_Plugin.GetName() );
		bstrCommand.CopyTo( outCommand );
		bstrText.CopyTo( outText );

		hr = HE_S_OK;	// Return OK if outCommand and outText are set.
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		IPluginUI::LoadView
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::LoadView()
{
	s_PeekProxy.ValidateConsole();
	if ( IsGraphs() ) return HE_S_OK;

	HE_ASSERT( s_ConsoleSend.IsValid() && s_ConsoleUI.IsValid() );
	if ( s_ConsoleSend.IsNotValid() || s_ConsoleUI.IsNotValid() ) return HE_E_FAIL;

	HeResult	hr = HE_E_FAIL;
	try {
		HE_ASSERT( !HasSite() );

		if( !m_idContext.IsValid() ) {
			SetContextId( CGlobalId( GUID_NULL ) );
		}

		LoadContext();

		hr = HE_S_OK;
	}
	HE_CATCH( hr );

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IPluginUI::UnloadView
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::UnloadView()
{
	s_PeekProxy.ValidateConsole();
	if ( IsGraphs() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		if ( s_Plugin.IsInitialized() ) {
			Unload();		// Check to see if Peek calls.
		}
		hr = HE_S_OK;
	}
	HE_CATCH( hr );

	return HE_S_OK;
}


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		SendPluginMessage
//		Send a message from console side plugin to engine side plugin.
// -----------------------------------------------------------------------------

HeResult
CRemotePlugin::SendPluginMessage( 
	CGlobalId		 inContextId,								
	CPeekStream&	 inMessage,
	DWORD			 inTimeout,
	DWORD&			 outTransId )
{
	if ( !s_Plugin.IsInitialized() ) return HE_E_FAIL;

	HE_ASSERT( s_ConsoleSend.IsValid() );
	if ( s_ConsoleSend.IsNotValid() ) return HE_E_FAIL;

	HeResult hr = HE_E_FAIL;
	try {
		const UInt8*	pMessage = NULL;

		size_t cbLength = inMessage.GetLength();
		if ( (cbLength > 0) && (cbLength < kMaxUint32)) {
			pMessage = inMessage.GetData();
			ASSERT( pMessage );
			if ( !pMessage ) return HE_S_OK;
			outTransId = s_ConsoleSend.SendPluginMessage(
				COmniPlugin::GetClassId(), inContextId, this,
				static_cast<UInt32>( cbLength ), const_cast<UInt8*>( pMessage ),
				inTimeout );
			hr = HE_S_OK;
		}
	}
	HE_CATCH( hr );

	return hr;
}
#endif // IMPLEMENT_PLUGINMESSAGE


#ifdef IMPLEMENT_SUMMARIZEPACKET
// -----------------------------------------------------------------------------
//		ISummarizePacket::SummarizePacket
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::SummarizePacket(
	IPacket*			pPacket,
	Helium::HEBSTR*		pbstrSummary,
	UInt32*				pColor,
	PeekSummaryLayer*	pLayer )
{
	s_PeekProxy.ValidateConsole();
	if ( !s_Plugin.IsInitialized() ) return HE_E_FAIL;

	HE_ASSERT( pPacket != NULL );
	if ( pPacket == NULL ) return HE_E_NULL_POINTER;
	HE_ASSERT( pbstrSummary != NULL );
	if ( pbstrSummary == NULL ) return HE_E_NULL_POINTER;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekConsoleContext*	pConsoleContext = dynamic_cast<CPeekConsoleContext*>( m_spContext.get() );
		if ( pConsoleContext ) {
			CPacket		Packet( pPacket );
			CPeekString	strSummary;
			int			nResult = pConsoleContext->OnGetPacketString(
				Packet, strSummary, *pColor, reinterpret_cast<Peek::SummaryLayer*>( pLayer ) );
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				std::wstring strBasic = strSummary;
				hr = HeLib::HeHEBSTRFromString( strBasic, pbstrSummary );
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}
#endif // IMPLEMENT_SUMMARIZEPACKET


// -----------------------------------------------------------------------------
//		IPeekPluginAbout::ShowAbout
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::ShowAbout()
{
	if ( !s_Plugin.IsInitialized() ) return HE_E_FAIL;
	if ( IsGraphs() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_NOT_IMPLEMENTED;

#ifdef HE_WIN32
	try {
		s_Plugin.OnAbout();
		hr = HE_S_OK;
	}
	HE_CATCH( hr );
#endif // HE_WIN32

	return hr;
}


// -----------------------------------------------------------------------------
//		IPeekPluginOptions::ShowOptions
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::ShowOptions()
{
	s_PeekProxy.ValidateConsole();

	if ( !s_Plugin.IsInitialized() ) return HE_E_FAIL;
	if ( IsGraphs() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_NOT_IMPLEMENTED;

#ifdef HE_WIN32
	try {
		COptions	theOptions = m_Options;
		if ( s_Plugin.OnOptions( theOptions ) ) {
			m_Options = theOptions;
		}
		hr = HE_S_OK;
	}
	HE_CATCH( hr );
#endif // HE_WIN32

	return hr;
}


// ============================================================================
// Internal functions
// ============================================================================

// ----------------------------------------------------------------------------
//		DoCreateContext
// ----------------------------------------------------------------------------

void
CRemotePlugin::DoCreateContext()
{
	if ( !IsValidContext() ) return;

	if ( m_spContext->EqualState( CPeekContext::kContextState_Initialized ) ) {
		int nReturn = m_spContext->OnCreateContext();
		if ( nReturn != PEEK_PLUGIN_SUCCESS ) {
			ReleaseContext();
			Peek::Throw( HE_E_FAIL );
		}
	}
	if ( m_spContext->CheckState( CPeekContext::kContextState_Created ) ) {
		ASSERT( (m_spdmPrefs.get() != NULL) ? m_spdmPrefs->IsOpen() : true );
		if ( m_spdmPrefs.get() && m_spdmPrefs->IsOpen() ) {
			m_spContext->OnContextPrefs( *m_spdmPrefs.get() );
			m_spdmPrefs->End();
		}
	}

  #if defined(IMPLEMENT_RESETPROCESSING) || defined(IMPLEMENT_ALL_UPDATESUMMARY)
	CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
	if ( pEngineContext ) {

	  #ifdef IMPLEMENT_RESETPROCESSING
		m_pfnReset = &CPeekEngineContext::OnReset;
	  #endif // IMPLEMENT_RESETPROCESSING

	  #ifdef IMPLEMENT_ALL_UPDATESUMMARY
		m_pfnSummary = &CPeekEngineContext::OnSummary;
	  #endif // IMPLEMENT_ALL_UPDATESUMMARY
	}
  #endif // IMPLEMENT_RESETPROCESSING or IMPLEMENT_ALL_UPDATESUMMARY
}


// ----------------------------------------------------------------------------
//		DoDisposeContext
// ----------------------------------------------------------------------------

void
CRemotePlugin::DoDisposeContext()
{
	m_spContext->PreDispose( this );

	if ( IsValidContext() ) {
		if ( m_spContext->CheckState( CPeekContext::kContextState_Created ) ) {
			m_spContext->OnDisposeContext();
		}

		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		if ( pEngineContext ) {
			CPeekEngineProxy* pProxy = pEngineContext->GetProxy();
			if ( pProxy ) {
				pProxy->ReleaseInterfacePointers();
			}
		}
	}
}


// ----------------------------------------------------------------------------
//		DoLoadPlugin
// ----------------------------------------------------------------------------

bool
CRemotePlugin::DoLoadPlugin()
{
	if ( s_Plugin.IsInitialized() ) return true;

	if ( s_PeekProxy.IsEngine() && !HasSite() ) {
		return false;
	}

	s_PeekProxy.SetInterfacePointers();

	int nReturn = s_Plugin.OnLoad( this, s_PeekProxy );
	if ( nReturn != PEEK_PLUGIN_SUCCESS ) {
		Peek::Throw( HE_E_FAIL );
	}

	s_Plugin.OnReadPrefs();

	return true;
}


// ----------------------------------------------------------------------------
//		DoUnloadPlugin
// ----------------------------------------------------------------------------

void
CRemotePlugin::DoUnloadPlugin()
{
	if ( !s_Plugin.IsInitialized() ) return;

	s_Plugin.OnWritePrefs();
	s_Plugin.OnUnload();
}


// ----------------------------------------------------------------------------
//		CreateContext
// ----------------------------------------------------------------------------

HeResult
CRemotePlugin::CreateContext()
{
	try {
		HE_ASSERT( s_Plugin.IsInitialized() );
		if ( HasContext() ) return HE_S_OK;
		if ( s_PeekProxy.IsEngine() && !HasSite() ) return HE_S_OK;
		if ( !HasContextId() ) return HE_S_OK;

		CPeekContextPtr		spContext;
		CContextManagerPtr	spContextManager = GetContextManager();
		HE_ASSERT( spContextManager );
		if ( !spContextManager ) Peek::Throw( HE_E_FAIL );

		if ( !spContextManager->Find( m_idContext, spContext ) ) {
			ASSERT( spContext == NULL );
			if ( s_PeekProxy.IsEngine() ) {
				CPeekEngineContext*		pEngineContext = new COmniEngineContext( m_idContext, this );
				HE_ASSERT( pEngineContext );
				if ( !pEngineContext ) Peek::Throw( HE_E_FAIL );
				HE_ASSERT( m_spUnkSite );
				pEngineContext->SetInterfacePointers( m_spUnkSite, this );
				spContext = CPeekContextPtr( pEngineContext );
			}
			else {
				CPeekConsoleContext*	pConsoleContext = new COmniConsoleContext( m_idContext );
				HE_ASSERT( pConsoleContext );
				if ( !pConsoleContext ) Peek::Throw( HE_E_FAIL );
				pConsoleContext->SetInterfacePointers( m_spUnkSite, this );
				spContext = CPeekContextPtr( pConsoleContext );
			}
			spContextManager->Add( spContext );
		}
		else {
			ASSERT( spContext != NULL );
			if ( s_PeekProxy.IsEngine() ) {
				CPeekEngineContext*		pEngineContext = dynamic_cast<CPeekEngineContext*>( spContext.get() );
				HE_ASSERT( pEngineContext );
				if ( !pEngineContext ) Peek::Throw( HE_E_FAIL );
				pEngineContext->SetInterfacePointers( m_spUnkSite, this );
			}
			else {
				CPeekConsoleContext*	pConsoleContext = dynamic_cast<CPeekConsoleContext*>( spContext.get() );
				HE_ASSERT( pConsoleContext );
				if ( !pConsoleContext ) Peek::Throw( HE_E_FAIL );
				pConsoleContext->SetInterfacePointers( m_spUnkSite, this );
			}
		}

		m_spContext = spContext;
		HE_ASSERT( HasContext() );
		if ( !HasContext() ) return HE_E_FAIL;

		if ( m_spContext && m_spContext->HasGraphs() && (m_spContext->GetGraphsPlugin() == this) ) {
			m_bGraphsContext = true;
		}
	}
	catch ( ... ) {
		ReleaseContext();
		Peek::Throw( HE_E_FAIL );
	}

	return HE_S_OK;
}


// ----------------------------------------------------------------------------
//		LoadContext
// ----------------------------------------------------------------------------

bool
CRemotePlugin::LoadContext()
{
	if ( !s_Plugin.IsInitialized() ) {
		if ( !DoLoadPlugin() ) return false;
	}

	CreateContext();
	DoCreateContext();

	return true;
}


// ----------------------------------------------------------------------------
//		ReleaseContext
// ----------------------------------------------------------------------------

HeResult
CRemotePlugin::ReleaseContext()
{
	if ( HasContext() ) {
		CContextManagerPtr	spContextManager = GetContextManager();
		HE_ASSERT( spContextManager );
		if ( spContextManager ) {
			spContextManager->Remove( m_spContext );
		}

		if ( m_spContext.use_count() == 1 ) {
			DoDisposeContext();
		}
		m_spContext.reset();
	}
	return HE_S_OK;
}
