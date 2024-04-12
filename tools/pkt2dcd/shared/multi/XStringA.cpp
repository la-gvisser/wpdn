// =============================================================================
//	TStringA< Y >.cpp
// =============================================================================
//	Copyright (c) AG Group, Inc. 1999-2000. All rights reserved.
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.

// #include "stdafx.h"

#include <ctype.h>
#include <cstdarg>

#ifndef ASSERT
#include <assert.h>
#define ASSERT assert
#endif

template<class Y>
CHAR TStringA< Y >::m_sEmptyString[] = "";			//BLOOP _T("");

// =============================================================================
//	TStringA< Y >
// =============================================================================

// -----------------------------------------------------------------------------
//	TStringA< Y >()													   [constructor]
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >::TStringA()
	: m_pData( NULL ),
	  m_nLength( 0 ),
	  m_nCapacity( 0 )
{
}


// -----------------------------------------------------------------------------
//	TStringA< Y >( const CHAR* )										   [constructor]
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >::TStringA( 
	const CHAR *inStr )
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
//	TStringA< Y >( const CHAR*, Y )								   [constructor]
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >::TStringA( 
	const CHAR*		inStr,
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
//	TStringA< Y >( const TStringA< Y >& ) 									   [constructor]
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >::TStringA( 
	const TStringA< Y >&	inStr )
		: m_pData( NULL ),
		  m_nLength( 0 ),
		  m_nCapacity( 0 )
{
	Assign( inStr );
}


// -----------------------------------------------------------------------------
//	TStringA< Y >( CHAR )											   [constructor]
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >::TStringA(
	CHAR	inChar )
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
//	TStringA< Y >( ConstStr255Param )									   [constructor]
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >::TStringA(
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
//	TStringA< Y >( int nResourceID )									   [constructor]
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >::TStringA(
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
TStringA< Y >::LoadResource(
	int nResourceID )
{
	int		nBufferSize = 255;
	CHAR	szStr[256];

	if ( ::LoadStringA( NULL, nResourceID, szStr, nBufferSize ) > 0 )
	{
		Assign( szStr );
	}
}


#endif


// -----------------------------------------------------------------------------
//	~TStringA< Y >														[destructor]
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >::~TStringA()
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
TStringA< Y >::Empty()
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
//	operator const CHAR*() const
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >::operator const CHAR*() const
{
	if ( m_pData == NULL )
	{
		return m_sEmptyString;
	}
	
	return m_pData;
}

// -----------------------------------------------------------------------------
//	Compare( const CHAR* )
// -----------------------------------------------------------------------------

template<class Y>
SInt16
TStringA< Y >::Compare(
	const CHAR*		rhs ) const
{
	ASSERT( rhs != NULL );
	
	return (SInt16) strcmp( m_pData, rhs );			//BLOOP _tcscmp
}


// -----------------------------------------------------------------------------
//	Compare( const String< Y >& )
// -----------------------------------------------------------------------------


template<class Y>
SInt16
TStringA< Y >::Compare( 
	const XCompare& rhs, int /*iFlag*/ ) const
{
	TStringA< Y >* pString = NULL;
	XCompare* pCompare = (XCompare*)&rhs;
	pCompare->QueryInterface( TStringAID, (void**)&pString );
	if ( pString == NULL && m_pData == NULL ) return 0;
	if ( pString == NULL ) return -1;
	if ( m_pData == NULL ) return 1;

	return (SInt16) strcmp( m_pData, (const CHAR*) *pString );				//BLOOP _tcscmp
}


// -----------------------------------------------------------------------------
//	Substring( Y, Y )
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >
TStringA< Y >::Substring(
	Y			inIndex,
	SInt32			inLength ) const
{
	ASSERT( m_nLength >= 0 );
	
	if ( inIndex + inLength > m_nLength ||
		 m_nLength <= 0 )
	{
		return TStringA< Y >();
	}
	
	return TStringA< Y >( m_pData + inIndex, inLength );
}

// -----------------------------------------------------------------------------
//	Substring( Y )
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >
TStringA< Y >::Substring(
	Y			inIndex ) const
{
	ASSERT( m_nLength >= 0 );
	
	if ( inIndex >= m_nLength ||
		 m_nLength <= 0 )
	{
		return TStringA< Y >();
	}

	return TStringA< Y >( m_pData + inIndex );
}


// -----------------------------------------------------------------------------
//	Assign( CHAR )													 [protected]
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >&
TStringA< Y >::Assign(
	CHAR	inChar )
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
//	Assign( const CHAR* )											 [protected]
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >&
TStringA< Y >::Assign(
	const CHAR*	inStr )
{
	ASSERT( inStr != NULL );
	
	if( inStr != NULL )
	{
		Y nStrLen = (Y)strlen( inStr );				//BLOOP _tcslen

		if( AdjustCapacity( nStrLen ) )
		{
			strcpy( m_pData, inStr );				//BLOOP _tcscpy
			m_nLength = nStrLen;
		}
	}

	return *this;
}


// -----------------------------------------------------------------------------
//	Assign( const CHAR*, Y )									 [protected]
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >&
TStringA< Y >::Assign(
	const CHAR*	inStr,
	Y				inLength )
{
	ASSERT( inStr != NULL );

	if( ( inStr != NULL ) && ( AdjustCapacity( inLength ) ) )
	{
		strncpy( m_pData, inStr, inLength );						//BLOOP _tcsncpy
		m_pData[inLength] = 0;
		m_nLength = (Y)inLength;
	}

	return *this;
}


// -----------------------------------------------------------------------------
//	Assign( const TStringA< Y >& )										 [protected]
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >&
TStringA< Y >::Assign(
	const TStringA< Y >&	inStr )
{
	if ( &inStr != this )
	{
		Y	nStrLen = inStr.GetLength();
		if( AdjustCapacity( nStrLen ) )
		{
			strcpy( m_pData, (const CHAR*) inStr );					//BLOOP _tcscpy
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
TStringA< Y >&
TStringA< Y >::Assign(
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
TStringA< Y >::CopyToPStr(
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
//	DoFind( const CHAR*, Y, Y, bool, Y& )
// -----------------------------------------------------------------------------

template<class Y>
bool
TStringA< Y >::DoFind(
	const CHAR*	inPattern,
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
			CHAR	cd = m_pData[i];
			CHAR	cp = inPattern[j];
			
			if ( inIgnoreCase )
			{
				// JG - I wasn't sure what the windows equivalent of tolower
				// was for use with TCHAR (it isn't in MacPrefix.h)
				// please let me know if this causes problems on windows
				cd = (CHAR) tolower( cd );
				cp = (CHAR) tolower( cp );
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
TStringA< Y >::Format( 
	const CHAR* inFormat, 
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
		nLength = _vsnprintf( m_pData, m_nCapacity - 1, inFormat, args );
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
//	DoFindPrevious( const CHAR*, Y, Y, bool, Y& )
// -----------------------------------------------------------------------------

template<class Y>
bool
TStringA< Y >::DoFindPrevious(
	const CHAR*	inPattern,
	Y			inPatternLength,
	Y			inLastOffset,
	bool		inIgnoreCase,
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
			CHAR	cd = m_pData[i];
			CHAR	cp = inPattern[j];
			
			if ( inIgnoreCase )
			{
				cd = (CHAR) tolower( cd );
				cp = (CHAR) tolower( cp );
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
//	Append( CHAR )													 [protected]
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >&
TStringA< Y >::Append(
	CHAR	inChar )
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
//	Append( const CHAR* )											 [protected]
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >&
TStringA< Y >::Append(
	const CHAR*	inStr )
{
	ASSERT( inStr != NULL );
	if( inStr != NULL )
	{
		Y	nStrLen	= strlen( inStr );					//BLOOP _tcslen
		if( ( nStrLen > 0 ) && ( AdjustCapacity( m_nLength + nStrLen ) ) )
		{
			strcat( m_pData, inStr );					//BLOOP _tcscat
			m_nLength += nStrLen;
		}
	}
	
	return *this;
}


// -----------------------------------------------------------------------------
//	Append( const TStringA< Y >& )										 [protected]
// -----------------------------------------------------------------------------

template<class Y>
TStringA< Y >&
TStringA< Y >::Append(
	const TStringA< Y >&	inStr )
{
	Y	nStrLen = inStr.GetLength();
	
	if( ( nStrLen > 0 ) && ( AdjustCapacity( m_nLength + nStrLen ) ) )
	{
		// Can't use strcat if both strings are the same.
		if( m_pData == (const CHAR*) inStr )
		{
			memcpy( m_pData + m_nLength, m_pData, nStrLen * sizeof(CHAR) );
			m_pData[m_nLength + nStrLen] = 0;
		}
		else
		{
			strcat( m_pData, (const CHAR*) inStr );			//BLOOP _tcscat
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
Y TStringA< Y >::GetBlockSize( 
	Y	/*inCharsNeeded*/ )
{
	return 16;
}

// -----------------------------------------------------------------------------
//	AdjustCapacity													 [protected]
// -----------------------------------------------------------------------------


template<class Y>
bool
TStringA< Y >::AdjustCapacity( 
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
	
	m_pData = (CHAR*) realloc( m_pData, nNewCapacity * sizeof(CHAR) );
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
X_RESULT TStringA< Y >::CalcHash( int hashSize ) const
{
	const CHAR* str = m_pData;

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