// =============================================================================
//	OmniEngineContext.cpp
// =============================================================================
//	Copyright (c) 2011-2017 Savvius, Inc. All rights reserved.

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
[!if NOTIFY_CODEEXAMPLES]

////////////////////////////////////////////////////////////////////////
// Code Examples
// Caution: defining either of the following symbols in the .h file
// will cause writes to disk on the OmniEngine computer:
//   EXAMPLE_DECODE
//   EXAMPLE_FILECAPTUREBUFFER
//
////////////////////////////////////////////////////////////////////////
[!endif]


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

[!if NOTIFY_CODEEXAMPLES]

#ifdef IMPLEMENT_CODEEXAMPLES
	ExampleInitialize();
#endif // IMPLEMENT_CODEEXAMPLES
[!endif]
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

[!if NOTIFY_CODEEXAMPLES]
#ifdef IMPLEMENT_CODEEXAMPLES
	ExampleCreateContext();
#ifdef EXAMPLE_DATATABLE
	DataTableExample();
#endif // EXAMPLE_DATATABLE
#endif // IMPLEMENT_CODEEXAMPLES

[!endif]
	return nResult;
}


#ifdef IMPLEMENT_PACKETPROCESSOREVENTS
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
#endif	// IMPLEMENT_PACKETPROCESSOREVENTS


// -----------------------------------------------------------------------------
//		OnDisposeContext
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnDisposeContext()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	PeekLock lock( m_Mutex );

[!if NOTIFY_CODEEXAMPLES]
#ifdef IMPLEMENT_CODEEXAMPLES
	ExampleDisposeContext();
#endif // IMPLEMENT_CODEEXAMPLES

[!endif]
	nResult = CPeekEngineContext::OnDisposeContext();

	return nResult;
}


#ifdef IMPLEMENT_FILTERPACKET
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

[!if NOTIFY_CODEEXAMPLES]
#ifdef IMPLEMENT_CODEEXAMPLES
	ExampleFilter( inPacket );
#endif // IMPLEMENT_CODEEXAMPLES

[!endif]
	return nResult;
}
#endif	 // IMPLEMENT_FILTERPACKET


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


#ifdef IMPLEMENT_PROCESSPACKET
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

[!if NOTIFY_CODEEXAMPLES]
#ifdef IMPLEMENT_CODEEXAMPLES
	ExampleProcessPacket( inPacket );
#endif // IMPLEMENT_CODEEXAMPLES

[!endif]
	return nResult;
}
#endif	// IMPLEMENT_PROCESSPACKET


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
	int	nResult = PEEK_PLUGIN_SUCCESS;

	(void)inMessage;
	(void)outResponse;

	PeekLock lock( m_Mutex );

	// Create and Load the appropriate Message object.
	std::unique_ptr<CPeekMessage>	spMessage = MessageFactory( inMessage );
	if ( spMessage.get() != nullptr ) {
		switch ( spMessage->GetType() ) {
			case CMessageGetOptions::s_nMessageType:
				nResult = ProcessGetOptions( spMessage.get() );
				break;
[!if NOTIFY_CODEEXAMPLES]

#ifdef IMPLEMENT_CODEEXAMPLES
[!if NOTIFY_TAB && NOTIFY_UIHANDLER]
			case CMessageTab::s_nMessageType:
				nResult = ExampleProcessTabMessage( spMessage.get() );
				break;
[!endif]
[!if !NOTIFY_UIHANDLER]
			case CMessageExample::s_nMessageType:
				nResult = ExampleProcessExample( spMessage.get() );
				break;
[!endif]
#endif // IMPLEMENT_CODEEXAMPLES
[!endif]

			default:
				nResult = PEEK_PLUGIN_FAILURE;
		}
	}

	if ( nResult == PEEK_PLUGIN_SUCCESS ) {
		spMessage->StoreResponse( outResponse );
	}

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

[!if NOTIFY_CODEEXAMPLES]
#ifdef IMPLEMENT_CODEEXAMPLES
	ExampleProcessTime();
#endif // IMPLEMENT_CODEEXAMPLES

[!endif]
	return nResult;
}


 #ifdef IMPLEMENT_RESETPROCESSING
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

[!if NOTIFY_CODEEXAMPLES]
#ifdef IMPLEMENT_CODEEXAMPLES
	ExampleReset();
#endif // IMPLEMENT_CODEEXAMPLES

[!endif]
	return nResult;
}
#endif	// IMPLEMENT_RESETPROCESSING

#ifdef IMPLEMENT_PACKETPROCESSOREVENTS
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

[!if NOTIFY_CODEEXAMPLES]
#ifdef IMPLEMENT_CODEEXAMPLES
	ExampleStartCapture();
#endif // IMPLEMENT_CODEEXAMPLES

[!endif]
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

[!if NOTIFY_CODEEXAMPLES]
#ifdef IMPLEMENT_CODEEXAMPLES
	ExampleStopCapture();
#endif // IMPLEMENT_CODEEXAMPLES

[!endif]
	return nResult;
}
#endif	// IMPLEMENT_PACKETPROCESSOREVENTS

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

[!if NOTIFY_CODEEXAMPLES]
#ifdef IMPLEMENT_CODEEXAMPLES
	ExampleSummary();
#endif // IMPLEMENT_CODEEXAMPLES

[!endif]
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

[!if NOTIFY_CODEEXAMPLES]
#ifdef IMPLEMENT_CODEEXAMPLES
	ExampleContextPrefs();
#endif // IMPLEMENT_CODEEXAMPLES

[!endif]
	return PEEK_PLUGIN_SUCCESS;
}


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		ProcessGetOptions
//
//	Process the Peek Plugin Message: Get Options.
//	This protocol is private to this plugin.
// -----------------------------------------------------------------------------

int
COmniEngineContext::ProcessGetOptions(
	CPeekMessage* ioMessage )
{
	PeekLock lock( m_Mutex );

	CMessageGetOptions*	pMessage = dynamic_cast<CMessageGetOptions*>( ioMessage );
	_ASSERTE( pMessage != nullptr );
	if ( pMessage == nullptr ) return PEEK_PLUGIN_FAILURE;

	// Create the modeler.
	if ( !pMessage->StartResponse() ) {
		return PEEK_PLUGIN_FAILURE;
	}

	// Set the response (output).
	pMessage->SetOptions( GetOptions() );

	return PEEK_PLUGIN_SUCCESS;
}
#endif // IMPLEMENT_PLUGINMESSAGE
[!if NOTIFY_CODEEXAMPLES]


#ifdef IMPLEMENT_CODEEXAMPLES
////////////////////////////////////////////////////////////////////////
// Code Examples

#include "RefreshFile.h"
#include <fstream>

#ifdef EXAMPLE_FILECAPTUREBUFFER
// -----------------------------------------------------------------------------
//		ExampleBufferOperations
// -----------------------------------------------------------------------------

void
COmniEngineContext::ExampleBufferOperations()
{
	static bool bDemoed = false;
	if ( bDemoed ) return;

	const UInt32	nPacketNumber = 3;
	UInt32 nOriginalCount = m_FileCaptureBuffer.GetCount();
	if ( nOriginalCount < nPacketNumber ) return;

	bDemoed = true;

	///////////////////////////////////////////////
	// Capture Buffer Operations Demo
	///////////////////////////////////////////////

	// Create two new buffers
	CFileCaptureBuffer	theNewFileCaptureBuffer;
	CCaptureBuffer		theNewCaptureBuffer;

	// Copy the buffers from the original
	theNewFileCaptureBuffer = m_FileCaptureBuffer;
	theNewCaptureBuffer = m_FileCaptureBuffer;

	// Show that the buffers are the same size
	_ASSERTE( theNewFileCaptureBuffer.GetCount() == nOriginalCount );
	_ASSERTE( theNewCaptureBuffer.GetCount() == nOriginalCount );
	_ASSERTE( theNewFileCaptureBuffer.GetCapacity() == m_FileCaptureBuffer.GetCapacity());
	_ASSERTE( theNewCaptureBuffer.GetCapacity() == m_FileCaptureBuffer.GetCapacity() );

	// Instantiate three empty packets
	CPacket OriginalBufferPacket;
	_ASSERTE( OriginalBufferPacket.IsEmpty() ); // The packet is null at this point.
	CPacket NewFileCaptureBufferPacket;
	CPacket NewCaptureBufferPacket;

	// Get packet index for the third packet
	const UInt32 nPacketIndex = m_FileCaptureBuffer.GetPacketIndex( nPacketNumber );

	// Get the third packet from each of the three buffers.
	// Note that GetPacket() returns a packet that holds pointers to the original data inside the buffer.
	// Thus the packet data has not been replicated.
	OriginalBufferPacket = m_FileCaptureBuffer.GetPacket( nPacketIndex );
	_ASSERTE( OriginalBufferPacket.IsAttached() ); // This copy is associated with the buffer
	NewFileCaptureBufferPacket = theNewFileCaptureBuffer.GetPacket( nPacketIndex );
	NewCaptureBufferPacket = theNewCaptureBuffer.GetPacket( nPacketIndex );

	// Show that the retrieved packets are "the same"
	_ASSERTE( NewFileCaptureBufferPacket == OriginalBufferPacket);
	_ASSERTE( NewCaptureBufferPacket  == OriginalBufferPacket );

	// Get a fresh copy of the third packet in the buffer.
	// This copy has replicated all the packet data from the source packet inside the buffer.
	CPacket OriginalBufferPacketDup;
	m_FileCaptureBuffer.ReadPacket( nPacketIndex, OriginalBufferPacketDup );
	_ASSERTE( OriginalBufferPacketDup.IsDetached() ); // This copy is not associated with the buffer

	// Demonstrate copying the packet.  The destination packet will be attached because the
	// source packet is attached.  This means that the packet data actually still resides in
	// in the original buffer.
	CPacket PacketCopy1( OriginalBufferPacket );
	CPacket PacketCopy2 = OriginalBufferPacket;

	_ASSERTE( PacketCopy1 == OriginalBufferPacket);
	_ASSERTE( PacketCopy1.IsAttached() );
	_ASSERTE( PacketCopy2 == OriginalBufferPacket);
	_ASSERTE( PacketCopy2.IsAttached() );

	///////////////////////////////////////////////
	// Packet Array Operations Demo
	///////////////////////////////////////////////

	CPacketArray theAttachedPacketArray( CPacketArray::kType_Attached );
	CPacketArray theDetachedPacketArray( CPacketArray::kType_Detached );

	theAttachedPacketArray.Load( m_FileCaptureBuffer );
	_ASSERTE( theAttachedPacketArray.GetCount() == nOriginalCount );
	theDetachedPacketArray.Load( m_FileCaptureBuffer );
	_ASSERTE( theDetachedPacketArray.GetCount() == nOriginalCount );

	// Read a packet from the attached array - keep it attached
	CPacket AttachedPacket1 = theAttachedPacketArray.Get( nPacketIndex );
	_ASSERTE( AttachedPacket1 == OriginalBufferPacket );
	_ASSERTE( AttachedPacket1.IsAttached() );

	// Read a packet from the attached array.  The destination packet
	// will be a detached packet, i.e. it will own its data.
	CPacket DetachedPacket1 = theAttachedPacketArray.Get( nPacketIndex, true );
	_ASSERTE( DetachedPacket1 == OriginalBufferPacket );
	_ASSERTE( DetachedPacket1.IsDetached() );

	// Read a packet from the detached array - the destination packet
	// will be detached in that its data will not be owned by a buffer,
	// but the data will be reference counted rather than replicated.
	CPacket DetachedPacket2 = theDetachedPacketArray.Get( nPacketIndex );
	_ASSERTE( DetachedPacket2 == OriginalBufferPacket );
	_ASSERTE( DetachedPacket2.IsDetached() );

	// Copy a packet.  The destination packet will be attached or
	// detached based based on disposition of the source packet.
	// A destination detached packet will have reference counted data.
	CPacket AttachedPacket2;
	CPacket DetachedPacket3;
	AttachedPacket2 = AttachedPacket1;
	_ASSERTE( AttachedPacket2.IsAttached() );
	_ASSERTE( AttachedPacket2 == AttachedPacket1 );
	DetachedPacket3 = DetachedPacket2;
	_ASSERTE( DetachedPacket3.IsDetached() );
	_ASSERTE( DetachedPacket3 == DetachedPacket2 );

	// An equivalent way of copying a detached packet
	// with reference counted data.
	CPacket DetachedPacket4( DetachedPacket2 );
	_ASSERTE( DetachedPacket4 == DetachedPacket2 );
	_ASSERTE( DetachedPacket4.IsDetached() );

	// Show two ways of using Copy with a detach argument to guarantee
	// that the resulting packet is detached and reference counted
	// regardless of whether the source packet is attached or detached.
	CPacket DetachedPacket5;
	CPacket DetachedPacket6;
	DetachedPacket5.Copy( AttachedPacket1, true );
	DetachedPacket6.Copy( DetachedPacket1, true );
	_ASSERTE( DetachedPacket5.IsDetached() );
	_ASSERTE( DetachedPacket6.IsDetached() );
	_ASSERTE( DetachedPacket5 == DetachedPacket6 );

	// Perform other packet array operations
	CPacketArray theNewPacketArray;

	theNewPacketArray.Add( PacketCopy2 );
	theNewPacketArray.Add( DetachedPacket1 );
	theNewPacketArray.Insert( 1, PacketCopy2 );
	theNewPacketArray.RemoveAt( 0 );
	theNewPacketArray.Replace( 0, PacketCopy2 );
	_ASSERTE( theNewPacketArray.GetCount() == 2 );

	// Append an array
	size_t	nDetachedCount = theDetachedPacketArray.GetCount();
	theNewPacketArray.Append( theDetachedPacketArray );
	_ASSERTE( theNewPacketArray.GetCount() == nDetachedCount + 2 );

	// Replace contents of an array with contents of another array
	theNewPacketArray.Copy( theAttachedPacketArray );
	_ASSERTE( theNewPacketArray.GetCount() == theAttachedPacketArray.GetCount() );
	_ASSERTE( theNewPacketArray.Get( 0 ).IsAttached() == theAttachedPacketArray.Get( 0 ).IsAttached() );
}
#endif // EXAMPLE_FILECAPTUREBUFFER


// -----------------------------------------------------------------------------
//		ExampleContextPrefs
// -----------------------------------------------------------------------------

void
COmniEngineContext::ExampleContextPrefs()
{
#ifdef IMPLEMENT_NOTIFICATIONS
	const bool bReceiveNotifies = m_Options.IsReceiveNotifies();
	GetNotifyService().DoSetReceiveNotifications( bReceiveNotifies );
#endif //IMPLEMENT_NOTIFICATIONS
}


#ifdef EXAMPLE_DECODE
// -----------------------------------------------------------------------------
//		ExampleDecodePacket
// -----------------------------------------------------------------------------

int
COmniEngineContext::ExampleDecodePacket(
	CPacket& inPacket )
{
	try {
		if ( inPacket.IsError() ) return PEEK_PLUGIN_SUCCESS;

		CLayerEthernet	layerEthernet;
		if ( !inPacket.GetLayer( layerEthernet ) ) return PEEK_PLUGIN_SUCCESS;

		CLayerIP	layerIP;
		if ( !inPacket.GetLayer( layerIP ) ) return PEEK_PLUGIN_SUCCESS;
		if ( layerIP.IsFragment() ) return PEEK_PLUGIN_SUCCESS;

		// This is example code, so we'll limit the capture
		if ( m_CaptureBuffer.IsFull() || m_CaptureBuffer.GetCount() > 499 ) return PEEK_PLUGIN_SUCCESS;

		m_CaptureBuffer.WritePacket( inPacket );
		UInt32	nPacketIndex = m_CaptureBuffer.GetCount();

		CPeekString		strTextFileName( m_strFileName + L".txt" );
		std::wofstream	myout;
		myout.open( strTextFileName, (std::ios_base::out | std::ios_base::app) );
		myout << std::boolalpha << std::setfill( L'0' );

		myout << L"inPacket " << nPacketIndex << std::endl;
		myout << L"Packet Length:      " << inPacket.GetPacketLength() << std::endl;
		myout << L"Destination:        " << layerEthernet.GetDestination().Format() << std::endl;
		myout << L"Source:             " << layerEthernet.GetSource().Format() << std::endl;
		myout << L"Protocol:           " << std::hex << std::setw( 2 ) << layerEthernet.GetProtocolType() << std::dec << std::endl;
		myout << L"Ethernet offset:    " << std::hex << std::setw( 2 ) << layerEthernet.GetOffset() << std::dec << std::endl;
		myout << std::endl;

		myout << L"IP Header Length:   " << layerIP.GetHeaderRef().GetLength() << " bytes" << std::endl;
		myout << L"Version:            " << layerIP.GetVersion() << std::endl;
		myout << L"Length per Header:  " << layerIP.GetLength() << std::endl;
		if ( layerIP.GetVersion() == kIpVersion_4 ) {
			CIpV4Header	ipHeader = layerIP.GetIp4Header();

			myout << L"Differentiated:     " << ipHeader.GetTypeOfService() << std::endl;
			myout << L"  Code Point:       " << std::hex << std::setw( 4 ) << ipHeader.GetDiffServCodePoint() << std::dec << std::endl;
			myout << L"Payload Length:     " << layerIP.GetPayloadLength() << std::endl;
			myout << L"Identifier:         " << ipHeader.GetIdentifier() << std::endl;
			myout << L"Fragment Flags:     " << ipHeader.GetFragmentFlags() << std::endl;
			myout << L"  Do Not Fragment:  " << ipHeader.IsDoNotFragment() << std::endl;
			myout << L"  More Fragments    " << ipHeader.IsMoreFragments() << std::endl;
			myout << L"Fragment Offset:    " << ipHeader.GetFragmentOffset() << std::endl;
			myout << L"Time To Live:       " << ipHeader.GetTimeToLive() << std::endl;
			myout << L"Protocol:           " << layerIP.GetPayloadProtocol() << std::endl;
			myout << L"Checksum:           " << std::hex << std::setw( 4 ) << ipHeader.GetHeaderChecksum() << std::dec << std::endl;
		}
		if ( layerIP.GetVersion() == kIpVersion_6 ) {
			CIpV6Header	ipHeader = layerIP.GetIp6Header();

			myout << L"Traffic Class:      " << ipHeader.GetClass() << std::endl;
			myout << L"Flow Label:         " << std::hex << std::setw( 8 ) << ipHeader.GetFlowLabel() << std::dec << std::endl;
			myout << L"Payload Length:     " << layerIP.GetPayloadLength() << std::endl;
			myout << L"Next Header:        " << std::hex << std::setw( 2 ) << ipHeader.GetPayloadProtocol() << std::dec << std::endl;
			myout << L"Hop Limit:          " << ipHeader.GetHopLimit() << std::endl;
		}
		myout << L"Source:             " << layerIP.GetSource().Format() << std::endl;
		myout << L"Destination:        " << layerIP.GetDestination().Format() << std::endl;
		myout << std::endl;

		CLayerTCP	layerTCP;
		if ( inPacket.GetLayer( layerTCP ) ) {
			CTcpHeader tcpHeader = layerTCP.GetHeader();

			myout << L"TCP Header:" << std::endl;
			myout << L"Source Port:        " << layerTCP.GetSource() << std::endl;
			myout << L"Destination Port:   " << layerTCP.GetDestination() << std::endl;
			myout << L"Sequence Number:    " << tcpHeader.GetSequenceNumber() << std::endl;
			myout << L"Ack Number:         " << tcpHeader.GetAckNumber() << std::endl;
			myout << L"TCP Offset:         " << tcpHeader.GetTCPOffset() << std::endl;
			myout << L"Flags:              " << std::hex << std::setw( 2 ) << tcpHeader.GetFlags() << std::dec << std::endl;
			myout << L"  CWR:              " << tcpHeader.IsCongestion() << L" : Congestion Window Reduced" << std::endl;
			myout << L"  ECE:              " << tcpHeader.IsEcnEcho() << L" : ECN-Echo" << std::endl;
			myout << L"  URG:              " << tcpHeader.IsUrgent() << L" : Urgent Pointer" << std::endl;
			myout << L"  ACK:              " << tcpHeader.IsAck() << L" : Acknowledgment" << std::endl;
			myout << L"  PSH:              " << tcpHeader.IsPush() << L" : Push" << std::endl;
			myout << L"  RST:              " << tcpHeader.IsReset() << L" : Reset" << std::endl;
			myout << L"  SYN:              " << tcpHeader.IsSyn() << L" : Synchronize sequence numbers" << std::endl;
			myout << L"  FIN:              " << tcpHeader.IsFin() << L" : No more data" << std::endl;
			myout << L"Window Size:        " << tcpHeader.GetWindowSize() << std::endl;
			myout << L"Checksum:           " << std::hex << std::setw( 4 ) << tcpHeader.GetChecksum() << std::dec << std::endl;
			myout << L"Urgent Pointer:     " << tcpHeader.GetUrgentPointer() << std::endl;
			myout << L"TCP Payload Length: " << layerIP.GetPayloadLength() - tcpHeader.GetTCPOffset() << std::endl;
		}

		CLayerUDP	layerUDP;
		if ( inPacket.GetLayer( layerUDP ) ) {
			CUdpHeader udpHeader = layerUDP.GetHeader();

			myout << L"UDP Header:" << std::endl;

			myout << L"Source Port:        " << layerUDP.GetSourcePort() << std::endl;
			myout << L"Destination Port:   " << layerUDP.GetDestinationPort() << std::endl;
			myout << L"UDP Payload Length: " << layerUDP.GetLength() - layerUDP.GetHeaderRef().GetUdpHdrLength() << std::endl;
			myout << L"UDP Checksum:       " << std::hex << std::setw( 4 ) << udpHeader.GetChecksum() << std::dec << std::endl;
		}

		myout << std::endl;
		myout << L"======================================================================";
		myout << std::endl << std::endl;
	}
	catch ( ... ) {
		return PEEK_PLUGIN_FAILURE;
	}

	return PEEK_PLUGIN_SUCCESS;
}
#endif // EXAMPLE_DECODE


// -----------------------------------------------------------------------------
//		ExamplePacketMembers
// -----------------------------------------------------------------------------

int
COmniEngineContext::ExamplePacketMembers(
	CPacket& inPacket )
{
	try {
		CPeekOutString	strMsg;

		strMsg << std::boolalpha;

		UInt32			nFlags = inPacket.GetFlags();
		strMsg << L"Flags: " << std::setfill( L'0' ) << std::setw(8)
			   << nFlags << std::setfill( L' ' ) << std::endl;

		UInt32			nFlowId = inPacket.GetFlowId();
		strMsg << L"FlowId: " << nFlowId << std::endl;

		tMediaType		mtPacket = inPacket.GetMediaType();
		strMsg << L"Media Type: " << mtPacket.fType << std::endl;
		strMsg << L"Media Sub Type: " << mtPacket.fSubType << std::endl;

		CPacketBuffer	thePacketBuffer = inPacket.GetPacketBuffer();
		bool			bValid = thePacketBuffer.IsValid();
		strMsg << L"Get Packet Buffer: Is Valid: " << bValid << std::endl;

		UInt16			nPacketDataLength = inPacket.GetPacketDataLength();
		strMsg << L"Packet Data Length: " << nPacketDataLength << std::endl;

		UInt16			nPacketLength = inPacket.GetPacketLength();
		strMsg << L"Packet Length: " << nPacketLength << std::endl;

		UInt64			nPacketNumber = inPacket.GetPacketNumber();
		strMsg << L"Packet Number: " << nPacketNumber << std::endl;

		UInt32			nProtoSpec = inPacket.GetProtoSpec();
		strMsg << L"Proto Spec: " << nProtoSpec << std::endl;

		UInt16			nProtoSpecId = inPacket.GetProtoSpecId();
		strMsg << L"Proto Spec Id: " << nProtoSpecId << std::endl;

		UInt16			nSliceLength = inPacket.GetSliceLength();
		strMsg << L"Slice Length: " << nSliceLength << std::endl;

		UInt32			nStatus = inPacket.GetStatus();
		strMsg << L"Status: " << nStatus << std::endl;

		UInt64			nTimeStamp = inPacket.GetTimeStamp();
		strMsg << L"Time Stamp: " << nTimeStamp << std::endl;

		UInt32			nVoIPCallFlowId = inPacket.GetVoIPCallFlowIndex();
		strMsg << L"VoIP Call Flow Id: " << nVoIPCallFlowId << std::endl;

		UInt64			nVoIPCallId = inPacket.GetVoIPCallId();
		strMsg << L"VoIP Call Id: " << nVoIPCallId << std::endl;


		bool	bBroadcast = inPacket.IsBroadcast();
		strMsg << L"Is Broadcast: " << bBroadcast << std::endl;

		bool	bEmpty = inPacket.IsEmpty();
		strMsg << L"Is Empty: " << bEmpty << std::endl;

		bool	bError = inPacket.IsError();
		strMsg << L"Is Error: " << bError << std::endl;

		bool	bFullDuplex = inPacket.IsFullDuplex();
		strMsg << L"Is FullDuplex: " << bFullDuplex << std::endl;

		bool	bMultiCast = inPacket.IsMultiCast();
		strMsg << L"Is MultiCast: " << bMultiCast << std::endl;

		bool	bWAN = inPacket.IsWAN();
		strMsg << L"Is WAN: " << bWAN << std::endl;

		bool	bWireless = inPacket.IsWireless();
		strMsg << L"Is Wireless: " << bWireless << std::endl;

		//const UInt8*	pData = nullptr;
		//UInt16		nDataLength = inPacket.GetPacketData( pData );
	}
	catch ( ... ) {
		;		// catch and squelch.
	}

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		ExampleCreateContext
// -----------------------------------------------------------------------------

void
COmniEngineContext::ExampleCreateContext()
{
	if ( IsGlobalContext() ) return;

	// Demonstrate usage of some helper interfaces:
	UInt64 nLinkSpeed( 0 );
	nLinkSpeed = m_PeekContextProxy.GetPacketBuffer().GetLinkSpeed();
	if ( nLinkSpeed > 0 ) nLinkSpeed = 0;
	nLinkSpeed = m_PeekContextProxy.GetAdapter().GetLinkSpeed();
	if ( nLinkSpeed > 0 ) nLinkSpeed = 0;

	bool bTest( false );
	bTest = m_PeekContextProxy.GetDynamicPacketBuffer().IsFull();
	if ( bTest ) bTest = false;
	bTest = m_PeekContextProxy.GetCapture().IsCapturing();
	if ( bTest ) bTest = false;

  #ifdef EXAMPLE_FILECAPTUREBUFFER
	// Demonstrate initialization of a file capture buffer
	CPeekString		strTempPath;
	strTempPath.GetEnvironmentVariable( L"TMP" );
	m_FileCaptureBuffer.SetRefreshFilePath( strTempPath );
	m_FileCaptureBuffer.SetRefreshFileBaseName( L"Outfile" ); // Note: this can be an empty string
	m_FileCaptureBuffer.SetRefreshFileType( kRefreshFileTypeTimeStamp );

	// Demo a size and time that are small and often enough to see easily,
	// depending on your network.
	m_FileCaptureBuffer.SetRefreshSize( 10, kMegabytes );
	m_FileCaptureBuffer.SetRefreshTimeInterval( 1, kTimeUnitMinutes );
  #endif // EXAMPLE_FILECAPTUREBUFFER

  #ifdef IMPLEMENT_NOTIFICATIONS
	// Receive all levels of notification (assuming the user has selected
	// the option of receiving notifications).
	GetNotifyService().SetSeverityMask( ENABLE_NOTIFY_SEVERITY_ALL );

	// Receive notifications from all sources except current instance of this plugin
	GetNotifyService().SetSeveritySrcExclusions( NOTIFY_SOURCES_EXCLUDE_ALL_INSTANCES );
  #endif // IMPLEMENT_NOTIFICATIONS
}


// -----------------------------------------------------------------------------
//		ExampleDisposeContext
// -----------------------------------------------------------------------------

void
COmniEngineContext::ExampleDisposeContext()
{
  #ifdef EXAMPLE_FILECAPTUREBUFFER
	m_FileCaptureBuffer.Deactivate();
  #endif // EXAMPLE_FILECAPTUREBUFFER

#ifdef IMPLEMENT_NOTIFICATIONS
	// The following is not necessary, but illustrates how notify receiving can be
	// disabled programmatically:
	//	GetNotifyService().DisableReceiveNotifies();
#endif //IMPLEMENT_NOTIFICATIONS
}


// -----------------------------------------------------------------------------
//		ExampleFilter
// -----------------------------------------------------------------------------

void
COmniEngineContext::ExampleFilter(
	CPacket& inPacket )
{
	(void)inPacket;
}


// -----------------------------------------------------------------------------
//		ExampleInitialize
// -----------------------------------------------------------------------------

void
COmniEngineContext::ExampleInitialize()
{
}


#ifdef IMPLEMENT_PLUGINMESSAGE
[!if !NOTIFY_UIHANDLER]
// -----------------------------------------------------------------------------
//		ExampleProcessExample
// -----------------------------------------------------------------------------

int
COmniEngineContext::ExampleProcessExample(
	CPeekMessage* ioMessage )
{
	CMessageExample*	pMessage = dynamic_cast<CMessageExample*>( ioMessage );
	_ASSERTE( pMessage != nullptr );
	if ( pMessage == nullptr ) return PEEK_PLUGIN_FAILURE;

	// Validate the message (input).
	const CPeekString&	strMessage = pMessage->GetMessage();
	const CPeekStream&	psData = pMessage->GetMessageData();

	_ASSERTE( strMessage == CMessageExample::GetTagMessage() );
	if ( strMessage != CMessageExample::GetTagMessage() ) {
		return PEEK_PLUGIN_FAILURE;
	}
	_ASSERTE( psData.GetLength() == 30 );
	if ( psData.GetLength() == 30 ) {
		const UInt8* pData = psData.GetData();
		_ASSERTE( pData != nullptr );
		if ( pData ) {
			for ( int i = 0; i < 30; i++ ) {
				_ASSERTE( *pData == 0x55 );
				if ( *pData != 0x55 ) {
					return PEEK_PLUGIN_FAILURE;
				}
				pData++;
			}
		}
	}
	else {
		return PEEK_PLUGIN_FAILURE;
	}

	// Create the modeler.
	if ( !pMessage->StartResponse() ) {
		return PEEK_PLUGIN_FAILURE;
	}

	// Set the response (output).
	pMessage->SetResponse( CMessageExample::GetTagResponse() );

	UInt8	ResponseData[500];
	memset( ResponseData, 0x56, sizeof( ResponseData ) );
	pMessage->SetResponseData( sizeof( ResponseData ), ResponseData );

	return PEEK_PLUGIN_SUCCESS;
}
[!endif]
[!if NOTIFY_TAB && NOTIFY_UIHANDLER]


// -----------------------------------------------------------------------------
//		ExampleProcessTabMessage
// -----------------------------------------------------------------------------

int
COmniEngineContext::ExampleProcessTabMessage(
	CPeekMessage* ioMessage )
{
	CMessageTab*	pMessage = dynamic_cast<CMessageTab*>( ioMessage );
	_ASSERTE( pMessage != nullptr );
	if ( pMessage == nullptr ) return PEEK_PLUGIN_FAILURE;

	// Process the message string.
	CPeekString	strMessage( pMessage->GetMessage() );

	// Create the response modeler.
	if ( !pMessage->StartResponse() ) return PEEK_PLUGIN_FAILURE;

	// No response content.
	CPeekString	strResponse( strMessage );
	strResponse.Reverse();
	strResponse.Append( L" : " );
	strResponse.Append( m_Id.Format() );

	pMessage->SetResponse( strResponse );

	return PEEK_PLUGIN_SUCCESS;
}
[!endif]
#endif // IMPLEMENT_PLUGINMESSAGE


// -----------------------------------------------------------------------------
//		ExampleProcessPacket
// -----------------------------------------------------------------------------

void
COmniEngineContext::ExampleProcessPacket(
	CPacket& inPacket )
{
	// Get the packet number.
	UInt64	nPacketNumber = inPacket.GetPacketNumber();

	// Get the packet ProtoSpec Id.
	UInt16	nProtoSpecId = inPacket.GetProtoSpecId();

	// Get the long name
#ifdef WP_LINUX
	// TODO Determine if Protospecs is available under Linux.
	CPeekOutString	strName;
	strName << nProtoSpecId;
#else
	CPeekString strName = GetProtospecs().GetLongName( nProtoSpecId );
#endif // WP_LINUX

	// DoLogMessage to create a log message
	CPeekOutString strMessage;
	if ( nPacketNumber < 0xFFFFFFFF ) {
		strMessage << L"Packet number " << nPacketNumber << L" is " << strName;

		// CAUTION: Do not implement both DoLogMessage and DoNotify at the same time.  DoNotify
		// is a "superset" that already includes messages to the logger.  Only implement DoNotify
		// if you need other plugins to receive notifications.

		// DoLogMessage posts a message to the message logger.  This is viewed on the Log tab for
		// the capture in OmniPeek.
		DoLogMessage( strMessage );

		// DoNotify broadcasts the message to all registered users which includes the message logger.
		// It also broadcasts to any plugins that have registered to receive notifications.
//		DoNotify( strMessage );
	}

  #ifdef EXAMPLE_DECODE
	ExampleDecodePacket( inPacket );
  #endif // EXAMPLE_DECODE

	ExamplePacketMembers( inPacket );

	CPacket	pktLocal = inPacket;
	ExamplePacketMembers( pktLocal );

	pktLocal.Detach();
	ExamplePacketMembers( pktLocal );

	CPacket	pktDetached( inPacket, true );
	ExamplePacketMembers( pktDetached );

	if ( pktLocal != inPacket ) {
		CPeekString	strMsg( L"Internal Failure." );
	}

  #ifdef EXAMPLE_FILECAPTUREBUFFER
	// Put the packet into the capture buffer
	m_FileCaptureBuffer.WritePacket( inPacket );
  #endif // EXAMPLE_FILECAPTUREBUFFER
}


// -----------------------------------------------------------------------------
//		ExampleProcessTime
// -----------------------------------------------------------------------------

void
COmniEngineContext::ExampleProcessTime()
{
//  #ifdef EXAMPLE_FILECAPTUREBUFFER
//	m_FileCaptureBuffer.Monitor();
//  #endif // EXAMPLE_FILECAPTUREBUFFER
}


// -----------------------------------------------------------------------------
//		ExampleReset
// -----------------------------------------------------------------------------

void
COmniEngineContext::ExampleReset()
{
}


// -----------------------------------------------------------------------------
//		ExampleStartCapture
// -----------------------------------------------------------------------------

void
COmniEngineContext::ExampleStartCapture()
{
  #ifdef EXAMPLE_DECODE
	CPeekString	strEnvTmp;
	strEnvTmp.GetEnvironmentVariable( L"TMP" );

	CPeekOutString	strFileName;
	strFileName << strEnvTmp << L"\\" << CPeekTime::GetTimeStringFileName();
	m_strFileName = strFileName;

	m_CaptureBuffer.Reset();
  #endif // EXAMPLE_DECODE

  #ifdef EXAMPLE_FILECAPTUREBUFFER
	m_FileCaptureBuffer.Activate();
  #endif // EXAMPLE_FILECAPTUREBUFFER
}


// -----------------------------------------------------------------------------
//		ExampleStopCapture
// -----------------------------------------------------------------------------

void
COmniEngineContext::ExampleStopCapture()
{
  #ifdef EXAMPLE_DECODE
	CPeekString		strPktFileName( m_strFileName + L".pkt" );
	m_CaptureBuffer.Save( strPktFileName );
  #endif // EXAMPLE_DECODE

  #ifdef EXAMPLE_FILECAPTUREBUFFER
	m_FileCaptureBuffer.Deactivate();
  #endif // EXAMPLE_FILECAPTUREBUFFER
}


// -----------------------------------------------------------------------------
//		ExampleSummary
// -----------------------------------------------------------------------------

void
COmniEngineContext::ExampleSummary()
{
  #ifdef EXAMPLE_FILECAPTUREBUFFER
	ExampleBufferOperations();
  #endif // EXAMPLE_FILECAPTUREBUFFER
}


#ifdef EXAMPLE_DATATABLE
// -----------------------------------------------------------------------------
//		DataTableExample
// -----------------------------------------------------------------------------

void
COmniEngineContext::DataTableExample()
{
	CPeekTable PeekTable;

	PeekTable.SetTableName( L"NewTable" );

	CPeekString strTableName = PeekTable.GetTableName();

	CPeekTableColumnCollection Columns = PeekTable.GetColumnCollection();

	CPeekTableRowCollection Rows = PeekTable.GetRowCollection();

	Columns.AddStringCol( L"Col1" );
	Columns.AddStringCol( L"Col2" );
	Columns.AddStringCol( L"Col3" );
	Columns.AddSInt16Col( L"Col4" );
	Columns.AddBoolCol( L"Col5" );
	Columns.AddUInt64Col( L"Col6" );

	CPeekTableRow row;
	Rows.Add( row );

	row.SetItem( 0, L"Row 1 Col1 Value");
	row.SetItem( 1, L"Row 1 Col2 Value");
	row.SetItem( 2, L"Row 1 Col3 Value");
	row.SetItem( 3, (SInt16)12345 );

	Rows.Add( row );
	row.SetItem( 0, L"Row 2 Col1 Value");
	row.SetItem( 1, L"Row 2 Col2 Value");
	row.SetItem( 2, L"Row 2 Col3 Value");
	row.SetItem( 3, (SInt16)-5432 );
	row.SetItem( 4, true );
	row.SetItem( 5, (UInt64)12345678901234567890 );

	CPeekTableRow	rowOut;
	Rows.GetTableRow( 1, rowOut );

	CPeekString		strTest1;
	rowOut.GetItem( 1, strTest1 );
	_ASSERTE( strTest1 == L"Row 2 Col2 Value" );

	CPeekString		strTest2;
	rowOut.GetItem( 2, strTest2 );
	_ASSERTE( strTest2 == L"Row 2 Col3 Value" );

	CPeekString		strTest3;
	rowOut.GetItemFromName( L"Col1", strTest3 );
	_ASSERTE( strTest3 == L"Row 2 Col1 Value" );

	SInt16 nTest1;
	rowOut.GetItem( 3, nTest1 );
	_ASSERTE( nTest1 == -5432 );

	bool bTest1;
	rowOut.GetItem( 4, bTest1 );
	_ASSERTE( bTest1 == true );

	UInt64 nTest2;
	rowOut.GetItem( 5, nTest2 );
	_ASSERTE( nTest2 == 12345678901234567890 );

	SInt32 nCount = Rows.GetCount();
	Rows.RemoveAt( 0 );	
	nCount = Rows.GetCount();
	_ASSERTE( nCount == 1 );

	CPeekTableColumn TestTableColumn;
	if ( Columns.GetTableColumn( 2, TestTableColumn ) == true ) {
		_ASSERTE( TestTableColumn.GetColumnName() == L"Col3" );
	}

	CPeekTable TestPeekTable1( false );
	rowOut.GetTableFromRow( TestPeekTable1 );

	CPeekTableColumnCollection TestColumns = TestPeekTable1.GetColumnCollection();	
	_ASSERTE( TestColumns.GetCount() == Columns.GetCount() );

	CPeekTableColumn ColumnTestOne;
	TestColumns.GetTableColFromName( L"Col4", ColumnTestOne );
	_ASSERTE( ColumnTestOne.GetColumnIndex() == 3 );

	CPeekTable TestPeekTable2( false );
	ColumnTestOne.GetTableFromColumn( TestPeekTable2 );

	CPeekString		strTest4;
	rowOut.GetItem( 2, strTest4 );
	_ASSERTE( strTest4 == L"Row 2 Col3 Value" );

	Rows.Clear();
	nCount = Rows.GetCount();
	_ASSERTE( nCount == 0 );
}
#endif // EXAMPLE_DATATABLE
//
////////////////////////////////////////////////////////////////////////
#endif // IMPLEMENT_CODEEXAMPLES
[!endif]
