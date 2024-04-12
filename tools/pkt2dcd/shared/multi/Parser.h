// ============================================================================
//	Parser.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Copyright (c) AG Group, Inc. 2000. All rights reserved.

#ifndef PARSER_H
#define PARSER_H

#include "XString.h"

class CByteStream;

// Exception class thrown by XParser.
class DECLSPEC XParseException
{
public:
			XParseException( SInt32 inLine=0, SInt32 inColumn=0 ) 
				: m_nLine( inLine ), m_nColumn( inColumn ) {}
	
	SInt32	GetLine() const { return m_nLine; }
	SInt32	GetColumn() const { return m_nColumn; }
	
private:
	SInt32	m_nLine;
	SInt32	m_nColumn;
};


class DECLSPEC XParser : virtual public XObject
{
public:
					XParser();
	virtual			~XParser();

	bool			SetDataSource( const CByteStream& inStream );
	bool			SetDataSource( const char* inData, SInt32 inDataLength );

protected:
	bool			ParseChar( char inChar );
	bool			ParseString( const char* inString );
	bool			ParseLong( const UInt32 inLong );
	bool			ParseStringNoCase( const char* inString );
	bool			ParseSpaces();
	bool			ParseEOL();
	bool			ParseNumber( SInt32& outNum );
	bool			ParseHexNumber( SInt32& outNum );
	
	// Get characters.
	char			NextChar();
	void			PreviousChar();
	
	void			SyntaxError();			// Throws XParseException.
	
	// Parse utility routines.
	static bool		IsSpace( char c );
	static bool		IsAlpha( char c );
	static bool		IsDigit( char c );
	static bool		IsEndline( char c );
	static bool		IsHexDigit( char c );
	static int		HexDigitValue( char c );
	static bool		IsAlphaDigit( char c );
	static bool		IsAlphaDigitEx( char c );
	static char		LowCase( char c );
	
	const char*		m_szSource;				// Source data.
	const char*		m_szSourceCurrent;		// Current position in source data.
	const char*		m_szSourceEnd;			// Last byte of source data.
		
	SInt32			m_nLine;				// Current line in data.
	SInt32			m_nColumn;				// Current column in data.
		
	class XParseBookmark
	{
	public:
		XParseBookmark( XParser* inParser ) :
			m_pParser( inParser ),
			m_szSourceCurrent( inParser->m_szSourceCurrent ),
			m_nLine( 1 ), m_nColumn( 1 ) {}
		
		void	Restore() { m_pParser->m_szSourceCurrent = m_szSourceCurrent; }
		void	Reset() { m_szSourceCurrent = m_pParser->m_szSourceCurrent; }
		void	GetSubString( XString& inString, SInt32 inNumEndSkip = 0 );

	protected:
		XParser*		m_pParser;
		const char*		m_szSourceCurrent;
		SInt32			m_nLine;
		SInt32			m_nColumn;
	};
	
	friend class XParseBookmark;
};


// -----------------------------------------------------------------------------
//	inline parse utilities
// -----------------------------------------------------------------------------

// Space, tab, line feed or return
inline bool XParser::IsSpace(char c)
{
    return (c == ' ') || (c == '\t') || (c == '\r') || (c == '\n');
}

inline bool XParser::IsEndline(char c)
{
    return (c == '\r') || (c == '\n');
}

// [a-zA-Z]
inline bool XParser::IsAlpha(char c)
{
    return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}

// [0-9]
inline bool XParser::IsDigit(char c)
{
    return (c >= '0') && (c <= '9');
}

// [0-9a-fA-F]
inline bool XParser::IsHexDigit(char c)
{
    return IsDigit(c) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F'));
}

// [0-9a-fA-F]
inline int XParser::HexDigitValue(char c)
{
    return ((c >= '0') && (c <= '9')) ? (c - '0')
        : (((c >= 'a') && (c <= 'f')) ? ((c - 'a') + 10) : ((c - 'A') + 10));
        
}

// [a-zA-Z0-9]
inline bool XParser::IsAlphaDigit(char c)
{
    return(IsAlpha(c) || IsDigit(c));
}

// [a-zA-Z0-9_.:] | '-'
inline bool XParser::IsAlphaDigitEx(char c)
{
    return IsAlphaDigit(c) || (c == '_') || (c == '.') || (c == ':') || (c == '-');
}

inline char XParser::LowCase(char c)
{
    return (char)(((c >= 'A') && (c <= 'Z')) ? ((c - 'A') + 'a') : c);
}

#endif /* PARSER_H */
