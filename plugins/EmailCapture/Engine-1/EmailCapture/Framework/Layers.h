// =============================================================================
//	Layers.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "IpHeaders.h"
#include "ByteVectRef.h"
#include "PSIDs.h"
#include "PeekHash.h"
#include "PeekStream.h"

class CPacket;


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Layers
//
//	A Layer contains references to specific protocol layers within a Packet.
//	The layer is composed of a header and the remainder of the packet. A
//	specific layer provides the specific protocol header object. The remaining
//	bytes of the packet may contain padding. Use the specific protocol header
//	to access the payload of the protocol.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


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
	CByteVectRef	m_bvRemaining;

	virtual void	Reset() {
		m_pPacket = NULL;
		m_nType = 0;
		m_bvHeader.Reset();
		m_bvRemaining.Reset();
	}
	virtual void	Set( CPacket* inPacket, UInt32 inType, CByteVectRef inHeader, CByteVectRef inRemaining ) {
		m_pPacket = inPacket;
		m_nType = inType;
		m_bvHeader = inHeader;
		m_bvRemaining = inRemaining;
	}

public:
	;		CLayer() : m_pPacket( NULL ), m_nType( 0 ) {}
	;		CLayer( CPacket* inPacket, UInt32 inType, CByteVectRef inHeader, CByteVectRef inRemaining )
		:	m_pPacket( inPacket )
		,	m_nType( inType )
		,	m_bvHeader( inHeader )
		,	m_bvRemaining( inRemaining )
	{
	}
	virtual	~CLayer() {}

	CPeekStream		GetRemainingBytes() const { return CPeekStream( m_bvRemaining ); }
	UInt16			GetRemainingLength() const { return static_cast<UInt16>( m_bvRemaining.GetCount() ); }
	CByteVectRef	GetRemainingRef() const { return m_bvRemaining; }
	CPeekStream		GetHeaderBytes() const { return CPeekStream( m_bvHeader ); }
	UInt16			GetHeaderLength() const { return static_cast<UInt16>( m_bvHeader.GetCount() ); }
	CByteVectRef	GetHeaderRef() const { return m_bvHeader; }
	UInt16			GetLength() const { return static_cast<UInt16>( m_bvHeader.GetCount() + m_bvRemaining.GetCount() ); }
	UInt16			GetOffset() const;
	CPacket*		GetPacket() const { return m_pPacket; }
	UInt32			GetType() const { return m_nType; }
};


// =============================================================================
//		CLayerEthernet
// =============================================================================

class CLayerEthernet
	:	public CLayer
	,	public IPeekHash
{
	friend class CPacket;

protected:
	CEthernetHeaderRef	m_HeaderRef;

protected:
	virtual void	Set( CPacket* inPacket, CByteVectRef inHeader,
						CByteVectRef inData, UInt16 inProtoSpecId = ProtoSpecDefs::IEEE_802_3 );

public:
	;				CLayerEthernet() {}
	;				CLayerEthernet( CPacket* inPacket, CByteVectRef inHeader, CByteVectRef inData );
	virtual			~CLayerEthernet() {}

	CEthernetAddress			GetDestination() const { return m_HeaderRef.GetDestination(); }
	CEthernetHeader				GetHeader() const { return CEthernetHeader( m_HeaderRef ); }
	const CEthernetHeaderRef&	GetHeaderRef() const { return m_HeaderRef; }
	UInt16						GetProtocolType() const { return m_HeaderRef.GetProtocolType(); }
	CEthernetAddress			GetSource() const { return m_HeaderRef.GetSource(); }

	// IPeekHash
	size_t			Hash() const { return m_HeaderRef.Hash(); }
};


// =============================================================================
//		CLayerIP
// =============================================================================

class CLayerIP
	:	public CLayer
{
	friend class CPacket;

protected:
	CIpV4HeaderRef	m_IpV4HeaderRef;
	CIpV6HeaderRef	m_IpV6HeaderRef;
	CIpHeaderRef*	m_pHeaderRef;

	void			AssertValid() const { Peek::ThrowIf( IsNotValid() ); }
	virtual void	Reset() { m_pHeaderRef = NULL; CLayer::Reset(); }
	virtual void	Set( CPacket* inPacket, CByteVectRef inHeader, CByteVectRef inData, UInt32 inVersion );

public:
	;				CLayerIP() : m_pHeaderRef( NULL ) {}
	;				CLayerIP( CPacket* inPacket, CByteVectRef inHeader, CByteVectRef inData, UInt32 inVersion );
	virtual			~CLayerIP() {}

	CIpAddress				GetDestination() const { AssertValid(); return m_pHeaderRef->GetDestination(); }
	//CIpHeader				GetHeader() const {
	//	AssertValid();
	//	switch ( m_pHeaderRef->GetVersion() ) {
	//		case 4:
	//			return CIpHeaderRef( m_IpV4HeaderRef );
	//		case 6:
	//			return CIpHeaderRef( m_IpV4HeaderRef );
	//	}
	//	return CIpHeader( NULL );
	//}
	const CIpHeaderRef&		GetHeaderRef() const { AssertValid(); return *m_pHeaderRef; }
	UInt16					GetHeaderLength() const { AssertValid(); return m_pHeaderRef->GetLength(); }
	CIpV4Header				GetIp4Header() const { return CIpV4Header( m_IpV4HeaderRef ); }
	const CIpV4HeaderRef&	GetIp4HeaderRef() const { return m_IpV4HeaderRef; }
	CIpV6Header				GetIp6Header() const { return CIpV6Header( m_IpV6HeaderRef ); }
	const CIpV6HeaderRef&	GetIp6HeaderRef() const { return m_IpV6HeaderRef; }
	UInt16					GetPayloadLength() const { AssertValid(); return m_pHeaderRef->GetPayloadLength(); }
	UInt8					GetPayloadProtocol() const { AssertValid(); return m_pHeaderRef->GetPayloadProtocol(); }
	CIpAddress				GetSource() const { AssertValid(); return m_pHeaderRef->GetSource(); }
	UInt32					GetVersion() const { AssertValid(); return m_pHeaderRef->GetVersion(); }

	bool			IsFragment() const;
	bool			IsNotValid() const { return (m_pHeaderRef == NULL); }
	bool			IsValid() const { return (m_pHeaderRef != NULL); }

	// IPeekHash
	size_t			Hash() const { return (m_pHeaderRef != NULL) ? m_pHeaderRef->Hash() : 0; }
};


// =============================================================================
//		CLayerTCP
// =============================================================================

class CLayerTCP
	:	public CLayer
{
	friend class CPacket;

protected:
	CTcpHeaderRef	m_HeaderRef;

	virtual void	Set( CPacket* inPacket, CByteVectRef inHeader, CByteVectRef inData );

public:
	;				CLayerTCP() {}
	;				CLayerTCP( CPacket* inPacket, CByteVectRef inHeader, CByteVectRef inData );
	virtual			~CLayerTCP() {}

	UInt16					GetDestination() const { return m_HeaderRef.GetDestination(); }
	CTcpHeader				GetHeader() const { return CTcpHeader( m_HeaderRef ); }
	const CTcpHeaderRef&	GetHeaderRef() const { return m_HeaderRef; }
	UInt16					GetLength() const { return m_HeaderRef.GetLength(); }
	UInt16					GetSource() const { return m_HeaderRef.GetSource(); }

	bool			IsNotValid() const { return !m_HeaderRef.IsValid(); }
	bool			IsValid() const { return m_HeaderRef.IsValid(); }

	// IPeekHash
	size_t			Hash() const { return m_HeaderRef.Hash(); }
};


// =============================================================================
//		CLayerUDP
// =============================================================================

class CLayerUDP
	:	public CLayer
{
	friend class CPacket;

protected:
	CUdpHeaderRef	m_HeaderRef;

	virtual void	Set( CPacket* inPacket, CByteVectRef inHeader, CByteVectRef inData );

public:
	;			CLayerUDP() {}
	virtual		~CLayerUDP() {}

	UInt16					GetDestination() const { return m_HeaderRef.GetDestination(); }
	CUdpHeader				GetHeader() const { return CUdpHeader( m_HeaderRef ); }
	const CUdpHeaderRef&	GetHeaderRef() const { return m_HeaderRef; }
	UInt16					GetLength() const { return m_HeaderRef.GetLength(); }
	UInt16					GetSource() const { return m_HeaderRef.GetSource(); }

	bool			IsNotValid() const { return m_HeaderRef.IsNotValid(); }
	bool			IsValid() const { return m_HeaderRef.IsValid(); }

	// IPeekHash
	size_t			Hash() const { return m_HeaderRef.Hash(); }
};
