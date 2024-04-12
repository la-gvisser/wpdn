// =============================================================================
//	XMLParser.cpp
// =============================================================================
//	Copyright (c) Wildpackets, Inc. 2000-2005. All rights reserved.
//	Copyright (c) AG Group, Inc. 1999-2000. All rights reserved.

#include "stdafx.h"
#include "XMLParser.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

// Used to map references to characters (eg, "&amp;" to '&').
struct MapReference
{
	const TCHAR*	szName;
	char			c;
};

 
// Predefined references.
const SInt32 kNumMapRefs	= 5;
static const MapReference s_MapReference[] =
{
    { _T("lt"),		_T('<')  },
    { _T("gt"),   	_T('>')  },
    { _T("amp"),  	_T('&')	 },
    { _T("quot"), 	_T('\"') },
    { _T("apos"), 	_T('\'') }
};


// =============================================================================
//	XXMLParser
// =============================================================================

// -----------------------------------------------------------------------------
//	XXMLParser													   [constructor]
// -----------------------------------------------------------------------------

XXMLParser::XXMLParser()
{
}


// -----------------------------------------------------------------------------
//	~XXMLParser														[destructor]
// -----------------------------------------------------------------------------

XXMLParser::~XXMLParser()
{
}


// -----------------------------------------------------------------------------
//	DoParse( const CByteStream& )
// -----------------------------------------------------------------------------

XXMLElement*
XXMLParser::DoParse(
	const CByteStream&	inStream )
{
	if( !SetDataSource( inStream ) ) return NULL;

	return ParseDocument();
}


// -----------------------------------------------------------------------------
//	DoParse( const char*, SInt32 )
// -----------------------------------------------------------------------------

XXMLElement*
XXMLParser::DoParse(
	const char*	inData,
	SInt32		inDataLength )
{
	if( !SetDataSource( inData, inDataLength ) ) return NULL;

	return ParseDocument();
}


// -----------------------------------------------------------------------------
//	ParseDocument													   [private]
// -----------------------------------------------------------------------------
// document ::= prolog element Misc*

XXMLElement*
XXMLParser::ParseDocument()
{
	ParseProlog();
		
	// Get root element.
	XXMLElement*	pRootElement = ParseElement();
	if( pRootElement == NULL )
	{
		SyntaxError();
	}
	
	ParseMisc();
	
	return pRootElement;
}


// -----------------------------------------------------------------------------
//	ParseProlog														   [private]
// -----------------------------------------------------------------------------
// prolog ::= XMLDecl? Misc* (doctypedecl Misc*)

void
XXMLParser::ParseProlog()
{
	ParseXMLDecl();
	ParseMisc();
	
	if( ParseDoctypeDecl() )
	{
		ParseMisc();
	}
}


// -----------------------------------------------------------------------------
//	ParseDoctypeDecl												   [private]
// -----------------------------------------------------------------------------
// doctypedecl ::= '<!DOCTYPE' S Name (S ExternalID)? S? ('['
//					(markupdecl | PEReference | S)* ']' S?)? '>' 

bool
XXMLParser::ParseDoctypeDecl()
{
	// Parse: '<!DOCTYPE'
	if( !ParseString( "<!DOCTYPE" ) )
	{
		return false;
	}
	
	// Parse: S
	if( !ParseSpaces() )
	{
		SyntaxError();
	}
	
	// Parse: Name
	XString	strName;
	if( !ParseName( strName ) )
	{
		SyntaxError();
	}
	
	// Parse: (S ExternalID)? S?
	ParseSpaces();
	if( ParseExternalID() )
	{
		ParseSpaces();
	}
	
	// Parse: ('[' (markupdecl | PEReference | S)* ']' S?)?
	if( ParseChar( '[' ) )
	{
		// Parse: (markupdecl | PEReference | S)* ']' S?)?
		char	c = NextChar();
		while( c != ']' )
		{
			if( c == 0 )
			{
				SyntaxError();
			}
			c = NextChar();
		}
		
		// Parse: S?
		ParseSpaces();
	}
	
	// Parse: '>'
	if( !ParseChar( '>' ) )
	{
		SyntaxError();
	}
	
	return true;
}


// -----------------------------------------------------------------------------
//	ParseExternalID													   [private]
// -----------------------------------------------------------------------------
// ExternalID ::= 'SYSTEM' S SystemLiteral | 
//						'PUBLIC' S PubidLiteral S SystemLiteral

bool
XXMLParser::ParseExternalID()
{
	// Parse: 'SYSTEM' S SystemLiteral
	if( ParseString( "SYSTEM" ) )
	{
		if( !ParseSpaces() || !ParseSystemLiteral() )
		{
			SyntaxError();
		}
	}
	else if( ParseString( "PUBLIC" ) )
	{
		if( !ParseSpaces() || 
			!ParsePubidLiteral() || 
			!ParseSpaces() || 
			!ParseSystemLiteral() )
		{
			SyntaxError();
		}
	}
	else
	{
		return false;
	}
	
	return true;
}


// -----------------------------------------------------------------------------
//	ParseSystemLiteral												   [private]
// -----------------------------------------------------------------------------
// SystemLiteral ::= ('"' [^"]* '"') | ("'" [^']* "'")

bool
XXMLParser::ParseSystemLiteral()
{
	char	cQuote = NextChar();
	if( cQuote != '\'' && cQuote != '\"' )
	{
		return false;
	}
	
	char	c = NextChar();
	while( c != cQuote )
	{
		if( c == 0 )
		{
			SyntaxError();
		}
	}
	
	return true;
}


// -----------------------------------------------------------------------------
//	ParsePubidLiteral												   [private]
// -----------------------------------------------------------------------------
// PubidLiteral ::= '"' PubidChar* '"' | "'" (PubidChar - "'")* "'" 

bool
XXMLParser::ParsePubidLiteral()
{
	char	cQuote = NextChar();
	if( cQuote != '\'' && cQuote != '\"' )
	{
		return false;
	}
	
	char	c = NextChar();
	while( c != cQuote )
	{
		if( c == 0 )
		{
			SyntaxError();
		}
		
		if( !IsPubidChar( c ) )
		{
			SyntaxError();
		}		
	}
	
	return true;
}


// -----------------------------------------------------------------------------
//	ParseMisc														   [private]
// -----------------------------------------------------------------------------
// Misc*
// Misc ::= Comment | S

void
XXMLParser::ParseMisc()
{
	for( ;; )
	{
		// Parse: S (optional).
		ParseSpaces();
		
		// Parse comment if any.
		XXMLElement*	pCommentElement = ParseComment();
		if( pCommentElement == NULL )
		{
			break;
		}
		else
		{
			// Just throw these comments away, as they aren't associated with
			// any element.  Who wants to keep comments, anyway?
			delete pCommentElement;
			pCommentElement = NULL;
		}
	}
}


// -----------------------------------------------------------------------------
//	ParseXMLDecl													   [private]
// -----------------------------------------------------------------------------
// XMLDecl ::= '<?xml' VersionInfo EncodingDecl? S? '?>'

bool
XXMLParser::ParseXMLDecl()
{
	// Parse: '<?xml'
	if( !ParseStringNoCase( "<?xml" ) )
	{
		return false;
	}
	
	// Parse: VersionInfo EncodingDecl? SDDecl? S? '?>'
	if( !ParseVersionInfo( m_strVersion ) )
	{
		SyntaxError();
	}
	
	// Parse EncodingDecl (optional).
	ParseEncodingDecl( m_strEncoding );
	
	// Parse S (spaces) (optional).
	ParseSpaces();
	
	// Parse end of declaration '?>'.
	if( !ParseString( "?>" ) )
	{
		SyntaxError();
	}
	
	return true;
}


// -----------------------------------------------------------------------------
//	ParseVersionInfo												   [private]
// -----------------------------------------------------------------------------
// VersionInfo ::= S 'version' Eq (' VersionNum ' | " VersionNum ")

bool
XXMLParser::ParseVersionInfo(
	XString&	outVersion )
{
	// Parse: S 'version' Eq
	if( !ParseDeclBeginning( "version" ) )
	{
		return false;
	}
	
	// Parse: (' VersionNum ' | " VersionNum ")
	char	c = NextChar();
	if( ( c != '\'' ) && ( c != '\"' ) )
	{
		SyntaxError();
	}
	
	// Parse version number and check delimiter.
	if( ( !ParseVersionNum( outVersion ) ) || ( NextChar() != c ) )
	{
		SyntaxError();
	}
	
	return true;
}


// -----------------------------------------------------------------------------
//	ParseDeclBeginning												   [private]
// -----------------------------------------------------------------------------
// S <inString> Eq

bool
XXMLParser::ParseDeclBeginning(
	const char*	inString )
{
	ASSERT( inString != NULL );
	if( inString == NULL ) return false;
	
	// Parse: S
	char	c = NextChar();
	if( !IsSpace(c) )
	{
		PreviousChar();
		return false;
	}
	ParseSpaces();
	
	// Parse: <szString>
	if( !ParseString( inString ) )
	{
		return false;
	}
	
	// Parse: Eq
	if( !ParseEq() )
	{
		SyntaxError();
	}
	
	return true;
}


// -----------------------------------------------------------------------------
//	ParseVersionNum													   [private]
// -----------------------------------------------------------------------------
// VersionNum ::=	([a-zA-Z0-9_.:] | '-')+

bool
XXMLParser::ParseVersionNum(
	XString&	outVersion )
{
	// Record current position.
	XParseBookmark	bookMark( this );
	
	char	c = NextChar();
	// Is an allowed number?
	if( !IsAlphaDigitEx(c) )
	{
		return false;
	}
	
	c = NextChar();
	// Get one or more chars.
	while( IsAlphaDigitEx(c) )
	{
		c = NextChar();
	}
	
	// Current char is not part of the version number.
	PreviousChar();
	
	// Get the version number.
	bookMark.GetSubString( outVersion );
	return true;
}


// -----------------------------------------------------------------------------
//	ParseEncodingDecl												   [private]
// -----------------------------------------------------------------------------
// EncodingDecl ::= S 'encoding' Eq ('"' EncName '"' | "'" EncName "'")

bool
XXMLParser::ParseEncodingDecl(
	XString&	outEncoding )
{
	// Parse: S 'encoding' Eq
	if( !ParseDeclBeginning( "encoding" ) )
	{
		return false;
	}
	
	// Parse: ('"' EncName '"' | "'" EncName "'")
	char	c = NextChar();
	if( ( c != '\'' ) && ( c != '\"' ) )
	{
		SyntaxError();
	}
	
	// Parse encoding name and check delimiter.
	if( ( !ParseEncName( outEncoding ) ) || ( NextChar() != c ) )
	{
		SyntaxError();
	}
	
	return true;
}


// -----------------------------------------------------------------------------
//	ParseEncName													   [private]
// -----------------------------------------------------------------------------
// EncName ::=  [A-Za-z] ([A-Za-z0-9._] | '-')*

bool
XXMLParser::ParseEncName(
	XString&	outEncoding )
{
	// Record current position.
	XParseBookmark	bookMark( this );
	
	char	c = NextChar();
	
	// Legal character?
	if( !IsAlpha(c) )
	{
		return false;
	}
	
	// Get one or more chars.
	c = NextChar();
	while( IsAlphaDigitEx(c) )
	{
		c = NextChar();
	}
	
	// Current character is not part of encoding string.
	PreviousChar();
	
	// Get the encoding string.
	bookMark.GetSubString( outEncoding );
	
	return true;
}


// -----------------------------------------------------------------------------
//	ParseElement													   [private]
// -----------------------------------------------------------------------------
// element	::= EmptyElemTag | STag content ETag
// EmptyElemTag	::= '<' Name (S Attribute)* S? '/>'
// STag			::= '<' Name (S Attribute)* s? '>'

XXMLElement*
XXMLParser::ParseElement()
{
	XXMLElement*	pNewElement = ParseTagBeginning();
	if( pNewElement == NULL ) return NULL;
	
	char	c = NextChar();
	if( c == '/')
	{
		c = NextChar();
		if( c == '>')
		{
			// Empty tag.
			return pNewElement;
		}
		
		SyntaxError();
	}
	
	if( c != '>' )
	{
		SyntaxError();
	}
	
	ParseContentETag( *pNewElement );
	
	return pNewElement;
}


// -----------------------------------------------------------------------------
//	ParseTagBeginning												   [private]
// -----------------------------------------------------------------------------
// '<' Name (S Attribute)* S?

XXMLElement*
XXMLParser::ParseTagBeginning()
{
	if( !ParseChar( '<' ) )
	{
		return NULL;
	}
	
	XString	strName;
	if( !ParseName( strName ) )
	{
		SyntaxError();
	}
	
	XXMLElement*	pNewElement = new XXMLElement( strName );
	while( ParseSpaces() && ParseAttribute( pNewElement ) )
	{
	}
	
	return pNewElement;
}

// -----------------------------------------------------------------------------
//	ParseContentETag												   [private]
// -----------------------------------------------------------------------------
// content  ::=  (element | CharData | Reference | CDSect | Comment)*
// CharData ::=  [^<&]* - ([^<&]* ']]>' [^<&]*) 
// Reference begins with '&'

void
XXMLParser::ParseContentETag(
	XXMLElement&	inElement )
{
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
					{
						return;
					}
					else if( !ParseMarkup( inElement ) )
					{
						SyntaxError();
					}
				}
				
				bookMark.Reset();
				bSawNonWSP = false;
			}
			break;
			
			default:
			{
				bSawNonWSP = true;
			}
			break;
		}
		
		c = NextChar();
	}
}


// -----------------------------------------------------------------------------
//	ParseMarkup														   [private]
// -----------------------------------------------------------------------------
// Comment begins with '<!--'
// CDSect begins with '<![CDATA['
// Element begins with '<'

bool
XXMLParser::ParseMarkup(
	XXMLElement&	inElement )
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
	XXMLElement*	pNewElement = ParseElement();
	if( pNewElement != NULL )
	{
		if( !inElement.AddChild( pNewElement ) )
		{
			SyntaxError();
		}
		return true;
	}
	
	return false;
}


// -----------------------------------------------------------------------------
//	ParseComment													   [private]
// -----------------------------------------------------------------------------
// Comment ::=	'<!--' ((Char - '-') | ( '-' (Char - '-')))* '-->'

XXMLElement*
XXMLParser::ParseComment()
{
	// Check start of comment.
	if( !ParseString( "<!--" ) )
	{
		return NULL;
	}
	
	// Record current position to extract content of comment later.
	XParseBookmark	bookMark( this );
	for( ;; )
	{
		// Look for the end of the comment.
		if( ParseString( "--" ) )
		{
			// Really the end?
			if( !ParseChar( '>' ) )
			{
				SyntaxError();
			}
			break;
		}
		
		// End of document?
#if _LINUX
		if( NextChar() == 0 )
#else
		if( NextChar() == NULL )
#endif
		{
			SyntaxError();
		}
	}
	
	// Extract comment content.
	XString	strComment;
	bookMark.GetSubString( strComment, 3 );
	
	// Construct an element.
	return new XXMLElement( strComment );
}


// -----------------------------------------------------------------------------
//	ParseCDATA														   [private]
// -----------------------------------------------------------------------------
// CDSect ::=	'<![CDATA[' CData ']]>'
// CData ::=	(Char* - (Char* ']]>' Char*))

bool
XXMLParser::ParseCDATA(
	XXMLElement&	inElement )
{
	// Parse: <![CDATA[
	if( !ParseString( "<![CDATA[" ) )
	{
		return false;
	}
	
	XParseBookmark	bookMark( this );
	// Parse: CData
	for( ;; )
	{
		// End of CData?
		if( ParseString( "]]>" ) )
		{
			break;
		}
		
		// Is character allowed?
		if( !IsXmlChar( NextChar() ) )
		{
			SyntaxError();
		}
	}
	
	// Get CDATA content and add it as-is to the value.
	XString	strCDATA;
	bookMark.GetSubString( strCDATA, 3 );
	inElement.AddValue( strCDATA, true );
	return true;
}


// -----------------------------------------------------------------------------
//	ParseName														   [private]
// -----------------------------------------------------------------------------

bool
XXMLParser::ParseName(
	XString&	inName )
{
	XParseBookmark	bookMark( this );
	
	char	c = NextChar();
	if( ( !IsAlpha(c) ) && ( c != '-' ) && ( c != ':' ) )
	{
		PreviousChar();
		return false;
	}
	
	for( ;; )
	{
		c = NextChar();
		if( !IsAlphaDigitEx(c) )
		{
			break;
		}
	}
	
	// Current char is not part of name.
	PreviousChar();
	
	// Extract name.
	bookMark.GetSubString( inName );
	return true;
}


// -----------------------------------------------------------------------------
//	ParseAttribute													   [private]
// -----------------------------------------------------------------------------
// Attribute ::=  Name Eq AttValue

bool
XXMLParser::ParseAttribute( 
	XXMLElement*	inElement )
{
	ASSERT( inElement != NULL );
	if( inElement == NULL ) return false;
	
	XString	strName;
	if( !ParseName( strName ) )
	{
		return false;
	}
	
	XString strValue;
	if( !ParseEq() || !ParseAttValue( strValue ) )
	{
		SyntaxError();
	}
	
	// Construct an attribute and add it to the element.
	inElement->AddAttribute( strName, strValue );
	return true;
}


// -----------------------------------------------------------------------------
//	ParseAttValue													   [private]
// -----------------------------------------------------------------------------
// '"' ([^<&"] | Reference)* '"'  | "'" ([^<&'] | Reference)* "'" 

bool
XXMLParser::ParseAttValue( 
	XString&	outValue )
{
	// Get value delimiter (quote or apostrophe).
	char	delimChar = NextChar();
	if( ( delimChar != '\'' ) && ( delimChar != '\"' ) )
	{
		PreviousChar();
		return false;
	}
	
	// Bookmark current position to extract value later.
	XParseBookmark	bookMark( this );
	
	char	c = NextChar();
	while( c != delimChar )
	{
		switch( c )
		{
			case 0:
			{
				// End of document.
				SyntaxError();
			}
			break;
			
			case '<':
			{
				// Tag.
				SyntaxError();
			}
			break;
			
			case '&':
			{
				// Reference.
				PreviousChar();
				
				// Put what we already have in the value.
				XString	strBefore;
				bookMark.GetSubString( strBefore );
				outValue += strBefore;
				
				// Get the reference.
				char	c2 = 0;
				if( !ParseReference( c2 ) )
				{
					SyntaxError();
				}
				
				// Put the char in the value.
				outValue += c2;
				
				// Record the new position (after the reference).
				bookMark.Reset();
			}
			break;

			default:
			{
	            if(!IsXmlChar(c))
				{
		            SyntaxError();
				}
			}
            break;
		}

		c = NextChar();
	}
	
	// Put the remaining portion of the value.
	XString	strRemaining;
	bookMark.GetSubString( strRemaining, 1 );
	outValue += strRemaining;
	
	return true;
}


// -----------------------------------------------------------------------------
//	ParseEq															   [private]
// -----------------------------------------------------------------------------
// Eq ::=	S? '=' S?

bool
XXMLParser::ParseEq()
{
	// Record current position.
	XParseBookmark	bookMark( this );
	
	// Parse spaces (optional).
	ParseSpaces();
	
	// Is an equal sign?
	if( !ParseChar( '=' ) )
	{
		// No, revert to original position.
		bookMark.Restore();
		return false;
	}
	
	// Skip spaces (optional).
	ParseSpaces();
	
	return true;
}


// -----------------------------------------------------------------------------
//	ParseReference													   [private]
// -----------------------------------------------------------------------------
// Reference ::=	EntityRef | CharRef
// EntityRef ::=	'&' Name ';'
// CharRef ::=		'&#' [0-9]+ ';' | '&#x' [0-9a-fA-F]+ ';'

bool
XXMLParser::ParseReference( char& outRef )
{
	// Reference begins with '&'.
	if( !ParseChar( '&' ) )
	{
		return false;
	}
	
	char	c = NextChar();
	if( c != '#' )
	{
		// EntityRef - get name of reference and check for ';' at the end.
		PreviousChar();
		XString strReferenceName;
		if( !ParseName( strReferenceName ) || !ParseChar( ';' ) )
		{
			SyntaxError();
		}
		
		// Look for the reference.
		if( !MapReferenceName( strReferenceName, outRef ) )
		{
			SyntaxError();
		}
		
		return true;
	}
	
	// Must be a CharRef.
	c = NextChar();
	
	// Compute the value (character code).
	SInt32	nNum	= 0;
	
	// Hex?
	if( c == 'x')
	{
		// Get the value.
		if( !ParseHexNumber( nNum ) )
		{
			SyntaxError();
		}
	}
	else
	{
		PreviousChar();
		if( !ParseNumber( nNum ) )
		{
			SyntaxError();
		}
	}
	
	// Check the end of the reference.
	if( !ParseChar( ';' ) )
	{
		SyntaxError();
	}
	
	// Return the character.
	outRef = static_cast<char>(nNum);
	
	return true;
}


// -----------------------------------------------------------------------------
//	ParseETag														   [private]
// -----------------------------------------------------------------------------
// ETag ::= '</' Name S? '>'

bool
XXMLParser::ParseETag(
	XXMLElement&	inElement )
{
	// Is it an end tag?
	if( !ParseString( "</" ) )
	{
		return false;
	}
	
	// Get tag name.
	XString	strEndTagName;
	if( !ParseName( strEndTagName ) )
	{
		SyntaxError();
	}
	
	// Start and end tag names must match.
	if( strEndTagName != inElement.GetName() )
	{
		SyntaxError();
	}
	
	// Skip spaces.
	ParseSpaces();
	
	// End of tag.
	if( !ParseChar( '>' ) )
	{
		SyntaxError();
	}
	
	return true;
}


// -----------------------------------------------------------------------------
//	MapReferenceName												   [private]
// -----------------------------------------------------------------------------

bool 
XXMLParser::MapReferenceName(
	const XString&	inStr, 
	char&			outChar)
{
	// Find the reference strName and return its equivalent
    for( SInt32 nIndex = 0; nIndex < kNumMapRefs; ++nIndex )
    {
        // Same name ?
        if( inStr == s_MapReference[nIndex].szName)
        {
            // return the equivalent
            outChar = s_MapReference[nIndex].c;
            return true;
        }
    }
    
    return false;
}
