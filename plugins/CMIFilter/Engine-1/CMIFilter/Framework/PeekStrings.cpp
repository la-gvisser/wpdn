// =============================================================================
//	PeekStrings.cpp
// =============================================================================
//	Copyright (c) 2010-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekStrings.h"
#include "PeekUnits.h"
#include <string.h>
#include <wchar.h>
#include <memory>

using std::string;
using std::wstring;
using std::vector;


// ----------------------------------------------------------------------------
//		TagCompare
// ----------------------------------------------------------------------------

bool
TagCompare(
	const CPeekString&	strA,
	const CPeekString&	strB )
{
	return (strA.CompareNoCase( strB ) == 0);
}


// ============================================================================
//		CPeekString
// ============================================================================

CPeekString::CPeekString(
	const CPeekStringA& inOther )
{
	Convert( inOther );
}


// ----------------------------------------------------------------------------
//		Convert
// ----------------------------------------------------------------------------

void
CPeekString::Convert(
	const std::string&	inOther )
{
	if ( inOther.empty() ) {
		clear();
	}
	else {
#ifdef _WIN32
		// This is more robust than wcstombs_s
		CString	str( inOther.c_str() );
		assign( str );
#else
		size_t	nNewSize = mbstowcs( nullptr, inOther.c_str(), inOther.size() );
		if ( nNewSize == 0 ) return;

		size_t						nSize = nNewSize;
		std::unique_ptr<wchar_t>	spBuf( new wchar_t[nSize] );
		if ( spBuf.get() == nullptr ) return;
		wchar_t*	pBuf = spBuf.get();

		nNewSize = mbstowcs( pBuf, inOther.c_str(), nSize );
		if ( nNewSize == 0 ) {
			assign( pBuf, nNewSize );
		}
#endif
	}
}

void
CPeekString::Convert(
	const char*	inOther,
	size_t		inLength )
{
	if ( (inOther == nullptr) || (inLength == 0) ) {
		clear();
	}
	else {
#ifdef _WIN32
		// This is more robust than wcstombs_s
		CString	str( inOther );
		assign( str );
#else
		size_t	nSize = inLength + 1;
		std::unique_ptr<wchar_t>	spBuf( new wchar_t[nSize] );
		if ( spBuf.get() == nullptr ) return;
		wchar_t*	pBuf = spBuf.get();

		size_t	nNewSize = mbstowcs( pBuf, inOther, nSize );
		if ( nNewSize > 0 ) {
			assign( pBuf, (nNewSize - 1) );
		}
#endif
	}
}


// ----------------------------------------------------------------------------
//		GetEnvironmentVariable
// ----------------------------------------------------------------------------

bool
CPeekString::GetEnvironmentVariable(
	const wchar_t*	inLabel )
{
	if ( inLabel == nullptr ) return false;

#ifdef _WIN32
	wchar_t*	pValue = nullptr;
	size_t		nLength = 0;
	errno_t		nError = _wdupenv_s( &pValue, &nLength, inLabel );
	if ( nError || (pValue == nullptr) || (nLength == 0) ) return false;

	assign( pValue, (nLength - 1) );
	free( pValue );
#else
	CPeekStringA	strLabel;
	strLabel.Convert( inLabel );
	char*	pValue = getenv( strLabel.c_str() );
	if ( pValue == nullptr ) return false;
	Convert( pValue );
#endif
	return true;
}


// ----------------------------------------------------------------------------
//		GetLength32
// ----------------------------------------------------------------------------

UInt32
CPeekString::GetLength32() const
{
	if ( length() > kMaxUInt32 ) throw;
	return static_cast<UInt32>( length() & 0xFFFFFFFF );
}


// ----------------------------------------------------------------------------
//		Tokenize
// ----------------------------------------------------------------------------

CPeekString
CPeekString::Tokenize(
	const wchar_t*	inSeparators,
	size_t&			ioOffset )
{
	CPeekString		strToken;

	if ( inSeparators != nullptr ) {
		std::wstring	strSeparators( inSeparators );
		if ( ioOffset < size() ) {
			while ( strSeparators.find_first_of( std::wstring::operator[]( ioOffset ) ) != std::wstring::npos ) {
				ioOffset++;
				if ( ioOffset > size() ) {
					return strToken;
				}
			}

			std::wstring::size_type	nOffset = find_first_of( strSeparators, ioOffset );
			if ( nOffset != std::wstring::npos ) {
				strToken = CPeekString( substr( ioOffset, (nOffset - ioOffset) ) );
				ioOffset = nOffset + 1;
			}
			else {
				strToken = CPeekString( substr( ioOffset ) );
				ioOffset = size() + 1;
			}
		}
		else {
			ioOffset = static_cast<size_t>( -1 );
		}
	}

	return strToken;
}


// ============================================================================
//		CPeekStringA
// ============================================================================

// ----------------------------------------------------------------------------
//		Convert
// ----------------------------------------------------------------------------

void
CPeekStringA::Convert(
	const std::wstring&	inOther )
{
	if ( inOther.empty() ) {
		clear();
	}
	else {
#ifdef _WIN32
		// This is more robust than wcstombs_s
		CStringA	strA( inOther.c_str() );
		assign( strA );
#else
		size_t	nSize = inOther.size() + 1;
		std::unique_ptr<char>	spBuf( new char[nSize] );
		if ( spBuf.get() == nullptr ) return;
		char*	pBuf = spBuf.get();

		size_t	nNewSize = wcstombs( pBuf, inOther.c_str(), nSize );
		if ( nNewSize > 0 ) {
			assign( pBuf, nNewSize );
		}
#endif
	}
}

void
CPeekStringA::Convert(
	const wchar_t*	inOther,
	size_t			inLength )
{
	if ( (inOther == nullptr) || (inLength == 0) ) {
		clear();
	}
	else {
#ifdef _WIN32
		// This is more robust than wcstombs_s
		CStringA	strA( inOther );
		assign( strA );
#else
		size_t	nSize = inLength + 1;
		std::unique_ptr<char>	spBuf( new char[nSize] );
		if ( spBuf.get() == nullptr ) return;
		char*	pBuf = spBuf.get();

		size_t	nNewSize = wcstombs( pBuf, inOther, nSize );
		if ( nNewSize > 0 ) {
			assign( pBuf, (nNewSize - 1) );
		}
#endif
	}
}


// ----------------------------------------------------------------------------
//		GetEnvironmentVariable
// ----------------------------------------------------------------------------

bool
CPeekStringA::GetEnvironmentVariable(
	const char*	inLabel )
{
	if ( inLabel == nullptr ) return false;

#if _WIN32
	char*		pValue = nullptr;
	size_t		nLength = 0;
	errno_t		nError = _dupenv_s( &pValue, &nLength, inLabel );
	if ( nError || (pValue == nullptr) || (nLength == 0) ) return false;

	assign( pValue, (nLength - 1) );
	free( pValue );
#else
	char*	pValue = getenv( inLabel );
	if ( pValue == nullptr ) return false;
	*this = pValue;
#endif

	return true;
}


// ----------------------------------------------------------------------------
//		GetLength32
// ----------------------------------------------------------------------------

UInt32
CPeekStringA::GetLength32() const {
	if ( length() > kMaxUInt32 ) throw;
	return static_cast<UInt32>( length() & 0xFFFFFFFF );
}


// ----------------------------------------------------------------------------
//		Tokenize
// ----------------------------------------------------------------------------

CPeekStringA
CPeekStringA::Tokenize(
	const char*	inSeparators,
	size_t&		ioOffset )
{
	CPeekStringA	strToken;

	if ( inSeparators == nullptr ) {
		std::string	strSeparators( inSeparators );
		if ( ioOffset < size() ) {
			while ( strSeparators.find_first_of( std::string::operator[]( ioOffset ) ) != std::string::npos ) {
				ioOffset++;
				if ( ioOffset > size() ) {
					return strToken;
				}
			}

			std::string::size_type	nOffset = find_first_of( strSeparators, ioOffset );
			if ( nOffset != std::string::npos ) {
				strToken = CPeekStringA( substr( ioOffset, (nOffset - ioOffset) ) );
				ioOffset = nOffset + 1;
			}
			else {
				strToken = CPeekStringA( substr( ioOffset ) );
				ioOffset = size() + 1;
			}
		}
		else {
			ioOffset = static_cast<size_t>( -1 );
		}
	}

	return strToken;
}


// ============================================================================
//		CArrayString
// ============================================================================

// ----------------------------------------------------------------------------
//		Find
// ----------------------------------------------------------------------------

bool
CArrayString::Find(
	const CPeekString&	inTarget,
	size_t&				outIndex ) const
{
	for ( size_t i = 0; i < size(); i++ ) {
		if ( at( i ).CompareNoCase( inTarget ) == 0 ) {
			outIndex = i;
			return true;
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		ToString
// ----------------------------------------------------------------------------

CPeekString
CArrayString::ToString() const
{
	size_t	nCount( size() );
	size_t*	aSizes( new size_t[nCount] );
	size_t	nSize( 0 );

	for ( size_t i = 0; i < nCount; i++ ) {
		aSizes[i] = at( i ).GetLength();
		nSize += aSizes[i];
	}
	nSize++;

	wchar_t	wSpace = L' ';
	CPeekString	str( (size_type)nSize, wSpace );
	size_t	nOffset( 0 );
	for ( size_t i = 0; i < nCount; i++ ) {
		str.Insert( nOffset, at( i ) );
		nOffset += aSizes[i];
	}

	delete [] aSizes;

	return str;
}


// ============================================================================
//		CArrayStringA
// ============================================================================

// ----------------------------------------------------------------------------
//		Find
// ----------------------------------------------------------------------------

bool
CArrayStringA::Find(
	const CPeekStringA&	inTarget,
	size_t&				outIndex ) const
{
	size_t	nCount = size();
	for ( size_t i = 0; i < nCount; i++ ) {
		if ( at( i ).CompareNoCase( inTarget ) == 0 ) {
			outIndex = i;
			return true;
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		ToString
// ----------------------------------------------------------------------------

CPeekStringA
CArrayStringA::ToString() const
{
	size_t	nCount( size() );
	size_t*	aSizes( new size_t[nCount] );
	size_t	nSize( 0 );

	for ( size_t i = 0; i < nCount; i++ ) {
		aSizes[i] = at( i ).GetLength();
		nSize += aSizes[i];
	}
	nSize++;

	CPeekStringA	str( nSize, L' ' );
	size_t			nOffset( 0 );
	for ( size_t i = 0; i < nCount; i++ ) {
		str.Insert( nOffset, at( i ) );
		nOffset += aSizes[i];
	}

	delete [] aSizes;

	return str;
}


// ============================================================================
//		FastCat
// ============================================================================

CPeekString
FastCat(
	const CPeekString**	inStrings )
{
	size_t	nLength = 0;
	for (const CPeekString** i = inStrings; *i != nullptr; i++ ) nLength += (*i)->GetLength();
	CPeekString strCat;
	strCat.reserve( nLength );
	for (const CPeekString** i = inStrings; *i != nullptr; i++ ) strCat += **i;
	return strCat;
}

CPeekStringA
FastCat(
	const CPeekStringA**	inStrings )
{
	size_t	nLength = 0;
	for (const CPeekStringA** i = inStrings; *i != nullptr; i++ ) nLength += (*i)->GetLength();
	CPeekStringA strCat;
	strCat.reserve( nLength );
	for (const CPeekStringA** i = inStrings; *i != nullptr; i++ ) strCat += **i;
	return strCat;
}

CPeekString
FastCat(
	const std::vector<const CPeekString*>&	inStrings )
{
	size_t	nLength( 0 );
	{
		for ( vector<const CPeekString*>::const_iterator i = inStrings.begin();
			  i != inStrings.end();
			  i++ )
			nLength += (*i)->GetLength();
	}

	CPeekString strCat;
	strCat.reserve( nLength );
	{
		for ( vector<const CPeekString*>::const_iterator i = inStrings.begin();
			  i != inStrings.end();
			  i++ )
			strCat += **i;
	}
	return strCat;
}

CPeekStringA
FastCat(
	const std::vector<const CPeekStringA*>&	inStrings )
{
	size_t	nLength( 0 );
	{
		for ( vector<const CPeekStringA*>::const_iterator i = inStrings.begin();
			  i != inStrings.end();
			  i++ )
			nLength += (*i)->GetLength();
	}

	CPeekStringA strCat;
	strCat.reserve( nLength );
	{
		for ( vector<const CPeekStringA*>::const_iterator i = inStrings.begin();
			  i != inStrings.end();
			  i++ )
			strCat += **i;
	}
	return strCat;
}
