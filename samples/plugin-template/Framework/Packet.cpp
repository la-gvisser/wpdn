// =============================================================================
//	Packet.cpp
// =============================================================================
//	Copyright (c) 2008-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "Peek.h"
#include "Packet.h"
#include "peekcore.h"
#include "PeekContextProxy.h"
#include "MediaTypes.h"

inline size_t AlignTo8( size_t in ) { return ((in + 7) & ~7); }


// =============================================================================
//		CPacketLayers
// =============================================================================

CPacketLayers::CPacketLayers(
	void*	inPtr /*= nullptr*/ )
	:	m_spLayers( nullptr )
{
	SetPtr( inPtr );
}

CPacketLayers::~CPacketLayers()
{
	ReleasePtr();
}


// -----------------------------------------------------------------------------
//		GetDataLayer
// -----------------------------------------------------------------------------

bool
CPacketLayers::GetDataLayer(
	UInt16			inProtoSpecId,
	UInt32*			outSourceProtoSpec,
	const UInt8**	outData,
	UInt16*			outBytesRemaining )
{
	_ASSERTE( IsValid() );
	if ( IsValid() ) {
		HeResult hr = m_spLayers->GetDataLayer( inProtoSpecId,
			outSourceProtoSpec, outData, outBytesRemaining );
		if ( hr != HE_S_OK ) return false;
		return true;
	}

	return false;

}


// -----------------------------------------------------------------------------
//		GetHeaderLayer
// -----------------------------------------------------------------------------

bool
CPacketLayers::GetHeaderLayer(
	UInt16			inProtoSpecId,
	UInt32*			outSourceProtoSpec,
	const UInt8**	outData,
	UInt16*			outBytesRemaining )
{
	_ASSERTE( IsValid() );
	if ( IsValid() ) {
		HeResult hr = m_spLayers->GetHeaderLayer( inProtoSpecId,
			outSourceProtoSpec, outData, outBytesRemaining );
		if ( hr != HE_S_OK ) return false;
		return true;
	}

	return false;
}


// -----------------------------------------------------------------------------
//		GetLayerInfo
// -----------------------------------------------------------------------------

bool
CPacketLayers::GetLayerInfo(
	UInt16				inProtoSpecId,
	CPacketLayerInfo&	outInfo )
{
	_ASSERTE( IsValid() );
	if ( IsNotValid() ) return false;

	HeResult		hr;
	UInt32			nProtoSpec;
	const UInt8*	pHeader = nullptr;
	UInt16			nHeaderRemaining;
	hr = m_spLayers->GetHeaderLayer( inProtoSpecId, &nProtoSpec, &pHeader, &nHeaderRemaining );
	if ( HE_FAILED( hr ) ) return false;

	const UInt8*	pData = nullptr;
	UInt16			nDataRemaining;
	hr = m_spLayers->GetDataLayer( inProtoSpecId, &nProtoSpec, &pData, &nDataRemaining );
	if ( HE_FAILED( hr ) ) return false;

	UInt16			nHeaderLength = static_cast<UInt16>( pData - pHeader );
	UInt16			nDataLength = nDataRemaining - 4;

	CByteVectRef	bvHeader( nHeaderLength, pHeader );
	CByteVectRef	bvData( nDataLength, pData );

	outInfo.Set( inProtoSpecId, bvHeader, bvData );
	return true;
}


// -----------------------------------------------------------------------------
//		GetPacketLayers
// -----------------------------------------------------------------------------

void
CPacketLayers::GetPacketLayers(
	UInt32*			outLayerCount,
	PacketLayerInfo	outLayerInfo[] )
{
	_ASSERTE( IsValid() );
	if ( IsValid() ) {
		Peek::ThrowIfFailed( m_spLayers->GetPacketLayers( outLayerCount, outLayerInfo ) );
	}
}


// =============================================================================
//		CPacket
// =============================================================================

CPacket::CPacket(
	void*	inPtr /*= nullptr*/ )
{
	SetPtr( inPtr );
}


// -----------------------------------------------------------------------------
//		AllocFrom	[static]
// -----------------------------------------------------------------------------

const UInt8*
CPacket::AllocFrom(
	IPacket*		inIPacket,
	tPacketInfo&	outInfo )
{
	_ASSERTE( inIPacket );
	Peek::ThrowIf( inIPacket == nullptr );
	if ( inIPacket == nullptr ) {
		memset( &outInfo, 0, sizeof( tPacketInfo ) );
		return nullptr;
	}

	size_t		nHeaderSize = 0;
	size_t		nMediaSize = 0;
	size_t		nDataSize = 0;
	size_t		nTotalBytes = 0;
	PeekPacket*	pNewPacketHeader = nullptr;
	UInt8*		pNewMediaHdr = nullptr;
	UInt8*		pNewPacketData = nullptr;

	// Get the Packet Header.
	const PeekPacket*	pPacketHeader;
	Peek::ThrowIfFailed( inIPacket->GetPacketHeader( &pPacketHeader ) );
	Peek::ThrowIf( pPacketHeader == nullptr );
	nHeaderSize = AlignTo8( sizeof( PeekPacket ) );

	// Get the optional Media Specific header.
	const MediaSpecificPrivateHeader* pMediaHdr =
		reinterpret_cast<const MediaSpecificPrivateHeader*>( pPacketHeader->fMediaSpecInfoBlock );
	if ( pMediaHdr ) {
		_ASSERTE( pMediaHdr->nSize != 0 );
		nMediaSize = AlignTo8( pMediaHdr->nSize );
	}

	// Get the Packet Data.
	nDataSize = AlignTo8( InternalGetActualLength( inIPacket ) );
	const UInt8*	pPacketData = nullptr;
	Peek::ThrowIfFailed( inIPacket->GetPacketData( &pPacketData ) );
	Peek::ThrowIf( pPacketData == nullptr );

	// Calculate bytes to allocate.
	nTotalBytes = nHeaderSize + nMediaSize + nDataSize;

	// Get the Media Type and Media Sub Type.
	tMediaType	mt = InternalGetMediaType( inIPacket );
	outInfo.nMediaType = mt.fType;
	outInfo.nMediaSubType = mt.fSubType;

	// Allocate the new buffer.
	UInt8*	pNewData = CPacketData::Alloc( nTotalBytes );
	if ( !pNewData ) throw( E_OUTOFMEMORY );
#ifdef _DEBUG
	memset( pNewData, 0, nTotalBytes );
#endif

	// Copy the Packet Header.
	pNewPacketHeader = reinterpret_cast<PeekPacket*>( pNewData );
	memcpy( pNewData, pPacketHeader, sizeof( PeekPacket ) );

	// Copy the optional Media Specific header.
	if ( nMediaSize > 0 ) {
		pNewMediaHdr = pNewData + nHeaderSize;
		memcpy( pNewMediaHdr, pMediaHdr, pMediaHdr->nSize );
		pNewPacketHeader->fMediaSpecInfoBlock = pNewMediaHdr;
	}

	// Copy the Packet data.
	pNewPacketData = pNewData + nHeaderSize + nMediaSize;
	memcpy( pNewPacketData, pPacketData, InternalGetActualLength( inIPacket ) );

	// Return the new Data.
	outInfo.pPacketHeader = pNewPacketHeader;
	outInfo.pPacketData = pNewPacketData;

	return pNewData;
}

UInt8*
CPacket::AllocFrom(
	const PeekPacket*	inPacket,
	const UInt8*		inData,
	tPacketInfo&		outInfo )
{
	_ASSERTE( inPacket );
	Peek::ThrowIf( inPacket == nullptr );
	if ( inPacket == nullptr ) {
		memset( &outInfo, 0, sizeof( tPacketInfo ) );
		return nullptr;
	}

	size_t		nHeaderSize = 0;
	size_t		nMediaSize = 0;
	size_t		nPacketSize = 0;
	size_t		nDataSize = 0;
	size_t		nTotalBytes = 0;
	PeekPacket*	pNewPacketHeader = nullptr;
	UInt8*		pNewMediaHdr = nullptr;
	UInt8*		pNewPacketData = nullptr;

	// Get the Packet Header.
	nHeaderSize = AlignTo8( sizeof( PeekPacket ) );

	// Get the optional Media Specific header.
	const MediaSpecificPrivateHeader* pMediaHdr =
		reinterpret_cast<const MediaSpecificPrivateHeader*>( inPacket->fMediaSpecInfoBlock );
	if ( pMediaHdr ) {
		_ASSERTE( pMediaHdr->nSize != 0 );
		nMediaSize = AlignTo8( pMediaHdr->nSize );
	}

	// Get the Packet Data size.
	nPacketSize = (inPacket->fSliceLength == 0) ? inPacket->fPacketLength : inPacket->fSliceLength;
	nDataSize = AlignTo8( nPacketSize );

	// Calculate bytes to allocate.
	nTotalBytes = nHeaderSize + nMediaSize + nDataSize;

	// Clear the Media Type and Media Sub Type.
	outInfo.nMediaType = kMediaType_802_3;
	outInfo.nMediaSubType = kMediaSubType_Native;

	// Allocate the new buffer.
	UInt8*	pNewData = CPacketData::Alloc( nTotalBytes );
	if ( !pNewData ) throw( E_OUTOFMEMORY );
#ifdef _DEBUG
	memset( pNewData, 0, nTotalBytes );
#endif

	// Copy the Packet Header.
	pNewPacketHeader = reinterpret_cast<PeekPacket*>( pNewData );
	memcpy( pNewData, inPacket, sizeof( PeekPacket ) );

	// Copy the optional Media Specific header.
	if ( nMediaSize > 0 ) {
		pNewMediaHdr = pNewData + nHeaderSize;
		memcpy( pNewMediaHdr, pMediaHdr, pMediaHdr->nSize );
		pNewPacketHeader->fMediaSpecInfoBlock = pNewMediaHdr;
	}

	// Copy the Packet data.
	pNewPacketData = pNewData + nHeaderSize + nMediaSize;
	memcpy( pNewPacketData, inData, nPacketSize );

	// Return the new Data.
	outInfo.pPacketHeader = pNewPacketHeader;
	outInfo.pPacketData = pNewPacketData;

	return pNewData;
}


// -----------------------------------------------------------------------------
//		Create
//		Create a packet object.
// -----------------------------------------------------------------------------

void
CPacket::Create(
	const PeekPacket*	inPacket,
	const UInt8*		inData,
	tMediaType			inMediaType )
{
	tPacketInfo		theInfo;
	const UInt8*	pData = AllocFrom( inPacket, inData, theInfo );
	_ASSERTE( pData );

	try {
		Create();
		_ASSERTE( IsValid() );
		if ( IsValid() ) {
			SetData( pData );
			// Initialize the new Instance from the Packet Info.
			Peek::ThrowIfFailed( m_spPacket->Initialize( nullptr, 0, theInfo.pPacketHeader,
				theInfo.pPacketData, inMediaType.fType, inMediaType.fSubType ) );
		}
		else {
			CPacketData::Dealloc( pData );
		}
	}
	catch( ... ) {
		CPacketData::Dealloc( pData );
		throw;
	}
}


// -----------------------------------------------------------------------------
//		Detach
//		Create an independent copy of the packet's header and data.
// -----------------------------------------------------------------------------

void
CPacket::Detach()
{
	tPacketInfo		theInfo;
	const UInt8*	pData = AllocFrom( m_spPacket, theInfo );
	_ASSERTE( pData );

	try {
		Create();
		_ASSERTE( IsValid() );
		if ( IsValid() ) {
			SetData( pData );
			// Initialize the new Instance from the Packet Info.
			Peek::ThrowIfFailed( m_spPacket->Initialize( nullptr, 0, theInfo.pPacketHeader,
				theInfo.pPacketData, theInfo.nMediaType, theInfo.nMediaSubType ) );
		}
		else {
			CPacketData::Dealloc( pData );
		}
	}
	catch( ... ) {
		CPacketData::Dealloc( pData );
		throw;
	}
}


// -----------------------------------------------------------------------------
//		GetAllPacketData
// -----------------------------------------------------------------------------

UInt16
CPacket::GetAllPacketData(
	const UInt8*&	outData ) const
{
	if ( IsNotValid() ) {
		outData = nullptr;
		return 0;
	}

	Peek::ThrowIfFailed( m_spPacket->GetPacketData( &outData ) );
	return InternalGetActualLength( m_spPacket );
}


// -----------------------------------------------------------------------------
//		GetLayer
// -----------------------------------------------------------------------------

bool
CPacket::GetLayer(
	CLayerEthernet&	outLayer )
{
	if ( GetLayers() ) {
		CPacketLayerInfo	layerInfo;
		if ( m_Layers.GetLayerInfo( ProtoSpecDefs::IEEE_802_3, layerInfo ) ) {
			outLayer.Set( this, layerInfo.GetHeader(), layerInfo.GetData() );
			return true;
		}
		if ( m_Layers.GetLayerInfo( ProtoSpecDefs::EType2, layerInfo ) ) {
			outLayer.Set( this, layerInfo.GetHeader(), layerInfo.GetData(), ProtoSpecDefs::EType2 );
			return true;
		}
	}
	return false;
}

bool
CPacket::GetLayer(
	CLayerIP&	outLayer )
{
	if ( GetLayers() ) {
		CPacketLayerInfo	layerInfo;
		if ( m_Layers.GetLayerInfo( ProtoSpecDefs::IP, layerInfo ) ) {
			outLayer.Set( this, layerInfo.GetHeader(), layerInfo.GetData(), kIpVersion_4 );
			return true;
		}
		if ( m_Layers.GetLayerInfo( ProtoSpecDefs::IPv6, layerInfo ) ) {
			outLayer.Set( this, layerInfo.GetHeader(), layerInfo.GetData(), kIpVersion_6 );
			return true;
		}
	}
	return false;
}

bool
CPacket::GetLayer(
	CLayerTCP&	outLayer )
{
	if ( GetLayers() ) {
		CPacketLayerInfo	layerInfo;
		if ( m_Layers.GetLayerInfo( ProtoSpecDefs::TCP, layerInfo ) ) {
			outLayer.Set( this, layerInfo.GetHeader(), layerInfo.GetData() );
			return true;
		}
	}
	return false;
}

bool
CPacket::GetLayer(
	CLayerUDP&	outLayer )
{
	if ( GetLayers() ) {
		CPacketLayerInfo	layerInfo;
		if ( m_Layers.GetLayerInfo( ProtoSpecDefs::UDP, layerInfo ) ) {
			outLayer.Set( this, layerInfo.GetHeader(), layerInfo.GetData() );
			return true;
		}
	}
	return false;
}


// -----------------------------------------------------------------------------
//		GetPacketBuffer
// -----------------------------------------------------------------------------

CPacketBuffer
CPacket::GetPacketBuffer() const
{
	CPacketBuffer	thePacketBuffer;
	if ( IsNotValid() || IsDetached() ) return thePacketBuffer;

	IPacketBuffer*	pPacketBuffer = nullptr;
	Peek::ThrowIfFailed( m_spPacket->GetPacketBuffer( &pPacketBuffer ) );
	thePacketBuffer.SetPtr( pPacketBuffer );
	pPacketBuffer->Release();

	return thePacketBuffer;
}


// -----------------------------------------------------------------------------
//		GetPacketData
// -----------------------------------------------------------------------------

UInt16
CPacket::GetPacketData(
	const UInt8*&	outData ) const
{
	if ( IsNotValid() ) {
		outData = nullptr;
		return 0;
	}

	Peek::ThrowIfFailed( m_spPacket->GetPacketData( &outData ) );
	const UInt8*	pEnd = nullptr;
	Peek::ThrowIfFailed( m_spPacket->GetPacketDataEnd( &pEnd ) );
	return static_cast<UInt16>( pEnd - outData );
}


// -----------------------------------------------------------------------------
//		GetPacketLayers
// -----------------------------------------------------------------------------

CPacketLayers
CPacket::GetPacketLayers()
{
	CPacketLayers	Layers;
	if ( IsValid() ) {
		HeLib::CHeQIPtr<IPacketLayers>	spLayers( m_spPacket );
		if ( spLayers == nullptr ) Error();
		Layers.SetPtr( spLayers.p );
	}
	return Layers;
}


// -----------------------------------------------------------------------------
//		GetPacketNumber
// -----------------------------------------------------------------------------

UInt64
CPacket::GetPacketNumber() const
{
	UInt64	nPacketNumber = static_cast<UInt64>( 0 );
	if ( IsValid() && GetPacketBuffer().IsValid() ) {
		Peek::ThrowIfFailed( m_spPacket->GetPacketNumber( &nPacketNumber ) );
	}
	return nPacketNumber;
}


// -----------------------------------------------------------------------------
//		IsEqual
// -----------------------------------------------------------------------------

bool
CPacket::IsEqual(
	const CPacket& inPacket ) const
{
	if ( IsEmpty() && inPacket.IsEmpty() ) return true;
	if ( IsEmpty() || inPacket.IsEmpty() ) return false;

	_ASSERTE( inPacket.IsValid() );
	_ASSERTE( IsValid() );

	// Get the interface pointer to the in packet
	IPacket* pInPacket = inPacket.GetIPacketPtr();
	_ASSERTE ( pInPacket );

	// Validate the length.
	const UInt8*	pPacketData = nullptr;
	const UInt8*	pInPacketData = nullptr;
	UInt16			nLength = GetPacketData( pPacketData );
	UInt16			nInLength = inPacket.GetPacketData( pInPacketData );
	if ( nInLength != nLength ) return false;

	// Validate the packet header.
	const PeekPacket*	pPacketHeader = nullptr;
	const PeekPacket*	pInPacketHeader = nullptr;
	Peek::ThrowIfFailed( m_spPacket->GetPacketHeader( &pPacketHeader ) );
	Peek::ThrowIfFailed( pInPacket->GetPacketHeader( &pInPacketHeader ) );

	int	nResult;
	// Compare the Packet Headers.
	size_t	nCompareLength = sizeof( PeekPacket ) - sizeof( UInt8* );
	nResult = memcmp( pInPacketHeader, pPacketHeader, nCompareLength );
	if ( nResult != 0 ) return false;

	if ( (pPacketHeader->fMediaSpecInfoBlock != nullptr) &&
		 (pInPacketHeader->fMediaSpecInfoBlock != nullptr) ) {
		const MediaSpecificPrivateHeader* pMediaHdr =
			reinterpret_cast<const MediaSpecificPrivateHeader*>( pPacketHeader->fMediaSpecInfoBlock );
		const MediaSpecificPrivateHeader* pInMediaHdr =
			reinterpret_cast<const MediaSpecificPrivateHeader*>( pInPacketHeader->fMediaSpecInfoBlock );
		if ( pInMediaHdr->nSize != pMediaHdr->nSize ) return false;
		nResult = memcmp( pMediaHdr, pInMediaHdr, pMediaHdr->nSize );
		if ( nResult != 0 ) return false;
	}

	// Compare the Packet Data.
	nResult = memcmp( pInPacketData, pPacketData, nLength );
	return (nResult == 0);
}


// -----------------------------------------------------------------------------
//		SizeOf
// -----------------------------------------------------------------------------

size_t
CPacket::SizeOf(
	const CPacket& inPacket )
{
	IPacket*	pPacket = inPacket.GetIPacketPtr();

	_ASSERTE( pPacket );
	Peek::ThrowIf( pPacket == nullptr );
	if ( pPacket == nullptr ) {
		return 0;
	}

	size_t	nHeaderSize = 0;
	size_t	nMediaSize = 0;
	size_t	nDataSize = 0;
	size_t	nSizeOf = 0;

	// Get the Packet Header.
	const PeekPacket*	pPacketHeader;
	Peek::ThrowIfFailed( pPacket->GetPacketHeader( &pPacketHeader ) );
	Peek::ThrowIf( pPacketHeader == nullptr );
	nHeaderSize = AlignTo8( sizeof( PeekPacket ) );

	// Get the optional Media Specific header.
	const MediaSpecificPrivateHeader* pMediaHdr =
		reinterpret_cast<const MediaSpecificPrivateHeader*>( pPacketHeader->fMediaSpecInfoBlock );
	if ( pMediaHdr ) {
		_ASSERTE( pMediaHdr->nSize != 0 );
		nMediaSize = AlignTo8( pMediaHdr->nSize );
	}

	// Get the Packet Data.
	nDataSize = AlignTo8( InternalGetActualLength( pPacket ) );
	const UInt8*	pPacketData = nullptr;
	Peek::ThrowIfFailed( pPacket->GetPacketData( &pPacketData ) );
	Peek::ThrowIf( pPacketData == nullptr );

	// Calculate bytes to allocate.
	nSizeOf = nHeaderSize + nMediaSize + nDataSize;

	return nSizeOf;
}
