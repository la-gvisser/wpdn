// =============================================================================
//	RemotePlugin.cpp
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

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
#include "PluginController.h"
#include <memory>
#include <vector>

using std::vector;

#ifdef _DEBUG
static bool		s_bDebugEngine = true;
#endif

#ifdef PEEK_UI
CPluginApp		gPluginApp;
HINSTANCE		GetInstanceHandle() { return gPluginApp.m_hInstance; }
#endif

static CPluginController	s_PluginController;

extern CContextManagerPtr	GetContextManager();

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define FMT_HEX04	std::hex << std::setfill( '0' ) << std::setw( 4 )
#define FMT_HEX08	std::hex << std::setfill( '0' ) << std::setw( 8 )


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
	_ASSERTE( se_set == 1 );
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

  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Constructed" );
  #endif
}

CRemotePlugin::~CRemotePlugin()
{
	ReleaseContext();

  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Destroyed" );

	CContextManagerPtr	pContMgr = GetContextManager();
	if ( pContMgr ) {
		if ( pContMgr->IsEmpty() ) {
			SaveMsgLog();
		}
	}
  #endif
}


#ifdef DEBUG_LOGINTERFACE
// -----------------------------------------------------------------------------
//		AddLogMsg
// -----------------------------------------------------------------------------

void
CRemotePlugin::AddLogMsg(
	const std::string& inMessage )
{
	inMessage;

#if _DEBUG
	static std::filebuf	fb;
	if ( !fb.is_open() ) {
		CPeekTimeLocal	ptNow( true );
		std::string		strFileName = "c:\\temp\\AAAPlugin33 " + ptNow.FormatTimeA() + ".txt";
		fb.open( strFileName.c_str(), std::ios::out );
	}
	std::ostream	os( &fb );

	CRemotePlugin*		pPlugin = this;;
	std::ostringstream	strmMsg;
	strmMsg << FMT_HEX08 << pPlugin << ": " << m_idContext.FormatA() << ": " << inMessage;

	os << strmMsg.str() << std::endl;

	if ( inMessage == "Close" ) {
		fb.close();
	}
#endif
}


// -----------------------------------------------------------------------------
//		SaveMsgLog
// -----------------------------------------------------------------------------

void
CRemotePlugin::SaveMsgLog()
{
	AddLogMsg( "Close" );
}
#endif


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
				m_PeekProxy.SetConsoleHost();
			  #ifdef DEBUG_LOGINTERFACE
				AddLogMsg( "Console Instance" );
			}
			else {
				AddLogMsg( "Engine Instance" );
			  #endif
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
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Final Release" );
  #endif
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
	_ASSERTE( outName != NULL );
	if ( outName == NULL ) return HE_E_NULL_POINTER;

//	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;

	try {
		HeLib::HeHEBSTRFromString( COmniPlugin::GetName(), outName );
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
		_ASSERTE( outClassId != NULL );
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
  #ifdef DEBUG_LOGINTERFACE
	std::ostringstream	strmMsg;
	strmMsg << "SetSite : Site = " << inUnkSite;
	AddLogMsg( strmMsg );
  #endif

	ObjectLock	lock( this );

	if ( HasSite() ) return HE_S_OK;

	HeResult hr = HE_S_OK;
	try {
		IHeObjectWithSiteImpl<CRemotePlugin>::SetSite( inUnkSite );
		if ( inUnkSite != NULL ) {
			DoLoadPlugin();
		}
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
  #ifdef DEBUG_LOGINTERFACE
	std::ostringstream	strmMsg;
	CGlobalId			idContext( inContextId );
	strmMsg << "Load : Id = " << idContext.FormatA();
	AddLogMsg( strmMsg );
  #endif

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
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Unload" );
  #endif

	ObjectLock	lock( this );

	HeResult	hr = HE_S_OK;
	try {
		ReleaseContext();

		CContextManagerPtr	spContextManager = GetContextManager();
		_ASSERTE( spContextManager.IsValid() );
		if ( spContextManager.IsValid() && spContextManager->IsEmpty() ) {
			DoUnloadPlugin();
		}
		// Not seeing Graphs plugins being deleted...
		if ( spContextManager.IsValid() ) {
			CPeekContextPtr		spContext;
			if ( spContextManager->Find( m_idContext, spContext ) ) {
				CPeekEngineContext*	pEngineContext =
					dynamic_cast<CPeekEngineContext*>( static_cast<CPeekContext*>( m_spContext ) );
				if ( pEngineContext != NULL ) {
					CPeekPlugin*	pGraphsPlugin = pEngineContext->GetGraphsPlugin();
					if ( pGraphsPlugin && (pGraphsPlugin->GetRemotePlugin() != this)  ) {
						pGraphsPlugin->GetRemotePlugin()->Unload();
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
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Load XML" );
  #endif

	if ( HasNoPlugin() && HasContextId() ) {
		COmniPluginPtr	spPlugin;
		if ( s_PluginController.Find( m_idContext, spPlugin ) ) {
			m_spPlugin = spPlugin;
		}
	}

	ObjectLock	lock( this );

	HeResult	hr = HE_S_OK;
	try {
		LoadContext();
		Peek::ThrowIf( !CPeekXml::Validate( inDocument, inNode ) );
		m_spdmPrefs = CPeekDataModelerPtr( new CPeekDataModeler( inDocument, inNode, kModeler_Load ) );
		if ( m_spdmPrefs.get() != NULL ) {
			_ASSERTE( m_spdmPrefs->IsOpen() );
			if ( HasContext() && m_spContext->CheckState( CPeekContext::kContextState_Created ) ) {
				m_spContext->OnContextPrefs( *m_spdmPrefs.get() );
				m_spdmPrefs->End();
			}
			else {
				m_spPlugin->OnContextPrefs( *m_spdmPrefs.get() );
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
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Save XML" );
  #endif

	//_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_E_FAIL;

	ObjectLock	lock( this );

	HeResult	hr = HE_S_OK;
	try {
		LoadContext();

		Peek::ThrowIf( !CPeekXml::Validate( ioDocument, ioNode ) );
		CPeekDataModeler pdmPrefs( ioDocument, ioNode, kModeler_Store );

		if ( HasContext() && m_spContext->CheckState( CPeekContext::kContextState_Created ) ) {
			m_spContext->OnContextPrefs( pdmPrefs );
		}
		else {
			m_spPlugin->OnContextPrefs( pdmPrefs );
		}
		pdmPrefs.End();
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

	_ASSERTE( pID != NULL );
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

	_ASSERTE( pbstrName != NULL );
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
	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

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
	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	ObjectLock	lock( this );

	_ASSERTE( pbEnabled != NULL );
	if ( pbEnabled == NULL ) return E_POINTER;

	HeResult	hr = HE_S_OK;

	try {
		*pbEnabled = FALSE;

		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		if ( pEngineContext != NULL ) {
			CPeekEngineProxy* pProxy = pEngineContext->GetProxy();
			if ( pProxy != NULL ) {
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
	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	ObjectLock	lock( this );

	_ASSERTE( ppSources != NULL );
	if ( ppSources == NULL ) return E_POINTER;
	_ASSERTE( pnSources != NULL );
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
	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

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
	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	ObjectLock	lock( this );

	_ASSERTE( pbEnabled != NULL );
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
						_ASSERTE( *pbEnabled == TRUE );
					}
					else {
						CContextManagerPtr	spContextManager = GetContextManager();
						_ASSERTE( spContextManager );
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
					_ASSERTE( spContextManager );
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
						_ASSERTE( *pbEnabled == FALSE );
					}
					else {
						CContextManagerPtr	spContextManager = GetContextManager();
						_ASSERTE( spContextManager );
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
					_ASSERTE( spContextManager );
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
	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

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
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Process Plugin Message" );
  #endif

	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekStream psMessage( inMessage );
		CPeekStream psResponse( outResponse );

		psMessage.Rewind();

		PluginMessageHdr	hdrReqPluginMessage;
		size_t				nResultBytes = psMessage.Read( sizeof( PluginMessageHdr ), &hdrReqPluginMessage );
		_ASSERTE( nResultBytes == sizeof( PluginMessageHdr ) );
		if ( nResultBytes != sizeof( PluginMessageHdr ) ) Peek::Throw( E_FAIL );

		_ASSERTE( inId == COmniPlugin::GetClassId() );
		_ASSERTE( hdrReqPluginMessage.PluginID == COmniPlugin::GetClassId() );
		if ( (inId != COmniPlugin::GetClassId()) ||
			(hdrReqPluginMessage.PluginID != COmniPlugin::GetClassId()) ) {
				Peek::Throw( E_FAIL );
		}

		UInt32				cbMsgLen = hdrReqPluginMessage.MessageSize;
		CPeekEngineContext*	pEngineContext = NULL;

		pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );

		int			nResult = PEEK_PLUGIN_FAILURE;
		CPeekStream	psPayload;
		if ( !psMessage.CopyTo( psPayload, cbMsgLen ) ) Peek::Throw( E_FAIL );

		if ( pEngineContext ) {
			_ASSERTE( hdrReqPluginMessage.CaptureID == m_idContext );
			if ( hdrReqPluginMessage.CaptureID != m_idContext ) Peek::Throw( E_FAIL );
			nResult = pEngineContext->OnProcessPluginMessage( psPayload, psResponse );
		}
		else if ( HasPlugin() ) {
			_ASSERTE( hdrReqPluginMessage.CaptureID == HeID::Null() );
			if ( hdrReqPluginMessage.CaptureID != HeID::Null() ) Peek::Throw( E_FAIL );
			nResult = m_spPlugin->OnProcessPluginMessage( psPayload, psResponse );
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
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Reset Processing" );
  #endif

	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		_ASSERTE( pEngineContext && m_pfnReset );
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
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Start Packet Processing" );
  #endif

	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	ObjectLock	lock( this );

	_ASSERTE( ullStartTime != 0 );

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		_ASSERTE( pEngineContext );
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
   #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Stop Packet Processing" );
  #endif

	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	ObjectLock	lock( this );

	_ASSERTE( ullStopTime != 0 );

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		_ASSERTE( pEngineContext );
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
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Reset Packet Processing" );
  #endif

	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_S_OK;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		_ASSERTE( pEngineContext );
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
	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;
	if ( IsGraphs() ) return HE_S_OK;		// Only filter on the Primary instance.

	_ASSERTE( pPacket != NULL );
	if ( pPacket == NULL ) return HE_E_NULL_POINTER;
	_ASSERTE( pcbBytesToAccept != NULL );
	if ( pcbBytesToAccept == NULL ) return HE_E_NULL_POINTER;
	_ASSERTE( pFilterID != NULL );
	if ( pFilterID == NULL ) return HE_E_NULL_POINTER;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		_ASSERTE( pEngineContext );
		if ( pEngineContext ) {
			CPacket		Packet( pPacket );
			CGlobalId	idFilter( GUID_NULL );		// Set to *pFilterID, if it gets initialized.
			int			nResult = pEngineContext->OnFilter( Packet, pcbBytesToAccept, idFilter );
			if ( nResult == PLUGIN_PACKET_ACCEPT ) {
				hr = HE_S_OK;
			}
			else if ( nResult == PLUGIN_PACKET_REJECT ) {
				hr = HE_S_FALSE;
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
	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;
	if ( IsGraphs() ) return HE_S_OK;

	_ASSERTE( pPacket != NULL );
	if ( pPacket == NULL ) return HE_E_NULL_POINTER;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		_ASSERTE( pEngineContext );
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
	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	_ASSERTE( ullCurrentTime != 0 );

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		_ASSERTE( pEngineContext );
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
	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	_ASSERTE( pSnapshot != NULL );
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
			nResult = ((*pEngineContext).*m_pfnSummary)();
		  #endif // IMPLEMENT_UPDATESUMMARYSTATS
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;

			  #ifdef IMPLEMENT_SUMMARYMODIFYENTRY
				CPeekContextProxy* pProxy = pEngineContext->GetPeekContextProxy();
				_ASSERTE( pProxy );
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
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Recv" );
  #endif

	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult hr = HE_S_OK;

	try {
		CPeekStream		psMessage( inMessage );
		CPeekStream		psPayload;
		CRemotePlugin*	pRemotePlugin = NULL;

		if ( HE_SUCCEEDED( inMsgResult ) ) {
			PluginMessageHdr	hdrReqPluginMessage;

			psMessage.Rewind();
			size_t nResultBytes = psMessage.Read( sizeof( PluginMessageHdr ), &hdrReqPluginMessage );
			_ASSERTE( nResultBytes == sizeof( PluginMessageHdr ) );
			if ( nResultBytes != sizeof( PluginMessageHdr ) ) Peek::Throw( HE_E_FAIL );

			bool	bFound = s_PluginController.Find(
				m_spPlugin->GetConsoleSend().GetPtr(), hdrReqPluginMessage.CaptureID, pRemotePlugin );
			if ( !bFound ) Peek::Throw( HE_E_FAIL );

			_ASSERTE( hdrReqPluginMessage.PluginID == COmniPlugin::GetClassId() );
			if ( hdrReqPluginMessage.PluginID != COmniPlugin::GetClassId() ) {
				Peek::Throw( HE_E_FAIL );
			}

			UInt32			cbMsgLen = hdrReqPluginMessage.MessageSize;
			bool			bResult = psMessage.CopyTo( psPayload, cbMsgLen );
			if ( !bResult ) Peek::Throw( HE_E_FAIL );
		}

		int						nResult = PEEK_PLUGIN_FAILURE;
		CPeekConsoleContext*	pConsoleContext = NULL;

		if ( pRemotePlugin->GetContextId().IsNotNull() ) {
			pConsoleContext = dynamic_cast<CPeekConsoleContext*>( pRemotePlugin->GetContext() );
		}

		if ( pConsoleContext ) {
			nResult = pConsoleContext->OnProcessPluginResponse( psPayload, inMsgResult, inTransId );
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
		else {
			if ( pRemotePlugin ) {
				nResult = pRemotePlugin->m_spPlugin->OnProcessPluginResponse(
					psPayload, inMsgResult, inTransId );
			}
			else {
				nResult = m_spPlugin->OnProcessPluginResponse(
					psPayload, inMsgResult, inTransId );
			}
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
#ifdef DEBUG_LOGINTERFACE
	std::ostringstream		strmMsg;
	strmMsg << "Set Engine   : Engine = " << inEngine <<
		" Has context: " << std::boolalpha << HasContext();
	AddLogMsg( strmMsg );
#endif

	m_PeekProxy.ValidateConsole();

	ObjectLock	lock( this );

	IConsoleSend*	pConsoleSend = NULL;
	IEngineInfo*	pEngineInfo = NULL;
	if ( inEngine != NULL ) {
		inEngine->QueryInterface( &pConsoleSend );
		inEngine->QueryInterface( &pEngineInfo );
	}

	try {
		if ( m_idContext.IsNotValid() ) {
			// This is the 'Global Plugin'.
			SetContextId( GUID_NULL );
		}

		m_PeekProxy.SetInterfacePointers();
		if ( pConsoleSend != NULL ) {
			COmniPluginPtr	spPlugin;
			if ( s_PluginController.Find( pConsoleSend, spPlugin ) ) {
				s_PluginController.AddOwner( pConsoleSend, this );
			}
			else {
				s_PluginController.Add( pConsoleSend, this, spPlugin );
			}
			m_spPlugin = spPlugin;
			_ASSERTE( HasPlugin() );

			if ( HasPlugin() ) {
				m_spPlugin->SetEngineInfo( pEngineInfo );

			  #ifdef DEBUG_LOGINTERFACE
				std::ostringstream		strmMsg;
				strmMsg << "<Set Engine> : Plugin = " << m_spPlugin.get();
				AddLogMsg( strmMsg );
			  #endif
			}

			LoadContext();
		}
		else {
			if ( HasPlugin() ) {
				CConsoleSend	PluginConsoleSend = m_spPlugin->GetConsoleSend();
				if ( PluginConsoleSend.IsValid() ) {
					s_PluginController.Remove( PluginConsoleSend.GetPtr(), this );
				}
				m_spPlugin.reset();
			}
		}
	}
	catch ( ... ) {
		// squelch
	}

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IPluginUI::SetConsoleUI
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::SetConsoleUI(
	IConsoleUI* inConsoleUI )
{
  #ifdef DEBUG_LOGINTERFACE
	std::ostringstream		strmMsg;
	strmMsg << "Set Console UI : Console UI = " << inConsoleUI <<
		" Has context = " << std::boolalpha << HasContext();
	AddLogMsg( strmMsg );
  #endif

	m_PeekProxy.ValidateConsole();

	if ( IsGraphs() ) return HE_S_OK;

	ObjectLock	lock( this );

	try {
		if ( inConsoleUI != NULL ) {
			LoadContext();
		}

		m_PeekProxy.SetInterfacePointers();
		SetConsole( inConsoleUI );

		CPeekConsoleContext*	pConsoleContext = dynamic_cast<CPeekConsoleContext*>( m_spContext.get() );
		if ( pConsoleContext ) {
			pConsoleContext->SetConsoleUI( inConsoleUI );
		}
	}
	catch ( ... ) {
		// squelch
	}

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
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Get UI Info" );
  #endif

	outCommand;
	outText;

	HeResult	hr = HE_E_FAIL;

  #ifdef IMPLEMENT_TAB
	m_PeekProxy.ValidateConsole();
	if ( IsGraphs() ) return HE_E_FAIL;

	_ASSERTE( outCommand != NULL );
	_ASSERTE( outText != NULL );
	if ( (outCommand == NULL) || (outText == NULL ) ) return HE_E_FAIL;

	ObjectLock	lock( this );

	try {
		if ( HasPlugin() && m_spPlugin->IsInstalledOnEngine() ) {
			CPeekString	strCmd( COmniPlugin::GetClassId().Format() );
			CHeBSTR	bstrCommand( strCmd );
			CHeBSTR		bstrText( COmniPlugin::GetName() );
			bstrCommand.CopyTo( outCommand );
			bstrText.CopyTo( outText );
		}

		hr = HE_S_OK;	// Return OK if outCommand and outText are set.
	}
	HE_CATCH( hr );
  #endif // IMPLEMENT_TAB

	return hr;
}


// -----------------------------------------------------------------------------
//		IPluginUI::LoadView
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::LoadView()
{
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Load View" );
  #endif

	m_PeekProxy.ValidateConsole();
	if ( IsGraphs() ) return HE_S_OK;

	_ASSERTE( HasPlugin() );
	if ( HasNoPlugin() ) return HE_E_FAIL;

	HeResult	hr = HE_E_FAIL;
	try {
		if ( m_idContext.IsValid() && m_idContext.IsNotNull() ) {
			CPeekConsoleContext*	pConsoleContext = dynamic_cast<CPeekConsoleContext*>( m_spContext.get() );
			if ( (pConsoleContext != NULL) && pConsoleContext->GetConsoleUI().IsValid() ) {
				// Load the Context's View.
				// _ASSERTE( HasSite() );
				int	nResult = pConsoleContext->OnLoadView();
				if ( nResult == PEEK_PLUGIN_SUCCESS ) {
					hr = HE_S_OK;
				}
			}
		}
		else {
			// Load the Plugin's View.
			_ASSERTE( !HasSite() );
			if( m_idContext.IsNotValid() ) {
				SetContextId( CGlobalId( GUID_NULL ) );
			}
			LoadContext();
			m_spPlugin->OnLoadView();
		}
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
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Unload View" );
  #endif

	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	m_PeekProxy.ValidateConsole();
	if ( IsGraphs() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		if ( m_idContext.IsNotNull() ) {
			CPeekConsoleContext*	pConsoleContext = dynamic_cast<CPeekConsoleContext*>( m_spContext.get() );
			if ( pConsoleContext ) {
				int	nResult = pConsoleContext->OnUnloadView();
				if ( nResult == PEEK_PLUGIN_SUCCESS ) {
					hr = HE_S_OK;
				}
			}
		}
		else {
			Unload();		// Todo: UnloadView() - Plugin only.
		}
		hr = HE_S_OK;
	}
	HE_CATCH( hr );

	return HE_S_OK;
}


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
	CPeekProxy::ValidateConsole();
	if ( HasNoInitializedPlugin() ) return HE_E_FAIL;

	_ASSERTE( pPacket != NULL );
	if ( pPacket == NULL ) return HE_E_NULL_POINTER;
	_ASSERTE( pbstrSummary != NULL );
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
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Show About" );
  #endif

	if ( HasNoPlugin() ) {
		COmniPluginPtr	spPlugin;
		if ( s_PluginController.FindFirst( spPlugin ) ) {
			m_spPlugin = spPlugin;
		}
	}

	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;
	if ( IsGraphs() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_NOT_IMPLEMENTED;

  #ifdef HE_WIN32
	try {
		m_spPlugin->OnAbout();
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
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Show Options" );
  #endif

	m_PeekProxy.ValidateConsole();

	if ( HasNoPlugin() ) {
		COmniPluginPtr	spPlugin;
		if ( s_PluginController.FindFirst( spPlugin ) ) {
			m_spPlugin = spPlugin;
		}
	}

	_ASSERTE( HasPlugin() );
	if ( HasNoPlugin() ) return HE_S_OK;
	if ( IsGraphs() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_NOT_IMPLEMENTED;

  #ifdef HE_WIN32
	try {
		LoadContext();
		if ( HasContext() && m_spContext->CheckState( CPeekContext::kContextState_Created ) ) {
			CPeekConsoleContext*	pContext =
				dynamic_cast<CPeekConsoleContext*>( m_spContext.get() );
			if ( pContext != NULL ) {
				pContext->OnOptions();
			}
		}
		else {
			m_spPlugin->OnOptions();
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
	if ( m_spContext->GetId().IsNotNull() && m_spContext->CheckState( CPeekContext::kContextState_Created ) ) {
		_ASSERTE( (m_spdmPrefs.get() != NULL) ? m_spdmPrefs->IsOpen() : true );
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
	if ( HasPlugin() ) {
		m_spContext->PreDispose( m_spPlugin );

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
}


// ----------------------------------------------------------------------------
//		DoLoadPlugin
// ----------------------------------------------------------------------------

bool
CRemotePlugin::DoLoadPlugin()
{
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "DoLoadPlugin" );
  #endif
	if ( HasInitializedPlugin() ) return true;

	if ( m_PeekProxy.IsEngine() ) {
		if ( !HasSite() ) return false;

		if ( HasNoPlugin() ) {
			COmniPluginPtr	spPlugin;
			if ( !s_PluginController.Find( NULL, spPlugin ) ) {
				s_PluginController.Add( NULL, this, spPlugin );
			}
			m_spPlugin = spPlugin;
		}
	}
	else if ( m_PeekProxy.IsConsole() ) {
		if ( HasNoPlugin() ) return false;
		if ( !m_spPlugin->HasConsoleSend() ) return false;
	}

	_ASSERTE( HasPlugin() );
	if ( HasNoPlugin() ) {
		Peek::Throw( HE_E_FAIL );
	}

	m_PeekProxy.SetInterfacePointers();

  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Call Plugin::OnLoad" );
  #endif
	int nReturn = m_spPlugin->OnLoad( this );
	if ( nReturn != PEEK_PLUGIN_SUCCESS ) {
		Peek::Throw( HE_E_FAIL );
	}

	m_spPlugin->OnReadPrefs();

	return true;
}


// ----------------------------------------------------------------------------
//		DoUnloadPlugin
// ----------------------------------------------------------------------------

void
CRemotePlugin::DoUnloadPlugin()
{
	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return;

	m_spPlugin->OnWritePrefs();
	m_spPlugin->OnUnload();
}


// ----------------------------------------------------------------------------
//		CreateContext
// ----------------------------------------------------------------------------

HeResult
CRemotePlugin::CreateContext()
{
	_ASSERTE( HasInitializedPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_E_FAIL;

	try {
		if ( HasContext() ) return HE_S_OK;
		if ( m_PeekProxy.IsEngine() && !HasSite() ) return HE_S_OK;
		if ( !HasContextId() ) return HE_S_OK;

		CPeekContextPtr		spContext;
		CContextManagerPtr	spContextManager = GetContextManager();
		_ASSERTE( spContextManager );
		if ( !spContextManager ) Peek::Throw( HE_E_FAIL );

		if ( !spContextManager->Find( m_idContext, spContext ) ) {
			_ASSERTE( spContext == NULL );
			if ( m_PeekProxy.IsEngine() ) {
				CPeekEngineContext*		pEngineContext = new COmniEngineContext( m_idContext, this );
				_ASSERTE( pEngineContext );
				if ( !pEngineContext ) Peek::Throw( HE_E_FAIL );
				_ASSERTE( m_spUnkSite );
				pEngineContext->SetInterfacePointers( m_spUnkSite, m_spPlugin );
				spContext = CPeekContextPtr( pEngineContext );
			}
			else {
				CPeekConsoleContext*	pConsoleContext = new COmniConsoleContext( m_idContext, this );
				_ASSERTE( pConsoleContext );
				if ( !pConsoleContext ) Peek::Throw( HE_E_FAIL );
				pConsoleContext->SetInterfacePointers( m_spUnkSite, m_spPlugin.get() );
				spContext = CPeekContextPtr( pConsoleContext );
			}
			spContextManager->Add( spContext );
		}
		else {
			_ASSERTE( spContext.IsValid() );
			if ( m_PeekProxy.IsEngine() ) {
				CPeekEngineContext*		pEngineContext = dynamic_cast<CPeekEngineContext*>( spContext.get() );
				_ASSERTE( pEngineContext );
				if ( !pEngineContext ) Peek::Throw( HE_E_FAIL );
				pEngineContext->SetInterfacePointers( m_spUnkSite, m_spPlugin.get() );
			}
			else {
				CPeekConsoleContext*	pConsoleContext = dynamic_cast<CPeekConsoleContext*>( spContext.get() );
				_ASSERTE( pConsoleContext );
				if ( !pConsoleContext ) Peek::Throw( HE_E_FAIL );
				pConsoleContext->SetInterfacePointers( m_spUnkSite, m_spPlugin.get() );
			}
		}

		m_spContext = spContext;
		_ASSERTE( HasContext() );
		if ( !HasContext() ) return HE_E_FAIL;

		if ( m_spContext.IsValid() && m_spContext->HasGraphs() &&
				(m_spContext->GetGraphsPlugin() == m_spPlugin) ) {
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
	if ( HasNoInitializedPlugin() ) {
		if ( !DoLoadPlugin() ) return false;
	}

	if ( HasInitializedPlugin() ) {
		bool	bConsoleReady = (m_PeekProxy.IsConsole()) ? m_spPlugin->HasConsoleSend() : true;
		if ( !HasContext() && bConsoleReady ) {
			CreateContext();
			DoCreateContext();
		}
	}

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
		_ASSERTE( spContextManager );
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