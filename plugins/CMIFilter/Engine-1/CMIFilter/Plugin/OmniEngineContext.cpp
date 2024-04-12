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
#include "TimeConv.h"
#include "CMIPacket.h"
#include "FileEx.h"
#include "Version.h"

class CRemotePlugin;

//#define DEBUG_LOG_ERROR(m)	LogError(m)
#define DEBUG_LOG_ERROR(m)

#ifdef DEBUG_STATS
#define X_DEBUG_INCR(x)	x++
#else
#define X_DEBUG_INCR(x)
#endif

const CPeekString	kLogFilename( L"CMIFilterOE-ErrorLog.txt" );

#ifdef IMPLEMENT_SUMMARYMODIFYENTRY
  static CPeekString	s_strPacketsProcessed( L"Packets Processed" );
#endif // IMPLEMENT_SUMMARYMODIFYENTRY

using namespace FilePath;


// =============================================================================
//		COmniEngineContext
// =============================================================================

COmniEngineContext::COmniEngineContext(
	CGlobalId&		inId,
	CRemotePlugin*	inRemotePlugin )
	:	CPeekEngineContext( inId, inRemotePlugin )
	,	m_bFailure( false )
	,	m_bIsInserting( false )
#ifdef DEBUG_STATS
	,	m_nCmiPacketNumber( 0 )
	,	m_nInputPacketNumber( 0 )
	,	m_nNonSpuriousTestPacketNum( 0 )
	,	m_nInsertErrors( 0 )
	,	m_nPackets( 0 )
	,	m_nPacketsCreated( 0 )
	,	m_nPacketsDeleted( 0 )
	,	m_nPacketsDeletedInInsert( 0 )
	,	m_nPacketsDeletedInProcess( 0 )
	,	m_nOrphans( 0 )
	,	m_nOrphanFiles( 0 )
	,	m_nIPIDOrphans( 0 )
#endif
{
}

COmniEngineContext::~COmniEngineContext()
{
	OnStopCapture( 0 );
}


#ifdef I_NOT_USED
// -----------------------------------------------------------------------------
//		OnCaptureStarting
//
//  The capture context will begin setup for capture.
//	The engine has entered its StartCapture() fuction.
//	Initialize plugin specific resources, but Capture Context specific objects
//	have not been created yet.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnCaptureStarting()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnCaptureStarted
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
	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnCaptureStopping
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
	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnCaptureStopped
//
//	The capture context has torn down the capture.
//	The engine is about to exit its StopCapture() fuction.
//	All Capture Context specific objects have been destroyed.
//	The plugin may not Insert Packets into the Capture Context.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnCaptureStopped()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}
#endif // I_NOT_USED


// -----------------------------------------------------------------------------
//		OnCreateContext
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnCreateContext()
{
	int	nResult = CPeekEngineContext::OnCreateContext();

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
	UInt16*		/*outBytesToAccept*/,
	CGlobalId&	/*outFilterId*/ )
{
	DEBUG_LOG_ERROR( L"OnFilter: Enter" );

	try {

		// Check to see if we are recursively in the packet insert stream.
		// If so then this packet is complete and has been accepted.
		if ( IsInserting() ) {
			DEBUG_LOG_ERROR( L"OnFilter: Exit - Inserting" );
			return PLUGIN_PACKET_ACCEPT;
		}

	  #ifdef DEBUG_STATS
		m_nInputPacketNumber++;
	  #endif

		// Reject error packets
		if ( inPacket.IsError() ) {
			DEBUG_LOG_ERROR( L"OnFilter: Exit - Error Packet" );
			return PLUGIN_PACKET_REJECT;
		}

		// Get the Ethernet layer
		CLayerEthernet layerEthernet;
		if ( !inPacket.GetLayer( layerEthernet ) ) {
			DEBUG_LOG_ERROR( L"OnFilter: Exit - Not Ethernet" );
			return PLUGIN_PACKET_REJECT;
		}

		// Get the IP layer
		CLayerIP layerIP;
		if ( !inPacket.GetLayer( layerIP ) ) {
			DEBUG_LOG_ERROR( L"OnFilter: Exit - Not IP" );
			return PLUGIN_PACKET_REJECT;
		}

		// Note: currently supports IPv4 only
		if ( layerIP.GetVersion() != kIpVersion_4 ) {
			DEBUG_LOG_ERROR( L"OnFilter: Exit - Not IPv4" );
			return PLUGIN_PACKET_REJECT;
		}

		// Only process UDP Packets or packet fragments: CMI is a UDP based protocol.
		// Or if GetLayer( UDP ) fails...
		if ( layerIP.GetIp4Header().GetPayloadProtocol() != kProtocol_UDP ) {
			DEBUG_LOG_ERROR( L"OnFilter: Exit - Not UDP" );
			return PLUGIN_PACKET_REJECT;
		}

		CCmiIpV4PacketPtr upPacket;
		{
			upPacket = CCmiIpV4PacketPtr( new CCmiUdpPacket( inPacket ) );
			ASSERT( upPacket );
			if ( upPacket.get() == nullptr ) Peek::Throw( E_FAIL );

			if ( !upPacket->IsValid() ) {
				ASSERT( layerIP.IsValid() );

				upPacket = CCmiIpV4PacketPtr( new CCmiIpV4Packet( inPacket) );
				ASSERT( upPacket );
				if ( upPacket.get() == nullptr ) Peek::Throw( E_FAIL );
				if ( upPacket->IsNotValid() ) {
					DEBUG_LOG_ERROR( L"OnFilter: Exit - Did not allocate packet" );
					return PLUGIN_PACKET_REJECT;
				}
			}
		}

		// Get the UDP layer
		CLayerUDP	layerUDP;
		bool		bUDP( inPacket.GetLayer( layerUDP ) );
		if ( bUDP ) {
			CPeekStream		stRemaining = layerUDP.GetRemainingBytes();
			const UInt8*	pId = stRemaining.GetData();
			UInt32			nBytesLeft = stRemaining.GetLength32();

			UInt32	nProtocolType = m_Options.GetProtocolType();
			bool	bHasInterceptId = m_Options.HasInterceptId();

			switch ( nProtocolType ) {
				case CInterceptId::kIdType_PCLI:
					{
						upPacket->SetDataOffset( 4 );
					}
					break;

				case CInterceptId::kIdType_SecurityId:
					{
						CSecurityId	idSecurity( nBytesLeft, pId );
						if ( bHasInterceptId ) {
							if ( !idSecurity.CompareInterceptId( m_Options.GetInterceptId() ) ) {
								DEBUG_LOG_ERROR( L"OnFilter: Exit - Not Security Id match" );
								return PLUGIN_PACKET_REJECT;
							}
						}
						upPacket->SetDataOffset( CSecurityId::s_nSize );
					}
					break;

				case CInterceptId::kIdType_Ericsson:
					{
						CEricsson	idEricsson( nBytesLeft, pId );
						if ( bHasInterceptId ) {
							if ( !idEricsson.CompareInterceptId( m_Options.GetInterceptId() ) ) {
								DEBUG_LOG_ERROR( L"OnFilter: Exit - Not Ericsson Id match" );
								return PLUGIN_PACKET_REJECT;
							}
						}
						upPacket->SetDataOffset( idEricsson.GetLength() );
					}
					break;

				case CInterceptId::kIdType_Arris:
					{
						CArris	idArris( nBytesLeft, pId );
						if ( bHasInterceptId ) {
							if ( !idArris.CompareInterceptId( m_Options.GetInterceptId() ) ) {
								DEBUG_LOG_ERROR( L"OnFilter: Exit - Not Arris Id match" );
								return PLUGIN_PACKET_REJECT;
							}
						}
						upPacket->SetDataOffset( idArris.GetLength() );
					}
					break;

				case CInterceptId::kIdType_Nokia:
					{
						CNokia	idNokia( nBytesLeft, pId );
						if ( bHasInterceptId ) {
							if ( !idNokia.CompareInterceptId( m_Options.GetInterceptId() ) ) {
								DEBUG_LOG_ERROR( L"OnFilter: Exit - Not Nokia Id match" );
								return PLUGIN_PACKET_REJECT;
							}
						}
						upPacket->SetDataOffset( idNokia.GetLength() );
					}
					break;

				case CInterceptId::kIdType_None:
					break;
			}
		}

		// Check the IP Addresses based on direction.
		if ( m_Options.IsIpFilter() ) {
			const CIpAddressPair	iapPacket( layerIP.GetSource(), layerIP.GetDestination() );
			UInt16					destPort = (bUDP) ? layerUDP.GetDestinationPort() : 0;
			if ( !m_Options.IsIpFilterMatch( iapPacket, bUDP, destPort ) ) {
				DEBUG_LOG_ERROR( L"OnFilter: Exit - Not Filter match" );
				return PLUGIN_PACKET_REJECT;
			}
		}

	  #ifdef DEBUG_STATS
		m_nNonSpuriousTestPacketNum++;
	  #endif

		CCmiPacketPtrList	ayNewPackets;
		ProcessPacket( m_Options, upPacket, ayNewPackets );
		InsertPackets( ayNewPackets );
		// TODO: any packets not inserted will be lost.
	}
	catch ( ... ) {
		DEBUG_LOG_ERROR( L"OnFilter: catch" );
		m_bFailure = true;
	}

	DEBUG_LOG_ERROR( L"OnFilter: Exit" );
	return PLUGIN_PACKET_REJECT;
}


// -----------------------------------------------------------------------------
//		OnNotify
//		: Implements INotify::Notify
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnNotify(
	 const CGlobalId&	/*inContextId*/,
	 const CGlobalId&	/*inSourceId*/,
	 UInt64				/*inTimeStamp*/,
	 PeekSeverity		/*inSeverity*/,
	 const CPeekString& /*inShortMessage*/,
	 const CPeekString& /*inLongMessage*/ )
{
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnProcessPacket
//		: Implements IProcessPacket::ProcessPacket
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnProcessPacket(
	CPacket&	/*inPacket*/ )
{
	return PEEK_PLUGIN_SUCCESS;
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

	// Create and Load the appropriate Message object.
	CPeekMessagePtr	spMessage = MessageFactory( inMessage );
	if ( spMessage.get() != nullptr ) {
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

	return nResult;
}
#endif // IMPLEMENT_PLUGINMESSAGE


// -----------------------------------------------------------------------------
//		OnProcessTime
//		: Implements IProcessTime::ProcessTime
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnProcessTime(
	UInt64	/*inCurrentTime*/ )
{
	try {
		if ( m_Options.IsSaveOrphans() ) {
			CheckForOrphans();
			CheckSaveOrphans();
		}
		else {
			CheckForOrphans();
		}
	}
	catch ( ... ) {
		m_bFailure = true;
	}

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnReset
//		: Implements IResetProcessing::ResetProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnReset()
{
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnResetCapture
//		: Implements IPacketProcessorEvents::OnResetPacketProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnResetCapture()
{
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnStartCapture
//		: Implements IPacketProcessorEvents::OnStartPacketProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnStartCapture(
	UInt64	/*inStartTime*/ )
{
	DEBUG_LOG_ERROR(L"Start Capture");

	// let's say that OnProcessTime and OnSummary are disabled until
	// they actually get called.
	ResetMonitorMode();

  #ifdef DEBUG_STATS
	m_nCmiPacketNumber			= 0;
	m_nInputPacketNumber		= 0;
	m_nNonSpuriousTestPacketNum	= 0;
	m_nOrphanFiles				= 0;
	m_nOrphans					= 0;
	m_nPacketsCreated			= 0;
	m_nPacketsDeleted			= 0;
	m_nPacketsDeletedInProcess	= 0;
	m_nPacketsDeletedInInsert	= 0;
	m_nInsertErrors				= 0;
	m_nPackets					= 0;
	m_nIPIDOrphans				= 0;
  #endif

	m_nLastSaveTime = GetCurrentPeekTime();

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnStopCapture
//		: Implements IPacketProcessorEvents::OnStopPacketProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnStopCapture(
	UInt64	/*inStopTime*/ )
{
	DEBUG_LOG_ERROR( L"Stop Capture" );

	ClearPackets();

	return PEEK_PLUGIN_SUCCESS;
}


#ifdef IMPLEMENT_ALL_UPDATESUMMARY
// -----------------------------------------------------------------------------
//		OnSummary
//		: Implements IUpdateSummaryStats::UpdateSummaryStats
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnSummary(
#ifdef IMPLEMENT_UPDATESUMMARYSTATS
	CSnapshot&	/*inSnapshot*/ )
#else
	)
#endif
{
	if ( !IsCapturing() ) {
		OnProcessTime( GetCurrentPeekTime() );
	}

	if ( m_bFailure ) {
		m_bFailure = false;
		CPeekOutString	strMessage;
		strMessage << L"CMIFilterOE: unhandled exception " << GetCaptureName();
		LogError( strMessage );
	}

#ifdef DEBUG_STATS
	DoSummaryModifyEntry( L"CMI Packets", L"CMI",
		(kSummaryType_PacketCount | kSummarySize_UInt64),
		&m_nPackets );

	DoSummaryModifyEntry( L"CMI Orphans", L"CMI",
		(kSummaryType_PacketCount | kSummarySize_UInt64),
		&m_nOrphans );

	DoSummaryModifyEntry( L"CMI Files", L"CMI",
		(kSummaryType_PacketCount | kSummarySize_UInt64),
		&m_nOrphanFiles );

	DoSummaryModifyEntry( L"CMI Packet Object Created", L"CMI",
		(kSummaryType_PacketCount | kSummarySize_UInt64),
		&m_nPacketsCreated );

	DoSummaryModifyEntry( L"CMI Packet Objects Deleted", L"CMI",
		(kSummaryType_PacketCount | kSummarySize_UInt64),
		&m_nPacketsDeleted );

	DoSummaryModifyEntry( L"CMI Packet Objects Deleted In Process", L"CMI",
		(kSummaryType_PacketCount | kSummarySize_UInt64),
		&m_nPacketsDeletedInProcess );

	DoSummaryModifyEntry( L"CMI Packet Objects Deleted In Insert", L"CMI",
		(kSummaryType_PacketCount | kSummarySize_UInt64),
		&m_nPacketsDeletedInInsert );

	DoSummaryModifyEntry( L"CMI Packet Objects Deleted by IP ID", L"CMI",
		(kSummaryType_PacketCount | kSummarySize_UInt64),
		&m_nIPIDOrphans );

	DoSummaryModifyEntry( L"CMI Packet Insert Errors", L"CMI",
		(kSummaryType_PacketCount | kSummarySize_UInt64),
		&m_nInsertErrors );

	size_t	nFragMapCount = m_FragmentMap.size();
	UInt32	nDeepFragCount = static_cast<UInt32>( m_FragmentMap.GetDeepCount() );
	DoSummaryModifyEntry( L"Fragment Map Count", L"CMI",
		(kSummaryType_PacketCount | kSummarySize_UInt32),
		&nFragMapCount );
	DoSummaryModifyEntry( L"Fragment Count", L"CMI",
		(kSummaryType_PacketCount | kSummarySize_UInt32),
		&nDeepFragCount );

	size_t	nOrphanCount( m_Orphans.size() );
	DoSummaryModifyEntry( L"Orphan List Count", L"CMI",
		(kSummaryType_PacketCount | kSummarySize_UInt32),
		&nOrphanCount );
  #endif // DEBUG_STATS

	return PEEK_PLUGIN_SUCCESS;
}
#endif // IMPLEMENT_ALL_UPDATESUMMARY


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


// -----------------------------------------------------------------------------
//		BuildNewPacket
// -----------------------------------------------------------------------------

bool
COmniEngineContext::BuildNewPacket(
	COptions&				inOptions,
	CCmiIpV4PacketListPtr&	inPacketList,
	std::vector<UInt8>&		inPayload,
	CCmiPacketPtr&			outPacket )
{
	if ( inPacketList == nullptr ) throw( E_FAIL );
	if ( inPacketList->empty() ) return false;

	// Calculate the size of the new packet.
	UInt16	nNewSize = static_cast<UInt16>( inPayload.size() ) + 4;
	UInt16	nPadding = 0;

	// If Insert MAC is enabled add the Ethernet Header length.
	if ( inOptions.IsInsertMAC() ) {
		nNewSize += sizeof( tEthernetHeader );
	}

	// If VLAN is enabled add the VLAN Header length.
	if ( inOptions.IsInsertVlan() ) {
		nNewSize += sizeof( tVLANHeader );
	}

	// Adjust the packet size for runts.
	if ( nNewSize < 64 ) {
		nPadding = 64 - nNewSize;
		nNewSize = 64;
	}

	// Get the first packet from the packet list.
	CCmiIpV4PacketPtr& upTemplatePacket( inPacketList->front() );
	if ( upTemplatePacket.get() == nullptr ) return false;

	CCmiPacketPtr	upPacket( new CCmiPacket );
	_ASSERTE( upPacket );
	if ( !upPacket ) Peek::Throw( HE_E_OUT_OF_MEMORY );

	// Allocate the space for the Packet Data and get it's address.
	_ASSERTE( !upPacket->HasData() );
	UInt8*	pPacketData( upPacket->CmiAllocFrom( upTemplatePacket, nNewSize ) );
	_ASSERTE( pPacketData );
	_ASSERTE( !upPacket->HasData() );

	size_t	nNewAvailable = nNewSize;

	size_t			nInDataSize( inPayload.size() );
	const UInt8*	pInData( inPayload.data() );

	_ASSERTE( nNewAvailable > sizeof( tEthernetHeader ) );
	if ( nNewAvailable < sizeof( tEthernetHeader ) ) return false;

	tEthernetHeader*	pEthernetHeader( reinterpret_cast<tEthernetHeader*>( pPacketData ) );
	if ( inOptions.IsInsertMAC() ) {
		inOptions.GetDstEthernet().Duplicate( 6, reinterpret_cast<UInt8*>( &pEthernetHeader->Destination ) );
		inOptions.GetSrcEthernet().Duplicate( 6, reinterpret_cast<UInt8*>( &pEthernetHeader->Source ) );
		UInt16	nProtocolType = (inOptions.IsMACProtocolIp())
			? (((*pInData & 0xF0) == 0x60) ? 0x86DD : 0x0800)	// IPv6 else IPv4
			: inOptions.GetMACProtocolType();
		pEthernetHeader->ProtocolType = HOSTTONETWORK16( nProtocolType );
	}
	else {
		if ( nInDataSize < sizeof( tEthernetHeader ) ) return false;

		memcpy( pPacketData, pInData, sizeof( tEthernetHeader ) );
		pInData += sizeof( tEthernetHeader );
		nInDataSize -= sizeof( tEthernetHeader );
	}
	pPacketData += sizeof( tEthernetHeader );
	nNewAvailable -= sizeof( tEthernetHeader );

	if ( inOptions.IsInsertVlan() ) {
		_ASSERTE( nNewAvailable > sizeof( tVLANHeader ) );
		if ( nNewAvailable < sizeof( tVLANHeader ) ) return false;

		// Clear the 4 byte VLAN Header
		tVLANHeader*	pVLanHeader( reinterpret_cast<tVLANHeader*>( pPacketData ) );

		// Set the VLAN Header Protocol Type
		pVLanHeader->nProtocolType = pEthernetHeader->ProtocolType;

		// Set the VLAN ID
		UInt16	nVLANID = inOptions.GetVlanId();
		pVLanHeader->nVlanTag = HOSTTONETWORK16( nVLANID );

		// Set the MAC Header Protocol Type to VLAN
		UInt16	nProtocolType( 0x8100 );
		pEthernetHeader->ProtocolType = HOSTTONETWORK16( nProtocolType );

		pPacketData += sizeof( tVLANHeader );
		nNewAvailable -= sizeof( tVLANHeader );
	}

	// Copy the reassembled data.
	_ASSERTE( nNewAvailable >= nInDataSize );
	if ( nNewAvailable < nInDataSize ) return false;

	memcpy( pPacketData, pInData, nInDataSize );
	pPacketData += nInDataSize;
	nNewAvailable -= nInDataSize;

	if ( nPadding > 0 ) {
		_ASSERTE( nNewAvailable >= nPadding );
		if ( nNewAvailable < nPadding ) return false;

		memset( pPacketData, 0, nPadding );
		pPacketData += nPadding;
	}

	// Clear out the FCS
	_ASSERTE( nNewAvailable >= sizeof( UInt32 ) );
	if ( nNewAvailable < sizeof( UInt32 ) ) return false;

	*(reinterpret_cast<UInt32*>( pPacketData )) = 0;
#ifdef _DEBUG
	pPacketData += sizeof( UInt32 );
	pPacketData = nullptr;
#endif

	// Create and Initialize the new Packet.
	if ( !upPacket->Initialize() ) return false;

	outPacket = std::move( upPacket );

  #ifdef DEBUG_STATS
	m_nCmiPacketNumber++;
  #endif

	return true;
}



// -----------------------------------------------------------------------------
//		CheckForOrphans
// -----------------------------------------------------------------------------

void
COmniEngineContext::CheckForOrphans()
{
	if ( m_FragmentMap.empty() ) return;

	vector<UInt64>	vEraseFragment;
	vector<UInt32>	vEraseAddress;

	UInt64		nDuration = m_Options.GetFragmentAge() * WPTIME_SECONDS;
	UInt64		nTimeOut = GetCurrentPeekTime() - nDuration;
	for ( auto itr = m_FragmentMap.begin(); itr != m_FragmentMap.end(); ++itr ) {
		UInt64					nPacketKey = itr->first;
		CCmiIpV4PacketListPtr&	upPacketList = itr->second;
		if ( (upPacketList.get() == nullptr) || (upPacketList->empty()) ) {
			vEraseFragment.push_back( nPacketKey );
			continue;
		}

		CCmiIpV4PacketPtr& upLastPacket( upPacketList->back() );
		_ASSERTE( upLastPacket );
		if ( upLastPacket.get() == nullptr ) continue;

		// Determine whether to expire the Packet List.
		if ( upLastPacket->GetTimeStamp() < nTimeOut ) {
			// Remove the list from the Fragment Map.
			vEraseFragment.push_back( nPacketKey );

			if ( m_Options.IsSaveOrphans() ) {
				// Move all the packets from this list into the Orphans list.
				std::move( upPacketList->begin(), upPacketList->end(), m_Orphans.end() );
			}
		}
	}

	for ( auto itr = vEraseFragment.begin(); itr != vEraseFragment.end(); ++itr ) {
		m_FragmentMap.erase( *itr );
	}
}


// -----------------------------------------------------------------------------
//		CheckSaveOrphans
// -----------------------------------------------------------------------------

void
COmniEngineContext::CheckSaveOrphans()
{
	UInt64	nNow( GetCurrentPeekTime() );
	UInt64	nDuration = m_Options.GetSaveInterval() * WPTIME_SECONDS;
	UInt64	nExpireTime = m_nLastSaveTime + nDuration;
	if ( nNow > nExpireTime ) {
		if ( SaveOrphans() ) {
			m_Orphans.clear();
		}
		m_nLastSaveTime = nNow;
	}
}


// -----------------------------------------------------------------------------
//		ClearPackets
// -----------------------------------------------------------------------------

void
COmniEngineContext::ClearPackets()
{
	ClearMaps();
	m_Orphans.clear();
}


// -----------------------------------------------------------------------------
//		InsertPackets
// -----------------------------------------------------------------------------

void
COmniEngineContext::InsertPackets(
	CCmiPacketPtrList&	inPacketList )
{
	try {
		// Insertion Flag.  This will allow the packet to be returned
		// as accepted at top of the OnFilter function, which is recursively 
		// called from the DoInsertPacket call.
		SetInserting( true );

		while ( !inPacketList.empty() ) {
			CCmiPacketPtr	upPacket( std::move( inPacketList.back() ) );
			inPacketList.pop_back();

			try {
				// Route the accepted packet to the capture buffer
				DoInsertPacket( *upPacket );
			}
			catch( ... ) {
				X_DEBUG_INCR( m_nInsertErrors );
				// TODO: this packet will probably fail next time...
				// TODO: All the packets are destroyed on exit from the caller.
				inPacketList.push_back( std::move( upPacket ) );
				break;
			}

			// Increment the counters if so specified
			X_DEBUG_INCR( m_nPackets );
			X_DEBUG_INCR( m_nPacketsDeleted );
			X_DEBUG_INCR( m_nPacketsDeletedInInsert );
		}
	}
	catch ( ... ) {
		m_bFailure = true;
	}

	// Turn off the insertion flag
	SetInserting( false );
}


// -----------------------------------------------------------------------------
//		LogMessage
// -----------------------------------------------------------------------------

void
COmniEngineContext::LogMessage(
	CPeekString	inMessage )
{
	ASSERT( m_pPlugin );
	if ( !m_pPlugin ) return;

	const CRemotePlugin&	refRemote( GetRemotePlugin() );
	CAppConfig				AppConfig( refRemote.GetPeekProxy().GetAppConfig() );
	if ( AppConfig.IsNotValid() ) return;

	CPeekString	strDirectory;
	// TODO determine path for Linux
#if NDEBUG
	strDirectory = AppConfig.GetConfigDir();
#else
	strDirectory = AppConfig.GetLogDir();
#endif
	if ( strDirectory.empty() ) return;

	FilePath::Path	path( strDirectory );

#ifdef _WIN32
	CPeekString		pathDir( path.GetLastDir() );
	pathDir.MakeLower();
	if ( pathDir == L"omniengine" ) {
		path.PopDir( true );
	}
#endif // _WIN32

	path.Append( L"Plugins", true );
	CFileEx::MakePath( path.get() );

	path.SetFileName( kLogFilename );

	// Write the message.
	CFileEx		theFile;
	if ( theFile.Open( path.get(), (CFileEx::modeCreate | CFileEx::modeNoTruncate | CFileEx::modeWrite) ) ) {
		theFile.SeekToEnd();
		// Format the message.
		CPeekStringA inMessageA( inMessage );

		CPeekTimeLocal	pkLocal( true );
		CPeekOutStringA	strOutMsg;
		strOutMsg << pkLocal.FormatDateA() << " " << pkLocal.FormatTimeA() << ": " << inMessageA << "\r\n";
		
		CPeekStringA strMessage( strOutMsg );
		theFile.Write( static_cast<UInt32>( strMessage.GetLength() ), strMessage.Format() ); 
	}
}


// -----------------------------------------------------------------------------
//		ProcessPacket
// -----------------------------------------------------------------------------

void
COmniEngineContext::ProcessPacket(
	COptions&			inOptions,
	CCmiIpV4PacketPtr&	inPacket,
	CCmiPacketPtrList&	outNewPacketList )
{
	DEBUG_LOG_ERROR( L"ProcessPacket: Enter" );

	UInt64	nPacketKey( inPacket->GetPacketKey() );
	CCmiIpV4PacketListPtr&	upPacketList( m_FragmentMap[nPacketKey] );
	if ( upPacketList == nullptr ) {
		m_FragmentMap[nPacketKey] = CCmiIpV4PacketListPtr( new CCmiIpV4PacketList() );
	}

	upPacketList->Add( inPacket );	// inPacket becomes nullptr.

	// If all the packets have been received, then reassemble them
	// into a complete packet
	if ( upPacketList->IsComplete() ) {
		std::vector<UInt8>	baPayload;
		DEBUG_LOG_ERROR( L"ProcessPacket: Reassemble" );
		if ( upPacketList->Reassemble( baPayload ) ) {
			CCmiPacketPtr upNewPacket;
			DEBUG_LOG_ERROR( L"ProcessPacket: BuildNewPacket" );
			if ( !BuildNewPacket( inOptions, upPacketList, baPayload, upNewPacket ) ) {
				return;	// The packets will eventually end up on the Orphan list.
			}
			outNewPacketList.push_back( std::move( upNewPacket ) );	// upNewPacket becomes nullptr.
		}
		m_FragmentMap.erase( nPacketKey );
	}

	DEBUG_LOG_ERROR( L"ProcessPacket: Exit" );
}


// ----------------------------------------------------------------------------
//		Save
// ----------------------------------------------------------------------------

int
COmniEngineContext::Save(
	const CPeekString&			inFilePath,
	const CCmiIpV4PacketList&	inPacketList,
	UInt32&						outFileSize )
{
	const int kGenericError( -1 );

	try {
		// Create and open the file.
		CFileEx	exFile( inFilePath, (CFileEx::modeCreate || CFileEx::modeWrite) );
		if ( !exFile.IsOpen() ) {
			PathT<CPeekString> thePath( inFilePath );

			CPeekString strPath( thePath.GetDir() + thePath.GetDrive() );

			bool bPathSuccess = CFileEx::MakePath( strPath );
			if ( !bPathSuccess ) {
				CPeekString	strErrorMsg( L"Unable to create folder \"" + strPath + 
					L"\" for orphan packet saving" );
				LogError( strErrorMsg, true );
				return kGenericError;
			}

			exFile.Open( inFilePath, (CFileEx::modeCreate || CFileEx::modeWrite) );
		}

		// Skip past the header.
		exFile.Seek( sizeof( PeekFileHeader ), CFileEx::SeekPosition::begin );

		// Write each packet to file.
		UInt32	nPackets = 0;
		for ( auto itr( inPacketList.begin() ); itr != inPacketList.end(); ++itr ) {
			// Byte swap the packet header while writing it out.
			PeekPacket*		packetHeader( (*itr)->GetPacketHeader() );
			PeekPacket7		thePacketHeader;

			thePacketHeader.fProtoSpec = 0;
			thePacketHeader.fFlags = static_cast<UInt8>( packetHeader->fFlags & 0x000000FF );	// Potentially losing info here.
			thePacketHeader.fStatus = static_cast<UInt8>( packetHeader->fStatus & 0x000000FF );	// Potentially losing info here.
			thePacketHeader.fPacketLength = HOSTTOBIG16( packetHeader->fPacketLength );
			thePacketHeader.fSliceLength = HOSTTOBIG16( packetHeader->fSliceLength );

			// TODO: verify
			//thePacketHeader.fTimeStamp = TimeConv::PeekToMicroPeek( packetHeader.fTimeStamp );
			//thePacketHeader.fTimeStamp = HOSTTOBIG64( thePacketHeader.fTimeStamp );
			UInt64	nLocalTime( CPeekTime::ToLocal( packetHeader->fTimeStamp ) );
			nLocalTime /= WPTIME_MICROSECONDS;
			thePacketHeader.fTimeStamp = HOSTTOBIG64( nLocalTime );

			// Write the packet header.
			size_t	nBytesWritten( exFile.Write( sizeof( PeekPacket7 ), &thePacketHeader ) );
			if ( nBytesWritten != sizeof( PeekPacket7 ) ) {
				exFile.Delete();
				return kGenericError;
			}

			// Calculate the number of bytes to be written and number 
			// of bytes to pad at the end (usually FCS).
			const UInt32	nPacketLength = (*itr)->GetPacketLength();
			CPeekStream		DataStream = (*itr)->GetPacketData();
			const UInt32	nBytesToWrite = static_cast<UInt32>( DataStream.GetLength() );
			UInt32			nNumPaddingBytes = 4;  // FCS length is 4

			if ( nPacketLength > nBytesToWrite ) {
				nNumPaddingBytes = nPacketLength - nBytesToWrite;
			}
			if ( nBytesToWrite + nNumPaddingBytes < 64 ) {
				nNumPaddingBytes = 64 - nBytesToWrite;
			}
			if ( nBytesToWrite > nPacketLength ) {
				ASSERT( 0 );
			}
			ASSERT( (nBytesToWrite + nNumPaddingBytes) >= 64 );
			if ( nNumPaddingBytes < 4 ) {
				ASSERT( 0 );
				nNumPaddingBytes = 4;
			}
			if ( (nBytesToWrite % 2) != 0 ) {
				nNumPaddingBytes++;
			}

			// Write the packet data.
			nBytesWritten = exFile.Write( nBytesToWrite, DataStream.GetData() );
			if ( nBytesWritten != nBytesToWrite ) {
				exFile.Delete();
				return kGenericError;
			}

			// Pad the result, is usually FCS
			UInt8* Padding = static_cast<UInt8*>( malloc( nNumPaddingBytes ) );
			if ( !Padding ) Peek::Throw( HE_E_OUT_OF_MEMORY );
			memset( Padding, 0, static_cast<size_t>( nNumPaddingBytes ) );

			nBytesWritten = exFile.Write( nNumPaddingBytes, &Padding );
			free ( Padding );
			if ( nBytesWritten != nNumPaddingBytes ) {
				exFile.Delete();
				return kGenericError;
			}

			// Increment the number of packets written.
			nPackets++;
		}

		// Get the file size
		UInt32	nFileSize( static_cast<UInt32>( exFile.GetLength() ) );

		// Back up and fill in the header length and count fields.
		PeekFileHeader	theFileHeader;
		memset( &theFileHeader, 0, sizeof( theFileHeader ) );
		theFileHeader.fVersion = kPacketFile_Version7;

		theFileHeader.fLength = HOSTTOBIG32( nFileSize );
		theFileHeader.fPacketCount = HOSTTOBIG32( nPackets );
		theFileHeader.fTimeDate = HOSTTOBIG32( TimeConv::AnsiToMac( GetCurrentPeekTime() ) );
		theFileHeader.fTimeStart = 0;
		theFileHeader.fTimeStop = 0;
		theFileHeader.fMediaType = HOSTTOBIG32( static_cast<UInt32>( kMediaType_802_3 ) );
		theFileHeader.fMediaSubType = HOSTTOBIG32( static_cast<UInt32>( kMediaSubType_Native ) );

#if (0)
		// TODO
		// Set up the version field.
		ENV_VERSION	FileVersion, ProductVersion;
		if ( g_Environment.GetModuleVersion( FileVersion, ProductVersion ) ) {
			UInt8*	pAppVers = (UInt8*) &theFileHeader.fAppVers;
			pAppVers[3] = (UInt8) FileVersion.MajorVer;
			pAppVers[2] = (UInt8) FileVersion.MinorVer;
			pAppVers[1] = (UInt8) FileVersion.MajorRev;
			pAppVers[0] = (UInt8) FileVersion.MinorRev;
		}
#endif

		// Set up the link speed field.
		theFileHeader.fLinkSpeed = 0;

		// Write the file header.
		exFile.Seek( 0, CFileEx::begin );

		size_t	nBytesWritten = exFile.Write( sizeof( theFileHeader ), &theFileHeader );
		if ( nBytesWritten != sizeof( theFileHeader ) ) {
			exFile.Delete();
			return -1;
		}

		// Close the file handle.
		exFile.Close();

		outFileSize = nFileSize;
	}
	catch (...) {
		return kGenericError;
	}

	return ERROR_SUCCESS;
}


// ----------------------------------------------------------------------------
//		SaveOrphans
// ----------------------------------------------------------------------------

bool
COmniEngineContext::SaveOrphans()
{
	if ( m_Orphans.empty() ) return false;

	// Validate the file name syntax:
	CPeekString strFileName( m_Options.GetFileName() );
	CPeekString	strErrorText;
	bool		bFormatError( false );

	if ( strFileName.GetLength() == 0 ) {
		bFormatError = true;
		strErrorText = L"Destination file path for orphan packet saving is null.";	
	}
	else {
		if ( !CFileEx::IsLegalFilePath( strFileName ) ) {
			bFormatError = true;
			CPeekOutString strOutMessage;
			strOutMessage << L"Illegal file path for orphan packet saving: \"" << strFileName << L"\""; 

			strErrorText = strOutMessage;
		}
	}

	if ( bFormatError ) {
		LogError( strErrorText );
		return false;
	}

	PathT<CPeekString> theFilePath( strFileName );

	// Check for an extension
	CPeekString theExtension( theFilePath.GetExtension() );
	bool		bAddExtension( false );
	if ( theExtension.IsEmpty() ) {
		bAddExtension = true;
	}

	CPeekString		strFileStem( theFilePath.GetFileStem() );
	strFileStem.Trim();
	strFileStem += L" ";
	strFileStem += CPeekTime::GetTimeStringFileName();

	if ( bAddExtension ) {
		theFilePath.SetExtension( L".pkt" );
	}

	theFilePath.SetFileStem( strFileStem );

	// Save the packets to a file
	UInt32	nFileSize( 0 );
	UInt32	nResult( Save( theFilePath.get(), m_Orphans, nFileSize ) );
	if ( nResult == 0 ) {
		X_DEBUG_INCR( m_nOrphanFiles );
	}
	else {
		CPeekOutString	strErrorMesssage;
		strErrorMesssage << L"Error saving orphan fragments to file \"" << theFilePath.get();
		LogError( strErrorMesssage, true );

		m_Orphans.clear();

		return false;
	}

	return true;
}


// ----------------------------------------------------------------------------
//		LogError
// ----------------------------------------------------------------------------

void
COmniEngineContext::LogError(
	CPeekString inErrorMessage,
	bool		inFormatMessage )
{
	CPeekString	strMessage;

#ifdef _WIN32
	if ( inFormatMessage ) {
		UInt32	dwLastError = ::GetLastError(); 
		TCHAR	szErrorString[256] = L"unknown error";
		if(dwLastError != 0) {
			::FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, nullptr, dwLastError, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
				szErrorString, sizeof(szErrorString) - 1, nullptr ); 
		}
		CPeekOutString	strMsg;

		strMsg << inErrorMessage << L"\" - error is \"" << szErrorString << L"\"";

		strMessage = strMsg;
	}
	else {
		strMessage = inErrorMessage;
	}
#else
	// TODO
	(void)inFormatMessage;
	strMessage = inErrorMessage;
#endif
	
	LogMessage( strMessage );
	DoLogMessage( strMessage );
}
