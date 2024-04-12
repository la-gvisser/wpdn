// =============================================================================
//	IpAddress.cpp
//  implements CIpAddress class
// =============================================================================
//	Copyright (c) 2004 WildPackets, Inc.

#include "StdAfx.h"
#include "IpAddress.h"


////////////////////////////////////////////////////////////////////////////////
//      CIpAddress
////////////////////////////////////////////////////////////////////////////////

CIpAddress::CIpAddress(
	const UInt32 nIpAddress )
	:	m_IpAddress( nIpAddress )
{
}

CIpAddress::~CIpAddress()
{
}


// -----------------------------------------------------------------------------
//      FormatA
// -----------------------------------------------------------------------------

CStringA
CIpAddress::FormatA() const
{
	UInt8*		pb = (UInt8 *) &m_IpAddress;
	CStringA	str;
	str.Format( "%03u.%03u.%03u.%03u", pb[3], pb[2], pb[1], pb[0] );
	return str;
}


////////////////////////////////////////////////////////////////////////////////
//      CIpAddressPort
////////////////////////////////////////////////////////////////////////////////

CIpAddressPort::CIpAddressPort()
	:	m_IpAddress( 0 )
	,	m_nPort( 0 )
{
}

CIpAddressPort::CIpAddressPort(
	CIpAddress	inIpAddress,
	UInt16		inPort )
	:	m_IpAddress( inIpAddress )
	,	m_nPort( inPort )
{
}


// -----------------------------------------------------------------------------
//		operator==
// -----------------------------------------------------------------------------

inline bool
CIpAddressPort::operator==(
	const CIpAddressPort&	inOther ) const
{
	return(
		(m_IpAddress == inOther.m_IpAddress) &&
		(m_nPort == inOther.m_nPort) );
}


////////////////////////////////////////////////////////////////////////////////
//      CIpAddressPortPair
////////////////////////////////////////////////////////////////////////////////

CIpAddressPortPair::CIpAddressPortPair(
	CIpAddressPort inSrc,
	CIpAddressPort inDst )
	:	m_Src( inSrc )
	,	m_Dst( inDst )
{
}


// -----------------------------------------------------------------------------
//		operator==
// -----------------------------------------------------------------------------

bool
CIpAddressPortPair::operator==(
	const CIpAddressPortPair& inOther ) const
{
	return(
		(m_Src == inOther.m_Src) &&
		(m_Dst == inOther.m_Dst) );
}


// -----------------------------------------------------------------------------
//		IsInverse
// -----------------------------------------------------------------------------

bool
CIpAddressPortPair::IsInverse(
	const CIpAddressPortPair& inOther ) const
{
	return(
		(m_Src == inOther.m_Dst) &&
		(m_Dst == inOther.m_Src) );
}
