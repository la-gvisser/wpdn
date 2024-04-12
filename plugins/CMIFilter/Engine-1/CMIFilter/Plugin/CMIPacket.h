// =============================================================================
//	CMIPacket.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "Packet.h"
#include "CMIHeaders.h"
#include <memory>

class CCmiPacket;
class CCmiIpV4Packet;

typedef std::unique_ptr<CCmiPacket>		CCmiPacketPtr;
typedef std::unique_ptr<CCmiIpV4Packet>	CCmiIpV4PacketPtr;
typedef std::unique_ptr<UInt8>			CCmiPacketAllocPtr;


// ============================================================================
//		CCmiPacket
// ============================================================================

class CCmiPacket
	: public CPacket
{
public:
	enum {
		kPacketType_Unknown,
		kPacketType_IPv4,
		kPacketType_TCP,
		kPacketType_UDP
	};

protected:
	int					m_nPacketType;
	CPacketLayers		m_CmiLayers;
	CCmiPacketAllocPtr	m_spAllocData;
	tMediaType			m_MediaType;

public:
	;		CCmiPacket()
			:	m_nPacketType( kPacketType_Unknown )
	{
	}
	;		CCmiPacket( const CPacket& inPacket ) 
			:	CPacket( inPacket, true )
			,	m_nPacketType( kPacketType_Unknown )
	{
	}
	virtual	~CCmiPacket() {}

	UInt8*	CmiAllocFrom( CCmiIpV4PacketPtr& inPacket, UInt16 inPacketSize );

	virtual bool	GetLayer() {
		ASSERT( 0 );
		return false;
	}
	PeekPacket*		GetPacketHeader() {
		PeekPacket*	pPacketHeader = nullptr;
		Peek::ThrowIfFailed( m_spPacket->GetPacketHeader( const_cast<const PeekPacket**>( &pPacketHeader ) ) );
		Peek::ThrowIf( pPacketHeader == nullptr );
		return pPacketHeader;
	}
	bool			GetLayers() {
		if ( !HaveLayers() ) {
			m_CmiLayers = GetPacketLayers();
		}
		return HaveLayers();
	}
	int				GetPacketType() const { return m_nPacketType; }

	bool	Initialize( /*const CmiPacketPtr& inPacketFrom, const std::vector<UInt8>& inPacketData*/ );
	bool	IsPacketType( int inType ) const { return (m_nPacketType == inType); }

	bool	HaveLayers() const { return m_CmiLayers.IsValid(); }
};


// ============================================================================
//		CCmiIpV4Packet
// ============================================================================

class CCmiIpV4Packet
	:	public CCmiPacket
{
protected:
	;	CCmiIpV4Packet() {}

protected:
	CCmiIpV4Header	m_IpV4Header;
	UInt16			m_nDataOffset;

public:
	;		CCmiIpV4Packet( const CPacket& inPacket );
	virtual	~CCmiIpV4Packet() {}

	UInt16					GetDataOffset() const { return m_nDataOffset; }
	const CCmiIpV4Header&	GetIPHeader() const { return m_IpV4Header; }
	virtual	bool			GetLayer();
	UInt64					GetPacketKey() const;
	UInt16					GetFragmentOffset() const { return m_IpV4Header.GetFragmentOffset(); }
	UInt16					GetFragmentOffsetBytes() const { return m_IpV4Header.GetFragmentOffsetBytes(); }
	const UInt8*			GetPayload() const { return m_IpV4Header.GetPayload(); }
	UInt16					GetPayloadLength() const { return m_IpV4Header.GetPayloadLength(); }
	CIpAddress				GetSource() const { return m_IpV4Header.GetSource(); }

	bool			IsMoreFragments() { return m_IpV4Header.IsMoreFragments(); }
	virtual	bool	IsValid() const { return m_IpV4Header.IsValid(); }

	void	SetDataOffset( UInt16 inOffset ) { m_nDataOffset = inOffset; }
};


// ============================================================================
//		CCmiUdpPacket
// ============================================================================

class CCmiUdpPacket
	:	public CCmiIpV4Packet
{
protected:
	;	CCmiUdpPacket()	{}

protected:
	CCmiUdpHeader	m_CmiUdpHeader;

protected:
	void	ReleaseLayers() { m_CmiLayers.ReleasePtr(); }

public:
	;		CCmiUdpPacket( const CPacket& inPacket )
			:	CCmiIpV4Packet( inPacket )
	{
		GetLayer();
	}
	virtual	~CCmiUdpPacket() {}

	UInt16				GetDestinationPort() const {
		return m_CmiUdpHeader.GetDestinationPort();
	}
	virtual bool		GetLayer();
	const UInt8*		GetPayload() const {
		return m_CmiUdpHeader.GetPayload();
	}
	UInt16				GetPayloadLength() const {
		return m_CmiUdpHeader.GetPayloadLength();
	}
	UInt16				GetPayloadDataLength() const { 
		return m_CmiUdpHeader.GetPayloadDataLength();
	}
	UInt16				GetSourcePort() const {
		return m_CmiUdpHeader.GetSourcePort();
	}
	const CCmiUdpHeader& GetUDPHeader() const { return m_CmiUdpHeader; }
	UInt16				GetLengthField() const {
		return m_CmiUdpHeader.GetLength();
	}

	virtual bool		IsValid() const { return m_CmiUdpHeader.IsValid(); }
};
