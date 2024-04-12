// =============================================================================
//	Packet.cpp
//		implementation of the CPacket class.
// =============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#include "Packet.h"
#include "MemUtil.h"
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>

using std::wostringstream;
using std::unique_ptr;
using namespace MemUtil;


namespace CaptureFile {

// ============================================================================
//		Globals
// ============================================================================

wstring
CByteVectorStream:: Format() const
{
	const UInt8*	pData( m_pData );
	wostringstream	ss;
	for ( size_t i = 0; i < m_nLength; ++i ) {
		if ( (i % 16) == 0 ) {
			ss << std::endl;
		}
		ss << std::hex << std::setfill( L'0' ) << std::setw( 2 ) << *pData++ << L" ";
	}
	ss << std::endl;
	return ss.str();
}


// ----------------------------------------------------------------------------
//		CEthernetAddress::Format
// ----------------------------------------------------------------------------

wstring
CEthernetAddress::Format() const
{
	wostringstream	ss;
	ss << std::hex;
	ss << std::setfill( L'0' );
	for ( int i = 0; i < 5; i++ ) {
		ss << std::setw( 2 ) << m_bytes[i] << L":";
	}
	ss << std::setw( 2 ) << m_bytes[5];

	return ss.str();
}

// ----------------------------------------------------------------------------
//		CIPv4Address::Format
// ----------------------------------------------------------------------------

wstring
CIPv4Address::Format() const
{
	wostringstream	ss;
	for ( int i = 3; i >= 1; --i ) {
		ss << m_bytes[i] << L".";
	}
	ss << m_bytes[0];

	return ss.str();
}

// ----------------------------------------------------------------------------
//		CIPv6Address::Format
// ----------------------------------------------------------------------------

wstring
CIPv6Address::Format() const
{
	const UInt16*	pShorts( reinterpret_cast<const UInt16*>( m_bytes.data() ) );

	wostringstream	ss;
	ss << std::hex;
	ss << std::setfill( L'0' );
	ss << std::setw( 4 ) << NETWORKTOHOST16( pShorts[0] );
	size_t	i = 1;
	for ( ; ((i < 7) && (pShorts[i] != 0)); ++i ) {
		ss << L":" << std::setw( 4 ) << NETWORKTOHOST16( pShorts[i] );
	}
	if ( (pShorts[i] == 0) ) {
		ss << L":";
		while ( (i < 7) && (pShorts[i] == 0) ) ++i;
	}
	for ( ; i < 8; ++i ) {
		ss << L":" << std::setw( 4 ) << NETWORKTOHOST16( pShorts[i] );
	}

	return ss.str();
}


// ============================================================================
//		CIPv4Header
// ============================================================================

size_t
CIPv4Header::Load(
	CByteVectorStream& inData )
{
	size_t	nLength( Length() );
	size_t	inLength( inData.Length() );

	if ( inLength < nLength ) {
		throw std::out_of_range( "Not enough data." );
	}

	UInt8	nVersionLength( inData.ReadUInt8() );
	m_nVersion = nVersionLength >> 4;
	m_nLength = (nVersionLength & 0x0F) * 4;	// Length in 32-bit units.

	m_nTypeOfService = inData.ReadUInt8();
	m_nTotalLength = NETWORKTOHOST16( inData.ReadUInt16() );
	m_nIdentifier = NETWORKTOHOST16( inData.ReadUInt16() );

	UInt16	nFlagsFragmentOffset( inData.ReadUInt16() );
	m_nFlags = nFlagsFragmentOffset >> 13;
	m_nFragmentOffset = nFlagsFragmentOffset & 0x1FFF;

	m_nTimeToLive = inData.ReadUInt8();
	m_nProtocol = inData.ReadUInt8();
	m_nHeaderChecksum = NETWORKTOHOST16( inData.ReadUInt16() );
	m_ipaSource.Load( inData );
	m_ipaDestination.Load( inData );

	return Length();
}


// ============================================================================
//		CIPv6Header
// ============================================================================

size_t
CIPv6Header::Load(
	CByteVectorStream& inData )
{
	size_t	nLength( Length() );
	size_t	inLength( inData.Length() );

	if ( inLength < nLength ) {
		throw std::out_of_range( "Not enough data." );
	}

	m_nVersionClassLabel = inData.ReadUInt32();
	m_nPayloadLength = inData.ReadUInt16();
	m_nNextHeader = inData.ReadUInt8();
	m_nHopLimit = inData.ReadUInt8();
	m_ipaSource.Load( inData );
	m_ipaDestination.Load( inData );

	return Length();
}


// ============================================================================
//		CPacket
// ============================================================================

bool
CPacket::ParsePacketData(
	const CByteVector&	inData )
{
	if ( inData.size() < 14 ) return false;

	try {
		CByteVectorStream	bvs( inData );

		m_Ethernet.Load( bvs );

		if ( m_Ethernet.m_nProtocolType == ETH_TYPE_IPV4 ) {
			m_spIP.reset( new CIPv4Header() );
		}
		else if ( m_Ethernet.m_nProtocolType == ETH_TYPE_IPV6 ) {
			m_spIP.reset( new CIPv6Header() );
		}
		if ( m_spIP ) {
			m_spIP->Load( bvs );
		}

		// if ( m_IP.Load( m_Ethernet.m_ProtocolType, bvs ) ) {
		// 	switch ( m_IP.Protocol() ) {
		// 		case IP_TYPE_TCP:
		// 			m_TCP.Load( bvs );
		// 			break;
				
		// 		case IP_TYPE_UDP:
		// 			m_UDP.Load( bvs );
		// 			break;

		// 		default:
		// 			break;
		// 	}
		// }
	}
	catch ( std::out_of_range& e ) {
		std::cerr << e.what() << std::endl;
		return false;
	}

	return true;
}

};
