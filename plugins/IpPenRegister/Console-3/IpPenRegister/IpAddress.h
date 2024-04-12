// ============================================================================
//	IpAddress.h
//  interface to CIpAddress
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "Utils.h"

class CIPHeader;
class CPeekContext;


// ============================================================================
//		Constants and Data Structures
// ============================================================================

enum {
	kIpVersion_None = 0,
	kIpVersion_4 = 4,
	kIpVersion_6 = 6
};


// ============================================================================
//		CMacAddress
// ============================================================================

class CMacAddress
{
protected:
	tMacAddress		m_MacAddress;

public:
	;			CMacAddress() { memset( &m_MacAddress, 0, sizeof( m_MacAddress ) ); } 

	void		SetMacAddress( const BYTE* pMacAddress ) {
					memcpy( m_MacAddress, pMacAddress, sizeof( tMacAddress ) );
	}
	CStringA	Format( bool inPad = true ) const;
};


// ============================================================================
//		CIpPort
// ============================================================================

class CIpPort
{
protected:
	UInt16			m_IpPort;

public:
	;				CIpPort( const UInt16 nIpPort = 0 );
	;				~CIpPort();

	CIpPort&		operator=( const CIpPort& Other ){ m_IpPort = Other.m_IpPort; return *this; }
	bool			operator==( const CIpPort& Other ) const { return (m_IpPort == Other.m_IpPort); }
	;				operator UInt16() const { return m_IpPort; }

	bool			IsNull() const { return (m_IpPort == 0); }
	bool			Compare( const UInt16& Other ) const { return (m_IpPort == Other); }
	CStringA		Format() const;
	CStringA		GetName( CPeekContext* inContext ) const;
};


// ============================================================================
//		CIpAddress
// ============================================================================

class CIpAddress
{
public:
	typedef union _tIPAddress {
		tIPv4Address	IPv4;
		tIPv6Address	IPv6;
	} tIPAddress;

protected:
	UInt32			m_nVersion;
	tIPAddress		m_IpAddress;
	CStringA		m_strFormat;
	CStringA		m_strFormatPad;

	void			SetFormat();

public:
	;				CIpAddress() : m_nVersion( kIpVersion_None ) {}
	;				CIpAddress( UInt32 inIpAddress );
	;				CIpAddress( const tIPv4Address* inIpAddress );
	;				CIpAddress( const tIPv6Address* inIpAddress );
	;				~CIpAddress() {}

	//CIpAddress&	operator=( const tIpAddress& Other ){ m_IpAddress = Other.word; return *this; }
	//CIpAddress&	operator=( const CIPAddress& Other ){ m_IpAddress = Other.m_IpAddress; return *this; }
	bool			operator==( const CIpAddress& Other ) const;
	//;				operator UInt32() const { return m_IpAddress; }

	bool			IsNull() const;
	//bool			Compare( const tIpAddress& Other ) const { return (m_IpAddress == Other.word); }
	CStringA		Format( bool inPad = true ) const { return ((inPad) ? m_strFormatPad : m_strFormat); }
	CStringA		GetName( CPeekContext* inContext ) const;
	UInt32			Hash() const;
};


// ============================================================================
//		CIpAddressPair
// ============================================================================

class CIpAddressPair
{
protected:
	CIpAddress		m_Source;
	CIpAddress		m_Destination;

public:
	;				CIpAddressPair(){}
	//;				CIpAddressPair( const tIpHeader* inIpHeader );

	inline bool		operator==( const CIpAddressPair& inOther ) const;
	inline bool		IsInverse( const CIpAddressPair& inOther ) const;
};


// ============================================================================
//		CIpAddressPort
// ============================================================================

class CIpAddressPort
{
protected:
	CIpAddress		m_IpAddress;
	CIpPort			m_IpPort;

public:
	;				CIpAddressPort() {}
	;				CIpAddressPort( CIpAddress inIpAddress, CIpPort inPort = 0 );

	inline bool		operator==( const CIpAddressPort& inOther ) const;

	UInt32			Hash() const { return( m_IpAddress.Hash() + m_IpPort); }
	CIpAddress		GetAddress() const { return m_IpAddress; }
	CIpPort			GetPort() const { return m_IpPort; }

	void			SetIpAddress( CIpAddress inIpAddress ){ m_IpAddress = inIpAddress; }
	void			SetPort( CIpPort inPort ){ m_IpPort = inPort; }
	bool			IsNull(){ return (m_IpAddress.IsNull() & m_IpPort.IsNull()); }
};


// ============================================================================
//		CIpAddressPortPair
// ============================================================================

class CIpAddressPortPair
{
public:
	typedef union _tIPHeaderPtr {
		const tIPv4Header*	pIPv4;
		const tIPv6Header*	pIPv6;
	} tIPHeaderPtr;

protected:
	CIpAddressPort		m_Source;
	CIpAddressPort		m_Destination;

public:
	;			CIpAddressPortPair() {};
	;			CIpAddressPortPair( CIpAddressPort inSource, CIpAddressPort inDestination );

	UInt32		Hash() const { return m_Source.Hash() ^ m_Destination.Hash(); }
	bool		operator==( const CIpAddressPortPair& inOther ) const;
	void		Init( CIPHeader* inIpHeader, const tTcpHeader* inTcpHeader = NULL );
	void		Init( CIPHeader* inIpHeader, const tUdpHeader* inUdpHeader );
	bool		IsInverse( const CIpAddressPortPair& inOther ) const;
	void		Invert( CIpAddressPortPair& inOther ) const {
					inOther.m_Source = m_Destination; inOther.m_Destination = m_Source; }

	CIpAddress	GetSrcAddress() const { return m_Source.GetAddress(); }
	CIpPort		GetSrcPort() const { return m_Source.GetPort(); }
	CIpAddress	GetDstAddress() const { return m_Destination.GetAddress(); }
	CIpPort		GetDstPort() const { return m_Destination.GetPort(); }

	void		SetSrcIpAddress( CIpAddress inIpAddress ){ m_Source.SetIpAddress( inIpAddress ); }
	void		SetSrcPort( UInt16 inPort ){ m_Source.SetPort( inPort ); }
	void		SetDstIpAddress( CIpAddress inIpAddress ){ m_Destination.SetIpAddress( inIpAddress ); }
	void		SetDstPort( UInt16 inPort ){ m_Destination.SetPort( inPort ); }
};


// ============================================================================
//		CIpAddressPortPairTraits
// ============================================================================

class CIpAddressPortPairTraits
	: public CElementTraits<CIpAddressPortPair>
{
public:
	static ULONG	Hash( const CIpAddressPortPair& inIpPortPair ) { return inIpPortPair.Hash(); }
	static bool		CompareElements( const CIpAddressPortPair& inItem1, const CIpAddressPortPair& inItem2 ) {
						return inItem1 == inItem2; }
};


// ============================================================================
//		CIPHeader
// ============================================================================

class CIPHeader
{
protected:
	bool		m_bValid;
	int			m_nVersion;
	int			m_nProtocolType;

	void		SetProtocolType( int inProtocol ) {
		switch ( inProtocol ) {
		case kProtocol_ICMP:
			m_nProtocolType = kType_ICMP;
			break;
		case kProtocol_TCP:
			m_nProtocolType = kType_TCP;
			break;
		case kProtocol_UDP:
			m_nProtocolType = kType_UDP;
			break;
		case kProtocol_IP:
		default:
			m_nProtocolType = kType_IP;
		}
	}

protected:
	void		Validated() { m_bValid = true; }

public:
	;			CIPHeader( int inVersion ) : m_nVersion( inVersion ), m_bValid( false ) {}
	virtual		~CIPHeader() {}

	virtual CIpAddress		GetDestination() const = 0;
	virtual UInt16			GetLength() const = 0;
	virtual const UInt8*	GetPayload() const = 0;
	virtual UInt16			GetPayloadLength() const = 0;
	virtual UInt8			GetPayloadProtocol() const = 0;
	int						GetProtocolType() const { return m_nProtocolType; }
	virtual CIpAddress		GetSource() const = 0;
	int						GetVersion() const { return m_nVersion; }
	virtual bool			IsFragment() const = 0;
	bool					IsValid() const { return m_bValid; }
};


// ============================================================================
//		CIPv4Header
// ============================================================================

class CIPv4Header
	:	public CIPHeader
{
	UInt16				m_nHeaderLength;
	const tIPv4Header*	m_pIpHeader;

public:
	;		CIPv4Header( UInt16 inLength, const tIPv4Header* inHeader )
			:	CIPHeader( kIpVersion_4 )
			,	m_pIpHeader( inHeader )
	{
		ASSERT( inHeader );
		if ( inHeader ) {
			m_nHeaderLength = (inHeader->VersionLength & 0x0F) * 4;
			ASSERT( m_nHeaderLength <= inLength );
			if ( m_nHeaderLength <= inLength ) {
				Validated();
				SetProtocolType( inHeader->Protocol );
			}
		}
	}
	virtual ~CIPv4Header() {}

	virtual CIpAddress		GetDestination() const;
	virtual UInt16			GetLength() const { return m_nHeaderLength; }
	virtual const UInt8*	GetPayload() const;
	virtual UInt16			GetPayloadLength() const { return NETWORKTOHOST16( m_pIpHeader->TotalLength ); }
	virtual UInt8			GetPayloadProtocol() const { return m_pIpHeader->Protocol; }
	virtual CIpAddress		GetSource() const;
	bool					IsFragment() const;
};


// ============================================================================
//		CIPv6Header
// ============================================================================

class CIPv6Header
	:	public CIPHeader
{
	UInt16				m_nHeaderLength;
	const tIPv6Header*	m_pIpHeader;

public:
	;		CIPv6Header( UInt16 inLength, const tIPv6Header* inHeader )
			:	CIPHeader( kIpVersion_6 )
			,	m_nHeaderLength( sizeof( tIPv6Header ) )
			,	m_pIpHeader( inHeader )
	{
		ASSERT( inHeader );
		ASSERT( m_nHeaderLength <= inLength );
		if ( inHeader && (m_nHeaderLength <= inLength) ) {
			Validated();
			SetProtocolType( m_pIpHeader->NextHeader );
		}
	}
	virtual ~CIPv6Header() {}

	virtual CIpAddress		GetDestination() const;
	virtual UInt16			GetLength() const { return m_nHeaderLength; }
	virtual const UInt8*	GetPayload() const;
	virtual UInt16			GetPayloadLength() const { return NETWORKTOHOST16( m_pIpHeader->PayloadLength ); }
	virtual UInt8			GetPayloadProtocol() const { return m_pIpHeader->NextHeader; }
	virtual CIpAddress		GetSource() const;
	bool					IsFragment() const { return false; }
};
