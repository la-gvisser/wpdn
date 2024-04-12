// ============================================================================
//	ProtocolType.h
//		interface for the CProtocolType class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PSIDs.h"
#include "Protospecs.h"
#include "GlobalTags.h"


// ============================================================================
//		CProtocolType
// ============================================================================

class CProtocolType
{
public:
	static const CPeekStringX	Format( tProtocolTypes inType ) {
		if ( IsValid( inType ) ) return Tags::kxTypesProtocol[inType];
		return Tags::kxTypesProtocol[kProtocol_Max];
	}

protected:
	size_t	m_Type;

public:
	;		CProtocolType() : m_Type( kProtocol_SMTP ) {}
	;		CProtocolType( tProtocolTypes inType ) : m_Type( inType ) {}
	;		CProtocolType( PROTOSPEC_ID inId )
		:	m_Type( kProtocol_Max )
	{
		switch ( inId ) {
			case ProtoSpecDefs::SMTP:
				m_Type = kProtocol_SMTP;
				break;
			case ProtoSpecDefs::POP3:
				m_Type = kProtocol_POP3;
				break;
		}
	}

	CProtocolType	operator++(int) {
		m_Type++;
		if ( m_Type > kProtocol_Max ) m_Type = kProtocol_Max;
		return *this;
	}

	;	operator size_t() { return m_Type; }
	;	operator tProtocolTypes() { return static_cast<tProtocolTypes>( m_Type ); }

	CPeekStringX	Format() { return Tags::kxTypesProtocol[m_Type]; }

	bool			IsValid() const { return (m_Type < kProtocol_Max); }
	static bool		IsValid( tProtocolTypes inType ) { return (inType < kProtocol_Max); }
};
