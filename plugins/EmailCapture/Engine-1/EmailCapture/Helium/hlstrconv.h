// =============================================================================
//	hlstrconv.h
// =============================================================================
//	Copyright (c) 2005-2009 WildPackets, Inc. All rights reserved.

#ifndef HLSTRCONV_H
#define HLSTRCONV_H

#include "hlexcept.h"
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <string>

namespace HeLib
{

template< std::size_t t_cchBuffer = 256 >
class CA2CAEX
{
public:
	CA2CAEX( const char* psz ) :
		m_psz( psz ),
		m_cch( static_cast<std::size_t>(-1) )
	{
	}

	CA2CAEX( const std::string& str ) :
		m_psz( str.c_str() ),
		m_cch( str.length() )
	{
	}

	~CA2CAEX() throw()
	{
	}

	operator const char*() const throw()
	{
		return m_psz;
	}

	std::size_t size() const throw()
	{
		if ( m_cch == static_cast<std::size_t>(-1) )
		{
			m_cch = (m_psz == NULL) ? 0 : std::strlen( m_psz );
		}
		return m_cch;
	}

public:
	const char* m_psz;
	std::size_t	m_cch;

private:
	CA2CAEX( const CA2CAEX& ) throw();
	CA2CAEX& operator=( const CA2CAEX& ) throw();
};
typedef CA2CAEX<> CA2CA;


template< std::size_t t_cchBuffer = 256 >
class CA2AEX
{
public:
	CA2AEX( const char* psz )
	{
		Init( psz, static_cast<std::size_t>(-1) );
	}

	CA2AEX( const char* psz, const std::size_t cch )
	{
		Init( psz, cch );
	}

	CA2AEX( const std::string& str )
	{
		Init( str.c_str(), str.length() );
	}

	~CA2AEX() throw()
	{
		if ( (m_psz != m_szBuffer) && (m_psz != NULL) )
		{
			std::free( m_psz );
		}
	}

	operator char*() const throw()
	{
		return m_psz;
	}

	std::size_t size() const throw()
	{
		return m_cch;
	}

private:
	void Init( const char* psz, const std::size_t cch )
	{
		if ( psz == NULL )
		{
			m_psz = NULL;
			m_cch = 0;
		}
		else
		{
			m_psz = m_szBuffer;
			m_cch = (cch == static_cast<std::size_t>(-1)) ? std::strlen( psz ) : cch;

			if ( m_cch > 0 )
			{
				const std::size_t	cchA = m_cch + 1;
				if ( cchA > t_cchBuffer )
				{
					m_psz = static_cast<char*>(std::malloc( cchA * sizeof(char) ));
					if ( m_psz == NULL ) HeThrow( HE_E_OUT_OF_MEMORY );
				}

				std::memcpy( m_psz, psz, m_cch * sizeof(char) );
			}

			m_psz[m_cch] = 0;
		}
	}

public:
	char*			m_psz;
	char			m_szBuffer[t_cchBuffer];
	std::size_t		m_cch;

private:
	CA2AEX( const CA2AEX& ) throw();
	CA2AEX& operator=( const CA2AEX& ) throw();
};
typedef CA2AEX<> CA2A;


template< std::size_t t_cchBuffer = 256 >
class CA2WEX
{
public:
	CA2WEX( const char* psz )
	{
		Init( psz, static_cast<std::size_t>(-1) );
	}

	CA2WEX( const char* psz, const std::size_t cch )
	{
		Init( psz, cch );
	}

	CA2WEX( const std::string& str )
	{
		Init( str.c_str(), str.length() );
	}

	~CA2WEX() throw()
	{
		if ( (m_psz != m_szBuffer) && (m_psz != NULL) )
		{
			std::free( m_psz );
		}
	}

	operator wchar_t*() const throw()
	{
		return m_psz;
	}

	std::size_t size() const throw()
	{
		return m_cch;
	}

private:
	void Init( const char* psz, const std::size_t cch )
	{
		if ( psz == NULL )
		{
			m_psz = NULL;
			m_cch = 0;
		}
		else
		{
			m_psz = m_szBuffer;
			m_cch = 0;

			const std::size_t	cchA = (cch == static_cast<std::size_t>(-1)) ? 
													std::strlen( psz ) : cch;
			if ( cchA > 0 )
			{
				const std::size_t	cchW = cchA + 1;
				if ( cchW > t_cchBuffer )
				{
					m_psz = static_cast<wchar_t*>(std::malloc( cchW * sizeof(wchar_t) ));
					if ( m_psz == NULL ) HeThrow( HE_E_OUT_OF_MEMORY );
				}

#ifdef _WIN32
				const int	cchC = ::MultiByteToWideChar( CP_THREAD_ACP, 0, 
										psz, static_cast<int>(cchA), 
										m_psz, static_cast<int>(cchA) );
				if ( (cchC == 0) && (cchA > 0) )
				{
					HeThrow( HE_E_FAIL );
				}
				m_cch = static_cast<std::size_t>(cchC);
#else
				const std::size_t	cchC = std::mbstowcs( m_psz, psz, cchA );
				if ( cchC == static_cast<std::size_t>(-1) )
				{
					HeThrow( HE_E_FAIL );
				}
				m_cch = cchC;
#endif /* _WIN32 */
			}

			m_psz[m_cch] = 0;
		}
	}

public:
	wchar_t*		m_psz;
	wchar_t			m_szBuffer[t_cchBuffer];
	std::size_t		m_cch;

private:
	CA2WEX( const CA2WEX& ) throw();
	CA2WEX& operator=( const CA2WEX& ) throw();
};
typedef CA2WEX<> CA2W;


template< std::size_t t_cchBuffer = 256 >
class CW2CWEX
{
public:
	CW2CWEX( const wchar_t* psz ) :
		m_psz( psz ),
		m_cch( static_cast<std::size_t>(-1) )
	{
	}

	CW2CWEX( const std::wstring& str ) :
		m_psz( str.c_str() ),
		m_cch( str.length() )
	{
	}

	~CW2CWEX() throw()
	{
	}

	operator const wchar_t*() const throw()
	{
		return m_psz;
	}

	std::size_t size() const throw()
	{
		if ( m_cch == static_cast<std::size_t>(-1) )
		{
			m_cch = (m_psz == NULL) ? 0 : std::wcslen( m_psz );
		}
		return m_cch;
	}

public:
	const wchar_t*	m_psz;
	std::size_t		m_cch;

private:
	CW2CWEX( const CW2CWEX& ) throw();
	CW2CWEX& operator=( const CW2CWEX& ) throw();
};
typedef CW2CWEX<> CW2CW;


template< std::size_t t_cchBuffer = 256 >
class CW2WEX
{
public:
	CW2WEX( const wchar_t* psz )
	{
		Init( psz, static_cast<std::size_t>(-1) );
	}

	CW2WEX( const wchar_t* psz, const std::size_t cch )
	{
		Init( psz, cch );
	}

	CW2WEX( const std::wstring& str )
	{
		Init( str.c_str(), str.length() );
	}

	~CW2WEX() throw()
	{
		if ( (m_psz != m_szBuffer) && (m_psz != NULL) )
		{
			std::free( m_psz );
		}
	}

	operator wchar_t*() const throw()
	{
		return m_psz;
	}

	std::size_t size() const throw()
	{
		return m_cch;
	}

private:
	void Init( const wchar_t* psz, const std::size_t cch )
	{
		if ( psz == NULL )
		{
			m_psz = NULL;
			m_cch = 0;
		}
		else
		{
			m_psz = m_szBuffer;
			m_cch = (cch == static_cast<std::size_t>(-1)) ? std::wcslen( psz ) : cch;

			if ( m_cch > 0 )
			{
				const std::size_t	cchW = m_cch + 1;
				if ( cchW > t_cchBuffer )
				{
					m_psz = static_cast<wchar_t*>(std::malloc( cchW * sizeof(wchar_t) ));
					if ( m_psz == NULL ) HeThrow( HE_E_OUT_OF_MEMORY );
				}

				std::memcpy( m_psz, psz, m_cch * sizeof(wchar_t) );
			}

			m_psz[m_cch] = 0;
		}
	}

public:
	wchar_t*		m_psz;
	wchar_t			m_szBuffer[t_cchBuffer];
	std::size_t		m_cch;

private:
	CW2WEX( const CW2WEX& ) throw();
	CW2WEX& operator=( const CW2WEX& ) throw();
};
typedef CW2WEX<> CW2W;


template< std::size_t t_cchBuffer = 256 >
class CW2AEX
{
public:
	CW2AEX( const wchar_t* psz )
	{
		Init( psz, static_cast<std::size_t>(-1) );
	}

	CW2AEX( const wchar_t* psz, const std::size_t cch )
	{
		Init( psz, cch );
	}

	CW2AEX( const std::wstring& str )
	{
		Init( str.c_str(), str.length() );
	}

	~CW2AEX() throw()
	{
		if ( (m_psz != m_szBuffer) && (m_psz != NULL) )
		{
			std::free( m_psz );
		}
	}

	operator char*() const throw()
	{
		return m_psz;
	}

	std::size_t size() const throw()
	{
		return m_cch;
	}

private:
	void Init( const wchar_t* psz, const std::size_t cch )
	{
		if ( psz == NULL )
		{
			m_psz = NULL;
			m_cch = 0;
		}
		else
		{
			m_psz = m_szBuffer;
			m_cch = 0;

			const std::size_t	cchW = (cch == static_cast<std::size_t>(-1)) ? 
													std::wcslen( psz ) : cch;
			if ( cchW > 0 )
			{
				const std::size_t	cchA = (cchW + 1) * 4;
				const std::size_t	cbA  = cchA * sizeof(char);
				if ( cchA > t_cchBuffer )
				{
					m_psz = static_cast<char*>(std::malloc( cbA ));
					if ( m_psz == NULL ) HeThrow( HE_E_OUT_OF_MEMORY );
				}

#ifdef _WIN32
				const int	cb = ::WideCharToMultiByte( CP_THREAD_ACP, 0, 
									psz, static_cast<int>(cchW), 
									m_psz, static_cast<int>(cbA), NULL, NULL );
				if ( (cb == 0) && (cchW > 0) )
				{
					HeThrow( HE_E_FAIL );
				}
				m_cch = static_cast<std::size_t>(cb);
#else
				const std::size_t	cb = std::wcstombs( m_psz, psz, cbA );
				if ( cb == static_cast<std::size_t>(-1) )
				{
					HeThrow( HE_E_FAIL );
				}
				m_cch = cb;
#endif /* _WIN32 */
			}

			m_psz[m_cch] = 0;
		}
	}

public:
	char*			m_psz;
	char			m_szBuffer[t_cchBuffer];
	std::size_t		m_cch;

private:
	CW2AEX( const CW2AEX& ) throw();
	CW2AEX& operator=( const CW2AEX& ) throw();
};
typedef CW2AEX<> CW2A;


template< std::size_t t_cchBuffer = 1024 >
class CW2UTF8EX
{
public:
	CW2UTF8EX( const wchar_t* psz )
	{
		Init( psz, static_cast<std::size_t>(-1) );
	}

	CW2UTF8EX( const wchar_t* psz, const std::size_t cch )
	{
		Init( psz, cch );
	}

	CW2UTF8EX( const std::wstring& str )
	{
		Init( str.c_str(), str.length() );
	}

	~CW2UTF8EX() throw()
	{
		if ( (m_psz != m_szBuffer) && (m_psz != NULL) )
		{
			std::free( m_psz );
		}
	}

	operator char*() const throw()
	{
		return m_psz;
	}

	std::size_t size() const throw()
	{
		return m_cch;
	}

private:
	void Init( const wchar_t* psz, const std::size_t cch )
	{
		if ( psz == NULL )
		{
			m_psz = NULL;
			m_cch = 0;
		}
		else
		{
			m_psz = m_szBuffer;
			m_cch = 0;

			const std::size_t	cchW = (cch == static_cast<std::size_t>(-1)) ? 
													std::wcslen( psz ) : cch;
			const std::size_t	cchA = (cchW + 1) * 4;
			const std::size_t	cbA  = cchA * sizeof(char);
			if ( cchA > t_cchBuffer )
			{
				m_psz = static_cast<char*>(std::malloc( cbA ));
				if ( m_psz == NULL ) HeThrow( HE_E_OUT_OF_MEMORY );
			}

			char*	utf8 = m_psz;
			for ( size_t i = 0; i < cchW; ++i )
			{
				std::size_t	n = 4;
				UTF32ToUTF8( psz[i], utf8, &n );
				utf8  += n;
				m_cch += n;
			}
			*utf8 = 0;
		}
	}

public:
	static void UTF32ToUTF8( UInt32 ch, char* pUTF8, std::size_t* nUTF8 )
	{
		static const UInt32 BYTE_MASK          = 0xBF;
		static const UInt32 BYTE_MARK          = 0x80;
		static const UInt32 FIRST_BYTE_MARK[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

		if ( ch < 0x80 )
		{
			*nUTF8 = 1;
		}
		else if ( ch < 0x800 )
		{
			*nUTF8 = 2;
		}
		else if ( ch < 0x10000 )
		{
			*nUTF8 = 3;
		}
		else if ( ch < 0x200000 )
		{
			*nUTF8 = 4;
		}
		else
		{
			*nUTF8 = 0;
			return;		// This code won't covert this correctly anyway.
		}

		pUTF8 += *nUTF8;

		// Fall throughs intentional.
		switch ( *nUTF8 ) 
		{
			case 4:
				--pUTF8; *pUTF8 = static_cast<char>((ch | BYTE_MARK) & BYTE_MASK); ch >>= 6;
			case 3:
				--pUTF8; *pUTF8 = static_cast<char>((ch | BYTE_MARK) & BYTE_MASK); ch >>= 6;
			case 2:
				--pUTF8; *pUTF8 = static_cast<char>((ch | BYTE_MARK) & BYTE_MASK); ch >>= 6;
			case 1:
				--pUTF8; *pUTF8 = static_cast<char>(ch | FIRST_BYTE_MARK[*nUTF8]);
		}
	}

public:
	char*			m_psz;
	char			m_szBuffer[t_cchBuffer];
	std::size_t		m_cch;

private:
	CW2UTF8EX( const CW2UTF8EX& ) throw();
	CW2UTF8EX& operator=( const CW2UTF8EX& ) throw();
};
typedef CW2UTF8EX<> CW2UTF8;


template< std::size_t t_cchBuffer = 256 >
class CUTF82WEX
{
public:
	CUTF82WEX( const char* psz )
	{
		Init( psz, static_cast<std::size_t>(-1) );
	}

	CUTF82WEX( const char* psz, const std::size_t cch )
	{
		Init( psz, cch );
	}

	CUTF82WEX( const std::string& str )
	{
		Init( str.c_str(), str.length() );
	}

	~CUTF82WEX() throw()
	{
		if ( (m_psz != m_szBuffer) && (m_psz != NULL) )
		{
			std::free( m_psz );
		}
	}

	operator wchar_t*() const throw()
	{
		return m_psz;
	}

	std::size_t size() const throw()
	{
		return m_cch;
	}

private:
	void Init( const char* psz, const std::size_t cch )
	{
		if ( psz == NULL )
		{
			m_psz = NULL;
			m_cch = 0;
		}
		else
		{
			m_psz = m_szBuffer;
			m_cch = 0;

			const std::size_t	cchA = (cch == static_cast<std::size_t>(-1)) ? std::strlen( psz ) : cch;
			const std::size_t	cchW = cchA + 1;
			const std::size_t	cbW  = cchW * sizeof(wchar_t);
			if ( cchW > t_cchBuffer )
			{
				m_psz = static_cast<wchar_t*>(std::malloc( cbW ));
				if ( m_psz == NULL ) HeThrow( HE_E_OUT_OF_MEMORY );
			}

			const char*	utf8s = psz;
			const char*	utf8e = psz + cchA;
			while ( utf8s < utf8e )
			{
				std::size_t	n  = static_cast<std::size_t>(utf8e - utf8s);
				UInt32		ch = 0;
				UTF8ToUTF32( utf8s, &n, &ch );
				utf8s += n;
				m_psz[m_cch++] = static_cast<wchar_t>(ch);
			}
			m_psz[m_cch] = 0;
		}
	}

public:
	static bool UTF8ToUTF32( const char* pUTF8, std::size_t* nUTF8, UInt32* pch )
	{
		if ( pch == NULL ) return false;
		*pch = 0;
		if ( pUTF8 == NULL ) return false;
		if ( *nUTF8 == 0 ) return false;

		static const unsigned char	utf8ByteTable[256] = 
		{
		//	0	1	2	3	4	5	6	7	8	9	a	b	c	d	e	f
			1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x00
			1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x10
			1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x20
			1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x30
			1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x40
			1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x50
			1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x60
			1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x70	End of ASCII range
			1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x80 0x80 to 0xc1 invalid
			1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x90 
			1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0xa0 
			1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0xb0 
			1,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	// 0xc0 0xc2 to 0xdf 2 byte
			2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	// 0xd0
			3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	// 0xe0 0xe0 to 0xef 3 byte
			4,	4,	4,	4,	4,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1	// 0xf0 0xf0 to 0xf4 4 byte, 0xf5 and higher invalid
		};

		const unsigned char*	p = reinterpret_cast<const unsigned char*>(pUTF8);
		const std::size_t		n = utf8ByteTable[static_cast<unsigned char>(*p)];
		if ( *nUTF8 < n ) return false;

		if ( !IsLegalUTF8( p, n ) ) return false;

		UInt32	ch = 0;
		switch ( n )
		{
			case 4:
				ch += *p; ch <<= 6; ++p;
			case 3:
				ch += *p; ch <<= 6; ++p;
			case 2:
				ch += *p; ch <<= 6; ++p;
			case 1:
				ch += *p;
		}

		static const UInt32 offsetsFromUTF8[6] =
		{
			0x00000000UL, 0x00003080UL, 0x000E2080UL,
			0x03C82080UL, 0xFA082080UL, 0x82082080UL
		};

		ch -= offsetsFromUTF8[n-1];

		*pch   = ch;
		*nUTF8 = n;

		return true;
	}

	static bool IsLegalUTF8( const unsigned char* pUTF8, std::size_t n )
	{
		if ( pUTF8 == NULL ) return false;
		const unsigned char*	p = pUTF8 + n;
		unsigned char			a;
		switch ( n )
		{
			default:
				return false;
			case 4:
				if ( (a = (*--p)) < 0x80 || (a > 0xBF) )
					return false;
				// Fall through OK...
			case 3:
				if ( (a = (*--p)) < 0x80 || (a > 0xBF) )
					return false;
				// Fall through OK...
			case 2:
				if ( (a = (*--p)) > 0xBF )
					return false;
				switch ( *pUTF8 )
				{
					// No fall-through in this inner switch.
					case 0xE0:
						if ( a < 0xA0 )
							return false;
						break;
					case 0xED:
						if ( a > 0x9F )
							return false;
						break;
					case 0xF0:
						if ( a < 0x90 )
							return false;
						break;
					case 0xF4:
						if ( a > 0x8F )
							return false;
						break;
					default:
						if ( a < 0x80 )
							return false;
						break;
				}
				// Fall through OK...
			case 1:
				if ( (*pUTF8 >= 0x80) && (*pUTF8 < 0xC2) )
					return false;
				// Fall through OK...
		}
		if ( *pUTF8 > 0xF4)
			return false;
		return true;
	}

public:
	wchar_t*		m_psz;
	wchar_t			m_szBuffer[t_cchBuffer];
	std::size_t		m_cch;

private:
	CUTF82WEX( const CUTF82WEX& ) throw();
	CUTF82WEX& operator=( const CUTF82WEX& ) throw();
};
typedef CUTF82WEX<> CUTF82W;


template< std::size_t t_cchBuffer = 256 >
class CW2UTF16EX
{
public:
	CW2UTF16EX( const wchar_t* psz )
	{
		Init( psz, static_cast<std::size_t>(-1) );
	}

	CW2UTF16EX( const wchar_t* psz, const std::size_t cch )
	{
		Init( psz, cch );
	}

	CW2UTF16EX( const std::wstring& str )
	{
		Init( str.c_str(), str.length() );
	}

	~CW2UTF16EX() throw()
	{
		if ( (m_psz != m_szBuffer) && (m_psz != NULL) )
		{
			std::free( m_psz );
		}
	}

	operator UInt16*() const throw()
	{
		return m_psz;
	}

	std::size_t size() const throw()
	{
		return m_cch;
	}

private:
	void Init( const wchar_t* psz, const std::size_t cch )
	{
		if ( psz == NULL )
		{
			m_psz = NULL;
			m_cch = 0;
		}
		else
		{
			m_psz = m_szBuffer;
			m_cch = 0;

			const std::size_t	cchW = (cch == static_cast<std::size_t>(-1)) ? std::wcslen( psz ) : cch;
			const std::size_t	cchU = (cchW + 1) * 2;
			const std::size_t	cbU  = cchU * sizeof(unsigned short);
			if ( cchU > t_cchBuffer )
			{
				m_psz = static_cast<UInt16*>(std::malloc( cbU ));
				if ( m_psz == NULL ) HeThrow( HE_E_OUT_OF_MEMORY );
			}

			static const UInt32 UNI_REPLACEMENT_CHAR = 0x0000fffd;
			static const UInt32 UNI_MAX_BMP          = 0x0000ffff;
			static const UInt32 UNI_MAX_LEGAL_UTF32  = 0x0010ffff;
			static const UInt32 UNI_SUR_HIGH_START   = 0xd800;
			static const UInt32 UNI_SUR_LOW_START    = 0xdc00;
			static const UInt32 UNI_SUR_LOW_END      = 0xdfff;
			static const UInt32 HALF_BASE            = 0x00010000;
			static const UInt32 HALF_MASK            = 0x03ff;

			UInt16*	utf16 = m_psz;
			for ( std::size_t i = 0; i < cchW; ++i )
			{
				UInt32	ch = static_cast<UInt32>(psz[i]);
				if ( ch <= UNI_MAX_BMP )
				{
					if ( (ch >= UNI_SUR_HIGH_START) && (ch <= UNI_SUR_LOW_END) )
					{
						// Illegal character!
						*utf16++ = UNI_REPLACEMENT_CHAR;	// Insert replacement char.
						m_cch++;
					}
					else
					{
						// Normal case.
						*utf16++ = static_cast<UInt16>(ch);
						m_cch++;
					}
				}
				else if ( ch >= UNI_MAX_LEGAL_UTF32 )
				{
					// Illegal UTF32 char!
					*utf16++ = UNI_REPLACEMENT_CHAR;	// Insert replacement char.
					m_cch++;
				}
				else
				{
					// In the range 0xffff - 0x10ffff.
					ch -= HALF_BASE;
					*utf16++ = static_cast<UInt16>((ch >> 10) + UNI_SUR_HIGH_START);
					*utf16++ = static_cast<UInt16>((ch & HALF_MASK) + UNI_SUR_LOW_START);
					m_cch += 2;
				}
			}
			*utf16 = 0;
		}
	}

public:
	UInt16*			m_psz;
	UInt16			m_szBuffer[t_cchBuffer];
	std::size_t		m_cch;

private:
	CW2UTF16EX( const CW2UTF16EX& ) throw();
	CW2UTF16EX& operator=( const CW2UTF16EX& ) throw();
};
typedef CW2UTF16EX<> CW2UTF16;


template< std::size_t t_cchBuffer = 256 >
class CUTF162WEX
{
public:
	CUTF162WEX( const UInt16* psz )
	{
		Init( psz, static_cast<std::size_t>(-1) );
	}

	CUTF162WEX( const UInt16* psz, const std::size_t cch )
	{
		Init( psz, cch );
	}

	~CUTF162WEX() throw()
	{
		if ( (m_psz != m_szBuffer) && (m_psz != NULL) )
		{
			std::free( m_psz );
		}
	}

	operator wchar_t*() const throw()
	{
		return m_psz;
	}

	std::size_t size() const throw()
	{
		return m_cch;
	}

private:
	void Init( const UInt16* psz, const std::size_t cch )
	{
		if ( psz == NULL )
		{
			m_psz = NULL;
			m_cch = 0;
		}
		else
		{
			m_psz = m_szBuffer;
			m_cch = 0;

			std::size_t	cchU;
			if ( cch == static_cast<std::size_t>(-1) )
			{
				// Determine the string length.
				const UInt16*		p = psz;
				while ( *p++ ) {};
				cchU = static_cast<std::size_t>(p - psz - 1);
			}
			else
			{
				cchU = cch;
			}

			const std::size_t	cchW = cchU + 1;
			const std::size_t	cbW  = cchW * sizeof(wchar_t);
			if ( cchW > t_cchBuffer )
			{
				m_psz = static_cast<wchar_t*>(std::malloc( cbW ));
				if ( m_psz == NULL ) HeThrow( HE_E_OUT_OF_MEMORY );
			}

			static const UInt32 UNI_SUR_HIGH_START   = 0xd800;
			static const UInt32 UNI_SUR_HIGH_END     = 0xdbff;
			static const UInt32 UNI_SUR_LOW_START    = 0xdc00;
			static const UInt32 UNI_SUR_LOW_END      = 0xdfff;
			static const UInt32 HALF_BASE            = 0x00010000;

			const UInt16*	utf16s = psz;
			const UInt16*	utf16e = psz + cchU;
			wchar_t*		pszDest = m_psz;
			while ( utf16s < utf16e )
			{
				UInt32	ch = *utf16s++;

				if ( (ch >= UNI_SUR_HIGH_START) && (ch <= UNI_SUR_HIGH_END) )
				{
					if ( utf16s < utf16e )
					{
						const UInt32	ch2 = *utf16s;
						if ( (ch2 >= UNI_SUR_LOW_START) && (ch2 <= UNI_SUR_LOW_END) )
						{
							ch = ((ch - UNI_SUR_HIGH_START) << 10) + (ch2 - UNI_SUR_LOW_START) + HALF_BASE;
							++utf16s;
						}
						else
						{
							// Error in conversion.
							HeThrow( HE_E_FAIL );
						}
					}
					else
					{
						// Missing the 16 bits following the high surrogate.
						HeThrow( HE_E_FAIL );
					}
				}
				else
				{
					if ( (ch >= UNI_SUR_LOW_START) && (ch <= UNI_SUR_LOW_END) )
					{
						HeThrow( HE_E_FAIL );
					}
				}

				*pszDest++ = static_cast<wchar_t>(ch);
			}
			*pszDest = 0;
			m_cch = pszDest - m_psz;
		}
	}

public:
	wchar_t*		m_psz;
	wchar_t			m_szBuffer[t_cchBuffer];
	std::size_t		m_cch;

private:
	CUTF162WEX( const CUTF162WEX& ) throw();
	CUTF162WEX& operator=( const CUTF162WEX& ) throw();
};
typedef CUTF162WEX<> CUTF162W;


#if !defined(HE_NO_STRING_CONVERSION_MACROS) && !defined(CW2T)

#if defined(_UNICODE) || defined(UNICODE)

	#define CW2T	HeLib::CW2W
	#define CW2TEX	HeLib::CW2WEX
	#define CW2CT	HeLib::CW2CW
	#define CW2CTEX	HeLib::CW2CWEX
	#define CT2W	HeLib::CW2W
	#define CT2WEX	HeLib::CW2WEX
	#define CT2CW	HeLib::CW2CW
	#define CT2CWEX	HeLib::CW2CWEX

	#define CA2T	HeLib::CA2W
	#define CA2TEX	HeLib::CA2WEX
	#define CA2CT	HeLib::CA2W
	#define CA2CTEX	HeLib::CA2WEX
	#define CT2A	HeLib::CW2A
	#define CT2AEX	HeLib::CW2AEX
	#define CT2CA	HeLib::CW2A
	#define CT2CAEX	HeLib::CW2AEX

	#define CT2UTF8	HeLib::CW2UTF8
	#define CUTF82T	HeLib::CUTF82W

#else

	#define CW2T	HeLib::CW2A
	#define CW2TEX	HeLib::CW2AEX
	#define CW2CT	HeLib::CW2A
	#define CW2CTEX	HeLib::CW2AEX
	#define CT2W	HeLib::CA2W
	#define CT2WEX	HeLib::CA2WEX
	#define CT2CW	HeLib::CA2W
	#define CT2CWEX	HeLib::CA2WEX

	#define CA2T	HeLib::CA2A
	#define CA2TEX	HeLib::CA2AEX
	#define CA2CT	HeLib::CA2CA
	#define CA2CTEX	HeLib::CA2CAEX
	#define CT2A	HeLib::CA2A
	#define CT2AEX	HeLib::CA2AEX
	#define CT2CA	HeLib::CA2CA
	#define CT2CAEX	HeLib::CA2CAEX

	#define CT2UTF8	HeLib::CA2CA
	#define CUTF82T	HeLib::CA2CA

#endif /* defined(_UNICODE) || defined(UNICODE) */

#define COLE2T CW2T
#define COLE2TEX CW2TEX
#define COLE2CT CW2CT
#define COLE2CTEX CW2CTEX
#define CT2OLE CT2W
#define CT2OLEEX CT2WEX
#define CT2COLE CT2CW
#define CT2COLEEX CT2CWEX

#endif /* !defined(HE_NO_STRING_CONVERSION_MACROS) && !defined(CW2T) */

} /* namespace HeLib */

#endif /* HLSTRCONV_H */
