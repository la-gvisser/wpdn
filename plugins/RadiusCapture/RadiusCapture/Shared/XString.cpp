// =============================================================================
//	TString< Y >.cpp
// =============================================================================
//	Copyright (c) AG Group, Inc. 1999-2000. All rights reserved.
//	Copyright (c) WildPackets, Inc. 2000-2001. All rights reserved.

//#include "StdAfx.h"

//Deprecated:
#pragma warning(disable : 4996)

#include <ctype.h>
#include <cstdarg>

#ifndef ASSERT
#include <assert.h>
#define ASSERT assert
#endif

const UInt32 kBlockSize	= 8;

template<class Y>
TCHAR TString< Y >::m_sEmptyString[] = _T("");

// =============================================================================
//	TString< Y >
// =============================================================================

// -----------------------------------------------------------------------------
//	TString< Y >()													   [constructor]
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >::TString()
	: m_pData( NULL ),
	  m_nLength( 0 ),
	  m_nCapacity( 0 )
{
}


// -----------------------------------------------------------------------------
//	TString< Y >( const TCHAR* )										   [constructor]
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >::TString(
	const TCHAR *inStr )
		: m_pData( NULL ),
		  m_nLength( 0 ),
		  m_nCapacity( 0 )
{
	ASSERT( inStr != NULL );

	if( inStr != NULL )
	{
		Assign( inStr );
	}
}


// -----------------------------------------------------------------------------
//	TString< Y >( const TCHAR*, Y )								   [constructor]
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >::TString(
	const TCHAR*	inStr,
	Y		 		inLength )
		: m_pData( NULL ),
		  m_nLength( 0 ),
		  m_nCapacity( 0 )
{
	ASSERT( inStr != NULL );

	if( inStr != NULL )
	{
		Assign( inStr, inLength );
	}
}


// -----------------------------------------------------------------------------
//	TString< Y >( const TString< Y >& ) 									   [constructor]
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >::TString(
	const TString< Y >&	inStr )
		: m_pData( NULL ),
		  m_nLength( 0 ),
		  m_nCapacity( 0 )
{
	Assign( inStr );
}


// -----------------------------------------------------------------------------
//	TString< Y >( TCHAR )											   [constructor]
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >::TString(
	TCHAR	inChar )
		: m_pData( NULL ),
		  m_nLength( 0 ),
		  m_nCapacity( 0 )
{
	ASSERT( inChar != 0 );

	if( inChar != 0 )
	{
		Assign( inChar );
	}
}


#if TARGET_OS_MAC


// -----------------------------------------------------------------------------
//	TString< Y >( ConstStr255Param )									   [constructor]
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >::TString(
	ConstStr255Param	inPStr )
		: m_pData( NULL ),
		  m_nLength( 0 ),
		  m_nCapacity( 0 )
{
	ASSERT( inPStr != 0 );

	if( inPStr != 0 )
	{
		Assign( inPStr );
	}
}


#endif


#if TARGET_OS_WIN32


// -----------------------------------------------------------------------------
//	TString< Y >( int nResourceID )									   [constructor]
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >::TString(
	int nResourceID )
		: m_pData( NULL ),
		  m_nLength( 0 ),
		  m_nCapacity( 0 )
{
	LoadResource( nResourceID );
}

// -----------------------------------------------------------------------------
//	LoadResource( int nResourceID )
// -----------------------------------------------------------------------------

template<class Y>
void
TString< Y >::LoadResource(
	int nResourceID )
{
	int		nBufferSize = 255;
	TCHAR	szStr[256];

	if ( ::LoadString( NULL, nResourceID, szStr, nBufferSize ) > 0 )
	{
		Assign( szStr );
	}
}


#endif


// -----------------------------------------------------------------------------
//	~TString< Y >														[destructor]
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >::~TString()
{
	if( m_pData != NULL )
	{
		free( m_pData );
	}
}


// -----------------------------------------------------------------------------
//	Empty
// -----------------------------------------------------------------------------

template<class Y>
void
TString< Y >::Empty()
{
	// Empty out the string.
	m_nLength	= 0;
	m_nCapacity	= 0;

	if( m_pData != NULL )
	{
		free( m_pData );
		m_pData = NULL;
	}
}


// -----------------------------------------------------------------------------
//	operator const TCHAR*() const
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >::operator const TCHAR*() const
{
	if ( m_pData == NULL )
	{
		return m_sEmptyString;
	}

	return m_pData;
}

// -----------------------------------------------------------------------------
//	Compare( const TCHAR* )
// -----------------------------------------------------------------------------

template<class Y>
SInt16
TString< Y >::Compare(
	const TCHAR*	rhs ) const
{
	ASSERT( rhs != NULL );

	return (SInt16) _tcscmp( m_pData, rhs );
}


// -----------------------------------------------------------------------------
//	Compare( const TString< Y >& )
// -----------------------------------------------------------------------------


template<class Y>
SInt16
TString< Y >::Compare(
	const XCompare& rhs, int /*iFlag*/ ) const
{
	TString< Y >* pString = NULL;
	XCompare* pCompare = (XCompare*)&rhs;
	pCompare->QueryInterface( XStringID, (void**)&pString );
	if ( pString == NULL && m_pData == NULL ) return 0;
	if ( pString == NULL ) return -1;
	if ( m_pData == NULL ) return 1;

	return (SInt16) _tcscmp( m_pData, (const TCHAR*) *pString );
}


// -----------------------------------------------------------------------------
//	Substring( Y, Y )
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >
TString< Y >::Substring(
	Y			inIndex,
	SInt32			inLength ) const
{
	ASSERT( m_nLength >= 0 );

	if ( inIndex + inLength > m_nLength ||
		 m_nLength <= 0 )
	{
		return TString< Y >();
	}

	return TString< Y >( m_pData + inIndex, inLength );
}

// -----------------------------------------------------------------------------
//	Substring( Y )
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >
TString< Y >::Substring(
	Y			inIndex ) const
{
	ASSERT( m_nLength >= 0 );

	if ( inIndex >= m_nLength ||
		 m_nLength <= 0 )
	{
		return TString< Y >();
	}

	return TString< Y >( m_pData + inIndex );
}


// -----------------------------------------------------------------------------
//	Assign( TCHAR )													 [protected]
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >&
TString< Y >::Assign(
	TCHAR	inChar )
{
	if( AdjustCapacity( 1 ) )
	{
		m_pData[0]	= inChar;
		m_pData[1]	= 0;
		m_nLength	= 1;
	}

	return *this;
}


// -----------------------------------------------------------------------------
//	Assign( const TCHAR* )											 [protected]
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >&
TString< Y >::Assign(
	const TCHAR*	inStr )
{
	ASSERT( inStr != NULL );

	if( inStr != NULL )
	{
		Y nStrLen = (Y)_tcslen( inStr );

		if( AdjustCapacity( nStrLen ) )
		{
			_tcscpy( m_pData, inStr );
			m_nLength = nStrLen;
		}
	}

	return *this;
}


// -----------------------------------------------------------------------------
//	Assign( const TCHAR*, Y )									 [protected]
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >&
TString< Y >::Assign(
	const TCHAR*	inStr,
	Y				inLength )
{
	ASSERT( inStr != NULL );

	if( ( inStr != NULL ) && ( AdjustCapacity( inLength ) ) )
	{
		_tcsncpy( m_pData, inStr, inLength );
		m_pData[inLength] = 0;
		m_nLength = (Y)inLength;
	}

	return *this;
}


// -----------------------------------------------------------------------------
//	Assign( const TString< Y >& )										 [protected]
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >&
TString< Y >::Assign(
	const TString< Y >&	inStr )
{
	if ( &inStr != this )
	{
		Y	nStrLen = inStr.GetLength();
		if( AdjustCapacity( nStrLen ) )
		{
			_tcscpy( m_pData, (const TCHAR*) inStr );
			m_nLength = nStrLen;
		}
	}

	return *this;
}


#if TARGET_OS_MAC


// -----------------------------------------------------------------------------
//	Assign( ConstStr255Param )										 [protected]
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >&
TString< Y >::Assign(
	ConstStr255Param	inPStr )
{
	ASSERT( inPStr != NULL );

	Y	nStrLen = inPStr[0];

	if ( ( nStrLen > 0 ) && ( AdjustCapacity( nStrLen ) ) )
	{
		::BlockMoveData( &inPStr[1], m_pData, nStrLen );
		m_nLength = nStrLen;
		m_pData[ m_nLength ] = 0;
	}

	return *this;
}


// -----------------------------------------------------------------------------
//	CopyToPStr
// -----------------------------------------------------------------------------

template<class Y>
bool
TString< Y >::CopyToPStr(
	UInt8*	outPStr )
{
	if ( m_nLength > 255 )
	{
		outPStr[0] = 0;
		return false;
	}

	outPStr[0] = m_nLength;
	::BlockMoveData( m_pData, outPStr + 1, outPStr[0] );

	return true;
}


#endif


// -----------------------------------------------------------------------------
//	DoFind( const TCHAR*, Y, Y, bool, Y& )
// -----------------------------------------------------------------------------

template<class Y>
bool
TString< Y >::DoFind(
	const TCHAR*	inPattern,
	Y			inPatternLength,
	Y			inFirstOffset,
	bool			inIgnoreCase,
	Y&			outFoundAt ) const
{
	Y	i, j;
	bool	bFound = false;

	for ( i = inFirstOffset, j = 0; (i < m_nLength) && (j < inPatternLength); i++, j++ )
	{
		while ( i < m_nLength )
		{
			TCHAR	cd = m_pData[i];
			TCHAR	cp = inPattern[j];

			if ( inIgnoreCase )
			{
				// JG - I wasn't sure what the windows equivalent of tolower
				// was for use with TCHAR (it isn't in MacPrefix.h)
				// please let me know if this causes problems on windows
				cd = (TCHAR) tolower( cd );
				cp = (TCHAR) tolower( cp );
			}

			if ( cd == cp )
			{
				if ( j == inPatternLength - 1 )
				{
					bFound = true;
				}
				break;
			}

			i -= j - 1;
			j = 0;
		}
	}

	if ( bFound )
	{
		outFoundAt = i - inPatternLength;
		return true;
	}

	return false;
}


// -----------------------------------------------------------------------------
//	Format
// -----------------------------------------------------------------------------

template<class Y>
// ::vsnprintf() does not work on Mac OS X (either that, or Metrowerk's va_args() implementation
// is incompatible with the system ::vsnprintf() call)
#if TARGET_OS_WIN32
int
TString< Y >::Format(
	const TCHAR* inFormat,
	... )
{
	// Need some data.
	if( m_pData == NULL )
	{
		AdjustCapacity( 1 );
	}

	int		nLength = 0;
	bool	bDone = false;
	while( !bDone )
	{
#if TARGET_OS_MAC
		STD_ va_list args;
#elif TARGET_OS_WIN32
		va_list args;
#endif // TARGET_OS_MAC
		va_start(args, inFormat);
		nLength = _vsntprintf( m_pData, m_nCapacity - 1, inFormat, args );
		va_end(args);
		if( nLength > 0 )
		{
			m_nLength = nLength;
			m_pData[m_nLength] = 0;
			bDone = true;
		}
		else
		{
			AdjustCapacity( m_nCapacity * 2 );
		}
	}

	return nLength;
}
#endif // TARGET_OS_WIN32


// -----------------------------------------------------------------------------
//	DoFindPrevious( const TCHAR*, Y, Y, bool, Y& )
// -----------------------------------------------------------------------------

template<class Y>
bool
TString< Y >::DoFindPrevious(
	const TCHAR*	inPattern,
	Y			inPatternLength,
	Y			inLastOffset,
	bool			inIgnoreCase,
	Y&			outFoundAt ) const
{
	SInt32	i, j;

	if ( inLastOffset > m_nLength - 1 )
	{
		inLastOffset = m_nLength - 1;
	}

	bool	bFound = false;

	for ( i = inLastOffset, j = inPatternLength - 1; (i >= 0) && (j >= 0);
			i--, j-- )
	{
		while ( i >= 0 )
		{
			TCHAR	cd = m_pData[i];
			TCHAR	cp = inPattern[j];

			if ( inIgnoreCase )
			{
				cd = (TCHAR) tolower( cd );
				cp = (TCHAR) tolower( cp );
			}

			if ( cd == cp )
			{
				if ( j == 0 )
				{
					bFound = true;
				}
				break;
			}

			i += inPatternLength - 2 - j;
			j = inPatternLength - 1;
		}
	}

	if ( bFound )
	{
		outFoundAt = i + 1;
		return true;
	}

	return false;
}


// -----------------------------------------------------------------------------
//	Append( TCHAR )													 [protected]
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >&
TString< Y >::Append(
	TCHAR	inChar )
{
	ASSERT( inChar != 0 );
	if( ( inChar != 0 ) && ( AdjustCapacity( m_nLength + 1 ) ) )
	{
		m_pData[m_nLength++] = inChar;
		m_pData[m_nLength] = 0;
	}

	return *this;
}


// -----------------------------------------------------------------------------
//	Append( const TCHAR* )											 [protected]
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >&
TString< Y >::Append(
	const TCHAR*	inStr )
{
	ASSERT( inStr != NULL );
	if( inStr != NULL )
	{
		Y	nStrLen	= _tcslen( inStr );
		if( ( nStrLen > 0 ) && ( AdjustCapacity( m_nLength + nStrLen ) ) )
		{
			_tcscat( m_pData, inStr );
			m_nLength += nStrLen;
		}
	}

	return *this;
}


// -----------------------------------------------------------------------------
//	Append( const TString< Y >& )										 [protected]
// -----------------------------------------------------------------------------

template<class Y>
TString< Y >&
TString< Y >::Append(
	const TString< Y >&	inStr )
{
	Y	nStrLen = inStr.GetLength();

	if( ( nStrLen > 0 ) && ( AdjustCapacity( m_nLength + nStrLen ) ) )
	{
		// Can't use strcat if both strings are the same.
		if( m_pData == (const TCHAR*) inStr )
		{
			memcpy( m_pData + m_nLength, m_pData, nStrLen * sizeof(TCHAR) );
			m_pData[m_nLength + nStrLen] = 0;
		}
		else
		{
			_tcscat( m_pData, (const TCHAR*) inStr );
		}
		m_nLength += nStrLen;
	}

	return *this;
}



// -----------------------------------------------------------------------------
//	GetBlockSize													[protected]
//  this allows a subclass to implement its own block scheme
// -----------------------------------------------------------------------------

template<class Y>
Y TString< Y >::GetBlockSize(
	Y	/*inCharsNeeded*/ )
{
	return (Y)kBlockSize;
}

// -----------------------------------------------------------------------------
//	AdjustCapacity													 [protected]
// -----------------------------------------------------------------------------


template<class Y>
bool
TString< Y >::AdjustCapacity(
	Y	inCharsNeeded )
{
	// Allow for the terminator.
	inCharsNeeded += 1;

	if( inCharsNeeded <= m_nCapacity )
	{
		return true;
	}

	Y	nNewCapacity = m_nCapacity;
	while( nNewCapacity <= inCharsNeeded )
	{
		nNewCapacity = (Y)(nNewCapacity + (Y)GetBlockSize( (Y)inCharsNeeded ));
	}

	m_pData = (TCHAR*) realloc( m_pData, nNewCapacity * sizeof(TCHAR) );
	if( m_pData != NULL )
	{
		if ( m_nCapacity == 0 )
		{
			m_pData[0] = 0;
		}
		m_nCapacity = nNewCapacity;
	}

	return m_pData != NULL;
}




/******************************************************
 * Calculate hash and length value for a string.
 *****************************************************/

template< class Y >
int TString< Y >::CalcHash( int hashSize )
{
	const TCHAR* str = m_pData;

    int h,len;
	int a=127;

    for(h = 0,len = 0; *str != '\0'; str++,len++)
	{
		h = (a*h + *str) % hashSize;
    }

	if (h < 0)
	{
		h = (abs(h)) % hashSize;
	}

	return h;
}

