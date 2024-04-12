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
#include "PluginMessages.h"

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

//const int	MIN_UDPHDR_LEN( 8 );
//const int	MIN_TCPHDR_LEN( 20 );
const int	MIN_ICMPHDR_LEN( 8 );
const int	MIN_ICMPv6HDR_LEN( 10 );
//const int	MIN_DNSHDR_LEN( 10 );

int
COmniEngineContext::OnFilter(
	CPacket&	inPacket,
	UInt16*		outBytesToAccept,
	CGlobalId&	/*outFilterId*/ )
{
	if ( inPacket.IsError() ) return PEEK_PLUGIN_FAILURE;

	CLayerIP	layerIP;
	if ( !inPacket.GetLayer( layerIP ) ) return PEEK_PLUGIN_FAILURE;
	if ( layerIP.IsFragment() ) return PEEK_PLUGIN_FAILURE;

	SInt16 nSliceLength = layerIP.GetOffset() + layerIP.GetHeaderLength();

	switch ( layerIP.GetPayloadProtocol() ) {
	case kProtocol_ICMP:
		nSliceLength += MIN_ICMPHDR_LEN;
		break;

	case kProtocol_ICMPv6:
		nSliceLength += MIN_ICMPv6HDR_LEN;
		break;

/* petertest - fix the case for 802.5 / token ring?
	UInt16	nIPOffset( pHdrData - ioParams->fPacketData );
	if ( nIPOffset > sizeof( tEthernetHeader ) ) {
		// Packet is 802.3/802.5.
		nOffset = static_cast<int>( nIPOffset );

		if ( ioParams->fMediaType == kPluginPacketMediaType_802_5 ) {
			// Skip over Token Ring source routing info.
			nOffset += GetPlugin()->GetSourceRouteInfoLength( ioParams->fPacket, ioParams->fPacketData );
		}
	}
*/
	case kProtocol_TCP:
		{
			CLayerTCP layerTCP;
			if ( !inPacket.GetLayer( layerTCP ) ) return PEEK_PLUGIN_FAILURE;
			nSliceLength += layerTCP.GetHeaderLength();
			break;
		}

	case kProtocol_UDP:
		{
			CLayerUDP layerUDP;
			if ( !inPacket.GetLayer( layerUDP ) ) return PEEK_PLUGIN_FAILURE;
			nSliceLength += layerUDP.GetHeaderLength();
			break;
		}
		break;

	default:  // IP (Other)
		break;
	}

	if ( outBytesToAccept != NULL ) *outBytesToAccept = nSliceLength;

	return PEEK_PLUGIN_SUCCESS;
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


#ifdef IMPLEMENT_PLUGINMESSAGE
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

	return nResult;
}
#endif // IMPLEMENT_PLUGINMESSAGE


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

	return PEEK_PLUGIN_SUCCESS;
}


#ifdef IMPLEMENT_PLUGINMESSAGE
#endif // IMPLEMENT_PLUGINMESSAGE
