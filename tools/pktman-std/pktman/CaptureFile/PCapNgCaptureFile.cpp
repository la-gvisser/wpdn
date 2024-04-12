// =============================================================================
//	PCapNgCaptureFile.cpp
//		implementation of the CPCapNgCaptureFile class.
// =============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#include "PCapNgCaptureFile.h"
#include "MemUtil.h"
#include <cassert>
#include <chrono>
#include <filesystem>
#include <thread>

#ifdef _LINUX
#include <unistd.h>
#endif

using MemUtil::Swap16;
using MemUtil::Swap32;
using MemUtil::Swap64;
using MemUtil::SwapIf16;
using MemUtil::SwapIf32;

using std::filesystem::path;
using std::unique_ptr;

// Use (void) to silence unused warnings.
#define assertm(exp, msg)	assert(((void)msg, exp))

#define ALSZ				sizeof(UInt32)
#define ALIGN32(v)			((((v)%ALSZ)==0)?(v):(v+(ALSZ-((v)%ALSZ))))


namespace CaptureFile {

// ============================================================================
//		CPCapNgCapture
// ============================================================================

// -----------------------------------------------------------------------------
//      SetLinkType
// -----------------------------------------------------------------------------

void
CPCapNgCapture::SetLinkType(
	UInt16	inType )
{
	m_nLinkType = inType;

	switch ( inType ) {
		case LINKTYPE_NULL:
		case LINKTYPE_ETHERNET:
			m_MediaType = kMediaType_Ethernet;
			break;

		case LINKTYPE_IEEE802_11:
		case LINKTYPE_IEEE802_11_RADIOTAP:
			m_MediaType = kMediaType_Wireless;
			break;

		default:
			break;
	}
}


// -----------------------------------------------------------------------------
//      SetMediaType
// -----------------------------------------------------------------------------

void
CPCapNgCapture::SetMediaType(
	const tMediaType&	inType )
{
	m_MediaType = inType;

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
//		CPCapNgCaptureReader
// ============================================================================

// -----------------------------------------------------------------------------
//      BeginNewSection
// -----------------------------------------------------------------------------

bool
CPCapNgCaptureReader::BeginNewSection(
	const AnyBlockHeader&	inSectionHeader,
	UInt64					inRemaining,
	UInt64&					outFirstPacketOffset )
{
	if ( (inSectionHeader.header.type != PCAPNG_SECTION_BLOCK) ) return false;

	bool			bSwapped = false;
	bool			bInterfaceDescription = false;
	PCapNgVersion	ngVersion{ 0, 0 };
	UInt64			nMultiplier = 1;
	UInt64			nSectionLength = 0;
	UInt16			nLinkType = 0;
	UInt32			nSnapLength = 0;

	switch ( inSectionHeader.section.magic ) {
		case PCAPNG_MAGIC:
			ngVersion = { inSectionHeader.section.major, inSectionHeader.section.minor };
			nSectionLength = inSectionHeader.section.sectionlength;
			break;

		case PCAPNG_MAGIC_SWAPPED:
			bSwapped = true;
			ngVersion = { Swap16( inSectionHeader.section.major ),
				Swap16( inSectionHeader.section.minor ) };
			nSectionLength = Swap64( inSectionHeader.section.sectionlength );
			break;

		default:
			return false;
	}

	if ( !IsVersion1( ngVersion ) ) return false;

	SetVersion( ngVersion );
	SetSectionLength( nSectionLength );

	// Ignore the Options: hardware, os, userapplication...
#ifdef _DEBUG
		if ( inRemaining > 0 ) {
			Seek( inRemaining );
		}

		BlockTrailer	SectionTrailer;
		Read( sizeof( BlockTrailer ), &SectionTrailer );
		if ( SectionTrailer.length != inSectionHeader.header.length ) return false;
#else
		Seek( inRemaining + sizeof( BlockTrailer ) );
#endif

	bool		bSuccess;
	do {
		UInt64				nRemaining = 0;
		UInt32				nUnsupportedOptionCount = 0;
		UInt64				nBlockHeaderOffset = GetPosition();
		AnyBlockHeader		any;
		bSuccess = ReadBlockHeader( any, nRemaining );
		if ( bSuccess ) {
			if ( any.header.type == PCAPNG_INTERFACE_DESCRIPTION_BLOCK ) {
				nLinkType = SwapIf16( bSwapped, any.description.linktype );
				nSnapLength = SwapIf32( bSwapped, any.description.snaplength );
				bInterfaceDescription = true;
				if ( nRemaining > 0 ) {
					UInt32				nDataLength = static_cast<UInt32>( nRemaining );
					unique_ptr<UInt8>	spData( new UInt8[nDataLength] );
					UInt32				cbRead = Read( nDataLength, spData.get() );
					nRemaining -= (cbRead < nDataLength) ? cbRead : nRemaining;

					UInt8*			pBegin = spData.get();
					UInt8*			pEnd = pBegin + cbRead;
					OptionHeader*	pOption = reinterpret_cast<OptionHeader*>( pBegin );
					while ( pOption->type != PCAPNG_OPTION_END ) {
						UInt16	nType = SwapIf16( bSwapped, pOption->type );
						UInt16	nLength = SwapIf16( bSwapped, pOption->length );

						switch ( nType ) {
							case PCAPNG_OPTION_TSRESOL:
								{
									// Length is defined to be 1.
									static UInt64 powersof10[10]{
										 1, 10, 100, 1000, 10000, 100000, 1000000,
										 10000000, 100000000, 1000000000
									};

									UInt8	nResolution = *(pBegin + sizeof( OptionHeader ));
									if ( nResolution < 9 ) {
										nMultiplier = powersof10[9 - nResolution];
									}
								}
								break;

#if (0)
							case PCAPNG_OPTION_TZONE:
								{
#ifdef _DEBUG
									SInt32*	pTimeZone = reinterpret_cast<SInt32*>( pBegin + sizeof( OptionHeader ) );
									SInt32	nTimeZone= SwapIf32( bSwapped, *pTimeZone );
#endif	// _DEBUG
								}
								break;
#endif

							default:
								nUnsupportedOptionCount++;
								break;
						}

						pBegin += sizeof( OptionHeader ) + ALIGN32( nLength );
						if ( pBegin > pEnd ) break;
						pOption = reinterpret_cast<OptionHeader*>( pBegin );
					}
				}
			}
			else if ( (any.header.type == PCAPNG_SIMPLE_PACKET_BLOCK) ||
					  (any.header.type == PCAPNG_ENHANCED_PACKET_BLOCK) ) {
				outFirstPacketOffset = nBlockHeaderOffset;
				break;
			}

#ifdef _DEBUG
			if ( nRemaining > 0 ) {
				Seek( nRemaining );
			}

			BlockTrailer	trailer;
			Read( sizeof( BlockTrailer ), &trailer );
			if ( trailer.length != any.header.length ) return false;
#else
			Seek( nRemaining + sizeof( BlockTrailer ) );
#endif	// _DEBUG
		}
		else {
			break;
		}
	} while ( bSuccess );

	if ( !bInterfaceDescription || (outFirstPacketOffset == 0) ) return false;
	if ( (nLinkType != LINKTYPE_ETHERNET) &&
		 (nLinkType != LINKTYPE_IEEE802_11_RADIOTAP) ) return false;

	SetSwapped( bSwapped );
	SetVersion( ngVersion );
	SetTimeStampMultiplier( nMultiplier );
	SetSectionLength( nSectionLength );
	SetLinkType( nLinkType );
	SetSnapLength( nSnapLength );

	// If Wireless, then set some reasonable defaults:
	// Data Rate = 2
	// Channel = first supported...
	// If WAN, then set some reasonable defaults:
	// Direction = 1
	// Protocol = PEEK_WAN_PROT_FRLY

	return true;
}


// -----------------------------------------------------------------------------
//      Open
// -----------------------------------------------------------------------------

bool
CPCapNgCaptureReader::Open(
	const path&	inPath )
{
	if ( !IsOpen() ) {
		if ( !CCaptureReader::Open( inPath ) ) {
			return false;
		}
		m_nFileFormat = CF_FORMAT_PCAP_NG;
	}

	if ( !Parse() ) return false;

	return IsOpen();
}


// -----------------------------------------------------------------------------
//      Parse
// -----------------------------------------------------------------------------

bool
CPCapNgCaptureReader::Parse()
{
	if ( !IsOpen() ) return false;

	MoveTo( 0 );
	for ( int i = 0; i < 11; ++i ) {
		if ( GetSize() >= (sizeof( BlockHeader ) + sizeof( SectionBlockHeader )) ) {
			break;
		}
		if ( i >= 10 ) return false;
		std::this_thread::sleep_for( std::chrono::microseconds( 1 ) );
	}

	AnyBlockHeader		any;
	UInt64				nRemaining = 0;
	if ( !ReadBlockHeader( any, nRemaining ) ) return false;

	UInt64	nPosition = 0;
	if ( !BeginNewSection( any, nRemaining, nPosition ) ) return false;

	SetPacketsPosition( nPosition );

	return true;
}


// -----------------------------------------------------------------------------
//      ReadBlockHeader
// -----------------------------------------------------------------------------

bool
CPCapNgCaptureReader::ReadBlockHeader(
	AnyBlockHeader&	outBlock,
	UInt64&			outRemaining )
{
#ifdef _DEBUG
	UInt64		nFileSize = GetSize();
	UInt64		nFilePos = GetPosition();
	UInt64		nRemaining( nFileSize - nFilePos );
#else
	UInt64		nRemaining( GetSize() - GetPosition() );
#endif	// _DEBUG

	if ( nRemaining < sizeof( BlockHeader ) ) return false;

	UInt32	nBytesRead = Read( sizeof( BlockHeader ), &outBlock.header );
	if ( nBytesRead != sizeof( BlockHeader ) ) return false;

	UInt32	nToRead = 0;

	switch ( outBlock.header.type ) {
		case PCAPNG_SECTION_BLOCK:
			nToRead = sizeof( SectionBlockHeader );
			break;

		case PCAPNG_INTERFACE_DESCRIPTION_BLOCK:
			nToRead = sizeof( InterfaceDescriptionBlockHeader );
			break;

		case PCAPNG_SIMPLE_PACKET_BLOCK:
			nToRead = sizeof( SimplePacketBlockHeader );
			break;

		case PCAPNG_NAME_RESOLUTION_BLOCK:
			nToRead = sizeof( NameResolutionBlockHeader );
			break;

		case PCAPNG_INTERFACE_STATISTICS_BLOCK:
			nToRead = sizeof( InterfaceStatisticsBlockHeader );
			break;

		case PCAPNG_ENHANCED_PACKET_BLOCK:
			nToRead = sizeof( EnhancedPacketBlockHeader );
			break;

		case PCAPNG_CUSTOM_BLOCK_COPY:
		case PCAPNG_CUSTOM_BLOCK_NO_COPY:
			nToRead = sizeof( CustomBlockHeader );
			break;

		default:
			return false;
	}

	nBytesRead = Read( nToRead, &outBlock.data );
	if ( nBytesRead != nToRead ) return false;

	outRemaining = outBlock.header.length -
		(sizeof( BlockHeader ) + nBytesRead) - sizeof( BlockTrailer );

	return true;
}


// -----------------------------------------------------------------------------
//      ReadFilePacket
// -----------------------------------------------------------------------------

bool
CPCapNgCaptureReader::ReadFilePacket(
	CFilePacket&	outPacket,
	UInt32&			outError )
{
	outError = CF_ERROR_NONE;

	CFileRollback		cfRollback( m_File );
	TPCapNgPacketHeader	pktHeader;
	UInt32				nOptionsLength = 0;
	if ( !ReadPacketHeader( pktHeader, nOptionsLength, outError ) ) {
		// Assume End of File, but could be an incomplete header.
		return false;
	}

	outPacket.SetDataRate( 0 );
	outPacket.SetMediaType( m_MediaType );
	outPacket.SetTimeStamp( pktHeader.m_nTimeStamp );
	outPacket.SetStatus( 0 );

	UInt32	nDataLength = pktHeader.GetDataLength();
	UInt32	nBufferLength = ALIGN32( nDataLength );

	CByteVector	bvPacketData;
	bvPacketData.reserve( nBufferLength + 4 );	// reserve space plus fcs.
	bvPacketData.resize( nBufferLength );

	UInt32	nDataRead = Read( nBufferLength, &bvPacketData[0] );
	if ( nDataRead != nBufferLength ) {
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

#ifdef _DEBUG
	UInt64			nSimpleLength = sizeof( BlockHeader ) + sizeof( SimplePacketBlockHeader ) +
		nBufferLength + nOptionsLength + sizeof( BlockTrailer );
	UInt64			nEnhancedLength = sizeof( BlockHeader ) + sizeof( EnhancedPacketBlockHeader ) +
		nBufferLength + nOptionsLength + sizeof( BlockTrailer );
	if ( nOptionsLength > 0 ) {
		Seek( nOptionsLength );
	}
	BlockTrailer	trailer;
	Read( sizeof( BlockTrailer ), &trailer );
	if ( (trailer.length != nSimpleLength) &&
		 (trailer.length != nEnhancedLength) ) return false;
#else
	size_t	nPositionEnd( GetPosition() );
	assertm( nPositionEnd == (nOptionsLength + sizeof( BlockTrailer )), "Need to seek to end of packet" );
	// Seek( nOptionsLength + sizeof( BlockTrailer ) );
#endif	// _DEBUG

	return true;
}


// -----------------------------------------------------------------------------
//      ReadFilePackets
// -----------------------------------------------------------------------------

bool
CPCapNgCaptureReader::ReadFilePackets(
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
CPCapNgCaptureReader::ReadPacketHeader(
	TPCapNgPacketHeader&	outHeader,
	UInt32&					outOptions,
	UInt32&					outError )
{
	outError = CF_ERROR_NONE;

	UInt64		nFileSize = GetSize();
	bool		bHavePacket = false;
	while ( !bHavePacket ) {
		AnyBlockHeader	any;
		UInt64			nRemaining = 0;
		if ( !ReadBlockHeader( any, nRemaining ) ) {
			std::this_thread::sleep_for( std::chrono::microseconds( 1 ) );
			outError = (GetSize() > nFileSize)
				? CF_ERROR_INCOMPLETE_HEADER
				: CF_ERROR_EOF; 
			return false;
		}

		if ( any.header.type == PCAPNG_SIMPLE_PACKET_BLOCK ) {
			outHeader.m_nTimeStamp = 0;
			outHeader.m_nPacketLength = any.simplepacket.packetlength;
			outHeader.m_nSliceLength = any.header.length - sizeof( BlockHeader ) -
				 sizeof( SimplePacketBlockHeader ) - sizeof( BlockTrailer );

			outOptions = any.header.length - sizeof( BlockHeader ) - sizeof( SimplePacketBlockHeader ) -
				outHeader.m_nSliceLength;

			bHavePacket = true;
		}
		else if ( any.header.type == PCAPNG_ENHANCED_PACKET_BLOCK ) {
			UInt64	nTimeHigh = SwapIf32( IsSwapped(), any.packet.timestamphigh );
			UInt64	nTimeLow = SwapIf32( IsSwapped(), any.packet.timestamplow );
			outHeader.m_nTimeStamp = (((nTimeHigh << 32ull) | nTimeLow) * GetTimeStampMultiplier()) +
				(kPeekTimeToAnsi * kResolutionPerSecond);
			outHeader.m_nPacketLength = any.packet.packetlength;
			outHeader.m_nSliceLength = (any.packet.length < any.packet.packetlength)
				? any.packet.length
				: 0;

			outOptions = any.header.length - sizeof( BlockHeader ) -
				sizeof( EnhancedPacketBlockHeader ) - ALIGN32( outHeader.GetDataLength() ) -
				sizeof( BlockTrailer );

			bHavePacket = true;
		}
		else if ( any.header.type == PCAPNG_SECTION_BLOCK ) {
			UInt64	nPosition = 0;
			if ( !BeginNewSection( any, nRemaining, nPosition ) ) return false;
			MoveTo( nPosition );
		}
		else {
			Seek( nRemaining + sizeof( BlockTrailer ) );
		}
	}

	return true;
}


// ============================================================================
//		CPCapNgCaptureWriter
// ============================================================================

// -----------------------------------------------------------------------------
//      Init
// -----------------------------------------------------------------------------

void
CPCapNgCaptureWriter::Init()
{
	if ( !IsOpen() ) return;

	// Write the Section Block
	BlockHeader			blockHeader;
	blockHeader.type = PCAPNG_SECTION_BLOCK;
	blockHeader.length = sizeof( BlockHeader ) + sizeof( SectionBlockHeader ) +
		sizeof( BlockTrailer );
	(void) Write( sizeof( blockHeader ), &blockHeader );

	SectionBlockHeader	sectionHeader;
	sectionHeader.magic = DEFAULT_PCAPNG_MAGIC;
	sectionHeader.major = m_Version.major;
	sectionHeader.minor = m_Version.minor;
	sectionHeader.sectionlength = 0xFFFFFFFFFFFFFFFFull;
	(void) Write( sizeof( sectionHeader ), &sectionHeader );

	BlockTrailer	blockTrailer;
	blockTrailer.length = blockHeader.length;
	(void) Write( sizeof( blockTrailer ), &blockTrailer );

	// Write the Interface Description Block
	blockHeader.type = PCAPNG_INTERFACE_DESCRIPTION_BLOCK;
	blockHeader.length = sizeof( BlockHeader ) +
		sizeof( InterfaceDescriptionBlockHeader ) + sizeof( BlockTrailer );
	(void) Write( sizeof( blockHeader ), &blockHeader );

	InterfaceDescriptionBlockHeader	descriptionHeader;
	descriptionHeader.linktype = m_nLinkType;
	descriptionHeader.reserved = 0;
	descriptionHeader.snaplength = m_nSnapLength;
	(void) Write( sizeof( descriptionHeader ), &descriptionHeader );

	blockTrailer.length = blockHeader.length;
	(void) Write( sizeof( blockTrailer ), &blockTrailer );
}


// -----------------------------------------------------------------------------
//      Open
// -----------------------------------------------------------------------------

bool
CPCapNgCaptureWriter::Open(
	const path&	inPath )
{
	if ( CCaptureWriter::Open( inPath ) ) {
		m_nFileFormat = CF_FORMAT_PCAP_NG;
	}

	return IsOpen();
}


// -----------------------------------------------------------------------------
//      WriteFilePacket
// -----------------------------------------------------------------------------

bool
CPCapNgCaptureWriter::WriteFilePacket(
	const CFilePacket&	inPacket )
{
	EnhancedPacketHeader	pktHeader;
	UInt64					nTime = inPacket.GetTimeStamp();

	pktHeader.packet.timestamphigh = static_cast<UInt32>( nTime >> 32);
	pktHeader.packet.timestamplow = static_cast<UInt32>( nTime & 0x0FFFFFFFF );
	pktHeader.packet.packetlength = static_cast<UInt32>( inPacket.GetLength() - 4 );
	pktHeader.packet.length = static_cast<UInt32>( inPacket.GetPacketDataLength() );
	if ( !inPacket.IsSliced() ) {
		pktHeader.packet.length -= 4;
	}

	UInt32	nDataLength = ALIGN32( pktHeader.packet.length );

	pktHeader.header.type = PCAPNG_ENHANCED_PACKET_BLOCK;
	pktHeader.header.length = sizeof( BlockHeader ) + sizeof( EnhancedPacketHeader ) + 
		nDataLength + sizeof( BlockTrailer );

	UInt32	nWrite = Write( sizeof( pktHeader ), &pktHeader );
	if ( nWrite != sizeof( pktHeader ) ) return false;

	nWrite = Write( pktHeader.packet.length, inPacket.GetPacketData() );
	if ( nWrite != pktHeader.packet.length ) return false;

	UInt32	nPaddingLength = nDataLength - pktHeader.packet.length;
	if ( nPaddingLength > 0 ) {
		UInt32	nPadding = 0;
		nWrite = Write( nPaddingLength, &nPadding );
		if ( nWrite != nPaddingLength ) return false;
	}

	return true;
}

}	// namespace CaptureFile
