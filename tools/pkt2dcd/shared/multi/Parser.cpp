// =============================================================================
//	Parser.cpp
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2005. All rights reserved.
//	Copyright (c) AG Group, Inc. 2000. All rights reserved.

#include "stdafx.h"
#include "MemUtil.h"
#include "ByteStream.h"
#include "Parser.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

// =============================================================================
//	XParser
// =============================================================================

// -----------------------------------------------------------------------------
//	XParser														   [constructor]
// -----------------------------------------------------------------------------

XParser::XParser() :
	m_szSource( NULL ),
	m_szSourceCurrent( NULL ),
	m_szSourceEnd( NULL ),
	m_nLine( 1 ),
	m_nColumn( 1 )
{
}


// -----------------------------------------------------------------------------
//	~XParser														[destructor]
// -----------------------------------------------------------------------------

XParser::~XParser()
{
}


// -----------------------------------------------------------------------------
//	DoParse( const CByteStream& )
// -----------------------------------------------------------------------------

bool
XParser::SetDataSource(
	const CByteStream&	inStream )
{
	ASSERT( inStream.GetData() != NULL );
	ASSERT( inStream.GetLength() > 0 );
	if( inStream.GetData() == NULL ) return false;
	if( inStream.GetLength() == 0 ) return false;

	// Initialize.
	m_szSource 			= (char*) inStream.GetData();
	m_szSourceCurrent	= m_szSource;
	m_szSourceEnd		= m_szSource + inStream.GetLength();

#if defined(_DEBUG)
	// CBB - Maintaining these values sucks way too much CPU
	//       so they have been ifdef'd for _DEBUG only
	m_nLine				= 1;
	m_nColumn			= 1;
#endif
	
	return true;
}


// -----------------------------------------------------------------------------
//	DoParse( const char*, SInt32 )
// -----------------------------------------------------------------------------

bool
XParser::SetDataSource(
	const char*	inData,
	SInt32		inDataLength )
{
	ASSERT( inData != NULL );
	ASSERT( inDataLength > 0 );
	if( ( inData == NULL ) || ( inDataLength == 0 ) ) return false;
	
	// Initialize.
	m_szSource 			= inData;
	m_szSourceCurrent	= m_szSource;
	m_szSourceEnd		= m_szSource + inDataLength;
#if defined(_DEBUG)
	m_nLine				= 1;
	m_nColumn			= 1;
#endif
	return true;
}

// -----------------------------------------------------------------------------
//	ParseLong														   [private]
// -----------------------------------------------------------------------------
 
bool
XParser::ParseLong(
	const UInt32 inLong )
{	
	if ( (m_szSourceCurrent + 4) >= m_szSourceEnd )
	{
		return false;
	}
	
	UInt32 nCompare =  MemUtil::Swap32(*((UInt32*)m_szSourceCurrent));

	if (nCompare == inLong)
	{
		m_szSourceCurrent += 4;
		return true;
	}

	return false;
}

// -----------------------------------------------------------------------------
//	ParseString														   [private]
// -----------------------------------------------------------------------------

bool
XParser::ParseString(
	const char*	inString )
{
	ASSERT( inString != NULL );
	if( inString == NULL ) return false;
	
	// Record current position.
	XParseBookmark	bookMark( this );
	
	// While not end of string.
	while( *inString != 0 )
	{
		// Next char.
		char	c = NextChar();
		
		// Same?
		if( c != *inString )
		{
			// Not the same; revert back to previous position.
			bookMark.Restore();
			return false;
		}
		
		// Next char.
		++inString;
	}
	
	return true;
}


// -----------------------------------------------------------------------------
//	ParseStringNoCase												   [private]
// -----------------------------------------------------------------------------

bool
XParser::ParseStringNoCase(
	const char*	inString )
{
	ASSERT( inString != NULL );
	if( inString == NULL ) return false;

	// Record current position.
	XParseBookmark	bookMark( this );
	
	while( *inString != 0 )
	{
		char	c = NextChar();
		if( LowCase(c) != LowCase( *inString ) )
		{
			bookMark.Restore();
			return false;
		}
		
		++inString;
	}
	
	return true;
}


// -----------------------------------------------------------------------------
//	ParseChar														   [private]
// -----------------------------------------------------------------------------
// Return false if next char doesn't match inChar.

bool
XParser::ParseChar(
	char	inChar )
{
	// End of document?
	if ( m_szSourceCurrent >= m_szSourceEnd )
	{
		return 0;
	}
	
	// Get next char.
	/*
	char	c = *m_szSourceCurrent;
 
	if (c == inChar)
	{
		m_szSourceCurrent++;
		return true;
	}

	return false;
	*/


	char	c = NextChar();
	if ( c != inChar )
	{
		PreviousChar();
		return false;
	}
	return true;
}


// -----------------------------------------------------------------------------
//	ParseSpaces														   [private]
// -----------------------------------------------------------------------------
// S ::= (#x20 | #x9 | #xD | #xA)+
  
bool
XParser::ParseSpaces()
{
	char	c = *m_szSourceCurrent;

	if( !IsSpace( c ) )
	{
		return false;
	}

	do
	{
		c = NextChar();
	} 
	while( IsSpace( c ) );

	PreviousChar();

	return true;
}


// -----------------------------------------------------------------------------
//	ParseEOL														   [protected]
// -----------------------------------------------------------------------------
  
bool
XParser::ParseEOL()
{
	char	c = *m_szSourceCurrent;

	if( IsEndline( c ) )
	{
		return true;
	}

	do
	{
		c = NextChar();
	} 
	while( IsEndline( c ) == false );

	PreviousChar();

	return true;
}

// -----------------------------------------------------------------------------
//	ParseNumber														   [private]
// -----------------------------------------------------------------------------

bool
XParser::ParseNumber(
	SInt32&	outNum )
{
	char	c = NextChar();
	
	// Start with a digit?
	if( !IsDigit(c) )
	{
		return false;
	}
	
	outNum = 0;
	while( IsDigit( c ) )
	{
		// Compute new number.
		outNum = outNum * 10 + c - _T('0');
		
		// Next char.
		c = NextChar();
	}
	
	// Current char is not part of the number.
	PreviousChar();

	return true;
}


// -----------------------------------------------------------------------------
//	ParseHexNumber													   [private]
// -----------------------------------------------------------------------------

bool
XParser::ParseHexNumber(
	SInt32&	outNum )
{
	char	c = NextChar();
	
	// Start with hex digit?
	if( !IsHexDigit(c) )
	{
		return false;
	}
	
	outNum = 0;
	while( IsHexDigit( c ) )
	{
		// Compute new number.
		outNum = outNum * 16 + HexDigitValue(c);
		
		// Next char.
		c = NextChar();
	}
	
	// Current char is not part of the number.
	PreviousChar();
	return true;
}


// -----------------------------------------------------------------------------
//	SyntaxError														   [private]
// -----------------------------------------------------------------------------

void
XParser::SyntaxError()
{
#if defined(_DEBUG)
	throw XParseException( m_nLine, m_nColumn );
#else
	throw XParseException( );
#endif
}


// -----------------------------------------------------------------------------
//	NextChar														   [private]
// -----------------------------------------------------------------------------
// S ::= (#x20 | #x9 | #xD | #xA)+

char
XParser::NextChar()
{
	// End of document?
	if ( m_szSourceCurrent >= m_szSourceEnd )
	{
		return 0;
	}
	
	// Get next char.
	char	c = *m_szSourceCurrent++;
	
// zz 2001-07-13 no, you may not skip CR. CR is significant as the ONLY
// line termination character on Mac textfiles, and thus is the only way
// to know if you hit an end-of-line.
//
//	// Skip \r if any.
//	if ( c == _T('\r') )
//	{
//		// End of document?
//		if ( m_szSourceCurrent >= m_szSourceEnd )
//		{
//			return 0;
//		}
//		
//		// Next char.
//		c = *m_szSourceCurrent++;
//	}

	// If new line, increment the line number.
	if ( c == _T('\n') )
	{
		m_nLine++;
		m_nColumn = 1;
	}
	else
	{
		m_nColumn++;
	}

	return c;
}


// -----------------------------------------------------------------------------
//	PreviousChar													   [private]
// -----------------------------------------------------------------------------

void
XParser::PreviousChar()
{
	if( m_szSourceCurrent - 1 < m_szSource )
	{
		m_szSourceCurrent = m_szSource;
	}
	else
	{
		// If new line, decrement the line number.
		m_szSourceCurrent -= 1;

		char	c = *m_szSourceCurrent;

		if ( c == _T('\n') )
		{
			m_nLine--;
			m_nColumn = 1;
		}
		else
		{
			m_nColumn--;
		}
	}
}


// =============================================================================
//	XParser::XParseBookmark
// =============================================================================

// -----------------------------------------------------------------------------
//	GetSubString
// -----------------------------------------------------------------------------

void 
XParser::XParseBookmark::GetSubString(
	XString&	inString, 
	SInt32		inNumEndSkip )
{
    ASSERT( m_pParser->m_szSourceCurrent + inNumEndSkip >= m_szSourceCurrent );
	if( m_pParser->m_szSourceCurrent + inNumEndSkip > m_szSourceCurrent )
	{
#ifdef _UNICODE
		int			len = (int)(m_pParser->m_szSourceCurrent - m_szSourceCurrent - inNumEndSkip);
		wchar_t*	pszw = (wchar_t*) alloca( (len + 1) * sizeof(wchar_t) );
		ASSERT( pszw != NULL );
		mbstowcs( pszw, m_szSourceCurrent, len );
	    inString = XString( pszw, len );
#else
	    inString = XString( m_szSourceCurrent, 
			m_pParser->m_szSourceCurrent - m_szSourceCurrent - inNumEndSkip );
#endif
	}
}
