// =============================================================================
//	XMLBuilder.cpp
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2005. All rights reserved.
//	Copyright (c) AG Group, Inc. 1999-2000. All rights reserved.

#include "stdafx.h"
#include "XMLBuilder.h"
#ifdef _WIN32
#include "CT2UTF8.h"
#endif

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

// Static members.
SInt32 XXMLBuilder::s_nDepth	= 0;	// Current element depth.


// =============================================================================
//	XXMLBuilder
// =============================================================================

// -----------------------------------------------------------------------------
//	XXMLBuilder													   [constructor]
// -----------------------------------------------------------------------------

XXMLBuilder::XXMLBuilder()
	: m_Stream( NULL ),
	  m_pRootElement( NULL )
{
}


// -----------------------------------------------------------------------------
//	~XXMLBuilder													[destructor]
// -----------------------------------------------------------------------------

XXMLBuilder::~XXMLBuilder()
{
}


// -----------------------------------------------------------------------------
//	DoBuild
// -----------------------------------------------------------------------------
// Build XML from given element.

bool
XXMLBuilder::DoBuild(
	XXMLElement*	inElement,
	CByteStream*	inStream )
{
	ASSERT( inElement != NULL );
	if( inElement == NULL ) return false;

	m_pRootElement	= inElement;
	m_Stream		= inStream;

	return BuildDocument();
}


// -----------------------------------------------------------------------------
//	BuildDocument													   [private]
// -----------------------------------------------------------------------------
// document ::= XMDDecl? Misc* element Misc*

bool
XXMLBuilder::BuildDocument()
{
	if( !BuildXMLDecl() )
	{
		return false;
	}

	if( !BuildElement( m_pRootElement ) )
	{
		return false;
	}

	BuildEndline();

	return true;
}


// -----------------------------------------------------------------------------
//	BuildXMLDecl													   [private]
// -----------------------------------------------------------------------------
// XMLDecl ::= '<?xml' VersionInfo EncodingDecl? S? '?>'

bool
XXMLBuilder::BuildXMLDecl()
{
	// Hard coded for xml 1.0.
#if TARGET_OS_WIN32
	BuildString( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" );
#elif TARGET_OS_MAC
	BuildString( "<?xml version=\"1.0\" encoding=\"macintosh\"?>" );
#else
	BuildString( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" );
#endif

	BuildEndline();

	return true;
}


// -----------------------------------------------------------------------------
//	BuildString														   [private]
// -----------------------------------------------------------------------------

bool
XXMLBuilder::BuildString(
	const char* inString )
{
	ASSERT( inString != NULL );
	if( inString == NULL )
	{
		return false;
	}

	return m_Stream->Write( inString );
}


// -----------------------------------------------------------------------------
//	BuildString														   [private]
// -----------------------------------------------------------------------------

bool
XXMLBuilder::BuildString(
	const wchar_t* inString )
{
	ASSERT( inString != NULL );
	if( inString == NULL )
	{
		return false;
	}

#if TARGET_OS_WIN32
	CW2A	utf8( inString, CP_UTF8 );
	return m_Stream->Write( utf8 );
#else
	return false;
#endif
}


// -----------------------------------------------------------------------------
//	BuildElement													   [private]
// -----------------------------------------------------------------------------
// element	::= EmptyElemTag | STag content ETag
// EmptyElemTag	::= '<' Name (S Attribute)* S? '/>'
// STag			::= '<' Name (S Attribute)* s? '>'

bool
XXMLBuilder::BuildElement(
	const XXMLElement* inElement )
{
	ASSERT( inElement != NULL );
	if( inElement == NULL ) return false;

	// Tab to current depth (makes it look pretty).
	for ( SInt32 i = 0; i < s_nDepth; i++ )
	{
		BuildTab();
	}

	// <Name
	if( !BuildTagBeginning( inElement ) )
	{
		return false;
	}

	// (S Attribute)*
	if( !BuildAttributes( inElement ) )
	{
		return false;
	}

	// How many children?
	SInt32	nChildren = inElement->CountChildren();

	// If no children, and no content, build empty tag end,
	// and we're done.
	if( nChildren == 0 && inElement->GetValue().GetLength() == 0 )
	{
		// />
		if( !BuildEmptyTagEnd() )
		{
			return false;
		}
	}
	else
	{
		// Have children, or content, just do regular tag end.
		if( !BuildTagEnd() )
		{
			return false;
		}

		// Endline if this element has children (makes it look pretty).
		if( nChildren > 0 )
		{
			BuildEndline();
		}

		// Recurse through all children.
		for( SInt32 j = 0; j < nChildren; j++ )
		{
			const XXMLElement*	pChild = inElement->GetChild( j );
			if( pChild != NULL )
			{
				s_nDepth++;
				BuildElement( pChild );
				s_nDepth--;
			}
		}

		// Tab in end tag if no children (makes it look pretty).
		if( nChildren > 0 )
		{
			for( SInt32 k = 0; k < s_nDepth; k++ )
			{
				BuildTab();
			}
		}

		// content </Name>
		if( !BuildContentETag( inElement ) )
		{
			return false;
		}
	}

	BuildEndline();

	return true;
}


// -----------------------------------------------------------------------------
//	BuildTagBeginning												   [private]
// -----------------------------------------------------------------------------
// '<' Name (S Attribute)* S?

bool
XXMLBuilder::BuildTagBeginning(
	const XXMLElement* inElement )
{
	BuildChar( '<' );

	if( !BuildString( inElement->GetName() ) )
	{
		return false;
	}

	return true;
}


// -----------------------------------------------------------------------------
//	BuildTagEnd														   [private]
// -----------------------------------------------------------------------------
// '>'

bool
XXMLBuilder::BuildTagEnd()
{
	BuildChar( '>' );
	return true;
}


// -----------------------------------------------------------------------------
//	BuildEmptyTagEnd												   [private]
// -----------------------------------------------------------------------------
// '/>'

bool
XXMLBuilder::BuildEmptyTagEnd()
{
	BuildString( "/>" );
	return true;
}

// -----------------------------------------------------------------------------
//	BuildAttributes													   [private]
// -----------------------------------------------------------------------------
// '<' Name (S Attribute)* S?
// Attribute ::=  Name Eq AttValue

bool
XXMLBuilder::BuildAttributes(
	const XXMLElement* inElement )
{
	ASSERT( inElement != NULL );
	if( inElement == NULL ) return false;

	SInt32	nAttributes = inElement->CountAttributes();
	for( SInt32 x = 0; x < nAttributes; x++)
	{
		const XXMLAttribute*	pAttribute = inElement->GetAttribute( x );
		if ( pAttribute != NULL )
		{
			if ( !BuildAttribute( pAttribute ) )
			{
				return false;
			}
		}
		else
		{
			//+++ Hmm, should throw?
			return false;
		}
	}

	return true;
}


// -----------------------------------------------------------------------------
//	BuildAttribute													   [private]
// -----------------------------------------------------------------------------
// '<' Name (S Attribute)* S?
// Attribute ::=  Name Eq AttValue

bool
XXMLBuilder::BuildAttribute(
	const XXMLAttribute* inAttribute )
{
	ASSERT( inAttribute != NULL );
	if( inAttribute == NULL ) return false;

	BuildSpace();
	BuildString( inAttribute->GetName() );
	BuildEq();
	BuildQuote();
	if (inAttribute->IsCDATA())
		BuildCDATAString( inAttribute->GetValue() );
	else
		BuildAttribString( inAttribute->GetValue() );
	BuildQuote();

	return true;
}


// -----------------------------------------------------------------------------
//	BuildContentETag												   [private]
// -----------------------------------------------------------------------------

bool
XXMLBuilder::BuildContentETag(
	const XXMLElement*	inElement )
{
	ASSERT( inElement != NULL );
	if( inElement == NULL ) return false;

	if (inElement->IsCDATA())
		BuildCDATAString( inElement->GetValue() );
	else
		BuildAttribString(inElement->GetValue());

	BuildString( "</" );
	if( !BuildString( inElement->GetName() ) )
	{
		return false;
	}
	BuildTagEnd();

	return true;
}


// -----------------------------------------------------------------------------
//	BuildAttribString												   [private]
// -----------------------------------------------------------------------------

bool
XXMLBuilder::BuildAttribString(
	const XString&	inString )
{
#if TARGET_OS_WIN32
	CT2UTF8		utf8( inString );
	const char*	pszString = utf8;
#else
	const char*	pszString = inString;
#endif

	size_t	nLength = strlen( pszString );
	for( size_t x = 0; x < nLength; x++ )
	{
		const char	c = pszString[x];

		switch( c )
		{
			case '\r':
			case '\n':
				// Skip CR/LF.
			break;

			case '&':
				BuildString( "&amp;" );
			break;

			case _T('<'):
				BuildString( "&lt;" );
			break;

			case _T('>'):
				BuildString( "&gt;" );
			break;

			case '\"':
				BuildString( "&quot;" );
			break;

			case '\'':
				BuildString( "&apos;" );
			break;

			default:
				BuildChar( c );
			break;
		}
	}

	return true;
}


// -----------------------------------------------------------------------------
//	BuildEq															   [private]
// -----------------------------------------------------------------------------

void
XXMLBuilder::BuildEq()
{
	BuildChar( '=' );
}


// -----------------------------------------------------------------------------
//	BuildTab														   [private]
// -----------------------------------------------------------------------------

void
XXMLBuilder::BuildTab()
{
	BuildChar( '\t' );
}


// -----------------------------------------------------------------------------
//	BuildQuote														   [private]
// -----------------------------------------------------------------------------

void
XXMLBuilder::BuildQuote()
{
	BuildChar( '\"' );
}


// -----------------------------------------------------------------------------
//	BuildSpace														   [private]
// -----------------------------------------------------------------------------

void
XXMLBuilder::BuildSpace()
{
	BuildChar( ' ' );
}


// -----------------------------------------------------------------------------
//	BuildEndline													   [private]
// -----------------------------------------------------------------------------

void
XXMLBuilder::BuildEndline()
{
	BuildChar( '\r' );
	BuildChar( '\n' );
}


// -----------------------------------------------------------------------------
//	BuildChar														   [private]
// -----------------------------------------------------------------------------

void
XXMLBuilder::BuildChar(
	char	inChar)
{
	m_Stream->WriteBytes( &inChar, sizeof(inChar) );
}


// -----------------------------------------------------------------------------
//	BuildCDATAString												   [private]
// -----------------------------------------------------------------------------

bool
XXMLBuilder::BuildCDATAString(
	const XString&	inString )
{
	return (BuildString( "<![CDATA[" ) &&
			BuildString( inString ) && 
			BuildString( "]]>" ));
}
