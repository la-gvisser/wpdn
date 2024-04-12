// =============================================================================
//	hlstr.h
// =============================================================================
//	Copyright (c) 2005-2008 WildPackets, Inc. All rights reserved.

#ifndef HLSTR_H
#define HLSTR_H

#include "hecom.h"
#include "hestr.h"
#include "hevariant.h"
#include "hlexcept.h"
#include "hlruntime.h"
#include <cstring>
#include <cwchar>
#include <string>

namespace HeLib
{

inline std::wstring
HeStringFromHEBSTR(
	 Helium::HEBSTR	bstr )
{
	if ( bstr == NULL ) return std::wstring();
	return std::wstring( bstr, HeStringLen( bstr ) );
}

inline HeResult
HeHEBSTRFromString(
	const std::wstring&	str,
	Helium::HEBSTR*		pbstr ) throw()
{
	if ( pbstr == NULL ) return HE_E_NULL_POINTER;
	*pbstr = HeAllocStringLen( str.c_str(), str.size() );
	if ( *pbstr == NULL ) return HE_E_OUT_OF_MEMORY;
	return HE_S_OK;
}

inline HeResult
HeHEBSTRFromString(
	const std::string&	str,
	Helium::HEBSTR*		pbstr ) throw()
{
	if ( pbstr == NULL ) return HE_E_NULL_POINTER;
	HeLib::CA2W	wstr( str.c_str(), str.size() );
	*pbstr = HeAllocStringLen( wstr, wstr.size() );
	if ( *pbstr == NULL ) return HE_E_OUT_OF_MEMORY;
	return HE_S_OK;
}

inline HeResult
HeHEBSTRToString(
	 Helium::HEBSTR	bstr,
	 std::wstring&	str ) throw()
{
	HeResult	hr = HE_S_OK;

	try
	{
		if ( bstr != NULL )
		{
			str.assign( bstr, HeStringLen( bstr ) );
		}
		else
		{
			str.clear();
		}
	}
	HE_CATCH(hr)

	return hr;
}

inline HeResult
HeHEBSTRToString(
	 Helium::HEBSTR	bstr,
	 std::string&	str ) throw()
{
	HeResult	hr = HE_S_OK;

	try
	{
		if ( bstr != NULL )
		{
			HeLib::CW2A	astr( bstr, HeStringLen( bstr ) );
			str = astr;
		}
		else
		{
			str.clear();
		}
	}
	HE_CATCH(hr)

	return hr;
}

class CHeBSTR
{
public:
	Helium::HEBSTR m_str;

	CHeBSTR() throw() : m_str( NULL )
	{
	}

	CHeBSTR( std::size_t cch ) : m_str( NULL )
	{
		if ( cch > 0 )
		{
			m_str = HeAllocStringLen( NULL, cch );
			if ( m_str == NULL )
			{
				HeThrow( HE_E_OUT_OF_MEMORY );
			}
		}
	}

	CHeBSTR( std::size_t cch, const wchar_t* sz ) : m_str( NULL )
	{
		if ( cch > 0 )
		{
			m_str = HeAllocStringLen( sz, cch );
			if ( m_str == NULL )
			{
				HeThrow( HE_E_OUT_OF_MEMORY );
			}
		}
	}

	CHeBSTR( const wchar_t* pSrc ) : m_str( NULL )
	{
		if ( pSrc != NULL )
		{
			m_str = HeAllocString( pSrc );
			if ( m_str == NULL )
			{
				HeThrow( HE_E_OUT_OF_MEMORY );
			}
		}
	}

	CHeBSTR( const char* psz ) : m_str( NULL )
	{
		if ( psz != NULL )
		{
			HeLib::CA2W	s( psz );
			m_str = HeAllocStringLen( s, s.size() );
			if ( m_str == NULL ) HeThrow( HE_E_OUT_OF_MEMORY );
		}
	}

	CHeBSTR( std::size_t cch, const char* pch ) : m_str( NULL )
	{
		if ( (pch == NULL) && (cch != 0) )
		{
			m_str = HeAllocStringLen( NULL, cch );
			if ( m_str == NULL ) HeThrow( HE_E_OUT_OF_MEMORY );
		}
		else if ( pch != NULL )
		{
			m_str = HeAllocStringLen( HeLib::CA2W( pch ), cch );
			if ( (m_str == NULL) && (cch != 0) )
			{
				HeThrow( HE_E_OUT_OF_MEMORY );
			}
		}
	}

	CHeBSTR( const std::wstring& str ) : m_str( NULL )
	{
		HeThrowIfFailed( HeHEBSTRFromString( str, &m_str ) );
	}

	CHeBSTR( const std::string& str ) : m_str( NULL )
	{
		HeThrowIfFailed( HeHEBSTRFromString( str, &m_str ) );
	}

	CHeBSTR( const CHeBSTR& src )
	{
		m_str = src.Copy();
		if ( !!src && (m_str == NULL) )
		{
			HeThrow( HE_E_OUT_OF_MEMORY );
		}
	}
	
	CHeBSTR( const Helium::HeID& id )
	{
		wchar_t	szID[64];
		if ( !id.ToString( szID, 64 ) )
		{
			HeThrow( HE_E_FAIL );
		}
		m_str = HeAllocString( szID );
		if ( m_str == NULL )
		{
			HeThrow( HE_E_OUT_OF_MEMORY );
		}
	}

	CHeBSTR& operator=( const CHeBSTR& src )
	{
		if ( m_str != src.m_str )
		{
			HeFreeString( m_str );
			m_str = src.Copy();
			if ( (src.m_str != NULL) && (m_str == NULL) )
			{
				HeThrow( HE_E_OUT_OF_MEMORY );
			}
		}
		return *this;
	}

	CHeBSTR& operator=( const wchar_t* pSrc )
	{
		if ( pSrc != m_str )
		{
			HeFreeString( m_str );
			if ( pSrc != NULL )
			{
				m_str = HeAllocString( pSrc );
				if ( m_str == NULL )
				{
					HeThrow( HE_E_OUT_OF_MEMORY );
				}
			}
			else
			{
				m_str = NULL;
			}
		}
		return *this;
	}

	CHeBSTR& operator=( const char* psz )
	{
		HeFreeString( m_str );
		HeLib::CA2W	s( psz );
		m_str = HeAllocStringLen( s, s.size() );
		if ( (m_str == NULL) && (psz != NULL) )
		{
			HeThrow( HE_E_OUT_OF_MEMORY );
		}
		return *this;
	}

	CHeBSTR& operator=( const std::wstring& str )
	{
		HeFreeString( m_str );
		if ( !str.empty() )
		{
			m_str = HeAllocStringLen( str.c_str(), str.size() );
			if ( m_str == NULL )
			{
				HeThrow( HE_E_OUT_OF_MEMORY );
			}
		}
		else
		{
			m_str = NULL;
		}
		return *this;
	}

	CHeBSTR& operator=( const std::string& str )
	{
		return operator=( str.c_str() );
	}

	~CHeBSTR() throw()
	{
		HeFreeString( m_str );
	}

	std::size_t Length() const throw()
	{
		return (m_str == NULL) ? 0 : HeStringLen( m_str );
	}

	std::size_t ByteLength() const throw()
	{
		return (m_str == NULL) ? 0 : HeStringByteLen( m_str );
	}

	operator Helium::HEBSTR() const throw()
	{
		return m_str;
	}

	Helium::HEBSTR* get_address_of() throw()
	{
		return &m_str;
	}

	// No! Never overload operator&, but you may use get_address_of() above or
	// access m_str directly.
//	Helium::HEBSTR* operator&() throw()
//	{
//		return &m_str;
//	}

	Helium::HEBSTR Copy() const throw()
	{
		if ( m_str == NULL ) return NULL;
		return HeAllocStringByteLen( reinterpret_cast<char*>(m_str), HeStringByteLen( m_str ) );
	}

	HeResult CopyTo( Helium::HEBSTR* pbstr ) throw()
	{
		if ( pbstr == NULL ) return HE_E_NULL_POINTER;
		*pbstr = Copy();
		if ( (*pbstr == NULL) && (m_str != NULL) )
		{
			return HE_E_OUT_OF_MEMORY;
		}
		return HE_S_OK;
	}

	HeResult CopyTo( Helium::HEVARIANT* pVarDest ) throw()
	{
		if ( pVarDest == NULL ) return HE_E_NULL_POINTER;

		pVarDest->vt      = Helium::HE_VT_BSTR;
		pVarDest->bstrVal = Copy();
		if ( (pVarDest->bstrVal == NULL) && (m_str != NULL) )
		{
			return HE_E_OUT_OF_MEMORY;
		}

		return HE_S_OK;
	}

	void Attach( Helium::HEBSTR src ) throw()
	{
		if ( m_str != src )
		{
			HeFreeString( m_str );
			m_str = src;
		}
	}

	Helium::HEBSTR Detach() throw()
	{
		Helium::HEBSTR	s = m_str;
		m_str = NULL;
		return s;
	}

	void Empty() throw()
	{
		HeFreeString( m_str );
		m_str = NULL;
	}

	bool operator!() const throw()
	{
		return (m_str == NULL);
	}

	// A HEBSTR is just a HECHAR* so we need a special version to signify
	// that we are appending a HEBSTR.
	HeResult AppendBSTR( Helium::HEBSTR p ) throw()
	{
		std::size_t		cch2 = HeStringLen( p );
		if ( cch2 == 0 ) return HE_S_OK;

		std::size_t		cch1 = this->Length();

		Helium::HEBSTR	bstrNew = HeAllocStringLen( NULL, cch1 + cch2 );
		if ( bstrNew == NULL ) return HE_E_OUT_OF_MEMORY;

		if ( m_str != NULL )
		{
			std::memcpy( bstrNew, m_str, cch1 * sizeof(Helium::HECHAR) );
		}
		std::memcpy( bstrNew + cch1, p, cch2 * sizeof(Helium::HECHAR) );
		bstrNew[cch1 + cch2] = 0;

		HeFreeString( m_str );
		m_str = bstrNew;
		return HE_S_OK;
	}

	HeResult AppendBytes( const char* psz, std::size_t cb ) throw()
	{
		if ( (psz == NULL) || (cb == 0) )
		{
			return HE_S_OK;
		}

		const std::size_t	n1 = ByteLength();
		Helium::HEBSTR		b  = HeAllocStringByteLen( NULL, n1 + cb );
		if ( b == NULL )
		{
			return HE_E_OUT_OF_MEMORY;
		}
		std::memcpy( b, m_str, n1 );
		std::memcpy( reinterpret_cast<char*>(b) + n1, psz, cb );
		*((wchar_t*)(reinterpret_cast<char*>(b)+n1+cb)) = 0;
		HeFreeString( m_str );
		m_str = b;
		return HE_S_OK;
	}

	HeResult Append( const CHeBSTR& bstrSrc ) throw()
	{
		return AppendBSTR( bstrSrc.m_str );
	}

	HeResult Append( const wchar_t* psz, std::size_t cch ) throw()
	{
		if ( (psz == NULL) || ((m_str != NULL) && (cch == 0)) )
		{
			return HE_S_OK;
		}

		const std::size_t	n1 = Length();
		Helium::HEBSTR		b  = HeAllocStringLen( NULL, n1 + cch );
		if ( b == NULL )
		{
			return HE_E_OUT_OF_MEMORY;
		}
		if ( m_str != NULL )
		{
			std::memcpy( b, m_str, n1 * sizeof(wchar_t) );
		}
		std::memcpy( b + n1, psz, cch * sizeof(wchar_t) );
		b[n1 + cch] = 0;
		HeFreeString( m_str );
		m_str = b;
		return HE_S_OK;
	}

	HeResult Append( wchar_t ch ) throw()
	{
		return Append( &ch, 1 );
	}

	HeResult Append( const wchar_t* psz ) throw()
	{
		return Append( psz, std::wcslen( psz ) );
	}

	HeResult Append( const std::wstring& str ) throw()
	{
		return Append( str.c_str(), str.size() );
	}

	HeResult Append( char ch ) throw()
	{
		wchar_t	chO = ch;
		return Append( &chO, 1 );
	}

	HeResult Append( const char* psz ) throw()
	{
		if ( psz == NULL ) return HE_S_OK;

		CHeBSTR	bstrTemp;
		try { bstrTemp = psz; } catch ( ... ) {}
		if ( bstrTemp.m_str == NULL )
		{
			return HE_E_OUT_OF_MEMORY;
		}
		return Append( bstrTemp );
	}

	HeResult Append( const std::string& str ) throw()
	{
		return Append( str.c_str() );
	}

	HeResult AssignBSTR( const Helium::HEBSTR bstrSrc ) throw()
	{
		HeResult	hr = HE_S_OK;
		if ( m_str != bstrSrc )
		{
			HeFreeString( m_str );
			if ( bstrSrc != NULL )
			{
				m_str = HeAllocStringByteLen(
					reinterpret_cast<char*>(bstrSrc), HeStringByteLen( bstrSrc ) );
				if ( m_str == NULL )
				{
					hr = HE_E_OUT_OF_MEMORY;
				}
			}
			else
			{
				m_str = NULL;
			}
		}

		return hr;
	}

	CHeBSTR& operator+=( const CHeBSTR& bstrSrc )
	{
		HeThrowIfFailed( AppendBSTR( bstrSrc.m_str ) );
		return *this;
	}

	CHeBSTR& operator+=( const wchar_t* pszSrc )
	{
		HeThrowIfFailed( Append( pszSrc ) );
		return *this;
	}

	bool operator<(const CHeBSTR& bstrSrc) const throw()
	{
		return HeStringCmp( m_str, bstrSrc.m_str, 0 ) == static_cast<HeResult>(HE_VARCMP_LT);
	}

	bool operator<( const wchar_t* pszSrc ) const
	{
		return operator<( CHeBSTR( pszSrc ) );
	}

	bool operator<( const char* pszSrc ) const
	{
		return operator<( CHeBSTR( pszSrc ) );
	}

	bool operator<( const std::wstring& str ) const
	{
		return operator<( str.c_str() );
	}

	bool operator<( const std::string& str ) const
	{
		return operator<( str.c_str() );
	}

	bool operator>( const CHeBSTR& bstrSrc ) const throw()
	{
		return HeStringCmp( m_str, bstrSrc.m_str, 0 ) == static_cast<HeResult>(HE_VARCMP_GT);
	}

	bool operator>( const wchar_t* pszSrc ) const
	{
		return operator>( CHeBSTR( pszSrc ) );
	}
	
	bool operator>( const char* pszSrc ) const
	{
		return operator>( CHeBSTR( pszSrc ) );
	}

	bool operator>( const std::wstring& str ) const
	{
		return operator>( str.c_str() );
	}

	bool operator>( const std::string& str ) const
	{
		return operator>( str.c_str() );
	}

	bool operator==( const CHeBSTR& bstrSrc ) const throw()
	{
		return HeStringCmp( m_str, bstrSrc.m_str, 0 ) == static_cast<HeResult>(HE_VARCMP_EQ);
	}

	bool operator==( const wchar_t* pszSrc ) const
	{
		return operator==( CHeBSTR( pszSrc ) );
	}

	bool operator==( wchar_t* pszSrc ) const
	{
		return operator==( (const wchar_t*) pszSrc );
	}

	bool operator==( const char* pszSrc ) const
	{
		return operator==( CHeBSTR( pszSrc ) );
	}

	bool operator==( const std::wstring& str ) const
	{
		return operator==( str.c_str() );
	}

	bool operator==( const std::string& str ) const
	{
		return operator==( str.c_str() );
	}

	bool operator==( int nNull ) const throw()
	{
		HE_ASSERT( nNull == 0 );
		(void) nNull;
		return (m_str == NULL);
	}

	bool operator!=( const CHeBSTR& bstrSrc ) const throw()
	{
		return !operator==( bstrSrc );
	}

	bool operator!=( const wchar_t* pszSrc ) const
	{
		return !operator==( pszSrc );
	}

	bool operator!=( wchar_t* pszSrc ) const
	{
		return operator!=( (const wchar_t*) pszSrc );
	}

	bool operator!=( const char* pszSrc ) const
	{
		return !operator==( pszSrc );
	}

	bool operator!=( const std::wstring& str ) const
	{
		return !operator==( str.c_str() );
	}

	bool operator!=( const std::string& str ) const
	{
		return !operator==( str.c_str() );
	}

	bool operator!=( int nNull ) const throw()
	{
		return !operator==( nNull );
	}

	int Compare( const CHeBSTR& bstr, UInt32 flags = 0 ) const throw()
	{
		return static_cast<int>(HeStringCmp( m_str, bstr.m_str, flags ) - 1);
	}

	int CompareNoCase( const CHeBSTR& bstr ) const throw()
	{
		return static_cast<int>(HeStringCmp( m_str, bstr.m_str, HE_VARCMP_IGNORECASE ) - 1);
	}

	HeResult WriteToStream( Helium::IHeStream* pStream ) throw()
	{
		if ( pStream == NULL ) return HE_E_NULL_POINTER;
			
		UInt32		cbStrLen = 0;
		if ( m_str != NULL )
		{
			cbStrLen = static_cast<UInt32>((HeStringLen( m_str ) + 1) * sizeof(UInt16));
		}

		UInt32		cb = 0;
		HeResult	hr = pStream->Write( &cbStrLen, sizeof(cbStrLen), &cb );
		if ( HE_SUCCEEDED(hr) && (cbStrLen > 0) )
		{
			CW2UTF16	strConv( m_str );
			hr = pStream->Write( (UInt16*) strConv, cbStrLen, &cb );
		}
		return hr;
	}

	HeResult ReadFromStream( Helium::IHeStream* pStream ) throw()
	{
		if ( pStream == NULL ) return HE_E_NULL_POINTER;
			
		Empty();
		
		UInt32		cbStrLen = 0;
		HeResult	hr = pStream->Read( &cbStrLen, sizeof(cbStrLen), NULL );
		if ( (hr == HE_S_OK) && (cbStrLen > 0) )
		{
			UInt16*	pUTF16 = (UInt16*) HeTaskMemAlloc( cbStrLen );
			if ( pUTF16 == NULL )
			{
				hr = HE_E_OUT_OF_MEMORY;
			}
			else
			{
				hr = pStream->Read( pUTF16, cbStrLen, NULL );
				if ( hr == HE_S_OK )
				{
					CUTF162W	strConv( pUTF16 );
					m_str = HeAllocStringLen( strConv, strConv.size() );
					if ( m_str == NULL )
					{
						hr = HE_E_OUT_OF_MEMORY;
					}
				}
				HeTaskMemFree( pUTF16 );
			}

			// If HeAllocStringByteLen or IHeStream::Read() failed, reset seek 
			// pointer to start of HEBSTR size.
			if ( hr != HE_S_OK )
			{
				const SInt64	nOffset = -(static_cast<SInt64>(sizeof(cbStrLen)));
				pStream->Seek( nOffset, Helium::IHeStream::STREAM_SEEK_CUR, NULL );
			}
		}
		if ( hr == HE_S_FALSE ) hr = HE_E_FAIL;
		return hr;
	}
};

} /* namespace HeLib */

#endif /* HLSTR_H */
