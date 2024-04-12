// =============================================================================
//	OmniEngineContext.cpp
// =============================================================================
//	Copyright (c) 2011-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniEngineContext.h"
#include "RemotePlugin.h"
#include "MemUtil.h"
#include "Packet.h"
#include "PacketArray.h"
#include "PluginMessages.h"

#include "PeekMessage.h"
#include "PeekFile.h"
#include "PeekPacket.h"
#include "PeekTime.h"
//#include "TimeConv.h"
#include "FileEx.h"
#include "Version.h"

class CRemotePlugin;


// =============================================================================
//		COmniEngineContext
// =============================================================================

COmniEngineContext::COmniEngineContext(
	CGlobalId&		inId,
	CRemotePlugin*	inRemotePlugin )
	:	CPeekEngineContext( inId, inRemotePlugin )
{
	(void)inRemotePlugin;

	PeekLock lock( m_Mutex );
}

COmniEngineContext::COmniEngineContext(
	CGlobalId&			inId,
	CRemoteStatsPlugin*	inRemotePlugin )
	:	CPeekEngineContext( inId, inRemotePlugin )
{
	(void)inRemotePlugin;

	PeekLock lock( m_Mutex );
}

COmniEngineContext::~COmniEngineContext()
{
}


// -----------------------------------------------------------------------------
//		OnCreateContext
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnCreateContext()
{
	PeekLock lock( m_Mutex );

	int	nResult = CPeekEngineContext::OnCreateContext();
	// IsGlobalContext() return true if this is NOT a capture or file context.

	CAdapter	adapter = m_PeekContextProxy.GetAdapter();
	if ( adapter.IsValid() ) {
		m_StatsHandler = adapter.GetSummaryStatsHandler();
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnDisposeContext
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnDisposeContext()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	PeekLock lock( m_Mutex );

	nResult = CPeekEngineContext::OnDisposeContext();

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnProcessPluginMessage
//		: Implements IProcessPluginMessage::ProcessPluginMessage
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnProcessPluginMessage(
	CPeekStream& inMessage,
	CPeekStream& outResponse )
{
	int	nResult = PEEK_PLUGIN_FAILURE;

	// Create and Load the appropriate Message object.
	std::unique_ptr<CPeekMessage>	spMessage = MessageFactory( inMessage );
	if ( spMessage.get() != nullptr ) {
		switch ( spMessage->GetType() ) {
			case CMessageGetAdapter::s_nMessageType:
				nResult = ProcessGetAdapterOptions( spMessage.get() );
				break;
		}

		if ( nResult == PEEK_PLUGIN_SUCCESS ) {
			spMessage->StoreResponse( outResponse );
		}
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnReset
//		: Implements IResetProcessing::ResetProcessing
//
//	The Capture Context has been reset. The Capture Context is reset before
//	capturing begins.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnReset()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	PeekLock lock( m_Mutex );

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnSummary
//		: Implements IUpdateSummaryStats::UpdateSummaryStats
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnSummary(
	CSnapshot&	inSnapshot )
{
	if ( m_StatsHandler.IsValid() ) {
		m_StatsHandler.UpdateSummaryStats( inSnapshot );
	}

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnContextPrefs
//
//	Preferences (settings) specific to this Capture Context are being loaded or
//	stored into this instance of this plugin.
//	The Peek Data Modeler handles both loading and storing via the Model method.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnContextPrefs(
	CPeekDataModeler& ioPrefs )
{
	PeekLock lock( m_Mutex );

	CPeekContext::OnContextPrefs( ioPrefs );

	m_Options.Model( ioPrefs );

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		ProcessGetAdapterOptions
// -----------------------------------------------------------------------------

int
COmniEngineContext::ProcessGetAdapterOptions(
	CPeekMessage* ioMessage )
{
	CMessageGetAdapter*	pMessage( dynamic_cast<CMessageGetAdapter*>( ioMessage ) );
	_ASSERTE( pMessage != nullptr );
	if ( pMessage == nullptr ) return PEEK_PLUGIN_FAILURE;

	// Create the response.
	if ( !pMessage->StartResponse() ) return PEEK_PLUGIN_FAILURE;

	CAdapter	adapter = m_PeekContextProxy.GetAdapter();
	if ( adapter.IsValid() ) {
		CAdapterInfo	adptInfo = adapter.GetAdapterInfo();
		CGlobalId		adptId = adptInfo.GetId();
		const COptions&	opt = m_pPlugin->GetOptions();
		CAdapterOptions	optAdapter;
		if ( opt.Find( adptId, optAdapter ) ) {
			pMessage->SetAdapterOptions( optAdapter );
		}
	}

	return PEEK_PLUGIN_SUCCESS;
}
