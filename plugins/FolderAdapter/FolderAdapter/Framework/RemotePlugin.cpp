// =============================================================================
//	RemotePlugin.cpp
// =============================================================================
//	Copyright (c) 2008-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "resource.h"
#include "RemotePlugin.h"
#include "RemoteAdapter.h"
#include "ContextManager.h"
#include "PeekProxy.h"
#include "Protospecs.h"
#include "omnigraphs.h"
#include "peekfilters.h"
#include "peeknames.h"
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

using namespace Helium;
using namespace HeLib;
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


// ============================================================================
//		GetPluginController
// ============================================================================

CPluginController&
GetPluginController() {
	return s_PluginController;
}


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
	:	m_pPlugin( nullptr )
	,	m_nPluginAdded( 0 )
#ifdef _WIN32
	,	m_hInstance( nullptr )
#endif
	,	m_nState( kState_Uninitialized )
	,	m_bGraphsContext( false )
{
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Constructed" );
  #endif
	m_spUnkSite = nullptr;
}

CRemotePlugin::~CRemotePlugin()
{
 	ReleaseContext();
	if ( HasSite() ) {
		DoUnloadPlugin();
	}
	m_nState = kState_Uninitialized;
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

#if defined(_DEBUG)
	static std::filebuf	fb;
	if ( !fb.is_open() ) {
		CPeekTimeLocal	ptNow( true );
#ifdef _WIN32
		std::string		strTempDir = "c:\\temp\\";	// TODO: Get Temp directory.
#else
		std::string		strTempDir = "/tmp/";		// TODO: Get Temp directory.
#endif
		std::string		strFileName = strTempDir + "FolderAdapter " + ptNow.FormatTimeA() + ".txt";
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
#endif // _DEBUG
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
//		CreateAdapter
// -----------------------------------------------------------------------------

CAdapter
CRemotePlugin::CreateAdapter()
{
	HeResult	hr = HE_S_OK;
	CAdapter	adapter;

	HE_TRY {
		// create a new RemoteAdapter.
		CHePtr<IAdapter> spRemoteAdapter;
		Peek::ThrowIfFailed( CHeCreator< CHeObject<CRemoteAdapter> >::CreateInstance(
			nullptr, IAdapter::GetIID(), reinterpret_cast<void**>( &spRemoteAdapter.p ) ) );

		// Propagate the site to the adapter.
		if ( m_spUnkSite.get() != nullptr ) {
			CHeQIPtr<IHeObjectWithSite>	spObjWithSite( spRemoteAdapter );
			if ( spObjWithSite.get() != nullptr ) {
				spObjWithSite->SetSite( m_spUnkSite.get() );
			}
		}

		adapter = CAdapter( spRemoteAdapter );
	}
	HE_CATCH( hr )
	(void) hr;

	return adapter;
}

CAdapter
CRemotePlugin::CreateAdapter(
	CAdapterInfo	inInfo )
{
	HeResult	hr = HE_E_FAIL;
	CAdapter	adapter;

	HE_TRY {
		if ( inInfo.IsValid() ) {
			adapter = CreateAdapter();
			if ( adapter.IsValid() ) {
				CAdapterInitialize	init( adapter );
				if ( init.Initialize( inInfo ) ) {
					hr = HE_S_OK;
				}
			}
		}
	}
	HE_CATCH( hr )
	(void) hr;

	return adapter;
}


// -----------------------------------------------------------------------------
//		DeleteAdapter
// -----------------------------------------------------------------------------

void
CRemotePlugin::DeleteAdapter(
	IAdapter*	inAdapter )
{
	{
		CHeQIPtr<IHeObjectWithSite>	spObjWithSite( inAdapter );
		if ( spObjWithSite.get() != nullptr ) {
			spObjWithSite->SetSite( nullptr );
		}
	}

 	CRemoteAdapter*	pRemoteAdapter = dynamic_cast<CRemoteAdapter*>( inAdapter );

 	delete pRemoteAdapter;
}


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
  #endif
}


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

	HeResult hr = HE_S_OK;
	try {
		IHeObjectWithSiteImpl<CRemotePlugin>::SetSite( inUnkSite );
		if ( inUnkSite != nullptr ) {
			DoLoadPlugin();
		}
		else {
			DoUnloadPlugin();
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

	HeResult	hr = HE_S_OK;
	try {
		SetContextId( inContextId );
		LoadContext();
		m_nState = kState_Initialized;
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

	HeResult	hr = HE_S_OK;
	try {
		ReleaseContext();

		CContextManagerPtr	spContextManager( GetContextManager() );
		_ASSERTE( spContextManager.IsValid() );
		if ( spContextManager.IsValid() && spContextManager->IsEmpty() ) {
			DoUnloadPlugin();
		}
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
		m_nState = kState_Uninitialized;	}
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
  #endif

	if ( HasNoPlugin() && HasContextId() ) {
		m_pPlugin = s_PluginController.Get();
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
				m_pPlugin->OnContextPrefs( *m_spdmPrefs );
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

	HeResult	hr = HE_S_OK;
	try {
		LoadContext();

		Peek::ThrowIf( !CPeekXml::Validate( ioDocument, ioNode ) );
		CPeekDataModeler pdmPrefs( ioDocument, ioNode, kModeler_Store );

		if ( HasContext() && m_spContext->CheckState( CPeekContext::kContextState_Created ) ) {
			m_spContext->OnContextPrefs( pdmPrefs );
		}
		else {
			m_pPlugin->OnContextPrefs( pdmPrefs );
		}
		pdmPrefs.End();
		hr = HE_S_OK;
	}
	HE_CATCH( hr );

	return hr;
}


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
			nResult = m_pPlugin->OnProcessPluginMessage( psPayload, psResponse );
		}

		if ( nResult == PEEK_PLUGIN_SUCCESS ) {
			outResponse = psResponse.GetIStreamPtr();
			hr = HE_S_OK;
		}
	}
	HE_CATCH( hr );

	return hr;
}


// ----------------------------------------------------------------------------
//		IPluginAdapter::DeleteAdapter
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::DeleteAdapter(
	const Helium::HeID&	adapterID )
{
	ObjectLock lock(this);

	CGlobalId	id( adapterID );
	int	nResult = m_pPlugin->OnDeleteAdapter( id );

	return nResult;
}


// -----------------------------------------------------------------------------
//		IPluginUISource::GetHandlerID
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::GetHandlerID(
	Helium::HeID*	pHandlerID )
{
#ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Get Options Handler ID" );
#endif

	_ASSERTE( pHandlerID != nullptr );
	if ( pHandlerID == nullptr ) return HE_E_INVALID_ARG;

// 	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		CGlobalId	id;
		int	nResult = m_pPlugin->OnGetOptionsHandlerId( id );
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
	return HE_S_OK;
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
#endif

	_ASSERTE( pFlags != nullptr );
	_ASSERTE( pOutData != nullptr );
	if ( (pFlags == nullptr) || (pOutData == nullptr) ) return HE_E_INVALID_ARG;

	_ASSERTE( HasPlugin() );
	if ( HasNoInitializedPlugin() ) return HE_S_OK;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		int			nResult = PEEK_PLUGIN_SUCCESS;
		UInt32	nFlags( 0 );
		CPeekStream	psData( pOutData );
		switch ( nType ) {
			case peekPluginHandlerUI_Options:
				nResult = m_pPlugin->OnGetOptionsData( &nFlags, psData );
				break;
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
		if ( (m_spdmPrefs.get() != nullptr) && m_spdmPrefs->IsOpen() ) {
			m_spContext->OnContextPrefs( *m_spdmPrefs );
			m_spdmPrefs->End();
		}
	}
}


// ----------------------------------------------------------------------------
//		DoDisposeContext
// ----------------------------------------------------------------------------

void
CRemotePlugin::DoDisposeContext()
{
	if ( HasPlugin() ) {
		m_spContext->PreDispose( m_pPlugin );

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

	if ( HasInitializedPlugin() ) {
		m_pPlugin->AddRemotePlugin();
		m_nPluginAdded++;
		return true;
	}

	if ( !HasSite() ) return false;

	if ( HasNoPlugin() ) {
		s_PluginController.Add( this );
		m_pPlugin = s_PluginController.Get();
	}
	_ASSERTE( HasPlugin() );
	if ( HasNoPlugin() ) {
		Peek::Throw( HE_E_FAIL );
	}

	m_pPlugin->AddRemotePlugin();
	m_nPluginAdded++;

  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Call Plugin::OnLoad" );
  #endif
	int nReturn = m_pPlugin->OnLoad( this );
	if ( nReturn != PEEK_PLUGIN_SUCCESS ) {
		Peek::Throw( HE_E_FAIL );
	}

	m_pPlugin->OnReadPrefs();

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

	if ( m_nPluginAdded > 0 ) {
		m_nPluginAdded--;
		if ( m_pPlugin->RemoveRemotePlugin() == 0 ) {
			m_pPlugin->OnWritePrefs();
			m_pPlugin->OnUnload( this );
		}
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
		if ( !HasSite() ) return HE_S_OK;
		if ( !HasContextId() ) return HE_S_OK;

		CPeekContextPtr		spContext;
		CContextManagerPtr	spContextManager( GetContextManager() );
		_ASSERTE( spContextManager.IsValid() );
		if ( spContextManager.IsNotValid() ) Peek::Throw( HE_E_FAIL );

		if ( !spContextManager->Find( m_idContext, spContext ) ) {
			_ASSERTE( spContext.IsNotValid() );
			CPeekEngineContext*		pEngineContext = new COmniEngineContext( m_idContext, this );
			_ASSERTE( pEngineContext );
			if ( !pEngineContext ) Peek::Throw( HE_E_FAIL );
			_ASSERTE( m_spUnkSite );
			pEngineContext->SetInterfacePointers( m_spUnkSite.get(), m_pPlugin );
			spContext = CPeekContextPtr( pEngineContext );
			spContextManager->Add( spContext );
		}
		else {
			_ASSERTE( spContext.IsValid() );
			CPeekEngineContext*		pEngineContext = dynamic_cast<CPeekEngineContext*>( spContext.get() );
			_ASSERTE( pEngineContext );
			if ( !pEngineContext ) Peek::Throw( HE_E_FAIL );
			pEngineContext->SetInterfacePointers( m_spUnkSite.get(), m_pPlugin );
		}

		m_spContext = spContext;
		_ASSERTE( HasContext() );
		if ( !HasContext() ) return HE_E_FAIL;

		if ( m_spContext.IsValid() && m_spContext->HasGraphs() &&
				(m_spContext->GetGraphsPlugin() == m_pPlugin) ) {
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
	if ( HasNoInitializedPlugin() ) return false;

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
