// ============================================================================
// Packet.h:
//      interface for the CPacket class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

class CIPv4PacketRef;


// ============================================================================
//		CPacket
// ============================================================================

class CPacket
{
public:
	enum {
		kPacketType_Unknown,
		kPacketType_IPv4,
		kPacketType_TCP,
		kPacketType_UDP
	};

protected:
	int				m_nType;
	UInt32			m_ProtoSpecMatched;
	UInt8			m_MediaType;
	UInt8			m_MediaSubType;
	PluginPacket	m_Packet;
	UInt64			m_nTimeStamp;
	UInt64			m_nPacketKey;
	UInt8			m_PacketData[1536];	// 1518 + tEthernetHeader + tVLANHeader

public:
	CPacket()
		:	m_nType( kPacketType_Unknown )
		,	m_ProtoSpecMatched( 0 )
		,	m_MediaType( 0 )
		,	m_MediaSubType( 0 )
		,	m_nTimeStamp( 0 )
		,	m_nPacketKey( 0 )
	{
		memset( &m_PacketData, 0, sizeof( m_PacketData ) );
	}
	CPacket( UInt32 inProtoSpecMatched, UInt8 inMediaType, UInt8 inMediaSubType,
				const PluginPacket* inPacket, const UInt8* inPacketData = NULL,
				int inType = kPacketType_Unknown );
	CPacket( const CIPv4PacketRef& inRef );
	~CPacket() {}

	const UInt8*	GetLayerData( UInt8 inLayer, UInt16* outBytesLeft ) const;
	const UInt8*	GetLayerHeader( UInt8 inLayer, UInt16* outBytesLeft ) const;
	UInt8		GetMediaType() const { return m_MediaType; }
	UInt8		GetMediaSubType() const { return m_MediaSubType; }
	const PluginPacket*	GetPacket() const { return &m_Packet; }
	const UInt8*		GetPacketData() const { return m_PacketData; }
	UInt16		GetPacketDataSize() const { return sizeof( m_PacketData ); }
	UInt16		GetPacketLength() const { return m_Packet.fPacketLength; }
	UInt64		GetPacketTimeStamp() const { return m_Packet.fTimeStamp; }
	UInt64		GetPacketKey() const { return m_nPacketKey; }
	UInt32		GetProtoSpecMatched() const { return m_ProtoSpecMatched; }
	UInt32		GetSrcIP() const { return 0; }
	UInt64		GetTimeStamp() const { return m_nTimeStamp; }
	int			GetPacketType() const { return m_nType; }

	bool		IsPacketType( int inType ) const { return (m_nType == inType); }

	UInt8*		LockPacketData() { return m_PacketData; }
	void		ReleasePacketData( UInt16 inLength ) { m_Packet.fPacketLength = inLength; }
	bool		SetPacketData( UInt16 inLength, const UInt8* inData );
};


// ============================================================================
//		CIPv4Packet
// ============================================================================

class CIPv4Packet
	:	public CPacket
{
protected:
	CIPv4Header		m_IPHeader;
	UInt16			m_nDataOffset;	// CMI Security Id and Ericsson header size.

public:
	CIPv4Packet( const CIPv4Packet* inPacket );
	CIPv4Packet( const CIPv4PacketRef& inRef );
	~CIPv4Packet() {}

	UInt16				GetDataOffset() const { return m_nDataOffset; }
	const CIPv4Header&	GetIPHeader() const { return m_IPHeader; }
	UInt64				GetPacketKey() const;

	bool				IsValid() const { return m_IPHeader.IsValid(); }

	void				SetDataOffset( UInt16 inOffset ) { m_nDataOffset = inOffset; }
	void				SetIPHeader();
};


// ============================================================================
//		CIPv4PacketRef
// ============================================================================

class CIPv4PacketRef
{
protected:
	UInt32				m_ProtoSpecMatched;
	UInt8				m_MediaType;
	UInt8				m_MediaSubType;
	const PluginPacket*	m_pPacket;
	const UInt8*		m_pPacketData;
	CIPv4Header			m_IPHeader;

public:
	;		CIPv4PacketRef( UInt32 inProtoSpecMatched, UInt8 inMediaType, UInt8 inMediaSubType,
					const PluginPacket* inPacket, const UInt8* inPacketData );
	;		~CIPv4PacketRef() {}

	bool				IsDescendentOf( UInt32 nProtocol ) const;
	bool				IsIP() const { return m_IPHeader.IsValid(); }
	const CIPv4Header&	GetIPHeader() const { return m_IPHeader; }
	const UInt8*		GetLayerData( UInt8 inLayer, UInt16* outBytesLeft ) const;
	const UInt8*		GetLayerHeader( UInt8 inLayer, UInt16* outBytesLeft ) const;

	UInt32				GetProtoSpecMatched() const { return m_ProtoSpecMatched; }
	UInt8				GetMediaType() const { return m_MediaType; }
	UInt8				GetMediaSubType() const { return m_MediaSubType; }
	const PluginPacket*	GetPacket() const { return m_pPacket; }
	const UInt8*		GetPacketData() const { return m_pPacketData; }
	UInt16				GetPacketDataLength() const { return (m_pPacket) ? m_pPacket->fPacketLength : 0; }
	UInt64				GetPacketKey() const;
	UInt8				GetPayloadProtocol() const { return m_IPHeader.GetPayloadProtocol(); }
	UInt16				GetUDPDestinationPort() const;
	UInt16				GetUDPSourcePort() const;
	UInt64				GetTimeStamp() const { return (m_pPacket) ? m_pPacket->fTimeStamp : 0; }

	bool				HasUDPHeader() const;
};


// ============================================================================
//		CUDPPacket
// ============================================================================

class CUDPPacket
	:	public CIPv4Packet
{
protected:
	CUDPHeader			m_UDPHeader;

public:
	CUDPPacket( const CIPv4PacketRef& inRef );
	~CUDPPacket() {}

	const CUDPHeader&	GetUDPHeader() const { return m_UDPHeader; }
	bool				IsValid() const { return m_UDPHeader.IsValid(); }
};
