// ============================================================================
//	IpAddress.cpp
//		implements CIpAddress class
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "IpAddress.h"


// ============================================================================
//      CUInt16
// ============================================================================

// -----------------------------------------------------------------------------
//      Format
// -----------------------------------------------------------------------------

CString
CUInt16::Format() const
{
	CString	str;
	str.Format( _T( "%u" ), m_Value );
	return str;
}


// ----------------------------------------------------------------------------
//      FormatA
// ----------------------------------------------------------------------------

CStringA
CUInt16::FormatA() const
{
	CStringA	str;
	str.Format( "%u", m_Value );
	return str;
}


// ============================================================================
//      CUInt32
// ============================================================================

// -----------------------------------------------------------------------------
//      Format
// -----------------------------------------------------------------------------

CString
CUInt32::Format() const
{
	CString	str;
	str.Format( _T( "%u" ), m_Value );
	return str;
}


// ----------------------------------------------------------------------------
//      FormatA
// ----------------------------------------------------------------------------

CStringA
CUInt32::FormatA() const
{
	CStringA	str;
	str.Format( "%u", m_Value );
	return str;
}


// ============================================================================
//      CMacAddress
// ============================================================================

//CMacAddress::CMacAddress(
//	const TCHAR* inAddress )
//{
//	CString	strSeperators( _T( ":.-" ) );
//	CString	strAddress( inAddress );
//	int	nIndex = 0;
//	for ( int i = 0; i < 6; i++ ) {
//		CString	strHex = strAddress.Mid( nIndex, 2 );
//		m_MacAddress[i] = static_cast<UInt8>( _tcstol( strHex, NULL, 16 ) );
//		nIndex += 2;
//		if ( strSeperators.Find( strAddress[nIndex] ) >= 0 ) {
//			nIndex++;
//		}
//	}
//}

// -----------------------------------------------------------------------------
//      Format
// -----------------------------------------------------------------------------

CString
CMacAddress::Format() const
{
	CString	str;
	if ( !IsNull() ) {
		const UInt8*	pb = reinterpret_cast<const UInt8*>( &m_MacAddress );
		str.Format( _T( "%02X:%02X:%02X:%02X:%02X:%02X" ), pb[0], pb[1], pb[2], pb[3], pb[4], pb[5] );
	}
	return str;
}


// ----------------------------------------------------------------------------
//      FormatA
// ----------------------------------------------------------------------------

CStringA
CMacAddress::FormatA() const
{
	CStringA	str;
	if ( !IsNull() ) {
		const UInt8*	pb = reinterpret_cast<const UInt8*>( &m_MacAddress );
		str.Format( "%02X:%02X:%02X:%02X:%02X:%02X", pb[0], pb[1], pb[2], pb[3], pb[4], pb[5] );
	}
	return str;
}


// ----------------------------------------------------------------------------
//      operator=(CString)
// ----------------------------------------------------------------------------

CMacAddress&
CMacAddress::operator=(
	CString	inAddress )
{
	CString	strSeperators( _T( ":.-" ) );
	CString	strAddress( inAddress );
	int	nIndex = 0;
	for ( int i = 0; i < 6; i++ ) {
		CString	strHex = strAddress.Mid( nIndex, 2 );
		m_MacAddress[i] = static_cast<UInt8>( _tcstol( strHex, NULL, 16 ) );
		nIndex += 2;
		if ( strSeperators.Find( strAddress[nIndex] ) >= 0 ) {
			nIndex++;
		}
	}
	return *this;
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
				m_strFormat.Format( L"%u.%u.%u.%u", pb[3], pb[2], pb[1], pb[0] );
				m_strFormatA.Format( "%u.%u.%u.%u", pb[3], pb[2], pb[1], pb[0] );
			}
			break;

		case kIpVersion_6:
			{
				const UInt16	*ps( m_IpAddress.IPv6.shorts );
				m_strFormat.Format( L"%X", NETWORKTOHOST16( ps[0] ) );
				m_strFormatA.Format( "%X", NETWORKTOHOST16( ps[0] ) );
				size_t	i( 1 );
				if ( ps[1] == 0 ) {
					m_strFormat.Append( L":" );
					m_strFormatA.Append( ":" );
					while ( (i < 7) && (ps[i] == 0) ) {
						i++;
					}
				}
				while ( i < 8 ) {
					m_strFormat.AppendFormat( L":%X", NETWORKTOHOST16( ps[i] ) );
					m_strFormatA.AppendFormat( ":%X", NETWORKTOHOST16( ps[i] ) );
					i++;
				}
			}
			break;
	}
}


// ============================================================================
//      CIpAddressPort
// ============================================================================

// ----------------------------------------------------------------------------
//		operator==
// ----------------------------------------------------------------------------

inline bool
CIpAddressPort::operator==(
	const CIpAddressPort&	inOther ) const
{
	return(
		(m_IpAddress == inOther.m_IpAddress) &&
		(m_Port == inOther.m_Port) );
}


// -----------------------------------------------------------------------------
//      Format
// -----------------------------------------------------------------------------

CString
CIpAddressPort::Format() const
{
	CString		str;
	str.Format( _T( "%s:%s" ), m_IpAddress.Format(), m_Port.Format() );
	return str;
}


// ============================================================================
//      CIpAddressPortPair
// ============================================================================

// ----------------------------------------------------------------------------
//		operator==
// ----------------------------------------------------------------------------

bool
CIpAddressPortPair::operator==(
	const CIpAddressPortPair&	inOther ) const
{
	return(
		(m_Source == inOther.m_Source) &&
		(m_Destination == inOther.m_Destination) );
}


// -----------------------------------------------------------------------------
//      Format
// -----------------------------------------------------------------------------

CString
CIpAddressPortPair::Format() const
{
	CString		str;
	str.Format( _T( "S-%s D-%s" ), m_Source.Format(), m_Destination.Format() );
	return str;
}


// ----------------------------------------------------------------------------
//		IsInverse
// ----------------------------------------------------------------------------

bool
CIpAddressPortPair::IsInverse(
	const CIpAddressPortPair&	inOther ) const
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
	CIpAddress	IpAddress;
	IpAddress = (m_pIpHeader) ? (NETWORKTOHOST32( m_pIpHeader->Destination.word )) : 0;
	return IpAddress;
}


// ----------------------------------------------------------------------------
//		GetPayload
// ----------------------------------------------------------------------------

const UInt8*
CIPv4Header::GetPayload() const
{
	if ( m_pIpHeader == NULL ) return NULL;
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
	CIpAddress	IpAddress;
	IpAddress = (m_pIpHeader) ? (NETWORKTOHOST32( m_pIpHeader->Source.word )) : 0;
	return IpAddress;
}


//// ----------------------------------------------------------------------------
////		IsFragment
//// ----------------------------------------------------------------------------
//
//bool
//CIPv4Header::IsFragment() const
//{
//	if ( m_pIpHeader == NULL ) return false;
//	const UInt8*	pHeaderData( reinterpret_cast<const UInt8*>( m_pIpHeader ) );
//
//	// Determine if this is an IP fragment
//	UInt16	nFlagsFragmentOffset = NETWORKTOHOST16( m_pIpHeader->FlagsFragmentOffset );
//
//	UInt8	nFragmentFlags( (nFlagsFragmentOffset & 0xE000) >> 13 );
//	UInt16	nFragmentOffset( nFlagsFragmentOffset & 0x1FFF );
//	if ( (nFragmentFlags == 0x01) || (nFragmentOffset != 0) ) {
//		// The packet is an IP Fragment
//		return true;
//	}
//	return false;
//}


// ============================================================================
//		CIPv6Header
// ============================================================================

// ----------------------------------------------------------------------------
//		GetDestination
// ----------------------------------------------------------------------------

CIpAddress
CIPv6Header::GetDestination() const
{
	CIpAddress	IpAddress;
	if ( m_pIpHeader ) {
		CIpAddress	IpAddress( &m_pIpHeader->Destination );
		return IpAddress;
	}
	return IpAddress;
}


// ----------------------------------------------------------------------------
//		GetPayload
// ----------------------------------------------------------------------------

const UInt8*
CIPv6Header::GetPayload() const
{
	if ( m_pIpHeader == NULL ) return NULL;
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
	CIpAddress	IpAddress;
	if ( m_pIpHeader ) {
		CIpAddress	IpAddress( &m_pIpHeader->Source );
		return IpAddress;
	}
	return IpAddress;
}
