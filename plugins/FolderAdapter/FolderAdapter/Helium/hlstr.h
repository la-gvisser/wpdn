// =============================================================================
//	hlstr.h
// =============================================================================
//	Copyright (c) 2005-2015 WildPackets, Inc. All rights reserved.

#ifndef HLSTR_H
#define HLSTR_H

#include "hecom.h"
#include "hestr.h"
#include "hevariant.h"
#include "hldef.h"
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
	if ( bstr == nullptr ) return std::wstring();
	return std::wstring( bstr, HeStringLen( bstr ) );
}

inline HeResult
HeHEBSTRFromString(
	const std::wstring&	str,
	Helium::HEBSTR*		pbstr ) throw()
{
	if ( pbstr == nullptr ) return HE_E_NULL_POINTER;
	*pbstr = HeAllocStringLen( str.c_str(), str.size() );
	if ( *pbstr == nullptr ) return HE_E_OUT_OF_MEMORY;
	return HE_S_OK;
}

inline HeResult
HeHEBSTRFromString(
	const std::string&	str,
	Helium::HEBSTR*		pbstr ) throw()
{
	if ( pbstr == nullptr ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	try
	{
		HeLib::CUTF82W	wstr( str.c_str(), str.size() );
		*pbstr = HeAllocStringLen( wstr, wstr.size() );
		if ( *pbstr == nullptr ) return HE_E_OUT_OF_MEMORY;
	}
	HE_CATCH(hr)

	return hr;
}

inline HeResult
HeHEBSTRToString(
	Helium::HEBSTR	bstr,
	std::wstring&	str ) throw()
{
	HeResult	hr = HE_S_OK;

	try
	{
		if ( bstr != nullptr )
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
		if ( bstr != nullptr )
		{
			HeLib::CW2UTF8	astr( bstr, HeStringLen( bstr ) );
			str.assign( astr, astr.size() );
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

	CHeBSTR() throw() : m_str( nullptr )
	{
	}

	explicit CHeBSTR( std::size_t cch ) : m_str( nullptr )
	{
		if ( cch > 0 )
		{
			m_str = HeAllocStringLen( nullptr, cch );
			if ( m_str == nullptr )
			{
				HeThrow( HE_E_OUT_OF_MEMORY );
			}
		}
	}

	CHeBSTR( std::size_t cch, const wchar_t* sz ) : m_str( nullptr )
	{
		if ( cch > 0 )
		{
			m_str = HeAllocStringLen( sz, cch );
			if ( m_str == nullptr )
			{
				HeThrow( HE_E_OUT_OF_MEMORY );
			}
		}
	}

	CHeBSTR( const wchar_t* pSrc ) : m_str( nullptr )
	{
		if ( pSrc != nullptr )
		{
			m_str = HeAllocString( pSrc );
			if ( m_str == nullptr )
			{
				HeThrow( HE_E_OUT_OF_MEMORY );
			}
		}
	}

	CHeBSTR( const char* psz ) : m_str( nullptr )
	{
		if ( psz != nullptr )
		{
			HeLib::CUTF82W	s( psz );
			m_str = HeAllocStringLen( s, s.size() );
			if ( m_str == nullptr ) HeThrow( HE_E_OUT_OF_MEMORY );
		}
	}

	CHeBSTR( std::size_t cch, const char* pch ) : m_str( nullptr )
	{
		if ( (pch == nullptr) && (cch != 0) )
		{
			m_str = HeAllocStringLen( nullptr, cch );
			if ( m_str == nullptr ) HeThrow( HE_E_OUT_OF_MEMORY );
		}
		else if ( pch != nullptr )
		{
			m_str = HeAllocStringLen( HeLib::CUTF82W( pch, cch ), cch );
			if ( (m_str == nullptr) && (cch != 0) )
			{
				HeThrow( HE_E_OUT_OF_MEMORY );
			}
		}
	}

	CHeBSTR( const std::wstring& str ) : m_str( nullptr )
	{
		HeThrowIfFailed( HeHEBSTRFromString( str, &m_str ) );
	}

	CHeBSTR( const std::string& str ) : m_str( nullptr )
	{
		HeThrowIfFailed( HeHEBSTRFromString( str, &m_str ) );
	}

	CHeBSTR( const CHeBSTR& src )
	{
		m_str = src.Copy();
		if ( !!src && (m_str == nullptr) )
		{
			HeThrow( HE_E_OUT_OF_MEMORY );
		}
	}

#ifdef HE_HAS_RVALUE_REFS
	CHeBSTR( CHeBSTR&& src ) throw()
	{
		m_str = src.m_str;
		src.m_str = nullptr;
	}
#endif /* HE_HAS_RVALUE_REFS */

	CHeBSTR( const Helium::HeID& id )
	{
		wchar_t	szID[64];
		if ( !id.ToString( szID, 64 ) )
		{
			HeThrow( HE_E_FAIL );
		}
		m_str = HeAllocString( szID );
		if ( m_str == nullptr )
		{
			HeThrow( HE_E_OUT_OF_MEMORY );
		}
	}

	CHeBSTR& operator=( const CHeBSTR& src )
	{
		if ( m_str != src.m_str )
		{
			Empty();
			if ( src.m_str != nullptr )
			{
				m_str = src.Copy();
				if ( m_str == nullptr )
				{
					HeThrow( HE_E_OUT_OF_MEMORY );
				}
			}
		}
		return *this;
	}

#ifdef HE_HAS_RVALUE_REFS
	CHeBSTR& operator=( CHeBSTR&& src ) throw()
	{
		if ( m_str != src.m_str )
		{
			Empty();
			m_str = src.m_str;
			src.m_str = nullptr;
		}
		return *this;
	}
#endif /* HE_HAS_RVALUE_REFS */

	CHeBSTR& operator=( const wchar_t* pSrc )
	{
		if ( pSrc != m_str )
		{
			Empty();
			if ( pSrc != nullptr )
			{
				m_str = HeAllocString( pSrc );
				if ( m_str == nullptr )
				{
					HeThrow( HE_E_OUT_OF_MEMORY );
				}
			}
		}
		return *this;
	}

	CHeBSTR& operator=( const char* psz )
	{
		Empty();
		if ( psz != nullptr )
		{
			HeLib::CUTF82W	s( psz );
			m_str = HeAllocStringLen( s, s.size() );
			if ( m_str == nullptr )
			{
				HeThrow( HE_E_OUT_OF_MEMORY );
			}
		}
		return *this;
	}

	CHeBSTR& operator=( const std::wstring& str )
	{
		Empty();
		if ( !str.empty() )
		{
			m_str = HeAllocStringLen( str.c_str(), str.size() );
			if ( m_str == nullptr )
			{
				HeThrow( HE_E_OUT_OF_MEMORY );
			}
		}
		return *this;
	}

	CHeBSTR& operator=( const std::string& str )
	{
		return operator=( str.c_str() );
	}

	~CHeBSTR() throw()
	{
		if ( m_str != nullptr )
		{
			HeFreeString( m_str );
		}
	}

	std::size_t Length() const throw()
	{
		return (m_str == nullptr) ? 0 : HeStringLen( m_str );
	}

	std::size_t ByteLength() const throw()
	{
		return (m_str == nullptr) ? 0 : HeStringByteLen( m_str );
	}

	operator Helium::HEBSTR() const throw()
	{
		return m_str;
	}

	Helium::HEBSTR* get_address_of() throw()
	{
		return &m_str;
	}

	void swap( CHeBSTR& other ) throw()
	{
		std::swap( m_str, other.m_str );
	}

	// No! Never overload operator&, but you may use get_address_of() above or
	// access m_str directly.
//	Helium::HEBSTR* operator&() throw()
//	{
//		return &m_str;
//	}

	Helium::HEBSTR Copy() const throw()
	{
		if ( m_str == nullptr ) return nullptr;
		return HeAllocStringByteLen( reinterpret_cast<char*>(m_str), HeStringByteLen( m_str ) );
	}

	HeResult CopyTo( Helium::HEBSTR* pbstr ) const throw()
	{
		if ( pbstr == nullptr ) return HE_E_NULL_POINTER;
		*pbstr = Copy();
		if ( (*pbstr == nullptr) && (m_str != nullptr) )
		{
			return HE_E_OUT_OF_MEMORY;
		}
		return HE_S_OK;
	}

	HeResult CopyTo( Helium::HEVARIANT* pVarDest ) const throw()
	{
		if ( pVarDest == nullptr ) return HE_E_NULL_POINTER;

		pVarDest->vt      = Helium::HE_VT_BSTR;
		pVarDest->bstrVal = Copy();
		if ( (pVarDest->bstrVal == nullptr) && (m_str != nullptr) )
		{
			return HE_E_OUT_OF_MEMORY;
		}

		return HE_S_OK;
	}

	void Attach( Helium::HEBSTR src ) throw()
	{
		if ( m_str != src )
		{
			if ( m_str != nullptr )
			{
				HeFreeString( m_str );
			}
			m_str = src;
		}
	}

	Helium::HEBSTR Detach() throw()
	{
		Helium::HEBSTR	s = m_str;
		m_str = nullptr;
		return s;
	}

	void Empty() throw()
	{
		if ( m_str != nullptr )
		{
			HeFreeString( m_str );
			m_str = nullptr;
		}
	}

	bool IsEmpty() const throw()
	{
		return (Length() == 0);
	}

	bool operator!() const throw()
	{
		return (m_str == nullptr);
	}

	// A HEBSTR is just a HECHAR* so we need a special version to signify
	// that we are appending a HEBSTR.
	HeResult AppendBSTR( Helium::HEBSTR p ) throw()
	{
		std::size_t		cch2 = HeStringLen( p );
		if ( cch2 == 0 ) return HE_S_OK;

		std::size_t		cch1 = Length();

		Helium::HEBSTR	bstrNew = HeAllocStringLen( nullptr, cch1 + cch2 );
		if ( bstrNew == nullptr ) return HE_E_OUT_OF_MEMORY;

		if ( m_str != nullptr )
		{
			std::memcpy( bstrNew, m_str, cch1 * sizeof(Helium::HECHAR) );
		}
		std::memcpy( bstrNew + cch1, p, cch2 * sizeof(Helium::HECHAR) );
		bstrNew[cch1 + cch2] = 0;

		if ( m_str != nullptr )
		{
			HeFreeString( m_str );
		}
		m_str = bstrNew;
		return HE_S_OK;
	}

	HeResult Append( const CHeBSTR& bstrSrc ) throw()
	{
		return AppendBSTR( bstrSrc.m_str );
	}

	HeResult Append( const wchar_t* psz, std::size_t cch ) throw()
	{
		if ( (psz == nullptr) || ((m_str != nullptr) && (cch == 0)) )
		{
			return HE_S_OK;
		}

		const std::size_t	n1 = Length();
		Helium::HEBSTR		b  = HeAllocStringLen( nullptr, n1 + cch );
		if ( b == nullptr )
		{
			return HE_E_OUT_OF_MEMORY;
		}
		if ( m_str != nullptr )
		{
			std::memcpy( b, m_str, n1 * sizeof(wchar_t) );
		}
		std::memcpy( b + n1, psz, cch * sizeof(wchar_t) );
		b[n1 + cch] = 0;
		if ( m_str != nullptr )
		{
			HeFreeString( m_str );
		}
		m_str = b;
		return HE_S_OK;
	}

	HeResult Append( wchar_t ch ) throw()
	{
		return Append( &ch, 1 );
	}

	HeResult Append( const wchar_t* psz ) throw()
	{
		if ( psz == nullptr ) return HE_S_OK;
		return Append( psz, std::wcslen( psz ) );
	}

	HeResult Append( const std::wstring& str ) throw()
	{
		return Append( str.c_str(), str.size() );
	}

	HeResult Append( char ch ) throw()
	{
		const wchar_t	chO = static_cast<wchar_t>(ch);
		return Append( &chO, 1 );
	}

	HeResult Append( const char* psz ) throw()
	{
		if ( psz == nullptr ) return HE_S_OK;

		CHeBSTR	bstrTemp;
		try { bstrTemp = psz; } catch ( ... ) {}
		if ( bstrTemp.m_str == nullptr )
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
			Empty();
			if ( bstrSrc != nullptr )
			{
				m_str = HeAllocStringByteLen(
					reinterpret_cast<char*>(bstrSrc), HeStringByteLen( bstrSrc ) );
				if ( m_str == nullptr )
				{
					hr = HE_E_OUT_OF_MEMORY;
				}
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
		return operator==( const_cast<const wchar_t*>(pszSrc) );
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
		return (m_str == nullptr);
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
		return operator!=( const_cast<const wchar_t*>(pszSrc) );
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

	HeResult WriteToStream( Helium::IHeStream* pStream ) const throw()
	{
		if ( pStream == nullptr ) return HE_E_NULL_POINTER;

		std::size_t	cch      = 0;
		UInt32		cbStrLen = 0;
		if ( m_str != nullptr )
		{
			cch      = HeStringLen( m_str );
			cbStrLen = static_cast<UInt32>((cch + 1) * sizeof(UInt16));
		}

		UInt32		cb = 0;
		HeResult	hr = pStream->Write( &cbStrLen, sizeof(cbStrLen), &cb );
		if ( HE_SUCCEEDED(hr) && (cbStrLen > 0) )
		{
#ifdef HE_WIN32
			// Avoid the conversion on Windows since wide char is already UTF16.
			hr = pStream->Write( m_str, cbStrLen, &cb );
#else
			CW2UTF16	strConv( m_str, cch );
			hr = pStream->Write( (UInt16*) strConv, cbStrLen, &cb );
#endif /* HE_WIN32 */
		}
		return hr;
	}

	HeResult ReadFromStream( Helium::IHeStream* pStream ) throw()
	{
		if ( pStream == nullptr ) return HE_E_NULL_POINTER;

		Empty();

		// Read the string byte length.
		UInt32		cbStrLen = 0;
		HeResult	hr = pStream->Read( &cbStrLen, sizeof(cbStrLen), nullptr );
		if ( hr == HE_S_OK )
		{
			if ( cbStrLen == 0 )
			{
				// Read nullptr string.
			}
			else if ( cbStrLen < sizeof(UInt16) )
			{
				HE_TRACE( "CHeBSTR::ReadFromStream() stream corrupt\n" );
				hr = HE_E_FAIL;
			}
			else if ( cbStrLen > 0x100000 )
			{
				HE_TRACE( "CHeBSTR::ReadFromStream() string too large, possible stream corruption\n" );
				hr = HE_E_ACCESS_DENIED;
			}
			else
			{
#ifdef HE_WIN32
				// Avoid the conversion on Windows since wide char is already UTF16.
				m_str = HeAllocStringByteLen( nullptr, cbStrLen - sizeof(UInt16) );
				if ( m_str == nullptr )
				{
					hr = HE_E_OUT_OF_MEMORY;
				}
				else
				{
					hr = pStream->Read( m_str, cbStrLen, nullptr );
					if ( hr != HE_S_OK )
					{
						HE_TRACE( "CHeBSTR::ReadFromStream() Read() failed\n" );
						Empty();
					}
				}
#else
				UInt16	buf[128];
				UInt16*	pUTF16 = buf;
				if ( cbStrLen > sizeof(buf) )
				{
					pUTF16 = (UInt16*) HeTaskMemAlloc( cbStrLen );
				}
				if ( pUTF16 == nullptr )
				{
					hr = HE_E_OUT_OF_MEMORY;
				}
				else
				{
					hr = pStream->Read( pUTF16, cbStrLen, nullptr );
					if ( hr == HE_S_OK )
					{
						const std::size_t	cch = (cbStrLen / sizeof(UInt16)) - 1;
						CUTF162W			strConv( pUTF16, cch );
						m_str = HeAllocStringLen( strConv, cch );
						if ( m_str == nullptr )
						{
							hr = HE_E_OUT_OF_MEMORY;
						}
					}
					if ( pUTF16 != buf )
					{
						HeTaskMemFree( pUTF16 );
					}
				}
#endif /* HE_WIN32 */
			}
		}

		if ( hr == HE_S_FALSE )
		{
			hr = HE_E_FAIL;
		}

		return hr;
	}
};

} /* namespace HeLib */

#endif /* HLSTR_H */
