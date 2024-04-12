// =============================================================================
//	SampleParser.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "SampleParser.h"
#include "MemUtil.h"
#include "TimeConv.h"
#include <NetflowHeaders.h>

using namespace std;
using namespace TimeConv;

// Use local timestamp for aggregation
bool	IsGetTimestamp() { return true; }


// =============================================================================
//		CSampleParser
// =============================================================================

// -----------------------------------------------------------------------------
//		ProcessBuffer
// -----------------------------------------------------------------------------

void
CSampleParser::ProcessBuffer(
	UInt8*	pBuffer,
	UInt32	nLength,
	UInt32	nRecvAddress )
{
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
#if 0
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
#endif

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

	m_heCritSection.Lock();

	m_DispatchPacket.Initialize( NULL, 0, &thePeekPacket, (pBuffer + kHeaderSize),
		kMediaType_802_3, kMediaSubType_802_11_gen );
	 
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

	m_heCritSection.Unlock();
}


// -----------------------------------------------------------------------------
//		SetCaptureSrcIP
// -----------------------------------------------------------------------------

void
CSampleParser::SetCaptureSrcIP(
	const Helium::HeID& capId,
	UInt32				nSrcIP )
{
	m_heCritSection.Lock();

	map<HeID, UInt32>::iterator mapIter;
	
	mapIter = m_mapCapIdToIp.find( capId );

	if( mapIter != m_mapCapIdToIp.end() ) {
		mapIter->second = nSrcIP;
	}
	else {
		m_mapCapIdToIp.insert( make_pair( capId, nSrcIP ) );
	}

	m_heCritSection.Unlock();
}


// -----------------------------------------------------------------------------
//		GetCaptureSrcIP
// -----------------------------------------------------------------------------

void
CSampleParser::GetCaptureSrcIP(
	const Helium::HeID& capId,
	UInt32*				pSrcIP )
{
	ASSERT( pSrcIP );
	if ( pSrcIP == NULL ) return;

	m_heCritSection.Lock();

	map<HeID, UInt32>::iterator mapIter;
	mapIter = m_mapCapIdToIp.find( capId );

	if ( mapIter != m_mapCapIdToIp.end() ) {
		*pSrcIP = mapIter->second;
	}
	else {
		*pSrcIP = 0;
	}

	m_heCritSection.Unlock();
}


// -----------------------------------------------------------------------------
//		RemoveCaptureSrcIP
// -----------------------------------------------------------------------------

void
CSampleParser::RemoveCaptureSrcIP(
	const Helium::HeID& capId )
{
	m_heCritSection.Lock();

	map<HeID, UInt32>::iterator mapIter;
	mapIter = m_mapCapIdToIp.find( capId );

	if ( mapIter != m_mapCapIdToIp.end() ) {
		m_mapCapIdToIp.erase( mapIter );
	}	

	m_heCritSection.Unlock();
}
