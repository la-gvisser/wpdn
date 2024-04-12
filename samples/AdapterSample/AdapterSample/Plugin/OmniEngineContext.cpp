// =============================================================================
//	OmniEngineContext.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniEngineContext.h"
#include "RemotePlugin.h"
#include "MemUtil.h"
#include "Packet.h"
#include "PacketArray.h"
#ifdef IMPLEMENT_PLUGINMESSAGE
  #include "PluginMessages.h"
#endif // IMPLEMENT_PLUGINMESSAGE

class CRemotePlugin;

#ifdef IMPLEMENT_SUMMARYMODIFYENTRY
static CPeekString	s_strPacketsProcessed( L"Packets Processed" );
#endif // IMPLEMENT_SUMMARYMODIFYENTRY



// =============================================================================
//		COmniEngineContext
// =============================================================================

COmniEngineContext::COmniEngineContext(
	CGlobalId&		inId,
	CRemotePlugin*	inRemotePlugin )
	:	CPeekEngineContext( inId, inRemotePlugin )
{
	inRemotePlugin;
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
	int	nResult = CPeekEngineContext::OnCreateContext();
	// IsGlobalContext() return true if this is NOT a capture or file context.

#ifdef IMPLEMENT_UPDATESUMMARYSTATS
	ClearSummaryStats();
#endif

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnDisposeContext
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnDisposeContext()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	nResult = CPeekEngineContext::OnDisposeContext();

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnFilter
//		: Implements IFilterPacket::FilterPacket
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnFilter(
	CPacket&	inPacket,
	UInt16*		outBytesToAccept,
	CGlobalId&	outFilterId )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	inPacket;
	outBytesToAccept;
	outFilterId;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnNotify
//		: Implements INotify::Notify
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnNotify(
	 const CGlobalId&	inContextId,
	 const CGlobalId&	inSourceId,
	 UInt64				inTimeStamp,
	 PeekSeverity		inSeverity,
	 const CPeekString& inShortMessage,
	 const CPeekString& inLongMessage )
{
	inContextId;
	inSourceId;
	inTimeStamp;
	inSeverity;
	inShortMessage;
	inLongMessage;

	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnProcessPacket
//		: Implements IProcessPacket::ProcessPacket
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnProcessPacket(
	CPacket&	inPacket )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	inPacket;

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
	int	nResult = PEEK_PLUGIN_SUCCESS;

	inMessage;
	outResponse;

#ifdef IMPLEMENT_PLUGINMESSAGE
	// Create and Load the appropriate Message object.
	CPeekMessagePtr	spMessage = MessageFactory( inMessage );
	if ( spMessage.get() != NULL ) {
		switch ( spMessage->GetType() ) {
			case CMessageGetOptions::s_nMessageType:
				nResult = ProcessGetOptions( spMessage.get() );
				break;

		default:
			nResult = PEEK_PLUGIN_FAILURE;
		}
	}

	if ( nResult == PEEK_PLUGIN_SUCCESS ) {
		spMessage->StoreResponse( outResponse );
	}
#endif // IMPLEMENT_PLUGINMESSAGE

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnProcessTime
//		: Implements IProcessTime::ProcessTime
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnProcessTime(
	UInt64	inCurrentTime )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	inCurrentTime;

#ifdef IMPLEMENT_UPDATESUMMARYSTATS
	ClearSummaryStats();
#endif

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnReset
//		: Implements IResetProcessing::ResetProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnReset()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

#ifdef IMPLEMENT_UPDATESUMMARYSTATS
	ClearSummaryStats();
#endif

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnResetCapture
//		: Implements IPacketProcessorEvents::OnResetPacketProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnResetCapture()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnStartCapture
//		: Implements IPacketProcessorEvents::OnStartPacketProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnStartCapture(
	UInt64	inStartTime )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	inStartTime;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnStopCapture
//		: Implements IPacketProcessorEvents::OnStopPacketProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnStopCapture(
	UInt64	inStopTime )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	inStopTime;

	return nResult;
}


#ifdef IMPLEMENT_ALL_UPDATESUMMARY
// -----------------------------------------------------------------------------
//		OnSummary
//		: Implements IUpdateSummaryStats::UpdateSummaryStats
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnSummary(
#ifdef IMPLEMENT_UPDATESUMMARYSTATS
	CSnapshot&	inSnapshot )
#else
	)
#endif
{
	int	nResult	= PEEK_PLUGIN_SUCCESS;

#ifdef IMPLEMENT_UPDATESUMMARYSTATS
	inSnapshot;
#endif

	return nResult;
}
#endif // IMPLEMENT_ALL_UPDATESUMMARY


#ifdef IMPLEMENT_UPDATESUMMARYSTATS
// -----------------------------------------------------------------------------
//		ClearSummaryStats
// -----------------------------------------------------------------------------

void
COmniEngineContext::ClearSummaryStats()
{
	memset( &m_Stats, 0, sizeof(m_Stats) );
}
#endif // IMPLEMENT_UPDATESUMMARYSTATS


// -----------------------------------------------------------------------------
//		OnContextPrefs
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnContextPrefs(
	CPeekDataModeler& ioPrefs )
{
	CPeekContext::OnContextPrefs( ioPrefs );

	m_Options.Model( ioPrefs );

	if ( ioPrefs.IsLoading() ) {
		m_pPlugin->SetOptions( ioPrefs );
	}

	return PEEK_PLUGIN_SUCCESS;
}


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		ProcessGetOptions
// -----------------------------------------------------------------------------

int
COmniEngineContext::ProcessGetOptions(
	CPeekMessage* ioMessage )
{
	CMessageGetOptions*	pMessage = dynamic_cast<CMessageGetOptions*>( ioMessage );
	ASSERT( pMessage != NULL );
	if ( pMessage == NULL ) return PEEK_PLUGIN_FAILURE;

	// Create the modeler.
	if ( !pMessage->StartResponse() ) {
		return PEEK_PLUGIN_FAILURE;
	}

	// Set the response (output).
	pMessage->SetOptions( GetOptions() );

	return PEEK_PLUGIN_SUCCESS;
}
#endif
