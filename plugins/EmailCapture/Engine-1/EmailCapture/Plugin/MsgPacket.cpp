// ============================================================================
//	MsgPacket.cpp
//		implementation of the CMsgPacket class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "MsgPacket.h"
#include "MsgStream.h"


// ============================================================================
//		CMsgPacket
// ============================================================================

CMsgPacket::CMsgPacket(
	const CPacket&	inPacket )
	:	m_Packet( inPacket, true )
	,	m_Type( kPacketType_Text )
	,	m_nDeliveryProtocol( 0 )
	,	m_nSequenceNumber( 0 )
	,	m_nTcpFlags( 0 )
	,	m_Direction( kDirection_FromServer )
	,	m_nSliceLength( PLUGIN_ACCEPT_WHOLE_PACKET )
{
}


// ----------------------------------------------------------------------------
//		GetResponse
// ----------------------------------------------------------------------------

bool
CMsgPacket::GetResponse(
	UInt32&	outResponse ) const
{
	if ( m_Direction != kDirection_FromServer ) return false;
	if ( m_strMsgText.IsEmpty() ) {
		return false;
	}
	return m_strMsgText.GetResponse( outResponse );
}


// ----------------------------------------------------------------------------
//		IsClose
// ----------------------------------------------------------------------------

bool
CMsgPacket::IsClose() const
{
	if ( (m_nTcpFlags & kTCPFlag_FIN) ||
		 (m_nTcpFlags & kTCPFlag_SYN) ||
		 (m_nTcpFlags & kTCPFlag_RST) ) {
		return true;
	}
	return false;
}


// ----------------------------------------------------------------------------
//		SetInfo
// ----------------------------------------------------------------------------

bool
CMsgPacket::SetInfo(
	UInt32&			outFailure,
	CPeekString&	outMessage )
{
	// Reject over sized packets.
	tMediaType	mt = m_Packet.GetMediaType();
	switch ( mt.fSubType ) {
		case kMediaSubType_Native:
			if ( m_Packet.GetPacketLength() > 1518 ) return false;
			break;

		case kMediaSubType_802_11_b:
		case kMediaSubType_802_11_a:
		case kMediaSubType_802_11_gen:
			if ( m_Packet.GetPacketLength() > 4096 ) return false;
			break;

		default:
			return false;
	}

	m_bvPacketData = m_Packet.GetPacketDataRef();

	try {
		if ( !m_Packet.GetLayer( m_layIp ) ) return false;
	}
	catch ( ... ) {
		outFailure = 'MSP1';
		outMessage = L"Failed to get packets IP layer.";
		return false;
	}
	if ( m_layIp.IsFragment() ) return false;

	try {
		if ( !m_Packet.GetLayer( m_layTcp ) ) return false;
	}
	catch ( ... ) {
		outFailure = 'MSP2';
		outMessage = L"Failed to get packet's TCP layer.";
		return false;

	}

	CTcpHeaderRef	hdrTcp( m_layTcp.GetHeaderRef() );
	size_t			nTcpPayloadLength( 0 );

	try {
		if ( m_layIp.GetVersion() == kIpVersion_4 ) {
			CIpV4HeaderRef	hdrIp( m_layIp.GetIp4HeaderRef() );
			m_IpPortPair.Set( hdrIp, hdrTcp );
			SetDirection( 
				(m_IpPortPair.GetSrcPort() < m_IpPortPair.GetDstPort())
				? kDirection_FromServer
				: kDirection_ToServer );
			m_nTcpFlags = hdrTcp.GetFlags();
			m_nSequenceNumber = hdrTcp.GetSequenceNumber();

			nTcpPayloadLength = GetTcpPayloadLength( hdrIp, hdrTcp );
		}
	}
	catch ( ... ) {
		outFailure = 'MSP3';
		outMessage = L"Failed to get packet's IP v4 header.";
		return false;
	}

	try {
		if ( m_layIp.GetVersion() == kIpVersion_6 ) {
			CIpV6HeaderRef	hdrIp = m_layIp.GetIp6HeaderRef();
			m_IpPortPair.Set( hdrIp, hdrTcp );
			m_nTcpFlags = hdrTcp.GetFlags();
			m_nSequenceNumber = hdrTcp.GetSequenceNumber();

			nTcpPayloadLength = GetTcpPayloadLength( hdrIp, hdrTcp );
		}
	}
	catch ( ... ) {
		outFailure = 'MSP4';
		outMessage = L"Failed to get packet's IP v6 header.";
		return false;
	}

	m_bvPayload = m_layTcp.GetRemainingRef().Left( nTcpPayloadLength );
	m_strMsgText.Set( m_bvPayload );

	m_nSliceLength = m_layTcp.GetOffset() + m_layTcp.GetHeaderLength();

	return true;
}
