// ============================================================================
//	IpAddress.h
//		interface to CIpAddress
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

class CIPHeader;


// ============================================================================
//		Constants and Data Structures
// ============================================================================

enum {
	kIpVersion_None = 0,
	kIpVersion_4 = 4,
	kIpVersion_6 = 6
};

#include "wppushpack.h"

typedef UInt8	tMacAddress[6];

typedef union _tIPv4Address {
	UInt32	word;
	UInt8	bytes[4];
} tIPv4Address;

typedef union _tIPv6Address {
	UInt64	longs[2];
	UInt32	words[4];
	UInt16	shorts[8];
	UInt8	bytes[16];
} tIPv6Address;

typedef struct _tEthernetHeader {
	tMacAddress	Destination;
	tMacAddress	Source;
	UInt16		ProtocolType;
} tEthernetHeader;

typedef struct _t80211Header {
	UInt16		VersionTypeSubType;
	UInt16		Duration;
	tMacAddress	BSSID;
	tMacAddress	Source;
	tMacAddress	Destination;
	UInt16		ProtocolType;
} t80211Header;

typedef struct _tVLANHeader {
	UInt16	nVlanTag;
	UInt16	nProtocolType;
} tVLANHeader;

typedef UInt16	tTcpPort;

typedef struct _tIPv4Header {
	UInt8			VersionLength;
	UInt8			TypeOfService;
	UInt16			TotalLength;
	UInt16			Identifier;
	UInt16			FlagsFragmentOffset;
	UInt8			TimeToLive;
	UInt8			Protocol;
	UInt16			HeaderChecksum;
	tIPv4Address	Source;
	tIPv4Address	Destination;
} tIPv4Header;

typedef struct _tIPv6Header {
	UInt32			VersionClassLabel;
	UInt16			PayloadLength;
	UInt8			NextHeader;
	UInt8			HopLimit;
	tIPv6Address	Source;
	tIPv6Address	Destination;
} tIPv6Header;

typedef struct _tTcpHeader {
	UInt16	SourcePort;
	UInt16	DestinationPort;
	UInt32	SequenceNumber;
	UInt32	AckNumber;
	UInt8	Offset;
	UInt8	Flags;
	UInt16	aWindowSize;
	UInt16	aChecksum;
	UInt16	UrgentPointer;
} tTcpHeader;

typedef struct _tUdpHeader {
	UInt16	SourcePort;
	UInt16	DestinationPort;
	UInt16	Length;
	UInt16	Checksum;
} tUdpHeader;

typedef struct _tDnsHeader {
	unsigned short	id;			// query identification number //

	unsigned short	rd:		1;	// recursion desired //
	unsigned short	tc:		1;	// truncated message //
	unsigned short	aa:		1;	// authoritive answer //
	unsigned short	opcode: 4;	// purpose of message //
	unsigned short	qr:     1;	// response flag //

	unsigned short	rcode:  4;	// response code //
	unsigned short	unused: 1;	// unused bits  //
	unsigned short	ad:		1;	// authentic data from named //
	unsigned short	cd:		1;	// checking disabled by resolver //
	unsigned short	ra:		1;	// recursion available //

	unsigned short	qdcount;	// number of question entries //
	unsigned short	ancount;	// number of answer entries //
	unsigned short	nscount;	// number of authority entries //
	unsigned short	arcount;	// number of resource entries //
} tDnsHeader;

#include "wppoppack.h"

static tMacAddress	gNullMacAddress = { 0, 0, 0, 0, 0, 0 };


// ============================================================================
//		CUInt16
// ============================================================================

class CUInt16
{
protected:
	UInt16		m_Value;

public:
	;			CUInt16( const UInt16 inValue = 0 )
				:	m_Value( inValue )
				{}

	CUInt16&	operator=( const UInt16 inValue ) {
		m_Value = inValue;
		return *this;
	}
	bool		operator==( const UInt16 inValue ) const {
		return (m_Value == inValue);
	}
	;			operator UInt16() const { return m_Value; }

	bool		IsNull() const { return (m_Value == 0); }
	bool		Compare( const UInt16& inValue ) const {
		return (m_Value == inValue);
	}
	CString		Format() const;
	CStringA	FormatA() const;
};


// ============================================================================
//		CUInt32
// ============================================================================

class CUInt32
{
protected:
	UInt32		m_Value;

public:
	;			CUInt32( const UInt32 inValue = 0 )
				:	m_Value( inValue )
				{}

	CUInt32&	operator=( const UInt32 inValue ) {
		m_Value = inValue;
		return *this;
	}
	bool		operator==( const UInt32 inValue ) const {
		return (m_Value == inValue);
	}
	;			operator UInt32() const { return m_Value; }

	bool		IsNull() const { return (m_Value == 0); }
	bool		Compare( const UInt32& inValue ) const {
		return (m_Value == inValue);
	}
	CString		Format() const;
	CStringA	FormatA() const;
};


// ============================================================================
//		CIpPort
// ============================================================================

class CIpPort
	:	public CUInt16
{
public:
	;			CIpPort( UInt16 inValue = 0 )
				:	CUInt16( inValue )
				{}
};


// ============================================================================
//		CMacAddress
// ============================================================================

class CMacAddress
{
protected:
	tMacAddress		m_MacAddress;

public:
	;			CMacAddress() { Reset(); }
	;			CMacAddress( const tMacAddress& inAddress ) { Copy( inAddress ); }
	//;			CMacAddress( const TCHAR* inAddress );

	CMacAddress&	operator=( const tMacAddress& inAddress ) {
		Copy( inAddress );
		return *this;
	}
	CMacAddress&	operator=( CString inAddress );
	bool		operator==( const tMacAddress& inAddress ) const {
		return Compare( inAddress );
	}
	bool		IsNull() const { return (m_MacAddress == gNullMacAddress); }
	void		Copy( const tMacAddress& inAddress ) {
		memcpy( &m_MacAddress, &inAddress, sizeof( tMacAddress ) );
	}
	bool		Compare( const tMacAddress& inAddress ) const {
		return (memcmp( &m_MacAddress, inAddress, sizeof( tMacAddress ) ) == 0);
	}
	void		Duplicate( size_t inLength, UInt8* outAddress ) const {
		if ( inLength >= sizeof( tMacAddress ) ) {
			memcpy( outAddress, &m_MacAddress, sizeof( tMacAddress ) );
		}
	}
	CString		Format() const;
	CStringA	FormatA() const;
	void		Reset() { memset( &m_MacAddress, 0, sizeof( tMacAddress ) ); }
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
	CString			m_strFormat;
	CStringA		m_strFormatA;

	void			SetFormat();

public:
	;				CIpAddress() : m_nVersion( kIpVersion_None ) {}
	;				CIpAddress( UInt32 inIpAddress );
	;				CIpAddress( const tIPv4Address* inIpAddress );
	;				CIpAddress( const tIPv6Address* inIpAddress );
	;				~CIpAddress() {}

	bool			operator==( const CIpAddress& Other ) const { return IsEqual( Other ); }
	;				operator UInt32() const {
		if ( m_nVersion == kIpVersion_4 ) return m_IpAddress.IPv4.word;
		return 0;
	}

	bool			IsNull() const;
	bool			IsEither( const CIpAddress& inA, const CIpAddress& inB ) {
		if ( IsEqual( inA ) ) return true;
		return IsEqual( inB );
	};
	bool			IsEqual( const CIpAddress& Other ) const {
		if ( m_nVersion != Other.m_nVersion ) return false;
		if ( m_nVersion == kIpVersion_4 ) return (m_IpAddress.IPv4.word == Other.m_IpAddress.IPv4.word);
		if ( m_nVersion == kIpVersion_6 ) return
			( (m_IpAddress.IPv6.longs[0] == Other.m_IpAddress.IPv6.longs[0]) &&
			  (m_IpAddress.IPv6.longs[1] == Other.m_IpAddress.IPv6.longs[1]) );
		return false;
	}
	CString			Format() const { return m_strFormat; }
	CStringA		FormatA() const { return m_strFormatA; }
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
	;		CIpAddressPair() {}
	;		CIpAddressPair( const CIpAddress& inSource, const CIpAddress& inDestination )
		:	m_Source( inSource )
		,	m_Destination( inDestination )
		{}

	bool	operator==( const CIpAddressPair& inOther ) const {
		if ( ((inOther.m_Source == m_Source) && (inOther.m_Destination == m_Destination)) ||
			 ((inOther.m_Source == m_Destination) && (inOther.m_Destination == m_Source)) ) {
				return true;
		}
		return false;
	}

	bool		EitherIs( const CIpAddress& in ) const { return ((m_Source == in) || (m_Destination == in)); }

	CIpAddress	GetSource() const { return m_Source; }
	CIpAddress	GetDestination() const { return m_Destination; }

	bool		IsExact( const CIpAddressPair& inOther ) const {
		return ((inOther.m_Source == m_Source) && (inOther.m_Destination == m_Destination));
	}
	bool		IsExactOpposite( const CIpAddressPair& inOther ) const {
		return ((inOther.m_Source == m_Destination) && (inOther.m_Destination == m_Source));
	}
	bool		IsNull() const { return (m_Source.IsNull() && m_Destination.IsNull()); }

	void		GetSource( const CIpAddress& in ) { m_Source = in; }
	void		GetDestination( const CIpAddress& in ) { m_Destination = in; }

};


// ============================================================================
//		CIpAddressPort
// ============================================================================

class CIpAddressPort
{
protected:
	CIpAddress	m_IpAddress;
	CIpPort		m_Port;

public:
	;			CIpAddressPort() {}
	;			CIpAddressPort( CIpAddress inIpAddress, CIpPort inPort = 0 )
				:	m_IpAddress( inIpAddress )
				,	m_Port( inPort )
				{}

	inline bool	operator==( const CIpAddressPort& inOther ) const;

	CString		Format() const;
	CIpAddress	GetAddress() const { return m_IpAddress; }
	CIpPort		GetPort() const { return m_Port; }
	UInt32		Hash() const { return (m_IpAddress.Hash() + m_Port); }

	bool		IsNull() const { return (m_IpAddress.IsNull() & m_Port.IsNull()); }

	void		SetIpAddress( CIpAddress inIpAddress ) { m_IpAddress = inIpAddress; }
	void		SetPort( CIpPort inPort ) { m_Port = inPort; }
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

	CString		Format() const;

	CIpAddress	GetSrcAddress() const { return m_Source.GetAddress(); }
	CIpPort		GetSrcPort() const { return m_Source.GetPort(); }
	CIpAddress	GetDstAddress() const { return m_Destination.GetAddress(); }
	CIpPort		GetDstPort() const { return m_Destination.GetPort(); }

	UInt32		Hash() const { return m_Source.Hash() ^ m_Destination.Hash(); }
	bool		operator==( const CIpAddressPortPair& inOther ) const;
	void		Init( CIPHeader* inIpHeader, const tTcpHeader* inTcpHeader = NULL );
	void		Init( CIPHeader* inIpHeader, const tUdpHeader* inUdpHeader );
	bool		IsInverse( const CIpAddressPortPair& inOther ) const;
	void		Invert( CIpAddressPortPair& inOther ) const {
					inOther.m_Source = m_Destination; inOther.m_Destination = m_Source; }

	void		SetSrcIpAddress( CIpAddress inIpAddress ){ m_Source.SetIpAddress( inIpAddress ); }
	void		SetSrcPort( UInt16 inPort ){ m_Source.SetPort( inPort ); }
	void		SetDstIpAddress( CIpAddress inIpAddress ){ m_Destination.SetIpAddress( inIpAddress ); }
	void		SetDstPort( UInt16 inPort ){ m_Destination.SetPort( inPort ); }
};


// ============================================================================
//		CIpAddressPortPairTraits
// ============================================================================

class CIpAddressPortPairTraits
	:	public CElementTraits<CIpAddressPortPair>
{
public:
	static ULONG	Hash( const CIpAddressPortPair& inIpPortPair ) { return inIpPortPair.Hash(); }
	static bool		CompareElements( const CIpAddressPortPair& inItem1, const CIpAddressPortPair& inItem2 ) {
		return inItem1 == inItem2;
	}
};


// ============================================================================
//		CIPHeader
// ============================================================================

class CIPHeader
{
protected:
	bool		m_bValid;
	int			m_nVersion;

protected:
	void		Validated() { m_bValid = true; }

public:
	;			CIPHeader( int inVersion ) : m_bValid( false ), m_nVersion( inVersion ) {}
	virtual		~CIPHeader() {}

	virtual CIpAddress		GetDestination() const = 0;
	virtual UInt16			GetLength() const = 0;
	virtual const UInt8*	GetPayload() const = 0;
	virtual UInt16			GetPayloadLength() const = 0;
	virtual UInt8			GetPayloadProtocol() const = 0;
	virtual CIpAddress		GetSource() const = 0;
	int						GetVersion() const { return m_nVersion; }
	//virtual bool			IsFragment() const = 0;
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
	;		CIPv4Header() : CIPHeader( kIpVersion_None ) {}
	;		CIPv4Header( UInt16 inLength, const tIPv4Header* inHeader )
			:	CIPHeader( kIpVersion_4 )
			,	m_nHeaderLength( 0 )
			,	m_pIpHeader( NULL )
	{
		Init( inLength, inHeader );
	}
	virtual ~CIPv4Header() {}

	virtual CIpAddress		GetDestination() const;
	UInt8					GetFragmentFlags() const {
		return (m_pIpHeader)
			? ((NETWORKTOHOST16( m_pIpHeader->FlagsFragmentOffset ) & 0xE000) >> 13)
			: 0;
	}
	UInt16					GetFragmentOffset() const {
		return (m_pIpHeader)
			? ((NETWORKTOHOST16( m_pIpHeader->FlagsFragmentOffset ) & 0x1FFF) * 8)
			: 0;
	}
	UInt16					GetIdentifier() const {
		return (m_pIpHeader) ? NETWORKTOHOST16( m_pIpHeader->Identifier ) : 0;
	}
	virtual UInt16			GetLength() const { return m_nHeaderLength; }
	virtual const UInt8*	GetPayload() const;
	virtual UInt16			GetPayloadLength() const {
		return (m_pIpHeader) ? (NETWORKTOHOST16( m_pIpHeader->TotalLength ) - m_nHeaderLength) : 0;
	}
	virtual UInt8			GetPayloadProtocol() const {
		return (m_pIpHeader) ? m_pIpHeader->Protocol : 0;
	}
	virtual CIpAddress		GetSource() const;
	bool					Init( UInt16 inLength, const tIPv4Header* inHeader ) {
		ASSERT( inHeader );
		if ( inHeader && (inLength >= sizeof( tIPv4Header )) ) {
			UInt16	nLength((inHeader->VersionLength & 0x0F) * 4);
			ASSERT( nLength <= inLength );
			if ( nLength <= inLength ) {
				m_nHeaderLength = nLength;
				m_pIpHeader = inHeader;
				Validated();
			}
		}
		return IsValid();
	}
	bool					HasFragments() const { return (GetFragmentFlags() & 0x01); }
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
		}
	}
	virtual ~CIPv6Header() {}

	virtual CIpAddress		GetDestination() const;
	virtual UInt16			GetLength() const { return m_nHeaderLength; }
	virtual const UInt8*	GetPayload() const;
	virtual UInt16			GetPayloadLength() const {
		return (m_pIpHeader) ? NETWORKTOHOST16( m_pIpHeader->PayloadLength ) : 0;
	}
	virtual UInt8			GetPayloadProtocol() const {
		return (m_pIpHeader) ? m_pIpHeader->NextHeader : 0;
	}
	virtual CIpAddress		GetSource() const;
};


// ============================================================================
//		CUDPHeader
// ============================================================================

class CUDPHeader
{
protected:
	UInt16				m_nPayloadLength;
	const tUdpHeader*	m_pUdpHeader;
	bool				m_bFragment;
	UInt16				m_nPayloadAvailable;

public:
	;			CUDPHeader() : m_nPayloadLength( 0 ), m_pUdpHeader( NULL ), m_bFragment( false ), m_nPayloadAvailable( 0 ) {}
	;			CUDPHeader( UInt16 inLength, const tUdpHeader* inHeader )
		:	m_nPayloadLength( 0 )
		,	m_pUdpHeader( NULL )
		,	m_bFragment( false )
		,	m_nPayloadAvailable( 0 )
	{
		Init( inLength, inHeader );
	}
	virtual		~CUDPHeader() {}

	UInt16			GetDestinationPort() const {
		return (m_pUdpHeader) ? m_pUdpHeader->DestinationPort : 0;
	}
	UInt16			GetLength() const { return sizeof( tUdpHeader ); }
	UInt16			GetLengthField() const {
		return (m_pUdpHeader) ? NETWORKTOHOST16( m_pUdpHeader->Length ) : 0;
	}
	const UInt8*	GetPayload() const {
		return (m_pUdpHeader)
			? (reinterpret_cast<const UInt8*>( m_pUdpHeader ) + GetLength())
			: NULL;
	}
	UInt16			GetPayloadLength() const { return m_nPayloadLength; }
	UInt16			GetPayloadDataLength() const { return (m_bFragment) ? m_nPayloadAvailable : m_nPayloadLength; }
	UInt16			GetSourcePort() const {
		return (m_pUdpHeader) ? m_pUdpHeader->SourcePort : 0;
	}
	bool			Init( UInt16 inLength, const tUdpHeader* inHeader ) {
		ASSERT( inHeader );
		if ( inHeader && (inLength >= sizeof( tUdpHeader )) ) {
			// this is a CMI specialization.
			UInt16	nAvailLength( inLength - 4 );
			UInt16	nLength( NETWORKTOHOST16( inHeader->Length ) );
			if ( nLength > nAvailLength ) {
				m_bFragment = true;
				m_nPayloadAvailable = nAvailLength - sizeof( tUdpHeader );
			}
			m_nPayloadLength = nLength - sizeof( tUdpHeader );
			m_pUdpHeader = inHeader;
		}
		return IsValid();
	}
	bool			IsValid() const { return (m_pUdpHeader != NULL); }
};
