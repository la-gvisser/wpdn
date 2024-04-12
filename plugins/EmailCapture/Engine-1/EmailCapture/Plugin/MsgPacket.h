// ============================================================================
//	MsgPacket.h
//		interface for the CMsgPacket class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "IpHeaders.h"
#include "PeekPacket.h"
#include "Packet.h"
#include "ByteVectRef.h"
#include "MsgText.h"
#include <vector>

enum _TCPFlag {
	kTCPFlag_FIN = 0x01,
	kTCPFlag_SYN = 0x02,
	kTCPFlag_RST = 0x04,
	kTCPFlag_PSH = 0x08,
	kTCPFlag_ACK = 0x10,
	kTCPFlag_URG = 0x20
};

class CPeekPlugin;

inline UInt64 AlignTo8( UInt64 in ) { return ((in + 7) & ~7); }


// ============================================================================
//		CMsgPacket
// ============================================================================

class CMsgPacket
{
public:
	enum PacketTypes {
		kPacketType_Text,
		kPacketType_BDAT,
		kPacketType_Xexch50
	};

protected:
	CPacket				m_Packet;
	PacketTypes			m_Type;

	CByteVectRef		m_bvPacketData;
	CLayerIP			m_layIp;
	CLayerTCP			m_layTcp;
	CByteVectRef		m_bvPayload;

	UInt32				m_nDeliveryProtocol;
	UInt32				m_nSequenceNumber;
	UInt8				m_nTcpFlags;
	SInt16				m_nSliceLength;
	CIpAddressPortPair	m_IpPortPair;
	MsgDirection		m_Direction;
	CMsgText			m_strMsgText;

protected:
	void	SetDirection( MsgDirection inDirection ) { m_Direction = inDirection; }

public:
//	;		CMsgPacket() : m_Type( kPacketType_Text ), m_nSequenceNumber( 0 ), m_nSliceLength( PLUGIN_ACCEPT_WHOLE_PACKET ) {}
	;		CMsgPacket( const CPacket& inPacket );

	bool		DoesSequenceNumberWrap() const { return (m_nSequenceNumber > GetLastSequenceNumber()); }

	//bool						GetAddress( CPeekStringA& outAddress ) const { return m_strMsgText.GetAddressA( outAddress ); }
	MsgDirection				GetDirection() const { return m_Direction; }
	UInt32						GetFirstSequenceNumber() const { return m_nSequenceNumber; }
	const CIpAddressPortPair&	GetIpPortPair() const { return m_IpPortPair; }
	UInt32						GetLastSequenceNumber() const { return (m_nSequenceNumber + m_strMsgText.GetLength32()); }
	size_t						GetLength() const { return m_strMsgText.GetLength(); }
	const CPacket&				GetPacket() const { return m_Packet; }
	CByteVectRef				GetPacketData() const { return m_bvPacketData; }
	CByteVectRef				GetPayload() const { return m_bvPayload; }
	bool						GetResponse( UInt32& outResponse ) const;
	SInt16						GetSliceLength() const { return m_nSliceLength; }
	const CPeekStringA&			GetStringA() const { return m_strMsgText; }
	PacketTypes					GetType() const { return m_Type; }

	bool		IsAck() const { return m_layTcp.GetHeaderRef().IsAck(); }
	bool		IsBdat() const { return m_strMsgText.IsBdat(); }
	bool		IsClose() const;
	bool		IsData() const { return m_strMsgText.IsData(); }
	bool		IsDirection( MsgDirection inDirection ) const { return (inDirection == m_Direction); }
	bool		IsEmpty() const { return m_strMsgText.IsEmpty(); }
	bool		IsEndOfMsg() const { return m_strMsgText.IsEndOfMsg(); }
	bool		IsError() const { return m_strMsgText.IsError(); }
	bool		IsFromServer() const { return (m_Direction == kDirection_FromServer); }
	bool		IsMailFrom() const { return m_strMsgText.IsMailFrom(); }
	bool		IsPlusOK() const { return m_strMsgText.IsPlusOK(); }
	bool		IsQuit() const { return m_strMsgText.IsQuit(); }
	bool		IsRcptTo() const { return m_strMsgText.IsRcptTo(); }
	bool		IsReset() const { return m_strMsgText.IsReset(); }
	bool		IsRetrieve() const { return m_strMsgText.IsRetrieve(); }
	bool		IsToServer() const { return (m_Direction == kDirection_ToServer); }
	bool		IsType( PacketTypes inType ) const { return (m_Type == inType); }
	bool		IsXexch50() const { return m_strMsgText.IsXexch50(); }

	bool		SetInfo( UInt32& outFailure, CPeekString& outMessage );
	void		SetType( PacketTypes inType ) { m_Type = inType; }
};


// ============================================================================
//		CMsgPacketPtr
// ============================================================================

typedef CAutoPtr<CMsgPacket>		CMsgPacketPtr;


// ============================================================================
//		CMsgPacketList
// ============================================================================

class CMsgPacketList
	:	public CAutoPtrArray<CMsgPacket>
{
public:
	;		CMsgPacketList() {}

	UInt64	GetBufferSize( size_t inInitIndex = 0 ) const {
		UInt64	nSize = 4096;
		size_t	nCount = GetCount();
		for ( size_t i = inInitIndex; i < nCount; i++ ) {
			const CMsgPacket*	pMsgPacket = GetAt( i );
			const CPacket&		pkPacket = pMsgPacket->GetPacket();
			nSize += AlignTo8( pkPacket.GetPacketDataLength() + 64 );
			nSize += AlignTo8( pkPacket.GetMediaSpecificInfo() );
		}
		return nSize;
	}
	UInt32	GetCount32() const { return (GetCount() < kIndex_Invalid) ? (GetCount() & 0x0FFFFFFFF) : 0; }
	size_t	GetTotalPacketLength() const {
		size_t	nSize = 0;
		size_t	nCount( GetCount() );
		for ( size_t i = 0; i < nCount; i++ ) {
			const CMsgPacket*	pMsgPacket = GetAt( i );
			_ASSERTE( pMsgPacket );
			if ( pMsgPacket ) nSize += pMsgPacket->GetLength();
		}
		return nSize;
	}
};
