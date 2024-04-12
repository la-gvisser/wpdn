// =============================================================================
//	CaptureFile.cpp
//		implementation of the CCaptureFile class.
// =============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#include "MemUtil.h"
#include "PCapCaptureFile.h"
#include <cassert>
#include <chrono>
#include <filesystem>
#include <thread>

#ifdef _LINUX
#include <unistd.h>
#endif

using MemUtil::SwapIf16;
using MemUtil::SwapIf32;
using std::filesystem::path;
using std::wstring;

// Use (void) to silence unused warnings.
#define assertm(exp, msg)	assert(((void)msg, exp))


namespace CaptureFile {

// ============================================================================
//		CPCapCapture
// ============================================================================

// -----------------------------------------------------------------------------
//      SetLinkType
// -----------------------------------------------------------------------------

void
CPCapCapture::SetLinkType(
	const tMediaType	inType )
{
	switch ( inType.fType ) {
		case kMediaType_802_3:
			switch ( inType.fSubType ) {
				case kMediaSubType_Native:
					m_nLinkType = DLT_EN10MB;
					break;

				case kMediaSubType_802_11_b:
				case kMediaSubType_802_11_a:
				case kMediaSubType_802_11_gen:
					m_nLinkType = DLT_IEEE802_11;
					break;

				default:
					m_nLinkType = 0;
					break;
				}
			break;

		case kMediaType_802_5:
			m_nLinkType = DLT_IEEE802;
			break;

		case kMediaType_Wan:
		case kMediaType_CoWan:
		default:
			throw( 0x8004005 );
			break;
	}
}


// ============================================================================
//		CPCapCaptureReader
// ============================================================================

// -----------------------------------------------------------------------------
//      Open
// -----------------------------------------------------------------------------

bool
CPCapCaptureReader::Open(
	const path&	inPath )
{
	if ( !IsOpen() ) {
		if ( !CCaptureReader::Open( inPath ) ) {
			return false;
		}

		m_nFileFormat = CF_FORMAT_PCAP;
	}

	if ( !Parse() ) return false;

	return IsOpen();
}


// -----------------------------------------------------------------------------
//      Parse
// -----------------------------------------------------------------------------

bool
CPCapCaptureReader::Parse()
{
	if ( !IsOpen() ) return false;

	MoveTo( 0 );
	for ( int i = 0; i < 11; ++i ) {
		if ( GetSize() >= sizeof( TcpDumpFileHeader ) ) {
			break;
		}
		if ( i >= 10 ) return false;
		std::this_thread::sleep_for( std::chrono::microseconds( 1 ) );
	}

	TcpDumpFileHeader	pcapHeader;
	UInt32	dwBytesRead = Read( sizeof( TcpDumpFileHeader ), &pcapHeader );
	if ( dwBytesRead < sizeof( TcpDumpFileHeader ) ) {
		return false;
	}

	switch ( pcapHeader.magic ) {
		case TCPDUMP_MAGIC:
			SetSwapped( false );
			SetVersion( TCPDUMP_VERSION_1 );
			SetTimeStampMultiplier( 1000 );
			break;

		case TCPDUMP_MAGIC_SWAPPED:
			SetSwapped( true );
			SetVersion( TCPDUMP_VERSION_1 );
			SetTimeStampMultiplier( 1000 );
			break;

		case TCPDUMP_MAGIC_1N:
			SetSwapped( false );
			SetVersion( TCPDUMP_VERSION_1 );
			SetTimeStampMultiplier( 1 );
			break;

		case TCPDUMP_MAGIC_1N_SWAPPED:
			SetSwapped( true );
			SetVersion( TCPDUMP_VERSION_1 );
			SetTimeStampMultiplier( 1 );
			break;

		case TCPDUMP_MAGIC_2:
			SetSwapped( false );
			SetVersion( TCPDUMP_VERSION_2 );
			SetTimeStampMultiplier( 1000 );
			break;

		case TCPDUMP_MAGIC_2_SWAPPED:
			SetSwapped( true );
			SetVersion( TCPDUMP_VERSION_2 );
			SetTimeStampMultiplier( 1000 );
			break;

		default:
			return false;
	}

	m_nMajorVersion = SwapIf16( IsSwapped(), pcapHeader.major );
	m_nMinorVersion = SwapIf16( IsSwapped(), pcapHeader.minor );
	m_nZone = SwapIf32( IsSwapped(), pcapHeader.zone );
	m_nSignificantFigures = SwapIf32( IsSwapped(), pcapHeader.sigfigs );
	m_nSnapLength = SwapIf32( IsSwapped(), pcapHeader.snaplen );
	m_nLinkType = SwapIf32( IsSwapped(), pcapHeader.linktype );

	if ( m_nSignificantFigures == 3 ) {
		// Don't adjust low part of time stamp if tcpdump for AIX (v4.0.2).
		SetTimeStampMultiplier( 1 );
	}

	switch ( m_nLinkType ) {
		case DLT_EN10MB:		// 10/100 Ethernet
		case DLT_LINUX_SLL:		// Linux cooked socket
			m_MediaType = tMediaType( kMediaType_802_3, kMediaSubType_Native );
			break;

		case DLT_IEEE802_11:	// 802.11
		case DLT_IEEE802_11_RADIO:
			m_MediaType = tMediaType( kMediaType_802_3, kMediaSubType_802_11_gen );
			break;

		case DLT_FRELAY:
			m_MediaType = tMediaType( kMediaType_Wan, kMediaSubType_wan_frameRelay );
			break;

		case DLT_RAW:
			// Not really Frame Relay, but closest match.
			m_MediaType = tMediaType( kMediaType_Wan, kMediaSubType_wan_frameRelay );
			break;

		default:
			m_MediaType = tMediaType( kMediaType_Max, kMediaSubType_Max );
			break;
	}

	// If Wireless, then set some reasonable defaults:
	// Data Rate = 2
	// Channel = first supported...
	// If WAN, then set some reasonable defaults:
	// Direction = 1
	// Protocol = PEEK_WAN_PROT_FRLY

	SetPacketsPosition( GetPosition() );

	return true;
}


// -----------------------------------------------------------------------------
//      ReadFilePacket
// -----------------------------------------------------------------------------

bool
CPCapCaptureReader::ReadFilePacket(
	CFilePacket&	outPacket,
	UInt32&			outError )
{
	outError = CF_ERROR_NONE;

	CFileRollback	cfRollback( m_File );

	TPCapPacketHeader	pktHeader;
	if ( !ReadPacketHeader( pktHeader, outError ) ) {
		// Assume End of File, but could be an incomplete header.
		return false;
	}

	outPacket.SetDataRate( 0 );
	outPacket.SetMediaType( m_MediaType );
	outPacket.SetTimeStamp( pktHeader.m_nTimeStamp );
	outPacket.SetStatus( 0 );

	UInt32		nDataLength = pktHeader.GetDataLength();
	CByteVector	bvPacketData;
	bvPacketData.reserve( nDataLength + 4 );	// reserve space plus fcs.
	bvPacketData.resize( nDataLength );

	UInt32	nDataRead = Read( nDataLength, &bvPacketData[0] );
	assertm( (nDataRead == nDataLength), "Amount read did not match bytes to read." );
	if ( nDataRead != nDataLength ) {
		outError = CF_ERROR_INCOMPLETE_PAYLOAD;
		return false;
	}

	if ( !pktHeader.IsSliced() ) {
		// add FCS and initialize it to zero.
		bvPacketData.resize( nDataLength + 4 );
		*reinterpret_cast<UInt32*>( (&bvPacketData[nDataLength]) ) = 0;
		// nDataLength += 4;
	}
	pktHeader.m_nPacketLength += 4;	// add the FCS.

	outPacket.SetPacketData( pktHeader.m_nPacketLength, bvPacketData.size(), &bvPacketData[0] );

	cfRollback.Commit();

	return true;
}


// -----------------------------------------------------------------------------
//      ReadFilePackets
// -----------------------------------------------------------------------------

bool
CPCapCaptureReader::ReadFilePackets(
	UInt32				inCount,
	CFilePacketList&	outPackets,
	UInt32&				outError )
{
	for ( UInt32 i = 0; i < inCount; i++ ) {
		CFilePacket	pkt;
		if ( !ReadFilePacket( pkt, outError ) ) break;
		outPackets.push_back( pkt );
	}

	return !outPackets.empty();
}


// -----------------------------------------------------------------------------
//      ReadPacketHeader
// -----------------------------------------------------------------------------

bool
CPCapCaptureReader::ReadPacketHeader(
	TPCapPacketHeader&	outHeader,
	UInt32&				outError )
{
	outError = CF_ERROR_NONE;

	UInt64					nFileSize = GetSize();
	UInt32					nCount = GetPacketHeaderLength();
	TcpDumpPacketHeader2	pktHeader;
	UInt32					nRead = Read( nCount, &pktHeader );
	if ( nRead < nCount ) {
		std::this_thread::sleep_for( std::chrono::microseconds( 1 ) );
		outError = (GetSize() > nFileSize)
			? CF_ERROR_INCOMPLETE_HEADER
			: CF_ERROR_EOF; 
		return false;
	}

	UInt64	nTime = SwapIf32( IsSwapped(), pktHeader.hdr.tshi ) + m_nZone;
	UInt64	nHighTime = (nTime + kPeekTimeToAnsi) * kResolutionPerSecond;
	UInt64	nLowTime = SwapIf32( IsSwapped(), pktHeader.hdr.tslo );
	nLowTime *= GetTimeStampMultiplier();
	outHeader.m_nTimeStamp = nHighTime + nLowTime;

	outHeader.m_nPacketLength = SwapIf32( IsSwapped(), pktHeader.hdr.len );

	UInt32	nDataLength = SwapIf32( IsSwapped(), pktHeader.hdr.caplen );
	outHeader.m_nSliceLength =
		(nDataLength < outHeader.m_nPacketLength) ? nDataLength : 0;

	return true;
}


// ============================================================================
//		CPCapCaptureWriter
// ============================================================================

// -----------------------------------------------------------------------------
//      Init
// -----------------------------------------------------------------------------

void
CPCapCaptureWriter::Init()
{
	if ( !IsOpen() ) return;

	TcpDumpFileHeader			pcapHeader;
	pcapHeader.magic = DEFAULT_TCPDUMP_MAGIC;
	pcapHeader.major = m_nMajorVersion;
	pcapHeader.minor = m_nMinorVersion;
	pcapHeader.zone = m_nZone;
	pcapHeader.sigfigs = m_nSignificantFigures;
	pcapHeader.snaplen = m_nSnapLength;
	pcapHeader.linktype = m_nLinkType;

	UInt32	nCount = sizeof( TcpDumpFileHeader );
	Write( nCount, &pcapHeader );
}


// -----------------------------------------------------------------------------
//      Open
// -----------------------------------------------------------------------------

bool
CPCapCaptureWriter::Open(
	const path&	inPath )
{
	if ( CCaptureWriter::Open( inPath ) ) {
		m_nFileFormat = CF_FORMAT_PCAP;
	}

	return IsOpen();
}


// -----------------------------------------------------------------------------
//      WriteFilePacket
// -----------------------------------------------------------------------------

bool
CPCapCaptureWriter::WriteFilePacket(
	const CFilePacket&	inPacket )
{
	UInt32				nCount = sizeof( TcpDumpPacketHeader );
	TcpDumpPacketHeader	pktHeader;

	UInt64	nTime = (inPacket.GetTimeStamp() / kResolutionPerSecond);
	UInt64	nHighTime = (nTime < kPeekTimeToAnsi) ? 0 : (nTime - kPeekTimeToAnsi);
	UInt64	nLowTime = (inPacket.GetTimeStamp() % kResolutionPerSecond) / (kResolutionPerSecond / 1000000);

	pktHeader.tshi = static_cast<UInt32>( nHighTime );
	pktHeader.tslo = static_cast<UInt32>( nLowTime );
	pktHeader.caplen = static_cast<UInt32>( inPacket.GetLength() - 4 );
	pktHeader.len = static_cast<UInt32>( inPacket.GetPacketDataLength() );
	if ( !inPacket.IsSliced() ) {
		pktHeader.len -= 4;
	}

	UInt32	nWrite = Write( nCount, &pktHeader );
	if ( nWrite != nCount ) return false;

	Write( pktHeader.len, inPacket.GetPacketData() );

	m_nPacketCount++;

	return true;
}

}	// namespace CaptureFile
