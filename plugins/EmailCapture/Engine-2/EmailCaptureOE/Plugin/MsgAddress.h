// ============================================================================
//	MsgAddress.h
//		interface for the CMsgAddress class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "TargetAddress.h"
#include "PeekStrings.h"
#include "MsgText.h"
#include <vector>


// ============================================================================
//		CMsgAddress
// ============================================================================

class CMsgAddress
{
public:
	enum MsgAddressType {
		kType_From,
		kType_To,
		kType_CC,
		kType_BCC
	};

protected:
	static CPeekStringA		m_strNoAddress;

protected:
	bool				m_bExplicit;	// Is this a SMTP Mail From or Rcpt To?
	MsgAddressType		m_Type;
	CPeekStringX		m_strName;
	CPeekStringX		m_strAddress;

public:
	;			CMsgAddress()
				:	m_bExplicit( false )
				,	m_Type( kType_BCC )
				,	m_strAddress( m_strNoAddress )
				{}
	;			CMsgAddress( const CPeekString& inAddress )
				:	m_bExplicit( false )
				,	m_Type( kType_BCC )
				,	m_strAddress( inAddress )
				{}
	;			CMsgAddress( const CPeekStringA& inAddress )
				:	m_bExplicit( false )
				,	m_Type( kType_BCC )
				,	m_strAddress( inAddress )
				{}
	;			CMsgAddress( MsgAddressType inType, const CPeekStringA& inAddress )
				:	m_bExplicit( false )
				,	m_Type( inType )
				,	m_strAddress( inAddress )
				{}
	;			CMsgAddress( MsgAddressType inType, const CPeekStringA& inAddress, const CPeekStringA& inName )
				:	m_bExplicit( false )
				,	m_Type( inType )
				,	m_strName( inName )
				,	m_strAddress( inAddress )
				{}
	;			CMsgAddress( MsgAddressType inType, const CPeekString& inAddress )
				:	m_bExplicit( false )
				,	m_Type( inType )
				,	m_strAddress( inAddress )
				{}
	virtual		~CMsgAddress(){}

	void		Reset() {
		m_bExplicit = false;
		m_Type = kType_BCC;
		m_strName.Empty();
		m_strAddress = m_strNoAddress;
	}

	CPeekString		Format() const;
	CPeekStringA	FormatA() const;
	CPeekString		FormatLong() const;
	CPeekStringA	FormatLongA() const;

	const CPeekStringX&	GetAddress() const { return m_strAddress; }
	const CPeekStringX&	GetName() const { return m_strName; }
	MsgAddressType		GetType() const { return m_Type; }
	const CPeekString&	GetTypeString() const;
	const CPeekStringA&	GetTypeStringA() const;

	bool	IsExplicit() const { return m_bExplicit; }
	bool	IsFrom() const { return (m_Type == kType_From); }
	bool	IsMatch( const CPeekStringA& inAddress ) const;

	void	SetExplicit(){ m_bExplicit = true; }
	void	SetType( MsgAddressType inType ) {
		if ( m_Type == kType_BCC ) {
			m_Type = inType;
		}
	}
	void	SetName( const CPeekStringA& inName ) { m_strName = inName; }
	void	SetAddress( const CPeekString& inAddress ) { m_strAddress = inAddress; }
};


// ============================================================================
//		CMsgAddressList
// ============================================================================

class CMsgAddressList
	: public std::vector<CMsgAddress>
{
public:
	size_t			Parse( CMsgAddress::MsgAddressType inType, const CMsgText& inMsgHdr );
	void			ResolveName( CMsgAddress& inTarget, bool bSetType, bool bAddMissing );
	CPeekStringA	FormatEmlA( CMsgAddress::MsgAddressType inType );
	CPeekString		FormatTxt( CMsgAddress::MsgAddressType inType );
	CPeekStringA	FormatTxtA( CMsgAddress::MsgAddressType inType );
	bool			Find( const CTargetAddress& inItem, size_t& outIndex ) const;
	bool			Contains( const CTargetAddress& inItem ) const;
};
