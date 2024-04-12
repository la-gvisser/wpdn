// ============================================================================
//	IpHeaders.cpp
//			implements IP Header classes
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "IpHeaders.h"
#include "PeekProxy.h"
#include "MediaTypes.h"

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

#define USE_MEDIA_UTILS

#ifdef USE_MEDIA_UTILS
#undef USE_MEDIA_UTILS
//class CMediaUtils;
#endif


// ============================================================================
//      CEthernetAddress
// ============================================================================

// ----------------------------------------------------------------------------
//		Format
// ----------------------------------------------------------------------------

CPeekString
CEthernetAddress::Format(
	bool	inPad ) const
{
#ifdef USE_MEDIA_UTILS
	inPad;	// not supported with Media Utils.
	TMediaSpec	ms;
	ms.fClass = kMediaSpecClass_Address;
	ms.fType = kMediaSpecType_EthernetAddr;
	ms.fMask = 0xFFFFFFFF;
	memcpy( &ms.fData, &m_EthernetAddress.bytes, sizeof( m_EthernetAddress ) );

	return CPeekProxy::GetMediaUtils().SpecToString( ms );
#else
	CPeekOutString	str;
	str << std::hex;
	if ( inPad ) {
		str << std::setfill( L'0' );
	}
	for ( int i = 0; i < 5; i++ ) {
		str << std::setw( 2 ) << m_EthernetAddress.bytes[i] << L":";
	}
	str <<  std::setw( 2 ) << m_EthernetAddress.bytes[5];

	return str;
#endif
}


// ----------------------------------------------------------------------------
//		Parse
// ----------------------------------------------------------------------------

void
CEthernetAddress::Parse(
	const CPeekString&	inAddress )
{
#ifdef USE_MEDIA_UTILS
	TMediaSpec	ms = CPeekProxy::GetMediaUtils().StringToSpec( inAddress, kMediaSpecType_EthernetAddr );
	if ( ms.fType == kMediaSpecType_EthernetAddr ) {
		memcpy( &m_EthernetAddress.bytes, &ms.fData, sizeof( m_EthernetAddress ) );
	}
#else
	CPeekString	strAddress( inAddress );
	size_t		nIndex = 0;
	int	i;
	for ( i = 0; i < sizeof( tEthernetAddress ); i++ ) {
		CPeekString	strToken = strAddress.Tokenize( L":", nIndex );
		if ( strToken.IsEmpty() ) break;
		m_EthernetAddress.bytes[i] = static_cast<UInt8>( wcstoul( strToken, NULL, 16 ) );
	}
	if ( i < sizeof( tEthernetAddress ) ) {
		memset( &m_EthernetAddress.bytes, 0, sizeof( tEthernetAddress ) );
	}
#endif
}


// ============================================================================
//      CIpAddress
// ============================================================================

CIpAddress::CIpAddress(
	UInt32 inIpAddress )
	:	m_nVersion( kIpVersion_4 )
{
	m_IpAddress.IPv4.word = inIpAddress;
}

CIpAddress::CIpAddress(
	const CIpAddress& inIpAddress )
	:	m_nVersion( kIpVersion_None )
{
	m_nVersion = inIpAddress.m_nVersion;
	m_IpAddress = inIpAddress.m_IpAddress;
}

CIpAddress::CIpAddress(
	const tIpV4Address*	inIpAddress )
	:	m_nVersion( kIpVersion_4 )
{
	m_IpAddress.IPv4.word = NETWORKTOHOST32( *reinterpret_cast<const UInt32*>( inIpAddress ) );
}

CIpAddress::CIpAddress(
	const tIpV6Address* inIpAddress )
	:	m_nVersion( kIpVersion_6 )
{
	memcpy( &m_IpAddress.IPv6.bytes, inIpAddress->bytes, sizeof( tIpV6Address ) );
}


// ----------------------------------------------------------------------------
//      operator+
// ----------------------------------------------------------------------------

CIpAddress&
CIpAddress::operator+(
	size_t inAddend )
{
	switch ( m_nVersion ) {
		case kIpVersion_4:
			m_IpAddress.IPv4.word += static_cast<UInt32>( inAddend & kMaxUInt32 );
			break;

		case kIpVersion_6:
			m_IpAddress.IPv6.longs[1] += inAddend;
			if ( m_IpAddress.IPv6.longs[1] == 0 ) {
				m_IpAddress.IPv6.longs[0]++;
			}
			break;
	}
	return *this;
}


// ----------------------------------------------------------------------------
//      operator==
// ----------------------------------------------------------------------------

bool
CIpAddress::operator==(
	const CIpAddress& inOther ) const
{
	if ( m_nVersion != inOther.m_nVersion ) return false;
	if ( m_nVersion == kIpVersion_None ) return true;
	switch ( m_nVersion ) {
		case kIpVersion_4:
			return (m_IpAddress.IPv4.word == inOther.m_IpAddress.IPv4.word);

		case kIpVersion_6:
			return ((m_IpAddress.IPv6.longs[0] == inOther.m_IpAddress.IPv6.longs[0]) &&
				(m_IpAddress.IPv6.longs[1] == inOther.m_IpAddress.IPv6.longs[1]));
	}
	return false;
}


// ----------------------------------------------------------------------------
//      Format
// ----------------------------------------------------------------------------

CPeekString
CIpAddress::Format(
	bool	inPad /*= false*/ ) const
{

	switch ( m_nVersion ) {
		case kIpVersion_4:
			{
#ifdef USE_MEDIA_UTILS
				inPad;
				TMediaSpec	ms;
				ms.fClass = kMediaSpecClass_Address;
				ms.fType = kMediaSpecType_IPAddr;
				ms.fMask = 0xFFFFFFFF;
				*reinterpret_cast<UInt32*>( &ms.fData ) = HOSTTONETWORK32( m_IpAddress.IPv4.word );

				return CPeekProxy::GetMediaUtils().SpecToString( ms );
#else
				const UInt8*	pb( m_IpAddress.IPv4.bytes );
				CPeekOutString	str;

				int	nWidth = 0;
				if ( inPad ) {
					str << std::setfill( L'0' );
					nWidth = 3;
				}

				for ( int i = 3; i >= 1; --i ) {
					str << std::setw( nWidth ) << pb[i] << L".";
				}
				str << std::setw( nWidth ) << pb[0];

				return str;
#endif
			}
			break;

		case kIpVersion_6:
			{
#ifdef USE_MEDIA_UTILS
				TMediaSpec	ms;
				ms.fClass = kMediaSpecClass_Address;
				ms.fType = kMediaSpecType_IPv6Addr;
				ms.fMask = 0xFFFFFFFF;
				memcpy( &ms.fData, &m_IpAddress.IPv6.bytes, sizeof( m_IpAddress.IPv6 ) );

				return CPeekProxy::GetMediaUtils().SpecToString( ms );
#else
				const UInt16	*ps( m_IpAddress.IPv6.shorts );
				CPeekOutString	str;
				str << std::hex;

				int	nWidth = 0;
				if ( inPad ) {
					str << std::setfill( L'0' );
					nWidth = 4;
				}

				str << std::setw( nWidth ) << NETWORKTOHOST16( ps[0] );
				size_t	i = 1;
				for ( ; ((i < 7) && (ps[i] != 0)); ++i ) {
					str << L":" << std::setw( nWidth ) << NETWORKTOHOST16( ps[i] );
				}
				if ( !inPad && (ps[i] == 0) ) {
					str << L":";
					while ( (i < 7) && (ps[i] == 0) ) ++i;
				}
				for ( ; i < 8; ++i ) {
					str << L":" << std::setw( nWidth ) << NETWORKTOHOST16( ps[i] );
				}

				return str;
#endif
			}
			break;

		default:
			break;
	}

	return L"";
}


// ----------------------------------------------------------------------------
//      FormatA
// ----------------------------------------------------------------------------

CPeekStringA
CIpAddress::FormatA(
	bool	inPad /*= false*/ ) const
{
	switch ( m_nVersion ) {
		case kIpVersion_4:
			{
#ifdef USE_MEDIA_UTILS
				TMediaSpec	ms;
				ms.fClass = kMediaSpecClass_Address;
				ms.fType = kMediaSpecType_IPAddr;
				ms.fMask = 0xFFFFFFFF;
				*reinterpret_cast<UInt32*>( &ms.fData ) = HOSTTONETWORK32( m_IpAddress.IPv4.word );
				CPeekStringA	strA;
				strA.Convert( CPeekProxy::GetMediaUtils().SpecToString( ms ) );

				return strA;
#else
				const UInt8*	pb( m_IpAddress.IPv4.bytes );
				CPeekOutStringA	strA;

				int	nWidth = 0;
				if ( inPad ) {
					strA << std::setfill( '0' );
					nWidth = 3;
				}

				for ( int i = 3; i >= 1; --i ) {
					strA << std::setw( nWidth ) << static_cast<unsigned int>( pb[i] ) << ".";
				}
				strA << std::setw( nWidth ) << static_cast<unsigned int>( pb[0] );

				return strA;
#endif
			}
			break;

		case kIpVersion_6:
			{
#ifdef USE_MEDIA_UTILS
				inPad;
				TMediaSpec	ms;
				ms.fClass = kMediaSpecClass_Address;
				ms.fType = kMediaSpecType_IPv6Addr;
				ms.fMask = 0xFFFFFFFF;
				memcpy( &ms.fData, &m_IpAddress.IPv6.bytes, sizeof( m_IpAddress.IPv6 ) );

				CPeekStringA	strA;
				strA.Convert( CPeekProxy::GetMediaUtils().SpecToString( ms ) );

				return strA;
#else
				const UInt16	*ps( m_IpAddress.IPv6.shorts );
				CPeekOutStringA	strA;
				strA << std::hex;

				int	nWidth = 0;
				if ( inPad ) {
					strA << std::setfill( '0' );
					nWidth = 4;
				}

				strA << std::setw( nWidth ) << NETWORKTOHOST16( ps[0] );
				size_t	i = 1;
				for ( ; ((i < 7) && (ps[i] != 0)); ++i ) {
					strA << ":" << std::setw( nWidth ) << NETWORKTOHOST16( ps[i] );
				}
				if ( !inPad && (ps[i] == 0) ) {
					strA << ":";
					while ( (i < 7) && (ps[i] == 0) ) ++i;
				}
				for ( ; i < 8; ++i ) {
					strA << ":" << std::setw( nWidth ) << NETWORKTOHOST16( ps[i] );
				}

				return strA;
#endif
			}
			break;

		default:
			break;
	}

	return "";
}


// ----------------------------------------------------------------------------
//      IsNull
// ----------------------------------------------------------------------------

bool
CIpAddress::IsNull() const
{
	switch ( m_nVersion ) {
		case kIpVersion_4:
			return (m_IpAddress.IPv4.word == 0);

		case kIpVersion_6:
			return ((m_IpAddress.IPv6.longs[0] == 0) && (m_IpAddress.IPv6.longs[1] == 0));

		case kIpVersion_None:
		default:
			break;
	}
	return true;
}


// ----------------------------------------------------------------------------
//      Parse
// ----------------------------------------------------------------------------

void
CIpAddress::Parse(
	const CPeekString&	inAddress )
{
	Reset();

	if ( inAddress.Find( L'.' ) >= 0 ) {
#ifdef USE_MEDIA_UTILS
		TMediaSpec	ms = CPeekProxy::GetMediaUtils().StringToSpec( inAddress, kMediaSpecType_IPAddr );
		if ( ms.fType == kMediaSpecType_IPAddr ) {
			m_nVersion = kIpVersion_4;
			m_IpAddress.IPv4.word = NETWORKTOHOST32( *reinterpret_cast<const UInt32*>( &ms.fData ) );
		}
#else
		m_nVersion = kIpVersion_4;
		m_IpAddress.IPv4.word = NETWORKTOHOST32( inet_addr( CPeekStringA( inAddress ) ) );
#endif
	}
	else if ( inAddress.Find( L':' ) >= 0 ) {
#ifdef USE_MEDIA_UTILS
		TMediaSpec	ms = CPeekProxy::GetMediaUtils().StringToSpec( inAddress, kMediaSpecType_IPv6Addr );
		if ( ms.fType == kMediaSpecType_IPv6Addr ) {
			m_nVersion = kIpVersion_6;
			memcpy( &m_IpAddress.IPv6, ms.fData, sizeof( m_IpAddress.IPv6 ) );
		}
#else
// #pragma message( "todo: implement IPv6 Parser." )
#endif
	}
}


// ----------------------------------------------------------------------------
//      Reset
// ----------------------------------------------------------------------------

void
CIpAddress::Reset() {
	m_nVersion = kIpVersion_None;
	memset( &m_IpAddress, 0, sizeof( m_IpAddress ) );
}


// ============================================================================
//      CPort
// ============================================================================

// ----------------------------------------------------------------------------
//      Format
// ----------------------------------------------------------------------------

CPeekString
CPort::Format() const
{
#ifdef USE_MEDIA_UTILS
	TMediaSpec	ms;
	ms.fClass = kMediaSpecClass_Port;
	ms.fType = kMediaSpecType_IPPort;
	ms.fMask = 0xC0000000;
	*reinterpret_cast<UInt16*>( &ms.fData ) = HOSTTONETWORK16( m_Port );

	return CPeekProxy::GetMediaUtils().SpecToString( ms );
#else
	CPeekOutString	str;
	str << m_Port;
	return str;
#endif
}


// ----------------------------------------------------------------------------
//      Parse
// ----------------------------------------------------------------------------

void
CPort::Parse(
	const CPeekString&	inPort )
{
#ifdef USE_MEDIA_UTILS
	TMediaSpec	ms = CPeekProxy::GetMediaUtils().StringToSpec( inPort, kMediaSpecType_IPPort );
	if ( ms.fType == kMediaSpecType_IPPort ) {
		m_Port = NETWORKTOHOST16( *reinterpret_cast<const UInt16*>( &ms.fData ) );
	}
#else
	m_Port = static_cast<UInt16>( _tstoi( inPort ) );
#endif
}


// ============================================================================
//      CIpAddressPort
// ============================================================================

// -----------------------------------------------------------------------------
//		Format
// -----------------------------------------------------------------------------

CPeekString
CIpAddressPort::Format() const
{
	CPeekOutString	str;
	str << m_IpAddress.Format() << L":" << m_Port;
	return str;
}


// ============================================================================
//      CIpAddressPortPair
// ============================================================================

// ----------------------------------------------------------------------------
//		Set
// ----------------------------------------------------------------------------

void
CIpAddressPortPair::Set(
	const CIpHeaderRef&	inIpHeader,
	const tTcpHeader*	inTcpHeader /* = NULL */ )
{
	_ASSERTE( inIpHeader.IsValid() );

	m_Source.SetIpAddress( inIpHeader.GetSource() );
	m_Destination.SetIpAddress( inIpHeader.GetDestination() );

	if ( inTcpHeader ) {
		m_Source.SetPort( NETWORKTOHOST16( inTcpHeader->SourcePort ) );
		m_Destination.SetPort( NETWORKTOHOST16( inTcpHeader->DestinationPort ) );
	}
}

void
CIpAddressPortPair::Set(
	const CIpHeaderRef&	inIpHeader,
	const tUdpHeader*	inUdpHeader )
{
	_ASSERTE( inIpHeader.IsValid() );
	_ASSERTE( inUdpHeader );

	m_Source.SetIpAddress( inIpHeader.GetSource() );
	m_Destination.SetIpAddress( inIpHeader.GetDestination() );

	if ( inUdpHeader ) {
		m_Source.SetPort( NETWORKTOHOST16( inUdpHeader->SourcePort ) );
		m_Destination.SetPort( NETWORKTOHOST16( inUdpHeader->DestinationPort ) );
	}
}


// ============================================================================
//		CIpV4Header
// ============================================================================

// ----------------------------------------------------------------------------
//		GetDestination
// ----------------------------------------------------------------------------

CIpAddress
CIpV4Header::GetDestination() const
{
	CIpAddress	IpAddress( NETWORKTOHOST32( m_Header.Destination.word ) );
	return IpAddress;
}


// ----------------------------------------------------------------------------
//		GetPayload
// ----------------------------------------------------------------------------

const UInt8*
CIpV4Header::GetPayload() const
{
	const UInt8*	pPayload( reinterpret_cast<const UInt8*>( &m_Header ) );
	pPayload += GetLength();
	return pPayload;
}


// ----------------------------------------------------------------------------
//		GetSource
// ----------------------------------------------------------------------------

CIpAddress
CIpV4Header::GetSource() const
{
	CIpAddress	IpAddress( NETWORKTOHOST32( m_Header.Source.word ) );
	return IpAddress;
}


// ----------------------------------------------------------------------------
//		IsFragment
// ----------------------------------------------------------------------------

bool
CIpV4Header::IsFragment() const
{
	const UInt8*	pHeaderData( reinterpret_cast<const UInt8*>( &m_Header ) );
	if ( pHeaderData == NULL ) return false;

	// Determine if this is an IP fragment
#pragma message( "Verify this." )
	UInt16	nFlagsFragmentOffset = NETWORKTOHOST16( m_Header.FlagsFragmentOffset );
	return ((nFlagsFragmentOffset & 0x3FFF) != 0 );

#if 0
	UInt8	nFragmentFlags( (nFlagsFragmentOffset & 0xE000) >> 13 );
	nFragmentFlags = (pHeaderData[6] & 0xE0) >> 5;

	UInt16	nFragmentOffset( nFlagsFragmentOffset & 0x001FFFFF );
	nFragmentOffset = (((pHeaderData[6] & 0x1F) << 8) & 0xFF00) + pHeaderData[7];

	if ( (nFragmentFlags == 0x01) || (nFragmentOffset != 0) ) {
		// The packet is an IP Fragment
		return true;
	}
	return false;
#endif
}


// ============================================================================
//		CIpV6Header
// ============================================================================

// ----------------------------------------------------------------------------
//		GetDestination
// ----------------------------------------------------------------------------

CIpAddress
CIpV6Header::GetDestination() const
{
	CIpAddress	IpAddress( &m_Header.Destination );
	return IpAddress;
}


// ----------------------------------------------------------------------------
//		GetPayload
// ----------------------------------------------------------------------------

const UInt8*
CIpV6Header::GetPayload() const
{
	const UInt8*	pPayload( reinterpret_cast<const UInt8*>( &m_Header ) );
	pPayload += GetLength();
	return pPayload;
}


// ----------------------------------------------------------------------------
//		GetSource
// ----------------------------------------------------------------------------

CIpAddress
CIpV6Header::GetSource() const
{
	CIpAddress	IpAddress( &m_Header.Source );
	return IpAddress;
}


// ============================================================================
//		CIpV4HeaderRef
// ============================================================================

// ----------------------------------------------------------------------------
//		GetDestination
// ----------------------------------------------------------------------------

CIpAddress
CIpV4HeaderRef::GetDestination() const
{
	AssertValid();

	CIpAddress	IpAddress( NETWORKTOHOST32( m_pHeader->Destination.word ) );
	return IpAddress;
}


// ----------------------------------------------------------------------------
//		GetPayload
// ----------------------------------------------------------------------------

const UInt8*
CIpV4HeaderRef::GetPayload() const
{
	const UInt8*	pPayload( reinterpret_cast<const UInt8*>( m_pHeader ) );
	if ( pPayload != NULL ) {
		pPayload += GetLength();
	}
	return pPayload;
}


// ----------------------------------------------------------------------------
//		GetSource
// ----------------------------------------------------------------------------

CIpAddress
CIpV4HeaderRef::GetSource() const
{
	AssertValid();

	CIpAddress	IpAddress( NETWORKTOHOST32( m_pHeader->Source.word ) );
	return IpAddress;
}


// ----------------------------------------------------------------------------
//		IsFragment
// ----------------------------------------------------------------------------

bool
CIpV4HeaderRef::IsFragment() const
{
	const UInt8*	pHeaderData( reinterpret_cast<const UInt8*>( m_pHeader ) );
	if ( pHeaderData == NULL ) return false;

	// Determine if this is an IP fragment
	UInt16	nFlagsFragmentOffset = NETWORKTOHOST16( m_pHeader->FlagsFragmentOffset );
	return ((nFlagsFragmentOffset & 0x3FFF) != 0 );

#if 0
	UInt8	nFragmentFlags( (nFlagsFragmentOffset & 0xE000) >> 13 );
	nFragmentFlags = (pHeaderData[6] & 0xE0) >> 5;

	UInt16	nFragmentOffset( nFlagsFragmentOffset & 0x001FFFFF );
	nFragmentOffset = (((pHeaderData[6] & 0x1F) << 8) & 0xFF00) + pHeaderData[7];

	if ( (nFragmentFlags == 0x01) || (nFragmentOffset != 0) ) {
		// The packet is an IP Fragment
		return true;
	}
	return false;
#endif
}


// ============================================================================
//		CIpV6HeaderRef
// ============================================================================

// ----------------------------------------------------------------------------
//		GetDestination
// ----------------------------------------------------------------------------

CIpAddress
CIpV6HeaderRef::GetDestination() const
{
	AssertValid();

	CIpAddress	IpAddress( &m_pHeader->Destination );
	return IpAddress;
}


// ----------------------------------------------------------------------------
//		GetPayload
// ----------------------------------------------------------------------------

const UInt8*
CIpV6HeaderRef::GetPayload() const
{
	const UInt8*	pPayload( reinterpret_cast<const UInt8*>( m_pHeader ) );
	if ( pPayload != NULL ) {
		pPayload += GetLength();
	}
	return pPayload;
}


// ----------------------------------------------------------------------------
//		GetSource
// ----------------------------------------------------------------------------

CIpAddress
CIpV6HeaderRef::GetSource() const
{
	AssertValid();

	CIpAddress	IpAddress( &m_pHeader->Source );
	return IpAddress;
}
