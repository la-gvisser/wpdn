// =============================================================================
//	Layers.cpp
// =============================================================================
//	Copyright (c) 2009-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "Layers.h"
#include "Packet.h"


// =============================================================================
//		CLayer
// =============================================================================

// -----------------------------------------------------------------------------
//		GetOffset
// -----------------------------------------------------------------------------

UInt16
CLayer::GetOffset() const
{
	const UInt8*	pHeader = m_bvHeader.GetData( 1 );
	const UInt8*	pPacketData = nullptr;
	UInt16			nPacketLen = m_pPacket->GetPacketData( pPacketData );
	_ASSERTE( pHeader >= pPacketData );
	UInt16			nOffset = static_cast<UInt16>( pHeader - pPacketData );
	_ASSERTE( nOffset <= nPacketLen );
	return (nOffset < nPacketLen) ? nOffset : nPacketLen;
}


// =============================================================================
//		CLayerEthernet
// =============================================================================

CLayerEthernet::CLayerEthernet(
	CPacket*		inPacket,
	CByteVectRef	inHeader,
	CByteVectRef	inData )
{
	Set( inPacket, inHeader, inData );
}


// -----------------------------------------------------------------------------
//		Set
// -----------------------------------------------------------------------------

void
CLayerEthernet::Set(
	CPacket*		inPacket,
	CByteVectRef	inHeader,
	CByteVectRef	inData,
	UInt16			inProtoSpecId /*= ProtoSpecDef::IEEE_802_3*/ )
{
	const tEthernetHeader*	pHeader =
		reinterpret_cast<const tEthernetHeader*>( inHeader.GetData( sizeof( tEthernetHeader ) ) );
	if ( pHeader ) {
		m_nType = inProtoSpecId;
		CLayer::Set( inPacket, inProtoSpecId, inHeader, inData );
		m_HeaderRef.Set( pHeader );
	}
}


// =============================================================================
//		CLayerIP
// =============================================================================

CLayerIP::CLayerIP(
	CPacket*		inPacket,
	CByteVectRef	inHeader,
	CByteVectRef	inData,
	UInt32			inVersion )
	:	m_pHeaderRef( nullptr )
{
	Set( inPacket, inHeader, inData, inVersion );
}


// ----------------------------------------------------------------------------
//		IsFragment
// ----------------------------------------------------------------------------

bool
CLayerIP::IsFragment() const
{
	if ( IsValid() ) {
		return m_pHeaderRef->IsFragment();
	}
	return false;
}


// -----------------------------------------------------------------------------
//		Set
// -----------------------------------------------------------------------------

void
CLayerIP::Set(
	CPacket*		inPacket,
	CByteVectRef	inHeader,
	CByteVectRef	inData,
	UInt32			inVersion )
{
	m_pPacket = inPacket;
	if ( inVersion == kIpVersion_4 ) {
		const tIpV4Header*	pHeader =
			reinterpret_cast<const tIpV4Header*>( inHeader.GetData( sizeof( tIpV4Header ) ) );
		if ( pHeader ) {
			m_nType = ProtoSpecDefs::IP;
			CLayer::Set( inPacket, ProtoSpecDefs::IP, inHeader, inData );
			m_IpV4HeaderRef.Set( pHeader );
			m_pHeaderRef = &m_IpV4HeaderRef;
		}
	}
	else if ( inVersion == kIpVersion_6 ) {
		const tIpV6Header*	pHeader =
			reinterpret_cast<const tIpV6Header*>( inHeader.GetData( sizeof( tIpV6Header ) ) );
		if ( pHeader ) {
			m_nType = ProtoSpecDefs::IPv6;
			CLayer::Set( inPacket, ProtoSpecDefs::IPv6, inHeader, inData );
			m_IpV6HeaderRef.Set( pHeader );
			m_pHeaderRef = &m_IpV6HeaderRef;
		}
	}
	else {
		Reset();
	}
}


// =============================================================================
//		CLayerTCP
// =============================================================================

CLayerTCP::CLayerTCP(
	CPacket*		inPacket,
	CByteVectRef	inHeader,
	CByteVectRef	inData )
{
	Set( inPacket, inHeader, inData );
}


// -----------------------------------------------------------------------------
//		Set
// -----------------------------------------------------------------------------

void
CLayerTCP::Set(
	CPacket*		inPacket,
	CByteVectRef	inHeader,
	CByteVectRef	inData )
{
	m_pPacket = inPacket;
	m_HeaderRef.Set( reinterpret_cast<const tTcpHeader*>(
		inHeader.GetData( sizeof( tTcpHeader ) ) ) );
	if ( m_HeaderRef.IsValid() ) {
		m_nType = ProtoSpecDefs::TCP;
		CLayer::Set( inPacket, ProtoSpecDefs::TCP, inHeader, inData );
	}
}


// =============================================================================
//		CLayerUDP
// =============================================================================

// -----------------------------------------------------------------------------
//		Set
// -----------------------------------------------------------------------------

void
CLayerUDP::Set(
	CPacket*		inPacket,
	CByteVectRef	inHeader,
	CByteVectRef	inData )
{
	m_pPacket = inPacket;
	m_HeaderRef.Set( reinterpret_cast<const tUdpHeader*>( inHeader.GetData( sizeof( tUdpHeader ) ) ) );
	if ( m_HeaderRef.IsValid() ) {
		m_nType = ProtoSpecDefs::UDP;
		CLayer::Set( inPacket, ProtoSpecDefs::UDP, inHeader, inData );
	}
}
