/*
CT2UTF8

A copy-paste-mutate bugfix of Microsoft's ATL CW2AEX<> (aka CT2A) that actually works for UTF-8.


CW2AEX is a part of the Active Template Library.
Copyright (C) 1996-2001 Microsoft Corporation
All rights reserved.

Bugfix copyright (c) 2002 WildPackets, Inc. All rights reserved.
*/
#pragma once

template< int t_nBufferLength = 128 >
class CW2AEX_WP
{
public:
	CW2AEX_WP( LPCWSTR psz ) throw(...) :
		m_psz( m_szBuffer )
	{
		Init( psz, CP_UTF8 );
	}
//	CW2AEX_WP( LPCWSTR psz, UINT nCodePage ) throw(...) :
//		m_psz( m_szBuffer )
//	{
//		Init( psz, nCodePage );
//	}
	~CW2AEX_WP() throw()
	{
		if( m_psz != m_szBuffer )
		{
			free( m_psz );
		}
	}

	operator LPSTR() const throw()
	{
		return( m_psz );
	}

private:
	void Init( LPCWSTR psz, UINT nCodePage ) throw(...)
	{
		if (psz == NULL)
		{
			m_psz = NULL;
			return;
		}
		int nLengthW = lstrlenW( psz )+1;
		int nLengthA = nLengthW*3;	// bugfix: allocate 3 bytes for UTF-8 expansion of each unicode char, original ATL code allocates only 2, which doesn't work for Japanese/Kanji

		if( nLengthA > t_nBufferLength )
		{
			m_psz = static_cast< LPSTR >( malloc( nLengthA*sizeof( char ) ) );
			if (m_psz == NULL)
			{
				AtlThrow( E_OUTOFMEMORY );
			}
		}

		::WideCharToMultiByte( nCodePage, 0, psz, nLengthW, m_psz, nLengthA, NULL, NULL );
	}

public:
	LPSTR m_psz;
	char m_szBuffer[t_nBufferLength];

private:
	CW2AEX_WP( const CW2AEX_WP& ) throw();
	CW2AEX_WP& operator=( const CW2AEX_WP& ) throw();
};
typedef CW2AEX_WP<> CW2UTF8;

#ifdef _UNICODE
	#define CT2UTF8 CW2UTF8
	#define CUTF82T	CA2W
#else
	#define CT2UTF8 CA2A
	#define CUTF82T	CA2A
#endif // _UNICODE
