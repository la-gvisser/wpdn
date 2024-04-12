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

#define FMT_HEX04	std::hex << std::uppercase << std::setfill( '0' ) << std::setw( 4 )
#define FMT_HEX08	std::hex << std::uppercase << std::setfill( '0' ) << std::setw( 8 )


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
#ifdef _WIN32
	,	m_hInstance( nullptr )
#endif
	,	m_bGraphsContext( false )
{
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Constructed" );
  #endif
}

CRemotePlugin::~CRemotePlugin()
{
	if ( HasSite() ) {
		DoUnloadPlugin();
	}

  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Destroyed" );

	CContextManagerPtr	spContextManager( GetContextManager() );
	if ( spContextManager.IsValid() ) {
		if ( spContextManager.IsValid()->IsEmpty() ) {
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

#if defined(_DEBUG) && defined(_WIN32)
	static std::filebuf	fb;
	if ( !fb.is_open() ) {
		CPeekTimeLocal	ptNow( true );
		std::string		strTempDir = "c:\\temp\\";	// TODO: Get Temp directory.
		std::string		strFileName = strTempDir + "HspAdapter " + ptNow.FormatTimeA() + ".txt";
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
#endif


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
		if ( m_spUnkSite != nullptr ) {
			CHeQIPtr<IHeObjectWithSite>	spObjWithSite( spRemoteAdapter );
			if ( spObjWithSite != nullptr ) {
				spObjWithSite->SetSite( m_spUnkSite );
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
		if ( spObjWithSite != nullptr ) {
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
		if ( inUnkSite == nullptr ) {
			DoUnloadPlugin();
		}
		IHeObjectWithSiteImpl<CRemotePlugin>::SetSite( inUnkSite );
		if ( inUnkSite != nullptr ) {
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

	HeResult	hr = HE_S_OK;
	try {
		SetContextId( inContextId );
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
  #endif

	if ( HasNoPlugin() && HasContextId() ) {
		m_pPlugin = s_PluginController.GetPlugin();
	}

	HeResult	hr = HE_S_OK;
	try {
		Peek::ThrowIf( !CPeekXml::Validate( inDocument, inNode ) );
		m_spdmPrefs = std::unique_ptr<CPeekDataModeler>(
			new CPeekDataModeler( inDocument, inNode, kModeler_Load ) );
		if ( m_spdmPrefs.get() != nullptr ) {
			_ASSERTE( m_spdmPrefs->IsOpen() );
			m_pPlugin->OnContextPrefs( *m_spdmPrefs );
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
		Peek::ThrowIf( !CPeekXml::Validate( ioDocument, ioNode ) );
		CPeekDataModeler pdmPrefs( ioDocument, ioNode, kModeler_Store );

		m_pPlugin->OnContextPrefs( pdmPrefs );
		pdmPrefs.End();
		hr = HE_S_OK;
	}
	HE_CATCH( hr );

	return hr;
}


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

		UInt32		cbMsgLen = hdrReqPluginMessage.MessageSize;
		int			nResult = PEEK_PLUGIN_FAILURE;
		CPeekStream	psPayload;
		if ( !psMessage.CopyTo( psPayload, cbMsgLen ) ) Peek::Throw( E_FAIL );
		psPayload.Rewind();

		if ( HasPlugin() ) {
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
#endif	// IMPLEMENT_PLUGINMESSAGE


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


#ifdef IMPLEMENT_UIHANDLER
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
#endif	// IMPLEMENT_UIHANDLER


// ============================================================================
// Internal functions
// ============================================================================

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

	if ( !HasSite() ) return false;

	if ( HasNoPlugin() ) {
		s_PluginController.Add( this );
		m_pPlugin = s_PluginController.GetPlugin();
	}
	_ASSERTE( HasPlugin() );
	if ( HasNoPlugin() ) {
		Peek::Throw( HE_E_FAIL );
	}

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

	if ( s_PluginController.RemoveOwner( this ) == 0 ) {
		m_pPlugin->OnWritePrefs();
		m_pPlugin->OnUnload();
	}
}
