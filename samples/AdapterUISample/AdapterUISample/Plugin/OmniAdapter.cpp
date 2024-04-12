// =============================================================================
//	OmniAdapter.cpp
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniAdapter.h"
#include "PeekTime.h"
#include "PeekMessage.h"
#include "PluginHandlersInterface.h"
#include "PluginMessages.h"
#include "FileEx.h"
#include "ixmldom.h"
#include "XmlUtil.h"
#include <map>

#ifdef _WIN32
#include "resource.h"
#endif // _WIN32

using std::map;
using std::string;
using std::wstring;
using namespace HeLib;

class CRemoteAdapter;


// =============================================================================
//		Globals and Constants
// =============================================================================

#define kPluginName				L"AdapterUI Sample"
#define kAdapterName			L"My_Adapter"
#define kAdapterIdentifier		L"AdapterUISample"

#define kTag_ConfigFileName		kAdapterName L".xml"
#define kTag_Options			L"Options"
#define kTag_Port				L"Port"

static const GUID		g_guidHTMLHandler = HTMLPreferencesHandler_ID;
static const CGlobalId	g_idHTMLHandler( g_guidHTMLHandler );

#ifdef _WIN32
int				COmniAdapter::s_nOptionsId( IDR_ZIP_OPTIONS );
#endif // _WIN32

static const UInt8	s_PacketData[] = {
	0x08, 0x00, 0x07, 0x57, 0xA9, 0xB6, 0x00, 0x00, 0x0C, 0x5D, 0x10, 0x46, 0x08, 0x00, 0x45, 0x00,
	0x00, 0x2C, 0xE1, 0x83, 0x00, 0x00, 0x34, 0x06, 0xAF, 0xFF, 0xCE, 0xDD, 0xE9, 0x66, 0xC0, 0xD8,
	0x7C, 0x2C, 0x00, 0x50, 0x06, 0xB0, 0x3D, 0x9D, 0x85, 0xF7, 0xE2, 0xFF, 0xB8, 0x01, 0x60, 0x12,
	0x7C, 0x00, 0xC1, 0x36, 0x00, 0x00, 0x02, 0x04, 0x05, 0xB4, 0x00, 0x00, 0xE8, 0xCE, 0xB2, 0xE8
};


// =============================================================================
//		COmniAdapter
// =============================================================================

UInt64	COmniAdapter::m_dbgPacketNumber( 0 );

COmniAdapter::COmniAdapter(
	CRemoteAdapter*	inRemoteAdapter )
	:	CPeekAdapter( inRemoteAdapter )
	,	m_strAdapterName( kAdapterName )
	,	m_nLinkSpeed( 1000000000 )
	,	m_nRunningCaptures( 0 )
	,	m_nPacketCount( 0 )
	,	m_nDroppedPackets( 0 )
{
}

COmniAdapter::~COmniAdapter()
{
}


// -----------------------------------------------------------------------------
//		AddPacketHandler
// -----------------------------------------------------------------------------

void
COmniAdapter::AddPacketHandler(
	CPacketHandler inHandler )
{
	PeekLock lock( m_Lock );
	m_ayPacketHandlers.AddUnique( inHandler );
}


#if (0)
// -----------------------------------------------------------------------------
//		GetDataFileFullName
// -----------------------------------------------------------------------------

CPeekString
COmniAdapter::GetDataFileFullName()
{
	CPeekString	strDataFileFullName( m_strPluginPath );
	if ( !strDataFileFullName.IsEmpty() ) {
		strDataFileFullName += kTag_ConfigFileName;
	}
	return strDataFileFullName;
}
#endif // 0


// -----------------------------------------------------------------------------
//		Initialize
// -----------------------------------------------------------------------------

bool
COmniAdapter::Initialize()
{
	m_nDroppedPackets = 0;

	// Create a file buffer.
	CPersistFile	pfFilePacketBuffer;
	pfFilePacketBuffer.Create();
	if ( pfFilePacketBuffer.IsNotValid() ) return false;

	// Get the packet buffer interface.
	m_PacketBuffer = pfFilePacketBuffer;
	if ( m_PacketBuffer.IsNotValid() ) return false;

	m_PacketBuffer.SetMediaType( GetMediaType() );
	m_PacketBuffer.SetLinkSpeed( GetLinkSpeed() );
	SetPacketCount( m_PacketBuffer.GetCount() );

	m_MediaInfo = m_PacketBuffer.GetMediaInfo();
	m_MediaInfo.SetMediaType( m_MediaType );
	m_MediaInfo.SetLinkSpeed( GetLinkSpeed() );

	m_AdapterInfo.Create();
	if ( m_AdapterInfo.IsNotValid() ) return false;

	m_AdapterInfo.SetPluginName( kPluginName );
	m_AdapterInfo.SetPersistent( false );	// adapter manager will not save on shutdown (we have to do the work)
	m_AdapterInfo.SetDeletable( true );
	m_AdapterInfo.SetTitle( kPluginName );
	m_AdapterInfo.SetIdentifier( kAdapterIdentifier );
	m_AdapterInfo.SetMediaType( m_MediaType );
	m_AdapterInfo.SetDescription( kPluginName );
	// m_AdapterInfo.SetAdapterFeatures(  PEEK_ADAPTER_QUERY_DROP_AFTER_STOP );
	// m_AdapterInfo.SetAdapterFeatures(  PEEK_ADAPTER_SETS_PER_CAPTURE_DROP ); // Remove m_nDroppedPackets.
	m_AdapterInfo.SetLinkSpeed( m_nLinkSpeed );
	m_AdapterInfo.SetMediaInfo( m_MediaInfo );
	m_AdapterInfo.SetHandlerId( g_idHTMLHandler );

	CHePtr<IMediaInfo> spMediaInfo;
	if ( HE_SUCCEEDED( spMediaInfo.CreateInstance( "PeekCore.MediaInfo" ) ) ) {
		const tMediaType	mt( kMediaType_802_3, kMediaSubType_Native );	// Explicit, but is the default.
		const UInt64		nLinkSpeed( 100000000 );
		spMediaInfo->SetMediaType( mt.fType );
		spMediaInfo->SetMediaSubType( mt.fSubType );
		spMediaInfo->SetMediaDomain( kMediaDomain_Null );
		spMediaInfo->SetLinkSpeed( nLinkSpeed );

		CMediaInfo mi( spMediaInfo );
		m_AdapterInfo.SetMediaInfo( mi );

		spMediaInfo.Release();
	}

	CGlobalId	idAdapter( true );
	m_AdapterInfo.SetIdentifier( idAdapter );

#if (0)
	CPeekString strConfigFile = GetDataFileFullName();
	CHePtr<Xml::IXMLDOMDocument>	spDoc;
	XmlUtil::LoadXmlFile( strConfigFile, &spDoc.p);
	if ( spDoc == nullptr ) return false;

	CHePtr<Xml::IXMLDOMElement> spRootNode;
	spDoc->get_documentElement( &spRootNode.p );
	if ( spRootNode == nullptr ) return false;

	string strFlowPort;
	if ( XmlUtil::GetAttribute( spRootNode, kTag_Port, strFlowPort ) ) {
		CPeekString	strPort( strFlowPort );
		SetPort( strPort );
	}
#endif // 0

	return true;
}


// -----------------------------------------------------------------------------
//		PauseCapture
// -----------------------------------------------------------------------------

void
COmniAdapter::PauseCapture()
{
	PeekAtomicIncrement32( &m_PauseCount );
}


#if (0)
// -----------------------------------------------------------------------------
//		ProcessBuffer
// -----------------------------------------------------------------------------

void
COmniAdapter::ProcessBuffer(
	UInt32				inLength,
	UInt8*				inBuffer,
	const CIpAddress&	inRecvAddress )
{
	inBuffer;
	inLength;
	inRecvAddress;

	// Copy the header so we can change it
	peek_hdr header;
	memcpy( &header, pBuffer, sizeof( peek_hdr ) );

	// byte swap 16 bit values
	header.packet_length	= ntohs( header.packet_length );
	header.slice_length		= ntohs( header.slice_length );
	header.ts.tv_sec		= ntohl( header.ts.tv_sec );
	header.ts.tv_usec		= ntohl( header.ts.tv_usec );

	if ( header.packet_length <= 0 ) return;
	if ( header.packet_length > nLength ) return;

	PeekPacket thePeekPacket;
	memset( &thePeekPacket, 0, sizeof( thePeekPacket ) );

	if ( IsGetTimestamp() ) {
		FileTimeConv nTimeStamp;
		::GetSystemTimeAsFileTime( &nTimeStamp.ft_struct );
		thePeekPacket.fTimeStamp = FileToPeek( nTimeStamp.ft_scalar );
	}
	else {
		UInt64 fOffset = 0;

		// cast time
		struct timeval ts		= (struct timeval)header.ts;

		UInt64 theSec = (UInt64)ts.tv_sec;
		theSec *= 1000000000ull;

		UInt64 theUsec = (UInt64)ts.tv_usec;
		theUsec *= 1000ull;

		UInt64 fThisTime = theSec + theUsec;

		// first time through
		if ( m_fStartTime == 0 ) {
			// get the absolute timestamp quick!
			UInt64 nTimeStamp;
			::GetSystemTimeAsFileTime( (LPFILETIME) &nTimeStamp );

			// convert to Peek time
			m_fStartTime = nTimeStamp * 100ull;

			// remember the time to use next time
			m_fFirstTime = fThisTime;
		}
		else {
			fOffset	= fThisTime - m_fFirstTime;
		}

		// add offset to absolute time
		pluginPacket.fTimeStamp	=  m_fStartTime + fOffset;
	}

	thePeekPacket.fFlags		= header.flags;
	thePeekPacket.fSliceLength	= header.slice_length;
	thePeekPacket.fPacketLength	= header.packet_length;
	thePeekPacket.fFlags		= header.flags;
	thePeekPacket.fStatus		= header.status;
	thePeekPacket.fProtoSpec	= 0;

	MediaSpecificHeaderAll theMediaSpecificHeader;
	memset( &theMediaSpecificHeader, 0, sizeof( theMediaSpecificHeader ) );

	thePeekPacket.fMediaSpecInfoBlock = reinterpret_cast<UInt8*>( &theMediaSpecificHeader );
	theMediaSpecificHeader.StdHeader.nSize = MAX_MEDIASPECIFICHEADER;
	theMediaSpecificHeader.StdHeader.nType = kMediaSpecificHdrType_Wireless3;

	SInt8 nNoisedBm				= header.noise_dBm;
	SInt8 nNoiseStrength		= header.signal_strength;
	SInt8 nSignaldBm			= header.signal_dBm;
	SInt8 nSignalStrength		= nSignaldBm + 95;

	theMediaSpecificHeader.MediaInfo.wireless.Channel.Channel	= header.channel;
	theMediaSpecificHeader.MediaInfo.wireless.DataRate			= header.data_rate;
	theMediaSpecificHeader.MediaInfo.wireless.SignalPercent		= nSignalStrength;
	theMediaSpecificHeader.MediaInfo.wireless.SignaldBm			= nSignaldBm;
	theMediaSpecificHeader.MediaInfo.wireless.NoisePercent		= nNoiseStrength;
	theMediaSpecificHeader.MediaInfo.wireless.NoisedBm			= nNoisedBm;
	theMediaSpecificHeader.MediaInfo.wireless.NoisedBm1			= PEEK_NULL_DBM;
	theMediaSpecificHeader.MediaInfo.wireless.NoisedBm2			= PEEK_NULL_DBM;
	theMediaSpecificHeader.MediaInfo.wireless.NoisedBm3			= PEEK_NULL_DBM;
	theMediaSpecificHeader.MediaInfo.wireless.SignaldBm1		= PEEK_NULL_DBM;
	theMediaSpecificHeader.MediaInfo.wireless.SignaldBm2		= PEEK_NULL_DBM;
	theMediaSpecificHeader.MediaInfo.wireless.SignaldBm3		= PEEK_NULL_DBM;

	int nFreq = 0;
	int nBand = 0;

	switch ( header.channel ) {
		case 1: nFreq = 2412; nBand = PEEK_802_11_BG_BAND; break;
		case 2: nFreq = 2417; nBand = PEEK_802_11_BG_BAND; break;
		case 3: nFreq = 2422; nBand = PEEK_802_11_BG_BAND; break;
		case 4: nFreq = 2427; nBand = PEEK_802_11_BG_BAND; break;
		case 5: nFreq = 2432; nBand = PEEK_802_11_BG_BAND; break;
		case 6: nFreq = 2437; nBand = PEEK_802_11_BG_BAND; break;
		case 7: nFreq = 2442; nBand = PEEK_802_11_BG_BAND; break;
		case 8: nFreq = 2447; nBand = PEEK_802_11_BG_BAND; break;
		case 9: nFreq = 2452; nBand = PEEK_802_11_BG_BAND; break;
		case 10: nFreq = 2457; nBand = PEEK_802_11_BG_BAND; break;
		case 11: nFreq = 2462; nBand = PEEK_802_11_BG_BAND; break;
		case 12: nFreq = 2467; nBand = PEEK_802_11_BG_BAND; break;
		case 13: nFreq = 2472; nBand = PEEK_802_11_BG_BAND; break;
		case 14: nFreq = 2477; nBand = PEEK_802_11_BG_BAND; break;
		case 36: nFreq = 5180; nBand = PEEK_802_11_A_BAND; break;
		case 40: nFreq = 5200; nBand = PEEK_802_11_A_BAND; break;
		case 42: nFreq = 5210; nBand = PEEK_802_11_A_BAND; break;
		case 44: nFreq = 5220; nBand = PEEK_802_11_A_BAND; break;
		case 48: nFreq = 5240; nBand = PEEK_802_11_A_BAND; break;
		case 50: nFreq = 5250; nBand = PEEK_802_11_A_BAND; break;
		case 52: nFreq = 5260; nBand = PEEK_802_11_A_BAND; break;
		case 56: nFreq = 5280; nBand = PEEK_802_11_A_BAND; break;
		case 58: nFreq = 5290; nBand = PEEK_802_11_A_BAND; break;
		case 60: nFreq = 5300; nBand = PEEK_802_11_A_BAND; break;
		case 64: nFreq = 5320; nBand = PEEK_802_11_A_BAND; break;
		case 149: nFreq = 5745; nBand = PEEK_802_11_A_BAND; break;
		case 152: nFreq = 5760; nBand = PEEK_802_11_A_BAND; break;
		case 153: nFreq = 5765; nBand = PEEK_802_11_A_BAND; break;
		case 157: nFreq = 5785; nBand = PEEK_802_11_A_BAND; break;
		case 160: nFreq = 5800; nBand = PEEK_802_11_A_BAND; break;
		case 161: nFreq = 5805; nBand = PEEK_802_11_A_BAND; break;
		case 165: nFreq = 5825; nBand = PEEK_802_11_A_BAND; break;
	}

	theMediaSpecificHeader.MediaInfo.wireless.Channel.Frequency = nFreq;
	theMediaSpecificHeader.MediaInfo.wireless.Channel.Band = nBand;

	PeekAutoLock lock( &m_Lock );

	CPacket	pktDispatch;

	PeekPacket thePeekPacket;
	memset( &thePeekPacket, 0, sizeof( thePeekPacket ) );
	thePeekPacket.fPacketLength = 100;

	// tMediaType	mtPacket; // = { kMediaType_802_3, kMediaSubType_Native };

	pktDispatch.Create( &thePeekPacket, inBuffer, m_MediaType );

	for ( auto i = m_ayPacketHandlers.begin(); i != m_ayPacketHandlers.end(); i++ ) {
		CPeekCapture	theCapture( i->GetPeekCapture() );
		if ( theCapture.IsNotValid() ) continue;

		CGlobalId	idCapture = theCapture.GetId();

		// New Code
		CIpAddress	ipAddress;
		if ( !m_CaptureIdToIpAddress.Find( idCapture, ipAddress ) ) {
			continue;
		}
		if ( ipAddress.IsNull() || (ipAddress == inRecvAddress) ) {
			HRESULT hr = i->HandlePacket( pktDispatch );
			if ( HE_SUCCEEDED( hr ) ) {
				;
			}
		}
	}

	vector< CHePtr<IPacketHandler> >::const_iterator Iter = m_vecPackethandler.begin();
	for ( ; Iter != m_vecPackethandler.end(); Iter++ ) {
		CHePtr<IPeekCapture> spCapture;
		(*Iter)->QueryInterface( IPeekCapture::GetIID(), (void**)&spCapture.p );
		if ( spCapture ) {
			HeID capId;
			spCapture->GetID( &capId );
			if ( capId.IsNull() ) continue;

			map<HeID, UInt32>::iterator mapIter;
			mapIter = m_mapCapIdToIp.find( capId );
			if ( mapIter == m_mapCapIdToIp.end() ) continue;

			if( (mapIter->second == 0) || (mapIter->second == nRecvAddress) ) {
				HRESULT hr = (*Iter)->HandlePacket( &m_DispatchPacket );
				if ( HE_SUCCEEDED( hr ) ) {
					;
				}
			}
		}
	}
}
#endif


// -----------------------------------------------------------------------------
//		RemovePacketHandler
// -----------------------------------------------------------------------------

void
COmniAdapter::RemovePacketHandler(
	CPacketHandler inHandler )
{
	PeekLock lock( m_Lock );
	m_ayPacketHandlers.Remove( inHandler );
}


// -----------------------------------------------------------------------------
//		ResumeCapture
// -----------------------------------------------------------------------------

void
COmniAdapter::ResumeCapture()
{
	if ( PeekAtomicDecrement32( &m_PauseCount ) == 0 ) {
		m_PauseEvent.Set();
	}
}


// -----------------------------------------------------------------------------
//		Run
// -----------------------------------------------------------------------------

int
COmniAdapter::Run(
	ThreadHelpers::Thread* /*pThread*/ )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try {
		CPacket		pktDispatch;

		tMediaType	mtPacket;
		mtPacket.fType = kMediaType_802_3;
		mtPacket.fSubType = kMediaSubType_Native;

		PeekPacket thePeekPacket;
		memset( &thePeekPacket, 0, sizeof( thePeekPacket ) );

		thePeekPacket.fPacketLength = sizeof( s_PacketData );

		// loop until capture stops
		while ( IsCapturing() ) {
			if ( m_PauseCount > 0 ) {
				m_PauseEvent.Wait( 200 );
			}
			else {
				thePeekPacket.fTimeStamp = CPeekTime::Now().i;
				pktDispatch.Create( &thePeekPacket, s_PacketData, m_MediaType );
				for ( auto i = m_ayPacketHandlers.begin(); i != m_ayPacketHandlers.end(); i++ ) {
					if ( !i->HandlePacket( pktDispatch ) ) {
						PeekAtomicIncrement32( &m_PauseCount );
						break;
					}
#if (0)
					// If Per Capture Dropped Packets is supported.
					// It's more common for the adapter to report the dropped packets from
					// GetDroppedPackets()
					else {
						CPeekCapture	capture = i->GetPeekCapture();
						if ( capture.IsValid() ) {
							UInt64	nPacketsDropped;
							if ( capture.GetProperty( L"PacketsDropped", &nPacketsDropped ) ) {
								++nPacketsDropped;
								capture.SetProperty( L"PacketsDropped", nPacketsDropped );
							}
						}
					}
#endif
				}
			}
		}
	}
	catch( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

#if (0)
	try {
		if ( !m_Socket.Bind( m_Port ) ) {
			return PEEK_PLUGIN_FAILURE;
		}

		UInt8	DataBuf[4096];

		// read peek header
		SOCKADDR_IN sockAddr;
		memset( &sockAddr, 0, sizeof( sockAddr ) );
		int nAddrLen = sizeof( sockAddr );

		struct timeval	timeout;  /* Timeout for select */
		fd_set			fdSockets;

		// loop until capture stops
		while ( ::WaitForSingleObject( m_hStopEvent, 0 ) != WAIT_OBJECT_0 ) {
			try {
				timeout.tv_sec = 1;
				timeout.tv_usec = 0;

				// populate fd sockets and find high socket
				FD_ZERO( &fdSockets );
				FD_SET( m_Socket, &fdSockets );

				// MSDN indicates that the first argument (nfds) is ignored.
				int nReadSocket = select(
					0, &fdSockets, (fd_set *) 0, (fd_set *) 0, &timeout );

				if ( nReadSocket < 0 ) {
					continue;	// this is bad
				}
				else if ( nReadSocket == 0 ) {
					continue;	// normal timeout
				}

				int nRecv = ::recvfrom( m_Socket, (char*) DataBuf,
					4096, 0, (SOCKADDR*)&sockAddr, &nAddrLen );

				if ( nRecv == SOCKET_ERROR ) {
					int iError = WSAGetLastError();
					if ( iError == WSAEMSGSIZE ) {
						HE_ASSERT( FALSE );
					}
				}

				CIpAddress	ipRecvAddress( sockAddr.sin_addr.S_un.S_addr );

				m_dbgPacketNumber++;
				AtlTrace( L"Packet received # %d \n", m_dbgPacketNumber );

				BOOL bKeeper = TRUE;
				if ( bKeeper ) {
					ProcessBuffer( nRecv, DataBuf, ipRecvAddress );
				}

			}
			catch (...) {
				::MessageBox( NULL, L"Exception",
					L"Caught an exception the capture loop", MB_OK );
			}
		}

		m_dbgPacketNumber = 0;

		CloseSocket();
	}
	catch( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}
#endif // 0

	return nResult;
}


// -----------------------------------------------------------------------------
//		StartCapture
// -----------------------------------------------------------------------------

int
COmniAdapter::StartCapture()
{
	PeekLock	lock( m_Lock );

	int	nResult = PEEK_PLUGIN_SUCCESS;

	try {
		if ( m_nRunningCaptures == 0 ) {
			m_PauseCount = 0;
			if ( !m_PauseEvent.IsValid() ) {
				if ( !m_PauseEvent.Create() ) {
					return PEEK_PLUGIN_FAILURE;
				}
			}
			if ( !m_CaptureThread.Create( this ) ) {
				return PEEK_PLUGIN_FAILURE;
			}
		}
		m_nRunningCaptures++;
	}
	catch( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		StopCapture
// -----------------------------------------------------------------------------

int
COmniAdapter::StopCapture()
{
	PeekLock	lock( m_Lock );

	int	nResult = PEEK_PLUGIN_SUCCESS;
	try {
		m_nRunningCaptures--;
		if ( m_nRunningCaptures == 0 ) {
			if ( m_PauseEvent.IsValid() ) {
				m_PauseEvent.Set();
			}
			m_CaptureThread.Join();
			m_PauseEvent.Close();
			m_PauseCount = 0;

#if (0)
			CloseSocket();
#endif // 0
		}
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		Terminate
// -----------------------------------------------------------------------------

void
COmniAdapter::Terminate()
{
}


// -----------------------------------------------------------------------------
//		CAdapterStats::GetStatistics
// -----------------------------------------------------------------------------

int
COmniAdapter::GetStatistics(
	IPacketHandler*	pHandler,
	void**			ppStatistics )
{
	if ( ppStatistics == NULL ) return PEEK_PLUGIN_FAILURE;
	*ppStatistics = NULL;

	CPacketHandler	handler( pHandler );

	PeekLock lock( m_Lock );

	CPacketHandlerIndex	nIndex;
	if ( m_ayPacketHandlers.Find( handler, nIndex ) ) {
		*ppStatistics = HeAllocString( L"[Count:100]" );
		if ( ppStatistics == nullptr ) return HE_E_OUT_OF_MEMORY;
	}

	return PEEK_PLUGIN_SUCCESS;
}
