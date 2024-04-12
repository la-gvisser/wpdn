// ============================================================================
//	PeekStrings.h
//		interface for: CPeekString, CPeekStringA, CPeekStringX.
// ============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "Peek.h"
#include <wchar.h>
#include <string>
#include <algorithm>
#include <functional>
#include <vector>
#include <string.h>
#include <sstream>
#include <locale>
#include <cctype>
#include <iomanip>

using std::vector;

#ifdef WP_LINUX
extern int wcscasecmp (__const wchar_t *__s1, __const wchar_t *__s2) __THROW;
#endif


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Peek String, FastCat and String Hash
//
//	A Peek String is a Standard Template Library string with MFC/ATL CString
//	semantics.
//
//	A Peek Out String is an std::ostringstream. Peek Out String is used to
//	format data in display format.
//
//	A Peek In String is a std::istringstream. Peek In String is used to parse
//	display format data into specific data formats.
//
//	CPeekString, CPeekInString and CPeekOutString are all 16-bit (UTF-16)
//	Unicode.
//	CPeekStringA, CPeekInStringA and CPeekOutStringA are all 8-bit (UTF-8)
//	ASCII.
//
//	FastCat is a fast string concatenation function.
//	The input is an array of pointers to std::string(s), with the last entry
//	set to nullptr. Performance testing has shown this algorithm to be
//	faster than the addition operator:
//		str = strA + strB + strC;
//	And simple append() or += operations:
//		str = strA; str += strB; str += strC;
//	Using vector<std::string*> for input negates the performance improvements.
//	(Unless the space in the vector is reserved.)
//	Example:
//		const CPeekString*	ayStrings[] = { &strA, &strB, &strC, nullptr };
//		CPeekString			str = FastCat( ayStrings );
//
//	String Hash is a string hashing function from:
//		http://www.cs.yorku.ca/~oz/hash.html
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


#define FMTA_HEX04	std::hex << std::setfill( '0' ) << std::setw( 4 )
#define FMTA_HEX08	std::hex << std::setfill( '0' ) << std::setw( 8 )
#define FMTW_HEX04	std::hex << std::setfill( L'0' ) << std::setw( 4 )
#define FMTW_HEX08	std::hex << std::setfill( L'0' ) << std::setw( 8 )

static char		s_azWhiteSpace[] = " \t\n\v\f";
static wchar_t	s_wzWhiteSpace[] = L" \t\n\v\f";

class CPeekStringA;

#ifdef PEEK_UI
extern HINSTANCE	GetInstanceHandle();
#endif


// ============================================================================
//		CPeekString
// ============================================================================

class CPeekString
	:	public std::wstring
	,	public COmniModeledData
{
#if (0)
protected:
#pragma warning (push)
#pragma warning(disable : 4996)

	CPeekString&	AppendFormat( const wchar_t* inFormat, ... ) {
		if ( inFormat != nullptr ) {
			va_list	arg_ptr;
			va_start( arg_ptr, inFormat );

			int	nLength = _vscwprintf( inFormat, arg_ptr ) + 1;
			if ( nLength > 0 ) {
				std::wstring	strBuf( (nLength + 1), L' ' );
				vswprintf( &strBuf[0], nLength, inFormat, arg_ptr );
				append( strBuf );
			}
			va_end( arg_ptr );
		}
		return *this;
	}

	CPeekString&	Format( const wchar_t* inFormat, ... ) {
		if ( inFormat != nullptr ) {
			va_list	arg_ptr;
			va_start( arg_ptr, inFormat );

			int	nLength = _vscwprintf( inFormat, arg_ptr ) + 1;
			if ( nLength > 0 ) {
				std::wstring	strBuf( (nLength + 1), L' ' );
				vswprintf( strBuf[0], nLength, inFormat, arg_ptr );
				assign( spBuf );
			}
			va_end( arg_ptr );
		}
		return *this;
	}

#pragma warning (pop)
#endif	// if (0)

	void			FormatList( const wchar_t* inFormat, va_list pArgs );

public:
	;			CPeekString() {}
	;			CPeekString( const std::wstring inOther )
		: std::wstring( inOther ) {}
	;			CPeekString( const wchar_t* inOther )
		: std::wstring( (inOther) ? inOther : L"" ) {}
	;			CPeekString( const wchar_t* inOther, size_t inLength )
		: std::wstring( ((inOther) ? inOther : L""), inLength ) {}
	;			CPeekString( size_type inLength, wchar_t inChar )
		: std::wstring( inLength, inChar ) {}
#ifdef PEEK_UI
	explicit	CPeekString( CString inOther )
		: std::wstring( inOther.GetString() ) {}
#endif
	explicit	CPeekString( const std::string& inOther ) { Convert( inOther ); }
	explicit	CPeekString( const CPeekStringA& inOther );
	;			~CPeekString() {}

	;				operator const wchar_t*() const { return c_str(); }
	const wchar_t	operator []( ptrdiff_t inOffset ) const { return std::wstring::operator[]( inOffset ); }
	bool			operator ==( const CPeekString& inOther ) const {
		return (compare( inOther ) == 0);
	}
	bool			operator ==( const wchar_t* inOther ) const {
		return (compare( inOther ) == 0);
	}
	bool			operator <=( const CPeekString& inOther ) const {
		return (compare( inOther ) <= 0);
	}
	bool			operator >=( const CPeekString& inOther ) const {
		return (compare( inOther ) >= 0);
	}
	bool			operator <( const CPeekString& inOther ) const {
		return (compare( inOther ) < 0);
	}
	bool			operator >( const CPeekString& inOther ) const {
		return (compare( inOther ) > 0);
	}

	CPeekString&	Append( const wchar_t* inValue ) {
		append( inValue );
		return *this;
	}

	int			Compare( const CPeekString& inOther ) const {
		return compare( inOther );
	}
	int			CompareNoCase( const CPeekString& inOther ) const {
#ifdef WP_LINUX
		return wcscasecmp( c_str(), inOther.c_str() );
#else
		return _wcsicmp( c_str(), inOther.c_str() );
#endif
	}
	void		Convert( const std::string& inOther );
	void		Convert( const char* inOther, size_t inLength );

	CPeekString&	Empty() { clear(); return *this; }

	int			Find( const wchar_t inTarget, size_t inOffset = 0 ) const {
		std::wstring::size_type	nIndex = find( inTarget, inOffset );
		if ( nIndex == std::wstring::npos ) return -1;
		return static_cast<int>( nIndex );
	}
	int			Find( const wchar_t* inTarget, size_t inOffset = 0 ) const {
		if ( inTarget == nullptr ) return -1;
		std::wstring::size_type	nIndex = find( inTarget, inOffset );
		if ( nIndex == std::wstring::npos ) return -1;
		return static_cast<int>( nIndex );
	}
	int			FindOneOf( const wchar_t* inTarget, size_t inOffset = 0 ) const {
		if ( inTarget == nullptr ) return -1;
		std::wstring::size_type	nIndex = find_first_of( inTarget, inOffset );
		if ( nIndex == std::wstring::npos ) return -1;
		return static_cast<int>( nIndex );
	}
	const wchar_t*	Format() const { return c_str(); }
	void			Format( const wchar_t* inFormat, ... );

	wchar_t		GetAt( size_t inIndex ) const { return at( inIndex ); }
	bool		GetEnvironmentVariable( const wchar_t* inLabel );
	size_t		GetLength() const { return length(); }
	UInt32		GetLength32() const;

	CPeekString&	Insert( size_t inOffset, const wchar_t* inValue ) {
		insert( inOffset, inValue );
		return *this;
	}
	bool		IsEmpty() const { return empty(); }

	CPeekString	Left( size_t inLength ) const {
		return CPeekString( substr( 0, inLength ) );
	}

#ifdef PEEK_UI
	bool		LoadString( UInt32 inId ) {
		wchar_t	szValue[MAX_PATH];
		if ( ::LoadStringW( GetInstanceHandle(), inId, szValue, MAX_PATH ) == 0 ) return false;
		assign( szValue );
		return true;
	}
#endif // PEEK_UI

	CPeekString&	MakeLower() {
		std::transform( begin(), end(), begin(), tolower );
		return *this;
	}
	CPeekString&	MakeUpper() {
		std::transform( begin(), end(), begin(), toupper );
		return *this;
	}
	wchar_t		Mid( size_t inOffset ) const { return at( inOffset ); }
	CPeekString	Mid( size_t inOffset, size_t inLength ) const {
		return CPeekString( substr( inOffset, inLength ) );
	}

	void		Preallocate( size_t inSize ) { reserve( inSize ); }

	void		Remove( wchar_t inChar ) {
		erase( std::remove( begin(), end(), inChar ), end() );
	}
	CPeekString&	Replace( const wchar_t inMatch, wchar_t inReplace ) {
		std::replace_if( begin(), end(),
			bind2nd(std::equal_to<wchar_t>(), inMatch), inReplace );
		return *this;
	}
	void		Reverse() {
		std::reverse( begin(), end() );
	}
	CPeekString	Right( size_t inLength ) const {
		return CPeekString( substr( (size() - inLength), inLength ) );
	}

	CPeekString		Tokenize( const wchar_t* inSeparators, size_t& ioOffset );
	CPeekString&	Trim( const wchar_t* inTrim = s_wzWhiteSpace ) {
		if ( inTrim != nullptr ) {
			TrimRight( inTrim );
			TrimLeft( inTrim );
		}
		return *this;
	}
	CPeekString&	TrimLeft( const wchar_t* inTrim = s_wzWhiteSpace ) {
		if ( inTrim != nullptr ) {
			std::wstring::size_type nIndex = find_first_not_of( inTrim );
			if ( nIndex != std::wstring::npos ) {
				erase( 0, nIndex );
			}
		}
		return *this;
	}
	CPeekString&	TrimRight( const wchar_t* inTrim = s_wzWhiteSpace ) {
		if ( inTrim != nullptr ) {
			std::wstring::size_type nIndex = find_last_not_of( inTrim );
			if ( nIndex != std::wstring::npos ) {
				erase( (nIndex + 1), std::wstring::npos );
			}
		}
		return *this;
	}

	// COmniModeledData
	virtual CPeekString	ModeledDataGet() const { return CPeekString( c_str() ); }
	virtual void		ModeledDataSet( const CPeekString& inValue ) { assign( inValue ); }
};


// ============================================================================
//		CPeekStringA
// ============================================================================

class CPeekStringA
	:	public std::string
	,	public COmniModeledData
{
#if (0)
protected:
#pragma warning (push)
#pragma warning(disable : 4996)

	CPeekStringA&	AppendFormat( const char* inFormat, ... ) {
		if ( inFormat != nullptr ) {
			va_list	arg_ptr;
			va_start( arg_ptr, inFormat );

			int	nLength = _vscprintf( inFormat, arg_ptr ) + 1;
			if ( nLength > 0 ) {
				std::string	strBuf( (nLength + 1), ' ' );
				vsprintf( &strBuf[0], inFormat, arg_ptr );
				append( spBuf );
			}
			va_end( arg_ptr );
		}
		return *this;
	}

	CPeekStringA&	Format( const char* inFormat, ... ) {
		if ( inFormat != nullptr ) {
			va_list	arg_ptr;
			va_start( arg_ptr, inFormat );

			int	nLength = _vscprintf( inFormat, arg_ptr ) + 1;
			if ( nLength > 0 ) {
				std::string	strBuf( (nLength + 1), ' ' );
				vsprintf( &strBuf[0], inFormat, arg_ptr );
				assign( spBuf );
			}
			va_end( arg_ptr );
		}
		return *this;
	}

#pragma warning (pop)
#endif	// if (0)

	void			FormatList( const char* inFormat, va_list pArgs );

public:
	;			CPeekStringA() {}
	;			CPeekStringA( const std::string inOther )
		: std::string( inOther ) {}
	;			CPeekStringA( const char* inOther )
		: std::string( (inOther) ? inOther : "" ) {}
	;			CPeekStringA( const char* inOther, size_t inLength )
		: std::string( ((inOther) ? inOther : ""), inLength ) {}
	;			CPeekStringA( size_type inLength, char inChar )
		: std::string( inLength, inChar ) {}
#ifdef PEEK_UI
	explicit	CPeekStringA( CStringA inOther )
		: std::string( inOther.GetString() ) {}
#endif // PEEK_UI
	explicit	CPeekStringA( const std::wstring& inOther ) { Convert( inOther ); }
	explicit	CPeekStringA( const CPeekString& inOther ) { Convert( inOther ); }
	;			~CPeekStringA() {}

	;			operator const char*() const { return c_str(); }
	const char	operator []( ptrdiff_t inOffset ) const { return std::string::operator[]( inOffset ); }
	bool		operator ==( const CPeekStringA& inOther ) const {
		return (compare( inOther ) == 0);
	}
	bool		operator ==( const char* inOther ) const {
		return (compare( inOther ) == 0);
	}
	bool		operator <=( const CPeekStringA& inOther ) const {
		return (compare( inOther ) <= 0);
	}
	bool		operator >=( const CPeekStringA& inOther ) const {
		return (compare( inOther ) >= 0);
	}
	bool		operator <( const CPeekStringA& inOther ) const {
		return (compare( inOther ) < 0);
	}
	bool		operator >( const CPeekStringA& inOther ) const {
		return (compare( inOther ) > 0);
	}

	CPeekStringA&	Append( const char* inValue ) {
		append( inValue );
		return *this;
	}

	int			Compare( const CPeekStringA& inOther ) const {
		return compare( inOther );
	}
	int			CompareNoCase( const CPeekStringA& inOther ) const {
		return strcasecmp( c_str(), inOther.c_str() );
	}
	void		Convert( const std::wstring& inOther );
	void		Convert( const wchar_t* inOther, size_t inLength );

	CPeekStringA&	Empty() { clear(); return *this; }

	int			Find( const char inTarget, size_t inOffset = 0 ) const {
		std::string::size_type	nIndex = find( inTarget, inOffset );
		if ( nIndex == std::string::npos ) return -1;
		return static_cast<int>( nIndex );
	}
	int			Find( const char* inTarget, size_t inOffset = 0 ) const {
		if ( inTarget == nullptr ) return -1;
		std::string::size_type	nIndex = find( inTarget, inOffset );
		if ( nIndex == std::string::npos ) return -1;
		return static_cast<int>( nIndex );
	}
	int			FindOneOf( const char* inTarget, size_t inOffset = 0 ) const {
		if ( inTarget == nullptr ) return -1;
		std::string::size_type	nIndex = find_first_of( inTarget, inOffset );
		if ( nIndex == std::string::npos ) return -1;
		return static_cast<int>( nIndex );
	}
	const char*		Format() const { return c_str(); }
	void			Format( const char* inFormat, ... );

	char		GetAt( size_t inIndex ) const { return at( inIndex ); }
	bool		GetEnvironmentVariable( const char* inLabel );
	size_t		GetLength() const { return length(); }
	UInt32		GetLength32() const;

	CPeekStringA&	Insert( size_t inOffset, const char* inValue ) {
		insert( inOffset, inValue );
		return *this;
	}
	bool		IsEmpty() const { return empty(); }

	CPeekStringA	Left( size_t inLength ) const {
		return CPeekStringA( substr( 0, inLength ) );
	}

#ifdef PEEK_UI
	bool		LoadString( UInt32 inId ) {
		char	szValue[MAX_PATH];
		if ( ::LoadStringA( GetInstanceHandle(), inId, szValue, MAX_PATH ) == 0 ) return false;
		assign( szValue );
		return true;
	}
#endif // PEEK_UI

	CPeekStringA&	MakeLower() {
		std::transform( begin(), end(), begin(), tolower );
		return *this;
	}
	CPeekStringA&	MakeUpper() {
		std::transform( begin(), end(), begin(), toupper );
		return *this;
	}
	char			Mid( size_t inOffset ) const { return at( inOffset ); }
	CPeekStringA	Mid( size_t inOffset, size_t inLength ) const {
		return CPeekStringA( substr( inOffset, inLength ) );
	}

	void		Preallocate( size_t inSize ) { reserve( inSize ); }

	void		Remove( char inChar ) {
		erase( std::remove( begin(), end(), inChar ), end() );
	}
	void		Reverse() {
		std::reverse( begin(), end() );
	}
	CPeekStringA&	Replace( const char inMatch, char inReplace ) {
		std::replace_if( begin(), end(), bind2nd(std::equal_to<char>(), inMatch), inReplace );
		return *this;
	}
	CPeekStringA	Right( size_t inLength ) const {
		return CPeekStringA( substr( (size() - inLength), inLength ) );
	}

	CPeekStringA	Tokenize( const char* inSeparators, size_t& ioOffset );
	CPeekStringA&	Trim( const char* inTrim = s_azWhiteSpace ) {
		if ( inTrim != nullptr ) {
			TrimRight( inTrim );
			TrimLeft( inTrim );
		}
		return *this;
	}
	CPeekStringA&	TrimLeft( const char* inTrim = s_azWhiteSpace ) {
		if ( inTrim != nullptr ) {
			std::string::size_type nIndex = find_first_not_of( inTrim );
			if ( nIndex != std::string::npos ) {
				erase( 0, nIndex );
			}
		}
		return *this;
	}
	CPeekStringA&	TrimRight( const char* inTrim = s_azWhiteSpace ) {
		if ( inTrim != nullptr ) {
			std::string::size_type nIndex = find_last_not_of( inTrim );
			if ( nIndex != std::string::npos ) {
				erase( (nIndex + 1), std::string::npos );
			}
		}
		return *this;
	}

	// COmniModeledData
	virtual CPeekString	ModeledDataGet() const { return CPeekString( *this ); }
	virtual void		ModeledDataSet( const CPeekString& inValue ) {
		Convert( inValue );
	}
};


// ============================================================================
//		CPeekInString
// ============================================================================

class CPeekInString
	:	public std::wistringstream
{
public:
	;		CPeekInString() {}
	;		~CPeekInString() {}

	;		operator CPeekString() { return CPeekString( str() ); }
};


// ============================================================================
//		CPeekOutString
// ============================================================================

class CPeekOutString
	:	public std::wostringstream
{
public:
	;		CPeekOutString() {}
	;		~CPeekOutString() {}

	;		operator CPeekString() { return CPeekString( str() ); }
};


// ============================================================================
//		CPeekInStringA
// ============================================================================

class CPeekInStringA
	:	public std::istringstream
{
public:
	;		CPeekInStringA() {}
	;		~CPeekInStringA() {}

	;		operator CPeekStringA() { return CPeekStringA( str() ); }
};


// ============================================================================
//		CPeekOutStringA
// ============================================================================

class CPeekOutStringA
	:	public std::ostringstream
{
public:
	;		CPeekOutStringA() {}
	;		~CPeekOutStringA() {}

	;		operator CPeekStringA() { return CPeekStringA( str() ); }
};


// ============================================================================
//		TagCompare
// ============================================================================

bool	TagCompare( const CPeekString& strA, const CPeekString& strB );


// ============================================================================
//		CArrayString
// ============================================================================

class CArrayString
	:	public vector<CPeekString>
{
public:
	bool		Find( const CPeekString& inTarget, size_t& outIndex ) const;

	size_t		Parse( wchar_t inSeperator, const wchar_t* inString );

	CPeekString	ToString() const;
};


// ============================================================================
//		CArrayStringA
// ============================================================================

class CArrayStringA
	:	public vector<CPeekStringA>
{
public:
	bool			Find( const CPeekStringA& inTarget, size_t& outIndex ) const;

	size_t			Parse( char inSeperator, const char* inString );

	CPeekStringA	ToString() const;
};


// ============================================================================
//		CPeekStringX
// ============================================================================

class CPeekStringX
	:	public COmniModeledData
{
protected:
	CPeekStringA	m_strA;
	CPeekString		m_strW;

public:
	;			CPeekStringX() {}
	;			CPeekStringX( const CPeekStringA& inStrA )
		: m_strA( inStrA ), m_strW( inStrA ) {}
	;			CPeekStringX( const CPeekString& inStrW )
		: m_strA( inStrW ), m_strW( inStrW ) {}
	;			~CPeekStringX() {}

	void		Empty() { m_strA.Empty(); m_strW.Empty(); }
	size_t		GetLength() const { return m_strA.GetLength(); }
	bool		IsEmpty() const { return m_strA.IsEmpty(); }
	const CPeekStringA&	GetA() const { return m_strA; }
	const CPeekString&	GetW() const { return m_strW; }

	CPeekStringX	operator=( const CPeekStringA& inStrA ) {
		m_strW = CPeekString( m_strA = inStrA );
		return *this;
	}
	CPeekStringX	operator=( const CPeekString& inStrW ) {
		m_strA = CPeekStringA( m_strW = inStrW );
		return *this;
	}

	bool		operator==( const CPeekStringX& inStrX ) const {
		return (m_strA.CompareNoCase( inStrX.m_strA ) == 0);
	}
	bool		operator!=( const CPeekStringX& inStrX ) const {
		return (m_strA.CompareNoCase( inStrX.m_strA ) != 0);
	}
	bool		operator==( const CPeekStringA& inStrA ) const {
		return (m_strA.CompareNoCase( inStrA ) == 0);
	}
	bool		operator==( const CPeekString& inStrW ) const {
		return (m_strW.CompareNoCase( inStrW ) == 0);
	}

	;			operator CPeekStringA() const { return m_strA; }
	;			operator const char*() const { return m_strA; }
	;			operator CPeekString() const { return m_strW; }
	;			operator const wchar_t*() const { return m_strW; }

	int			CompareNoCase( const CPeekStringA& in ) const {
		return m_strA.CompareNoCase( in );
	}
	int			CompareNoCase( const CPeekString& in ) const {
		return m_strW.CompareNoCase( in );
	}

	// COmniModeledData
	virtual CPeekString	ModeledDataGet() const { return m_strW; }
	virtual void		ModeledDataSet( const CPeekString& inValue ) {
		m_strA = CPeekStringA( m_strW = inValue);
	}
};


// ============================================================================
//		FastCat
// ============================================================================

extern CPeekString	FastCat( const CPeekString** inStrings );
extern CPeekStringA	FastCat( const CPeekStringA** inStrings );
extern CPeekString	FastCat( const std::vector<const CPeekString*>& inStrings );
extern CPeekStringA	FastCat( const std::vector<const CPeekStringA*>& inStrings );


// ============================================================================
//		StringHash
// ============================================================================

namespace StringHash
{

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4706)	// Warning: assignment within conditional expression
#endif

// -----------------------------------------------------------------------------
//		DJB2
// -----------------------------------------------------------------------------
//	Pulled off the internet from http://www.cs.yorku.ca/~oz/hash.html:
//		This algorithm (k=33) was first reported by Dan Bernstein
//		many years ago in comp.lang.c. Another version of this
//		algorithm (now favored by Bernstein) uses xor:
//			hash(i) = hash(i - 1) * 33 ^ str[i];
//		The magic of number 33 (why it works better than many other
//		constants, prime or not) has never been adequately explained.

inline unsigned long
DJB2( const char* str )
{
	unsigned long	hash = 5381;
	while ( const int c = static_cast<int>( *str++ ) ) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}
	return hash;
}

inline unsigned long
DJB2( const wchar_t* str )
{
	unsigned long	hash = 5381;
	while ( const int c = static_cast<int>( *str++ ) ) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}
	return hash;
}

inline unsigned long
DJB2( const std::string&	inString )
{
	return DJB2( inString.c_str() );
}

inline unsigned long
DJB2( const std::wstring & inString )
{
	return DJB2( inString.c_str() );
}

// useful for THashMap hashor
class Hashor
{
public :
	template < typename T >
	int hashcode( const T& inString ) const {
		return static_cast<int>( DJB2( inString ) );
	}
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} /* namespace StringHash */
