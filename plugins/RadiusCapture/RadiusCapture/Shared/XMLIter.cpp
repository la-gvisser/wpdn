// =============================================================================
//	XMLIter.cpp
// =============================================================================
//	Copyright (c) Wildpackets, Inc. 2001-2005. All rights reserved.

#include "stdafx.h"
#include "XMLIter.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

XXMLParseIterator::XXMLParseIterator()
{
}

XXMLParseIterator::~XXMLParseIterator()
{
}

// =============================================================================
// InitParser - read out the prolog part of the file
// =============================================================================
bool XXMLParseIterator::InitParser()
{
	XParseBookmark	bookMark( this );
	m_bEmptyTag = false;

	try
	{
		ParseProlog();
	}
	catch (XParseException e)
	{
		e;	// To avoid "unused variable" warning in release build.
		TRACE(_T("Caught XMLParser exception line %d, column %d\n"), (int)e.GetLine(), (int)e.GetColumn());
		bookMark.Restore();
		return false;
	}

	return true;
}

// =============================================================================
// NextRoot - read in all of the next element's attributes and elements 
// =============================================================================
bool XXMLParseIterator::NextRoot(XXMLElement& element, XXMLElement* pParent)
{
	XParseBookmark	bookMark( this );
	m_bEmptyTag = false;

	try
	{
		if (ParseElement(element, pParent))
			return true;
	}
	catch (XParseException e)
	{
		e;	// To avoid "unused variable" warning in release build.
		TRACE(_T("Caught XMLParser exception line %d, column %d\n"), (int)e.GetLine(), (int)e.GetColumn());
	}

	bookMark.Restore();
	return false;
}

// =============================================================================
// NextChild - read in the current element's tag and attributes, but no elements
// =============================================================================
bool XXMLParseIterator::NextChild(XXMLElement& element, XXMLElement* pParent)
{
	m_bEmptyTag = false;
	XParseBookmark	bookMark( this );

	try
	{
		ParseSpaces();

		if (ParseTagBeginning(element, pParent))
		{
			char	c = NextChar();
			if( c == '/')
			{
				c = NextChar();
				if( c == '>' )
				{
					m_bEmptyTag = true;
					// Empty tag.
					return true;
				}
				
				SyntaxError();
			}
			
			if( c != '>' )
			{
				SyntaxError();
			}

			return true;
		}
	}
	catch (XParseException e)
	{
		e;	// To avoid "unused variable" warning in release build.
		TRACE(_T("Caught XMLParser exception line %d, column %d\n"), (int)e.GetLine(), (int)e.GetColumn());
	}

	bookMark.Restore();
	return false;
}

// =============================================================================
// ParseTagBeginning - same as XXMLParser, except foes not create a new element, has one passed in
// =============================================================================
bool XXMLParseIterator::ParseTagBeginning(XXMLElement& element, XXMLElement* /*pParent*/)
{
	ParseSpaces();

	if( !ParseChar( '<' ) )
	{
		return false;
	}
	
	XString	strName;
	if( !ParseName( strName ) )
	{
		return false;
	}
	
	element.SetName(strName);

	while( ParseSpaces() && ParseAttribute( &element ) )
	{
	}
	
	return true;
}

// =============================================================================
// ParseElement - parse the next child element and all of it's content
// =============================================================================
bool XXMLParseIterator::ParseElement(XXMLElement& element, XXMLElement* pParent)
{
	if (NextChild(element, pParent))
	{
		if (m_bEmptyTag)
			return true;	// that's it, this one is finished

		ParseContentETag( element, pParent );
		return true;
	}

	return false;
}

// =============================================================================
// ParseContentETag - Parse a single element tag
// =============================================================================
#ifdef _WIN32
#pragma warning(disable:4702)	// Warning: unreachable code
#endif
bool XXMLParseIterator::ParseContentETag(XXMLElement& inElement, XXMLElement* pParent)
{
	ParseSpaces();

	// Record current position.
	XParseBookmark	bookMark( this );
	
	char	c = NextChar();
	bool	bSawNonWSP = false;

	for( ;; )
	{
		// "]]>" not allowed in content.
		if( ParseString( "]]>" ) )
		{
			SyntaxError();
		}
		
		switch( c )
		{
			case 0:	// End of document.
			{
				SyntaxError();
			}
			break;

			case '\t':
			case '\r':
			case '\n':
			case ' ':
				break;

			case '&':
			case '<':
			{
				// Move back a character.
				PreviousChar();
				
				// Put what we already have in the value.
				XString	strValue;
				if (bSawNonWSP)
				{
					bookMark.GetSubString( strValue );

					if( strValue.GetLength() > 0 )
					{
						inElement.AddValue( strValue );
					}
				}

				// Tag or reference?
				if( c == '&' )
				{
					// Get the reference.
					if( !ParseReference(c) )
					{
						SyntaxError();
					}
					
					// Add it to the value.
					inElement.AddValue( c );
				}
				else
				{
					// Is an end tag?
					if( ParseETag( inElement ) )
					{	// don't continue, we only want to parse 1
						return true;
					}
					else if( !ParseMarkup( inElement, pParent ) )
					{
						SyntaxError();
					}
				}
				
				bookMark.Reset();
			}
			break;
			default:
				bSawNonWSP = true;
		}
		
		c = NextChar();
	}

	// Unreachable, but still required by strict compilers.
	return true;
}
#ifdef _WIN32
#pragma warning(default:4702)
#endif

// =============================================================================
// ParseMarkup - Same as base but calls our ParseElement()
// =============================================================================
bool XXMLParseIterator::ParseMarkup(XXMLElement& inElement, XXMLElement* pParent)
{
	// Try to read comment.
	XXMLElement*	pNewComment = ParseComment();
	if( pNewComment != NULL )
	{
		if( !inElement.AddChild( pNewComment ) )		// +++ Want to add comments?
		{
			SyntaxError();
		}
		return true;
	}
	
	// Try to read CDATA.
	if( ParseCDATA( inElement ) )
	{
		return true;
	}
	
	// Try to read element.
	if(ParseElement(inElement, pParent))
	{
		return true;
	}
	
	return false;
}

// =============================================================================
// EndElement - used to parse past the end of the element's tag
// =============================================================================
bool XXMLParseIterator::EndElement(XXMLElement* pElement)
{
	XParseBookmark	bookMark( this );

	try
	{
		ParseSpaces();
		if (XXMLParser::ParseETag(*pElement))
			return true;
	}
	catch(XParseException e)
	{
		e;	// To avoid "unused variable" warning in release build.
		TRACE(_T("Caught XMLParser exception line %d, column %d\n"), (int)e.GetLine(), (int)e.GetColumn());
	}

	bookMark.Restore();
	return false;
}
