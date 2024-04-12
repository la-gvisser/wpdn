// =============================================================================
//	Layers.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "IpHeaders.h"
#include "ByteVectRef.h"
#include "PSIDs.h"
#include "PeekStream.h"

class CPacket;


// =============================================================================
//		CLayer
// =============================================================================

class CLayer
{
	friend class CPacket;

protected:
	CPacket*		m_pPacket;
	UInt32			m_nType;
	CByteVectRef	m_bvHeader;
	CByteVectRef	m_bvData;

	virtual void	Reset() {
		m_pPacket = NULL;
		m_nType = 0;
		m_bvHeader.Reset();
		m_bvData.Reset();
	}
	virtual void	Set( CPacket* inPacket, UInt32 inType, CByteVectRef inHeader, CByteVectRef inData ) {
		m_pPacket = inPacket;
		m_nType = inType;
		m_bvHeader = inHeader;
		m_bvData = inData;
	}

public:
	;		CLayer() : m_pPacket( NULL ), m_nType( 0 ) {}
	;		CLayer( CPacket* inPacket, UInt32 inType, CByteVectRef inHeader, CByteVectRef inData )
		:	m_pPacket( inPacket )
		,	m_nType( inType )
		,	m_bvHeader( inHeader )
		,	m_bvData( inData )
	{
	}
	virtual	~CLayer() {}

	CPeekStream		GetDataBytes() const { return CPeekStream( m_bvData ); }
	UInt16			GetDataLength() const { return static_cast<UInt16>( m_bvData.GetCount() ); }
	CByteVectRef	GetDataRef() const { return m_bvData; }
	CPeekStream		GetHeaderBytes() const { return CPeekStream( m_bvHeader ); }
	UInt16			GetHeaderLength() const { return static_cast<UInt16>( m_bvHeader.GetCount() ); }
	CByteVectRef	GetHeaderRef() const { return m_bvHeader; }
	UInt16			GetLength() const { return static_cast<UInt16>( m_bvHeader.GetCount() + m_bvData.GetCount() ); }
	UInt16			GetOffset() const;
	CPacket*		GetPacket() const { return m_pPacket; }
	UInt32			GetType() const { return m_nType; }
};


// =============================================================================
//		CLayerEthernet
// =============================================================================

class CLayerEthernet
	:	public CLayer
{
	friend class CPacket;

protected:
	CEthernetHeader	m_Header;

	virtual void	Set( CPacket* inPacket, CByteVectRef inHeader,
						CByteVectRef inData, UInt16 inProtoSpecId = ProtoSpecDefs::IEEE_802_3 );

public:
	;				CLayerEthernet() {}
	;				CLayerEthernet( CPacket* inPacket, CByteVectRef inHeader, CByteVectRef inData );
	virtual			~CLayerEthernet() {}

	CEthernetAddress		GetDestination() const { return m_Header.GetDestination(); }
	const CEthernetHeader&	GetHeader() const { return m_Header; }
	UInt16					GetProtocolType() const { return m_Header.GetProtocolType(); }
	CEthernetAddress		GetSource() const { return m_Header.GetSource(); }

	bool			IsNotValid() const { return !m_Header.IsValid(); }
	bool			IsValid() const { return m_Header.IsValid(); }
};


// =============================================================================
//		CLayerIP
// =============================================================================

class CLayerIP
	:	public CLayer
{
	friend class CPacket;

protected:
	CIpV4Header		m_IpV4Header;
	CIpV6Header		m_IpV6Header;
	CIpHeader*		m_pHeader;

	void			AssertValid() const { Peek::ThrowIf( IsNotValid() ); }
	virtual void	Reset() { m_pHeader = NULL; CLayer::Reset(); }
	virtual void	Set( CPacket* inPacket, CByteVectRef inHeader, CByteVectRef inData, UInt32 inVersion );

public:
	;				CLayerIP() : m_pHeader( NULL ) {}
	;				CLayerIP( CPacket* inPacket, CByteVectRef inHeader, CByteVectRef inData, UInt32 inVersion );
	virtual			~CLayerIP() {}

	CIpAddress		GetDestination() const { AssertValid(); return m_pHeader->GetDestination(); }
	const CIpHeader&	GetHeader() const { AssertValid(); return *m_pHeader; }
	UInt16			GetHeaderLength() const { AssertValid(); return m_pHeader->GetLength(); }
	CIpV4Header		GetIp4Header() const { return m_IpV4Header; }
	CIpV6Header		GetIp6Header() const { return m_IpV6Header; }
	UInt16			GetPayloadLength() const { AssertValid(); return m_pHeader->GetPayloadLength(); }
	UInt8			GetPayloadProtocol() const { AssertValid(); return m_pHeader->GetPayloadProtocol(); }
	CIpAddress		GetSource() const { AssertValid(); return m_pHeader->GetSource(); }
	UInt32			GetVersion() const { AssertValid(); return m_pHeader->GetVersion(); }

	bool			IsFragment() const;
	bool			IsNotValid() const { return (m_pHeader == NULL); }
	bool			IsValid() const { return (m_pHeader != NULL); }
};


// =============================================================================
//		CLayerTCP
// =============================================================================

class CLayerTCP
	:	public CLayer
{
	friend class CPacket;

protected:
	CTcpHeader		m_Header;

	virtual void	Set( CPacket* inPacket, CByteVectRef inHeader, CByteVectRef inData );

public:
	;				CLayerTCP() {}
	;				CLayerTCP( CPacket* inPacket, CByteVectRef inHeader, CByteVectRef inData );
	virtual			~CLayerTCP() {}

	UInt16				GetDestination() const { return m_Header.GetDestination(); }
	const CTcpHeader&	GetHeader() const { return m_Header; }
	UInt16				GetPayloadLength() const { return 0; }
	UInt16				GetSource() const { return m_Header.GetSource(); }

	bool			IsNotValid() const { return !m_Header.IsValid(); }
	bool			IsValid() const { return m_Header.IsValid(); }
};


// =============================================================================
//		CLayerUDP
// =============================================================================

class CLayerUDP
	:	public CLayer
{
	friend class CPacket;

protected:
	CUdpHeader		m_Header;

	virtual void	Set( CPacket* inPacket, CByteVectRef inHeader, CByteVectRef inData );

public:
	;			CLayerUDP() {}
	virtual		~CLayerUDP() {}

	UInt16				GetDestination() const { return m_Header.GetDestination(); }
	const CUdpHeader&	GetHeader() const { return m_Header; }
	UInt16				GetPayloadLength() const { return m_Header.GetLength(); }
	UInt16				GetSource() const { return m_Header.GetSource(); }

	bool			IsNotValid() const { return m_Header.IsNotValid(); }
	bool			IsValid() const { return m_Header.IsValid(); }
};
