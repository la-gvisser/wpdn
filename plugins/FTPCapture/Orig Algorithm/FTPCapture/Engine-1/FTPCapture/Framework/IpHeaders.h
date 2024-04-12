// ============================================================================
//	IpHeaders.h
//		interface to IP Header classes
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#pragma once

#include "MemUtil.h"
#include "Ethernet.h"
#include "Peek.h"
#include "PeekArray.h"
#include "PeekStrings.h"

class CIpHeader;


// ============================================================================
//		Global Types
// ============================================================================

enum {
	kIpVersion_None = 0,
	kIpVersion_4 = 4,
	kIpVersion_6 = 6
};

typedef enum {
	kType_ICMP,
	kType_TCP,
	kType_UDP,
	kType_IP,
	kType_Max
} tProtocolType;

enum {
	kProtocol_ICMP = 1,
	kProtocol_ICMPv6 = 0x3A,
	kProtocol_TCP = 6,
	kProtocol_UDP = 17,
	kProtocol_IP = 0
};


// ============================================================================
//		TAddressPair
// ============================================================================

template <class T>
class TAddressPair
{
protected:
	T				m_Source;
	T				m_Destination;

public:
	;				TAddressPair() {}
	;				TAddressPair( const T& inSource, const T& inDestination )
		:	m_Source( inSource )
		,	m_Destination( inDestination )
	{}


	inline bool		operator==( const TAddressPair& inOther ) const {
		return ((m_Source == inOther.m_Source) && (m_Destination == inOther.m_Destination));
	}
	inline bool		operator!=( const TAddressPair& inOther ) const {
		return !operator==( inOther );
	}
	inline bool		IsInverse( const TAddressPair& inOther ) const {
		return ((m_Source == inOther.m_Destination) && (m_Destination == inOther.m_Source));
	}
	//petertest - added
	inline void		Invert( TAddressPair& inOther ) const {
		inOther.m_Source = m_Destination;
		inOther.m_Destination = m_Source;
	}
	//petertest - I added:
	void		Invert() {
		CIpAddress IpTemp( m_Source );
		m_Source = m_Destination;
		m_Destination = IpTemp;
	}

	T				GetDestination() const { return m_Destination; }
	T				GetSource() const { return m_Source; }

	//petertest - added these two:
	inline void		SetSource( const T& inSource ) { m_Source = inSource; }
	inline void		SetDestination( const T& inDestination ) { m_Destination = inDestination; }
};


// ============================================================================
//		CEthernetAddress
// ============================================================================

class CEthernetAddress
	:	public COmniModeledData
{
protected:
	tEthernetAddress		m_EthernetAddress;

public:
	;			CEthernetAddress() { memset( &m_EthernetAddress, 0, sizeof( m_EthernetAddress ) ); }
	;			CEthernetAddress( const tEthernetAddress& inAddress ) : m_EthernetAddress( inAddress ) {}
	;			CEthernetAddress( const CPeekString& inAddress ) { Parse( inAddress ); }

	CEthernetAddress&	operator=( const CPeekString& inOther ) { Parse( inOther ); return *this; }
	bool				operator==( const CEthernetAddress& inOther ) {
		return (memcmp( &m_EthernetAddress.bytes, &inOther.m_EthernetAddress.bytes, sizeof( tEthernetAddress ) ) == 0);
	}
	bool				operator!=( const CEthernetAddress& inOther ) {
		return !operator==( inOther );
	}

	CPeekString	Format( bool inPad = true ) const;

	bool		IsNull() const {
		return ((m_EthernetAddress.words.msb == 0) && (m_EthernetAddress.words.lsb == 0));
	}

	void		Parse( const CPeekString& inAddress );

	void		Set( const BYTE* inAddress ) {
		memcpy( &m_EthernetAddress.bytes, inAddress, sizeof( tEthernetAddress ) );
	}

	// COmniModeledData
	virtual CPeekString	ModeledDataGet() const { return Format(); }
	virtual void		ModeledDataSet( const CPeekString& inValue ) { Parse( inValue ); }
};


// ============================================================================
//		CMacAddressPair
// ============================================================================

typedef TAddressPair<CEthernetAddress>	CMacAddressPair;


// ============================================================================
//		CIpAddress
// ============================================================================

class CIpAddress
	:	public COmniModeledData
{
public:
	typedef union _tIPAddress {
		tIpV4Address	IPv4;
		tIpV6Address	IPv6;
	} tIpAddress;

protected:
	UInt32			m_nVersion;
	tIpAddress		m_IpAddress;

public:
	;				CIpAddress() : m_nVersion( kIpVersion_None ) {}
	;				CIpAddress( UInt32 inIpAddress );
	;				CIpAddress( const CIpAddress& inIpAddress );
	;				CIpAddress( const tIpV4Address* inIpAddress );
	;				CIpAddress( const tIpV6Address* inIpAddress );
	;				CIpAddress( const CPeekString& inAddress ) { Parse( inAddress ); }
	;				~CIpAddress() {}

	CIpAddress&		operator=( CPeekString& inOther ) { Parse( inOther ); return *this; }
	CIpAddress&		operator+( size_t inAddend );
	bool			operator==( const CIpAddress& inOther ) const;
	bool			operator!=( const CIpAddress& inOther ) const { return !operator==( inOther ); }
	CIpAddress&		operator++() { return operator+( 1 ); }
	;				operator UInt32() const { return (m_nVersion == kIpVersion_4) ? m_IpAddress.IPv4.word : 0; }

	CPeekString		Format( bool inPad = false ) const;
	CPeekStringA	FormatA( bool inPad = false ) const;
	UInt32			GetVersion() const { return m_nVersion; }
	UInt32			Hash32() const;
	bool			IsNull() const;
	void			Parse( const CPeekString& inAddress );
	void			Reset();

	// COmniModeledData
	virtual CPeekString	ModeledDataGet() const { return Format(); }
	virtual void		ModeledDataSet( const CPeekString& inValue ) { Parse( inValue ); }
};


// ============================================================================
//		CIpAddressPair
// ============================================================================

typedef TAddressPair<CIpAddress>	CIpAddressPair;

/*
// petertest - I added this:
// ============================================================================
//		CIpAddressPairTraits
// ============================================================================

class CIpAddressPairTraits
	: public CElementTraits<CIpAddressPair>
{
public:
	static bool		CompareElements( const CIpAddressPair& inItem1, const CIpAddressPair& inItem2 ) {
		return inItem1 == inItem2;
	}
	static ULONG	Hash( const CIpAddressPair& inIpAddressPair ) { 
		return ( inIpAddressPair.GetSource().Hash32() ^ inIpAddressPair.GetDestination().Hash32() );
	}
};
*/

// ============================================================================
//		CIpAddressList
// ============================================================================

class CIpAddressList
	:	public CPeekArray<CIpAddress>
{
public:
	;		CIpAddressList() {}
	;		~CIpAddressList() {}

	CIpAddressList&	operator=( const CIpAddressList& inOther ) {
		if ( this != &inOther ) {
			Copy( inOther );
		}
		return *this;
	}

	size_t	AddUnique( CIpAddress inAddress ) {
		size_t	nIndex( kIndex_Invalid );
		return (Find( inAddress, nIndex )) ? Add( inAddress ) : nIndex;
	}
	void	AddUnique( const CIpAddressList& inList ) {
		size_t	nCount = inList.GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			AddUnique( inList[i] );
		}
	}
	bool	Find( CIpAddress inAddress, size_t& outIndex ) const {
		size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			if ( inAddress == GetAt( i ) ) {
				outIndex = i;
				return true;
			}
		}
		return false;
	}
	CPeekString	Format() const {
		CPeekString	str;
		size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			if ( !str.IsEmpty() ) str.Append( L"; " );
			str.Append( GetAt( i ).Format() );
		}
		return str;
	}
};


// ============================================================================
//		CPort
// ============================================================================

class CPort
	:	public	COmniModeledData
{
protected:
	UInt16		m_Port;

public:
	;			CPort( const UInt16 inPort = 0 ) : m_Port( inPort ) {}

	CPort&		operator=( const CPort& Other ){ if ( this != &Other ) { m_Port = Other.m_Port; } return *this; }
	CPort&		operator=( const CPeekString& inOther ){ Parse( inOther ); return *this; }
	bool		operator==( const CPort& Other ) const { return (m_Port == Other.m_Port); }
	bool		operator!=( const CPort& Other ) const { return (m_Port != Other.m_Port); }
	;			operator UInt16() const { return m_Port; }

	bool		Compare( const UInt16& inOther ) const { return (m_Port == inOther); }

	CPeekString	Format() const;

	bool		IsNull() const { return (m_Port == 0); }

	void		Parse( const CPeekString& inPort );

	// COmniModeledData
	virtual CPeekString	ModeledDataGet() const { return Format(); }
	virtual void		ModeledDataSet( const CPeekString& inValue ) { Parse( inValue ); }
};


// ============================================================================
//		CPortPair
// ============================================================================

typedef TAddressPair<CPort>		CPortPair;


// ============================================================================
//		CIpAddressPort
// ============================================================================

class CIpAddressPort
{
protected:
	CIpAddress	m_IpAddress;
	CPort		m_Port;

public:
	;				CIpAddressPort() {}
	;			CIpAddressPort( CIpAddress inIpAddress, CPort inPort = 0 )
		:	m_IpAddress( inIpAddress )
		,	m_Port( inPort ) {
	}

	bool		operator==( const CIpAddressPort& inOther ) const {
		return(
			(m_IpAddress == inOther.m_IpAddress) &&
			(m_Port == inOther.m_Port) );
	}
	bool		operator!=( const CIpAddressPort& inOther ) const { return !operator==( inOther ); }

	CPeekString		Format() const;

	CIpAddress		GetAddress() const { return m_IpAddress; }
	CPort			GetPort() const { return m_Port; }

	UInt32			Hash() const { return( m_IpAddress.Hash32() + m_Port); }
	
	bool			IsNull(){ return (m_IpAddress.IsNull() & m_Port.IsNull()); }

	void			SetIpAddress( CIpAddress inIpAddress ){ m_IpAddress = inIpAddress; }
	void			SetPort( CPort inPort ){ m_Port = inPort; }
};


// ============================================================================
//		CIpAddressPortPair
// ============================================================================

class CIpAddressPortPair
	:	public TAddressPair<CIpAddressPort>
{
public:
	;			CIpAddressPortPair() {}
	//petertest - I uncommented this:
	;			CIpAddressPortPair( CIpAddressPort inSource, CIpAddressPort inDestination )
	:	TAddressPair<CIpAddressPort>( inSource, inDestination )	{}
	;			CIpAddressPortPair( const CIpHeader& inIpHeader, const tTcpHeader* inTcpHeader ) {
		Set( inIpHeader, inTcpHeader );
	}

	CIpAddress	GetSrcAddress() const { return m_Source.GetAddress(); }
	CPort		GetSrcPort() const { return m_Source.GetPort(); }
	CIpAddress	GetDstAddress() const { return m_Destination.GetAddress(); }
	CPort		GetDstPort() const { return m_Destination.GetPort(); }

	UInt32		Hash() const { return m_Source.Hash() ^ m_Destination.Hash(); }

	bool		IsInverse( const CIpAddressPortPair& inOther ) const {
		return( (m_Source == inOther.m_Destination) && (m_Destination == inOther.m_Source) );
	}
	void		Invert( CIpAddressPortPair& inOther ) const {
		inOther.m_Source = m_Destination;
		inOther.m_Destination = m_Source;
	}

	void		Set( const CIpHeader& inIpHeader, const tTcpHeader* inTcpHeader = NULL );
	void		Set( const CIpHeader& inIpHeader, const tUdpHeader* inUdpHeader );
	void		SetDstIpAddress( CIpAddress inIpAddress ){ m_Destination.SetIpAddress( inIpAddress ); }
	void		SetDstPort( UInt16 inPort ){ m_Destination.SetPort( inPort ); }
	void		SetSrcIpAddress( CIpAddress inIpAddress ){ m_Source.SetIpAddress( inIpAddress ); }
	void		SetSrcPort( UInt16 inPort ){ m_Source.SetPort( inPort ); }
};


// petertest - I uncommented this:
// ============================================================================
//		CIpAddressPortPairTraits
// ============================================================================

class CIpAddressPortPairTraits
	: public CElementTraits<CIpAddressPortPair>
{
public:
	static bool		CompareElements( const CIpAddressPortPair& inItem1, const CIpAddressPortPair& inItem2 ) {
		return inItem1 == inItem2;
	}
	static ULONG	Hash( const CIpAddressPortPair& inIpPortPair ) { return inIpPortPair.Hash(); }
};


//class CIpAddressPortPairTraits
//	: public CElementTraits<CIpAddressPortPair>
//{
//public:
//	static ULONG	Hash( const CIpAddressPortPair& inIpPortPair ) { return inIpPortPair.Hash(); }
//	static bool		CompareElements( const CIpAddressPortPair& inItem1, const CIpAddressPortPair& inItem2 ) {
//		return inItem1 == inItem2; }
//};


// ============================================================================
//		CEthernetHeader
// ============================================================================

class CEthernetHeader
{
	friend class CLayerEthernet;

protected:
	const tEthernetHeader*	m_pHeader;

	void		AssertValid() const { Peek::ThrowIf( IsNotValid() ); }
	void		Reset() { m_pHeader = NULL; }
	void		Set( const tEthernetHeader* inHeader ) { m_pHeader = inHeader; }

public:
	;			CEthernetHeader( const tEthernetHeader* inHeader = NULL ) : m_pHeader( inHeader ) {}
	;			~CEthernetHeader(){}

	CEthernetAddress	GetDestination() const { AssertValid(); return m_pHeader->Destination; }
	UInt16				GetProtocolType() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->ProtocolType ); }
	CEthernetAddress	GetSource() const { AssertValid(); return m_pHeader->Source; }

	bool		IsNotValid() const { return (m_pHeader == NULL); }
	bool		IsValid() const { return (m_pHeader != NULL); }
};


// ============================================================================
//		CIpHeader
// ============================================================================

class CIpHeader
{
protected:
	bool		m_bValid;
	int			m_nVersion;
	int			m_nProtocolType;

	void		AssertValid() const { Peek::ThrowIf( IsNotValid() ); }
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
	;			CIpHeader( int inVersion ) : m_nVersion( inVersion ), m_bValid( false ) {}
	virtual		~CIpHeader() {}

	virtual CIpAddressPair	GetAddresses() const = 0;
	virtual CIpAddress		GetDestination() const = 0;
	virtual UInt16			GetLength() const = 0;				// Header Length
	virtual const UInt8*	GetPayload() const = 0;
	virtual UInt16			GetPayloadLength() const = 0;
	virtual UInt8			GetPayloadProtocol() const = 0;
	int						GetProtocolType() const { return m_nProtocolType; }
	virtual CIpAddress		GetSource() const = 0;
	int						GetVersion() const { return m_nVersion; }

	virtual bool			IsFragment() const = 0;
	bool					IsNotValid() const { return !m_bValid; }
	bool					IsValid() const { return m_bValid; }
};


// ============================================================================
//		CIpV4Header
// ============================================================================

class CIpV4Header
	:	public CIpHeader
{
	friend class CLayerIP;

protected:
	const tIpV4Header*	m_pHeader;

	void		AssertValid() const { Peek::ThrowIf( IsNotValid() ); }
	void		Reset() { m_pHeader = NULL; }
	void		Set( const tIpV4Header* inHeader ) {
		ASSERT( inHeader );
		if ( inHeader ) {
			SetProtocolType( inHeader->Protocol );
			m_pHeader = inHeader;
			Validated();
		}
	}

public:
	;			CIpV4Header() : CIpHeader( kIpVersion_4 ), m_pHeader( NULL ) {}
	;			CIpV4Header( const tIpV4Header* inHeader )
		:	CIpHeader( kIpVersion_4 )
		,	m_pHeader( inHeader )
	{
		Set( inHeader );
	}
	virtual		~CIpV4Header() {}

	;			operator const tIpV4Header*() { return m_pHeader; }

	CIpAddressPair	GetAddresses() const {
		CIpAddressPair Addresses( GetSource(), GetDestination() );
		return Addresses;
	}
	CIpAddress		GetDestination() const;
	UInt8			GetTypeOfService() const { AssertValid(); return m_pHeader->TypeOfService; }
	UInt8			GetDiffServCodePoint() const { return (GetTypeOfService() >> 2); }
	UInt8			GetFragmentFlags() const {
		Peek::ThrowIf( IsNotValid() );
		return static_cast<UInt8>( (NETWORKTOHOST16( m_pHeader->FlagsFragmentOffset ) & 0xE000) >> 13 );
	}
	UInt16			GetFragmentOffset() const {
		Peek::ThrowIf( IsNotValid() );
		return (NETWORKTOHOST16( m_pHeader->FlagsFragmentOffset ) & 0x1FFF);
	}
	UInt16			GetFragmentOffsetBytes() const {
		Peek::ThrowIf( IsNotValid() );
		return ( NETWORKTOHOST16( m_pHeader->FlagsFragmentOffset ) & 0x1FFF) * 8;
	}
	UInt16			GetHeaderChecksum() { AssertValid(); return NETWORKTOHOST16( m_pHeader->HeaderChecksum ); }
	UInt16			GetIdentifier() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->Identifier ); }
	UInt16			GetLength() const { AssertValid(); return ((m_pHeader->VersionLength & 0x0F) * 4); }
	const UInt8*	GetPayload() const;
	UInt16			GetPayloadLength() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->TotalLength ); }
	UInt8			GetPayloadProtocol() const { AssertValid(); return m_pHeader->Protocol; }
	CIpAddress		GetSource() const;
	UInt8			GetTimeToLive() const { AssertValid(); return m_pHeader->TimeToLive; }
	UInt8			GetVersion() const { AssertValid(); return ((m_pHeader->VersionLength & 0xF0) >> 4); }

	bool			IsDoNotFragment() const { return ((GetFragmentFlags() & 0x02) != 0); }
	bool			IsFragment() const;
	bool			IsMoreFragments() const { return ((GetFragmentFlags() & 0x01) != 0); }
};


// ============================================================================
//		CIpV6Header
// ============================================================================

class CIpV6Header
	:	public CIpHeader
{
	friend class CLayerIP;

protected:
	const tIpV6Header*	m_pHeader;

	void		AssertValid() const { Peek::ThrowIf( IsNotValid() ); }
	void		Reset() { m_pHeader = NULL; }
	void		Set( const tIpV6Header* inHeader ) {
		ASSERT( inHeader );
		if ( inHeader ) {
			SetProtocolType( inHeader->NextHeader );
			m_pHeader = inHeader;
			Validated();
		}
	}

public:
	;			CIpV6Header() : CIpHeader( kIpVersion_6 ), m_pHeader( NULL ) {}
	;			CIpV6Header( const tIpV6Header* inHeader )
		:	CIpHeader( kIpVersion_6 )
		,	m_pHeader( inHeader )
	{
		ASSERT( inHeader );
		SetProtocolType( m_pHeader->NextHeader );
		Validated();
	}
	virtual		~CIpV6Header() {}

	;			operator const tIpV6Header*() { return m_pHeader; }

	CIpAddressPair	GetAddresses() const {
		CIpAddressPair Addresses( GetSource(), GetDestination() );
		return Addresses;
	}
	CIpAddress		GetDestination() const;
	UInt16			GetLength() const { return sizeof( tIpV6Header ); }
	const UInt8*	GetPayload() const;
	UInt8			GetPayloadProtocol() const { AssertValid(); return m_pHeader->NextHeader; }
	UInt16			GetPayloadLength() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->PayloadLength ); }
	CIpAddress		GetSource() const;
	UInt8			GetVersion() const {
		AssertValid();
		return static_cast<UInt8>( NETWORKTOHOST32( m_pHeader->VersionClassLabel ) >> 24 );		// VCL & 0xF0000000
	}
	UInt16			GetClass() const {
		AssertValid();
		return static_cast<UInt16>( (NETWORKTOHOST32( m_pHeader->VersionClassLabel ) & 0x0FF00000) >> 20 );
	}
	UInt32			GetFlowLabel() const {
		AssertValid();
		return (NETWORKTOHOST32( m_pHeader->VersionClassLabel ) & 0x000FFFFF);
	}
	UInt8			GetHopLimit() const { AssertValid(); return m_pHeader->HopLimit; }

	bool			IsFragment() const { return false; }
};


// ============================================================================
//		CTcpHeader
// ============================================================================

class CTcpHeader
{
	friend class CLayerTCP;

protected:
	const tTcpHeader*	m_pHeader;

	void		AssertValid() const { Peek::ThrowIf( IsNotValid() ); }
	void		Reset() { m_pHeader = NULL; }
	void		Set( const tTcpHeader* inHeader ) { m_pHeader = inHeader; }

public:
	;			CTcpHeader( const tTcpHeader* inHeader = NULL ) : m_pHeader( inHeader ) {}
	;			~CTcpHeader() {}

	;			operator const tTcpHeader*() const { return m_pHeader; }

	UInt32		GetAckNumber() const { AssertValid(); return NETWORKTOHOST32( m_pHeader->AckNumber ); }
	UInt16		GetChecksum() { AssertValid(); return NETWORKTOHOST16( m_pHeader->Checksum ); }
	CPort		GetDestination() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->DestinationPort ); }
	UInt8		GetFlags() const { AssertValid(); return m_pHeader->Flags; }
	UInt16		GetLength() const { AssertValid(); return ((m_pHeader->Offset >> 4) * 4); }
	CPortPair	GetPorts() const { CPortPair Ports( GetSource(), GetDestination() ); return Ports; }
	UInt32		GetSequenceNumber() const { AssertValid(); return NETWORKTOHOST32( m_pHeader->SequenceNumber ); }
	CPort		GetSource() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->SourcePort ); }
	UInt16		GetTCPOffset() const { AssertValid(); return ((m_pHeader->Offset >> 4) * 4); }
	UInt16		GetWindowSize() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->WindowSize ); }
	UInt16		GetUrgentPointer() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->UrgentPointer ); }

	bool		IsAck() const { return ((GetFlags() & 0x10) != 0); }
	bool		IsCongestion() const { return ((GetFlags() & 0x80) != 0); }
	bool		IsEcnEcho() const { return ((GetFlags() & 0x40) != 0); }
	bool		IsFin() const { return ((GetFlags() & 0x01) != 0); }
	bool		IsNotValid() const { return (m_pHeader == NULL); }
	bool		IsPush() const { return ((GetFlags() & 0x08) != 0); }
	bool		IsReset() const { return ((GetFlags() & 0x04) != 0); }
	bool		IsSyn() const { return ((GetFlags() & 0x02) != 0); }
	bool		IsUrgent() const { return ((GetFlags() & 0x20) != 0); }
	bool		IsValid() const { return (m_pHeader != NULL); }
};


// ============================================================================
//		CUdpHeader
// ============================================================================

class CUdpHeader
{
	friend class CLayerUDP;

protected:
	const tUdpHeader*	m_pHeader;

	void		AssertValid() const { Peek::ThrowIf( IsNotValid() ); }
	void		Reset() { m_pHeader = NULL; }
	void		Set( const tUdpHeader* inHeader ) { m_pHeader = inHeader; }

public:
	;			CUdpHeader( const tUdpHeader* inHeader = NULL ) : m_pHeader( inHeader ) {}
	;			~CUdpHeader() {}

	UInt16		GetChecksum() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->Checksum ); }
	CPort		GetDestination() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->DestinationPort ); }
	UInt16		GetLength() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->Length ); }
	CPortPair	GetPorts() const { CPortPair Ports( GetSource(), GetDestination() ); return Ports; }
	CPort		GetSource() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->SourcePort ); }

	bool		IsNotValid() const { return (m_pHeader == NULL); }
	bool		IsValid() const { return (m_pHeader != NULL); }
};


// ============================================================================
//		CDnsHeader
// ============================================================================

class CDnsHeader
{
protected:
	const tDnsHeader*	m_pHeader;

	void		AssertValid() const { Peek::ThrowIf( IsNotValid() ); }
	void		Reset() { m_pHeader = NULL; }
	void		Set( const tDnsHeader* inHeader ) { m_pHeader = inHeader; }

public:
	;			CDnsHeader( const tDnsHeader* inHeader = NULL ) : m_pHeader( inHeader ) {}
	;			~CDnsHeader() {}

	UInt16		GetAnswerCount() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->ancount ); }
	UInt16		GetAuthorityCount() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->nscount ); }
	UInt16		GetIdentifier() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->id ); }
	UInt8		GetOpCode() const { AssertValid(); return m_pHeader->opcode; }
	UInt16		GetQuestionCount() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->qdcount ); }
	UInt16		GetResourceCount() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->arcount ); }
	UInt8		GetResponseCode() const { AssertValid(); return m_pHeader->rcode; }

	bool		IsAuthenticData() const { AssertValid(); return (m_pHeader->ad != 0); }
	bool		IsAuthorativeAnswer() const { AssertValid(); return (m_pHeader->aa != 0); }
	bool		IsCheckingDisabled() const { AssertValid(); return (m_pHeader->cd != 0); }
	bool		IsNotValid() const { return (m_pHeader == NULL); }
	bool		IsRecursion() const { AssertValid(); return (m_pHeader->rd != 0); }
	bool		IsRecursionAvailable() const { AssertValid(); return (m_pHeader->ra != 0); }
	bool		IsResponse() const { AssertValid(); return (m_pHeader->qr != 0); }
	bool		IsTrucated() const { AssertValid(); return (m_pHeader->tc != 0); }
	bool		IsValid() const { return (m_pHeader != NULL); }
};
