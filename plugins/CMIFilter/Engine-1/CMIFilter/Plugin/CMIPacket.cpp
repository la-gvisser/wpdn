// =============================================================================
//	CMIPacket.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "CMIPacket.h"


inline size_t AlignTo8( size_t in ) { return ((in + 7) & ~7); }


// =============================================================================
//		CCmiPacketLayers
// =============================================================================

// ============================================================================
//		CCmiPacket
// ============================================================================

// -----------------------------------------------------------------------------
//		CmiAllocFrom
//  A modified, public, version of the CPacket AllocFrom method for CMI use.
// -----------------------------------------------------------------------------

UInt8*
CCmiPacket::CmiAllocFrom(
	CCmiIpV4PacketPtr&	inPacket,
	UInt16				inPacketSize )
{
	_ASSERTE( inPacket != nullptr );
	Peek::ThrowIf( inPacket == nullptr );

	_ASSERTE( inPacketSize <= kMaxUInt16 );
	Peek::ThrowIf( inPacketSize > kMaxUInt16 );

	IPacket*	pInPacket( inPacket->m_spPacket );
	_ASSERTE( pInPacket != nullptr );
	Peek::ThrowIf( pInPacket == nullptr );

	size_t	nHeaderSize( AlignTo8( sizeof( PeekPacket ) ) );
	size_t	nMediaSize( 0 );
	size_t	nDataSize( AlignTo8( inPacketSize ) );

	// Get the Packet Header.
	const PeekPacket*	pInPacketHeader( nullptr );
	Peek::ThrowIfFailed( pInPacket->GetPacketHeader( &pInPacketHeader ) );
	Peek::ThrowIf( pInPacketHeader == nullptr );

	// Get the optional Media Specific header.
	const MediaSpecificPrivateHeader* pInPacketMediaHdr =
		reinterpret_cast<const MediaSpecificPrivateHeader*>( pInPacketHeader->fMediaSpecInfoBlock );
	if ( pInPacketMediaHdr != nullptr ) {
		_ASSERTE( pInPacketMediaHdr->nSize != 0 );
		nMediaSize = AlignTo8( pInPacketMediaHdr->nSize );
	}

	size_t nTotalBytes( nHeaderSize + nMediaSize + nDataSize );

	// Allocate the new buffer.
	UInt8*	pNewData( CPacketData::Alloc( nTotalBytes ) );
	if ( !pNewData ) throw( E_OUTOFMEMORY );

	PeekPacket*	pPacketHeader( reinterpret_cast<PeekPacket*>( pNewData ) );
	memset( pNewData, 0, nTotalBytes );

	// Copy the relevant Packet Header fields.
	pPacketHeader->fTimeStamp = pInPacketHeader->fTimeStamp;
	pPacketHeader->fPacketLength = inPacketSize;

	// Copy the optional Media Specific header.
	if ( nMediaSize > 0 ) {
		UInt8*	pMediaHeader( pNewData + nHeaderSize );
		memcpy( pMediaHeader, pInPacketMediaHdr, pInPacketMediaHdr->nSize );
		pPacketHeader->fMediaSpecInfoBlock = pMediaHeader;
	}

	// Set the Allocated Data pointer.
	m_spAllocData = CCmiPacketAllocPtr( pNewData );

	// Save off the Media Type.
	m_MediaType = InternalGetMediaType( pInPacket );

	UInt8*	pPacketData( pNewData + nHeaderSize + nMediaSize );
	return pPacketData;
}


// ----------------------------------------------------------------------------
//		Initialize
// ----------------------------------------------------------------------------

bool
CCmiPacket::Initialize()
{
	_ASSERTE( m_spAllocData != nullptr );
	if ( m_spAllocData == nullptr ) return false;

	try {
		Create();
		ASSERT( CPacket::IsValid() );
		if ( CPacket::IsValid() ) {
			UInt8*	pData( reinterpret_cast<UInt8*>( m_spAllocData.get() ) );
			_ASSERTE( pData != nullptr );
			if ( pData == nullptr ) return false;

			// The Allocated Data is: PeekPacket, MediaSpecInfoBlock, Packet Data.
			// Each item is 8 byte aligned.
			size_t		nHeaderSize( AlignTo8( sizeof( PeekPacket ) ) );
			PeekPacket*	pPacketHeader( reinterpret_cast<PeekPacket*>( pData ) );
			_ASSERTE( pPacketHeader != nullptr );
			if ( pPacketHeader == nullptr ) return false;

			// Packet Data location when there is no MediaSpecInfoBlock.
			UInt8*	pPacketData( pData + nHeaderSize );

			if ( pPacketHeader->fMediaSpecInfoBlock != nullptr ) {
				const MediaSpecificPrivateHeader* pMediaHeader(
					reinterpret_cast<const MediaSpecificPrivateHeader*>( pPacketHeader->fMediaSpecInfoBlock ) );
				_ASSERTE( pMediaHeader != nullptr );
				if ( pMediaHeader == nullptr ) return false;

				// Adjust the Packet Data pointer to after the MediaSpecInfoBlock.
				size_t	nMediaSize( AlignTo8( pMediaHeader->nSize ) );
				pPacketData += nMediaSize;
			}

			// m_MediaType is set in CmiAllocFrom.
			Peek::ThrowIfFailed( m_spPacket->Initialize( nullptr, 0, pPacketHeader,
				pPacketData, m_MediaType.fType, m_MediaType.fSubType ) );
		}
	}
	catch( ... ) {
		throw;
	}

	return true;
}


// ============================================================================
//		CCmiIpV4Packet
// ============================================================================

CCmiIpV4Packet::CCmiIpV4Packet(
	const CPacket&	inPacket )
		:	CCmiPacket( inPacket )
		,	m_nDataOffset( 0 )
{
	if ( !GetLayer() ) return;

	ASSERT( IsValid() );
}


// ----------------------------------------------------------------------------
//		GetLayer
// ----------------------------------------------------------------------------
bool
CCmiIpV4Packet::GetLayer()
{
	CLayerIP	layerIP;
	if ( !CPacket::GetLayer( layerIP) )	return false;

	m_nPacketType = kPacketType_IPv4;

	const CIpV4HeaderRef&	HeaderRef( layerIP.GetIp4HeaderRef() );
	const tIpV4Header*		pIpV4Header = HeaderRef;

	m_IpV4Header.AdjustCmi( pIpV4Header );

	return true;
}


// ----------------------------------------------------------------------------
//		GetPacketKey
// ----------------------------------------------------------------------------

UInt64
CCmiIpV4Packet::GetPacketKey() const
{
	UInt64 nPacketKey =
		(static_cast<UInt64>( static_cast<UInt32>( m_IpV4Header.GetSource() ) ) << 32) ^
		(static_cast<UInt64>( static_cast<UInt32>( m_IpV4Header.GetDestination() ) ) << 16) ^
		GetIPHeader().GetIdentifier();
	return nPacketKey;
}


// ============================================================================
//		CCmiUdpPacket
// ============================================================================

// ----------------------------------------------------------------------------
//		GetLayer
// ----------------------------------------------------------------------------

bool		
CCmiUdpPacket::GetLayer()
{
	if ( !GetLayers() ) return false;
	if ( IsNotValid() ) return false;

	UInt32			nProtoSpec;
	const UInt8*	pHeader = nullptr;
	UInt16			nHeaderRemaining;

	if ( m_CmiLayers.GetHeaderLayer( ProtoSpecDefs::UDP, &nProtoSpec, &pHeader, &nHeaderRemaining ) ) {
		m_nPacketType = kPacketType_UDP;
		m_CmiUdpHeader.AdjustCmi( reinterpret_cast<const tUdpHeader*>( pHeader ), nHeaderRemaining );
	}

	return true;
}
