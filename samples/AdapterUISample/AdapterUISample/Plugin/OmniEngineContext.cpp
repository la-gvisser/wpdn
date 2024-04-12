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

#ifdef IMPLEMENT_UPDATESUMMARYSTATS
	ClearSummaryStats();
#endif // IMPLEMENT_UPDATESUMMARYSTATS

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnCaptureStarting											  [Omni 7.0]
//
//  The capture context will begin setup for capture.
//	The engine has entered its StartCapture() fuction.
//	Initialize plugin specific resources, but Capture Context specific objects
//	have not been created yet.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnCaptureStarting()
{
	PeekLock lock( m_Mutex );

	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnCaptureStarted											  [Omni 7.0]
//
//  The capture context is capturing.
//	The engine is about to exit its StartCapture() fuction.
//	The Capture Context is now capturing, all Capture Context specific objects
//	have been created and initialized.
//	The plugin may now Insert Packets into the Capture Context.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnCaptureStarted()
{
	PeekLock lock( m_Mutex );

	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnCaptureStopping											  [Omni 7.0]
//
//	The capture context will start to shutdown capturing.	
//	The engine has entered its StopCapture() fuction.
//	All Capture Context specific objects are still initialized and functional.
//	The plugin may still Insert Packets into the Capture Context.
//	The StopCapture() function will not continue until all plugins have
//	returned from this function.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnCaptureStopping()
{
	PeekLock lock( m_Mutex );

	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnCaptureStopped											  [Omni 7.0]
//
//	The capture context has torn down the capture.
//	The engine is about to exit its StopCapture() fuction.
//	All Capture Context specific objects have been destroyed.
//	The plugin may not Insert Packets into the Capture Context.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnCaptureStopped()
{
	PeekLock lock( m_Mutex );

	int	nResult = PEEK_PLUGIN_SUCCESS;

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
//		OnFilter
//		: Implements IFilterPacket::FilterPacket
//
//	This method is called when an Advanced Filter contains an Analysis Module
//	node that set to 'this' type of plugin.
//
//	To accept the packet into the Capture Buffer return PLUGIN_PACKET_ACCEPT.
//	To reject the packet return PEEK_PLUGIN_REJECT (or any non-zero value).
//	To Slice the packet set OutBytesToAccept to the number of bytes
//	the plugin will accept of the packet.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnFilter(
	CPacket&	inPacket,
	UInt16*		outBytesToAccept,
	CGlobalId&	outFilterId )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	PeekLock lock( m_Mutex );

	(void)inPacket;
	(void)outBytesToAccept;
	(void)outFilterId;

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
	int	nResult = PEEK_PLUGIN_SUCCESS;

	(void)inContextId;
	(void)inSourceId;
	(void)inTimeStamp;
	(void)inSeverity;
	(void)inShortMessage;
	(void)inLongMessage;

	PeekLock lock( m_Mutex );

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnProcessPacket
//		: Implements IProcessPacket::ProcessPacket
//
//	The Packet has been inserted into the Capture Buffer.
//	Perform any analysis of the packet at this time.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnProcessPacket(
	CPacket&	inPacket )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	(void)inPacket;

	PeekLock lock( m_Mutex );

	return nResult;
}


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		OnProcessPluginMessage
//		: Implements IProcessPluginMessage::ProcessPluginMessage
//
//	An instance of 'this' plugin in OmniPeek has sent this instance of the
//	plugin on OmniEngine a message. The contents and responses are private to
//	this plugin.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnProcessPluginMessage(
	CPeekStream& inMessage,
	CPeekStream& outResponse )
{
	PeekLock lock( m_Mutex );

	int	nResult = PEEK_PLUGIN_SUCCESS;

	(void)inMessage;
	(void)outResponse;

	return nResult;
}
#endif // IMPLEMENT_PLUGINMESSAGE


// -----------------------------------------------------------------------------
//		OnProcessTime
//		: Implements IProcessTime::ProcessTime
//
//	While the Capture Context is capturing, this method is called once a
//	second.
//	When a capture file is being processed, this method is called at one second
//	intervals based on the timestamps of the packets.
//	If the duration between timestamps is excessive, then this method may be
//	called multiple times between packets. And the difference between time
//	stamps bay be longer than one second.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnProcessTime(
	UInt64	inCurrentTime )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	(void)inCurrentTime;

	PeekLock lock( m_Mutex );

#ifdef IMPLEMENT_UPDATESUMMARYSTATS
	ClearSummaryStats();
#endif

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

#ifdef IMPLEMENT_UPDATESUMMARYSTATS
	ClearSummaryStats();
#endif

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnResetCapture
//		: Implements IPacketProcessorEvents::OnResetPacketProcessing
//
//	The Capture Context has processed 2^64 packets and the  packet index is
//	about to rollover.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnResetCapture()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	PeekLock lock( m_Mutex );

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnStartCapture
//		: Implements IPacketProcessorEvents::OnStartPacketProcessing
//
//	The Capture Context has been setup to capture.
//	The plugin may Insert Packets.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnStartCapture(
	UInt64	inStartTime )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	(void)inStartTime;

	PeekLock lock( m_Mutex );

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnStopCapture
//		: Implements IPacketProcessorEvents::OnStopPacketProcessing
//
//	The Capture Context has stopped capturing.
//	Most of the Capture Context specific objects have been reset or destroyed.
//	The plugin may not Insert Packets into the Capture Context.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnStopCapture(
	UInt64	inStopTime )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	(void)inStopTime;

	PeekLock lock( m_Mutex );

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
	(void)inSnapshot;
#endif

	PeekLock lock( m_Mutex );

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
