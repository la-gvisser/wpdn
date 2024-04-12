// ============================================================================
// Packet.cpp:
//      implementation of the CPacket class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "Packet.h"
#include "Utils.h"


// ============================================================================
//		CPacket
// ============================================================================

CPacket::CPacket(
	UInt32				inProtoSpecMatched,
	UInt8				inMediaType,
	UInt8				inMediaSubType,
	const PluginPacket*	inPacket,
	const UInt8*		inPacketData,
	int					inType /*=kPacketType_Unknown*/ )
	:	m_ProtoSpecMatched( inProtoSpecMatched )
	,	m_MediaType( inMediaType )
	,	m_MediaSubType( inMediaSubType )
	,	m_Packet( *inPacket )
	,	m_nTimeStamp( 0 )
	,	m_nPacketKey( 0 )
	,	m_nType( inType )
{
	m_nTimeStamp = ::GetTimeStamp();
	memset( m_PacketData, 0, sizeof( m_PacketData ) );
	if ( inPacketData && (inPacket->fPacketLength <= GetPacketDataSize()) ) {
		memcpy( m_PacketData, inPacketData, inPacket->fPacketLength );
	}
}

CPacket::CPacket(
	const CIPv4PacketRef&	inRef )
	:	m_ProtoSpecMatched( inRef.GetProtoSpecMatched() )
	,	m_MediaType( inRef.GetMediaType() )
	,	m_MediaSubType( inRef.GetMediaSubType() )
	,	m_Packet( *inRef.GetPacket() )
	,	m_nTimeStamp( inRef.GetTimeStamp() )
	,	m_nPacketKey( inRef.GetPacketKey() )
	,	m_nType( kPacketType_Unknown )
{
	memset( m_PacketData, 0, sizeof( m_PacketData ) );
	if ( inRef.GetPacketData() && (inRef.GetPacketDataLength() < sizeof( m_PacketData )) ) {
		memcpy( m_PacketData, inRef.GetPacketData(), inRef.GetPacketDataLength() );
	}
}


// ----------------------------------------------------------------------------
//		GetLayerData
// ----------------------------------------------------------------------------

const UInt8*
CPacket::GetLayerData(
	UInt8	inLayer,
	UInt16*	outBytesLeft ) const
{
	return theApp.GetPlugin().DoPacketGetLayer(
		(kPacketLayerType_Data | inLayer),
		m_MediaType,
		m_MediaSubType,
		&m_Packet,
		m_PacketData,
		outBytesLeft );
}


// ----------------------------------------------------------------------------
//		GetLayerHeader
// ----------------------------------------------------------------------------

const UInt8*
CPacket::GetLayerHeader(
	UInt8	inLayer,
	UInt16*	outBytesLeft ) const
{
	return theApp.GetPlugin().DoPacketGetLayer(
		(kPacketLayerType_Header | inLayer),
		m_MediaType,
		m_MediaSubType,
		&m_Packet,
		m_PacketData,
		outBytesLeft );
}


// ----------------------------------------------------------------------------
//		SetPacketData
// ----------------------------------------------------------------------------

bool
CPacket::SetPacketData(
	UInt16			inLength,
	const UInt8*	inData )
{
	if ( inLength > sizeof( m_PacketData ) ) return false;
	memcpy( m_PacketData, inData, inLength );
	memset( &m_PacketData[inLength], 0, (sizeof( m_PacketData ) - inLength) );
	m_Packet.fPacketLength = inLength;
	m_Packet.fSliceLength = 0;
	return true;
}


// ============================================================================
//		CIPv4Packet
// ============================================================================

CIPv4Packet::CIPv4Packet(
	const CIPv4Packet*	inPacket )
	:	CPacket( 0, inPacket->GetMediaType(), inPacket->GetMediaSubType(),
			inPacket->GetPacket(), inPacket->GetPacketData(), inPacket->GetPacketType() )
	,	m_nDataOffset( 0 )
{
	UInt16			nBytesLeft = 0;
	const UInt8*	pIpHeader = theApp.GetPlugin().DoPacketGetLayer(
		(kPacketLayerType_Header | kPacketLayer_IP),
		m_MediaType,
		m_MediaSubType,
		&m_Packet,
		m_PacketData,
		&nBytesLeft );
	if ( pIpHeader != NULL ) {
		if ( nBytesLeft >= sizeof( tIPv4Header ) ) {
			if ( m_IPHeader.Init( nBytesLeft, reinterpret_cast<const tIPv4Header*>( pIpHeader ) ) ) {
				m_nPacketKey = 
					(static_cast<UInt64>( m_IPHeader.GetSource() ) << 32) |
					m_IPHeader.GetDestination() |
					(static_cast<UInt64>( m_IPHeader.GetIdentifier() ) << 24);
			}
		}
	}
}

CIPv4Packet::CIPv4Packet(
	const CIPv4PacketRef&	inRef )
	:	CPacket( inRef.GetProtoSpecMatched(), inRef.GetMediaType(), inRef.GetMediaSubType(),
			inRef.GetPacket(), inRef.GetPacketData(), kPacketType_IPv4 )
	,	m_nDataOffset( 0 )
{
	UInt16			nBytesLeft = 0;
	const UInt8*	pIpHeader = theApp.GetPlugin().DoPacketGetLayer(
		(kPacketLayerType_Header | kPacketLayer_IP),
		m_MediaType,
		m_MediaSubType,
		&m_Packet,
		m_PacketData,
		&nBytesLeft );
	if ( pIpHeader != NULL ) {
		if ( nBytesLeft >= sizeof( tIPv4Header ) ) {
			if ( m_IPHeader.Init( nBytesLeft, reinterpret_cast<const tIPv4Header*>( pIpHeader ) ) ) {
				m_nPacketKey = 
					(static_cast<UInt64>( m_IPHeader.GetSource() ) << 32) |
					m_IPHeader.GetDestination() |
					(static_cast<UInt64>( m_IPHeader.GetIdentifier() ) << 24);
			}
		}
	}
}


// ----------------------------------------------------------------------------
//		GetPacketKey
// ----------------------------------------------------------------------------

UInt64
CIPv4Packet::GetPacketKey() const
{
	UInt64 nPacketKey =
		(static_cast<UInt64>( static_cast<UInt32>( m_IPHeader.GetSource() ) ) << 32) |
		(static_cast<UInt64>( static_cast<UInt32>( m_IPHeader.GetDestination() ) ) << 16) |
		GetIPHeader().GetIdentifier();
	return nPacketKey;
}


// ----------------------------------------------------------------------------
//		SetIPHeader
// ----------------------------------------------------------------------------

void
CIPv4Packet::SetIPHeader()
{
	UInt16			nBytesLeft = 0;
	const UInt8*	pIpHeader = theApp.GetPlugin().DoPacketGetLayer(
		(kPacketLayerType_Header | kPacketLayer_IP),
		m_MediaType,
		m_MediaSubType,
		&m_Packet,
		m_PacketData,
		&nBytesLeft );
	if ( pIpHeader != NULL ) {
		if ( nBytesLeft >= sizeof( tIPv4Header ) ) {
			if ( m_IPHeader.Init( nBytesLeft, reinterpret_cast<const tIPv4Header*>( pIpHeader ) ) ) {
				m_nPacketKey = 
					(static_cast<UInt64>( m_IPHeader.GetSource() ) << 32) |
					m_IPHeader.GetDestination() |
					(static_cast<UInt64>( m_IPHeader.GetIdentifier() ) << 24);
			}
		}
	}
}


// ============================================================================
//		CIPv4PacketRef
// ============================================================================

CIPv4PacketRef::CIPv4PacketRef(
	UInt32				inProtoSpecMatched,
	UInt8				inMediaType,
	UInt8				inMediaSubType,
	const PluginPacket*	inPacket,
	const UInt8*		inPacketData )
	:	m_ProtoSpecMatched( inProtoSpecMatched )
	,	m_MediaType( inMediaType )
	,	m_MediaSubType( inMediaSubType )
	,	m_pPacket( inPacket )
	,	m_pPacketData( inPacketData )
{
	{
		const UInt8*	pIpHeader( NULL );
		UInt16			nIpHeaderLength( 0 );
		const UInt8*	pPayload( NULL );
		UInt16			nPayloadLength( 0 );
		UInt32			nDeliveryProtocol( 0 );
		int				nResult = theApp.GetPlugin().GetIPInfo(
			m_ProtoSpecMatched,
			m_pPacket,
			m_pPacketData,
			m_MediaType,
			m_MediaSubType,
			&pIpHeader,
			&nIpHeaderLength,
			&pPayload,
			&nPayloadLength,
			&nDeliveryProtocol );
		if ( (nResult < 0) || (nIpHeaderLength < sizeof( tIPv4Header )) ) return;
		m_IPHeader.Init( nIpHeaderLength, reinterpret_cast<const tIPv4Header*>( pIpHeader ) );
	}
	{
		UInt16			nBytesLeft = 0;
		const UInt8*	pIpHeader = theApp.GetPlugin().DoPacketGetLayer(
			(kPacketLayerType_Header | kPacketLayer_IP),
			m_MediaType,
			m_MediaSubType,
			m_pPacket,
			m_pPacketData,
			&nBytesLeft );
		if ( pIpHeader != NULL ) {
			if ( nBytesLeft >= sizeof( tIPv4Header ) ) {
				m_IPHeader.Init( nBytesLeft, reinterpret_cast<const tIPv4Header*>( pIpHeader ) );
			}
		}
	}
}


// ----------------------------------------------------------------------------
//		IsDescendentOf
// ----------------------------------------------------------------------------

bool
CIPv4PacketRef::IsDescendentOf(
	UInt32	inProtocol ) const
{
	UInt32	ayProtocols[] = { inProtocol };
	UInt32	nInstance( 0 );
	int		nResult = theApp.GetPlugin().DoIsDescendentOf(
		m_ProtoSpecMatched,
		ayProtocols,
		COUNTOF( ayProtocols ),
		&nInstance );
	return (nResult == PLUGIN_RESULT_SUCCESS );
}


// ----------------------------------------------------------------------------
//		GetLayerData
// ----------------------------------------------------------------------------

const UInt8*
CIPv4PacketRef::GetLayerData(
	UInt8	inLayer,
	UInt16*	outBytesLeft ) const
{
	return theApp.GetPlugin().DoPacketGetLayer(
		(kPacketLayerType_Data | inLayer),
		m_MediaType,
		m_MediaSubType,
		m_pPacket,
		m_pPacketData,
		outBytesLeft );
}


// ----------------------------------------------------------------------------
//		GetLayerHeader
// ----------------------------------------------------------------------------

const UInt8*
CIPv4PacketRef::GetLayerHeader(
	UInt8	inLayer,
	UInt16*	outBytesLeft ) const
{
	return theApp.GetPlugin().DoPacketGetLayer(
		(kPacketLayerType_Header | inLayer),
		m_MediaType,
		m_MediaSubType,
		m_pPacket,
		m_pPacketData,
		outBytesLeft );
}


// ----------------------------------------------------------------------------
//		GetPacketKey
// ----------------------------------------------------------------------------

UInt64
CIPv4PacketRef::GetPacketKey() const
{
	UInt64 nPacketKey =
		(static_cast<UInt64>( GetIPHeader().GetSource() ) << 32) |
		(static_cast<UInt64>( GetIPHeader().GetDestination() ) << 16) |
		GetIPHeader().GetIdentifier();
	return nPacketKey;
}


// ----------------------------------------------------------------------------
//		GetUDPDestinationPort
// ----------------------------------------------------------------------------

UInt16
CIPv4PacketRef::GetUDPDestinationPort() const
{
	const tUdpHeader*	pUdpHeader = reinterpret_cast<const tUdpHeader*>( m_IPHeader.GetPayload() );
	if ( pUdpHeader == NULL ) return 0;
	return NETWORKTOHOST16( pUdpHeader->DestinationPort );
}


// ----------------------------------------------------------------------------
//		GetUDPSourcePort
// ----------------------------------------------------------------------------

UInt16
CIPv4PacketRef::GetUDPSourcePort() const
{
	const tUdpHeader*	pUdpHeader = reinterpret_cast<const tUdpHeader*>( m_IPHeader.GetPayload() );
	if ( pUdpHeader == NULL ) return 0;
	return NETWORKTOHOST16( pUdpHeader->SourcePort );
}


// ----------------------------------------------------------------------------
//		GetUDPSourcePort
// ----------------------------------------------------------------------------

bool
CIPv4PacketRef::HasUDPHeader() const {
	int				nProtoSpec = ProtoSpecDefs::UDP;
	UInt16			nBytesLeft( 0 );
	const UInt8*	pUdpHeader( GetLayerData( nProtoSpec, &nBytesLeft ) );
	return (pUdpHeader != NULL);
}


// ============================================================================
//		CUDPPacket
// ============================================================================

CUDPPacket::CUDPPacket(
	const CIPv4PacketRef&	inRef )
	:	CIPv4Packet( inRef )
{
	m_nType = kPacketType_UDP;

	UInt16			nBytesLeft = 0;
	const UInt8*	pUdpHeader = theApp.GetPlugin().DoPacketGetLayer(
		(kPacketLayerType_Header | kPacketLayer_UDP),
		inRef.GetMediaType(),
		inRef.GetMediaSubType(),
		inRef.GetPacket(),
		inRef.GetPacketData(),
		&nBytesLeft );
	if ( pUdpHeader != NULL ) {
		m_UDPHeader.Init( nBytesLeft, reinterpret_cast<const tUdpHeader*>( pUdpHeader ) );
	}
}
