// ============================================================================
//	CMIHeaders.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#pragma once

#include "IpHeaders.h"


typedef struct _tVLANHeader {
	UInt16	nVlanTag;
	UInt16	nProtocolType;
} tVLANHeader;


// ============================================================================
//		CCmiEthernetAddress
// ============================================================================

class CCmiEthernetAddress
	:	public CEthernetAddress
{
public:
	;			CCmiEthernetAddress() : CEthernetAddress() {}
	;			CCmiEthernetAddress( const tEthernetAddress& inAddress ) : CEthernetAddress( inAddress ) {}
	;			CCmiEthernetAddress( const wchar_t* inAddress ) : CEthernetAddress( inAddress ) {}

	CCmiEthernetAddress&	operator=( const wchar_t* inOther ) { 
		return static_cast<CCmiEthernetAddress&>( CEthernetAddress::operator=( inOther ) ); 
	}
	bool					operator==( const CCmiEthernetAddress& inOther ) {
		return CEthernetAddress::operator ==( static_cast<CEthernetAddress>( inOther ) );
	}
	bool					operator!=( const CCmiEthernetAddress& inOther ) {
		return CEthernetAddress::operator != ( static_cast<CEthernetAddress>( inOther ) );
	}

	void		Duplicate( size_t inLength, UInt8* outAddress ) const {
		if ( inLength >= sizeof( tEthernetAddress ) ) {
			memcpy( outAddress, &m_EthernetAddress, sizeof( tEthernetAddress ) );
		}
	}
};


// ============================================================================
//		CCmiIpV4Header
// ============================================================================

class CCmiIpV4Header
	:	public CIpV4HeaderRef
{
	friend class CCmiIpV4Packet;

public:
	;		CCmiIpV4Header() : CIpV4HeaderRef() {}
	virtual ~CCmiIpV4Header() {}

	void	AdjustCmi( const tIpV4Header* inHeader )  {
		Set( inHeader );
	}
};


// ============================================================================
//		CCmiUdpHeader
// ============================================================================

class CCmiUdpHeader
	: public CUdpHeaderRef
{

protected:
	bool		m_bFragment;
	UInt16		m_nUdpPayloadLength;
	UInt16		m_nUdpPayloadAvailable;

public:
	;			CCmiUdpHeader() 
		: CUdpHeaderRef()
		, m_bFragment( false )
		, m_nUdpPayloadLength( 0 )
		, m_nUdpPayloadAvailable( 0 )
	{}

	virtual		~CCmiUdpHeader() {}
	
	bool		AdjustCmi( const tUdpHeader* inHeader, UInt16 inHeaderBytesRemaining ) {
		ASSERT( inHeader != nullptr );
		if ( inHeader == nullptr ) return false;

		CUdpHeaderRef::Set( inHeader );

		if ( inHeaderBytesRemaining >= sizeof( tUdpHeader ) )  {
			UInt16 nLengthPerUdpHeader = GetLength();
			UInt16 nAvailableLength( inHeaderBytesRemaining - 4 );
			if ( nLengthPerUdpHeader > nAvailableLength ) {
				m_bFragment = true;
				m_nUdpPayloadAvailable = nAvailableLength - sizeof( tUdpHeader );
			}
			m_nUdpPayloadLength = nLengthPerUdpHeader - sizeof( tUdpHeader );
		}
		return IsValid();
	}

	UInt16		GetPayloadLength() const { return m_nUdpPayloadLength; }
	UInt16		GetPayloadDataLength() const {
		return (m_bFragment) ? m_nUdpPayloadAvailable : m_nUdpPayloadLength;
	}
};
