// ============================================================================
//	IpHeaders.h
//		interface to IP Header classes
// ============================================================================
//	Copyright (c) 2009-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "MemUtil.h"
#include "PeekUnits.h"
#include "PeekStrings.h"
#include "Ethernet.h"
#include "Peek.h"
#include <algorithm>
#include <vector>

using std::vector;

class CIpHeader;
class CIpHeaderRef;
class CTcpHeader;
class CTcpHeaderRef;


// ============================================================================
//		GetTcpPayloadLength
// ============================================================================

size_t	GetTcpPayloadLength( const CIpHeader& inIpHdr, const CTcpHeader& inTcpHdr );
size_t	GetTcpPayloadLength( const CIpHeaderRef& inIpHdr, const CTcpHeaderRef& inTcpHdr );


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
	kProtocol_None = 0,
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
	:	public IPeekHash
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
	inline bool		operator<( const TAddressPair& inOther ) const {
		return (m_Source < inOther.m_Source) && (m_Destination < inOther.m_Destination);
	}

	TAddressPair	Inverse() const { return TAddressPair( m_Destination, m_Source ); }
	void			Invert() {
		T	temp = m_Destination;
		m_Destination = m_Source;
		m_Source = temp;
	}
	inline bool		IsInverse( const TAddressPair& inOther ) const {
		return ((m_Source == inOther.m_Destination) && (m_Destination == inOther.m_Source));
	}

	T				GetDestination() const { return m_Destination; }
	T				GetSource() const { return m_Source; }

	// IPeekHash
	size_t			Hash() const {
		return (m_Source.Hash() ^ m_Destination.Hash());
	}
};


// ============================================================================
//		CEthernetAddress
// ============================================================================

class CEthernetAddress
	:	public IPeekHash
	,	public COmniModeledData
{
protected:
	tEthernetAddress		m_EthernetAddress;

public:
	;			CEthernetAddress() { memset( &m_EthernetAddress, 0, sizeof( m_EthernetAddress ) ); }
	;			CEthernetAddress( const tEthernetAddress& inAddress ) : m_EthernetAddress( inAddress ) {}
	;			CEthernetAddress( const CPeekString& inAddress ) { Parse( inAddress ); }

	;					operator const tEthernetAddress&() const { return m_EthernetAddress; }
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

	void		Reset() {
		memset( &m_EthernetAddress, 0, sizeof( m_EthernetAddress ) );
	}

	void		Set( const UInt8* inAddress ) {
		memcpy( &m_EthernetAddress.bytes, inAddress, sizeof( tEthernetAddress ) );
	}

	// IPeekHash
	size_t		Hash() const { return m_EthernetAddress.Hash(); }

	// COmniModeledData
	virtual CPeekString	ModeledDataGet() const { return Format(); }
	virtual void		ModeledDataSet( const CPeekString& inValue ) { Parse( inValue ); }
};


// ============================================================================
//		CCEthernetAddressPair
// ============================================================================

typedef TAddressPair<CEthernetAddress>	CCEthernetAddressPair;


// ============================================================================
//		CIpAddress
// ============================================================================

class CIpAddress
	:	public IPeekHash
	,	public COmniModeledData
{
public:
	typedef union _tIPAddress {
		tIpV4Address	IPv4;
		tIpV6Address	IPv6;

		size_t	Hash( UInt32 inVersion ) const {
			switch (inVersion ) {
				case kIpVersion_4:
					return IPv4.Hash();
				case kIpVersion_6:
					return IPv6.Hash();
				default:
					return 0;
			}
		}
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

	;				operator UInt32() const { return (m_nVersion == kIpVersion_4) ? m_IpAddress.IPv4.word : 0; }
	CIpAddress&		operator=( CPeekString& inOther ) { Parse( inOther ); return *this; }
	CIpAddress&		operator+( size_t inAddend );
	bool			operator==( const CIpAddress& inOther ) const;
	bool			operator!=( const CIpAddress& inOther ) const { return !operator==( inOther ); }
	CIpAddress&		operator++() { return operator+( 1 ); }

	CPeekString		Format( bool inPad = false ) const;
	CPeekStringA	FormatA( bool inPad = false ) const;

	UInt32			GetVersion() const { return m_nVersion; }

	bool			IsNull() const;

	void			Parse( const CPeekString& inAddress );

	void			Reset();

	// IPeekHash
	size_t			Hash() const { return m_IpAddress.Hash( m_nVersion ); }

	// COmniModeledData
	virtual CPeekString	ModeledDataGet() const { return Format(); }
	virtual void		ModeledDataSet( const CPeekString& inValue ) { Parse( inValue ); }
};


// ============================================================================
//		CIpAddressPair
// ============================================================================

typedef TAddressPair<CIpAddress>	CIpAddressPair;


// ============================================================================
//		CIpAddressList
// ============================================================================

class CIpAddressList
	:	public vector<CIpAddress>
{
public:
	;		CIpAddressList() {}
	;		~CIpAddressList() {}

	CIpAddressList&	operator=( const CIpAddressList& inOther ) {
		if ( this != &inOther ) {
			std::copy( inOther.begin(), inOther.end(), begin() );
		}
		return *this;
	}

	size_t	Add( CIpAddress inAddress ) {
		push_back( inAddress );
		return (size() - 1);
	}
	size_t	AddUnique( CIpAddress inAddress ) {
		size_t	nIndex( kIndex_Invalid );
		return (Find( inAddress, nIndex )) ? nIndex : Add( inAddress );
	}
	void	AddUnique( const CIpAddressList& inList ) {
		size_t	nCount = inList.size();
		for ( size_t i = 0; i < nCount; i++ ) {
			AddUnique( inList[i] );
		}
	}

	bool	Find( CIpAddress inAddress, size_t& outIndex ) const {
		size_t	nCount = size();
		for ( size_t i = 0; i < nCount; i++ ) {
			if ( inAddress == at( i ) ) {
				outIndex = i;
				return true;
			}
		}
		return false;
	}
	CPeekString	Format() const {
		CPeekString	str;
		size_t	nCount = size();
		for ( size_t i = 0; i < nCount; i++ ) {
			if ( !str.IsEmpty() ) str.Append( L"; " );
			str.Append( at( i ).Format() );
		}
		return str;
	}
};


// ============================================================================
//		CPort
// ============================================================================

class CPort
	:	public IPeekHash
	,	public COmniModeledData
{
protected:
	UInt16		m_Port;

public:
	;			CPort( const UInt16 inPort = 0 ) : m_Port( inPort ) {}

	;			operator UInt16() const { return m_Port; }
	CPort&		operator=( const CPort& Other ){
		if ( this != &Other ) {
			m_Port = Other.m_Port;
		}
		return *this;
	}
	CPort&		operator=( const CPeekString& inOther ){ Parse( inOther ); return *this; }
	bool		operator==( const CPort& Other ) const { return (m_Port == Other.m_Port); }
	bool		operator!=( const CPort& Other ) const { return (m_Port != Other.m_Port); }

	bool		Compare( const UInt16& inOther ) const { return (m_Port == inOther); }

	CPeekString		Format() const;
	CPeekStringA	FormatA() const;

	bool		IsNull() const { return (m_Port == 0); }

	void		Parse( const CPeekString& inPort );

	void		Reset() { m_Port = 0; }

	// IPeekHash
	size_t		Hash() const { return m_Port; }

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
	:	public IPeekHash
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
	bool		operator!=( const CIpAddressPort& inOther ) const {
		return !operator==( inOther );
	}
	bool		operator<( const CIpAddressPort& inOther ) const {
		if ( m_IpAddress < inOther.m_IpAddress ) return true;
		if ( m_IpAddress == inOther.m_IpAddress ) {
			return ( m_Port < inOther.m_Port );
		}
		return false;
	}
	bool		operator>( const CIpAddressPort& inOther ) const {
		if ( m_IpAddress > inOther.m_IpAddress ) return true;
		if ( m_IpAddress == inOther.m_IpAddress ) {
			return ( m_Port > inOther.m_Port );
		}
		return false;
	}


	CPeekString		Format() const;

	CIpAddress		GetAddress() const { return m_IpAddress; }
	CPort			GetPort() const { return m_Port; }

	bool			IsNull(){ return (m_IpAddress.IsNull() & m_Port.IsNull()); }

	void			SetIpAddress( CIpAddress inIpAddress ){ m_IpAddress = inIpAddress; }
	void			SetPort( CPort inPort ){ m_Port = inPort; }

	// IPeekHash
	size_t			Hash() const { return (m_IpAddress.Hash() ^ m_Port.Hash()); }
};


// ============================================================================
//		CIpAddressPortPair
// ============================================================================

class CIpAddressPortPair
	:	public TAddressPair<CIpAddressPort>
{
public:
	;			CIpAddressPortPair() {}
	;			CIpAddressPortPair( CIpAddressPort inSource, CIpAddressPort inDestination )
		:	TAddressPair<CIpAddressPort>( inSource, inDestination )
	{}
	;			CIpAddressPortPair( const CIpHeaderRef& inIpHeader, const tTcpHeader* inTcpHeader ) {
		Set( inIpHeader, inTcpHeader );
	}

	CIpAddress	GetSrcAddress() const { return m_Source.GetAddress(); }
	CPort		GetSrcPort() const { return m_Source.GetPort(); }
	CIpAddress	GetDstAddress() const { return m_Destination.GetAddress(); }
	CPort		GetDstPort() const { return m_Destination.GetPort(); }

	// bool		IsInverse( const CIpAddressPortPair& inOther ) const;
	void		Invert( CIpAddressPortPair& inOther ) const {
		inOther.m_Source = m_Destination;
		inOther.m_Destination = m_Source;
	}

	void		Set( const CIpHeaderRef& inIpHeader, const tTcpHeader* inTcpHeader = nullptr );
	void		Set( const CIpHeaderRef& inIpHeader, const tUdpHeader* inUdpHeader );
	void		SetDstIpAddress( CIpAddress inIpAddress ){ m_Destination.SetIpAddress( inIpAddress ); }
	void		SetDstPort( UInt16 inPort ){ m_Destination.SetPort( inPort ); }
	void		SetSrcIpAddress( CIpAddress inIpAddress ){ m_Source.SetIpAddress( inIpAddress ); }
	void		SetSrcPort( UInt16 inPort ){ m_Source.SetPort( inPort ); }

	// IPeekHash
	size_t		Hash() const { return m_Source.Hash() ^ m_Destination.Hash(); }
};


#if _WIN32
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
	static UInt32	Hash( const CIpAddressPortPair& inIpPortPair ) {
		return static_cast<UInt32>( inIpPortPair.Hash() );
	}
};
#endif


// ============================================================================
//		CEthernetHeader
// ============================================================================

class CEthernetHeader
	:	public IPeekHash
{
	friend class CLayerEthernet;

protected:
	tEthernetHeader	m_Header;

public:
	;			CEthernetHeader( const tEthernetHeader* inHeader = nullptr )
	{
		if ( inHeader ) {
			m_Header = *inHeader;
		}
		else {
			Reset();
		}
	}
	;			~CEthernetHeader() {}

	;			operator const tEthernetHeader&() const { return m_Header; }

	CEthernetAddress	GetDestination() const { return m_Header.Destination; }
	UInt16				GetProtocolType() const { return NETWORKTOHOST16( m_Header.ProtocolType ); }
	CEthernetAddress	GetSource() const { return m_Header.Source; }

	void		Reset() { memset( &m_Header, 0, sizeof( m_Header ) ); }

	void		Set( const tEthernetHeader*	inHeader ) {
		if ( inHeader != nullptr ) {
			m_Header = *inHeader;
		}
		else {
			Reset();
		}
	}

	// IPeekHash
	size_t		Hash() const { return m_Header.Hash(); }
};


// ============================================================================
//		CEthernetHeaderRef
// ============================================================================

class CEthernetHeaderRef
	:	public IPeekHash
{
	friend class CLayerEthernet;

protected:
	const tEthernetHeader*	m_pHeader;

	void		AssertValid() const { Peek::ThrowIf( IsNotValid() ); }
	void		Set( const tEthernetHeader* inHeader ) { m_pHeader = inHeader; }

public:
	;			CEthernetHeaderRef( const tEthernetHeader* inHeader = nullptr ) : m_pHeader( inHeader ) {}
	;			~CEthernetHeaderRef() { m_pHeader = nullptr; }

	;			operator const tEthernetHeader*() const { return m_pHeader; }

	CEthernetAddress	GetDestination() const { AssertValid(); return m_pHeader->Destination; }
	UInt16				GetProtocolType() const {
		AssertValid();
		return NETWORKTOHOST16( m_pHeader->ProtocolType );
	}
	CEthernetAddress	GetSource() const { AssertValid(); return m_pHeader->Source; }

	bool		IsNotValid() const { return (m_pHeader == nullptr); }
	bool		IsValid() const { return (m_pHeader != nullptr); }

	void		Reset() { m_pHeader = nullptr; }

	// IPeekHash
	size_t		Hash() const { AssertValid(); return m_pHeader->Hash(); }
};


// ============================================================================
//		CIpHeader
// ============================================================================

class CIpHeader
	:	public IPeekHash
{
protected:
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
				break;
		}
	}

public:
	;			CIpHeader( int inVersion )
		:	m_nVersion( inVersion )
		,	m_nProtocolType( 0 )
	{}
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

	// IPeekHash
	virtual size_t			Hash() const { return ((m_nVersion << 29) ^ m_nProtocolType); }
};


// ============================================================================
//		CIpV4Header
// ============================================================================

class CIpV4Header
	:	public CIpHeader
{
	friend class CLayerIP;

protected:
	tIpV4Header	m_Header;

public:
	;			CIpV4Header() : CIpHeader( kIpVersion_4 ) { Reset(); }
	;			CIpV4Header( const tIpV4Header* inHeader )
		:	CIpHeader( kIpVersion_4 )
	{
		Set( inHeader );
	}
	virtual		~CIpV4Header() {}

	;				operator const tIpV4Header*() const { return &m_Header; }

	CIpAddressPair	GetAddresses() const {
		CIpAddressPair Addresses( GetSource(), GetDestination() );
		return Addresses;
	}
	CIpAddress		GetDestination() const;
	UInt8			GetTypeOfService() const { return m_Header.TypeOfService; }
	UInt8			GetDiffServCodePoint() const { return (GetTypeOfService() >> 2); }
	UInt8			GetFragmentFlags() const {
		return static_cast<UInt8>( (NETWORKTOHOST16( m_Header.FlagsFragmentOffset ) & 0xE000) >> 13 );
	}
	UInt16			GetFragmentOffset() const {
		return (NETWORKTOHOST16( m_Header.FlagsFragmentOffset ) & 0x1FFF);
	}
	UInt16			GetFragmentOffsetBytes() const {
		return ( NETWORKTOHOST16( m_Header.FlagsFragmentOffset ) & 0x1FFF) * 8;
	}
	UInt16			GetHeaderChecksum() { return NETWORKTOHOST16( m_Header.HeaderChecksum ); }
	UInt16			GetIdentifier() const { return NETWORKTOHOST16( m_Header.Identifier ); }
	UInt16			GetLength() const { return ((m_Header.VersionLength & 0x0F) * 4); }
	const UInt8*	GetPayload() const;
	UInt16			GetPayloadLength() const { 
		return ( NETWORKTOHOST16( m_Header.TotalLength ) - GetLength() );
	}
	UInt8			GetPayloadProtocol() const { return m_Header.Protocol; }
	CIpAddress		GetSource() const;
	UInt8			GetTimeToLive() const { return m_Header.TimeToLive; }
	UInt8			GetVersion() const { return ((m_Header.VersionLength & 0xF0) >> 4); }

	bool			IsDoNotFragment() const { return ((GetFragmentFlags() & 0x02) != 0); }
	bool			IsFragment() const;
	bool			IsMoreFragments() const { return ((GetFragmentFlags() & 0x01) != 0); }

	void			Set( const tIpV4Header* inHeader ) {
		if ( inHeader ) {
			SetProtocolType( inHeader->Protocol );
			m_Header = *inHeader;
		}
		else {
			Reset();
		}
	}
	void			Reset() { memset( &m_Header, 0, sizeof( m_Header ) ); }

	// IPeekHash
	size_t			Hash() const { return m_Header.Hash(); }
};


// ============================================================================
//		CIpV6Header
// ============================================================================

class CIpV6Header
	:	public CIpHeader
{
	friend class CLayerIP;

protected:
	tIpV6Header	m_Header;

public:
	;			CIpV6Header() : CIpHeader( kIpVersion_6 ) { Reset(); }
	;			CIpV6Header( const tIpV6Header* inHeader )
		:	CIpHeader( kIpVersion_6 )
	{
		Set( inHeader );
	}
	virtual		~CIpV6Header() {}

	;			operator const tIpV6Header&() { return m_Header; }

	CIpAddressPair	GetAddresses() const {
		CIpAddressPair Addresses( GetSource(), GetDestination() );
		return Addresses;
	}
	CIpAddress		GetDestination() const;
	UInt16			GetLength() const { return sizeof( tIpV6Header ); }
	const UInt8*	GetPayload() const;
	UInt8			GetPayloadProtocol() const { return m_Header.NextHeader; }
	UInt16			GetPayloadLength() const { return NETWORKTOHOST16( m_Header.PayloadLength ); }
	CIpAddress		GetSource() const;
	UInt8			GetVersion() const {
		return static_cast<UInt8>( NETWORKTOHOST32( m_Header.VersionClassLabel ) >> 24 );		// VCL & 0xF0000000
	}
	UInt16			GetClass() const {
		return static_cast<UInt16>( (NETWORKTOHOST32( m_Header.VersionClassLabel ) & 0x0FF00000) >> 20 );
	}
	UInt32			GetFlowLabel() const {
		return (NETWORKTOHOST32( m_Header.VersionClassLabel ) & 0x000FFFFF);
	}
	UInt8			GetHopLimit() const { return m_Header.HopLimit; }

	bool		IsFragment() const { return false; }

	void		Reset() { memset( &m_Header, 0, sizeof( m_Header ) ); }

	void		Set( const tIpV6Header* inHeader ) {
		if ( inHeader != nullptr ) {
			SetProtocolType( inHeader->NextHeader );
			m_Header = *inHeader;
		}
		else {
			Reset();
		}
	}
	// IPeekHash
	size_t		Hash() const { return m_Header.Hash(); }
};


// ============================================================================
//		CIpHeaderRef
// ============================================================================

class CIpHeaderRef
	:	public IPeekHash
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
				break;
		}
	}

protected:
	void		Validated() { m_bValid = true; }

public:
	;			CIpHeaderRef( int inVersion )
					: m_bValid( false )
					, m_nVersion( inVersion )
					, m_nProtocolType( kProtocol_None ) {}
	virtual		~CIpHeaderRef() {}

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

	// IPeekHash
	virtual size_t			Hash() const { return ((m_nVersion << 29) ^ m_nProtocolType); }
};


// ============================================================================
//		CIpV4HeaderRef
// ============================================================================

class CIpV4HeaderRef
	:	public CIpHeaderRef
{
	friend class CLayerIP;

protected:
	const tIpV4Header*	m_pHeader;

	void		AssertValid() const { Peek::ThrowIf( IsNotValid() ); }
	void		Reset() { m_pHeader = nullptr; }
	void		Set( const tIpV4Header* inHeader ) {
		_ASSERTE( inHeader );
		if ( inHeader ) {
			SetProtocolType( inHeader->Protocol );
			m_pHeader = inHeader;
			Validated();
		}
	}

public:
	;			CIpV4HeaderRef() : CIpHeaderRef( kIpVersion_4 ), m_pHeader( nullptr ) {}
	;			CIpV4HeaderRef( const tIpV4Header* inHeader )
		:	CIpHeaderRef( kIpVersion_4 )
		,	m_pHeader( inHeader )
	{
		Set( inHeader );
	}
	virtual		~CIpV4HeaderRef() {}

	;			operator const tIpV4Header*() const { return m_pHeader; }

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
	UInt16			GetPayloadLength() const { AssertValid(); 
		return ( NETWORKTOHOST16( m_pHeader->TotalLength ) - GetLength() ); }
	UInt8			GetPayloadProtocol() const { AssertValid(); return m_pHeader->Protocol; }
	CIpAddress		GetSource() const;
	UInt8			GetTimeToLive() const { AssertValid(); return m_pHeader->TimeToLive; }
	UInt8			GetVersion() const { AssertValid(); return ((m_pHeader->VersionLength & 0xF0) >> 4); }

	bool			IsDoNotFragment() const { return ((GetFragmentFlags() & 0x02) != 0); }
	bool			IsFragment() const;
	bool			IsMoreFragments() const { return ((GetFragmentFlags() & 0x01) != 0); }

	// IPeekHash
	virtual size_t	Hash() const { return (m_pHeader != nullptr) ? m_pHeader->Hash() : 0; }
};


// ============================================================================
//		CIpV6HeaderRef
// ============================================================================

class CIpV6HeaderRef
	:	public CIpHeaderRef
{
	friend class CLayerIP;

protected:
	const tIpV6Header*	m_pHeader;

	void		AssertValid() const { Peek::ThrowIf( IsNotValid() ); }
	void		Reset() { m_pHeader = nullptr; }
	void		Set( const tIpV6Header* inHeader ) {
		_ASSERTE( inHeader );
		if ( inHeader ) {
			SetProtocolType( inHeader->NextHeader );
			m_pHeader = inHeader;
			Validated();
		}
	}

public:
	;			CIpV6HeaderRef() : CIpHeaderRef( kIpVersion_6 ), m_pHeader( nullptr ) {}
	;			CIpV6HeaderRef( const tIpV6Header* inHeader )
		:	CIpHeaderRef( kIpVersion_6 )
		,	m_pHeader( inHeader )
	{
		_ASSERTE( inHeader );
		SetProtocolType( m_pHeader->NextHeader );
		Validated();
	}
	virtual		~CIpV6HeaderRef() {}

	;			operator const tIpV6Header*() const { return m_pHeader; }

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

	// IPeekHash
	virtual size_t	Hash() const { return (m_pHeader != nullptr) ? m_pHeader->Hash() : 0; }
};


// ============================================================================
//		CTcpHeader
// ============================================================================

class CTcpHeader
	:	public IPeekHash
{
	friend class CLayerTCP;

protected:
	tTcpHeader	m_Header;

public:
	;			CTcpHeader( const tTcpHeader* inHeader = nullptr )
	{
		Set( inHeader );
	}
	;			~CTcpHeader() {}

	;			operator const tTcpHeader&() const { return m_Header; }

	UInt32		GetAckNumber() const { return NETWORKTOHOST32( m_Header.AckNumber ); }
	UInt16		GetChecksum() { return NETWORKTOHOST16( m_Header.Checksum ); }
	CPort		GetDestination() const { return NETWORKTOHOST16( m_Header.DestinationPort ); }
	UInt8		GetFlags() const { return m_Header.Flags; }
	UInt16		GetLength() const { return ((m_Header.Offset >> 4) * 4); }
	CPortPair	GetPorts() const { CPortPair Ports( GetSource(), GetDestination() ); return Ports; }
	UInt32		GetSequenceNumber() const { return NETWORKTOHOST32( m_Header.SequenceNumber ); }
	CPort		GetSource() const { return NETWORKTOHOST16( m_Header.SourcePort ); }
	UInt16		GetTCPOffset() const { return ((m_Header.Offset >> 4) * 4); }
	UInt16		GetWindowSize() const { return NETWORKTOHOST16( m_Header.WindowSize ); }
	UInt16		GetUrgentPointer() const { return NETWORKTOHOST16( m_Header.UrgentPointer ); }

	bool		IsAck() const { return ((GetFlags() & 0x10) != 0); }
	bool		IsCongestion() const { return ((GetFlags() & 0x80) != 0); }
	bool		IsEcnEcho() const { return ((GetFlags() & 0x40) != 0); }
	bool		IsFin() const { return ((GetFlags() & 0x01) != 0); }
	bool		IsPush() const { return ((GetFlags() & 0x08) != 0); }
	bool		IsReset() const { return ((GetFlags() & 0x04) != 0); }
	bool		IsSyn() const { return ((GetFlags() & 0x02) != 0); }
	bool		IsUrgent() const { return ((GetFlags() & 0x20) != 0); }

	void		Reset() { memset( &m_Header, 0, sizeof( m_Header ) ); }

	void		Set( const tTcpHeader* inHeader ) {
		if ( inHeader != nullptr ) {
			m_Header = *inHeader;
		}
		else {
			Reset();
		}
	}

	// IPeekHash
	size_t		Hash() const { return m_Header.Hash(); }
};


// ============================================================================
//		CTcpHeaderRef
// ============================================================================

class CTcpHeaderRef
	:	public IPeekHash
{
	friend class CLayerTCP;

protected:
	const tTcpHeader*	m_pHeader;

	void		AssertValid() const { Peek::ThrowIf( IsNotValid() ); }
	void		Reset() { m_pHeader = nullptr; }
	void		Set( const tTcpHeader* inHeader ) { m_pHeader = inHeader; }

public:
	;			CTcpHeaderRef( const tTcpHeader* inHeader = nullptr ) : m_pHeader( inHeader ) {}
	;			~CTcpHeaderRef() {}

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
	bool		IsNotValid() const { return (m_pHeader == nullptr); }
	bool		IsPush() const { return ((GetFlags() & 0x08) != 0); }
	bool		IsReset() const { return ((GetFlags() & 0x04) != 0); }
	bool		IsSyn() const { return ((GetFlags() & 0x02) != 0); }
	bool		IsUrgent() const { return ((GetFlags() & 0x20) != 0); }
	bool		IsValid() const { return (m_pHeader != nullptr); }

	// IPeekHash
	size_t		Hash() const { return (m_pHeader != nullptr) ? m_pHeader->Hash() : 0; }
};


// ============================================================================
//		CUdpHeader
// ============================================================================

class CUdpHeader
	:	public IPeekHash
{
	friend class CLayerUDP;

protected:
	tUdpHeader	m_Header;

public:
	;			CUdpHeader( const tUdpHeader* inHeader = nullptr )
	{
		Set( inHeader );
	}
	;			~CUdpHeader() {}

	;			operator const tUdpHeader&() const { return m_Header; }

	UInt16		GetChecksum() const { return NETWORKTOHOST16( m_Header.Checksum ); }
	CPort		GetDestination() const { return NETWORKTOHOST16( m_Header.DestinationPort ); }
	UInt16		GetLength() const { return NETWORKTOHOST16( m_Header.Length ); }
	CPortPair	GetPorts() const { CPortPair Ports( GetSource(), GetDestination() ); return Ports; }
	CPort		GetSource() const { return NETWORKTOHOST16( m_Header.SourcePort ); }

	void		Reset() { memset( &m_Header, 0, sizeof( m_Header ) ); }

	void		Set( const tUdpHeader* inHeader ) {
		if ( inHeader != nullptr ) {
			m_Header = *inHeader;
		}
		else {
			Reset();
		}
	}

	// IPeekHash
	size_t		Hash() const { return m_Header.Hash(); }
};


// ============================================================================
//		CUdpHeaderRef
// ============================================================================

class CUdpHeaderRef
	:	public IPeekHash
{
	friend class CLayerUDP;

protected:
	const tUdpHeader*	m_pHeader;

	void		AssertValid() const { Peek::ThrowIf( IsNotValid() ); }
	void		Reset() { m_pHeader = nullptr; }
	void		Set( const tUdpHeader* inHeader ) { m_pHeader = inHeader; }

public:
	;			CUdpHeaderRef( const tUdpHeader* inHeader = nullptr ) : m_pHeader( inHeader ) {}
	;			~CUdpHeaderRef() {}

	;			operator const tUdpHeader*() const { return m_pHeader; }

	UInt16		GetChecksum() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->Checksum ); }
	CPort		GetDestinationPort() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->DestinationPort ); }
	UInt16		GetLength() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->Length ); }
	const UInt8* GetPayload() const { return (m_pHeader) ?
		(reinterpret_cast<const UInt8*>( m_pHeader ) + GetUdpHdrLength()) : nullptr; }
	CPortPair	GetPorts() const { CPortPair Ports( GetSourcePort(), GetDestinationPort() ); return Ports; }
	CPort		GetSourcePort() const { AssertValid(); return NETWORKTOHOST16( m_pHeader->SourcePort ); }
	UInt16		GetUdpHdrLength() const { return sizeof( tUdpHeader ); }

	bool		IsNotValid() const { return (m_pHeader == nullptr); }
	bool		IsValid() const { return (m_pHeader != nullptr); }

	// IPeekHash
	size_t		Hash() const { return (m_pHeader != nullptr) ? m_pHeader->Hash() : 0; }
};


// ============================================================================
//		CDnsHeader
// ============================================================================

class CDnsHeader
	:	public IPeekHash
{
protected:
	tDnsHeader	m_Header;

public:
	;			CDnsHeader( const tDnsHeader* inHeader = nullptr )
	{
		Set( inHeader );
	}
	;			~CDnsHeader() {}

	;			operator const tDnsHeader&() const { return m_Header; }

	UInt16		GetAnswerCount() const { return NETWORKTOHOST16( m_Header.ancount ); }
	UInt16		GetAuthorityCount() const { return NETWORKTOHOST16( m_Header.nscount ); }
	UInt16		GetIdentifier() const { return NETWORKTOHOST16( m_Header.id ); }
	UInt8		GetOpCode() const { return m_Header.opcode; }
	UInt16		GetQuestionCount() const { return NETWORKTOHOST16( m_Header.qdcount ); }
	UInt16		GetResourceCount() const { return NETWORKTOHOST16( m_Header.arcount ); }
	UInt8		GetResponseCode() const { return m_Header.rcode; }

	bool		IsAuthenticData() const { return (m_Header.ad != 0); }
	bool		IsAuthorativeAnswer() const { return (m_Header.aa != 0); }
	bool		IsCheckingDisabled() const { return (m_Header.cd != 0); }
	bool		IsRecursion() const { return (m_Header.rd != 0); }
	bool		IsRecursionAvailable() const { return (m_Header.ra != 0); }
	bool		IsResponse() const { return (m_Header.qr != 0); }
	bool		IsTrucated() const { return (m_Header.tc != 0); }

	void		Reset() { memset( &m_Header, 0, sizeof( m_Header ) ); }

	void		Set( const tDnsHeader* inHeader ) {
		if ( inHeader != nullptr ) {
			m_Header = *inHeader;
		}
		else {
			Reset();
		}
	}

	// IPeekHash
	size_t		Hash() const { return m_Header.Hash(); }
};


// ============================================================================
//		CDnsHeaderRef
// ============================================================================

class CDnsHeaderRef
	:	public IPeekHash
{
protected:
	const tDnsHeader*	m_pHeader;

	void		AssertValid() const { Peek::ThrowIf( IsNotValid() ); }
	void		Reset() { m_pHeader = nullptr; }
	void		Set( const tDnsHeader* inHeader ) { m_pHeader = inHeader; }

public:
	;			CDnsHeaderRef( const tDnsHeader* inHeader = nullptr ) : m_pHeader( inHeader ) {}
	;			~CDnsHeaderRef() {}

	;			operator const tDnsHeader*() const { return m_pHeader; }

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
	bool		IsNotValid() const { return (m_pHeader == nullptr); }
	bool		IsRecursion() const { AssertValid(); return (m_pHeader->rd != 0); }
	bool		IsRecursionAvailable() const { AssertValid(); return (m_pHeader->ra != 0); }
	bool		IsResponse() const { AssertValid(); return (m_pHeader->qr != 0); }
	bool		IsTrucated() const { AssertValid(); return (m_pHeader->tc != 0); }
	bool		IsValid() const { return (m_pHeader != nullptr); }

	// IPeekHash
	size_t		Hash() const { return (m_pHeader != nullptr) ? m_pHeader->Hash() : 0; }
};
