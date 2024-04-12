// ============================================================================
//	XMLParser.h
// ============================================================================
//	Copyright (c) Wildpackets, Inc. 2000-2004. All rights reserved.
//	Copyright (c) AG Group, Inc. 1999-2000. All rights reserved.

#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "XString.h"
#include "Parser.h"
#include "XMLDoc.h"

class DECLSPEC XXMLParser : public XParser
{
public:
					XXMLParser();
	virtual			~XXMLParser();
	
	// Throw XXMLException.
	XXMLElement*	DoParse( const char* inData, SInt32 inDataSize );
	XXMLElement*	DoParse( const CByteStream& inStream );
	
	const XString&	GetXMLVersion() { return m_strVersion;  }
	const XString&	GetEncoding() { return m_strEncoding; }
		
protected:
	XString			m_strVersion;
	XString			m_strEncoding;

	XXMLElement*	ParseDocument();
	XXMLElement*	ParseTagBeginning();
	XXMLElement*	ParseElement();
	XXMLElement*	ParseComment();
	bool			ParseAttribute( XXMLElement* inElement );
	bool			ParseETag( XXMLElement& inElement );
	bool			ParseMarkup( XXMLElement& inElement );
	void			ParseContentETag( XXMLElement& inElement );
	bool			ParseCDATA( XXMLElement& inElement );
	bool			ParseVersionInfo( XString& outVersion );
	bool			ParseVersionNum( XString& outVersion );
	bool			ParseEncodingDecl( XString& outEncoding);
	bool			ParseEncName( XString& outEncoding );
	bool			ParseAttValue( XString& outValue );		
	bool			ParseName( XString& inName );
	bool			ParseReference( char& outRef );
	bool			ParseDeclBeginning( const char* inString );
	bool			ParseDoctypeDecl();
	bool			ParseExternalID();
	bool			ParseSystemLiteral();
	bool			ParsePubidLiteral();
	bool			ParseXMLDecl();
	void			ParseMisc();
	void			ParseProlog();
	bool			ParseEq();
	
	// Map reference name to character.
	bool 			MapReferenceName( const XString& inStr, char& outChar);
	
	// Parse utility routines.
	static bool		IsXmlChar( char c );
	static bool		IsPubidChar( char c );
};


// -----------------------------------------------------------------------------
//	inline parse utilities
// -----------------------------------------------------------------------------

inline bool XXMLParser::IsXmlChar(char c)
{
    return (c == 0x09) || (c == 0x0a) || (c == 0x0d)  || (c >= 0x20);
}

inline bool XXMLParser::IsPubidChar(char c)
{
	if( IsAlphaDigit(c) ) return true;
	
	static const char	s_szChars[] = "\x20\xD\xA-'()+,./:=?;!*#@$_%";
	const char* pEnd = s_szChars + (sizeof(s_szChars) / sizeof(s_szChars[0]));
	for( const char* pChar = s_szChars; pChar < pEnd; ++pChar )
	{
		if( c == *pChar )
		{
			return true;
		}
	}
	
	return false;
}

#endif /* XMLPARSER_H */
