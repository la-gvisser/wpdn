// =============================================================================
//	RemoteStatsPlugin.cpp
// =============================================================================
//	Copyright (c) 2008-2017 Savvius, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef IMPLEMENT_SUMMARY

#include "resource.h"
#include "RemoteStatsPlugin.h"
#include "RemoteAdapter.h"
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

using namespace Helium;
using namespace HeLib;
using std::vector;

#if defined(_DEBUG) && defined(_WIN32)
static bool		s_bDebugEngine = true;
#endif // _DEBUG && _WIN32

// {6C650BC2-B06D-48BC-98B2-608AD9FC9BCF}
GUID			g_StatsPluginId = 
	{ 0x6C650BC2, 0xB06D, 0x48BC, { 0x98, 0xB2, 0x60, 0x8A, 0xD9, 0xFC, 0x9B, 0xCF } };

CPeekString		g_strHspAdapterStats( L"HspAdapter" );

extern Helium::HeCID		g_StatsClassId;

extern CContextManagerPtr	GetContextManager();
extern CPluginController&	GetPluginController();

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define FMT_HEX04	std::hex << std::uppercase << std::setfill( '0' ) << std::setw( 4 )
#define FMT_HEX08	std::hex << std::uppercase << std::setfill( '0' ) << std::setw( 8 )


// =============================================================================
//		CRemoteStatsPlugin
// =============================================================================

CRemoteStatsPlugin::CRemoteStatsPlugin()
	:	m_pPlugin( nullptr )
#ifdef _WIN32
	,	m_hInstance( nullptr )
#endif
	,	m_bGraphsContext( false )
	,	m_pfnSummary( nullptr )
{
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Constructed" );
  #endif
}

CRemoteStatsPlugin::~CRemoteStatsPlugin()
{
	ReleaseContext();

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
CRemoteStatsPlugin::AddLogMsg(
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

	CRemoteStatsPlugin*		pPlugin = this;;
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
CRemoteStatsPlugin::SaveMsgLog()
{
	AddLogMsg( "Close" );
}
#endif


// -----------------------------------------------------------------------------
//		FinalConstruct
// -----------------------------------------------------------------------------

HeResult
CRemoteStatsPlugin::FinalConstruct()
{
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Engine Instance" );
  #endif // DEBUG_LOGINTERFACE

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		FinalRelease
// -----------------------------------------------------------------------------

void
CRemoteStatsPlugin::FinalRelease()
{
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Final Release" );
  #endif
}


// -----------------------------------------------------------------------------
//		IPeekPlugin::GetName
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteStatsPlugin::GetName(
	Helium::HEBSTR*	outName )
{
	_ASSERTE( outName != nullptr );
	if ( outName == nullptr ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_E_FAIL;

	try {
		HeLib::HeHEBSTRFromString( g_strHspAdapterStats, outName );
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
CRemoteStatsPlugin::GetClassID(
	Helium::HeCID*	outClassId )
{
	HeResult	hr = HE_E_FAIL;

	try {
		_ASSERTE( outClassId != nullptr );
		if ( outClassId == nullptr ) return HE_E_NULL_POINTER;

		*outClassId = g_StatsClassId;

		hr = HE_S_OK;
	}
	HE_CATCH( hr );

	return hr;
}


// ----------------------------------------------------------------------------
//		IObjectWithSiteImpl::SetSite
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteStatsPlugin::SetSite(
	IHeUnknown *inUnkSite )
{
  #ifdef DEBUG_LOGINTERFACE
	std::ostringstream	strmMsg;
	strmMsg << "SetSite : Site = " << inUnkSite;
	AddLogMsg( strmMsg );
  #endif

	HeResult hr = HE_S_OK;
	try {
		IHeObjectWithSiteImpl<CRemoteStatsPlugin>::SetSite( inUnkSite );
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		IPeekPluginLoad::Load
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteStatsPlugin::Load(
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
		if ( !inContextId.IsNull() ) {
			DoLoadPlugin();
			SetContextId( inContextId );
			LoadContext();
		}
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		IPeekPluginLoad::Unload
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteStatsPlugin::Unload()
{
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Unload" );
  #endif

	HeResult	hr = HE_S_OK;
	try {
		ReleaseContext();

		CContextManagerPtr	spContextManager( GetContextManager() );
		_ASSERTE( spContextManager.IsValid() );
		// Not seeing Graphs plugins being deleted...
		if ( spContextManager.IsValid() ) {
			CPeekContextPtr		spContext;
			if ( spContextManager->Find( m_idContext, spContext ) ) {
				CPeekEngineContext*	pEngineContext =
					dynamic_cast<CPeekEngineContext*>( static_cast<CPeekContext*>( m_spContext ) );
				if ( pEngineContext != nullptr ) {
					CPeekPlugin*	pGraphsPlugin = pEngineContext->GetGraphsPlugin();
					if ( pGraphsPlugin && (pGraphsPlugin->GetRemoteStatsPlugin() != this)  ) {
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


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		IProcessPluginMessage::ProcessPluginMessage
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteStatsPlugin::ProcessPluginMessage(
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

		_ASSERTE( inId == g_StatsClassId );
		_ASSERTE( hdrReqPluginMessage.PluginID == g_StatsClassId );
		if ( (inId != g_StatsClassId) || (hdrReqPluginMessage.PluginID != g_StatsClassId) ) {
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
#endif	// IMPLEMENT_PLUGINMESSAGE


// -----------------------------------------------------------------------------
//		IUpdateSummaryStats::UpdateSummaryStats
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteStatsPlugin::UpdateSummaryStats(
	IHeUnknown*	pSnapshot )
{
	_ASSERTE( pSnapshot != nullptr );
	if ( pSnapshot == nullptr ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_E_FAIL;
	try {
		CContextManagerPtr pContextManager( GetContextManager() );
		if ( pContextManager ) {
			CPeekContextPtr	spContext;
			if ( pContextManager->Find( m_idContext, spContext ) ) {
				CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( spContext.get() );
				if ( pEngineContext && m_pfnSummary ) {
					int			nResult = PEEK_PLUGIN_FAILURE;

					HeLib::CHePtr<ISnapshotContext> spSnapshotContext;
					if ( HE_SUCCEEDED( spSnapshotContext.CreateInstance( CSnapshotContext_CID ) ) ) {
						spSnapshotContext->Initialize( pSnapshot, pEngineContext );
						nResult = ((*pEngineContext).*m_pfnSummary)( spSnapshotContext );
					}
					else {
						nResult = ((*pEngineContext).*m_pfnSummary)( pSnapshot );
					}

					if ( nResult == PEEK_PLUGIN_SUCCESS ) {
						hr = HE_S_OK;
					}
				}
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		IPluginUISource::GetHandlerID
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteStatsPlugin::GetHandlerID(
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
		if ( m_pPlugin ) {
			CGlobalId	id;
			int	nResult = m_pPlugin->OnGetOptionsHandlerId( id );
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				*pHandlerID = id;
				hr = HE_S_OK;
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		IPluginUISource::SetHandlerID
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteStatsPlugin::SetHandlerID(
	const Helium::HeID* pHandlerID )
{
	(void) pHandlerID;
	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IPluginUISource::GetData
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteStatsPlugin::GetData(
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
			case peekPluginHandlerUI_CaptureTab:
				nResult = m_pPlugin->OnGetCaptureTabData( &nFlags, psData );
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
CRemoteStatsPlugin::DoCreateContext()
{
	if ( !IsValidContext() ) return;

	if ( m_spContext->EqualState( CPeekContext::kContextState_Initialized ) ) {
		int nReturn = m_spContext->OnCreateContext();
		if ( nReturn != PEEK_PLUGIN_SUCCESS ) {
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

	CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( m_spContext.get() );
	if ( pEngineContext ) {
		m_pfnSummary = &CPeekEngineContext::OnSummary;
	}
}


// ----------------------------------------------------------------------------
//		DoDisposeContext
// ----------------------------------------------------------------------------

void
CRemoteStatsPlugin::DoDisposeContext()
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
CRemoteStatsPlugin::DoLoadPlugin()
{
  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "DoLoadPlugin" );
  #endif
	if ( HasInitializedPlugin() ) return true;

	if ( !HasSite() ) return false;

	if ( HasNoPlugin() ) {
		CPluginController&	PluginController( GetPluginController() );
		PluginController.Add( this );
		m_pPlugin = PluginController.GetPlugin();
	}
	_ASSERTE( HasPlugin() );
	if ( HasNoPlugin() ) {
		Peek::Throw( HE_E_FAIL );
	}

  #ifdef DEBUG_LOGINTERFACE
	AddLogMsg( "Call Plugin::OnLoad" );
  #endif

#if (0)
	int nReturn = m_pPlugin->OnLoad( this );
	if ( nReturn != PEEK_PLUGIN_SUCCESS ) {
		Peek::Throw( HE_E_FAIL );
	}

	m_pPlugin->OnReadPrefs();
#endif

	return true;
}


// ----------------------------------------------------------------------------
//		DoUnloadPlugin
// ----------------------------------------------------------------------------

void
CRemoteStatsPlugin::DoUnloadPlugin()
{
	if ( HasInitializedPlugin() ) {
		m_pPlugin->OnUnload();
	}
}


// ----------------------------------------------------------------------------
//		CreateContext
// ----------------------------------------------------------------------------

HeResult
CRemoteStatsPlugin::CreateContext()
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
			pEngineContext->SetInterfacePointers( m_spUnkSite, m_pPlugin );
			spContext = CPeekContextPtr( pEngineContext );
			spContextManager->Add( spContext );
		}
		else {
			_ASSERTE( spContext.IsValid() );
			CPeekEngineContext*		pEngineContext = dynamic_cast<CPeekEngineContext*>( spContext.get() );
			_ASSERTE( pEngineContext );
			if ( !pEngineContext ) Peek::Throw( HE_E_FAIL );
			pEngineContext->SetInterfacePointers( m_spUnkSite, m_pPlugin );
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
CRemoteStatsPlugin::LoadContext()
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
CRemoteStatsPlugin::ReleaseContext()
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

#endif // IMPLEMENT_SUMMARY
