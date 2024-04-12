// =============================================================================
//	CaptureFile.cpp
//		implementation of the CCaptureFile class.
// =============================================================================
//	Copyright (c) 2011-2017 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "PCapCaptureFile.h"

using std::wstring;


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
	const wstring&	inFileName,
	bool			inScanPackets )
{
	if ( CCaptureReader::Open( inFileName, inScanPackets ) ) {
		m_nFileFormat = CF_FORMAT_PCAP;
	}

	return IsOpen();
}


// -----------------------------------------------------------------------------
//      Parse
// -----------------------------------------------------------------------------

void
CPCapCaptureReader::Parse(
	bool	inScanPackets /*= false*/ )
{
	if ( !IsOpen() ) return;

	TcpDumpFileHeader	pcapHeader;
	UInt32				dwBytesRead = GetFile().Read(  sizeof( TcpDumpFileHeader ), &pcapHeader );
	if ( dwBytesRead < sizeof( TcpDumpFileHeader ) ) return;

	switch ( pcapHeader.magic ) {
		case TCPDUMP_MAGIC:
			SetSwapped( false );
			SetVersion( TCPDUMP_VERSION_1 );
			break;

		case TCPDUMP_MAGIC_SWAPPED:
			SetSwapped( true );
			SetVersion( TCPDUMP_VERSION_1 );
			break;

		case TCPDUMP_MAGIC_2:
			SetSwapped( false );
			SetVersion( TCPDUMP_VERSION_2 );
			break;

		case TCPDUMP_MAGIC_2_SWAPPED:
			SetSwapped( true );
			SetVersion( TCPDUMP_VERSION_2 );
			break;

		default:
			return;
	}

	m_nMajorVersion = SwapIf16( IsSwapped(), pcapHeader.major );
	m_nMinorVersion = SwapIf16( IsSwapped(), pcapHeader.minor );
	m_nZone = SwapIf32( IsSwapped(), pcapHeader.zone );
	m_nSignificantFigures = SwapIf32( IsSwapped(), pcapHeader.sigfigs );
	m_nSnapLength = SwapIf32( IsSwapped(), pcapHeader.snaplen );
	m_nLinkType = SwapIf32( IsSwapped(), pcapHeader.linktype );

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

	m_ayOffsets.SetFirstOffset( GetFile().GetSize(), 1, GetFile().GetPosition() );

	if ( inScanPackets ) {
		SeekToPacket( kMaxUInt32 );
		SeekToPacket( 0 );
	}

	return;
}


// -----------------------------------------------------------------------------
//      ReadFilePacket
// -----------------------------------------------------------------------------

bool
CPCapCaptureReader::ReadFilePacket(
	CFilePacket&	outPacket,
	UInt32&			outError,
	UInt32			inIndex )
{
	outError = CF_ERROR_NONE;

	if ( inIndex != static_cast<UInt32>( NEXT_PACKET ) ) {
		if ( !SeekToPacket( inIndex ) ) return false;
		if ( inIndex >= m_ayOffsets.size() ) return false;
	}

	TPCapPacketHeader	pktHeader;
	if ( !ReadPacketHeader( pktHeader ) ) {
		// Assume End of File, but could be an incomplete header.
		return false;
	}

	outPacket.SetDataRate( 0 );
	outPacket.SetMediaType( m_MediaType );
	outPacket.SetTimeStamp( pktHeader.m_nTimeStamp );
	outPacket.SetStatus( 0 );

	UInt32 nDataLength = pktHeader.GetDataLength();
	CByteVector	bvPacketData;
	bvPacketData.reserve( nDataLength + 4 );	// reserve space plus fcs.
	bvPacketData.resize( nDataLength );

	UInt32	nDataRead = GetFile().Read( nDataLength, &bvPacketData[0] );
	_ASSERTE( nDataRead == nDataLength );
	if ( nDataRead != nDataLength ) {
		outError = CF_ERROR_INCOMPLETE_PAYLOAD;
		return false;
	}

	if ( !pktHeader.IsSliced() ) {
		// add fcs and initialize it to zero.
		bvPacketData.resize( nDataLength + 4 );
		*reinterpret_cast<UInt32*>( (&bvPacketData[nDataLength]) ) = 0;
		// nDataLength += 4;
	}
	pktHeader.m_nPacketLength += 4;	// add the fcs.

	outPacket.SetPacketData( pktHeader.m_nPacketLength, bvPacketData.size(), &bvPacketData[0] );

	// Update the packet Offsets if inIndex is not NEXT_PACKET.
	if ( inIndex != static_cast<UInt32>( NEXT_PACKET ) ) {
		m_ayOffsets.SetOffset( (inIndex + 1), GetFile().GetPosition() );
	}

	return true;
}


// -----------------------------------------------------------------------------
//      ReadFilePackets
// -----------------------------------------------------------------------------

bool
CPCapCaptureReader::ReadFilePackets(
	CFilePacketList&	outPackets,
	UInt32&				outError,
	UInt32				inCount,
	UInt32				inFirst /*= NEXT_PACKET*/)
{
	UInt32	nLast = inCount + ((inFirst == (UInt32)NEXT_PACKET) ? 0 : inFirst);
	for ( UInt32 i = inFirst; i < nLast; i++ ) {
		CFilePacket	pkt;
		if ( !ReadFilePacket( pkt, outError, i ) ) break;
		outPackets.push_back( pkt );
	}

	return !outPackets.empty();
}


// -----------------------------------------------------------------------------
//      ReadPacketHeader
// -----------------------------------------------------------------------------

bool
CPCapCaptureReader::ReadPacketHeader(
	TPCapPacketHeader&	outHeader )
{
	UInt32					nCount = GetPacketHeaderLength();
	TcpDumpPacketHeader2	pktHeader;
	UInt32					nRead = GetFile().Read( nCount, &pktHeader );
	if ( nRead < nCount ) return false;

	UInt64	nTime = SwapIf32( IsSwapped(), pktHeader.hdr.tshi ) + m_nZone;
	UInt64	nHighTime = (nTime + kPeekTimeToAnsi) * kResolutionPerSecond;
	UInt64	nLowTime = SwapIf32( IsSwapped(), pktHeader.hdr.tslo );
	if ( m_nSignificantFigures != 3 ) {
		// Don't adjust low part of time stamp if tcpdump for AIX (v4.0.2).
		nLowTime *= 1000;
	}
	outHeader.m_nTimeStamp = nHighTime + nLowTime;

	outHeader.m_nPacketLength = SwapIf32( IsSwapped(), pktHeader.hdr.len );

	UInt32	nDataLength = SwapIf32( IsSwapped(), pktHeader.hdr.caplen );
	outHeader.m_nSliceLength =
		(nDataLength < outHeader.m_nPacketLength) ? nDataLength : 0;

	return true;
}


// -----------------------------------------------------------------------------
//      SeekToPacket
// -----------------------------------------------------------------------------

bool
CPCapCaptureReader::SeekToPacket(
	UInt32	inIndex )
{
	if ( m_ayOffsets.empty() ) return false;
	if ( m_ayOffsets.HasIndex( inIndex ) ) {
		GetFile().SeekTo( m_ayOffsets[inIndex] );
		return true;
	}

	if ( m_ayOffsets.IsComplete() ) return false;

	size_t	nIndex = m_ayOffsets.GetHighest();
	while ( nIndex <= inIndex ) {
		UInt64	nOffset = m_ayOffsets[nIndex];
		GetFile().SeekTo( nOffset );

		TPCapPacketHeader	pktHeader;
		if ( !ReadPacketHeader( pktHeader ) ) return false;
		if ( pktHeader.GetDataLength() == 0 ) return false;

		UInt32	nPacketLength = GetPacketHeaderLength() + pktHeader.GetDataLength();
		nOffset += nPacketLength;
		if ( nOffset > GetFileSize() ) { //  m_ayOffsets.IsEOF( nOffset ) -> nOffset > GetFileSize()
			// Not all of this packet, the one request or a previous one, is in the file.
			m_ayOffsets.SetComplete();
			return false;
		}

		nIndex++;
		if ( !m_ayOffsets.SetOffset( nIndex, nOffset ) ) {
			if ( m_ayOffsets.IsEOF( nOffset ) ) {
				// Packet (inIndex + 1) is not in the file.
				m_ayOffsets.SetComplete();
			}
			break;
		}
	}

	if ( !m_ayOffsets.HasIndex( inIndex ) ) return false;

	GetFile().SeekTo( m_ayOffsets[inIndex] );
	return true;
}


// ============================================================================
//		CPCapCaptureWriter
// ============================================================================

// -----------------------------------------------------------------------------
//      Init
// -----------------------------------------------------------------------------

#if (0)
void
CPCapCaptureWriter::Init()
{
	if ( !IsOpen() ) return;

	TcpDumpFileHeader	pcapHeader;
	pcapHeader.magic = DEFAULT_MAGIC;
	pcapHeader.major = m_nMajorVersion;
	pcapHeader.minor = m_nMinorVersion;
	pcapHeader.zone = m_nZone;
	pcapHeader.sigfigs = m_nSignificantFigures;
	pcapHeader.snaplen = m_nSnapLength;
	pcapHeader.linktype = m_nLinkType;

	UInt32	nCount = sizeof( TcpDumpFileHeader );
	UInt32	nWrite = GetFile().Write( nCount, &pcapHeader );
	_ASSERTE( nWrite == nCount );
}
#endif

void
CPCapCaptureWriter::Init(
	const CCaptureFile*	inTemplate )
{
	if ( !IsOpen() ) return;

	TcpDumpFileHeader			pcapHeader;
	const CPCapCaptureWriter*	pTempate = dynamic_cast<const CPCapCaptureWriter*>( inTemplate );
	if ( pTempate ) {
		// TODO ???
		pcapHeader.magic = DEFAULT_MAGIC;
		pcapHeader.major = m_nMajorVersion;
		pcapHeader.minor = m_nMinorVersion;
		pcapHeader.zone = m_nZone;
		pcapHeader.sigfigs = m_nSignificantFigures;
		pcapHeader.snaplen = m_nSnapLength;
		pcapHeader.linktype = m_nLinkType;
	}
	else {
		pcapHeader.magic = DEFAULT_MAGIC;
		pcapHeader.major = m_nMajorVersion;
		pcapHeader.minor = m_nMinorVersion;
		pcapHeader.zone = m_nZone;
		pcapHeader.sigfigs = m_nSignificantFigures;
		pcapHeader.snaplen = m_nSnapLength;
		pcapHeader.linktype = m_nLinkType;
	}

	UInt32	nCount = sizeof( TcpDumpFileHeader );
	(void)GetFile().Write( nCount, &pcapHeader );
}


// -----------------------------------------------------------------------------
//      Open
// -----------------------------------------------------------------------------

bool
CPCapCaptureWriter::Open(
	const wstring&	inFileName )
{
	if ( CCaptureWriter::Open( inFileName ) ) {
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

	UInt32	nWrite = GetFile().Write( nCount, &pktHeader );
	if ( nWrite != nCount ) return false;

	GetFile().Write( pktHeader.len, inPacket.GetPacketData() );

	m_nPacketCount++;

	return true;
}

}	// namespace CaptureFile
