// =============================================================================
//	GlobalId.h
// =============================================================================
//	Copyright (c) 2009-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "heid.h"
#include "hlruntime.h"
#include "hlid.h"
#include "hlcom.h"
#include "PeekStrings.h"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Global Identifier
//
//	A Global Identifier is a GUID with additional services and support: creation
//	formatting (display), parsing and validation.
//
//	A Global Identifier has three states: Invalid (all ones), Null (All zeros)
//	and Valid (not Invalid). It may also be Self Created.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


#ifdef WP_LINUX
static const GUID	GUID_NULL = {
	0x00000000,
	0x0000,
	0x0000,
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};
#endif

static const GUID	GUID_INVALID = {
	0xFFFFFFFF,
	0xFFFF,
	0xFFFF,
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
};


// =============================================================================
//		CGlobalId
// =============================================================================

class CGlobalId
{
protected:
	GUID	m_Id;
	bool	m_bSelfCreated;

public:
	static bool	IsEqual( const GUID& inA, const GUID& inB ) {
#ifdef WIN64
		return (
			(reinterpret_cast<const UInt64 *>( &inA )[0] ==
				reinterpret_cast<const UInt64 *>( &inB )[0]) &&
			(reinterpret_cast<const UInt64 *>( &inA )[1] ==
				reinterpret_cast<const UInt64 *>( &inB )[1]) );
#else
		return (memcmp( &inA, &inB, sizeof( GUID ) ) == 0);
#endif
	}
	static bool	IsNotEqual( const GUID& inA, const GUID& inB ) {
#ifdef WIN64
		return (
			(reinterpret_cast<const UInt64 *>( &inA )[0] !=
				reinterpret_cast<const UInt64 *>( &inB )[0]) ||
			(reinterpret_cast<const UInt64 *>( &inA )[1] !=
				reinterpret_cast<const UInt64 *>( &inB )[1]) );
#else
		return (memcmp( &inA, &inB, sizeof( GUID ) ) != 0);
#endif
	}

public:
	;		CGlobalId( bool inCreate = false )
				: m_Id( GUID_INVALID )
				, m_bSelfCreated( false )
	{
		if ( inCreate ) Create();
	}
	;		CGlobalId( const GUID& inId ) : m_Id( inId ), m_bSelfCreated( false ) {}
	;		CGlobalId( const Helium::HeID& inId )
				: m_bSelfCreated( false )
	{
		m_Id = *(reinterpret_cast<const GUID *>( &inId ));
	}
	;		CGlobalId( const wchar_t* inId )
				: m_bSelfCreated( false )
	{
		Parse( inId );
	}

	bool	operator==( const CGlobalId& inOther ) const { return IsEqual( m_Id, inOther.m_Id ); }
	bool	operator!=( const CGlobalId& inOther ) const { return IsNotEqual( m_Id, inOther.m_Id ); }
	bool	operator<( const CGlobalId& inOther ) const {
		return (
			(reinterpret_cast<const UInt64 *>( this )[0] <
				reinterpret_cast<const UInt64 *>( &inOther )[0]) ||
			(reinterpret_cast<const UInt64 *>( this )[1] <
				reinterpret_cast<const UInt64 *>( &inOther )[1]) );
	}
	;		operator GUID&() { return m_Id; }
	;		operator Helium::HeID() { return *(reinterpret_cast<Helium::HeID*>( &m_Id )); }

	CGlobalId&	operator=( const CGlobalId& inOther ) {
		m_Id = inOther.m_Id;
		m_bSelfCreated = inOther.m_bSelfCreated;
		return *this;
	}
	CGlobalId&	operator=( const GUID& inOther ) {
		m_Id = inOther;
		m_bSelfCreated = false;
		return *this;
	}
	CGlobalId&	operator=( const Helium::HeID& inOther ) {
		m_Id = *(reinterpret_cast<const GUID*>( &inOther ));
		m_bSelfCreated = false;
		return *this;
	}
	CGlobalId&	operator=( const char* inOther ) {
		Helium::HeID	id = HeLib::HeIDFromString( inOther );
		m_Id = *(reinterpret_cast<const GUID*>( &id ));
		m_bSelfCreated = false;
		return *this;
	}
	CGlobalId&	operator=( const wchar_t* inOther ) {
		Parse( inOther );
		return *this;
	}
	CGlobalId&	operator=( const CPeekString& inOther ) {
		Parse( inOther.c_str() );
		return *this;
	}

	void	Create() {
		Peek::ThrowIfFailed( HE_SUCCEEDED( HeLib::HeCreateID( (Helium::HeID*)&m_Id ) ) );
		m_bSelfCreated = true;
	}

	CPeekString		Format() const {
		HeLib::CHeIDToStringW	strId( *(reinterpret_cast<const Helium::HeID*>( &m_Id )) );
		return CPeekString( strId.m_sz );
	}
	CPeekStringA	FormatA() const {
		HeLib::CHeIDToStringA	strId( *(reinterpret_cast<const Helium::HeID*>( &m_Id )) );
		return strId.m_sz;
	}

	void	Invalidate() { m_Id = GUID_INVALID; m_bSelfCreated = false; }
	bool	IsSelfCreated() { return m_bSelfCreated; }
	bool	IsNotSelfCreated() { return !m_bSelfCreated; }
	bool	IsNotValid() const { return IsEqual( m_Id, GUID_INVALID ); }
	bool	IsNull() const { return IsEqual( m_Id, GUID_NULL ); }
	bool	IsNotNull() const { return IsNotEqual( m_Id, GUID_NULL ); }
	bool	IsValid() const { return IsNotEqual( m_Id, GUID_INVALID ); }

	void	Parse( const CPeekString& inId ) {
		Parse( inId.c_str() );
	}
	void	Parse( const wchar_t* inId ) {
		Reset();
		Helium::HeID	id = HeLib::HeIDFromString( inId );
		m_Id = *(reinterpret_cast<const GUID*>( &id ));
	}
	void	Parse( const char* inId ) {
		Reset();
		Helium::HeID	id = HeLib::HeIDFromString( inId );
		m_Id = *(reinterpret_cast<const GUID*>( &id ));
	}

	void	Reset() { m_Id = GUID_INVALID; m_bSelfCreated = false; }
};
