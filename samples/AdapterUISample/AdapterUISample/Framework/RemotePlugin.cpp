// =============================================================================
//	RemotePlugin.cpp
// =============================================================================
//	Copyright (c) 2008-2015 Savvius, Inc. All rights reserved.

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
#include "OmniEngineContext.h"
#include "PluginController.h"
#include "omnicommands.h"
#include "omnigraphs.h"
#include "peekfilters.h"
#include "imemorystream.h"
#include <memory>
#include <vector>

using std::vector;

#if defined(_DEBUG) && defined(_WIN32)
static bool		s_bDebugEngine = true;
#endif // _DEBUG && _WIN32

#ifdef PEEK_UI
CPluginApp		gPluginApp;
HINSTANCE		GetInstanceHandle() { return gPluginApp.m_hInstance; }
#endif // PEEK_UI

static CPluginController	s_PluginController;

extern CContextManagerPtr	GetContextManager();

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define FMT_HEX04	std::hex << std::uppercase << std::setfill( '0' ) << std::setw( 4 )
#define FMT_HEX08	std::hex << std::uppercase << std::setfill( '0' ) << std::setw( 8 )


#ifdef _WIN32
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
#endif // _WIN32


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

#endif // PEEK_UI


// =============================================================================
//		CRemotePlugin
// =============================================================================

CRemotePlugin::CRemotePlugin()
#ifdef _WIN32
	:	m_hInstance( nullptr )
	,	m_bGraphsContext( false )
#else
	:	m_bGraphsContext( false )
#endif
	,	m_pfnReset( nullptr )
#ifdef IMPLEMENT_ALL_UPDATESUMMARY
	,	m_pfnSummary( nullptr )
#endif // IMPLEMENT_ALL_UPDATESUMMARY
{
  #ifdef IMPLEMENT_NOTIFICATIONS
	m_idAction.SetNull();
  #endif // IMPLEMENT_NOTIFICATIONS

  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Constructed" );
  #endif // DEBUG_LOGINTERFACE
}

CRemotePlugin::~CRemotePlugin()
{
	ReleaseContext();

	CContextManagerPtr	spContextManager( GetContextManager() );
	_ASSERTE( spContextManager.IsValid() );
	if ( spContextManager.IsValid() ) {
		Unload();
	}

  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Destroyed" );

	CContextManagerPtr	spContextManager( GetContextManager() );
	if ( spContextManager.IsValid() ) {
		if ( spContextManager.IsValid()->IsEmpty() ) {
			SaveMsgLog();
		}
	}
  #endif // DEBUG_LOGINTERFACE
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

#if defined(_DEBUG) && defined(_WIN32)
	static std::filebuf	fb;
	if ( !fb.is_open() ) {
		CPeekTimeLocal	ptNow( true );
		std::string		strTempDir = "c:\\temp\\";	// TODO: Get Temp directory.
		std::string		strFileName = strTempDir + "AdapterUISample " + ptNow.FormatTimeA() + ".txt";
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
#endif // _DEBUG && _WIN32
}


// -----------------------------------------------------------------------------
//		SaveMsgLog
// -----------------------------------------------------------------------------

void
CRemotePlugin::SaveMsgLog()
{
	AddLogMsg( "Close" );
}
#endif // DEBUG_LOGINTERFACE


// -----------------------------------------------------------------------------
//		FinalConstruct
// -----------------------------------------------------------------------------

HeResult
CRemotePlugin::FinalConstruct()
{
	try {
  #ifdef DEBUG_LOGINTERFACE
		AddLogMsg( "Engine Instance" );
  #endif // DEBUG_LOGINTERFACE
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
  #endif // DEBUG_LOGINTERFACE
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
	_ASSERTE( outName != nullptr );
	if ( outName == nullptr ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_E_FAIL;

	try {
		HeLib::HeHEBSTRFromString( COmniPlugin::GetName(), outName );
		if ( *outName == nullptr ) return HE_E_OUT_OF_MEMORY;
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
	HeResult	hr = HE_E_FAIL;

	try {
		_ASSERTE( outClassId != nullptr );
		if ( outClassId == nullptr ) return HE_E_NULL_POINTER;

		*outClassId = COmniPlugin::GetClassId();

		hr = HE_S_OK;
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		IPluginAdapter::DeleteAdapter
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::DeleteAdapter(
	const Helium::HeID&	inAdapterId )
{
	HeResult	hr = HE_E_FAIL;

	try {
		int	nResult = m_spPlugin->DeleteAdapter( CGlobalId( inAdapterId ) );
		if ( nResult == PEEK_PLUGIN_SUCCESS ) {
			hr = HE_S_OK;
		}
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
  #endif // DEBUG_LOGINTERFACE

	HeResult hr = HE_S_OK;
	try {
		IHeObjectWithSiteImpl<CRemotePlugin>::SetSite( inUnkSite );
		if ( inUnkSite != nullptr ) {
			DoLoadPlugin();
		}
		else {
			DoUnloadPlugin( false );
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
  #endif // DEBUG_LOGINTERFACE

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
  #endif // DEBUG_LOGINTERFACE

	HeResult	hr = HE_S_OK;
	try {
		ReleaseContext();

		CContextManagerPtr	spContextManager( GetContextManager() );
		_ASSERTE( spContextManager.IsValid() );
		if ( spContextManager.IsValid() && spContextManager->IsEmpty() ) {
			DoUnloadPlugin( false );
		}
		// Not seeing Graphs plugins being deleted...
		if ( spContextManager.IsValid() ) {
			CPeekContextPtr		spContext;
			if ( spContextManager->Find( m_idContext, spContext ) ) {
				CPeekEngineContext*	pEngineContext =
					dynamic_cast<CPeekEngineContext*>( static_cast<CPeekContext*>( m_spContext ) );
				if ( pEngineContext != nullptr ) {
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

	return hr;
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
  #endif // DEBUG_LOGINTERFACE

	if ( HasNoPlugin() && HasContextId() ) {
		COmniPluginPtr	spPlugin;
		if ( s_PluginController.Find( m_idContext, spPlugin ) ) {
			m_spPlugin = spPlugin;
		}
	}

	HeResult	hr = HE_S_OK;
	try {
		LoadContext();
		Peek::ThrowIf( !CPeekXml::Validate( inDocument, inNode ) );
		m_spdmPrefs = std::unique_ptr<CPeekDataModeler>(
			new CPeekDataModeler( inDocument, inNode, kModeler_Load ) );
		if ( m_spdmPrefs.get() != nullptr ) {
			_ASSERTE( m_spdmPrefs->IsOpen() );
			if ( HasContext() && m_spContext->CheckState( CPeekContext::kContextState_Created ) ) {
				m_spContext->OnContextPrefs( *m_spdmPrefs );
				m_spdmPrefs->End();
			}
			else {
				m_spPlugin->OnContextPrefs( *m_spdmPrefs );
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
  #endif // DEBUG_LOGINTERFACE

	//_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_E_FAIL;

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
	_ASSERTE( pID != nullptr );
	if ( pID == nullptr ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	try {
		*pID = m_idAction;
	}
	catch ( ... ) {
		hr = HE_E_UNEXPECTED;
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
	HeResult	hr = HE_S_OK;

	try {
		m_idAction = id;
	}
	catch ( ... ) {
		hr = HE_E_UNEXPECTED;
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
	_ASSERTE( pbstrName != nullptr );
	if ( pbstrName == nullptr ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	try {
		*pbstrName = static_cast<Helium::HEBSTR>( m_strName.AllocSysString() );
	}
	catch ( ... ) {
		hr = HE_E_UNEXPECTED;
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
	HeResult hr = HE_S_OK;

//	try {
//		m_strName = bstrName;
//	}
//	catch ( ... ) {
//		hr = HE_E_UNEXPECTED;
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

	HeResult	hr = HE_S_OK;

	//	try {
	//		m_EnabledSeverities.SetAt( severity, bEnable );
	//	}
	//	catch ( ... ) {
	//		hr = HE_E_UNEXPECTED;
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

	_ASSERTE( pbEnabled != nullptr );
	if ( pbEnabled == nullptr ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	try {
		*pbEnabled = FALSE;

		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		if ( pEngineContext != nullptr ) {
			CPeekEngineProxy* pProxy = pEngineContext->GetProxy();
			if ( pProxy != nullptr ) {
				CNotifyService& NotifySvc = pProxy->GetNotifyService();
				*pbEnabled = NotifySvc.IsSeverityEnabled( severity );
			}
		}
	}
	catch ( ... ) {
		hr = HE_E_UNEXPECTED;
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

	_ASSERTE( ppSources != nullptr );
	if ( ppSources == nullptr ) return HE_E_NULL_POINTER;
	_ASSERTE( pnSources != nullptr );
	if ( pnSources == nullptr ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	try {
		*ppSources = nullptr;
		*pnSources = 0;
/*
		if ( m_ayDisabledSources.GetCount() > 0 ) {
			const size_t	cbSources = m_ayDisabledSources.GetCount() * sizeof(GUID);
			*ppSources = static_cast<HeID*>( HeTaskMemAlloc( cbSources ) );

			if ( *ppSources == nullptr ) AtlThrow( HE_E_OUTOFMEMORY );
			memcpy( *ppSources, m_ayDisabledSources.GetData(), cbSources );
			*pnSources = (ULONG) m_ayDisabledSources.GetCount();
		}
*/
	}
	catch ( ... ) {
		hr = HE_E_UNEXPECTED;
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

	HeResult	hr = HE_S_OK;

	try {
/*
		m_ayDisabledSources.RemoveAll();

		if ( (pSources != nullptr) && (nSources > 0) ) {
			for ( ULONG i = 0; i < nSources; i++ ) {
				m_ayDisabledSources.Add( pSources[i] );
			}
		}
*/
	}
	catch ( ... ) {
		hr = HE_E_UNEXPECTED;
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

	_ASSERTE( pbEnabled != nullptr );
	if ( pbEnabled == nullptr ) return HE_E_NULL_POINTER;

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
						CContextManagerPtr	spContextManager( GetContextManager() );
						_ASSERTE( spContextManager.IsValid() );
						if ( spContextManager.IsValid() && spContextManager->IsIdInManager( CGlobalId( guidSource ) ) ) {
							*pbEnabled = FALSE;
						}
					}
				}
				break;

			case NOTIFY_SOURCES_EXCLUDE_ALL_INSTANCES:
				{
					*pbEnabled = TRUE;
					CContextManagerPtr	spContextManager( GetContextManager() );
					_ASSERTE( spContextManager.IsValid() );
					if ( spContextManager.IsValid() && spContextManager->IsIdInManager( CGlobalId( guidSource ) ) ) {
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
						CContextManagerPtr	spContextManager( GetContextManager() );
						_ASSERTE( spContextManager.IsValid() );
						if ( spContextManager.IsValid() && spContextManager->IsIdInManager( CGlobalId( guidSource ) ) ) {
							*pbEnabled = TRUE;
						}
					}
				}
				break;

			case NOTIFY_SOURCES_INCLUDE_ALL_INSTANCES:
				{
					*pbEnabled = FALSE;
					CContextManagerPtr	spContextManager( GetContextManager() );
					_ASSERTE( spContextManager.IsValid() );
					if ( spContextManager.IsValid() && spContextManager->IsIdInManager( CGlobalId( guidSource ) ) ) {
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
		hr = HE_E_UNEXPECTED;
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
		hr = HE_E_UNEXPECTED;
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
  #endif // DEBUG_LOGINTERFACE

	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

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
		CPeekEngineContext*	pEngineContext = nullptr;

		pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );

		int			nResult = PEEK_PLUGIN_FAILURE;
		CPeekStream	psPayload;
		if ( !psMessage.CopyTo( psPayload, cbMsgLen ) ) Peek::Throw( E_FAIL );
		psPayload.Rewind();

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
  #endif // DEBUG_LOGINTERFACE

	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

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
  #endif // DEBUG_LOGINTERFACE

	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

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
  #endif // DEBUG_LOGINTERFACE

	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

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
  #endif // DEBUG_LOGINTERFACE

	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

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


//---------------------------------------------------------------------
//		IPeekCaptureEvents::OnCaptureStarting
//---------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::OnCaptureStarting()
{
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Capture Starting" );
  #endif // DEBUG_LOGINTERFACE

	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		_ASSERTE( pEngineContext );
		if ( pEngineContext ) {
			int	nResult = pEngineContext->OnCaptureStarting();
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}


//---------------------------------------------------------------------
//		IPeekCaptureEvents::OnCaptureStarted
//---------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::OnCaptureStarted()
{
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Capture Started" );
  #endif // DEBUG_LOGINTERFACE

	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		_ASSERTE( pEngineContext );
		if ( pEngineContext ) {
			int	nResult = pEngineContext->OnCaptureStarted();
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}


//---------------------------------------------------------------------
//		IPeekCaptureEvents::OnCaptureStopping
//---------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::OnCaptureStopping()
{
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Capture Stopping" );
  #endif // DEBUG_LOGINTERFACE

	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		_ASSERTE( pEngineContext );
		if ( pEngineContext ) {
			int	nResult = pEngineContext->OnCaptureStopping();
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}


//---------------------------------------------------------------------
//		IPeekCaptureEvents::OnCaptureStopped
//---------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::OnCaptureStopped()
{
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Capture Stopped" );
  #endif // DEBUG_LOGINTERFACE

	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	HeResult	hr = HE_E_FAIL;
	try {
		CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
		_ASSERTE( pEngineContext );
		if ( pEngineContext ) {
			int	nResult = pEngineContext->OnCaptureStopped();
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

	_ASSERTE( pPacket != nullptr );
	if ( pPacket == nullptr ) return HE_E_NULL_POINTER;
	_ASSERTE( pcbBytesToAccept != nullptr );
	if ( pcbBytesToAccept == nullptr ) return HE_E_NULL_POINTER;
	_ASSERTE( pFilterID != nullptr );
	if ( pFilterID == nullptr ) return HE_E_NULL_POINTER;

	// Do not lock.

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

	_ASSERTE( pPacket != nullptr );
	if ( pPacket == nullptr ) return HE_E_NULL_POINTER;

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

	_ASSERTE( pSnapshot != nullptr );
	if ( pSnapshot == nullptr ) return HE_E_NULL_POINTER;

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


// -----------------------------------------------------------------------------
//		IPluginUISource::GetHandlerID
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::GetHandlerID(
	Helium::HeID*	pHandlerID )
{
#ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Get Options Handler ID" );
#endif // DEBUG_LOGINTERFACE

	_ASSERTE( pHandlerID != nullptr );
	if ( pHandlerID == nullptr ) return HE_E_INVALID_ARG;

// 	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		CGlobalId	id;
		int	nResult = m_spPlugin->OnGetOptionsHandlerId( id );
		if ( nResult == PEEK_PLUGIN_SUCCESS ) {
			*pHandlerID = id;
			hr = HE_S_OK;
		}
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		IPluginUISource::SetHandlerID
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::SetHandlerID(
	const Helium::HeID* pHandlerID )
{
	(void) pHandlerID;
	return HE_E_NOT_IMPLEMENTED;
}


// -----------------------------------------------------------------------------
//		IPluginUISource::GetData
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::GetData(
	int					nType,
	UInt32*				pFlags,
	Helium::IHeStream*	pOutData )
{
#ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Get Data" );
#endif // DEBUG_LOGINTERFACE

	_ASSERTE( pFlags != nullptr );
	_ASSERTE( pOutData != nullptr );
	if ( (pFlags == nullptr) || (pOutData == nullptr) ) return HE_E_INVALID_ARG;

	_ASSERTE( HasPlugin() );
	if ( HasNoPlugin() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		int			nResult = PEEK_PLUGIN_FAILURE;
		UInt32	nFlags( 0 );
		CPeekStream	psData( pOutData );
		if ( nType == peekPluginHandlerUI_About ) {
			nResult = m_spPlugin->OnGetAboutData( &nFlags, psData );
		}
		else if ( nType == peekPluginHandlerUI_Options ) {
			nResult = m_spPlugin->OnGetOptionsData( &nFlags, psData );
		}
		else if ( nType == peekPluginHandlerUI_CaptureTab ) {
			nResult = m_spPlugin->OnGetCaptureTabData( &nFlags, psData );
		}
		else if ( nType == peekPluginHandlerUI_EngineTab ) {
			nResult = m_spPlugin->OnGetEngineTabData( &nFlags, psData );
		}
		if ( nResult == PEEK_PLUGIN_SUCCESS ) {
			*pFlags = nFlags;
			hr = HE_S_OK;
		}
	}
	HE_CATCH( hr );

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
		_ASSERTE( (m_spdmPrefs.get() != nullptr) ? m_spdmPrefs->IsOpen() : true );
		if ( m_spdmPrefs.get() && m_spdmPrefs->IsOpen() ) {
			m_spContext->OnContextPrefs( *m_spdmPrefs );
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
  #endif // DEBUG_LOGINTERFACE
	if ( HasInitializedPlugin() ) return true;

	if ( !HasSite() ) return false;

	if ( HasNoPlugin() ) {
		COmniPluginPtr	spPlugin;
		if ( !s_PluginController.Find( nullptr, spPlugin ) ) {
			s_PluginController.Add( nullptr, this, spPlugin );
		}
		m_spPlugin = spPlugin;
	}
	_ASSERTE( HasPlugin() );
	if ( HasNoPlugin() ) {
		Peek::Throw( HE_E_FAIL );
	}

	m_PeekProxy.SetInterfacePointers( m_spUnkSite );

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
CRemotePlugin::DoUnloadPlugin(
	bool inMustHavePlugin )
{
#ifdef _DEBUG
	_ASSERTE( !inMustHavePlugin && HasPlugin() );
#else
	(void)inMustHavePlugin;
#endif

	if ( HasNoInitializedPlugin() ) return;
#ifdef _WIN32
	m_spPlugin->OnWritePrefs();
#endif // _WIN32

	long	nUseCount( m_spPlugin.use_count() );
	// If only this->m_spPlugin and s_PluginController have references,
	// then Unload and remove the plugin.
	// This destructor will remove the final reference to the plugin.
	if ( nUseCount <= 2 ) {
		m_spPlugin->OnUnload();
		s_PluginController.Remove( nullptr );
	}
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
		CContextManagerPtr	spContextManager( GetContextManager() );
		_ASSERTE( spContextManager.IsValid() );
		if ( spContextManager.IsNotValid() ) Peek::Throw( HE_E_FAIL );

		if ( !spContextManager->Find( m_idContext, spContext ) ) {
			_ASSERTE( spContext.IsNotValid() );
#ifdef WP_LINUX
			_ASSERTE( m_PeekProxy.IsEngine() );
#endif // WP_LINUX
			if ( m_PeekProxy.IsEngine() ) {
				CPeekEngineContext*		pEngineContext = new COmniEngineContext( m_idContext, this );
				_ASSERTE( pEngineContext );
				if ( !pEngineContext ) Peek::Throw( HE_E_FAIL );
				_ASSERTE( m_spUnkSite );
				pEngineContext->SetInterfacePointers( m_spUnkSite, m_spPlugin );
				spContext = CPeekContextPtr( pEngineContext );
			}
			spContextManager->Add( spContext );
		}
		else {
			_ASSERTE( spContext.IsValid() );
#ifdef WP_LINUX
			_ASSERTE( m_PeekProxy.IsEngine() );
#endif // WP_LINUX
			if ( m_PeekProxy.IsEngine() ) {
				CPeekEngineContext*		pEngineContext = dynamic_cast<CPeekEngineContext*>( spContext.get() );
				_ASSERTE( pEngineContext );
				if ( !pEngineContext ) Peek::Throw( HE_E_FAIL );
				pEngineContext->SetInterfacePointers( m_spUnkSite, m_spPlugin.get() );
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
		if ( !HasContext() ) {
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
		CContextManagerPtr	spContextManager( GetContextManager() );
		_ASSERTE( spContextManager.IsValid() );
		if ( spContextManager.IsValid() ) {
			spContextManager->Remove( m_spContext );
		}

		if ( m_spContext.use_count() == 1 ) {
			DoDisposeContext();
		}
		m_spContext.reset();
	}

	return HE_S_OK;
}
