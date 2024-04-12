// =============================================================================
//	CaptureFile.cpp
//		implementation of the CCaptureFile class.
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

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
			}
			break;

		case kMediaType_802_5:
			m_nLinkType = DLT_IEEE802;
			break;

		case kMediaType_Wan:
		case kMediaType_CoWan:
		default:
			// TODO
			_ASSERTE( inType.fType != inType.fType );
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
	DWORD				dwBytesRead = GetFile().Read(  sizeof( TcpDumpFileHeader ), &pcapHeader );
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
	}

	return;
}


// -----------------------------------------------------------------------------
//      ReadFilePacket
// -----------------------------------------------------------------------------

bool
CPCapCaptureReader::ReadFilePacket(
	CFilePacket&	outPacket,
	UInt32			inIndex )
{
	if ( !SeekToPacket( inIndex ) ) return false;
	if ( inIndex >= m_ayOffsets.size() ) return false;

	TPCapPacketHeader	pktHeader;
	if ( !ReadPacketHeader( pktHeader ) ) return false;

	outPacket.SetDataRate( 0 );
	outPacket.SetMediaType( m_MediaType );
	outPacket.SetTimeStamp( pktHeader.m_nTimeStamp );
	outPacket.SetStatus( 0 );

	CByteVector	bvPacketData;
	bvPacketData.resize( pktHeader.GetDataLength() );

	size_t	nCount = bvPacketData.size();
	_ASSERTE( nCount < kMaxUInt32 );
	if ( nCount >= kMaxUInt32 ) {
		throw -1;
	}
	UInt32	nDataRead = GetFile().Read( static_cast<UInt32>( nCount ), &bvPacketData[0] );
	_ASSERTE( nDataRead == bvPacketData.size() );
	if ( nDataRead != bvPacketData.size() ) return false;

	outPacket.SetPacketData( pktHeader.m_nPacketLength, bvPacketData.size(), &bvPacketData[0] );

	// Update the packet Offsets.
	if ( (inIndex + 1) < m_ayOffsets.size() ) {
		m_ayOffsets.SetOffset( (inIndex + 1), GetFile().GetPosition() );
	}

	return true;
}


// -----------------------------------------------------------------------------
//      ReadPacketHeader
// -----------------------------------------------------------------------------

bool
CPCapCaptureReader::ReadPacketHeader(
	TPCapPacketHeader&	outHeader )
{
	DWORD					nCount = GetPacketHeaderLength();
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
		if ( m_ayOffsets.IsEOF( nOffset ) ) {
			// Packet inIndex is not in the file.
			m_ayOffsets.SetComplete();
			return false;
		}

		nIndex++;
		nOffset += nPacketLength;
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
		// Todo.
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
	UInt32	nWrite = GetFile().Write( nCount, &pcapHeader );
	_ASSERTE( nWrite == nCount );
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
	UInt32	nHighTime = (nTime < kPeekTimeToAnsi) ? 0 : static_cast<UInt32>( nTime - kPeekTimeToAnsi );
	UInt32	nLowTime = (inPacket.GetTimeStamp() % kResolutionPerSecond) / (kResolutionPerSecond / 1000000);
	size_t	nCapLen = inPacket.GetLength() - 4;
	_ASSERTE( nCapLen < kMaxUInt32 );
	if ( nCapLen >= kMaxUInt32 ) {
		throw -1;
	}
	size_t	nLength = inPacket.GetPacketDataLength();
	_ASSERTE( nLength < kMaxUInt32 );
	if ( nLength >= kMaxUInt32 ) {
		throw -1;
	}

	pktHeader.tshi = nHighTime;
	pktHeader.tslo = nLowTime;
	pktHeader.caplen = static_cast<UInt32>( nCapLen );
	pktHeader.len = static_cast<UInt32>( nLength );
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
