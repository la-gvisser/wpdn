// ============================================================================
//	IpAddress.cpp
//  implements CIpAddress class
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "IpAddress.h"
#include "PeekContext.h"

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif


// ============================================================================
//      CMacAddress
// ============================================================================

// ----------------------------------------------------------------------------
//		Format
// ----------------------------------------------------------------------------

CStringA
CMacAddress::Format(
	bool	inPad ) const
{
	CStringA	str;
	if ( inPad ) {
		str.Format(
			"%02X:%02X:%02X:%02X:%02X:%02X",
			m_MacAddress[0], m_MacAddress[1], m_MacAddress[2],
			m_MacAddress[3], m_MacAddress[4], m_MacAddress[5] );
	}
	else {
		str.Format(
			"%X:%X:%X:%X:%X:%X",
			m_MacAddress[0], m_MacAddress[1], m_MacAddress[2],
			m_MacAddress[3], m_MacAddress[4], m_MacAddress[5] );
	}
	return str;
}


// ============================================================================
//      CIpPort
// ============================================================================

CIpPort::CIpPort(
	const UInt16 nIpPort )
	:	m_IpPort( nIpPort )
{
}

CIpPort::~CIpPort()
{
}


// ----------------------------------------------------------------------------
//      Format
// ----------------------------------------------------------------------------

CStringA
CIpPort::Format() const
{
	CStringA	str;
	str.Format( "%u", static_cast<UInt32>( m_IpPort ) );
	return str;
}


// ----------------------------------------------------------------------------
//      GetName
// ----------------------------------------------------------------------------

CStringA
CIpPort::GetName(
	CPeekContext*	inContext ) const
{
	PluginNameTableEntry	nteIpPort;
	TCHAR					szName[256];
	UInt16					nPort( NETWORKTOHOST16( m_IpPort ) );

	memset( &nteIpPort, 0, sizeof( PluginNameTableEntry ) );
	nteIpPort.fEntryType = kEntryType_IPPort;
	nteIpPort.fEntry = reinterpret_cast<UInt8*>( &nPort );
	nteIpPort.fName = szName;
	
	inContext->DoLookupName( &nteIpPort );

	CStringA	str( szName );
	return str;
}


// ============================================================================
//      CIpAddress
// ============================================================================

CIpAddress::CIpAddress(
	UInt32 inIpAddress )
	:	m_nVersion( kIpVersion_4 )
{
	m_IpAddress.IPv4.word = inIpAddress;
	SetFormat();
}

CIpAddress::CIpAddress(
	const tIPv4Address* inIpAddress )
	:	m_nVersion( kIpVersion_4 )
{
	m_IpAddress.IPv4.word = NETWORKTOHOST32( *reinterpret_cast<const UInt32*>( inIpAddress ) );
	SetFormat();
}

CIpAddress::CIpAddress(
	const tIPv6Address* inIpAddress )
	:	m_nVersion( kIpVersion_6 )
{
	memcpy( &m_IpAddress.IPv6.bytes, inIpAddress->bytes, sizeof( tIPv6Address ) );
	SetFormat();
}


// ----------------------------------------------------------------------------
//      operator==
// ----------------------------------------------------------------------------

bool
CIpAddress::operator==(
	const CIpAddress& Other ) const
{
	if ( m_nVersion != Other.m_nVersion ) return false;
	if ( m_nVersion == kIpVersion_None ) return true;
	switch ( m_nVersion ) {
		case kIpVersion_4:
			return (m_IpAddress.IPv4.word == Other.m_IpAddress.IPv4.word);

		case kIpVersion_6:
			return ((m_IpAddress.IPv6.longs[0] == Other.m_IpAddress.IPv6.longs[0]) &&
				(m_IpAddress.IPv6.longs[1] == Other.m_IpAddress.IPv6.longs[1]));
	}
	return false;
}


// ----------------------------------------------------------------------------
//      GetName
// ----------------------------------------------------------------------------

CStringA
CIpAddress::GetName(
	CPeekContext*	inContext ) const
{
	CStringA				str;

	ASSERT( inContext );
	if ( inContext == NULL ) return str;

	PluginNameTableEntry	nteIpAddress;
	wchar_t					szName[256] = { 0 };
	UInt32					nIpAddress( NETWORKTOHOST32( m_IpAddress.IPv4.word ) );

	if ( m_nVersion != kIpVersion_None ) {
		memset( &nteIpAddress, 0, sizeof( PluginNameTableEntry ) );
		if ( m_nVersion == kIpVersion_4 ) {
			nteIpAddress.fEntryType = kEntryType_IPAddr;
			nteIpAddress.fEntry = reinterpret_cast<const UInt8*>( &nIpAddress );
		}
		else {
			nteIpAddress.fEntryType = kEntryType_IPv6Addr;
			nteIpAddress.fEntry = reinterpret_cast<const UInt8*>( &m_IpAddress );
		}
		nteIpAddress.fName = szName;
		
		inContext->DoLookupName( &nteIpAddress );

		str = szName;
	}

	return str;
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
//      Hash
// ----------------------------------------------------------------------------

UInt32
CIpAddress::Hash() const
{
	switch ( m_nVersion ) {
		case kIpVersion_4:
			return m_IpAddress.IPv4.word;

		case kIpVersion_6:
			return (m_IpAddress.IPv6.words[0] + m_IpAddress.IPv6.words[1] +
				m_IpAddress.IPv6.words[2] + m_IpAddress.IPv6.words[3]);

		case kIpVersion_None:
		default:
			break;
	}
	return 0;
}


// ----------------------------------------------------------------------------
//      SetFormat
// ----------------------------------------------------------------------------

void
CIpAddress::SetFormat()
{
	switch ( m_nVersion ) {
		case kIpVersion_4:
			{
				const UInt8	*pb( m_IpAddress.IPv4.bytes );
				m_strFormat.Format( "%u.%u.%u.%u", pb[3], pb[2], pb[1], pb[0] );
				m_strFormatPad.Format( "%03u.%03u.%03u.%03u", pb[3], pb[2], pb[1], pb[0] );
			}
			break;

		case kIpVersion_6:
			{
				const UInt16	*ps( m_IpAddress.IPv6.shorts );
				m_strFormat.Format( "%X", NETWORKTOHOST16( ps[0] ) );
				m_strFormatPad.Format( "%04X", NETWORKTOHOST16( ps[0] ) );
				size_t	i( 1 );
				if ( ps[1] == 0 ) {
					m_strFormat.Append( ":" );
					m_strFormatPad.Append( ":" );
					while ( (i < 7) && (ps[i] == 0) ) {
						i++;
					}
				}
				while ( i < 8 ) {
					m_strFormat.AppendFormat( ":%X", NETWORKTOHOST16( ps[i] ) );
					m_strFormatPad.AppendFormat( ":%04X", NETWORKTOHOST16( ps[i] ) );
					i++;
				}
			}
			break;
	}
}


// ============================================================================
//      CIpAddressPair
// ============================================================================

//CIpAddressPair::CIpAddressPair(
//	const tIpHeader*	inIpHeader )
//{
//	if ( inIpHeader ) {
//		m_Source = ntohl( inIpHeader->Source.word );
//		m_Destination = ntohl( inIpHeader->Destination.word );
//	}
//}


// ----------------------------------------------------------------------------
//      operator ==
// ----------------------------------------------------------------------------

bool
CIpAddressPair::operator==(
	const CIpAddressPair&	inOther ) const
{
	return (m_Source == inOther.m_Source) && (m_Destination == inOther.m_Destination);
}


// ----------------------------------------------------------------------------
//      IsInverse
// ----------------------------------------------------------------------------

bool
CIpAddressPair::IsInverse(
	const CIpAddressPair&	inOther ) const
{
	return (m_Destination == inOther.m_Source) && (m_Source == inOther.m_Destination);
}


// ============================================================================
//      CIpAddressPort
// ============================================================================

CIpAddressPort::CIpAddressPort(
	CIpAddress	inIpAddress,
	CIpPort		inPort )
	:	m_IpAddress( inIpAddress )
	,	m_IpPort( inPort )
{
}


// ----------------------------------------------------------------------------
//		operator==
// ----------------------------------------------------------------------------

inline bool
CIpAddressPort::operator==(
	const CIpAddressPort&	inOther ) const
{
	return(
		(m_IpAddress == inOther.m_IpAddress) &&
		(m_IpPort == inOther.m_IpPort) );
}


// ============================================================================
//      CIpAddressPortPair
// ============================================================================

CIpAddressPortPair::CIpAddressPortPair(
	CIpAddressPort inSrc,
	CIpAddressPort inDst )
	:	m_Source( inSrc )
	,	m_Destination( inDst )
{
}

// ----------------------------------------------------------------------------
//		operator==
// ----------------------------------------------------------------------------

bool
CIpAddressPortPair::operator==(
	const CIpAddressPortPair& inOther ) const
{
	return(
		(m_Source == inOther.m_Source) &&
		(m_Destination == inOther.m_Destination) );
}


// ----------------------------------------------------------------------------
//		Init
// ----------------------------------------------------------------------------

void
CIpAddressPortPair::Init(
	CIPHeader*			inIpHeader,
	const tTcpHeader*	inTcpHeader /* = NULL */ )
{
	ASSERT( inIpHeader );

	if ( inIpHeader ) {
		m_Source.SetIpAddress( inIpHeader->GetSource() );
		m_Destination.SetIpAddress( inIpHeader->GetDestination() );
	}
	if ( inTcpHeader ) {
		m_Source.SetPort( NETWORKTOHOST16( inTcpHeader->SourcePort ) );
		m_Destination.SetPort( NETWORKTOHOST16( inTcpHeader->DestinationPort ) );
	}
}

void
CIpAddressPortPair::Init(
	CIPHeader*			inIpHeader,
	const tUdpHeader*	inUdpHeader )
{
	ASSERT( inIpHeader );
	ASSERT( inUdpHeader );

	if ( inIpHeader ) {
		m_Source.SetIpAddress( inIpHeader->GetSource() );
		m_Destination.SetIpAddress( inIpHeader->GetDestination() );
	}
	if ( inUdpHeader ) {
		m_Source.SetPort( NETWORKTOHOST16( inUdpHeader->SourcePort ) );
		m_Destination.SetPort( NETWORKTOHOST16( inUdpHeader->DestinationPort ) );
	}
}


// ----------------------------------------------------------------------------
//		IsInverse
// ----------------------------------------------------------------------------

bool
CIpAddressPortPair::IsInverse(
	const CIpAddressPortPair& inOther ) const
{
	return(
		(m_Source == inOther.m_Destination) &&
		(m_Destination == inOther.m_Source) );
}


// ============================================================================
//		CIPv4Header
// ============================================================================

// ----------------------------------------------------------------------------
//		GetDestination
// ----------------------------------------------------------------------------

CIpAddress
CIPv4Header::GetDestination() const
{
	CIpAddress	IpAddress( NETWORKTOHOST32( m_pIpHeader->Destination.word ) );
	return IpAddress;
}


// ----------------------------------------------------------------------------
//		GetPayload
// ----------------------------------------------------------------------------

const UInt8*
CIPv4Header::GetPayload() const
{
	const UInt8*	pPayload( reinterpret_cast<const UInt8*>( m_pIpHeader ) );
	pPayload += GetLength();
	return pPayload;
}


// ----------------------------------------------------------------------------
//		GetSource
// ----------------------------------------------------------------------------

CIpAddress
CIPv4Header::GetSource() const
{
	CIpAddress	IpAddress( NETWORKTOHOST32( m_pIpHeader->Source.word ) );
	return IpAddress;
}


// ----------------------------------------------------------------------------
//		IsFragment
// ----------------------------------------------------------------------------

bool
CIPv4Header::IsFragment() const
{
	const UInt8*	pHeaderData( reinterpret_cast<const UInt8*>( m_pIpHeader ) );

	// Determine if this is an IP fragment
	UInt16	nFlagsFragmentOffset = ntohs( m_pIpHeader->FlagsFragmentOffset );

	UInt8	nFragmentFlags( (nFlagsFragmentOffset & 0xE000) >> 13 );
	nFragmentFlags = (pHeaderData[6] & 0xE0) >> 5;
	
	UInt16	nFragmentOffset( nFlagsFragmentOffset & 0x001FFFFF );
	nFragmentOffset = (((pHeaderData[6] & 0x1F) << 8) & 0xFF00) + pHeaderData[7];

	if ( (nFragmentFlags == 0x01) || (nFragmentOffset != 0) ) {
		// The packet is an IP Fragment
		return true;
	}
	return false;
}


// ============================================================================
//		CIPv6Header
// ============================================================================

// ----------------------------------------------------------------------------
//		GetDestination
// ----------------------------------------------------------------------------

CIpAddress
CIPv6Header::GetDestination() const
{
	CIpAddress	IpAddress( &m_pIpHeader->Destination );
	return IpAddress;
}


// ----------------------------------------------------------------------------
//		GetPayload
// ----------------------------------------------------------------------------

const UInt8*
CIPv6Header::GetPayload() const
{
	const UInt8*	pPayload( reinterpret_cast<const UInt8*>( m_pIpHeader ) );
	pPayload += GetLength();
	return pPayload;
}


// ----------------------------------------------------------------------------
//		GetSource
// ----------------------------------------------------------------------------

CIpAddress
CIPv6Header::GetSource() const
{
	CIpAddress	IpAddress( &m_pIpHeader->Source );
	return IpAddress;
}
