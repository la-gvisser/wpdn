// ============================================================================
//	StringX.h
//		interface for the CStringX.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once


// ============================================================================
//		CStringX
// ============================================================================

class CStringX
{
protected:
	CStringA	m_strA;
	CStringW	m_strW;

public:
	;			CStringX() {}
	;			CStringX( const CStringA inStrA ) : m_strW( inStrA ), m_strA( inStrA ) {}
	;			CStringX( const CStringW inStrW ) : m_strW( inStrW ), m_strA( inStr ) {}
	;			CStringX( PCSTR inSz ) : m_strA( inSz ), m_strW( inSz ) {}
	;			~CStringX() {}

	void		Empty() { m_strA.Empty(); m_strW.Empty(); }
	size_t		GetLength() const { return m_strA.GetLength(); }
	bool		IsEmpty() const { return m_strA.IsEmpty(); }
	CStringA	GetA() const { return m_strA; }
	CString		GetW() const { return m_strW; }

	CStringX	operator=( CStringA inStrA ) { m_strW = m_strA = inStrA; return *this; }
	CStringX	operator=( CString inStrW ) { m_strA = m_strW = inStrW; return *this; }

	bool		operator==( const CStringX& inStrX ) const { return (m_strA.CompareNoCase( inStrX.m_strA ) == 0); }
	bool		operator!=( const CStringX& inStrX ) const { return (m_strA.CompareNoCase( inStrX.m_strA ) != 0); }
	bool		operator==( CStringA inStrA ) const { return (m_strA.CompareNoCase( inStrA ) == 0); }
	bool		operator==( CStringW inStrW ) const { return (m_strW.CompareNoCase( inStrW ) == 0); }

	;			operator CStringA() const { return m_strA; }
	;			operator PCSTR() const { return static_cast<PCSTR>( m_strA ); }
	;			operator CStringW() const { return m_strW; }
	;			operator PCTSTR() const { return static_cast<PCTSTR>( m_strW ); }

	int			CompareNoCase( const CStringA in ) const { return m_strA.CompareNoCase( in ); }
	int			CompareNoCase( const CStringW in ) const { return m_strW.CompareNoCase( in ); }
};
