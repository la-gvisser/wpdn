// =============================================================================
//	Layers.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

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
	const UInt8*	pPacketData = NULL;
	UInt16			nPacketLen = m_pPacket->GetPacketData( pPacketData );
	ASSERT( pHeader >= pPacketData );
	UInt16			nOffset = static_cast<UInt16>( pHeader - pPacketData );
	ASSERT( nOffset <= nPacketLen );  nPacketLen;
	return nOffset;
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
		m_Header.Set( pHeader );
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
	:	m_pHeader( NULL )
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
		return m_pHeader->IsFragment();
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
			m_IpV4Header.Set( pHeader );
			m_pHeader = &m_IpV4Header;
		}
	}
	else if ( inVersion == kIpVersion_6 ) {
		const tIpV6Header*	pHeader =
			reinterpret_cast<const tIpV6Header*>( inHeader.GetData( sizeof( tIpV6Header ) ) );
		if ( pHeader ) {
			m_nType = ProtoSpecDefs::IPv6;
			CLayer::Set( inPacket, ProtoSpecDefs::IPv6, inHeader, inData );
			m_IpV6Header.Set( pHeader );
			m_pHeader = &m_IpV6Header;
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
	m_Header.Set( reinterpret_cast<const tTcpHeader*>( inHeader.GetData( sizeof( tTcpHeader ) ) ) );
	if ( m_Header.IsValid() ) {
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
	m_Header.Set( reinterpret_cast<const tUdpHeader*>( inHeader.GetData( sizeof( tUdpHeader ) ) ) );
	if ( m_Header.IsValid() ) {
		m_nType = ProtoSpecDefs::UDP;
		CLayer::Set( inPacket, ProtoSpecDefs::UDP, inHeader, inData );
	}
}